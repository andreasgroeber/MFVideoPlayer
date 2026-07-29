#pragma once

#include <windows.h>

// Menü-IDs
constexpr int ID_MENU_OPEN = 200;
constexpr int ID_MENU_CLOSE = 201;

// Erstellt die Menüleiste
HMENU CreateAppMenu();

// Öffnet den Windows-Dateidialog
bool OpenVideoFileDialog(HWND hwnd, wchar_t* fileName, DWORD fileNameSize);