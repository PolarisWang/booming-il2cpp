# CMake generated Testfile for 
# Source directory: /home/debian/agent/booming-il2cpp/testing/src/native/engine-bridge
# Build directory: /home/debian/agent/booming-il2cpp/testing/foundation-dll/src/native/engine-bridge
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test([=[test_engine_bridge_smoke]=] "/home/debian/agent/booming-il2cpp/testing/foundation-dll/src/native/engine-bridge/test_engine_bridge_smoke")
set_tests_properties([=[test_engine_bridge_smoke]=] PROPERTIES  LABELS "unit;engine-bridge" _BACKTRACE_TRIPLES "/home/debian/agent/booming-il2cpp/testing/src/native/CMakeLists.txt;155;add_test;/home/debian/agent/booming-il2cpp/testing/src/native/engine-bridge/CMakeLists.txt;15;add_chaos_test;/home/debian/agent/booming-il2cpp/testing/src/native/engine-bridge/CMakeLists.txt;0;")
add_test([=[test_engine_bridge_stress]=] "/home/debian/agent/booming-il2cpp/testing/foundation-dll/src/native/engine-bridge/test_engine_bridge_stress")
set_tests_properties([=[test_engine_bridge_stress]=] PROPERTIES  LABELS "stress;engine-bridge" _BACKTRACE_TRIPLES "/home/debian/agent/booming-il2cpp/testing/src/native/CMakeLists.txt;155;add_test;/home/debian/agent/booming-il2cpp/testing/src/native/engine-bridge/CMakeLists.txt;21;add_chaos_test;/home/debian/agent/booming-il2cpp/testing/src/native/engine-bridge/CMakeLists.txt;0;")
