# Runtime Support Stubs — 架构审查报告

## 审查范围

`src/native/runtime-core/runtime_stubs/` 目录下 20 个文件 + 测试基础设施。

## 整体评估

Runtime Support Stubs 已达到工业级质量。所有文件均通过功能完整实现、单元测试覆盖、性能基准和生产级文档的审计。以下是结构告警和架构建议。

## 结构告警

### A1: 重复的 `resolve_string_arg` 实现（中优先级）

`datetime_stubs.cpp:38-57` 和 `guid_stubs.cpp` 各自维护独立的 `resolve_string_arg` 静态函数：

```cpp
static CHAOS_IL2CPP_INTPTR resolve_string_arg(CHAOS_IL2CPP_INTPTR value) noexcept {
    if (value == 0) return 0;
    if (!chaos_is_string_id(value)) return value;
    auto view = string_table::Resolve(chaos_extract_string_id(value));
    // ... allocate StubStringHeader + memcpy
}
```

**影响**：代码重复约 20 行，维护时需要同步修改两处。

**建议**：抽取到 `stub_common.h` 作为内联函数（`inline`）或独立的 `string_utils.h`。

### A2: `stub_common.h` 职责过宽（中优先级）

当前 `stub_common.h` 混合了四个不相关的职责：

| 职责 | 类型 |
|------|------|
| 数组访问器 (`ManagedArrayAccessor`, `get_managed_array`) | 数组工具 |
| 字符串头 + 数据访问 (`StubStringHeader`, `stub_string_data`) | 字符串工具 |
| PRNG (`stub_xorshift32`) | 随机工具 |
| GC 分配 (`GcAllocateAtomic` 间接 via `gc_helpers.h`) | 内存工具 |

**影响**：`stub_common.h` 的 include 依赖会引入不需要的工具链。例如 `stub_xorshift32` 在 stub 域外不需要。

**建议**：拆分建议：
- `stub_array.h` — `ManagedArrayAccessor` + `get_managed_array`
- `stub_string.h` — `StubStringHeader` + `stub_string_data`
- `stub_random.h` — `stub_xorshift32`

### A3: codegen ↔ stub ABI 一致性无正式规格（低优先级）

`extern "C"` 函数的签名在 codegen 和 stub 之间靠手动同步。无自动化一致性检查。

**当前保护**：
- 头文件声明（`*_stubs.h`）作为单一真相源
- 测试验证了每个函数的输入输出行为

**建议**：长期可以考虑引入 ABI 签名校验（函数指针类型静态断言）。当前测试覆盖已足够。

## 架构评估

### 分层

```
codegen (managed code → extern "C" call)
    ↓
runtime_stubs/*.cpp (extern "C" entry points)
    ↓
runtime-core (GC, thread_state, string_table, reflection)
```

职责清晰，无循环依赖。每个 stub 文件对应一个 .NET 功能域。

### 分配策略

通过 `GcAllocateAtomic` 统一由 GC 管理，codegen 无直接 GC API 调用（通过 stub 间接）。符合 `统一内存分配体系` 的 A→B→C 三阶段透明切换策略。

### 冷路径标识

使用 `CHAOS_IL2CPP_LOG_WARN("Stub", "<module> called")` 标识冷路径入口。在 CHECK 构建下可追踪意外调用路径。

### 国际化约束

字符串操作约定为 ASCII-only + 文档约束。`char_stubs.cpp` 使用 BMP Unicode 范围表支持非 ASCII 数字/字母分类。

## 建议清单

| 优先级 | 项目 | 工作量 | 影响 |
|--------|------|--------|------|
| 📋 P3 | 抽取 `resolve_string_arg` 到公共工具 | 0.5d | 消除代码重复 |
| 📋 P3 | 拆分 `stub_common.h` | 1d | 改善依赖管理 |
| 📋 P4 | ABI 签名校验静态断言 | 1d | 长期维护性 |

## 评估结论

**Runtime Support Stubs 已达到工业化标准。** 三个结构告警均不阻塞当前发布，建议作为后续维护 backlog 处理。全部 Phase 4 完成条件已满足。

---

*审查日期: 2026-05-23*
*审查范围: src/native/runtime-core/runtime_stubs/* + testing/src/native/runtime-core/runtime_stubs/*
