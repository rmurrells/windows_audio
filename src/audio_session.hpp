#pragma once

#include <tuple>
#include <atlbase.h>
#include <audiopolicy.h>
#include <TlHelp32.h>

struct AudioSession
{
    AudioSession(CComPtr<IAudioSessionControl> const &pAudioSessionControl);
    ~AudioSession() = default;
    std::tuple<float, HRESULT> get_master_volume() const;
    std::tuple<BOOL, HRESULT> get_mute() const;
    PROCESSENTRY32W const &get_pentry() const;
    std::tuple<AudioSessionState, HRESULT> get_state() const;
    HRESULT set_master_volume(float fLevel, LPCGUID EventContext);
    HRESULT set_mute(BOOL mute, LPCGUID EventContext);

private:
    CComPtr<IAudioSessionControl2> pAudioSessionControl2;
    CComPtr<ISimpleAudioVolume> pSimpleAudioVolume;
    PROCESSENTRY32W pentry;
};