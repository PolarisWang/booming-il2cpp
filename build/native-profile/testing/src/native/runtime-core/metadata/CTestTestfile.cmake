# CMake generated Testfile for 
# Source directory: D:/agent/booming-il2cpp/testing/src/native/runtime-core/metadata
# Build directory: D:/agent/booming-il2cpp/build/native-profile/testing/src/native/runtime-core/metadata
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
if(CTEST_CONFIGURATION_TYPE MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test([=[metadata_registry_test]=] "D:/agent/booming-il2cpp/build/native-profile/testing/src/native/runtime-core/metadata/Debug/metadata_registry_test.exe")
  set_tests_properties([=[metadata_registry_test]=] PROPERTIES  LABELS "unit;core" _BACKTRACE_TRIPLES "D:/agent/booming-il2cpp/testing/src/native/CMakeLists.txt;131;add_test;D:/agent/booming-il2cpp/testing/src/native/runtime-core/metadata/CMakeLists.txt;2;add_chaos_test;D:/agent/booming-il2cpp/testing/src/native/runtime-core/metadata/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test([=[metadata_registry_test]=] "D:/agent/booming-il2cpp/build/native-profile/testing/src/native/runtime-core/metadata/Release/metadata_registry_test.exe")
  set_tests_properties([=[metadata_registry_test]=] PROPERTIES  LABELS "unit;core" _BACKTRACE_TRIPLES "D:/agent/booming-il2cpp/testing/src/native/CMakeLists.txt;131;add_test;D:/agent/booming-il2cpp/testing/src/native/runtime-core/metadata/CMakeLists.txt;2;add_chaos_test;D:/agent/booming-il2cpp/testing/src/native/runtime-core/metadata/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  add_test([=[metadata_registry_test]=] "D:/agent/booming-il2cpp/build/native-profile/testing/src/native/runtime-core/metadata/MinSizeRel/metadata_registry_test.exe")
  set_tests_properties([=[metadata_registry_test]=] PROPERTIES  LABELS "unit;core" _BACKTRACE_TRIPLES "D:/agent/booming-il2cpp/testing/src/native/CMakeLists.txt;131;add_test;D:/agent/booming-il2cpp/testing/src/native/runtime-core/metadata/CMakeLists.txt;2;add_chaos_test;D:/agent/booming-il2cpp/testing/src/native/runtime-core/metadata/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test([=[metadata_registry_test]=] "D:/agent/booming-il2cpp/build/native-profile/testing/src/native/runtime-core/metadata/RelWithDebInfo/metadata_registry_test.exe")
  set_tests_properties([=[metadata_registry_test]=] PROPERTIES  LABELS "unit;core" _BACKTRACE_TRIPLES "D:/agent/booming-il2cpp/testing/src/native/CMakeLists.txt;131;add_test;D:/agent/booming-il2cpp/testing/src/native/runtime-core/metadata/CMakeLists.txt;2;add_chaos_test;D:/agent/booming-il2cpp/testing/src/native/runtime-core/metadata/CMakeLists.txt;0;")
else()
  add_test([=[metadata_registry_test]=] NOT_AVAILABLE)
endif()
