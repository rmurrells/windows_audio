#include "audio_session.hpp"

#include <iostream>
#include <stdexcept>

#include "int_to_hex_string.hpp"

AudioSession::AudioSession(CComPtr<IAudioSessionControl> const &pAudioSessionControl)
{
    if (auto hr = pAudioSessionControl.QueryInterface(&this->pAudioSessionControl2); FAILED(hr))
    {
        throw std::runtime_error("IAudioSessionControl QueryInterface failed with " + int_to_hex_string(hr));
    }

    if (auto hr = this->pAudioSessionControl2.QueryInterface(&this->pSimpleAudioVolume); FAILED(hr))
    {
        throw std::runtime_error("IAudioSessionControl2 QueryInterface failed with " + int_to_hex_string(hr));
    }

    DWORD pid = 0;
    if (auto hr = this->pAudioSessionControl2->GetProcessId(&pid); FAILED(hr))
    {
        throw std::runtime_error("IAudioSessionControl2 GetProcessId failed with " + int_to_hex_string(hr));
    }

    auto handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, pid);
    if (handle == INVALID_HANDLE_VALUE)
    {
        throw std::runtime_error("CreateToolhelp32Snapshot failed");
    }

    this->pentry.dwSize = sizeof(PROCESSENTRY32W);
    int res = Process32FirstW(handle, &this->pentry);
    if (!res)
    {
        throw std::runtime_error("Process32FirstW failed");
    }

    while (res)
    {
        if (pentry.th32ProcessID == pid)
        {
            CloseHandle(handle);
            break;
        }
        res = Process32NextW(handle, &pentry);
    }
}

std::tuple<float, HRESULT> AudioSession::get_master_volume() const
{
    float pfLevel = 0;
    auto hr = this->pSimpleAudioVolume->GetMasterVolume(&pfLevel);
    return std::make_tuple(pfLevel, hr);
}

std::tuple<BOOL, HRESULT> AudioSession::get_mute() const
{
    BOOL pbMute = 0;
    auto hr = this->pSimpleAudioVolume->GetMute(&pbMute);
    return std::make_tuple(pbMute, hr);
}

PROCESSENTRY32W const &AudioSession::get_pentry() const
{
    return this->pentry;
}

std::tuple<AudioSessionState, HRESULT> AudioSession::get_state() const
{
    AudioSessionState state;
    auto hr = pAudioSessionControl2->GetState(&state);
    return std::make_tuple(state, hr);
}

void AudioSession::print() const
{
    auto pentry = this->get_pentry();
    std::cout << "  pid: " << pentry.th32ProcessID << '\n';
    if (auto [volume, hr] = this->get_master_volume(); FAILED(hr))
    {
        std::cout << "  Failed to get_master_volume with hr: " << int_to_hex_string(hr) << "\n";
    } else {
        std::cout << "  Volume: " << volume << '\n';
    }
    if (auto [mute, hr] = this->get_mute(); FAILED(hr))
    {
        std::cout << "  Failed to get_mute with hr: " << int_to_hex_string(hr) << "\n";
    } else {
        std::cout << "  Mute: " << mute << '\n';
    }
    if(auto [state, hr] = this->get_state(); FAILED(hr))
    {
        std::cout << "  Failed to get_state with hr: " << int_to_hex_string(hr) << "\n";
    } else {
        std::cout << "  State: " << state << '\n';
    }
    std::cout << '\n';
}

HRESULT AudioSession::set_master_volume(float fLevel, LPCGUID EventContext)
{
    return this->pSimpleAudioVolume->SetMasterVolume(fLevel, EventContext);
}

HRESULT AudioSession::set_mute(BOOL mute, LPCGUID EventContext)
{
    return this->pSimpleAudioVolume->SetMute(mute, EventContext);
}