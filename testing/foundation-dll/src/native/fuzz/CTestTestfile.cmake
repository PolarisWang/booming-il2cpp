# CMake generated Testfile for 
# Source directory: /home/debian/agent/booming-il2cpp/testing/src/native/fuzz
# Build directory: /home/debian/agent/booming-il2cpp/testing/foundation-dll/src/native/fuzz
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test([=[test_fuzz_il]=] "/home/debian/agent/booming-il2cpp/testing/foundation-dll/src/native/fuzz/test_fuzz_il")
set_tests_properties([=[test_fuzz_il]=] PROPERTIES  LABELS "fuzz;interpreter" TIMEOUT "120" _BACKTRACE_TRIPLES "/home/debian/agent/booming-il2cpp/testing/src/native/CMakeLists.txt;155;add_test;/home/debian/agent/booming-il2cpp/testing/src/native/fuzz/CMakeLists.txt;30;add_chaos_test;/home/debian/agent/booming-il2cpp/testing/src/native/fuzz/CMakeLists.txt;0;")
add_test([=[test_fuzz_gc]=] "/home/debian/agent/booming-il2cpp/testing/foundation-dll/src/native/fuzz/test_fuzz_gc")
set_tests_properties([=[test_fuzz_gc]=] PROPERTIES  LABELS "fuzz;gc" TIMEOUT "120" _BACKTRACE_TRIPLES "/home/debian/agent/booming-il2cpp/testing/src/native/CMakeLists.txt;155;add_test;/home/debian/agent/booming-il2cpp/testing/src/native/fuzz/CMakeLists.txt;37;add_chaos_test;/home/debian/agent/booming-il2cpp/testing/src/native/fuzz/CMakeLists.txt;0;")
