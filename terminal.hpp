#pragma once

#include <windows.h>

void loadingBar(int length, int duration) {
    for (int i = 0; i <= length; i++) {
        int percentage = (i * 100) / length;
        
        std::cout << "\033[1;33m\r{";
        for (int j = 0; j < i; j++) std::cout << "\033[1;31m=";
        for (int j = i; j < length; j++) std::cout << " ";
        std::cout << "\033[1;33m} \033[1;31m" << percentage << "% " << std::flush;
        
        std::this_thread::sleep_for(std::chrono::milliseconds(duration / length));
    }
    std::cout << "\033[1;32m Done!" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

void clearScreen() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coordScreen = {0, 0};
    DWORD cCharsWritten;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD dwConSize;

    if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
        return;

    dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

    FillConsoleOutputCharacter(hConsole, TEXT(' '), dwConSize, coordScreen, &cCharsWritten);
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    FillConsoleOutputAttribute(hConsole, csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten);
    SetConsoleCursorPosition(hConsole, coordScreen);
}

void hideCursor() {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    
    // Get current cursor information
    GetConsoleCursorInfo(consoleHandle, &cursorInfo);
    
    // Set the cursor visibility to false
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(consoleHandle, &cursorInfo);
}

void setColor(int textColor, int backgroundColor) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    int colorAttribute = (backgroundColor << 4) | textColor;
    SetConsoleTextAttribute(hConsole, colorAttribute);
}