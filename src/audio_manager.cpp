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
        throw std::runtime_error("CoCreateInstance failed with " + iths(hr));
    }

    if (auto hr = this->pMMDeviceEnumerator->GetDefaultAudioEndpoint(
            eRender,
            eConsole,
            &this->pMMDevice);
        FAILED(hr))
    {
        throw std::runtime_error("GetDefaultAudioEndpoint failed with " + iths(hr));
    }

    if (auto hr = this->pMMDevice->Activate(
            __uuidof(IAudioEndpointVolume),
            CLSCTX_ALL,
            NULL,
            (void **)&this->pAudioEndpointVolume);
        FAILED(hr))
    {
        throw std::runtime_error("Activate IAudioEndpointVolume failed with " + iths(hr));
    }

    if (auto hr = this->pMMDevice->Activate(
            __uuidof(IAudioSessionManager2),
            CLSCTX_ALL,
            NULL,
            (void **)&this->pAudioSessionManager2);
        FAILED(hr))
    {
        throw std::runtime_error("Activate IAudioSessionManager2 failed with " + iths(hr));
    }

    if (auto hr = this->pAudioSessionManager2->GetSessionEnumerator(
            &this->pAudioSessionEnumerator);
        FAILED(hr))
    {
        throw std::runtime_error("GetSessionEnumerator failed with " + iths(hr));
    }
}

std::tuple<float, HRESULT> AudioManager::get_speaker_volume() const
{
    float pfLevel = 0;
    auto hr = this->pAudioEndpointVolume->GetMasterVolumeLevelScalar(&pfLevel);
    return std::make_tuple(pfLevel, hr);
}

std::tuple<BOOL, HRESULT> AudioManager::get_speaker_mute() const
{
    BOOL pbMute = 0;
    auto hr = this->pAudioEndpointVolume->GetMute(&pbMute);
    return std::make_tuple(pbMute, hr);
}

HRESULT AudioManager::set_speaker_volume(float fLevel, LPCGUID EventContext) {
    return this->pAudioEndpointVolume->SetMasterVolumeLevelScalar(fLevel, EventContext);
}

HRESULT AudioManager::set_speaker_mute(BOOL mute, LPCGUID EventContext) {
    return this->pAudioEndpointVolume->SetMute(mute, EventContext);
}

AudioSessionMap AudioManager::get_audio_sessions()
{
    int session_count = 0;
    if (auto hr = this->pAudioSessionEnumerator->GetCount(
            &session_count);
        FAILED(hr))
    {
        throw std::runtime_error("GetCount failed with " + iths(hr));
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
            throw std::runtime_error("GetSession (" + std::to_string(session_idx) + ") failed with " + iths(hr));
        }

        AudioSession audio_session(pAudioSessionControl);
        if (auto [hr, state] = audio_session.get_state(); FAILED(hr))
        {
            throw std::runtime_error("get_state (" + std::to_string(session_idx) + ") failed with " + iths(hr));
        }
        else if (state != AudioSessionStateExpired)
        {
            auto session_name = audio_session.get_pentry().szExeFile;
            audio_sessions.emplace(session_name, AudioSessionGroup(session_name)).first->second.add(std::move(audio_session));
        }
    }
    return audio_sessions;
}