# System.Private.CoreLib 剩余 14 个失败 Family 修复 Roadmap

## 1. 目标

将 System.Private.CoreLib 的 14 个未通过验证的 capability family 逐个修复到 **native-proof passed**（fact 全通过 + audit ALIGNED），使已验证通过数从 22 → 36。

## 2. 范围边界

- **范围**：System.Private.CoreLib 中目前未通过的 14 个 family（不含已修复的 enum-parsing、reflection-member-basics）
- **评估标准**：`verify_family()` pipeline 的 fact + audit 阶段通过
- **操作对象**：codegen emitter（C#）、测试代码（C#  subjects）、native stubs（C++）、pipeline 脚本（Python）

## 3. 非目标

- 不涵盖 benchmark/hotupdate/post-hotupdate-benchmark 阶段（这些是 advisory/strict 模式，不阻塞 native-proof gate）
- 不涵盖其他 assembly（System.Linq、System.Collections.Immutable 等）的修复
- 不做全量回归测试（只对发生变更的 family 重跑 pipeline）
- 不修改 IL 翻译路径的 emitter 核心逻辑（除非 convert-to-cpp crash 需要）

## 4. 全局优先级约束

| 优先级 | 内容 |
|--------|------|
| P1 性能最优 | entry.exe 构建和 fact 验证以能跑通为准，不做性能优化 |
| P2 方案完美性 | 修复方案要干净：不走新增 stub、不走 skip 绕过、不在 mechanism_audit 加特例 |
| P3 HotUpdate | 涉及 native 层修复时，考虑 hotupdate 兼容性 |

## 5. 阶段列表

### Phase 1: 低 hanging fruit（6 families）

**目标**：用最少工作量迅速通过 6 个接近通过的 family。

| # | Family | 故障模式 | 预估工作量 |
|---|--------|---------|-----------|
| 1.1 | `error-info-basic` | codegen 已完成但未跑 verify-family | ~5 min |
| 1.2 | `threading-monitor-interlocked` | subjects C# 编译错误（缺 using） | ~5 min |
| 1.3 | `string-construction` | 空壳，需完成 onboard | ~15 min |
| 1.4 | `thread-sync` | 空壳，需完成 onboard | ~15 min |
| 1.5 | `reflection-binding` | fact 11/12（1 个失败） | ~15 min |
| 1.6 | `reflection-type` | fact 42/43（1 个失败） | ~15 min |

### Phase 2: entry.exe 构建失败（5 families）

**目标**：诊断并修复 entry.exe cmake 构建问题。

| # | Family | 故障模式 | 预估工作量 |
|---|--------|---------|-----------|
| 2.1 | `buffer-memory` | entry.exe build failed | ~30 min |
| 2.2 | `span-memory-buffers` | entry.exe build failed | ~30 min |
| 2.3 | `string-char-text-core` | entry.exe build failed | ~30 min |
| 2.4 | `interface-dispatch` | entry.exe build failed | ~30 min |
| 2.5 | `reflection-member-complete` | entry.exe build failed + 21/22 fact | ~45 min |

### Phase 3: convert-to-cpp 崩溃（2 families）

**目标**：诊断并解决 IL2CPP 转换阶段的崩溃。

| # | Family | 故障模式 | 预估工作量 |
|---|--------|---------|-----------|
| 3.1 | `time-date-time-timespan` | convert-to-cpp crash | ~60 min |
| 3.2 | `type-runtime-handles` | convert-to-cpp crash + 12/13 fact | ~60 min |

### Phase 4: 清理收尾

| # | 工作 | 预估工作量 |
|---|------|-----------|
| 4.1 | 删除 orphan 目录（逗号名 family） | ~2 min |
| 4.2 | 刷新 dashboard | ~2 min |
| 4.3 | 刷新 ledger closureStatus | ~5 min |

## 6. 每阶段完成定义

### Phase 1: 低 hanging fruit

- `goal`: 6 个 family 全部通过 verify_family()
- `exit_criteria`: 每个 family 的 fact + audit 通过
- `deliverables`: 每个 family 的 unified-verification-report.json（overall_status=passed）
- `dependencies`: 无（可独立推进）

#### 1.1 error-info-basic
- `action`: 直接跑 `verify_family()` pipeline
- `risk`: codegen 已完成但 entry.exe 可能 build 失败 — 如失败则移到 Phase 2

#### 1.2 threading-monitor-interlocked
- `action`: 检查 subjects C# 文件，补上 `using System.Threading;` 等缺失的 import
- `risk`: 极低

#### 1.3 string-construction
- `action`: 按 `dev-foundation-dll-onboard-family` 流程：检查 ledger → Phase 1 AutoGenerate → Phase 2 手写 → codegen pipeline → verify-family
- `risk`: 可能遇到 codegen 不支持的 opcode

#### 1.4 thread-sync
- `action`: 同上 onboard 流程
- `risk`: 同上

#### 1.5 reflection-binding (11/12)
- `action`: 运行 entry.exe 获取 bitmask → 定位失败 subject → 检查 codegen 输出或加 custom entry
- `risk`: 可能需要 codegen emitter 层修复

#### 1.6 reflection-type (42/43)
- `action`: 同上流程
- `risk`: 类似 reflection-binding 风险

### Phase 2: entry.exe 构建失败

- `goal`: 5 个 family 的 entry.exe 成功构建
- `exit_criteria`: cmake build 通过 + entry.exe 可运行
- `deliverables`: native/entry.exe，fact 验证通过

#### 2.1-2.5 通用诊断流程
1. 手动执行 cmake build 获取完整 MSVC 错误输出
2. 分析错误类型：
   - LNK2019/LNK1120（未解析符号）→ 检查 runtime stubs 或 link 依赖
   - C++ 编译错误 → 检查 codegen 生成的 .cpp 文件
   - LNK2038 配置不匹配 → 检查 CMakeLists.txt 配置
3. 修复后重跑 pipeline 验证

- `dependencies`: chaos_runtime_core.lib 必须在 RelWithDebInfo 下可用
- `risk`: 多个 family 可能共享同一个底层问题（如 struct emission bug、patchdata 集成）

### Phase 3: convert-to-cpp 崩溃

- `goal`: 2 个 family 的 chaos-il2cpp convert-to-cpp 成功完成
- `exit_criteria`: native-aot.generated.cpp 生成 + entry.exe 构建 + fact 通过
- `deliverables`: 生成的 C++ 代码 + 通过的 verification report
- `dependencies`: Driver DLL 必须包含修复

#### 3.1 time-date-time-timespan
- `action`: 手工运行 il2cpp.exe conversion 捕获崩溃堆栈 → 分析是 codegen emitter 还是 IR lowering 的问题 → 修复后重跑

#### 3.2 type-runtime-handles
- `action`: 同上流程

- `risk`: 崩溃可能在 managed 层（Driver/C# codegen），需要 dotnet 调试技能

### Phase 4: 清理收尾

- `goal`: dashboard 正确反映最新状态
- `exit_criteria`:
  - orphan 目录已删除
  - claims 显示 36+ family 通过 native-proof
  - ledger 中对应 family 的 closureStatus 已更新
- `deliverables`: 刷新后的 family-verification-claims.json + capability-family-ledger.json

## 7. 子任务映射

| task_id | phase | status | purpose | depends_on | estimated_effort |
|---------|-------|--------|---------|------------|-----------------|
| fix-error-info-basic | 1 | planned | 跑 verify-family pipeline | 无 | 5 min |
| fix-threading-monitor | 1 | planned | 补 using 修复 C# 编译错误 | 无 | 5 min |
| onboard-string-construction | 1 | planned | 完整接入 string-construction | 无 | 15 min |
| onboard-thread-sync | 1 | planned | 完整接入 thread-sync | 无 | 15 min |
| fix-reflection-binding | 1 | planned | 修复 1 个 fact 失败 | Phase 2 完成后 | 15 min |
| fix-reflection-type | 1 | planned | 修复 1 个 fact 失败 | Phase 2 完成后 | 15 min |
| fix-buffer-memory-entry | 2 | planned | 诊断 entry.exe 构建失败 | 无 | 30 min |
| fix-span-memory-entry | 2 | planned | 诊断 entry.exe 构建失败 | 无 | 30 min |
| fix-string-char-entry | 2 | planned | 诊断 entry.exe 构建失败 | 无 | 30 min |
| fix-interface-dispatch-entry | 2 | planned | 诊断 entry.exe 构建失败 | 无 | 30 min |
| fix-reflection-member-complete | 2 | planned | 诊断 entry.exe 构建 + fact 失败 | 无 | 45 min |
| fix-time-datetime-convert | 3 | planned | 诊断 convert-to-cpp 崩溃 | 无 | 60 min |
| fix-type-handles-convert | 3 | planned | 诊断 convert-to-cpp 崩溃 | 无 | 60 min |
| cleanup-orphan-dirs | 4 | planned | 删除孤儿目录 | 无 | 2 min |
| refresh-dashboard | 4 | planned | 刷新 claims + ledger | phase-1,2,3 全部完成 | 5 min |

## 8. 依赖

- **chaos_runtime_core.lib**（RelWithDebInfo）必须预先构建好
- **Driver DLL**（Chaos.IL2CPP.Driver）必须包含所有 codegen emitter 修复
- **native libs**（chaos_bootstrap, chaos_common, chaos_interpreter 等）必须存在

## 9. 风险

| 风险 | 影响 | 缓解方案 |
|------|------|---------|
| 多个 entry.exe 构建失败共享同一 codegen 底层 bug | Phase 2 阻塞 | 先诊断第一个 family，如发现 codegen 层面的问题，统一修复后批量重跑 |
| convert-to-cpp 崩溃在 managed 层，需 C# 调试 | Phase 3 进展慢 | 运行 il2cpp.exe 带 --verbose 获取更多信息；检查 .g.output 日志 |
| 空壳 family（string-construction）的 managed API 在 codegen 中无翻译路径 | Phase 1 阻塞 | 检查 ledger 中 methodSubjectIds，跳过 codegen 不支持的 API |
| CMake build 目录被 Windows 锁定 | 全流程 | 重启终端或使用不同 build 目录名 |
| fact 验证时需要重新构建 entry.exe，但 build 目录可能被锁定 | Phase 2 | 手动构建 entry.exe 后复制到 native/ 目录 |

## 10. 备选路径

- 如果 Phase 2 的 5 个 family 全是同一 linker 问题（如 missing `FillExternalRuntimeStubs`），可以统一修复后批量重跑 pipeline
- 如果 Phase 3 的 convert-to-cpp crash 在 managed 层修复成本过高，可以考虑为对应 subject 添加 custom entry 绕开 crash 路径
- 空壳 family 如果在 Phase 1 阶段遇到 codegen 不支持的 opcode，登记为已知 limitation 并跳过

## 11. 当前建议推进顺序

```
Phase 1.1 (error-info-basic)
  → Phase 1.2 (threading-monitor-interlocked)
  → Phase 1.3 (string-construction onboard)
  → Phase 1.4 (thread-sync onboard)
  → Phase 2 (entry.exe build failures — any order)
  → Phase 1.5 + 1.6 (reflection-binding, reflection-type — need entry.exe from Phase 2)
  → Phase 3 (convert-to-cpp crashes)
  → Phase 4 (cleanup)
```

Phase 2 内部的 5 个 family 无依赖关系，可以按任意顺序处理。

## 12. 调度策略

- `dispatch_model`: `sequential`
- `child_execution_mode`: `auto`
- `auto_continue`: `true`
- `auto_stop_policy`: `blocking-only`
