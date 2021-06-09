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
}

ArgMap::ArgMap() : arg_map{}
{
    this->arg_map.emplace("help", [](Tokens const &tokens, AudioManager &audio_manager)
                          {
                              std::cout
                                  << "\n"
                                  << "list: display the names of current audio processes.\n\n"
                                  << "list_info: display the names of current audio processes and relevant information.\n\n"
                                  << "set <name> <volume scale from 0 - 1>: sets all processes with the process name to the supplied volume scale.\n\n"
                                  << "mute <name> <1 / 0>: mutes / unmutes all processes with the process name.\n\n"
                                  << "quit: terminates the program.\n\n";
                          });
    this->arg_map.emplace("list", [](Tokens const &tokens, AudioManager &audio_manager)
                          {
                              std::cout << "speakers\n";
                              for (auto const &audio_session_group : audio_manager.get_audio_sessions())
                              {
                                  std::wcout << audio_session_group.second.get_name() << '\n';
                              }
                          });
    this->arg_map.emplace("list_info", [](Tokens const &tokens, AudioManager &audio_manager)
                          {
                              std::cout << "\nspeakers:\n";
                              if (auto [volume, hr] = audio_manager.get_speaker_volume(); FAILED(hr))
                              {
                                  std::cout << "  Failed to get_speaker_volume with hr: " << int_to_hex_string(hr) << "\n";
                              }
                              else
                              {
                                  std::cout << "  Volume: " << volume << '\n';
                              }
                              if (auto [mute, hr] = audio_manager.get_speaker_mute(); FAILED(hr))
                              {
                                  std::cout << "  Failed to get_speaker_mute with hr: " << int_to_hex_string(hr) << "\n";
                              }
                              else
                              {
                                  std::cout << "  Mute: " << mute << "\n\n";
                              }
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

                              auto const &vol_str = tokens[2];
                              double volume{0};
                              try
                              {
                                  volume = std::stod(vol_str);
                              }
                              catch (...)
                              {
                                  std::cout << "Cannot convert " << vol_str << " to volume scale.\n";
                                  return;
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
                          });
    this->arg_map.emplace("mute", [](Tokens const &tokens, AudioManager &audio_manager)
                          {
                              if (!check_arg_no(tokens, 3))
                              {
                                  return;
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
                                  return;
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
