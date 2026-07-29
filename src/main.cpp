#include "AppMenu.h"
#include "MFVideoPlayer.h"

#include <windows.h>
#include <mfapi.h>
#include <comdef.h>
#include <commctrl.h>

#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "comctl32.lib")

#define ID_BTN_PLAYPAUSE 2 // bessere Lesbarkeit historisch so enstanden davor war load 1 play 2 pause 3 --> Load nun über menü play/pause sind ein button
#define Time_SLIDER 100
#define Abschnitte_SLIDER 1000

/* Globale Refernzen auf UI Elemente und Player.*/
// Media Player Instanz
MFVideoPlayer* g_player = nullptr; 
// Fensterbereich das Video rendert
HWND g_videoWindow = nullptr;      

// Button       
HWND g_btnPlayPause = nullptr;
bool g_isPlaying = false;
// Zeitleiste + Zeit
HWND g_slider = nullptr;
HWND g_timeText = nullptr;
HWND g_speedText = nullptr;
// Geschwindigkeit
float g_playbackRate = 1.0f;

/* Msgverarbeitung des Fensters */
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {

    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case ID_MENU_OPEN:          // Öffnen aus AppMenu.h
        {
            wchar_t fileName[MAX_PATH];
            if (OpenVideoFileDialog(hwnd, fileName, MAX_PATH))
            {
                if (g_player)
                {
                    g_player->Open(fileName);
                    // Slider nach neuem Video auf Anfang setzen
                    SendMessage( g_slider,TBM_SETPOS,TRUE, 0);
                }
            }
            return 0;
        }
        case ID_MENU_CLOSE:         // Programm beenden
        {
            DestroyWindow(hwnd);
            return 0;
        }
        case ID_BTN_PLAYPAUSE:      // Play/Pause Button
        {
            if (g_player)
            {
                if (g_isPlaying)    // Video läuft -> Pause
                {
                    g_player->Pause();
                    g_isPlaying = false;
                    SetWindowTextW( g_btnPlayPause,L"Play");
                }
                else                // Video pausiert -> Play
                {
                    g_player->Play();
                    g_isPlaying = true;
                    SetWindowTextW( g_btnPlayPause,L"Pause");
                }
            }
            return 0;
        }

        } // Ende innerer switch

        break; // verhindert Fallthrough
    }
    case WM_SIZE:
    {
        int width = LOWORD(lParam);     //loword sind untere 16 bits hiword oberen 16 bits aus lparam , lparam wiederum ist ein windows eigener Datentyp
        int height = HIWORD(lParam);    // typedef LONG_PTR LPARAM;
        int buttonArea = 80;    

        int videoWidth = width - 40;
        int videoHeight = height - buttonArea - 40;

        if (g_videoWindow)
        {
            MoveWindow( g_videoWindow, 20, 20,max(videoWidth, 1), max(videoHeight, 1),TRUE); // Aus Windows.h
        }
        // Buttons zentrieren
        int buttonWidth = 100;
        int buttonSpacing = 30;
        int totalWidth = buttonWidth;
        int startX = (width - totalWidth) / 2;
        int buttonY = height - 60;
        if (g_slider)
        {
            MoveWindow( g_slider,20, height - 110,width - 40, 30, TRUE);
        }

        if (g_btnPlayPause)
        {
            MoveWindow( g_btnPlayPause,startX,buttonY, buttonWidth,40,TRUE);
        }
        if (g_timeText)
        {
            MoveWindow(g_timeText,width - 250, height - 75,150, 30,TRUE);
        }
        if (g_speedText)
        {
            MoveWindow(g_speedText,width - 90, height - 75, 80, 30, TRUE);
        }
        break;
    }
    case WM_HSCROLL:
    {
        if ((HWND)lParam == g_slider)
        {
            int sliderPos = SendMessage(g_slider,TBM_GETPOS,0, 0);
            double duration = g_player->GetDuration();
            double seconds =  duration * sliderPos / Abschnitte_SLIDER;
            g_player->Seek(seconds);
        }
        break;
    }
    case WM_DESTROY:
    {
        g_player = nullptr;
        PostQuitMessage(0);
        return 0;
    }
    case WM_TIMER:
    {
        if (wParam == 1 && g_player && g_slider)
        {
            double current = g_player->GetPosition();
            double duration = g_player->GetDuration();
            int curMin = (int)current / 60;
            int curSec = (int)current % 60;
            int durMin = (int)duration / 60;
            int durSec = (int)duration % 60;
            wchar_t buffer[64];

            swprintf_s(buffer,L"%02d:%02d / %02d:%02d", curMin, curSec,durMin, durSec);
            SetWindowTextW( g_timeText,buffer);
            float rate = g_player->GetRate();
            wchar_t speed[32];
            swprintf_s( speed,L"%.2fx", rate);
            SetWindowTextW(g_speedText,speed);
            if (duration > 0.0)
            {
                int sliderPosition =static_cast<int>((current / duration) * Abschnitte_SLIDER);
                SendMessage(g_slider,TBM_SETPOS, TRUE,sliderPosition);
            }
        }
        break;
    }
    case WM_MOUSEWHEEL:
    {
        int delta = GET_WHEEL_DELTA_WPARAM(wParam);
        if (delta > 0)
        { g_playbackRate += 0.05f;}
        else
        {g_playbackRate -= 0.05f;}
        // Grenzen
        if (g_playbackRate < 0.05f)
            g_playbackRate = 0.05f;
        if (g_playbackRate > 5.0f)      //3x ist für MP4 grenze
            g_playbackRate = 5.0f;
        if (g_player)
        { g_player->SetRate(g_playbackRate);}
        wchar_t speed[32];
        swprintf_s(speed,L"%.2fx", g_playbackRate);
        SetWindowTextW( g_speedText, speed);
        break;
    }


    }
    return DefWindowProc(hwnd, msg, wParam, lParam);    // Übernimmt erstmal jeden nicht behandeltenfall und führt  die Default Aktion aus
}

//WinMain entpsricht int main() eines normallen programms
int WINAPI WinMain(_In_ HINSTANCE hInstance,_In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine,_In_ int nCmdShow)
{

    // ----------------------------
    // COM starten MF basiert auf COM bzw. ist Abhängig davon
    // ----------------------------

    HRESULT hr = CoInitializeEx(nullptr,COINIT_APARTMENTTHREADED);  //HResult ist ein Windows DAtentyp mit spezifischen Rückgabewerten wie S_OK,S_FALSE etc.
    if (FAILED(hr))
    {
        MessageBoxW(nullptr,L"COM konnte nicht gestartet werden.",L"Fehler", MB_OK);
        return 1;
    }
    // ----------------------------
    // Windows Common Controls für den Slider
    // ----------------------------
    INITCOMMONCONTROLSEX icex{};
    icex.dwSize = sizeof(icex);
    icex.dwICC = ICC_BAR_CLASSES;
    InitCommonControlsEx(&icex);

    // ----------------------------
    // Media Foundation init für Video und Audiofunktion
    // ----------------------------
    hr = MFStartup( MF_VERSION);
    if (FAILED(hr))
    {
        MessageBoxW( nullptr, L"Media Foundation konnte nicht gestartet werden.", L"Fehler", MB_OK);
        CoUninitialize();
        return 1;
    }


    // ---------------------------
    // Fensterklasse registrieren
    // ----------------------------
    WNDCLASSW wc{};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"AAVideoPlayer";
    if(!RegisterClassW(&wc))
    {MessageBoxW( nullptr,L"Fensterklasse konnte nicht registriert werden.",L"Fehler", MB_OK);
        MFShutdown();
        CoUninitialize();
        return 1;
    }
    // ----------------------------
    // Fenster erstellen
    // ----------------------------
    int windowWidth = 1280;
    int windowHeight = 900;

    // Bildschirmgröße
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // Mittelpunkt berechnen
    int posX = (screenWidth - windowWidth) / 2;
    int posY = (screenHeight - windowHeight) / 2;
    //MenüBar
    HMENU hMenuBar =CreateAppMenu();
    //Hauptfenster
    HWND hwnd = CreateWindowExW(0,
        L"AAVideoPlayer",
        L"AA Video Player",
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
        posX,
        posY,
        windowWidth,
        windowHeight,
        nullptr,
        hMenuBar,
        hInstance,
        nullptr);

    if (!hwnd)
    {
        MFShutdown();
        CoUninitialize();

        return 1;
    }
    // Video-Fenster erstellen
    g_videoWindow = CreateWindowExW(
        0,
        L"STATIC",
        nullptr,
        WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_TABSTOP , 
        20,
        20,
        windowWidth-100,
        windowHeight-200,
        hwnd,
        nullptr,
        hInstance,
        nullptr);

    SetFocus(hwnd);     //bedeutet das die Tastatur-Signale alle an das Fenster gehen indemfall da dass hauptfenster, nicht ans videofenster

    if (!g_videoWindow)
    {
        MFShutdown();
        CoUninitialize();

        return 1;
    }

    // ----------------------------
    // Button
    // ----------------------------

 g_btnPlayPause = CreateWindowW(
        L"BUTTON", L"Play",
        WS_VISIBLE | WS_CHILD,
        0,
        0,
        100,
        40,
        hwnd,
        (HMENU)2,
        hInstance,
        nullptr);
    // ----------------------------
    // Times
    // ----------------------------
    g_timeText = CreateWindowW(
        L"STATIC",
        L"00:00 / 00:00",
        WS_CHILD | WS_VISIBLE,
        0,
        0,
        150,
        30,
        hwnd,
        nullptr,
        hInstance,
        nullptr);

    g_speedText = CreateWindowW(
        L"STATIC",
        L"1.00x",
        WS_CHILD | WS_VISIBLE,
        0,
        0,
        80,
        30,
        hwnd,
        nullptr,
        hInstance,
        nullptr);
    // ----------------------------
    // Slider
    // ----------------------------
    g_slider = CreateWindowExW(
        0,
        TRACKBAR_CLASSW,
        nullptr,
        WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS,
        20,
        0,
        500,
        30,
        hwnd,
        (HMENU)Time_SLIDER,
        hInstance,
        nullptr);

    SendMessage( g_slider, TBM_SETRANGE,TRUE,MAKELPARAM(0, Abschnitte_SLIDER));

    // Fenster anzeigen und initiale Größe setzen Größenanpassung der Controls erzwingen

    RECT rect;
    GetClientRect(hwnd, &rect);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    SetTimer(hwnd, 1,100, nullptr);
    SendMessage(hwnd, WM_SIZE, SIZE_RESTORED,MAKELPARAM(rect.right, rect.bottom));

   // ShowWindow(hwnd, nCmdShow);
   // UpdateWindow(hwnd);

    // ----------------------------
    // Media Player init
    // ----------------------------

    {   
        MFVideoPlayer player;       // aus MFVideoPlayer.h
        g_player = &player;


        if (!player.Initialize(g_videoWindow))
        {
            MessageBoxW(
                hwnd,
                L"Player Initialisierung fehlgeschlagen.",
                L"Fehler",
                MB_OK);
        }
        //Windows Nachrichtenschleife
        // Wartet auf Benutzereingaben und Fensterereignisse
        MSG msg{};
        while (GetMessage(&msg,nullptr, 0,0) > 0)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        // ----------------------------
        // free
        // ----------------------------
        player.Shutdown();
        g_player = nullptr;
        //Media Foundation beenden
        MFShutdown();
        CoUninitialize();
    }


    return 0;
}