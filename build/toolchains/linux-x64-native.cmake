set(ROADMAP0_TARGET_NAME "linux-x64-native" CACHE STRING "Roadmap 0 target id")
if(ROADMAP0_TOOLCHAIN_VALIDATE_ONLY)
  set(ROADMAP0_TOOLCHAIN_VALIDATE_ONLY ON CACHE BOOL "Validate preset routing without real toolchain")
  return()
endif()

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR x86_64)
# The CI workflow (gc-linux-x64-ci.yml) passes -DCMAKE_C_COMPILER=gcc-14 /
# -DCMAKE_CXX_COMPILER=g++-14 explicitly; keep the defaults aligned here so
# local `cmake --preset linux-x64-native` produces the same toolchain.
if(NOT CMAKE_C_COMPILER)
  set(CMAKE_C_COMPILER gcc-14)
endif()
if(NOT CMAKE_CXX_COMPILER)
  set(CMAKE_CXX_COMPILER g++-14)
endif()
set(ROADMAP0_PLATFORM_GATE "linux-x64-native" CACHE STRING "Roadmap 0 gate id")
