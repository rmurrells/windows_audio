#include "audio_session_group.hpp"

#include <iostream>

#include "int_to_hex_string.hpp"

AudioSessionGroup::AudioSessionGroup(WCHAR const *name) : name{name}
{
}

void AudioSessionGroup::add(AudioSession &&audio_session)
{
    this->audio_sessions.emplace_back(audio_session);
}

std::wstring const &AudioSessionGroup::get_name() const
{
    return this->name;
}

std::vector<AudioSession> const &AudioSessionGroup::get_sessions() const
{
    return this->audio_sessions;
}

void AudioSessionGroup::print() const
{
    for (auto const &audio_session : this->audio_sessions)
    {
        audio_session.print();
    }
}

ErrorGroup AudioSessionGroup::set_master_volume(float const fLevel, LPCGUID EventContext)
{
    return this->run_fn(std::move(std::bind(&AudioSession::set_master_volume, std::placeholders::_1, fLevel, EventContext)));
}

ErrorGroup AudioSessionGroup::set_mute(bool const mute, LPCGUID EventContext)
{
    return this->run_fn(std::move(std::bind(&AudioSession::set_mute, std::placeholders::_1, mute, EventContext)));
}

template <class Fn>
ErrorGroup AudioSessionGroup::run_fn(Fn &&fn)
{
    ErrorGroup err;
    for (auto &audio_session : this->audio_sessions)
    {
        if (auto hr = fn(&audio_session); FAILED(hr))
        {
            err.emplace_back(audio_session.get_pentry(), hr);
        }
    }
    return err;
}