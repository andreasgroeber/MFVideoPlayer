#include "AppMenu.h"

#include <commdlg.h>

#pragma comment(lib, "comdlg32.lib")


HMENU CreateAppMenu()
{
    // Obere Menüleiste
    HMENU menuBar = CreateMenu();

    // Untermenü „Allgemein“
    HMENU generalMenu = CreatePopupMenu();
    AppendMenuW(generalMenu, MF_STRING,  ID_MENU_OPEN, L"Öffnen");
    AppendMenuW( generalMenu, MF_STRING,ID_MENU_CLOSE, L"Schließen");
    AppendMenuW( menuBar, MF_POPUP, reinterpret_cast<UINT_PTR>( generalMenu), L"Allgemein");
    return menuBar;
}


bool OpenVideoFileDialog(HWND hwnd, wchar_t* fileName, DWORD fileNameSize)
{
    if (!fileName ||
        fileNameSize == 0)
    {
        return false;
    }

    fileName[0] = L'\0';
    OPENFILENAMEW ofn{};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = fileName;
    ofn.nMaxFile =fileNameSize;
    ofn.lpstrFilter =                   //Filtert nach diesen Dateiformaten
        L"Videodateien\0"
        L"*.mp4;*.avi;*.mkv;*.wmv;*.mov\0"
        L"Alle Dateien\0"
        L"*.*\0";
  ofn.nFilterIndex = 1;
  ofn.Flags = OFN_FILEMUSTEXIST |OFN_PATHMUSTEXIST;

    return GetOpenFileNameW(
        &ofn) == TRUE;
}