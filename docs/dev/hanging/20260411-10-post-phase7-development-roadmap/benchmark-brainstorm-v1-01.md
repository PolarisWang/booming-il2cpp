# Brainstorm: Benchmark 数据统计与 Dashboard 方案

**任务**: 20260411-10-post-phase7-development-roadmap
**日期**: 2026-04-11
**主题**: Phase B 三方 Benchmark 数据存储、查询与可视化

---

## 核心需求

1. 三种模式 (managed / native / interpreter) 不一定一次性全跑，需要增量记录
2. 多平台多设备 — Windows, macOS, Android (真机/模拟器), iOS (真机/模拟器)
3. 设备信息自动识别: OS名 + CPU型号
4. 生成静态 HTML dashboard，本地浏览器直接打开
5. `docs/benchmark/` 同步保存最新最全信息 (根文件)
6. 每次跑完自动更新 dashboard 到 `docs/benchmark/`

---

## 已探索方案

### 方案 A: 分离模式文件
每种模式各一个文件，3 个文件构成完整记录。最简单，但"一份记录"需要合并读取。

### 方案 B: 合并单文件
每个 subject-platform 一个 JSON，mode 分区。直观但不支持历史。

### 方案 C: Append-Only JSON Lines ← 选定
每个 subject 一个 `records.jsonl`，每次运行 append 一行。天然历史记录，查询最新值得到"当前状态"。

---

## 选定方案的关键决策

| 问题 | 决策 |
|------|------|
| 文件位置 | `subjects/{id}/benchmark-records/records.jsonl` |
| 设备信息来源 | 自动检测: OS+CPU (各平台不同命令) |
| 总览默认平台 | Windows (统计次数最多) |
| 根存储 | `docs/benchmark/` 同步最新最全数据 |
| 自动化 | 每次 benchmark 跑完自动重新生成 dashboard |

---

## 结论

进入设计阶段，编写 design-v1-01.md 详述: record schema、文件结构、设备检测逻辑、dashboard 页面结构、自动化流程。
