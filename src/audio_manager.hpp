#pragma once

#include <unordered_map>

#include <atlbase.h>
#include <audiopolicy.h>
#include <mmdeviceapi.h>

#include "audio_session_group.hpp"
#include "co_initialize_container.hpp"

using AudioSessionMap = std::unordered_map<std::wstring, AudioSessionGroup>;

class AudioManager
{
public:
    AudioManager();
    ~AudioManager() = default;
    AudioManager(AudioManager const &audio_manager) = delete;
    AudioManager &operator=(AudioManager const &) = delete;
    AudioManager(AudioManager &&audio_manager) = delete;
    AudioManager &operator=(AudioManager &&audio_manager) = delete;
    AudioSessionMap get_audio_sessions();

private:
    CoInitializeContainer coinitialize_container;
    CComPtr<IMMDeviceEnumerator> pMMDeviceEnumerator;
    CComPtr<IMMDevice> pMMDevice;
    CComPtr<IAudioSessionManager2> pAudioSessionManager2;
    CComPtr<IAudioSessionEnumerator> pAudioSessionEnumerator;
};