#include <algorithm>
#include <iostream>
#include <iterator>
#include <chrono>
#include <thread>

#include <atlexcept.h>

#include "arg_map.hpp"
#include "audio_manager.hpp"
#include "int_to_hex_string.hpp"

int main()
{
    AudioManager audio_manager;
    ArgMap arg_map;
    std::string line;

    while (true)
    {
        std::cout << "command: ";
        std::getline(std::cin, line);
        try
        {
            if (line == "quit")
            {
                break;
            }
            arg_map.process(line, audio_manager);
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
    }
loop_end:
    std::cout << "Exited successfully\n";
}