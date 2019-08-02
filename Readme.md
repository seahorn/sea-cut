# Environment Setup

## Remote Linux Server


Project Root/
</br>
|
</br>
|-- llvm-project
</br>
|-- [llvm_config.sh](github.com)

1. Clone llvm prjoject to the remote server.
</br>
$ ` git clone https://github.com/llvm/llvm-project.git -b release/9.x`
</br>
2. git clone https://github.com/MichelleXiaoxiaoWang/SeaSTL.git
</br>
3. run llvm_config.sh
</br>
$ `SeaSTL/SeaCut/llvm_config.sh`
</br>
4. Install the project
</br>
$ `cd build-llvm-debug`
</br>
$ `ninja install`
</br>

## Local Machine

1. Download and install CLion ([Link](https://www.jetbrains.com/clion/)).
2. CLone the project to your local
</br>
`$ git clone https://github.com/MichelleXiaoxiaoWang/SeaSTL.git`
</br>
3. Use CLion to open the project `/SeaSTL/SeaCut`
4. In CLion -> Preferences -> Build, Excution, Deployment -> Toolchains, add the configuration:
</br>
a. Add your remote credentials. (This should be the same as remote Linux credentials).
</br>∏
b. CMake: `/usr/bin/cmake`</br>
c. Make: `/usr/bin/make`</br>
d. C Compiler: `/usr/bin/clang-6.0`</br>
e. C++ Compler: `/usr/bin/clang++-6.0`</br>
f. Debugger: `/usr/bin/gdb`
5. In CLion -> Preferences -> Build, Excution, Deployment -> CMake, add the configuration: </br>
a. CMake Option: ```-DCMAKE_BUILD_TYPE=Debug -DSEACUT_USE_LINKER=lld-6.0 -DLLVM_DIR={$PROJECT_DIRETORY}/build-llvm-debug/run -DCMAKE_C_COMPILER=clang-6.0 -DCMAKE_CXX_COMPILER=clang++-6.0```</br>
where `{$PROJECT_DIRETORY}` is your project diretory in remote server.</br>
b. Generatio Path: `cmake-build-debug`</br>
c. Build Options: `-j 16`

# Runing the Project in Remote

1. In the SeatCut directory run
</br>
$ `./config.sh ~/build-llvm-debug/run`

2. $ `cd build/` and $ `ninja`


## Runing Test Cases
$ `ninja test-sea-cut`