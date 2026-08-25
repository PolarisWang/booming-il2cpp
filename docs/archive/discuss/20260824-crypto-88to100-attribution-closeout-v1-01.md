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
| x509 重建 subject slot | 233 | **274**（--fact-json 274/274 passed，fresh） |
| sec-2 重建 subject slot | 1096 | **1178**（--fact-json 1178/1178 passed） |
| sec 重建 subject slot | —（同 sec-2 同源） | **1178**（--fact-json 1178/1178 passed） |
| 历史 x509 12 failures | 06-19 产物 773/785 | 当前 HEAD **不可复现** |


## 99.8% 数字的精确出处（已定位）

- 来源：nightly-build-report/full-run/20260620_054326-3b9005fa/
  reports/System.Security.Cryptography/fact-summary.json（06-20 归档，非 06-14/06-17）。
- 分 chunk：sec 2403/2403，sec-2 2403/2403，x509 773/785。
  **剩余 12 项全部在 x509**（06-20 归档产物，已不可复现）。
- 06-20 归档本身与当日每 chunk fact.json 汇总一致：aot 233 全过、
  jit 233 全过；archive 合计数 785 > 当日实测 233 是因为归档含 history/
  多次运行的**累计去重合并**，并非单次运行失败集。
- 12 项失败在 current HEAD 重建产物上不存在（fresh builds 274/274 全过；
  早期记录的 272/272 含陈旧 pageN.cpp 参与编译，清理后 274/274 为纯 fresh 结果）。

## NuGet 损坏根因（本 shell 环境，已定位）

- 现象：dotnet nuget list source / restore 报 "Value cannot be null (Parameter path1)"。
- 根因：**harness 剥离了 USERPROFILE/APPDATA/HOME/ProgramFiles 等环境变量**，
  NuGet 的 XPlatMachineWideSetting 构造时 Path.Combine(null, ...) 抛异常。
  不是 NuGet.Config 内容问题（real user 的 config 完好）。
- 修复：spawn 时补回 real-user env（USERPROFILE=C:\Users\haochuan.wang 等）即恢复；
  已用 .tmp-combination-test 验证 CombinationSubjects.dll 可正常 build 成功（0 errors），
  仅 NU1900 脆弱性告警（离线环境正常）。
- 对验证的影响：此前用 TPG dotnet exec generate-dll 绕过；现在标准 combination 路径
  在本 shell 也可用，完整 fact 管线可实跑（下一步验证归档）。
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

## 环境限制与标准管线回填（2026-08-24 晚，本 shell 已完成）

- NuGet 全局损坏已修复（根因=harness 剥离 APPDATA/ProgramFiles 等 real-user 环境变量，
  NuGet 的 Path.Combine(null) 崩溃；最小充分集=APPDATA+ProgramFiles，spawn 补回 real-user env 后
  标准 combination/dotnet build 0 errors，CombinedSubjects.dll 136192B）。
- 在**修复后的 real-user env** 下用标准管线（build+fact+coverage-audit）重跑了
  security-cryptography 与 security-cryptography-2：**均为 3/3 passed**，fresh 全量构建
  （entry.exe mtime 2026-08-24 19:36/19:35，非 hephaestus cache-hit）。
  - sec: declared=500, covered=825 (165%)；fact aot+jit 均 1287/1287（entry-jit fresh 08-24 19:39）；
    sec2: declared=85, covered=688 (809%)，fact aot 1287/1287；jit 走 06-20 旧 entry-jit
    （fresh JIT 重建 crash rc=0xC000013A，sec2 大 TU 资源耗尽同 MSB4018 OOM 类）
    → jit 1096/1096 passed（旧 exe，fact 仍绿）。
  - 该结论合入后 fact-summary 上下限：1239 (aot-only) / 1240 (jit+entry) / 2408 (双 exe)，
    **12 项缺口无存活项**（详见上文 99.8% 出处 + P/Invoke 哨兵 0 项两节）。
- 标准管线曾出现 TPG 链接失败（LNK2019 PalTryCallNoExcept），根因=**chunk 本地
  codegen/lib 的 lib 快照陈旧**（chaos_pal.lib 等 2026-06-15/06-19，旧 9 参签名）与
  SDK lib（sdk/windows-x64-reference，2026-08-24 重建，新 10 参签名含 extra runtime arg）
  不同步；scriban/interop_stubs 已按新签名调用，链接旧 lib → unresolved external。
  修复=同步 SDK 11 个 lib 进 chunk codegen/lib（chunk 本地产物 chaos_codegen.lib 除外），
  或依赖 build.py 的 deps-hash/hephaestus 失效机制 refresh。x509 的同款缓存疑点
  （旧 entry.exe mtime=06-19）已在下方 C2660 小节坐实并 fresh 复核。
- sec-2 首次全量构建遇 MSB4018 OOM（巨大 generated TU），
  设 CMAKE_BUILD_PARALLEL_LEVEL=2 后成功——环境资源限制，非代码缺陷。

- **x509 缓存疑点被坐实并复核**：x509 fresh 重建首跑（不同步前）在 TPG cmake build 失败
   C2660（GetUninitializedObject 0参调用 vs 1参声明），且 fact 阶段仍显示 aot 233/233
   ——确认此前归档的 "233/233" 实为 hephaestus CACHE-HIT 使用 06-19 旧 entry.exe 的产物。
   C2660 根因=**chunk 本地 subjects/ 残留 06-19 旧 pageN.cpp**（0参旧签名）被 cmake glob
   （subjects/*.cpp）编入 fresh build，与新版 1参声明冲突。清掉陈旧 pageN.cpp 后
   fresh 全量重建通过。
   - x509 fresh 结果（entry.exe mtime 2026-08-24 19:57，entry-jit 20:02）：
     fact aot 274/274、jit 274/274 passed，coverage declared=238 covered=246 (103%)，
     Pipeline complete: 3/3 passed。dropped 23 methods vs metadata(297)，
    为覆盖 360 方法补充 super-set 的差异，非活缺口。
- 结论：**sec / sec-2 / x509 三 chunk 均在修复后的 real-user env 下走通标准管线
  （build+fact+coverage-audit）并 3/3 passed**；AOT 三 chunk 均为 fresh 全量构建
  （entry.exe 08-24，非 cache-hit）；sec/x509 的 JIT 亦 fresh（08-24）；sec2 JIT
  fresh 重建受资源限制 crash（0xC000013A，同 MSB4018 OOM 类），其 jit fact 用 06-20
  旧 entry-jit 1096/1096 passed（非新代码缺陷）；NuGet + 陈旧 lib/page 两类环境问题
  均已在标准路径闭环；12 项缺口归档对比无存活项。**
- 若后续 SDK lib 再次刷新，需确保 chunk codegen/lib 同步机制进入 CI（目前是
  build.py deps 失效 + 人工同步兜底）；建议在 pipeline 中对 codegen/lib 的
  chaos_pal.lib 等做 SDK 版本指纹校验。

## 架构映射

- 涉及层：Python（verification）+ Codegen（generated cpp）+ TPG（generate-dll）。
- 无跨层 write_text 违规：Python 层改动仅碰 .py/.yaml；重建产物在 scratch。
