# Design: Benchmark 数据记录与 Dashboard 系统 v1.01

**任务**: 20260411-10-post-phase7-development-roadmap / Phase B
**日期**: 2026-04-11
**状态**: 设计确认

---

## 1. 整体架构

```
┌─────────────────────────────────────────────────────────────┐
│                     数据流                                    │
│                                                             │
│  run benchmark --mode native --record                       │
│         │                                                   │
│         ▼                                                   │
│  device_detector.py                                         │
│  └─ 自动采集: OS + CPU → device_id / device_name           │
│         │                                                   │
│         ▼                                                   │
│  benchmark_records.py                                       │
│  └─ append → subjects/{id}/benchmark-records/records.jsonl │
│         │                                                   │
│         ▼ (自动触发)                                        │
│  benchmark_dashboard_generator.py                           │
│  ├─ 读取所有 subjects/*/benchmark-records/records.jsonl    │
│  ├─ 生成 docs/benchmark/latest/{subject}.json              │
│  ├─ 生成 docs/benchmark/overview.json                      │
│  └─ 生成 docs/benchmark/dashboard.html                     │
└─────────────────────────────────────────────────────────────┘
```

---

## 2. 目录结构

### 2.1 原始数据 (source of truth)

```
subjects/
└── {subject_id}/
    └── benchmark-records/
        └── records.jsonl        ← Append-only, 所有平台/设备/模式
```

### 2.2 根存储 (docs/benchmark — 最新最全快照)

```
docs/benchmark/
├── README.md                    ← 说明: 此目录由工具自动生成
├── dashboard.html               ← 最新 dashboard (自动更新)
├── overview.json                ← 所有 subject × 所有模式的最新汇总
└── subjects/
    ├── BenchArithmetic.json     ← 该 subject 全维度最新记录
    ├── BenchAllocation.json
    └── ...
```

### 2.3 生成规则

- `docs/benchmark/subjects/{id}.json` = 该 subject 所有 (mode × platform × device) 的最新一条记录
- `docs/benchmark/overview.json` = 所有 subject 的最新 Windows 数据汇总 (用于总览表)
- `docs/benchmark/dashboard.html` = 嵌入 overview.json 数据的自包含 HTML

---

## 3. Record Schema

### 3.1 单条记录 (records.jsonl 每行)

```json
{
  "runId": "20260411-142300-BenchArithmetic-native-iphone14pro",
  "subject": "BenchArithmetic",
  "mode": "native",
  "platform": "ios-arm64",
  "device": {
    "id": "ios-arm64-apple-a16",
    "name": "iPhone 14 Pro (Apple A16)",
    "os": "iOS 17.2.1",
    "cpu": "Apple A16",
    "arch": "arm64",
    "isSimulator": false
  },
  "recordedAt": "2026-04-11T14:23:00+08:00",
  "gitCommit": "df60f22",
  "gitBranch": "main",
  "metrics": {
    "meanDurationMs": 3.2,
    "minDurationMs": 2.9,
    "maxDurationMs": 3.8,
    "stdDevMs": 0.2,
    "opsPerSecond": 312500,
    "sampleCount": 10,
    "iterationsPerSample": 10000
  }
}
```

### 3.2 device.id 生成规则

格式: `{platform}-{cpu_normalized}`

| 平台 | 生成方式 | 示例 |
|------|---------|------|
| Windows | `windows-x64-{cpu}` | `windows-x64-intel-core-i9-12900k` |
| macOS | `macos-arm64-{cpu}` | `macos-arm64-apple-m3-pro` |
| Android 真机 | `android-{abi}-{cpu}` | `android-arm64-snapdragon-8-gen2` |
| Android 模拟器 | `android-sim-{abi}-{cpu}` | `android-sim-x86-64-intel-core-i9` |
| iOS 真机 | `ios-arm64-{chip}` | `ios-arm64-apple-a16` |
| iOS 模拟器 | `ios-sim-{arch}-{cpu}` | `ios-sim-arm64-apple-m3-pro` |

CPU 名称规范化: 全小写，空格转 `-`，去除特殊字符

### 3.3 device.name 格式

`{设备型号} ({CPU型号})`

示例:
- `iPhone 14 Pro (Apple A16)`
- `Pixel 8 (Google Tensor G3)`
- `Windows PC (Intel Core i9-12900K)`
- `MacBook Pro 14" (Apple M3 Pro)`
- `iPhone 15 Pro Simulator (Apple M3 Pro)`

---

## 4. 设备信息自动检测

### 4.1 各平台检测方案

`build/toolchains/run/testing/device_detector.py`

**Windows:**
```python
# OS: platform.system() + platform.version()
# CPU: platform.processor()  →  "Intel64 Family 6 Model 158 Stepping 13, GenuineIntel"
# 或 wmic cpu get Name  →  "Intel(R) Core(TM) i9-12900K CPU @ 3.20GHz"
import subprocess
cpu = subprocess.check_output("wmic cpu get Name /value", shell=True)
# → "Name=Intel(R) Core(TM) i9-12900K CPU @ 3.20GHz"
```

**macOS:**
```python
# CPU: sysctl -n machdep.cpu.brand_string
# → "Apple M3 Pro"
import subprocess
cpu = subprocess.check_output(["sysctl", "-n", "machdep.cpu.brand_string"]).decode().strip()
```

**Android (via adb):**
```python
# 设备型号: adb -s {serial} shell getprop ro.product.model
# CPU: adb -s {serial} shell getprop ro.hardware  或  getprop ro.product.board
# ABI: adb -s {serial} shell getprop ro.product.cpu.abi
# 是否模拟器: adb -s {serial} shell getprop ro.build.characteristics → "emulator" or ""
```

**iOS 真机 (via xcrun):**
```python
# xcrun devicectl list devices --json
# → { "result": { "devices": [{ "deviceProperties": { "name": "iPhone 14 Pro", "hardwareModel": "iPhone15,2", "osVersionNumber": "17.2.1" }, "hardwareProperties": { "cpuType": { "name": "arm64e" } } }] } }
```

**iOS 模拟器:**
```python
# xcrun simctl list devices --json
# → { "devices": { "com.apple.CoreSimulator.SimRuntime.iOS-17-2": [{ "name": "iPhone 15 Pro", "udid": "...", "state": "Booted" }] } }
# CPU = 宿主机 CPU (macOS sysctl)
# isSimulator = True
```

### 4.2 检测结果缓存

检测到的设备信息缓存到 `artifacts/.device-cache.json`，避免每次运行都重新检测：
```json
{
  "cachedAt": "2026-04-11T14:00:00+08:00",
  "device": { "id": "...", "name": "...", ... }
}
```

---

## 5. Dashboard 页面设计

### 5.1 生成方式

```python
# benchmark_dashboard_generator.py
def generate_dashboard(repo_root: Path, output_path: Path):
    # 1. 扫描所有 subjects/*/benchmark-records/records.jsonl
    # 2. 解析全部记录，构建内存数据集
    # 3. 计算 latest per (subject, mode, platform, device)
    # 4. 序列化数据为 JSON 字符串
    # 5. 注入 HTML 模板: <script>window.BENCHMARK_DATA = {...};</script>
    # 6. 写入 output_path
```

### 5.2 三个 Tab

#### Tab 1: 总览 Overview

```
筛选栏: 平台 [Windows x64 ▼]  指标 [meanDurationMs ▼]

Subject            │ C# (.NET) │ AOT (C++) │ Interpreter │ AOT/C#  │ HU/C#
───────────────────┼───────────┼───────────┼─────────────┼─────────┼──────
BenchArithmetic    │ 12.3 ms   │  2.1 ms   │  156.7 ms   │  5.9x ✓ │ 12.7x ✓
BenchAllocation    │ 24.5 ms   │  8.2 ms   │  312.4 ms   │  3.0x ✓ │ 12.7x ✓
BenchDispatch      │  8.1 ms   │  1.1 ms   │  (no data)  │  7.4x ✓ │  --
BenchGameLoop      │ 45.2 ms ⚠ │ 12.3 ms   │  (no data)  │  3.7x ✓ │  --

图例:
  ✓ = 达标 (AOT≥2x / HU≤50x)   ✗ = 未达标   ⚠ = 数据 >7天未更新   (no data) = 尚未记录

默认排序: Windows x64 最新数据
切换平台: 下拉选择其他平台 (有数据的平台才显示)
```

#### Tab 2: Subject 详情

```
Subject [BenchArithmetic ▼]

──── 模式对比 (最新记录) ─────────────────────────────────
平台/设备 [Windows x64 - Intel i9 ▼]

  C# (.NET)    ██████████████████████  12.3 ms
  AOT (C++)    ███                      2.1 ms
  Interpreter  ████████████████████████████████████████  156.7 ms

  AOT 比 C# 快 5.9x   ·   解释器比 C# 慢 12.7x

──── 趋势图 ──────────────────────────────────────────────
模式 [native ▼]   设备 [全部叠加 ▼]

ms │
20 │ ╲  Windows x64
10 │  ──────────────────────────────  Windows
 5 │
 3 │    ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─  iPhone 14 Pro
 0 └──────────────────────────────────────→ 最近 20 次
    4/01  4/03  4/05  4/07  4/09  4/11

──── 设备横向 ─────────────────────────────────────────────
模式 [native ▼]

设备                              │ meanMs │ ops/sec │ 最后更新
Windows x64 (Intel i9-12900K)    │   2.1  │  476K   │ 2026-04-11
macOS ARM64 (Apple M3 Pro)        │   1.8  │  556K   │ 2026-04-10
iPhone 14 Pro (Apple A16)         │   3.2  │  312K   │ 2026-04-11
iPhone 15 Simulator (Apple M3)    │   5.8  │  172K   │ 2026-04-09 ⚠
Android Pixel 8 (Google Tensor G3)│   2.9  │  344K   │ 2026-04-10
```

#### Tab 3: 设备对比

```
Subject [BenchArithmetic ▼]   模式 [native ▼]

雷达图: 各设备相对 Windows x64 的归一化 ops/sec (1.0 = Windows 基线)

  Windows x64     ████████████  1.00x
  macOS M3 Pro    █████████████ 1.17x  ← M3 比 i9 快 17%
  iPhone 14 Pro   █████████     0.66x
  Pixel 8         ██████████    0.72x
  iPhone 15 Sim   █████         0.36x  ← 模拟器性能折扣
```

### 5.3 新鲜度标注规则

| 条件 | 显示方式 |
|------|---------|
| 数据 < 1 天 | 正常，无标注 |
| 数据 1-7 天 | 灰色小字 "4/09" |
| 数据 > 7 天 | ⚠️ 橙色 + tooltip "上次记录于 N 天前" |
| 三方数据时间差 > 3 天 | 总览表对比列显示 `~` 前缀 + tooltip |

---

## 6. 自动化流程

每次 `run benchmark` 完成后自动触发 dashboard 更新：

```python
# benchmark.py dispatch() 末尾
if record_mode:
    benchmark_records.append(repo_root, record)       # Step 1: 写入 records.jsonl
    benchmark_dashboard_generator.update_docs(         # Step 2: 更新 docs/benchmark/
        repo_root,
        subject_id=subject_id
    )
    print(f"✓ Dashboard updated: docs/benchmark/dashboard.html")
```

`update_docs()` 执行:
1. 重新计算该 subject 的最新记录 → 写 `docs/benchmark/subjects/{id}.json`
2. 重新计算 overview → 写 `docs/benchmark/overview.json`
3. 重新生成 HTML → 写 `docs/benchmark/dashboard.html`

---

## 7. docs/benchmark/ 文件规格

### overview.json

```json
{
  "generatedAt": "2026-04-11T15:00:00+08:00",
  "defaultPlatform": "windows-x64",
  "subjects": {
    "BenchArithmetic": {
      "windows-x64-intel-core-i9-12900k": {
        "managed":     { "meanDurationMs": 12.3, "recordedAt": "...", "gitCommit": "..." },
        "native":      { "meanDurationMs":  2.1, "recordedAt": "...", "gitCommit": "..." },
        "interpreter": { "meanDurationMs": 156.7,"recordedAt": "...", "gitCommit": "..." }
      }
    }
  }
}
```

### subjects/{id}.json

```json
{
  "subject": "BenchArithmetic",
  "generatedAt": "2026-04-11T15:00:00+08:00",
  "latestByDimension": {
    "windows-x64-intel-core-i9-12900k": {
      "managed":     { "metrics": {...}, "recordedAt": "...", "gitCommit": "..." },
      "native":      { "metrics": {...}, "recordedAt": "...", "gitCommit": "..." },
      "interpreter": null
    },
    "ios-arm64-apple-a16": {
      "native": { "metrics": {...}, "recordedAt": "...", "gitCommit": "..." }
    }
  }
}
```

---

## 8. 新增文件清单

| 文件 | 用途 |
|------|------|
| `build/toolchains/run/testing/device_detector.py` | 各平台设备信息自动检测 |
| `build/toolchains/run/testing/benchmark_records.py` | JSON Lines append/query 逻辑 |
| `build/toolchains/run/testing/benchmark_dashboard_generator.py` | HTML + JSON 生成 |
| `build/toolchains/run/testing/templates/benchmark-dashboard.html` | Dashboard HTML 模板 |
| `build/toolchains/run/commands/benchmark.py` | CLI 入口 (record + dashboard 子命令) |
| `contracts/artifacts/v0/schemas/benchmark-record.schema.json` | Record 字段 schema |
| `docs/benchmark/README.md` | 说明: 此目录由工具自动维护 |
| `docs/benchmark/dashboard.html` | 自动生成，不手动编辑 |
| `docs/benchmark/overview.json` | 自动生成 |
| `docs/benchmark/subjects/*.json` | 每个 subject 自动生成 |
| `subjects/*/benchmark-records/records.jsonl` | 各 subject 原始记录 |

---

## 9. CLI 命令一览

```bash
# 跑 benchmark 并自动记录 + 更新 docs/benchmark/
run benchmark --subject BenchArithmetic --mode native --record

# 仅重新生成 dashboard (不跑 benchmark)
run benchmark --dashboard

# 生成并用系统默认浏览器打开
run benchmark --dashboard --open

# 查看某 subject 最新记录摘要
run benchmark status --subject BenchArithmetic

# 查看所有 subject 最新状态 (CLI 表格)
run benchmark status --all
```
