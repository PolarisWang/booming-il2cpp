cmake_minimum_required(VERSION 3.20)
project(chaos_native_libs CXX)
set(CMAKE_CXX_STANDARD 20)

# Config tier: controlled by pipeline --native-config
set(CHAOS_IL2CPP_CONFIG_TIER "check" CACHE STRING "Build config tier")
if(CHAOS_IL2CPP_CONFIG_TIER STREQUAL "profile")
  add_compile_definitions(CHAOS_IL2CPP_CONFIG_TIER=CHAOS_IL2CPP_CONFIG_TIER_PROFILE)
  add_compile_definitions(CHAOS_IL2CPP_CONFIG_PROFILE)
  add_compile_definitions(CHAOS_IL2CPP_LOG_LEVEL=2)
elseif(CHAOS_IL2CPP_CONFIG_TIER STREQUAL "ship")
  add_compile_definitions(CHAOS_IL2CPP_CONFIG_TIER=CHAOS_IL2CPP_CONFIG_TIER_SHIP)
  add_compile_definitions(CHAOS_IL2CPP_CONFIG_SHIP)
  add_compile_definitions(CHAOS_IL2CPP_LOG_LEVEL=0)
else()
  add_compile_definitions(CHAOS_IL2CPP_CONFIG_TIER=CHAOS_IL2CPP_CONFIG_TIER_CHECK)
  add_compile_definitions(CHAOS_IL2CPP_CONFIG_CHECK)
  add_compile_definitions(CHAOS_IL2CPP_LOG_LEVEL=3)
endif()

# Tier manager: the real implementation (tier_manager.cpp) is compiled
# alongside registration_globals.cpp; suppress the stub with this define.
add_compile_definitions(CHAOS_IL2CPP_HAS_TIER_MANAGER=1)

# Native libraries in dependency order:
#   fmt → unordered_dense (header-only) → pal → common → interpreter
#   → runtime-core → support → hot-update → jit → bootstrap
add_subdirectory("/home/debian/agent/booming-il2cpp/third_party/fmt" "/home/debian/agent/booming-il2cpp/testing/foundation-dll/System.Security.Cryptography/chunks/security-cryptography/native/codegen/codegen/lib/../.native_build/output/fmt")
add_subdirectory("/home/debian/agent/booming-il2cpp/third_party/unordered_dense" "/home/debian/agent/booming-il2cpp/testing/foundation-dll/System.Security.Cryptography/chunks/security-cryptography/native/codegen/codegen/lib/../.native_build/output/unordered_dense")
add_subdirectory("/home/debian/agent/booming-il2cpp/src/native/pal" "/home/debian/agent/booming-il2cpp/testing/foundation-dll/System.Security.Cryptography/chunks/security-cryptography/native/codegen/codegen/lib/../.native_build/output/pal")
add_subdirectory("/home/debian/agent/booming-il2cpp/src/native/common" "/home/debian/agent/booming-il2cpp/testing/foundation-dll/System.Security.Cryptography/chunks/security-cryptography/native/codegen/codegen/lib/../.native_build/output/common")
add_subdirectory("/home/debian/agent/booming-il2cpp/src/native/interpreter" "/home/debian/agent/booming-il2cpp/testing/foundation-dll/System.Security.Cryptography/chunks/security-cryptography/native/codegen/codegen/lib/../.native_build/output/interpreter")
add_subdirectory("/home/debian/agent/booming-il2cpp/src/native/runtime-core" "/home/debian/agent/booming-il2cpp/testing/foundation-dll/System.Security.Cryptography/chunks/security-cryptography/native/codegen/codegen/lib/../.native_build/output/runtime_core")
add_subdirectory("/home/debian/agent/booming-il2cpp/src/native/support" "/home/debian/agent/booming-il2cpp/testing/foundation-dll/System.Security.Cryptography/chunks/security-cryptography/native/codegen/codegen/lib/../.native_build/output/support")
add_subdirectory("/home/debian/agent/booming-il2cpp/src/native/hot-update" "/home/debian/agent/booming-il2cpp/testing/foundation-dll/System.Security.Cryptography/chunks/security-cryptography/native/codegen/codegen/lib/../.native_build/output/hot_update")
add_subdirectory("/home/debian/agent/booming-il2cpp/src/native/jit" "/home/debian/agent/booming-il2cpp/testing/foundation-dll/System.Security.Cryptography/chunks/security-cryptography/native/codegen/codegen/lib/../.native_build/output/jit")
add_subdirectory("/home/debian/agent/booming-il2cpp/src/native/bootstrap" "/home/debian/agent/booming-il2cpp/testing/foundation-dll/System.Security.Cryptography/chunks/security-cryptography/native/codegen/codegen/lib/../.native_build/output/bootstrap")