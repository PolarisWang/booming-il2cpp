---
name: dev-il2cpp-foundation-dll-optimizer
description: dev-il2cpp-foundation-dll-optimizer — foundation-dll 深度优化专家
---

> ⚠️ **本文件通过 Skill 工具加载，作用是注入领域知识到当前对话上下文。**
> 本文件**不是可执行的 agent**。当前 Agent 需阅读下方"执行流程"作为实现参考。
>
> **当前 Agent 请做**：阅读知识域和约束 → 自行实现代码 → 自行验证
> **当前 Agent 不要做**：加载后等待"Expert 自动执行"——它不会，Skill 只加载文本。
>

# dev-il2cpp-foundation-dll-optimizer — foundation-dll 深度优化专家

## 领域边界

### 我负责的（自己修）
- **基准测试**：运行 foundation-dll benchmark，chaos-aot vs CoreCLR .NET 8/10 JIT vs Unity IL2CPP vs Mono 全平台对比
- **瓶颈分析**：八类瓶颈（GC分配/调用分派/内联缺失/翻译低效/边界检查/运行时Helper/MemoryBarrier/HotUpdate兼容）
- **根因溯源**：每个慢方法追溯到具体 codegen 文件:行号、运行时函数、翻译路径
- **三方代码参考**：分析 CoreCLR JIT 和 Unity IL2CPP 在等效场景下的实现策略
- **优化实现**：codegen 修改、运行时优化、翻译路径调整
- **提交规范**：每次优化提交必须有优化前后数据 + 三方对比
- **迭代控制**：反复优化直到所有方法 vs CoreCLR < 1.2x 且无 >2.0x 的方法

### 我负责的（精确诊断后移交 Expert）
- GC 分配策略修改 → `dev-il2cpp-gc-expert`
- CodeGen 翻译逻辑/模板修改 → `dev-il2cpp-codegen-expert`
- 运行时 crash/dangling pointer 定位 → `dev-il2cpp-debug-expert`
- 大规模跨 family 优化战役 → `dev-optimization-campaign`

### 我不负责的（超出以下范围 → 标记 remaining，回 Dispatcher 重新分发）

- **翻译路径设计**（IL→C++ 翻译策略选择）→ 超出范围，标记 remaining，原因：需要翻译域知识
- **GC 引擎核心算法**（分代策略、写屏障语义）→ 超出范围，标记 remaining，原因：需要 GC 域知识
- **运行时非优化类修改** → 超出范围，标记 remaining，原因：需要运行时域知识
- **编译失败 / codegen stub**（LNK 错误、C++ 编译错、CMake 错误、dotnet build 失败）→ 超出范围，标记 remaining，原因：需要构建修复域知识

## 输出格式（Dispatcher 回读用）

每个 Expert 处理完任务后，必须在当前上下文中输出：

```
✅ done: [已处理的子任务 ID 列表]
⏳ remaining: [未处理的子任务 ID 列表 + 原因]
```

---

## 触发条件

- 从 `dev-il2cpp-core-agent` 路由（关键词：benchmark、优化、profile、性能对比、比.NET慢、优化迭代）
- 用户直接输入 `/dev-il2cpp-foundation-dll-optimizer`
- benchmark 回归检测报告中有 RED/CRITICAL 级别的方法

---

## 优化总览

```
Phase 0: 基线扫描
  ├── chaos-aot benchmark
  ├── CoreCLR .NET 8/10 benchmark
  ├── Unity IL2CPP benchmark (参考)
  └── Mono benchmark (参考)
  └── 输出: 三层对比基线

Phase 1: 瓶颈分析（逐方法）
  ├── 过滤慢方法（vs CoreCLR >50%）
  ├── 瓶颈分类（8类）
  ├── 根因溯源到文件:行号
  └── 对照 CoreCLR/Unity 实现
  └── 输出: 方法级分析报告 + 优先级排序

Phase 2: 方案设计
  ├── 按瓶颈类型出优化方案
  ├── 三优先级权衡（P1性能>P2架构>P3热更）
  └── 预期收益估算
  └── 输出: 优化方案

Phase 3: 实现 → 验证 → 提交
  ├── 实现优化
  ├── 重跑 benchmark
  ├── 前后数据对比
  └── 三段式 commit
  └── 输出: commit + 数据对比

Phase 4: 迭代判定
  ├── 还有 CRITICAL (>2.0x) ？→ P0 立即修复
  ├── 还有 RED (>1.5x) ？→ 下一轮
  ├── 还有 YELLOW (>1.2x) ？→ 询问用户
  └── 全部 GREEN (<1.2x) ？→ ✅ 优化达标
```

---

## 瓶颈分类框架

| 类别 | 子类 | 典型根因 | 溯源目标 | 三方参考 |
|------|------|---------|---------|---------|
| **GC 分配** | 临时分配、装箱、数组分配、闭包 | codegen 缺栈分配 | `ObjectModelEmission`, `GcAllocate` | Unity IL2CPP: 栈分配模式 |
| **调用分派** | 虚方法、接口、间接调用 | VTable 多层查找 | `InvocationPlanning`, `vtable_registry` | CoreCLR: VTable slot 直接索引 |
| **内联缺失** | 小方法、委托、属性未内联 | Planner inlining 范围 | `InliningPlanner` | CoreCLR JIT: 多级内联预算 |
| **翻译低效** | 过多 C++ 指令、ABI 转换 | Emission 策略不够优 | `MethodEmission`, `ExceptionEmission` | Unity IL2CPP 翻译输出 |
| **边界检查** | 数组/字符串索引 | 缺少边界检查消除 | `StructuredIR`, array stubs | CoreCLR JIT: 范围分析消除 |
| **运行时 Helper** | ExternalRuntimeFallback | 方法未走 AOT | `ExternalRuntimeHelpers` | — |
| **Memory Barrier** | 多余 volatile/写屏障 | barrier 过度生成 | `GcSlotMap`, memory order | CoreCLR JIT: 消除冗余 barrier |
| **HotUpdate 兼容** | mode-aware 分支开销 | dispatch 层级过多 | `hotpatch_dispatch` | — |

---

## 🚨 第一步：输出分类声明（强制阻断）

**在使用任何工具之前，必须先输出：**

```
classification: domains=[优化] mode=knowledge-inject expert=dev-il2cpp-foundation-dll-optimizer
```

**不输出分类声明，不得进行任何文件操作。**

---

## Phase 0: 基线扫描

### Step 0.1: chaos-aot benchmark

```bash
python testing/foundation-dll/verification/chunk_pipeline.py \
  --chunks <target> --stages build,benchmark,benchmark_report
```

### Step 0.2: CoreCLR benchmark

```bash
python testing/foundation-dll/verification/chunk_pipeline.py \
  --chunks <target> --stages managed_benchmark
```

### Step 0.3: Unity IL2CPP + Mono benchmark（参考）

如果目标环境可用，构建等效测试并运行 benchmark。记录方法级耗时和分配量。

### Step 0.4: 生成三层对比基线

```
输出: comparison.json（chaos-aot vs coreclr vs unity vs mono）
每个方法: { chaosAotVsCoreclrPct, unityVsCoreclrPct, monoVsCoreclrPct }
聚合: 按 chunk 几何平均
```

---

## Phase 1: 瓶颈分析（逐方法深度溯源）

### Step 1.1: 过滤慢方法

```
对 benchmark 中每个方法:
  ratio = chaos_duration / coreclr_duration
  分级:
    CRITICAL (ratio > 2.0)  → P0 blocker，立即通知
    RED      (1.5 < ratio <= 2.0) → 必须优化
    YELLOW   (1.2 < ratio <= 1.5) → 二次优化目标
    GREEN    (ratio <= 1.2) → 达标
```

### Step 1.2: 逐方法分析（使用以下模板）

```markdown
## Method: <SubjectId>

### 性能数据
| 平台 | 耗时(ns) | vs CoreCLR |
|------|---------|-----------|
| Chaos AOT | xx.x | -yy% |
| CoreCLR .NET 8 | xx.x | baseline |
| Unity IL2CPP | xx.x | -yy% |
| Mono | xx.x | -yy% |

### 瓶颈分类
- 主类别: <GC分配/调用分派/内联缺失/翻译低效/边界检查/Helper/Barrier/HotUpdate>
- 子类: <具体子类>
- 确信度: <高/中/低>（验证: <做了什么测试确认>）

### 根因溯源
- codegen 文件: <路径:行号> — <函数名>
- 运行时: <路径:行号> — <函数名>
- 翻译路径: <翻译表条目>

### 三方代码参考
- **CoreCLR JIT**: <文件:行号> — <策略描述>
- **Unity IL2CPP**: <文件:行号> — <策略描述>

### 建议方案
1. 短期: <快速修复，影响 N 个方法，预期提升> <复杂度>
2. 长期: <完整方案，影响 N+ 个方法，预期提升> <复杂度>
```

### Step 1.3: 分析三方策略

| 平台 | 分析方法 | 参考源 |
|------|---------|--------|
| **CoreCLR JIT** | 查找 JIT 生成对应 IL 时的优化策略 | `src/coreclr/jit/`（开源） |
| **Unity IL2CPP** | 对比生成的 C++ 代码量和运行时调用 | `il2cpp-codegen/`（开源） |
| **Mono** | 参考其 JIT/interpret 实现（注意 Mono 通常更慢） | `src/mono/`（开源） |

### Step 1.4: 优先级排序

按 `ratio * 调用频率` 排序——ratio 高且调用频繁的方法优先。

---

## Phase 2: 方案设计

### Step 2.1: 典型方案

| 瓶颈类型 | 典型方案 | 预期提升 | 复杂度 |
|---------|---------|---------|--------|
| GC 分配 | 栈替代堆、值类型替代引用、缓存分配 | 30-80% | 中-高 |
| 调用分派 | VTable slot 预计算、接口分派优化 | 10-40% | 高 |
| 内联缺失 | 扩展 inlining、调优内联预算 | 20-60% | 中 |
| 翻译低效 | 消除冗余 C++ 指令、优化 ABI | 10-30% | 低-中 |
| 边界检查 | 循环范围分析、已知长度跳过 | 10-50% | 高 |
| 运行时 Helper | interpreter routing、直接 AOT | 100x+ | 中 |
| Memory Barrier | 消除冗余 volatile、优化写屏障 | 5-20% | 高 |
| HotUpdate 兼容 | 优化 mode-aware 分支 | 5-15% | 中 |

### Step 2.2: 三优先级权衡

```
P1 性能: 预期提升 ? 是否引入分配 ? 增加分支 ? 增加代码体积 ?
P2 架构: 遵循既有模式 ? 增加新抽象层 ? 与其他模块耦合 ?
P3 热更: 影响 hotpatch dispatch ? 需要 patch data 变更 ?
```

### Step 2.3: 预期收益估算

```
优化前: 45.2 ns, 48 B alloc    优化后: 31.2 ns, 0 B alloc（估算）
提升: +31%, -100% alloc         实现: 1-2天    影响: 12 方法
```

---

## Phase 3: 实现 → 验证 → 提交

### Step 3.1: 实现

实现优化，必要时调用对应 Expert：
- CodeGen 修改 → `Skill("dev-il2cpp-codegen-expert")`
- GC 优化 → `Skill("dev-il2cpp-gc-expert")`
- 翻译路径 → `Skill("dev-il2cpp-translation-expert")`

### Step 3.2: 重跑 benchmark

```bash
python testing/foundation-dll/verification/chunk_pipeline.py \
  --chunks <affected> --stages build,benchmark,benchmark_report
```

### Step 3.3: 生成优化前后数据

```markdown
## 优化前后数据
| 方法 | 优化前(ms) | 优化后(ms) | 提升 | 分配前(B) | 分配后(B) |
|------|-----------|-----------|------|----------|----------|
| List`1::Add | 45.2 | 31.2 | +31% | 48 | 0 |

## 三方对比（优化后）
| 平台 | 耗时(ns) | vs Chaos |
|------|---------|---------|
| CoreCLR .NET 8 | 29.8 | +4.7% |
| Unity IL2CPP | 32.1 | -2.8% |
| Mono | 55.3 | +77% |
```

### Step 3.4: 提交

```
perf: <方法名/类别> 优化描述

<优化前后数据表格>
<三方对比表格>

root_cause: <根因>
fix_strategy: <策略>
regression_check: <回归验证范围>
```

---

## Phase 4: 迭代判定

每次优化完成后：

```
1. 重跑全 benchmark（Phase 0）
2. 遍历所有方法的 chaos-aot vs coreclr ratio
3. 分级判定:
   ┌──────────┬─────────┬─────────────────────────┐
   │ 等级     │ 阈值    │ 处理                    │
   ├──────────┼─────────┼─────────────────────────┤
   │ CRITICAL │ > 2.0x  │ P0 blocker，立即修复    │
   │ RED      │ 1.5-2.0 │ 必须优化，进下一轮       │
   │ YELLOW   │ 1.2-1.5 │ 询问用户是否继续         │
   │ GREEN    │ < 1.2x  │ 达标                    │
   └──────────┴─────────┴─────────────────────────┘
4. 迭代:
   有 CRITICAL → Phase 1（P0 优先级）
   有 RED → Phase 1
   只有 YELLOW → 询问用户
   全部 GREEN → ✅ 优化达标
```

---

## 执行前 Checklist

```
□ 目标: 基线扫描 / 瓶颈分析 / 实现优化 / 迭代判定？
□ 当前基线数据路径？
□ CoreCLR/Unity/Mono 数据是否可用？
□ 是首次优化还是迭代优化？
□ 需要调用其他 Expert Agent？
```

## 执行后 Checklist

```
□ benchmark 重跑通过
□ 优化前后数据对比表格已生成
□ 三方对比表格已生成
□ 无 CRITICAL 级别方法残留
□ 分级判定完成: GREEN/YELLOW/RED/CRITICAL
□ 新故障模式 → 通知 dev-il2cpp-debug-expert
```

## 输出约束

1. **50% 阈值** — 比 CoreCLR 慢 >50% 的方法必须进入优化队列
2. **全平台对比** — 每次优化必须有 CoreCLR + Unity IL2CPP + Mono 对比数据
3. **优化前后数据** — 每次提交必须包含前/后 benchmark 数据
4. **三段式 commit** — `root_cause` / `fix_strategy` / `regression_check`
5. **迭代继续条件** — 存在 RED/CRITICAL 时必须继续
6. **达标标准** — 全部 GREEN（vs CoreCLR < 1.2x）且无 CRITICAL（vs CoreCLR < 2.0x）
7. **故障模式同步** — 发现新故障模式通知 `dev-il2cpp-debug-expert`

## 集成点

| 上游 | 本 skill | 下游 |
|------|----------|------|
| `dev-il2cpp-core-agent` → 优化路由 | **dev-il2cpp-foundation-dll-optimizer** | `dev-il2cpp-gc-expert` |
| 用户直接输入 | | `dev-il2cpp-codegen-expert` |
| benchmark_diff.py 检测回归 | | `dev-il2cpp-debug-expert` |
| | | `dev-optimization-campaign` |
