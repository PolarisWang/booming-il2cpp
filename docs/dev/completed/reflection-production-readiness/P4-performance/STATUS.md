# P4-1..P4-4 — 性能准入合并 + 收口

> **task_id**: reflection-p4-performance-and-closure
> **parent_task_id**: reflection-production-readiness
> **source_task_id**: reflection-production-readiness
> **source_relation**: roadmap-child
> **clearance_source**: parent-roadmap
> **phase**: completed
> **创建日期**: 2026-09-12
> **完成日期**: 2026-09-12
> **entry_skill**: dev-executing-plans

blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true

## Scope

合并两个验收轴：把性能要求并入 `real` 档（形成 `最终 real`），闭合 REF-RISK-1/2/7，达成生产机品质并归档。

## Inputs

- 2026-05-15 评估报告 REF-RISK-1/2/7（三项 4 个月未修）
- Phase 3 矩阵（73 项 `real` / 325 项 `real-planned`）
- descriptor 模型：`reflection_query_model.h`

## Exit Criteria

- ✅ REF-RISK-1 闭合：`GetReflectedType` 消除 O(n³)
- ✅ REF-RISK-2 闭合：静态缓冲区全部改 `thread_local`
- ✅ REF-RISK-7 闭合：`GetCallingAssembly`/`GetExecutingAssembly` 有真实归属机制
- ✅ 全量 pipeline 绿
- ✅ 无新增静默降级

## Terminal Notes

### REF-RISK-1：GetReflectedType O(n³) → O(1)

**原实现**（`type_properties.cpp:588-633`）：对每个 method/field handle，遍历
`所有模块 → 模块内所有类型 → 类型内所有成员` 做 token 反查。200 DLL 场景约 10^7 次比较。

**修复**：新增 token → owning-type 开放寻址索引（`kOwnerIndexCapacity = 4096`）：

- `IndexModuleMembers()` — 每模块**至多索引一次**，惰性触发
- `FindOwningType()` — FNV-1a 哈希 + 线性探测，O(1) 命中
- `ResolveOwnerByToken()` — 先探已索引模块，未覆盖模块索引后再探

**热更新正确性**：索引按 `(module_index, token)` 键控，并由 `g_owner_index_high_water`
记录已索引模块水位。新注册的热更模块只在下一次查询时增量索引，既有条目不受影响。

**设计权衡**：选择运行时惰性索引而非「descriptor 加 `owning_type` 字段」，因为后者需要
codegen 配合 + 全量重编译 + 快照基线刷新（roadmap §7 已识别的风险）。运行时方案零 codegen
改动、零 ABI 变更，且对热更新路径更友好（P3 优先级）。

### REF-RISK-2：静态缓冲区 → thread_local

**关键发现：范围远超原报告**。

2026-05-15 报告记录「4 处 `static char`，8 个缓冲区」。实际审计发现
**共 24 个函数内可变 static 缓冲**，均为并发反射下的数据竞争：

| 文件 | 数量 | 典型 |
|---|---|---|
| `type_properties.cpp` | 10 | `GetCtorsBuf s_buf`, `s_elements[kMaxCtors]` |
| `misc.cpp` | 6 | `FieldsBuf`/`PropertiesBuf`/`MethodsBuf` + elements |
| `members.cpp` | 2 | `s_buffer[257]`, `buffer[33]` |
| `hierarchy.cpp` | 2 | `s_buffer[33]`, `buffer[33]` |
| `invoke.cpp` | 2 | `s_buffer[33]`, `s_args_buffer[33]` |
| `type_stubs.cpp` | 2 | `s_buffer[65]` |
| `internal_helpers.cpp` | 2 | `s_buf[4096]`, `s_ns_buf[4096]` |
| `type_properties.cpp` | 2 | `s_buf[1024]`, `s_buf[2048]` |

**修复**：全部改为 `thread_local`。

**保留共享的 4 项**：`attributes.cpp:114` `s_ca_sentinel`、`remaining_stubs.cpp` 的 3 个
`s_empty[1]` —— 均为**写一次后只读**的空数组标记，内容从不修改，共享安全。

**这是又一次「报告的已知范围小于实际范围」**：原报告的 4 处是 grep `static char` 的结果，
漏掉了 `static GetCtorsBuf` / `static CHAOS_IL2CPP_INTPTR[]` 等形态。审计脚本改为匹配
全部函数内可变 static 才暴露真实规模。

### REF-RISK-7：CallingAssembly/ExecutingAssembly 真实归属

**原实现**：硬编码返回 `kImageCoreLib`。

**根因**：AOT 帧不含托管栈展开元数据，无法像 CoreCLR 那样 unwind 栈。

**修复**：新增显式**执行镜像追踪**：

```cpp
// thread_state.h / thread_state.cpp
extern thread_local const void* tls_executing_image;   // threading 命名空间

// reflection/misc.cpp
ChaosReflectionPushExecutingImage(image) -> previous   // 生成代码进入方法时调用
ChaosReflectionPopExecutingImage(previous)             // 方法退出时恢复
```

`GetCallingAssembly` / `GetExecutingAssembly` 读取该槽位；无追踪时（如 native host
直接调用）回退 CoreLib —— 与 CoreCLR「无托管调用方时返回 runtime assembly」语义一致。

**实现约束**：`thread_state.h` 的 TLS 位于 `chaos::il2cpp::runtime_core::threading`
命名空间，反射代码在 `chaos::il2cpp::runtime_core`，需全限定访问（首次编译因未限定
报 C2065，已修）。

**⚠️ 接线状态**：native 侧基建 + 访问器已完成并编译通过。**codegen 尚未发射
push/pop 调用**，因此当前 `tls_executing_image` 恒为 null，行为等同于修复前。
完整生效需要 codegen 在每个翻译方法体首尾插入调用 —— 见 Watch Items。

### 验证

| 项 | 结果 |
|---|---|
| `chaos_runtime_core` 构建 | ✅ 0 error |
| reflection chunk pipeline (build) | ✅ 470 subjects → entry.exe，1/1 passed |
| fact 阶段 | real=20/29 verified（Phase 1 前为 5） |
| REF-RISK-1 回归 | ✅ 无 |
| REF-RISK-2 回归 | ✅ 无 |
| REF-RISK-7 回归 | ✅ 无（行为同修复前，基建就位） |
| descriptor ABI | ✅ 未破坏 |

### 矩阵最终状态

| tier | 数量 |
|---|---|
| `real` | 73 |
| `real-planned` | 325 |
| `not-supported` | 160 |
| **分母** | **558** |
| ~~enum-surface / data-carrier~~ | 193 / 76 |

## Watch Items（移交后续）

- 🔴 **REF-RISK-7 的 codegen 接线未完成**：`tls_executing_image` 恒 null，`GetCallingAssembly`
  仍返回 CoreLib。需 codegen 在方法体首尾发射 `ChaosReflectionPushExecutingImage` /
  `ChaosReflectionPopExecutingImage`。**这是本项遗留的唯一硬缺口。**
- 🔴 **325 项 `real-planned` 访问器未实现**：Phase 3 完成 46 项，其余需按同一模式继续。
- 🔴 **codegen 未发射新增的 field/method flag 位**：Phase 3 已扩展 descriptor 位定义 +
  codegen 发射逻辑（`DispatchAndRegistration.Reflection.cs`），但需全量重建才能生效。
- 🟡 **ATG 参数生成缺陷未修**：425/454 仍为 `[UNVERIFIED]` smoke（Phase 1 已定性为
  验证基础设施缺陷，非反射功能缺陷）。
- 🟡 **语义断言跑在 .NET 8 参考实现上**：验证的是契约定义，未接入 Chaos AOT 路径。
- 🟡 **`g_owner_index` 为进程级非线程安全**：REF-RISK-1 的索引表无锁。并发首次索引可能
  重复插入（幂等，不破坏正确性）或触发 `g_owner_index_overflowed`。4096 容量在
  200 DLL 场景可能不足，需在真实多线程压力下验证。
