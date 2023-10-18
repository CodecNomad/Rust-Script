#pragma once

#include <sapi.h>
#include <string>

namespace tts {

	inline void read_out(const char* message) {
	    ISpVoice* p_voice = nullptr;

	    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
	    if (FAILED(hr)) {
	        return;
	    }

	    hr = CoCreateInstance(CLSID_SpVoice, nullptr, CLSCTX_ALL, IID_ISpVoice, reinterpret_cast<void**>(&p_voice));
	    if (SUCCEEDED(hr)) {
	        std::wstring wide_message;
	        if (message != nullptr) {
	            const int len = MultiByteToWideChar(CP_UTF8, 0, message, -1, nullptr, 0);
	            wide_message.resize(len);
	            MultiByteToWideChar(CP_UTF8, 0, message, -1, &wide_message[0], len);
	        }
	        hr = p_voice->Speak(wide_message.c_str(), 0, nullptr);

	        if (FAILED(hr)) {
	            return;
	        }

	        p_voice->Release();
	    }
	    else {
	        return;
	    }

	    CoUninitialize();
	}

}
