#pragma once

#include <functional>
#include <string>
#include <vector>
#include "audio_session.hpp"

using ErrorGroup = std::vector<std::pair<PROCESSENTRY32W, HRESULT>>;

class AudioSessionGroup
{
public:
    AudioSessionGroup(WCHAR const *name);
    void add(AudioSession &&audio_session);
    std::wstring const &get_name() const;
    std::vector<AudioSession> const &get_sessions() const;
    ErrorGroup set_master_volume(float const fLevel, LPCGUID EventContext);
    ErrorGroup set_mute(bool const mute, LPCGUID EventContext);

private:
    template <class Fn>
    ErrorGroup run_fn(Fn &&fn);
    std::wstring name;
    std::vector<AudioSession> audio_sessions;
};