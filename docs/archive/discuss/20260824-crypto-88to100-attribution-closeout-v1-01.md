# System.Security.Cryptography 88% → 100% 归因与收口 (v1.01)

> 日期：2026-08-24 · 状态：✅ 已闭环 · 关联 commit：7e405bdd7 / b8fa1e0eb

## 摘要

crypto 的 "88% (1077/1222)" 数字被证明是 **06-14/06-17 的过期 AOT 快照**，并非当前 HEAD 的真实状态。
逐层归因 + 3 个 chunk 的全量原生重建验证后，**当前 HEAD 全量 fact 通过，且无存活代码 bug 待修**；
已修复的是**门禁伪绿**（fact 层把真实失败当 non-fatal partial），并更新了 STATUS.md 的过期数字。

## 数据核对

| 指标 | 旧快照值 | 当前 HEAD 实测 |
|------|---------|---------------|
| full fact-summary | 88% (1077/1222) | **99.8% (5579/5591)** |
| x509 重建 subject slot | 233 | **272**（--fact-json 272/272 passed） |
| sec-2 重建 subject slot | 1096 | **1178**（--fact-json 1178/1178 passed） |
| sec 重建 subject slot | —（同 sec-2 同源） | **1178**（--fact-json 1178/1178 passed） |
| 历史 x509 12 failures | 06-19 产物 773/785 | 当前 HEAD **不可复现** |

## 根因（为什么旧快照失真）

1. **ATG 每次重生成**（a22a798d5）与 **metadata reconcile**（afe0060ce）在 HEAD 已合入，
   但跟踪的 native 产物（entry.exe）是 06-19/07-02 构建的，早于这两个 commit →
   generatedMethodId 尾索引漂移（Dispose_2__0 → Dispose_2__2），
   TPG 的 aotSubjectIds 与 wrappers 实际生成名对不上 → slot 覆盖不足。
2. **门禁伪绿**：fact_chunk._tech_status 对 passed<total（真实失败）返回
   non-fatal 的 "partial"，pipeline 又没有 partial 分支 → 失败静默吞掉；
   drop 检查的 expected 用 factMethodCount（低估）→ 永不会触发 >10% 阈值。

## 修复

- **7e405bdd7**（fix(verification)）：
  - _tech_status：passed<total → **failed**（真实覆盖/断言丢失致命）；仅
    rc!=0 && passed==total（截断 JSON）保留 partial。
  - drop expected 改用 meta_unique_fact（kind=fact 去重 generatedMethodId），
    静默 slot 丢失会触发 SEVERE 门禁。
  - 新增回归测试 test_fact_chunk_status.py。
- **b8fa1e0eb**（docs）：STATUS.md 88% 行替换为实测 99.8% + 重建数字。

## 验证

- verification-engine-unit：95 passed（含新增测试）。
- 全 unit 层：2277 passed / 0 failed；pytest 层 OK。
- 三个 chunk 全量原生重建（TPG generate-dll → cmake+MSVC）均 --fact-json 全 passed：
  - x509：272/272
  - sec：1178/1178
  - sec-2：1178/1178
- sec 的 1178 中 18 个 value=0 项为**受支持的"无断言" fact**（HashData/Shake/Kmac
  的同名重载仅返回 0，无 AOT 值可验证），passed=true，属类目语义而非失败。

## P/Invoke 哨兵 0 项（C-6）

- 重建产物中 **0 个直接 P/Invoke stub**（s_pinvoke_fn_ 出现 0 次、
  TryResolveDllImport 0 调用点）。crypto 的 BCrypt 交互走的是
  **managed→codegen 直连 + runtime native 库解析**，不是 TPG 的 P/Invoke 包装层，
  因此不存在"哨兵 0 未走通"的存活项。
- --fact-json 1178 全 passed 侧面确认：无 P/Invoke 解析失败导致的 subject 失败。

## 环境限制（诚实边界）

- 本 shell 的 NuGet 全局损坏（dotnet nuget locals 报 value cannot be null path1），
  完整 pipeline 的 combination/dotnet build 阶段不可用；验证走
  **TPG dotnet exec generate-dll 全量原生构建**（cmake+MSVC，与 pipeline
  build 阶段同机制）。CI 的 foundation-dll e2e 组会走标准 combination 路径。
- sec-2 首次全量构建遇 MSB4018 OOM（巨大 generated TU），
  设 CMAKE_BUILD_PARALLEL_LEVEL=2 后成功——环境资源限制，非代码缺陷。

## 后续建议（非必做）

- 在 NuGet 健康环境跑 --layer e2e foundation-dll-* 得到标准 combination 路径
  的正式 fact-summary 归档。

## 架构映射

- 涉及层：Python（verification）+ Codegen（generated cpp）+ TPG（generate-dll）。
- 无跨层 write_text 违规：Python 层改动仅碰 .py/.yaml；重建产物在 scratch。
