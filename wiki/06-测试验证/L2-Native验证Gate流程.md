# L2 Native 验证 Gate 流程

## 概述

L2 (Semantic Correctness) 验证是 foundation DLL verification pipeline 中的 native gate。它将 IL2CPP 生成的 C++ AOT 代码编译为 native exe，通过 return value checksum + assert intrinsic 双机制验证语义正确性。

## 架构

```
managed entrypoint DLL  →  fact_l2_verifier.py  →  expected_checksums.h
                                                     ↓
native_verify_main.cpp  +  native-aot.generated.cpp  →  verify_<family>.exe
                                                     ↓
                                          自验证: checksum match + assert=0
```

### 双机制验证

1. **Return value checksum** — managed L2Harness 通过反射运行每个 entry，记录返回值作为预期 checksum。Native exe 的每个 `RunNativeAot(i)` 结果与之比较。
2. **Assert intrinsic** — CodeGen 将 `Assert.Equal(T, T)` 内联为 C++ 比较 + `__chaos_assert_failures` 计数器。Native exe 退出时报告 assert 失败次数。

### 异常处理

L2 模式下运行时未初始化。抛异常的 method（如 `Convert.ToChar(bool)`）会调用 `RaiseManagedException`。通过 `SetExceptionFallback()` 注册 setjmp/longjmp 回调，在 `RaiseManagedException` 检测到 runtime == null 时调用回调回到 verifier 主循环。

## 已知问题

### Tag bit 冲突 (chaos_managed_pointer_local_slot_tag)

`chaos_normalize_native_int_argument` 使用 bit 0 作为 managed pointer tag。当 method 返回奇数 checksum（如 65 = 'A'）时，bit 0 被误判为 tagged pointer，导致解引用无效地址（segfault）。

**当前修复**：`fact_l2_verifier.py` 在编译前拷贝 generated .cpp 并将 `chaos_managed_pointer_local_slot_tag` 从 `1` 改为 `0`。L2 模式下不存在实际 tagged pointer，因此安全。

**长期修复**：codegen 应通过 compile-time macro guard 控制 tag 值，或为 verify mode 单独 emit 变体。

## 手动运行

```bash
# 直接使用手动 rebuild 脚本
cd verification/.../<family>/native_test/l2-verify/build
_rebuild_verify.bat
verify_<family>.exe

# 使用 fact_l2_verifier.py
python build/toolchains/run/testing/foundation_dll/fact_l2_verifier.py <family>
python build/toolchains/run/testing/foundation_dll/fact_l2_verifier.py <family> --verbose
```

## 流水线集成

`batch_native_aot_runner.py` 的 Step 3 自动调用 `fact_l2_verifier.py`。验证结果写入 development-tracking.json。

## 验证标准

- L1: 生成代码包含 `chaos_eval_stack` lowering 模式（非 SimpleForward）
- L2: `verify_<family>.exe` 退出码 = 0
- L2 输出: `L2: N/M passed (assert_failures=0, return_failures=0)`
