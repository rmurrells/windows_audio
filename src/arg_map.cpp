#include <iostream>
#include <iterator>

#include "arg_map.hpp"

namespace
{
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

    void set_mute(Tokens const &tokens, AudioManager &audio_manager, bool const set)
    {
        if (!check_arg_no(tokens, 2))
        {
            return;
        }
        auto &audio_map = audio_manager.get_audio_sessions();
        auto const &name = tokens[1];
        if (auto it = audio_map.find(std::wstring{name.begin(), name.end()}); it != audio_map.end())
        {
            it->second.set_mute(set, NULL);
        }
        else
        {
            std::cout << "Unrecognized process name: " << name << '\n';
        }
    }
}

ArgMap::ArgMap() : arg_map{}
{
    this->arg_map.emplace("help", [](Tokens const &tokens, AudioManager &audio_manager)
                          {
                              std::cout
                                  << "\n"
                                  << "list: display the names of current audio processes.\n\n"
                                  << "list_info: display the names of current audio processes and relevant information.\n\n"
                                  << "set <process name> <volume scale from 0 - 1>: sets all processes with the process name to the supplied volume scale.\n\n"
                                  << "mute <process name> <true / false>: mutes / unmutes all processes with the process name.\n\n";
                          });
    this->arg_map.emplace("list", [](Tokens const &tokens, AudioManager &audio_manager)
                          {
                              for (auto const &audio_session_group : audio_manager.get_audio_sessions())
                              {
                                  std::wcout << audio_session_group.second.get_name() << '\n';
                              }
                          });
    this->arg_map.emplace("list_info", [](Tokens const &tokens, AudioManager &audio_manager)
                          {
                              for (auto const &audio_session_group : audio_manager.get_audio_sessions())
                              {
                                  audio_session_group.second.print();
                              }
                          });
    this->arg_map.emplace("set", [](Tokens const &tokens, AudioManager &audio_manager)
                          {
                              if (!check_arg_no(tokens, 3))
                              {
                                  return;
                              }
                              auto &audio_map = audio_manager.get_audio_sessions();
                              auto const &name = tokens[1];
                              if (auto it = audio_map.find(std::wstring{name.begin(), name.end()}); it != audio_map.end())
                              {
                                  try
                                  {
                                      it->second.set_master_volume(std::stod(tokens[2]), NULL);
                                  }
                                  catch (...)
                                  {
                                      std::cout << "Cannot convert " << tokens[2] << " to volume scale.\n";
                                  }
                              }
                              else
                              {
                                  std::cout << "Unrecognized process name: " << name << '\n';
                              }
                          });
    this->arg_map.emplace("mute", [](Tokens const &tokens, AudioManager &audio_manager)
                          {
                              if (!check_arg_no(tokens, 3))
                              {
                                  return;
                              }
                              auto &audio_map = audio_manager.get_audio_sessions();
                              auto const &name = tokens[1];
                              if (auto it = audio_map.find(std::wstring{name.begin(), name.end()}); it != audio_map.end())
                              {
                                  try
                                  {
                                      it->second.set_mute(std::stoi(tokens[2]), NULL);
                                  }
                                  catch (...)
                                  {
                                      std::cout << "Cannot convert \"" << tokens[2] << "\" to bool.\n";
                                  }
                              }
                              else
                              {
                                  std::cout << "Unrecognized process name: " << name << '\n';
                              }
                          });
}

void ArgMap::process(std::string const &line, AudioManager &audio_manager)
{
    Tokens tokens;
    std::copy(std::istream_iterator<std::string>{std::stringstream{line}},
              std::istream_iterator<std::string>{},
              std::back_inserter(tokens));
    if (tokens.empty())
    {
        return;
    }
    if (auto it = this->arg_map.find(tokens[0]); it != arg_map.end())
    {
        it->second(tokens, audio_manager);
    }
    else
    {
        std::cout << "Unrecognized command: " << line << '\n';
    }
}
