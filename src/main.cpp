#include <algorithm>
#include <iostream>
#include <iterator>
#include <chrono>
#include <thread>

#include <atlexcept.h>

#include "audio_manager.hpp"
#include "int_to_hex_string.hpp"

using Tokens = std::vector<std::string>;

bool check_arg_no(Tokens const &tokens, Tokens::size_type target)
{
    auto actual{tokens.size()};
    if (actual < target)
    {
        std::cout << "Expected " << std::to_string(target) << " arguments, got " << actual << '\n';
        return false;
    }
    return true;
}

Tokens split(std::string const &line, char delim)
{
    Tokens tokens;
    std::string::size_type start = 0;
    std::string::size_type end = line.find('\"');

    while ((end = line.find(delim, start)) != std::string::npos)
    {
        if (end != start)
        {
            tokens.emplace_back(line.substr(start, end - start));
        }
        start = end + 1;
    }
    if (start != line.size())
    {
        tokens.emplace_back(line.substr(start));
    }
    return tokens;
}

void trim_tokens(Tokens &tokens)
{
    for (auto it = tokens.begin(); it != tokens.end();)
    {
        auto start = it->find_first_not_of(' ');
        if (start == std::string::npos)
        {
            it = tokens.erase(it);
        }
        else
        {
            auto end = it->find_last_not_of(' ');
            *it = it->substr(start, (end - start + 1));
            ++it;
        }
    }
}

Tokens tokenize(std::string const &line)
{
    Tokens tokens = split(line, '\"');
    trim_tokens(tokens);
    return tokens;
}

bool process(std::string const &line, AudioManager &audio_manager)
{
    auto tokens = tokenize(line);
    if (tokens.empty())
    {
        return true;
    }
    auto const &command = tokens[0];
    if (command == "help")
    {
        std::cout
            << "\n"
            << "list (l): display the names of current audio processes.\n\n"
            << "list_info (li): display the names of current audio processes and relevant information.\n\n"
            << "set (s) <name> <volume scale from 0 - 1>: set all processes with the process name to the supplied volume scale.\n\n"
            << "mute (m) <name> <1 / 0>: mutes / unmute all processes with the process name.\n\n"
            << "quit (q): terminate the program.\n\n";
    }
    else if (command == "list" || command == "l")
    {
        std::cout << "speakers\n";
        for (auto const &audio_session_group : audio_manager.get_audio_sessions())
        {
            std::wcout << audio_session_group.second.get_name() << '\n';
        }
    }
    else if (command == "list_info" || command == "li")
    {
        std::cout << "\nspeakers:\n";
        if (auto [volume, hr] = audio_manager.get_speaker_volume(); FAILED(hr))
        {
            std::cout << "  Failed to get_speaker_volume with hr: " << iths(hr) << "\n";
        }
        else
        {
            std::cout << "  Volume: " << volume << '\n';
        }
        if (auto [mute, hr] = audio_manager.get_speaker_mute(); FAILED(hr))
        {
            std::cout << "  Failed to get_speaker_mute with hr: " << iths(hr) << "\n";
        }
        else
        {
            std::cout << "  Mute: " << mute << "\n\n";
        }
        for (auto const &audio_session_group : audio_manager.get_audio_sessions())
        {
            audio_session_group.second.print();
        }
    }
    else if (command == "set" || command == "s")
    {
        if (!check_arg_no(tokens, 3))
        {
            return true;
        }

        auto const &vol_str = tokens[2];
        double volume{0};
        try
        {
            volume = std::stod(vol_str);
        }
        catch (...)
        {
            std::cout << "Cannot convert " << vol_str << " to volume scale.\n";
            return true;
        }

        auto &audio_map = audio_manager.get_audio_sessions();
        auto const &name = tokens[1];
        if (auto it = audio_map.find(std::wstring{name.begin(), name.end()}); it != audio_map.end())
        {
            it->second.set_master_volume(volume, NULL);
        }
        else if (name == "speakers")
        {
            audio_manager.set_speaker_volume(volume, NULL);
        }
        else
        {
            std::cout << "Unrecognized process name: " << name << '\n';
        }
    }
    else if (command == "mute" || command == "m")
    {
        if (!check_arg_no(tokens, 3))
        {
            return true;
        }

        auto const &set_str = tokens[2];
        bool set{false};
        try
        {
            set = std::stoi(set_str);
        }
        catch (...)
        {
            std::cout << "Cannot convert \"" << set_str << "\" to bool.\n";
            return true;
        }

        auto &audio_map = audio_manager.get_audio_sessions();
        auto const &name = tokens[1];
        if (auto it = audio_map.find(std::wstring{name.begin(), name.end()}); it != audio_map.end())
        {
            it->second.set_mute(set, NULL);
        }
        else if (name == "speakers")
        {
            audio_manager.set_speaker_mute(set, NULL);
        }
        else
        {
            std::cout << "Unrecognized process name: " << name << '\n';
        }
    }
    else if (command == "quit" || command == "q")
    {
        return false;
    }
    else
    {
        std::cout << "Unrecognized command: " << line << '\n';
    }
    return true;
}

int main()
{
    AudioManager audio_manager;
    std::string line;

    while (true)
    {
        std::cout << "command: ";
        std::getline(std::cin, line);
        try
        {
            if (!process(line, audio_manager))
            {
                break;
            }
        }
        catch (ATL::CAtlException const &e)
        {
            std::cout << "ATL::CAtlException - " << iths(HRESULT(e)) << '\n';
        }
        catch (std::exception const &e)
        {
            std::cout << "std::exception - " << e.what() << '\n';
        }
        catch (...)
        {
            std::cout << "Uncaught exception\n";
        }
    }
loop_end:
    std::cout << "Exited successfully\n";
}