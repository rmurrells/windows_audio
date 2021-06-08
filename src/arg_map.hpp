#pragma once

#include <string>
#include <unordered_map>

#include "audio_manager.hpp"

using Tokens = std::vector<std::string>;

class ArgMap
{
public:
    ArgMap();
    void process(std::string const &line, AudioManager &audio_manager);
private:
    std::unordered_map<std::string, std::function<void(Tokens const &, AudioManager &)>> arg_map;
};