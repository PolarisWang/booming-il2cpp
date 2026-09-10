---
name: assert-migration
description: Assert migration — codegen intrinsic + _exitCode→Assert + handwritten conversion
metadata:
  type: project
---
# Assert 迁移 — _exitCode 转为 Assert intrinsic 模式

## 当前阶段
- **phase**: phase4-handwritten-conversion + phase5-build-pipeline + phase6-test-assertions — **all complete**
- **phase4**: complete (28 handwritten files converted, zero _exitCode remaining)
- **phase5**: complete (Scriban template + ProjectModel + CppProjectEmitter — TPG builds 0 errors)
- **phase6**: complete (42+ test files modified, ~200+ assertions added)

## 边界拍板
- 所有三类代码全改为 Assert 模式：Subject_N() / handwritten/ / test/
- Assert.Throws<T>() 需要验证具体异常类型
- 250+ handwritten 文件逐个手工改
- `[Conditional]` 不适用（会连带剥离被测代码），改用 C++ `#ifdef CHAOS_VERIFICATION`

## 方案选择: codegen intrinsic 识别

Codegen 识别 `Chaos.TestFramework.Assert.*` 调用，内联生成 C++ 代码，用 `#ifdef CHAOS_VERIFICATION` 包裹。

| C# Assert | 生成的 C++ (#ifdef 块内) |
|-----------|------------------------|
| `AreEqual(42, result)` | `if (42 != result) { _exitCode = 1; throw AssertionException(); }` |
| `IsTrue(cond)` | `if (!cond) { _exitCode = 1; throw AssertionException(); }` |
| `IsFalse(cond)` | `if (cond) { _exitCode = 1; throw AssertionException(); }` |
| `IsNull(val)` | `if (val != nullptr) { ... }` |
| `IsNotNull(val)` | `if (val == nullptr) { ... }` |
| `Throws<T>(() => call)` | `bool __caught; try { call; } catch(T*) { __caught=true; } if(!__caught) { ... }` |

benchmark 时: `cmake -DCHAOS_VERIFICATION=OFF` → C++ preprocessor 完全移除 assert 代码，零开销。
codegen 只跑一次，同一 .cpp 文件编译两次。

## 验证结果

1. **Codegen inline shapes 全部正确** — 6 个基本 Assert 方法 + Throws<T> 全部使用 `if constexpr (CHAOS_IL2CPP_VERIFICATION_ENABLED)` 包裹
2. **Assert.Throws<T> lookahead 正确** — 通过 IL 前瞻识别 ldnull+ldftn+newobj Action 模式，直接展开为 try/catch
3. **无 Benchmark/HotUpdate 污染** — 所有 handwritten/test 文件仅用于 Fact 验证
4. **发现并修复 2 个编译错误**:
   - `Assert.AreEqual(TaskStatus, TaskStatus)` → 转型 `(int)`（无 enum/object 重载）
   - `Assert.AreEqual(Task<int>, Task<int>)` → `Assert.IsTrue(==)`（引用相等检查）
5. **26 个 stub 文件改用正确 Assert** — `Assert.IsTrue(true)` → `Assert.IsNotNull(result)` 或裸调用。零残留。

## 三优先级权衡结论
- P1 性能最优: `#ifdef` 模式 benchmark 时完全消除，零开销 ✓
- P2 架构完美: intrinsic 模式避免 delegate 翻译复杂度，产出的 C++ 干净 ✓
- P3 HotUpdate: 不涉及 hotupdate 路径 ✓

## 六阶段 Roadmap

### Phase 1 — Assert SDK 补充 ✅
- `Assert.Throws<T>(Action)` 泛型重载 added
- 文件: `src/reference/Chaos.TestFramework.Sdk/Assert.cs`

### Phase 2 — Codegen intrinsic emitter ✅
- 已注册 InlineShapeDescriptor 的 Assert 方法（`RuntimeHelperShapeRegistry.BuildDefault()`）:
  - `AreEqual` — all primitive overloads
  - `IsTrue` / `IsFalse` — boolean conditions
  - `IsNull` / `IsNotNull` — null checks
  - `Fail` — unconditional throw
- Throws<T> 通过 IL 前瞻分析实现（`EmitAssertThrowsInline` / `FindThrowsTargetMethod`）:
  - 识别 `ldnull + ldftn <target> + newobj Action::.ctor` 模式
  - 提取目标方法 native symbol，直接展开为 `try { target(); throw; } catch (chaos_managed_exception&) { }`
  - 回退路径: `ChaosInvokeAction(action)` delegate invoke
- 每个 inline 使用 `if constexpr (CHAOS_IL2CPP_VERIFICATION_ENABLED)` 包裹，benchmark 构建下零开销
- 异常类型: `throw chaos_managed_exception{}` — 被 dispatch 模板的 `catch(chaos_managed_exception&)` 捕获
- 文件: 
  - `src/managed/Chaos.IL2CPP.Generator/RuntimeSupport/RuntimeHelperShapeRegistry.cs`
  - `src/managed/Chaos.IL2CPP.Generator/Emission/NativeAotLoweringPlanner.ExceptionEmission.cs`

### Phase 3 — Python family_entrypoint.py 改造
- Subject_N() 从 `_exitCode` 改为 Assert 模式
- expected_exception → `Assert.Throws<T>`
- expected_value → `Assert.AreEqual`
- smoke → 裸调用或 `Assert.IsTrue(true)`
- 文件: `testing/foundation-dll/verification/orchestration/family_entrypoint.py`

### Phase 4 — handwritten/ 250+ 文件手工转换
- 逐个文件改 CustomEntrySubject_N()
- `catch { _exitCode = 1 }` → `Assert.Throws<T>`
- 按 family 分批

### Phase 5 — 构建管线适配
- CMakeLists.txt: `option(CHAOS_VERIFICATION "Enable Assert verification" ON)`
- fact 构建: `-DCHAOS_VERIFICATION=ON`
- benchmark 构建: `-DCHAOS_VERIFICATION=OFF`
- 文件: TPG scriban 模板 `TestProject.CMakeLists.txt.scriban`

### Phase 6 — test/*Tests.cs 补充 Assert
- 在已有 `[Fact]` 方法中添加 Assert.AreEqual/IsTrue 断言

## 关键文档
- 本文作为设计摘要权威输入
- 无独立 design-v1-01.md（方案已在 brainstorm 中确认）

## 下一步入口
用户确认 roadmap 后 → 进入 `dev-writing-plans` 拆解 Phase 1 实现
