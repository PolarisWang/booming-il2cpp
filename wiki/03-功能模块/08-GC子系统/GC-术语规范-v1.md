# Chaos IL2CPP — GC 术语规范（v1）

> **状态**：v1（2026-08-11，由 native 编码规范阶段2 + GC 代码审计产出）
> **目的**：统一 GC 子系统代码/注释/文档中的术语，消除同义混用（源码中存在），为后续阶段 2 收敛 + 去代号提供**权威术语表**。
> **原则**：以 `wiki/03-功能模块/08-GC子系统/` 既有文档为准，对齐 CoreCLR 术语。

## 1. 权威术语表

| 规范用词 | 已被弃用的同义混用 | 含义 |
|---|---|---|
| `nursery` | `young-gen` | 年轻代（新生代）区域 |
| `Generation 0`（gen0） | `young` | 第 0 代（对象首次分配处） |
| `Generation 1`（gen1） | `Gen1` / `Gen 1` / `gen one` | 第 1 代（nursery GC 幸存后） |
| `full GC` | `major GC` | 全代回收 |
| `background GC`（BGC） | `concurrent GC` | 并发后台回收 |
| `card bundle` | — | 粗粒度卡片簇（对齐 CoreCLR card_bundle） |
| `write barrier` | `card mark` / `dirty` | 写屏障（记录引用写入） |
| `weak handle` | `finalizable` / `pinned handle` | 弱引用句柄 |
| `finalizable` | `finalizer` | 终结器可达对象 |
| `Publish…` | `Collect…` / `ProcessCollectedWeakHandles` | 弱句柄/终结器**发布**阶段的统一动词 |
| `safepoint`（STW） | `global safepoint` | 停世界点（generation odd） |
| `demote` | `demotion` | T4 → 解释器降级（热更/去优化） |
| `SPB` | — | safepoint-but（并发扫描模型，见 cross-gen stress 根因） |

## 2. 代码/注释内统一要求
- 一律用「权威用词」列。
- **禁止** `Gen1`（数字无空格驼峰）、`Gen 1`（带空格）、`young-gen`（连字符小写）混用 → 统一 `Generation 1` / `nursery`。
- 弱句柄/终结器动词统一为 `Publish`（弱句柄发布），反对 `Collect`/`ProcessCollectedWeakHandles` 混用（见 gc_bgc.cpp vs gc_old_gen.cpp 差异）。
- 新代码/新注释必须遵守；存量代码在阶段 2 后续收敛时逐步替换。

## 3. 已知差异（待收敛，file 定位）
- `gc_old_gen.cpp:2642` `GcProcessCollectedWeakHandles` vs `gc_bgc.cpp:553` "Publish dead finalizables + weak handles"。
- `gc_api.h:84` `kind` 标注 "Currently unused" 与 `gc_old_gen.h`/`gc_bgc.h` 的 finalizer/weak-handle 流程术语不一致。

## 4. 关联
- 编码规范：`wiki/.../native-coding-conventions-v1.md`
- 审计计划：`docs/dev/in-progress/jit-regalloc-a2/native-codebase-standardization-plan.md`
