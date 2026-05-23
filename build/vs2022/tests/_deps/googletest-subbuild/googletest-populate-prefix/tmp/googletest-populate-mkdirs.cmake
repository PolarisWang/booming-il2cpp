# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "D:/agent/booming-il2cpp/build/vs2022/tests/_deps/googletest-src"
  "D:/agent/booming-il2cpp/build/vs2022/tests/_deps/googletest-build"
  "D:/agent/booming-il2cpp/build/vs2022/tests/_deps/googletest-subbuild/googletest-populate-prefix"
  "D:/agent/booming-il2cpp/build/vs2022/tests/_deps/googletest-subbuild/googletest-populate-prefix/tmp"
  "D:/agent/booming-il2cpp/build/vs2022/tests/_deps/googletest-subbuild/googletest-populate-prefix/src/googletest-populate-stamp"
  "D:/agent/booming-il2cpp/build/vs2022/tests/_deps/googletest-subbuild/googletest-populate-prefix/src"
  "D:/agent/booming-il2cpp/build/vs2022/tests/_deps/googletest-subbuild/googletest-populate-prefix/src/googletest-populate-stamp"
)

set(configSubDirs Debug)
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/agent/booming-il2cpp/build/vs2022/tests/_deps/googletest-subbuild/googletest-populate-prefix/src/googletest-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "D:/agent/booming-il2cpp/build/vs2022/tests/_deps/googletest-subbuild/googletest-populate-prefix/src/googletest-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
