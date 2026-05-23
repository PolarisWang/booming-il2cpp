# CMake generated Testfile for 
# Source directory: D:/agent/booming-il2cpp/testing/src/native/support
# Build directory: D:/agent/booming-il2cpp/build/vs2022/tests/src/native/support
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
if(CTEST_CONFIGURATION_TYPE MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test([=[test_log_system]=] "D:/agent/booming-il2cpp/build/vs2022/tests/src/native/support/Debug/test_log_system.exe")
  set_tests_properties([=[test_log_system]=] PROPERTIES  LABELS "unit;support" _BACKTRACE_TRIPLES "D:/agent/booming-il2cpp/testing/src/native/CMakeLists.txt;131;add_test;D:/agent/booming-il2cpp/testing/src/native/support/CMakeLists.txt;4;add_chaos_test;D:/agent/booming-il2cpp/testing/src/native/support/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test([=[test_log_system]=] "D:/agent/booming-il2cpp/build/vs2022/tests/src/native/support/Release/test_log_system.exe")
  set_tests_properties([=[test_log_system]=] PROPERTIES  LABELS "unit;support" _BACKTRACE_TRIPLES "D:/agent/booming-il2cpp/testing/src/native/CMakeLists.txt;131;add_test;D:/agent/booming-il2cpp/testing/src/native/support/CMakeLists.txt;4;add_chaos_test;D:/agent/booming-il2cpp/testing/src/native/support/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  add_test([=[test_log_system]=] "D:/agent/booming-il2cpp/build/vs2022/tests/src/native/support/MinSizeRel/test_log_system.exe")
  set_tests_properties([=[test_log_system]=] PROPERTIES  LABELS "unit;support" _BACKTRACE_TRIPLES "D:/agent/booming-il2cpp/testing/src/native/CMakeLists.txt;131;add_test;D:/agent/booming-il2cpp/testing/src/native/support/CMakeLists.txt;4;add_chaos_test;D:/agent/booming-il2cpp/testing/src/native/support/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test([=[test_log_system]=] "D:/agent/booming-il2cpp/build/vs2022/tests/src/native/support/RelWithDebInfo/test_log_system.exe")
  set_tests_properties([=[test_log_system]=] PROPERTIES  LABELS "unit;support" _BACKTRACE_TRIPLES "D:/agent/booming-il2cpp/testing/src/native/CMakeLists.txt;131;add_test;D:/agent/booming-il2cpp/testing/src/native/support/CMakeLists.txt;4;add_chaos_test;D:/agent/booming-il2cpp/testing/src/native/support/CMakeLists.txt;0;")
else()
  add_test([=[test_log_system]=] NOT_AVAILABLE)
endif()
