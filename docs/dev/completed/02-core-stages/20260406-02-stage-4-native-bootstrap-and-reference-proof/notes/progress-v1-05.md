# 杩涘害璁板綍 v1-05

## 鏃堕棿

- 2026-04-06 10:27:18 +08:00

## 宸插畬鎴?
- 浠?TDD 鏂瑰紡鏂板 `tests/unit/run/test_stage4_runtime_core_skeleton.py`锛屽厛閿佸畾 `src/native/runtime-core/` 鐩綍銆乣RuntimeAbiV0` 瀵煎嚭琛ㄩ鏋朵笌 `windows-x64-reference` root CMake 璺敱鎺ョ嚎銆?- 瑙傚療鍒?RED 鍚庯紝鏂板 `src/native/runtime-core/CMakeLists.txt`銆乣runtime_core.h`銆乣runtime_core.cpp`锛岃惤鍦?Stage 4 Chunk 3 鐨勭涓€鐗堟渶灏?ABI 楠ㄦ灦銆?- 褰撳墠 `runtime-core` 宸叉彁渚?`chaos_runtime_get_abi_v0()` 瀵煎嚭琛紝浠ュ強 first-proof 蹇呴渶鍏ュ彛鐨勬渶灏忓疄鐜版垨鏄惧紡 stub锛屽寘鎷細
  - `runtime_init` / `runtime_shutdown`
  - `thread_attach` / `thread_detach`
  - `object_new`
  - `string_new_utf8`
  - `class_init`
  - `field_get_value` / `field_set_value`
  - `assembly_get_image` / `image_find_type` / `type_find_method` / `type_find_field`
  - `raise_managed_exception`
- 灏?`src/native/runtime-core` 鎺ュ叆 `CMakeLists.txt` 鐨?`windows-x64-reference` 璺敱锛屽悓鏃朵繚鐣欐棦鏈?`tests/contracts/native/abi` compile-only anchor銆?- 淇 `tests/unit/run/test_stage1b_layout.py` 涓笌褰撳墠椤圭洰寮曠敤鍏崇郴涓嶄竴鑷寸殑鏈熸湜鍊硷紝浣垮叾瑕嗙洊 `Chaos.IL2CPP.Driver -> Chaos.IL2CPP.CodeGen` 涓?`Chaos.IL2CPP.CodeGen -> Scriban` 鐨勭幇鐘躲€?
## 楠岃瘉

- `python -m unittest tests.unit.run.test_stage4_runtime_core_skeleton`
- `python -m unittest tests.unit.run.test_stage3_managed_minimal_closure`
- `python -m unittest tests.unit.run.test_stage4_native_reference_proof`
- `python -m unittest tests.unit.run.test_stage1b_layout`

## 褰撳墠鐘舵€?
- 浠诲姟鐘舵€侊細`in_progress`
- 褰撳墠闃舵锛歚implementation`
- 璇存槑锛歚runtime-core` 鐩墠浠嶆槸鈥滄湭缁忚繃鐪熷疄 native 缂栬瘧楠岃瘉鐨勬渶灏忛鏋垛€濄€傚綋鍓嶇幆澧冨悓鏃剁己灏?`cmake` 涓庡彲鐢ㄧ殑鐙珛 C++ 缂栬瘧鍣紝鍥犳杩樻棤娉曞畬鎴?Chunk 3 鍘熻鍒掗噷鐨?compile-level smoke銆?
## 涓嬩竴姝?
- 褰?`cmake` 鎴栧彲鐢?C++ 缂栬瘧鍣ㄦ仮澶嶅悗锛屼紭鍏堝 `src/native/runtime-core` 鎵ц鐪熷疄缂栬瘧楠岃瘉锛岀‘璁?`windows-x64-reference` 浠嶈兘鍚屾椂鎵胯浇 contract smoke 涓庢柊澧?`chaos_runtime_core`銆?- 濡傜户缁湪褰撳墠鐜涓嬬洸鍐欙紝鍙繘鍏?Chunk 4锛屼负 `src/native/bootstrap/` 涓?`src/native/support/` 钀?proof host 楠ㄦ灦锛屼絾閭ｄ竴杞粛浼氬浜庘€滄湭缂栬瘧楠岃瘉鈥濈姸鎬併€?
