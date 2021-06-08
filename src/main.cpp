#include <iostream>

#include <chrono>
#include <thread>

#include <atlexcept.h>

#include "audio_manager.hpp"
#include "int_to_hex_string.hpp"

int main()
{
    try
    {
        AudioManager audio_manager;
        auto audio_session_map = audio_manager.get_audio_sessions();

        std::wstring process_name(L"firefox.exe");
        auto it = audio_session_map.find(process_name);
        if (it != audio_session_map.end())
        {
            std::wcout << process_name << ":\n\n";

            if(auto err = it->second.set_master_volume(0.6, NULL); !err.empty()) {
                std::cout << "Failed to set_master_volume for:\n";
                for(auto const & [pentry, hr] : err) {
                    std::cout << " pid: " << pentry.th32ProcessID << " hr: " << int_to_hex_string(hr) << '\n';
                }
            }
            
            if(auto err = it->second.set_mute(true, NULL); !err.empty()) {
                std::cout << "Failed to set_master_volume for:\n";
                for(auto const & [pentry, hr] : err) {
                    std::cout << " pid: " << pentry.th32ProcessID << " hr: " << int_to_hex_string(hr) << '\n';
                }
            }

            auto audio_sessions = it->second.get_sessions();
            for (auto const &audio_session : audio_sessions)
            {
                audio_session.print();
            }
        }
        else
        {
            std::wcout << "Could not find session called " << process_name << '\n';
        }
    }
    catch (ATL::CAtlException const &e)
    {
        std::cout << "ATL::CAtlException - " << int_to_hex_string(HRESULT(e)) << '\n';
    }
    catch (std::exception const &e)
    {
        std::cout << "std::exception - " << e.what() << '\n';
    }
    catch (...)
    {
        std::cout << "Uncaught exception\n";
    }
    std::cout << "Exited successfully\n";
}