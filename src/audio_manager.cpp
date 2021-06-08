#pragma once

#include "audio_manager.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

#include "int_to_hex_string.hpp"

AudioManager::AudioManager()
{
    if (auto hr = this->pMMDeviceEnumerator.CoCreateInstance(
            __uuidof(MMDeviceEnumerator),
            NULL,
            CLSCTX_ALL);
        FAILED(hr))
    {
        throw std::runtime_error("CoCreateInstance failed with " + int_to_hex_string(hr));
    }

    if (auto hr = this->pMMDeviceEnumerator->GetDefaultAudioEndpoint(
            eRender,
            eConsole,
            &this->pMMDevice);
        FAILED(hr))
    {
        throw std::runtime_error("GetDefaultAudioEndpoint failed with " + int_to_hex_string(hr));
    }

    if (auto hr = this->pMMDevice->Activate(
            __uuidof(IAudioSessionManager2),
            CLSCTX_ALL,
            NULL,
            (void **)&this->pAudioSessionManager2);
        FAILED(hr))
    {
        throw std::runtime_error("Activate failed with " + int_to_hex_string(hr));
    }

    if (auto hr = this->pAudioSessionManager2->GetSessionEnumerator(
            &this->pAudioSessionEnumerator);
        FAILED(hr))
    {
        throw std::runtime_error("GetSessionEnumerator failed with " + int_to_hex_string(hr));
    }
}

AudioSessionMap AudioManager::get_audio_sessions()
{
    int session_count = 0;
    if (auto hr = this->pAudioSessionEnumerator->GetCount(
            &session_count);
        FAILED(hr))
    {
        throw std::runtime_error("GetCount failed with " + int_to_hex_string(hr));
    }

    AudioSessionMap audio_sessions;
    for (int session_idx = 0; session_idx < session_count; ++session_idx)
    {
        CComPtr<IAudioSessionControl> pAudioSessionControl;
        if (auto hr = this->pAudioSessionEnumerator->GetSession(
                session_idx,
                &pAudioSessionControl);
            FAILED(hr))
        {
            throw std::runtime_error("GetSession (" + std::to_string(session_idx) + ") failed with " + int_to_hex_string(hr));
        }

        AudioSession audio_session(pAudioSessionControl);
        if (auto [hr, state] = audio_session.get_state(); FAILED(hr)) {
            throw std::runtime_error("get_state (" + std::to_string(session_idx) + ") failed with " + int_to_hex_string(hr));
        }
        else if (state != AudioSessionStateExpired)
        {
            auto session_name = audio_session.get_pentry().szExeFile;
            audio_sessions.emplace(session_name, AudioSessionGroup(session_name)).first->second.add(std::move(audio_session));
        }
    }
    return audio_sessions;
}