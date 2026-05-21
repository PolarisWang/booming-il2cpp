# CMake generated Testfile for 
# Source directory: D:/agent/booming-il2cpp/testing/src/native/abi
# Build directory: D:/agent/booming-il2cpp/build/testing/abi
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
if(CTEST_CONFIGURATION_TYPE MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test([=[test_abi_method_table]=] "D:/agent/booming-il2cpp/build/testing/abi/Debug/test_abi_method_table.exe")
  set_tests_properties([=[test_abi_method_table]=] PROPERTIES  _BACKTRACE_TRIPLES "D:/agent/booming-il2cpp/testing/src/native/CMakeLists.txt;127;add_test;D:/agent/booming-il2cpp/testing/src/native/abi/CMakeLists.txt;16;add_chaos_test;D:/agent/booming-il2cpp/testing/src/native/abi/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test([=[test_abi_method_table]=] "D:/agent/booming-il2cpp/build/testing/abi/Release/test_abi_method_table.exe")
  set_tests_properties([=[test_abi_method_table]=] PROPERTIES  _BACKTRACE_TRIPLES "D:/agent/booming-il2cpp/testing/src/native/CMakeLists.txt;127;add_test;D:/agent/booming-il2cpp/testing/src/native/abi/CMakeLists.txt;16;add_chaos_test;D:/agent/booming-il2cpp/testing/src/native/abi/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  add_test([=[test_abi_method_table]=] "D:/agent/booming-il2cpp/build/testing/abi/MinSizeRel/test_abi_method_table.exe")
  set_tests_properties([=[test_abi_method_table]=] PROPERTIES  _BACKTRACE_TRIPLES "D:/agent/booming-il2cpp/testing/src/native/CMakeLists.txt;127;add_test;D:/agent/booming-il2cpp/testing/src/native/abi/CMakeLists.txt;16;add_chaos_test;D:/agent/booming-il2cpp/testing/src/native/abi/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test([=[test_abi_method_table]=] "D:/agent/booming-il2cpp/build/testing/abi/RelWithDebInfo/test_abi_method_table.exe")
  set_tests_properties([=[test_abi_method_table]=] PROPERTIES  _BACKTRACE_TRIPLES "D:/agent/booming-il2cpp/testing/src/native/CMakeLists.txt;127;add_test;D:/agent/booming-il2cpp/testing/src/native/abi/CMakeLists.txt;16;add_chaos_test;D:/agent/booming-il2cpp/testing/src/native/abi/CMakeLists.txt;0;")
else()
  add_test([=[test_abi_method_table]=] NOT_AVAILABLE)
endif()
