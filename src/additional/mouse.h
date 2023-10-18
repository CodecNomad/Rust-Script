#pragma once

#include <windows.h>
#include <thread>
#include <chrono>
#include <ctime>
#include <cstdlib>

namespace mouse {
	// Unused function
    inline void click(const bool right) {
        std::srand(static_cast<unsigned>(std::time(nullptr)));

        INPUT input[2] = {};
        input[0].type = INPUT_MOUSE;
        if (right)
            input[0].mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
        else
            input[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

        std::this_thread::sleep_for(std::chrono::milliseconds((200 - rand() % 20)));

        input[1].type = INPUT_MOUSE;
        if (right)
            input[1].mi.dwFlags = MOUSEEVENTF_RIGHTUP;
        else
            input[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;

        SendInput(2, input, sizeof(INPUT));
    }

    inline void move(const double delta_x, const double delta_y) {
        INPUT input{};
        input.type = INPUT_MOUSE;
        input.mi.mouseData = 0;
        input.mi.time = 0;
        input.mi.dx = static_cast<LONG>(delta_x);
        input.mi.dy = static_cast<LONG>(delta_y);
        input.mi.dwFlags = MOUSEEVENTF_MOVE;

        SendInput(1, &input, sizeof(INPUT));
    }

}