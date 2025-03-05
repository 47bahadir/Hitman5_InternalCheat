// SpeechManager.h
#pragma once
#include <string>
#include <sapi.h>
#include <wrl/client.h>

#pragma warning(push)
#pragma warning(disable: 4996)  // Disable C4996 warning
#include <sphelper.h>
#pragma warning(pop)  // Restore original warning settings

namespace SpeechManager {
	// Initialize speech synthesis system
	// Must be called before using other functions
	bool Initialize();

	// Clean up speech resources
	// Releases COM objects and uninitialized subsystems
	void Cleanup();

	// Convert text to speech with interruption support
	// @param text: UTF-16 encoded string to speak
	// @return true if synthesis started successfully
	bool Speak(const std::wstring& text);

	// Set output volume level
	// @param volume: Range 0 (silent) to 100 (max)
	void SetVolume(USHORT volume);

	// Set speech rate (speed)
	// @param rate: Range -10 (slowest) to 10 (fastest)
	void SetRate(short rate);
};