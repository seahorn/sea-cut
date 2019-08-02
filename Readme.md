# Environment Setup

## Remote Linux Server


Project Root/
|
|-- llvm-project
|-- llvm_config.sh

1. Clone llvm project to the remote server.
    $ `git clone https://github.com/llvm/llvm-project.git -b release/9.x`

2. git clone https://github.com/MichelleXiaoxiaoWang/SeaSTL.git
3. run llvm_config.sh
    $ `SeaSTL/SeaCut/llvm_config.sh`
4. Install the project
    $ `cd build-llvm-debug`
    $ `ninja install`


## Local Machine

1. Download and install CLion ([Link](https://www.jetbrains.com/clion/)).
2. CLone the project to your local
    $ `git clone https://github.com/MichelleXiaoxiaoWang/SeaSTL.git`

3. Use CLion to open the project `/SeaSTL/SeaCut`
4. In CLion -> Preferences -> Build, Execution, Deployment -> Toolchains, add the configuration:

    a. Add your remote credentials. (This should be the same as remote Linux credentials).
    b. CMake: `/usr/bin/cmake`
    c. Make: `/usr/bin/make`
    d. C Compiler: `/usr/bin/clang-6.0`
    e. C++ Compler: `/usr/bin/clang++-6.0`
    f. Debugger: `/usr/bin/gdb`
5. In CLion -> Preferences -> Build, Excution, Deployment -> CMake, add the configuration: 
a. CMake Option: 
    ```-DCMAKE_BUILD_TYPE=Debug -DSEACUT_USE_LINKER=lld-6.0 -DLLVM_DIR={$PROJECT_DIRETORY}/build-llvm-debug/run -DCMAKE_C_COMPILER=clang-6.0 -DCMAKE_CXX_COMPILER=clang++-6.0```
where `{$PROJECT_DIRETORY}` is your project diretory in remote server.
b. Generatio Path: `cmake-build-debug`
c. Build Options: `-j 16`

# Runing the Project in Remote

1. In the SeatCut directory run
    $ `./config.sh ~/build-llvm-debug/run`

2. $ `cd build/` and $ `ninja`


## Runing Test Cases
$ `ninja test-sea-cut`