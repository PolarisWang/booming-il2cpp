# CMake generated Testfile for 
# Source directory: /home/debian/agent/booming-il2cpp/testing/src/native/bootstrap
# Build directory: /home/debian/agent/booming-il2cpp/testing/foundation-dll/src/native/bootstrap
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test([=[test_bootstrap_smoke]=] "/home/debian/agent/booming-il2cpp/testing/foundation-dll/src/native/bootstrap/test_bootstrap_smoke")
set_tests_properties([=[test_bootstrap_smoke]=] PROPERTIES  LABELS "unit;bootstrap" _BACKTRACE_TRIPLES "/home/debian/agent/booming-il2cpp/testing/src/native/CMakeLists.txt;155;add_test;/home/debian/agent/booming-il2cpp/testing/src/native/bootstrap/CMakeLists.txt;21;add_chaos_test;/home/debian/agent/booming-il2cpp/testing/src/native/bootstrap/CMakeLists.txt;0;")
add_test([=[test_bootstrap_stress]=] "/home/debian/agent/booming-il2cpp/testing/foundation-dll/src/native/bootstrap/test_bootstrap_stress")
set_tests_properties([=[test_bootstrap_stress]=] PROPERTIES  LABELS "stress;bootstrap" _BACKTRACE_TRIPLES "/home/debian/agent/booming-il2cpp/testing/src/native/CMakeLists.txt;155;add_test;/home/debian/agent/booming-il2cpp/testing/src/native/bootstrap/CMakeLists.txt;29;add_chaos_test;/home/debian/agent/booming-il2cpp/testing/src/native/bootstrap/CMakeLists.txt;0;")
add_test([=[test_bootstrap_register]=] "/home/debian/agent/booming-il2cpp/testing/foundation-dll/src/native/bootstrap/test_bootstrap_register")
set_tests_properties([=[test_bootstrap_register]=] PROPERTIES  LABELS "unit;bootstrap" TIMEOUT "30" _BACKTRACE_TRIPLES "/home/debian/agent/booming-il2cpp/testing/src/native/CMakeLists.txt;155;add_test;/home/debian/agent/booming-il2cpp/testing/src/native/bootstrap/CMakeLists.txt;36;add_chaos_test;/home/debian/agent/booming-il2cpp/testing/src/native/bootstrap/CMakeLists.txt;0;")
add_test([=[test_bootstrap_bootstrap]=] "/home/debian/agent/booming-il2cpp/testing/foundation-dll/src/native/bootstrap/test_bootstrap_bootstrap")
set_tests_properties([=[test_bootstrap_bootstrap]=] PROPERTIES  LABELS "unit;bootstrap" TIMEOUT "30" _BACKTRACE_TRIPLES "/home/debian/agent/booming-il2cpp/testing/src/native/CMakeLists.txt;155;add_test;/home/debian/agent/booming-il2cpp/testing/src/native/bootstrap/CMakeLists.txt;45;add_chaos_test;/home/debian/agent/booming-il2cpp/testing/src/native/bootstrap/CMakeLists.txt;0;")
add_test([=[test_bootstrap_delegate]=] "/home/debian/agent/booming-il2cpp/testing/foundation-dll/src/native/bootstrap/test_bootstrap_delegate")
set_tests_properties([=[test_bootstrap_delegate]=] PROPERTIES  LABELS "unit;bootstrap" TIMEOUT "30" _BACKTRACE_TRIPLES "/home/debian/agent/booming-il2cpp/testing/src/native/CMakeLists.txt;155;add_test;/home/debian/agent/booming-il2cpp/testing/src/native/bootstrap/CMakeLists.txt;54;add_chaos_test;/home/debian/agent/booming-il2cpp/testing/src/native/bootstrap/CMakeLists.txt;0;")
add_test([=[test_bootstrap_resolve]=] "/home/debian/agent/booming-il2cpp/testing/foundation-dll/src/native/bootstrap/test_bootstrap_resolve")
set_tests_properties([=[test_bootstrap_resolve]=] PROPERTIES  LABELS "unit;bootstrap" TIMEOUT "30" _BACKTRACE_TRIPLES "/home/debian/agent/booming-il2cpp/testing/src/native/CMakeLists.txt;155;add_test;/home/debian/agent/booming-il2cpp/testing/src/native/bootstrap/CMakeLists.txt;63;add_chaos_test;/home/debian/agent/booming-il2cpp/testing/src/native/bootstrap/CMakeLists.txt;0;")
add_test([=[test_bootstrap_icall]=] "/home/debian/agent/booming-il2cpp/testing/foundation-dll/src/native/bootstrap/test_bootstrap_icall")
set_tests_properties([=[test_bootstrap_icall]=] PROPERTIES  LABELS "unit;bootstrap" TIMEOUT "30" _BACKTRACE_TRIPLES "/home/debian/agent/booming-il2cpp/testing/src/native/CMakeLists.txt;155;add_test;/home/debian/agent/booming-il2cpp/testing/src/native/bootstrap/CMakeLists.txt;72;add_chaos_test;/home/debian/agent/booming-il2cpp/testing/src/native/bootstrap/CMakeLists.txt;0;")
