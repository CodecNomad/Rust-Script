#pragma once

#include <windows.h>
#include <string>
#include <cstdlib>

namespace disguise {

    inline void change_window_name() {
        GetConsoleWindow();
        std::wstring random_title;
        const int title_length = rand() % 26;
        for (int i = 0; i < title_length; ++i) {
            const wchar_t random_char = L'A' + rand() % 26;
            random_title += random_char;
        }

        SetConsoleTitle(random_title.c_str());
    }

    inline void run() {
        change_window_name();
    }

}
