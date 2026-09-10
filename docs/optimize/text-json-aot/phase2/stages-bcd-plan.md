# Phase 2 Plan: Stages B → C → D

## Stage B: Managed Object Return Construction（2周）

**目标**：使 codegen 能生成 C++ 代码构造托管对象（JsonDocument、JsonNode 等）作为返回值。

**关键问题**：
```
当前：
  JsonDocument::Parse("42")
    → 返回 JsonDocument（托管引用）
    → codegen 无法生成构造代码
    → bridge thunk → 解释器

目标：
  JsonDocument::Parse("42")
    → codegen 生成 C++ 代码
    → 直接调用托管对象构造函数
    → 返回有效 JsonDocument 引用
```

**实现步骤**：

| Step | 内容 | 工时 |
|------|------|------|
| B1 | AOT IR 中识别返回托管对象的 call 指令 | 2天 |
| B2 | 为托管对象构造函数生成 extern 声明 | 2天 |
| B3 | 修改 EmitLinearCallTarget 对托管返回做特殊处理 | 3天 |
| B4 | 测试 + Benchmark | 2天 |

**前置条件**：Stage A 完成 ✅

---

## Stage C: Async State Machine Compilation（3-4周）

**目标**：使 codegen 能编译 async 状态机方法（SerializeAsync、DeserializeAsync）。

**关键问题**：
```
当前：
  SerializeAsync<Int32>(stream, 42)
    → IL 包含 async state machine（MoveNext、SetStateMachine）
    → codegen 遇到 state machine → CHAOS_IL2CPP_FAIL
    → bridge thunk → 解释器

目标：
  SerializeAsync<Int32>(stream, 42)
    → codegen 识别 async state machine 模式
    → 生成 C++ 状态机结构体
    → AOT 编译
```

**实现步骤**：

| Step | 内容 | 工时 |
|------|------|------|
| C1 | async state machine IR 模式识别 | 5天 |
| C2 | 状态机结构体 C++ 代码生成 | 7天 |
| C3 | awaiter 模式支持（TaskAwaiter、ValueTaskAwaiter） | 5天 |
| C4 | 测试 + Benchmark | 5天 |

**前置条件**：Stage A + B 完成

---

## Stage D: Complex Control Flow Compilation（3周）

**目标**：使 Utf8JsonWriter 内部的所有复杂控制流（异常处理、嵌套条件、多态虚函数）可 AOT 编译。

**关键问题**：
```
当前：
  Utf8JsonWriter.WriteNumberValue(42)
    → IL 包含 try/catch、复杂嵌套分支
    → codegen 部分区域 CHAOS_IL2CPP_FAIL
    → 方法整体 bridge thunk → 解释器

目标：
  Utf8JsonWriter.WriteNumberValue(42)
    → codegen 正确处理 try/catch
    → 生成完整 C++ 代码
    → AOT 编译
```

**实现步骤**：

| Step | 内容 | 工时 |
|------|------|------|
| D1 | 增强结构化 IR 对 try/catch 的支持 | 5天 |
| D2 | 多态虚函数调用去虚拟化 | 5天 |
| D3 | 复杂嵌套分支的 C++ 生成优化 | 3天 |
| D4 | 测试 + Benchmark | 3天 |

**前置条件**：Stage A + B + C 完成

---

## 整体时间线

```
Week 1-2:    Stage B ────────────► JsonDocument 返回可构造
                  ↓
Week 3-6:    Stage C ────────────────────► async 可编译
                  ↓
Week 7-9:    Stage D ────────────────────────────► Utf8JsonWriter 全部编译
                  ↓
Week 10:     集成验证 ──────────────────────────────────────► Text.Json 全部 AOT
```

## 风险

| 风险 | Stage | 概率 | 缓解 |
|------|-------|------|------|
| 托管对象布局不确定 | B | 中 | 使用 AOT IR 的类型信息，不依赖具体布局 |
| async 状态机变体多 | C | 高 | 只支持常见模式（Task/ValueTask），复杂 case fallback |
| 异常处理与结构化 IR 冲突 | D | 中 | 对无法处理的方法保留 bridge thunk fallback |

## 优先级建议

1. **Stage B 最高优先** — 托管对象返回构造是消除 bridge thunk 的关键
2. **Stage C 次优** — async 影响 SerializeAsync/DeserializeAsync 约 8 个方法
3. **Stage D 最后** — 影响范围最广，但收益取决于 B+C 是否已消除大部分桥接
