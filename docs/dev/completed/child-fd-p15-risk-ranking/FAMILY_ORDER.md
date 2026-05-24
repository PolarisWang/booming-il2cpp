# FAMILY_ORDER.md — 基础排序执行顺序

> 基于 Risk_Score = Failure_Severity × Complexity_Multiplier 排序。
> Tier 划分：P0 ≥ 7.5, P1 ≥ 5.0, P2 ≥ 3.0, P3 < 3.0

## P0 — 最高风险（6 families）

| Rank | Assembly | Family | Score | 失败阶段 | 方法数 | 说明 |
|:----:|----------|--------|:-----:|----------|:-----:|------|
| 1 | System.Text.Json | document-element | **9.0** | audit | 69 | p1_lowering VIOLATION：0/138 有 lowering，69 方法全部未优化 |
| 2 | System.Formats.Asn1 | tag-model | **7.5** | codegen | 19 | KI-001：chaos_* macros 未定义 |
| 3 | System.Private.CoreLib | buffer-memory | **7.5** | codegen, audit | 10 | patch entry _exitCode 重复定义（编译错误） |
| 4 | System.Private.CoreLib | convert-char | **7.5** | codegen | 18 | KI-001 |
| 5 | System.Private.CoreLib | custom-generics | **7.5** | codegen | 6 | KI-001，无 entry.exe |
| 6 | System.Private.CoreLib | garbage-collection | **7.5** | codegen, fact | 7 | GC.LatencyMode 不在运行时中 → subjects 编译失败 |

## P1 — 高风险（2 families）

| Rank | Assembly | Family | Score | 失败阶段 | 方法数 | 说明 |
|:----:|----------|--------|:-----:|----------|:-----:|------|
| 7 | System.IO.Compression.Brotli | decoder | **5.0** | codegen | 3 | KI-001 |
| 8 | System.Private.CoreLib | interface-dispatch,pinvoke-dllimport,primitive-numeric-conversions-core | **5.0** | codegen, jit_codegen, fact | 0 | 三合一复合 slug，特殊注册 |

## P2 — 中风险（0 families）

（reports/ 目录不是真实 family，已排除）

## P3 — 低风险（4 families）

| Rank | Assembly | Family | Score | 失败阶段 | 说明 |
|:----:|----------|--------|:-----:|----------|------|
| 9 | --family | pinvoke-dllimport | **2.0** | preflight | 缺 capability-family-contract.json |
| 10 | --family | threading-monitor-interlocked | **2.0** | preflight | 缺 capability-family-contract.json |
| 11 | System.Runtime.Serialization | surrogates-selection | **2.0** | preflight | 缺 capability-family-contract.json |
| 12 | System.Security.Claims | windows-identity | **2.0** | preflight | 缺 capability-family-contract.json |

## 系统性依赖修复

以下修复必须先于 Phase 2 逐 family 攻坚完成：

1. **KI-001**: codegen ↔ runtime headers 接口对齐（影响 5/12 未通过 family）→ 阻塞 Phase 2
2. **GC.LatencyMode**: 补 CRAG GC 接口或修改 subjects 移除引用（影响 1 family）
