#pragma once

#include <iostream>
#include <thread>
#include <chrono>

void loadingBar(int length, int duration) {
    for (int i = 0; i <= length; i++) {
        int percentage = (i * 100) / length;

        std::cout << "\033[93m\r{";  // Yellow curly brace
        for (int j = 0; j < i; j++) std::cout << "\033[91m="; // Red progress
        for (int j = i; j < length; j++) std::cout << " ";
        std::cout << "\033[93m} \033[91m" << percentage << "% " << std::flush; // Yellow brace, red percentage

        std::this_thread::sleep_for(std::chrono::milliseconds(duration / length));
    }
    std::cout << "\033[92m Done!\033[0m" << std::endl; // Green "Done!" with reset
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

void clearScreen() {
    std::cout << "\033[2J\033[H" << std::flush;
}

void hideCursor() {
    std::cout << "\033[?25l" << std::flush;
}

void showCursor() {
    std::cout << "\033[?25h" << std::flush;
}

void setColor(int textColor, int backgroundColor) {
    std::cout << "\033[" << textColor << ";" << backgroundColor << "m";
}