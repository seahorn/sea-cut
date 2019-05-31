cmake_minimum_required(VERSION 3.5)

project(SeaCut)

message(STATUS "LLVM_DIR is ${LLVM_DIR}")
find_package(LLVM 8.0 REQUIRED CONFIG EXACT PATHS ${LLVM_DIR})

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
        
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(AddLLVM)
include(HandleLLVMOptions)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${LLVM_CXXFLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LLVM_LDFLAGS}")

include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})
add_definitions(${LLVM_DEFINITIONS})

file(TO_NATIVE_PATH "${LLVM_TOOLS_BINARY_DIR}/llvm-config${CMAKE_EXECUTABLE_SUFFIX}" LLVM_CONFIG)

execute_process(COMMAND ${LLVM_CONFIG} --system-libs --link-static  OUTPUT_VARIABLE LLVM_SYSTEM_LIBS_RAW)
string(STRIP "${LLVM_SYSTEM_LIBS_RAW}" LLVM_SYSTEM_LIBS_RAW)  # strip whitespace from start & end
string(REPLACE " " ";" LLVM_SYSTEM_LIBS "${LLVM_SYSTEM_LIBS_RAW}")  # convert into a list
if("${LLVM_SYSTEM_LIBS}" STREQUAL "")
  message(WARNING "'llvm-config --system-libs --link-static' is empty; this is possibly wrong.")
endif()

execute_process(COMMAND ${LLVM_CONFIG} --libs  OUTPUT_VARIABLE LLVM_LIBS_RAW)
string(STRIP "${LLVM_LIBS_RAW}" LLVM_LIBS_RAW)  # strip whitespace from start & end
string(REPLACE " " ";" LLVM_LIBS "${LLVM_LIBS_RAW}")  # convert into a list
if("${LLVM_LIBS}" STREQUAL "")
  message(WARNING "'llvm-config --libs' is empty; this is possibly wrong.")
endif()

SET(CLANG_LIBS
	clangAnalysis
	clangAST
	clangBasic
	clangDriver
	clangEdit
	clangFrontend
	clangLex
	clangParse
	clangRewrite
	clangSema
	clangSerialization
	clangTooling
	clangASTMatchers
)

add_compile_options(-fno-rtti)
add_compile_options(-fno-exceptions)
#add_compile_options(-march=native)

if (SEACUT_USE_LINKER)
  set(OLD_CMAKE_REQUIRED_FLAGS ${CMAKE_REQUIRED_FLAGS})
  set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -fuse-ld=${SEACUT_USE_LINKER}")
  check_cxx_source_compiles("int main() { return 0; }" CXX_SUPPORTS_CUSTOM_LINKER)
  if (NOT CXX_SUPPORTS_CUSTOM_LINKER)
	  message(FATAL_ERROR "Host compiler does not support '-fuse-ld=${SEACUT_USE_LINKER}'")
  endif()
  set(CMAKE_REQUIRED_FLAGS ${OLD_CMAKE_REQUIRED_FLAGS})
  append("-fuse-ld=${SEACUT_USE_LINKER}"
    CMAKE_EXE_LINKER_FLAGS CMAKE_MODULE_LINKER_FLAGS CMAKE_SHARED_LINKER_FLAGS)
  message(STATUS "Using linker: ${SEACUT_USE_LINKER}")
endif()


# set(LLVM_LINK_COMPONENTS
# 	Support
# )

set(SEACUT_SOURCES

)

add_executable(sea-cut sea-cut.cpp
  ${SEACUT_SOURCES}
)

#llvm_config(sea-cut ${LLVM_LINK_COMPONENTS})
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

target_link_libraries(sea-cut
	${LLVM_SYSTEM_LIBS}
        ${LLVM_LIBS}
	${CLANG_LIBS}
    )


# add_executable(test-smt-jit doctest_main_smt_jit.cpp
#  ${SEACUT_SOURCES}
#  parser_tests.cpp
# )
# llvm_config(test-smt-jit ${LLVM_LINK_COMPONENTS})
# target_link_libraries(test-smt-jit LINK_PUBLIC bvlib sexpresso)

# enable_testing()
# add_test(NAME test-smt-jit COMMAND test-smt-jit DEPENDS test-smt-jit)
