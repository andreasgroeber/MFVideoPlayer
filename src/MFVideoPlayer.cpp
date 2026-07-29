#include "MFVideoPlayer.h"
#include "MediaEngineNotify.h"

#include <mfapi.h>
#include <mfidl.h>
#include <mfmediaengine.h>
#include <comdef.h>
#include <oleauto.h>
#include <audiopolicy.h>

#pragma comment(lib,"mfplat.lib")
#pragma comment(lib,"mfuuid.lib")


// HWND --> Handler auf Windowsfenster


MFVideoPlayer::MFVideoPlayer()
{
    m_engine = nullptr;
    m_notify = nullptr;
    m_hwnd = nullptr;
}


/*MFVideoPlayer::~MFVideoPlayer()
{
    if (m_notify)
    {
        m_notify->Release();
        m_notify = nullptr;
    }


    if (m_engine)
    {
        m_engine->Shutdown();

        m_engine->Release();
        m_engine = nullptr;
    }
}*/

MFVideoPlayer::~MFVideoPlayer()
{
}


//Initialisiert die Engine,Verbindet angegebene Fenster
bool MFVideoPlayer::Initialize(HWND hwnd)
{
    m_hwnd = hwnd;
    // Factory erzeugt Media Engine Instanzen
    IMFMediaEngineClassFactory* factory = nullptr;
    // COM Objekt für die Media Engine erstellen
    HRESULT hr = CoCreateInstance( CLSID_MFMediaEngineClassFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&factory));
    if (FAILED(hr))
    {
        MessageBoxW( hwnd,L"Media Engine Factory konnte nicht erstellt werden.",L"Fehler",MB_OK);
        return false;
    }
    // Callback Objekt erstellen. --> Für Ereignisse wie Laden oder Ende des Vids
    m_notify = new MediaEngineNotify();

    if (!m_notify)
    {
        factory->Release();
        return false;
    }
    // Attribute enthalten Einstellungen --> erstellung der Media Enginge
    IMFAttributes* attributes = nullptr;
    hr = MFCreateAttributes(&attributes, 2);
    if (FAILED(hr))
    {
        if (m_notify)
        {
            m_notify->Release();
            m_notify = nullptr;
        }

        if (factory)
            factory->Release();
        return false;
    }
    // Callback + Media Engine connection
    hr = attributes->SetUnknown( MF_MEDIA_ENGINE_CALLBACK, m_notify);

    if (FAILED(hr))
    {
        attributes->Release();
        factory->Release();

        m_notify->Release();
        m_notify = nullptr;
        return false;
    }
    // Fenster festlegen, in welches gerendert wird.
    attributes->SetUINT64(MF_MEDIA_ENGINE_PLAYBACK_HWND,(UINT64)m_hwnd);

    if (FAILED(hr))
    {
        attributes->Release();
        factory->Release();
        return false;
    }
    //Audio Kategorie
    attributes->SetUINT32( MF_MEDIA_ENGINE_AUDIO_CATEGORY,AudioCategory_GameMedia);
    IMFMediaEngine* engine = nullptr;
    // Media Engine erstellen
    hr = factory->CreateInstance( 0,attributes, &engine);
    if (SUCCEEDED(hr))
    {
        engine->QueryInterface(IID_PPV_ARGS(&m_engine));
        engine->Release();
    }
    // Nicht mehr benötigte COM Objekte freigeben
    attributes->Release();
    factory->Release();

    if (FAILED(hr))
    {
        MessageBoxW(
            hwnd,
            L"Media Engine konnte nicht erstellt werden.",
            L"Fehler",
            MB_OK);

        return false;
    }
    return true;
}

bool MFVideoPlayer::Open(const wchar_t* filename)   //lädt video Datei Aktuell nur über das Menü verfügbar
{
    if (!m_engine || !filename)
        return false;

    BSTR url = SysAllocString(filename);
    HRESULT hr = m_engine->SetSource(url);
    SysFreeString(url);

    if (FAILED(hr))
    {
        MessageBoxW( m_hwnd, L"Video konnte nicht geladen werden.",L"Fehler", MB_OK);
        return false;
    }
    m_engine->Load();
    return true;
}
//------------------------
//Video Standartfunktionen 
//------------------------
void MFVideoPlayer::Play()
{
    if (!m_engine)
        return;
    HRESULT hr = m_engine->Play();
    if (FAILED(hr))
    {
        MessageBoxW( m_hwnd,L"Play fehlgeschlagen", L"Fehler", MB_OK);
    }
}

void MFVideoPlayer::Pause()
{
    if (!m_engine)
    {return;}
    {
        m_engine->Pause();
    }
}

void MFVideoPlayer::Shutdown()
{
    if (m_engine)
    {
        m_engine->Pause();
        m_engine->Shutdown();
        m_engine->Release();
        m_engine = nullptr;
    }
    if (m_notify)
    {
        m_notify->Release();
        m_notify = nullptr;
    }
}

double MFVideoPlayer::GetDuration()
{
    if (!m_engine)
        return 0.0;
    return m_engine->GetDuration();
}

double MFVideoPlayer::GetPosition()
{
    if (!m_engine)
        return 0.0;
    return m_engine->GetCurrentTime();
}

void MFVideoPlayer::Seek(double seconds)
{
    if (!m_engine)
        return;
    m_engine->SetCurrentTime(seconds);
}

void MFVideoPlayer::SetRate(float rate)
{
    if (!m_engine)
        return;
    m_engine->SetPlaybackRate(rate);
}

float MFVideoPlayer::GetRate()
{
    if (!m_engine)
        return 1.0f;
    return m_engine->GetPlaybackRate();
}
