# Foundation-DLL 命名空间分块管线

## 概述

从旧的按"family"（功能族）拆分方式，迁移到按 namespace 自动分块的管线。每个 chunk 包含一组 namespace，由 `manifest.py` 自动生成分区定义。

## 目录结构

```
testing/foundation-dll/
├── System.Private.CoreLib/
│   ├── _dll/
│   │   ├── dll-method-manifest.json   ← DLL 全部方法清单
│   │   ├── namespace-partition.json   ← namespace → chunk 分区定义
│   │   └── reports/
│   │       ├── latest/                ← 最新汇总
│   │       └── history/               ← benchmark 历史
│   ├── chunks/
│   │   ├── system/
│   │   │   ├── chunk.json
│   │   │   ├── managed/subjects/     ← subjects DLL + metadata
│   │   │   ├── native/               ← entry.exe
│   │   │   └── results/              ← stage 输出
│   │   ├── io/
│   │   └── ... (17 chunks)
│   ├── CMakeLists.txt
│   └── _assembly.json
```

## 管线阶段

| 阶段 | 范围 | 功能 |
|------|------|------|
| `manifest` | DLL 级 | 扫描 DLL 生成方法清单 + 分区定义 |
| `build` | chunk 级 | AutoTestGenerator → subjects DLL → TPG → entry.exe |
| `fact` | chunk 级 | `entry.exe --fact-json` → `results/fact.json` |
| `benchmark` | chunk 级 | `entry.exe --benchmark-all N` → `results/benchmark.json` |
| `hotupdate` | chunk 级 | `entry.exe --hotupdate` → `results/hotupdate.json` |
| `coverage-audit` | DLL 级 | 验证 chunk 声明方法 ⊆ subjects metadata |
| `aggregate` | DLL 级 | 收集所有 chunk results → `_dll/reports/latest/` |

## 使用方式

```bash
cd testing/foundation-dll

# 对单个 chunk 运行 coverage-audit
python -m verification.chunk_pipeline --chunk numerics --stages coverage-audit

# 运行 build + fact + coverage-audit
python -m verification.chunk_pipeline --chunk io-2 --stages build,fact,coverage-audit

# 全量运行
python -m verification.chunk_pipeline --all-chunks
```

## Chunk 生成

由 `verification/manifest.py` 完成：

```bash
# 扫描 DLL 生成 dll-method-manifest.json + namespace-partition.json
python verification/manifest.py --dll-path <path-to-dll>
```

分区策略：按 namespace 分组，每 chunk 最多 500 methods。

## AutoTestGenerator namespace 过滤

`AutoTestGenerator` 支持 `--namespace-filter` 参数，只扫描指定 namespace 的类型：

```bash
dotnet exec AutoTestGenerator.dll \
  --dll <target.dll> \
  --all-types \
  --namespace-filter "System.IO,System.IO.Enumeration" \
  --emit-metadata <path> \
  --chunk-slug io-2 \
  --output <outdir>
```

## 关键变更

- 删除了旧 family 目录结构（65 个按功能命名的目录）
- 删除了旧管线阶段文件（codegen.py, pre_verification_audit.py 等）
- 删除了 `_contracts/` 目录（信息内联到 namespace-partition.json）
- AutoTestGenerator → TPG 两步替代了 contract → family_entrypoint → codegen → TPG emit → CMake build 五步
