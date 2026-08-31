# GC 工业化极致计划 — 设计文档

## 架构评估

### 影响域（≥4 域，需 Workflow 委托）

| 域 | 受影响模块 | 改动量 | 专家 |
|:---|------|:---:|:---:|
| **GC 核心** | safepoint、写屏障、region 分配器、BGC、L1 卡表 | ~22k LOC 中重写 30-40% | dev-il2cpp-gc-expert |
| **CodeGen** | 写屏障指令序列、store 指令 | ~10+ 文件 | dev-il2cpp-codegen-expert |
| **JIT** | 寄存器根扫描、Windows APC 回退、safepoint 插入 | ~5+ 文件 | dev-il2cpp-codegen-expert |
| **Interpreter** | 帧 root 扫描、写屏障 | ~3+ 文件 | dev-il2cpp-runtime-expert |
| **CI/测试** | gc-ci.yml、nightly、known-fail、baseline、test 框架 | ~15+ 配置/测试文件 | project-test-governance |
| **诊断** | ETW、GCMemoryInfo、GCNotification | ~8+ 文件 | dev-il2cpp-gc-expert |
| **平台** | ARM64 全 CI、Linux/macOS、EventPipe | ~10+ 文件 | dev-il2cpp-build-fixer |

### 涉及四层架构
- **GC 层**（src/native/runtime-core/gc/）：核心改造
- **运行时层**（src/native/runtime-core/core/、thread_state.cpp）：safepoint 重写、分配器统一
- **CodeGen 层**：写屏障指令序列、store 指令
- **CI/测试层**：测试框架、CI 配置、known-fail 管理

## 边界拍板

- **目标**：超越 CoreCLR WKS 的工业化成熟度，消除所有已知并发正确性缺陷
- **非目标**：不重写 full GC 的全并行 mark-sweep（CoreCLR 级），沿用现有并行架构
- **对标对象**：CoreCLR WKS（Workstation GC）
- **深度**：A3 完全对齐 — 硬 STW + 单 region 分配器 + LEAF 汇编 barrier + 全链路安全
- **验证策略**：验证先行 — CI 护网/ASAN/TSAN 就绪后再改核心代码
- **差异化优势**：热更新 MemoryDomain 保持全球领先，每个 Phase 至少含 1 个小维护 commit

## Authority 决策

- A2b 跨代 UAF → 结构性消除（A3 深度），非测试断言调整
- 三套并行分配系统 → 统一为单 region 分配器
- 写屏障 → LEAF 汇编（纯汇编，无 C++ call，x64 + ARM64 两套）
- `forbid_suspend.h` ack-and-continue → 硬 STW 后废弃
- L1 卡表 → 恒定卡表（永不 realloc）或 RCU
- BGC root-scan → 挂起根因定位修复，非已知-fail 归档

## 当前结论

### 推荐方案：并行双轨（Parallel Dual-Track）

```
Track A（验证基础设施）          Track B（A2b A3 架构设计）
   │                                │
   ├─ CI 门禁加固                    ├─ 读 CoreCLR WKS safepoint 源码
   ├─ ASAN/TSAN 接入 CI              ├─ 单 region 分配器架构设计
   ├─ 性能基线建立                   ├─ LEAF barrier 设计
   ├─ known-fail 数据库              ├─ codegen/JIT 接口设计
   │                                │
   └─── 两轨汇合 ───→ Phase 1-5
```

### Track A — 验证基础设施（并行启动）

| # | 任务 | 预估 |
|:--|------|:----:|
| A1 | Stress 测试 `-LE` 排除改为 `LABEL` 白名单，移除 `\|\| true` | 2-3天 |
| A2 | ASAN 接入 CI（per-PR 门禁，非仅 nightly） | 1周 |
| A3 | TSAN 接入 CI（并发测试专用） | 1周 |
| A4 | 性能基线（Release build、P99、阶段分解、对比 CoreCLR） | 1-2周 |
| A5 | known-fail 数据库带 expiry | 3天 |
| A6 | Cross-test 全域态污染检测 | 1周 |

### Track B — A2b A3 架构设计（并行启动）

| # | 任务 | 预估 |
|:--|------|:----:|
| B1 | 读 CoreCLR WKS safepoint 源码（SuspendThread + GetThreadContext + LEAF barrier） | 1-2周 |
| B2 | 设计 CRAG 单 region 分配器（统一 TLAB/nursery/GcBumpCache/old-gen） | 1周 |
| B3 | 设计 LEAF 汇编写屏障（x64 + ARM64 两套） | 1周 |
| B4 | 设计 codegen/JIT 接口改动的契约（store+barrier 原子窗口） | 1周 |
| B5 | 设计 forbid_suspend.h ack-and-continue 废弃方案 | 3天 |

### Phase 1: A2b A3 实现（ASAN 护网下）

- 重写 safepoint 主路径（SuspendThread + GetThreadContext）
- 统一 region 分配器（消除三套并行路径）
- 实现 LEAF 汇编 barrier（替代现有 C++ barrier）
- 废弃 forbid_suspend.h 的 ack-and-continue
- 验证：gc_region_barrier_stress_test 0/1000 挂起

### Phase 2: 残余并发 bug 修复

- L1 卡表重分配 → 恒定卡表（永不 realloc）或 RCU
- BGC root-scan 挂起 → 最终根因定位
- CrossPageCompact 空转 → 提前退出条件
- CodegenStSFld 三路不一致 → 统一路径
- chaos_is_gc_pointer nursery 盲点修复

### Phase 3: 诊断/配置完备

- ETW 事件 9→40+（BGC 阶段火点接入）
- GCMemoryInfo BCL 托管类型
- GCNotification 托管 API 接线
- 配置 knobs 24→~75（含每代 HardLimit、BGC servo 参数）
- BGC servo 闭环调速

### Phase 4: 性能/平台对齐

- Full GC 100MB ≤ CoreCLR 1.5x（Release 测）
- Page decommission / FindPage 索引化
- ARM64 全 CI 绿
- Server GC 全量回归（per-PR 门禁）
- 跨平台诊断（EventPipe / Linux 等价物）
- 大页 / CPU 亲和

### Phase 5: 差异化优势

- MemoryDomain 热更新深度加固 + 文档体系
- 精确 GcLayout 扫描 + 寄存器根（JIT T2.2 基础上强化）
- NUMA 深度优化（动态 heap count）
- 域卸载 O(region) 差异化能力成体系
- 长期：GC 模型检查 / 形式化验证可行性探针

## 风险评估摘要

| 风险 | 触发条件 | 后果 | 缓解策略 |
|:----|---------|:----:|:--------|
| A3 实现中引入新并发回归 | Track A 护网未就绪即开始改 | 回归不可见 | 严格门禁：Track A 绿后才合入 Phase 1 代码 |
| 单人执行疲劳 | 连续 6+ 月高强度 | 质量下降/放弃 | 阶段切到月粒度，每阶段结束有明确完成信号 |
| CoreCLR 源码细节过多 | 读不完 | 设计延迟 | 只读 safepoint/barrier/分配器核心路径 |
| 双轨认知开销 | 每日切换 | 效率下降 | 固定节奏：上午 Track A，下午 Track B |
| 发现未知并发 bug | ASAN 满 CI 跑 | 计划外工作量 | Phase 2 预置 ~30% 缓冲时间 |
| 热更新优势退化 | 长期无人关注 | 差异化丧失 | 每个 Phase 含 1 个小热更新维护 commit |

## 三优先级权衡结论

| 优先级 | 评分 | 裁决 |
|:------|:----:|:-----|
| 性能最优 (P1) | 中高 | 性能优化排在并发正确性之后，但 Track B 设计阶段已考虑性能约束 |
| 架构完美 (P2) | 高 | 验证先行确保改造安全，设计先行确保架构一致 |
| 热更新支持 (P3) | 高 | 差异化优势有独立 Phase 5，不挤占资源 |

## 问题清零

- `blocking_questions = []` ✅
- `question_clearance = cleared` ✅
- `clearance_confirmed_by_user = true` ✅

## 下一步入口

→ `roadmap`（需用户确认分流）