# Windows Audio Tool

Simple command line tool to manually set the audio level of running processes.

# Requirements

* an up to date C compiler

* [VSCode](https://code.visualstudio.com)

# Installation

* Open "Developer Command Prompt for VS 20XX".

* Open the top level of the repository using VSCode in the developer command promt, e.g.:
```
code windows_audio
```

* If running for the first time: hold 'ctrl-shift p' in VSCode and type "C/C++ Edit Configurations (UI)" in the resulting bar to generate a c_cpp_properties.json file needed for compilation.

* Compile with 'ctrl-shift b' or click Terminal -> Run Build Task...

# Use

Run the windows_audio.exe binary.

## Commands
* list (l): display the names of current audio processes
* list_info (li): display the names of current audio processes and relevant information
* set (s) <name> <volume scale from 0. - 1.>: set all processes with the process name to the supplied volume scale
* mute (m) <name> <1 / 0>: mutes / unmute all processes with the process name
* quit (q): terminate the program
