# GC 基准回归测试套件 实现计划

> **面向执行 Agent：** 本计划对应 roadmap 子任务 gc-p3-02 (GC 基准回归测试套件 G-15)。预计 1 个工作日完成核心实现。

**目标：** 构建可重复的 GC 基准测试套件，测量分配吞吐量、GC 暂停时间、多线程分配等核心指标，输出 JSON 可对比报告。

**架构：** 单个 `gc_benchmark.cpp` 实现 Benchmark 框架 + 所有场景。框架提供 Timer（高精度计时）、Metrics（统计）、Report（JSON 输出）三个基础组件。每个场景继承 Benchmark 基类，实现 Run()。

**架构审核模式：** normal

**结构告警重点：** 保持单文件可读性，不提前抽象；仅在场景 > 8 个时考虑拆分文件。

**权责图审核主题：** n/a

**设计文档：** n/a（继承父 roadmap 边界）

**问题清零来源：** parent-roadmap

**计划来源：** roadmap-child-auto

---

## 待创建/修改的文件

### 新建文件
| 文件 | 用途 |
|------|------|
| `tests/contracts/native/runtime-core/gc_benchmark.cpp` | 基准框架 + 所有场景 |

### 修改文件
| 文件 | 修改 |
|------|------|
| `tests/contracts/native/runtime-core/CMakeLists.txt` | 添加 gc_benchmark 可执行文件目标 |

## 步骤

- [ ] **步骤 1：实现 Benchmark 框架** — Timer（高精度时钟）、Metrics（count/sum/min/max/avg/p50/p95/p99）、Report（JSON 序列化）
- [ ] **步骤 2：实现场景 1 — 小对象分配吞吐量**：顺序分配 64B 对象，测量 allocs/sec
- [ ] **步骤 3：实现场景 2 — 混合大小分配吞吐量**：随机大小 16-1024B 混合分配
- [ ] **步骤 4：实现场景 3 — GC 暂停时间测量**：触发 N 次 young GC，测量每次 STW 暂停
- [ ] **步骤 5：实现场景 4 — 多线程分配**：4/8 线程并发分配，聚合吞吐量
- [ ] **步骤 6：实现场景 5 — 大对象分配**：LOH 路径（>32KB），测量分配延迟
- [ ] **步骤 7：实现场景 6 — BGC 交互**：分配线程 + BGC 并发执行，测量分配是否被阻塞
- [ ] **步骤 8：实现 main 函数**：场景注册、命令行参数（--scenario, --threads, --duration）、JSON 报告输出
- [ ] **步骤 9：更新 CMakeLists.txt** — 添加 gc_benchmark 可执行文件目标，链接 GC 库
- [ ] **步骤 10：构建 + 运行验证** — WKS 模式编译通过，至少运行 1-2 个场景确认输出

## 基准场景详述

| 场景 | 描述 | 关键指标 | 预期 |
|------|------|---------|------|
| `alloc_small` | 顺序分配 64B 对象 | allocs/sec, avg_latency_ns | 基线可对比 |
| `alloc_mixed` | 随机 16-1024B 混合分配 | allocs/sec | 基线可对比 |
| `gc_pause` | 触发 N 次 young GC，记录 STW 暂停 | pause_avg_ms, pause_max_ms | <5ms avg |
| `mt_alloc` | N 线程并发分配 | total_allocs/sec, thread_balance | 近线性扩展 |
| `loh_alloc` | 32KB-1MB 大对象分配 | allocs/sec, avg_latency_us | 基线可对比 |
| `bgc_interaction` | 分配线程 + BGC 共存 | allocs_during_bgc, max_pause_us | 不被 BGC 阻塞 |

## 验证方式

1. 构建：`cmake --build build_debug --target gc_benchmark` 通过
2. 运行默认场景：`gc_benchmark` 输出 JSON 报告到 stdout
3. 指定场景：`gc_benchmark --scenario alloc_small --duration 3` 正常运行
4. 报告包含：timestamp、scenario、metrics(duration, allocs, throughput, latencies)

## 收尾

执行完成后：结构告警自审 → 测试通过 → 归档 docs 目录 → 同步父 roadmap STATUS.md
