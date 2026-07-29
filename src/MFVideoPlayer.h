#pragma once


#include <windows.h>

#include <mfapi.h>
#include <mfidl.h>
#include <mfmediaengine.h>

// Die genaue Definition befindet sich in MediaEngineNotify.h.
class MediaEngineNotify;

// Wrapper-Klasse für Media Engine.
class MFVideoPlayer
{

public:

    MFVideoPlayer();
   ~MFVideoPlayer(); // Ressourcen müssen vor Zerstörung über Shutdown() freigegeben werden. Wird gefixt

   //VideoFunktionen
    bool Initialize(HWND hwnd);
    bool Open( const wchar_t* filename);
    void Play();
    void Pause();
    void Shutdown();
   //TimeSlider und Geschwindigkeitsfunktionen
    double GetDuration();
    double GetPosition();
    void SetRate(float rate);
    float GetRate();
    void Seek(double seconds);


private:

    HWND m_hwnd = nullptr;
    IMFMediaEngineEx* m_engine = nullptr;  // IMFMediaEngineEx erweitert IMFMediaEngine um zusätzliche Funktionen wie z.B. Seek und Geschwindigkeitsänderungen.
    MediaEngineNotify* m_notify = nullptr;

};