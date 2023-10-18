#include <Windows.h>
#include <iostream>
#include <thread>
#include <mutex>
#include "additional/all.h"

namespace globals {
    settings script_settings(90, 15, 1);
    gun current_gun = rust_guns::ak;

    double barrel_multiplier = barrel::barrel_none;
	double sight_multiplier = sight::scope_none;

    bool script_on = false;
    bool mouse_held = false;
    int spray_num = 0;
};

     
static LONG (__stdcall*nt_delay_execution)(BOOL alertable, PLARGE_INTEGER delay_interval) = reinterpret_cast<LONG(
	__stdcall*)(BOOL, PLARGE_INTEGER)>(GetProcAddress(
    GetModuleHandle(L"ntdll.dll"), "NtDelayExecution"));
 
void query_sleep(const long long ms)
{
    const auto start = std::chrono::high_resolution_clock::now();
 
    LARGE_INTEGER interval;
 
    interval.QuadPart = -ms;
 
    nt_delay_execution(false, &interval);
 
    std::chrono::duration<double, std::milli> elapsed = std::chrono::high_resolution_clock::now() - start;
 
    while (ms >= elapsed.count())
    {
    	elapsed = std::chrono::high_resolution_clock::now() - start;
    }
}

LRESULT CALLBACK keyboard_proc(const int n_code, const WPARAM w_param, const LPARAM l_param) {
    if (n_code >= 0) {
        if (w_param == WM_KEYDOWN) {
	        const auto* kbd_data = reinterpret_cast<KBDLLHOOKSTRUCT*>(l_param);
		    if (kbd_data && kbd_data->vkCode == VK_OEM_MINUS) {
		        if (abs(globals::sight_multiplier - sight::scope_holo) < 0.1)
		        {
			        globals::sight_multiplier = sight::scope_none;
                    std::thread tts_thread(tts::read_out, "Set ironsight.");
					tts_thread.detach();
		        } else {
			        globals::sight_multiplier = sight::scope_holo;
                    std::thread tts_thread(tts::read_out, "Set holo.");
					tts_thread.detach();
		        }
		        
		    }
            if (kbd_data && kbd_data->vkCode == VK_OEM_PLUS) {
		        if (abs(globals::barrel_multiplier - barrel::barrel_silencer) < 0.1)
		        {
			        globals::barrel_multiplier = barrel::barrel_none;
                    std::thread tts_thread(tts::read_out, "Set barrel.");
					tts_thread.detach();
		        } else {
			         globals::barrel_multiplier = barrel::barrel_silencer;
                    std::thread tts_thread(tts::read_out, "Set silencer.");
					tts_thread.detach();
		        }
		        
		    }
        }
        return CallNextHookEx(nullptr, n_code, w_param, l_param);
    }
    return CallNextHookEx(nullptr, n_code, w_param, l_param);
}

LRESULT CALLBACK mouse_proc(const int n_code, const WPARAM w_param, const LPARAM l_param) {
    if (n_code >= 0) {
        if (w_param == WM_LBUTTONDOWN) {
            globals::mouse_held = true;
        }
        else if (w_param == WM_LBUTTONUP) {
            globals::mouse_held = false;
            globals::spray_num = 0;
        }
        return CallNextHookEx(nullptr, n_code, w_param, l_param);
    }
    return CallNextHookEx(nullptr, n_code, w_param, l_param);
}

void move_mouse_to_recoil() {
	const int max_spray = static_cast<int>(globals::current_gun.recoil_patterns.size());
    if (globals::spray_num > max_spray - 1) {
        return;
    }

	const double x_recoil = globals::current_gun.recoil_patterns[globals::spray_num].first * globals::sight_multiplier * globals::barrel_multiplier;
	const double y_recoil = globals::current_gun.recoil_patterns[globals::spray_num].second * globals::sight_multiplier * globals::barrel_multiplier;

	const double sensitivity_multiplier = (globals::script_settings.sensitivity * 2) * 3.0 * (globals::script_settings.fov / 100.0);

	const double x_movement = x_recoil / (-0.03 * sensitivity_multiplier);
    const double y_movement = y_recoil / (-0.03 * sensitivity_multiplier);
    const double delay = globals::current_gun.delay;
    double pixel_loss_x = 0;
    double pixel_loss_y = 0;
    for (int i = 0; i != globals::script_settings.smoothing_factor; i++) {
	    const double mov_x = (x_movement * 10) / globals::script_settings.smoothing_factor;
	    const double mov_y = (y_movement * 10) / globals::script_settings.smoothing_factor;
        pixel_loss_x += mov_x - static_cast<int>(mov_x);
        pixel_loss_y += mov_y - static_cast<int>(mov_y);
        int compensation_x = 0;
        int compensation_y = 0;
        if (pixel_loss_x >= 1) {
            compensation_x += static_cast<int>(pixel_loss_x);
            pixel_loss_x -= static_cast<int>(pixel_loss_x);
        }
        if (pixel_loss_y >= 1) {
            compensation_y += static_cast<int>(pixel_loss_y);
            pixel_loss_y -= static_cast<int>(pixel_loss_y);
        }
	    mouse::move(static_cast<int>(mov_x) + compensation_x, static_cast<int>(mov_y) + compensation_y);
        query_sleep(delay / globals::script_settings.smoothing_factor);
    }
}


void main_script_loop()
{
	while (true)
	{
		if (globals::script_on && globals::mouse_held)
		{
			globals::spray_num += 1;
			move_mouse_to_recoil();
		}
	}
}

int weapon_detection_loop()
{
	const HWND hwnd_desktop = GetDesktopWindow();

	while ( true ) {
		constexpr int y2 = 1060;
		constexpr int x2 = 1140;
	    constexpr int y1 = 1000;
	    constexpr int x1 = 760;

	    const double ak = wpd::is_on_screen("img/" + rust_guns::ak.name + ".png", hwnd_desktop, x1, y1, x2, y2);
        const double m2 = wpd::is_on_screen("img/" + rust_guns::m2.name + ".png", hwnd_desktop, x1, y1, x2, y2);
        const double hmlmg = wpd::is_on_screen("img/" + rust_guns::hmlmg.name + ".png", hwnd_desktop, x1, y1, x2, y2);
        const double mp5 = wpd::is_on_screen("img/" + rust_guns::mp5.name + ".png", hwnd_desktop, x1, y1, x2, y2);
        const double thompson = wpd::is_on_screen("img/" + rust_guns::thompson.name + ".png", hwnd_desktop, x1, y1, x2, y2);
        const double custom = wpd::is_on_screen("img/" + rust_guns::custom.name + ".png", hwnd_desktop, x1, y1, x2, y2);


		const double largest_percentage = std::max({ak, m2, hmlmg, mp5, thompson, custom});

		constexpr double min_percentage = 0.75;
		if (largest_percentage >= min_percentage)
		{
			constexpr double tolerance = 0.01;
			if (std::abs(largest_percentage - ak) < tolerance) {
		        globals::current_gun = rust_guns::ak;
		        globals::script_on = true;
		    } else if (std::abs(largest_percentage - m2) < tolerance) {
		        globals::current_gun = rust_guns::m2;
		        globals::script_on = true;
		    } else if (std::abs(largest_percentage - hmlmg) < tolerance) {
		        globals::current_gun =  rust_guns::hmlmg;
		        globals::script_on = true;
		    } else if (std::abs(largest_percentage - mp5) < tolerance) {
		        globals::current_gun =  rust_guns::mp5;
		        globals::script_on = true;
		    } else if (std::abs(largest_percentage - thompson) < tolerance) {
		        globals::current_gun =  rust_guns::thompson;
		        globals::script_on = true;
		    } else if (std::abs(largest_percentage - custom) < tolerance) {
		        globals::current_gun =  rust_guns::custom;
		        globals::script_on = true;
		    }
		} else {
		    globals::script_on = false;
		}
        query_sleep(1);
    }
}

int main() {
	disguise::run();

    std::thread tts_thread(tts::read_out, "Time to beam some monkeyboobos!");
    tts_thread.detach();

    std::thread weapon_thread(weapon_detection_loop);
    weapon_thread.detach();

    const HHOOK keyboard_hook = SetWindowsHookEx(WH_KEYBOARD_LL, keyboard_proc, nullptr, 0);
    const HHOOK mouse_hook = SetWindowsHookEx(WH_MOUSE_LL, mouse_proc, nullptr, 0);

    if (keyboard_hook == nullptr || mouse_hook == nullptr) {
        MessageBox(nullptr, L"Hook failed to install.", L"Error", MB_ICONERROR);
        return 1;
    }

    std::thread main_script_thread(main_script_loop);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(keyboard_hook);
    UnhookWindowsHookEx(mouse_hook);

    main_script_thread.join();
    return 0;
}