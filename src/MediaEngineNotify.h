#pragma once

#include <mfmediaengine.h>

// Callback-Klasse für die Media Engine.
// Media Foundation ruft diese Klasse auf, wenn bestimmte Ereignisse
// im Media Player auftreten (z.B. Laden beendet, Wiedergabe fertig usw.).
// IMFMediaEngineNotify ist ein COM-Interface und benötigt deshalb
// QueryInterface, AddRef und Release für die Speicherverwaltung.
class MediaEngineNotify : public IMFMediaEngineNotify
{
public:

    // Startet den COM-Referenzzähler mit 1.
    // COM-Objekte werden über Referenzzählung verwaltet.
    MediaEngineNotify(): 
        m_ref(1)
    {

    }
    // Wird von COM aufgerufen, um nach unterstützten Interfaces zu fragen.
    //
    // IMFMediaEngineNotify erbt von IUnknown, deshalb müssen mindestens
    // diese beiden Interfaces unterstützt werden.
    STDMETHODIMP QueryInterface(   REFIID riid,void** ppv)
    {
        if (riid == IID_IUnknown || riid == IID_IMFMediaEngineNotify)
        {
            *ppv = static_cast<IMFMediaEngineNotify*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }

    // Erhöht den COM-Referenzzähler.
    // Sobald ein weiterer Zeiger auf dieses Objekt existiert m_ref++
    STDMETHODIMP_(ULONG) AddRef()
    {
        return ++m_ref;
    }

    // Verringert den COM-Referenzzähler.
    // Wenn keine Referenzen mehr existieren -> delete
    STDMETHODIMP_(ULONG) Release()
    {
        ULONG ref = --m_ref;
        if (ref == 0)
            delete this;
        return ref;
    }
    // Wird von Media Foundation automatisch aufgerufen,
    // wenn ein Ereignis der Media Engine auftritt.
    // Aktuell werden die Events ignoriert.
    STDMETHODIMP EventNotify( DWORD meEvent,DWORD_PTR param1,DWORD param2)
    {
        return S_OK;
    }
private:
    // COM-Referenzzähler, verwaltet Lifetime des Objekts
    LONG m_ref;
};