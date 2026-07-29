# MFVideoPlayer

Ein selbst entwickelter Video-Player in C++ unter Verwendung der Windows API und Microsoft Media Foundation.
Das Projekt entstand als Lernprojekt, um praktische Erfahrungen mit nativer Windows-Entwicklung, Multimedia-Verarbeitung und der Media Foundation API zu sammeln.

## Autor
Andreas Gröber

## Technologien

- C++
- Windows API (Win32)
- Microsoft Media Foundation
- Visual Studio 2022

## Aktuelle Funktionalitäten (1.0)

- Öffnen von Videodateien über einen Dateiauswahldialog
- Wiedergabe von Videodateien über Microsoft Media Foundation
- Play/Pause Steuerung
- Fortschrittsanzeige über eine Zeitleiste (Slider)
- Anzeige der aktuellen Position und Gesamtdauer des Videos sowie die Videogeschwinigkeit
- Änderung der Wiedergabegeschwindigkeit über das Mausrad
- 
- Menüleiste mit Datei öffnen und Programm schließen

## Geplante Erweiterungen (2.0)

- Dynamische Anpassung der Oberfläche beim Ändern der Fenstergröße
- Lautstärkeregelung
- Audio- und Videospurauswahl
- Verbesserte Benutzeroberfläche/Stylen des Fensters
- Anzeige von Videoinformationen (Auflösung, Codec, FPS)


## Voraussetzungen

Zum Kompilieren werden benötigt:

- Windows 10/11
- Visual Studio 2022
- C++ Desktopentwicklung
- Windows SDK

## Build
Das Projekt kann direkt über die enthaltene Visual-Studio-Solution geöffnet und kompiliert werden.
Empfohlene Konfiguration: x64 Debug/Release

## Hinweise
Der Player verwendet die native Windows Media Foundation API und ist daher aktuell ausschließlich für Windows-Systeme ausgelegt.

## Lizenz
Dieses Projekt dient hauptsächlich Lernzwecken.
Eine Nutzung oder Weiterentwicklung ist erlaubt.
