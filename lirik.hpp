// Credit: https://github.com/refcede/Lyrics

#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <chrono>

void printLirik() {
    std::vector<std::tuple<std::string, int, int>> lirik = {
        {"It's stuck with you forever....", 1000, 70},
        {"so promise you won't let it go", 1000, 70},
        {"I'll trust the universe", 700, 70}, 
        {"will always bring me to you...", 2500, 70},
        {"", 1100, 0},
        {"I'll imagine we fell in love", 700, 70},
        {"I'll nap under moonlight skies with you", 800, 60},
        {"I think I'll picture us, you with the waves", 1300, 60},
        {"The ocean's colors on your face", 1700, 60},
        {"I'll leave my heart with your air", 1000, 80},
        {"So let me fly with you", 1000, 90},
        {"Will you be", 1000, 90},
        {"forever", 1000, 90},
        {"with me?", 1000, 90},
        {"", 400, 0},
    };

    // Cetak lirik dengan huruf muncul satu per satu dengan delay yang berbeda
    for (const auto& baris : lirik) {
        const std::string& teks = std::get<0>(baris);
        int delayBaris = std::get<1>(baris);
        int delayHuruf = std::get<2>(baris);

        for (char c : teks) {
            std::cout << c << std::flush;
            std::this_thread::sleep_for(std::chrono::milliseconds(delayHuruf)); // Delay per huruf
        }
        std::cout << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(delayBaris)); // Delay setelah baris selesai
    }
}
