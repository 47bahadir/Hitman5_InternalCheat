// SpeechManager.cpp
#include "pch.h"
#include "speech.h"
#include <iostream>

#pragma comment(lib, "sapi.lib")        // Link SAPI speech library
#pragma comment(lib, "comsuppw.lib")    // Link COM support library

using namespace Microsoft::WRL;        // Use WRL smart pointers

namespace {
	// Anonymous namespace for internal implementation
	//===============================================
	// **Core Speech Components**
	//===============================================
	ComPtr<ISpVoice> g_pVoice;         // Speech synthesizer instance
	bool g_initialized = false;        // Initialization status flag

	//===============================================
	// **Debug Logging**
	//===============================================
	void DebugLog(const std::wstring& msg) {
		OutputDebugStringW((L"[Speech] " + msg + L"\n").c_str()); // Output to debugger
	}

	//===============================================
	// **Voice Token Initialization**
	// Purpose: Enumerate and select English voice token
	// Returns: true - suitable device found, false - use default
	//===============================================
	bool InitVoiceToken() {
		ComPtr<IEnumSpObjectTokens> pEnum;
		// Enumerate available voice tokens
		HRESULT hr = SpEnumTokens(SPCAT_VOICES, nullptr, nullptr, &pEnum);
		if (FAILED(hr)) return false;

		// Get voice count
		ULONG count = 0;
		pEnum->GetCount(&count);
		DebugLog(L"Found " + std::to_wstring(count) + L" voices");

		// Iterate to find English voice
		for (ULONG i = 0; i < count; ++i) {
			ComPtr<ISpObjectToken> pToken;
			if (SUCCEEDED(pEnum->Item(i, &pToken))) {
				WCHAR* lang = nullptr;
				ComPtr<ISpDataKey> attrs;

				// Get voice attributes
				if (SUCCEEDED(pToken->OpenKey(L"Attributes", &attrs)) &&
					SUCCEEDED(attrs->GetStringValue(L"Language", &lang))) {

					// Check for English (language code 409 = en-US)
					if (wcscmp(lang, L"409") == 0) {
						g_pVoice->SetVoice(pToken.Get()); // Set voice token
						CoTaskMemFree(lang);
						return true;
					}
					CoTaskMemFree(lang);
				}
			}
		}
		return false; // No suitable device found
	}
}

namespace SpeechManager {
	//===============================================
	// **Initialize Speech System**
	// Execution Steps:
	// 1. Initialize COM components
	// 2. Create speech synthesizer instance
	// 3. Select voice token
	// 4. Set default volume/rate
	//===============================================
	bool Initialize() {
		if (g_initialized) return true; // Prevent re-initialization

		// Initialize COM (single-threaded apartment)
		HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
		if (FAILED(hr)) {
			DebugLog(L"COM init failed: " + std::to_wstring(hr));
			return false;
		}

		// Create speech synthesizer instance
		hr = CoCreateInstance(CLSID_SpVoice, nullptr, CLSCTX_ALL,
			IID_PPV_ARGS(&g_pVoice));
		if (FAILED(hr)) {
			DebugLog(L"Create voice failed: " + std::to_wstring(hr));
			CoUninitialize(); // Cleanup COM
			return false;
		}

		// Attempt to select English voice
		if (!InitVoiceToken()) {
			DebugLog(L"Using default voice"); // Use first available
		}

		// Set default parameters
		SetVolume(100);  // Maximum volume
		SetRate(0);      // Normal speaking rate
		g_initialized = true;
		return true;
	}

	//===============================================
	// **Cleanup Speech Resources**
	// Execution Steps:
	// 1. Stop current speech
	// 2. Release voice instance
	// 3. Uninitialize COM
	//===============================================
	void Cleanup() {
		if (g_pVoice) {
			// Purge queue and stop playback
			g_pVoice->Speak(nullptr, SPF_PURGEBEFORESPEAK, nullptr);
			g_pVoice->Pause();     // Pause speech
			g_pVoice = nullptr;    // Release resources
		}
		if (g_initialized) {
			CoUninitialize();      // Must pair with CoInitializeEx
			g_initialized = false;
		}
	}

	//===============================================
	// **Text-to-Speech Synthesis**
	// Parameters: text - UTF-16 text to speak
	// Features:
	// - Asynchronous playback (SPF_ASYNC)
	// - Auto-interrupt previous speech (SPF_PURGEBEFORESPEAK)
	// - Non-XML text (SPF_IS_NOT_XML)
	//===============================================
	bool Speak(const std::wstring& text) {
		// Lazy initialization
		if (!g_initialized && !Initialize()) {
			DebugLog(L"Not initialized");
			return false;
		}

		// Perform TTS synthesis
		HRESULT hr = g_pVoice->Speak(
			text.c_str(),
			SPF_ASYNC | SPF_IS_NOT_XML | SPF_PURGEBEFORESPEAK, // Async|Non-XML|Purge
			nullptr
		);

		if (FAILED(hr)) {
			DebugLog(L"Speak failed: " + std::to_wstring(hr));
			return false;
		}
		return true;
	}

	//===============================================
	// **Volume Control (0-100)**
	//===============================================
	void SetVolume(USHORT volume) {
		if (g_pVoice) g_pVoice->SetVolume(volume);
	}

	//===============================================
	// **Speaking Rate Control (-10 to 10)**
	// - Negative: Slower
	// - 0: Normal
	// - Positive: Faster
	//===============================================
	void SetRate(short rate) {
		if (g_pVoice) g_pVoice->SetRate(rate);
	}
}