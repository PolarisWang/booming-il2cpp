# System.Security.Cryptography 后续建议执行记录 (v1)

> 日期：2026-08-25 · 关联 closeout：20260824-crypto-88to100-attribution-closeout-v1-01.md
> 目的：复核 closeout 归档的「后续建议（非必做）」各项，记录最终处置。

## 待办项总览

| ID | 建议 | 状态 | 处置 / commit |
|----|------|------|--------------|
| H1 | chunk codegen/lib 与 SDK lib 做版本指纹校验进 CI（防 LNK2019） | OPEN | 未实现，人工/文档兜底 |
| H2 | codegen/lib + subjects 陈旧 pageN.cpp 清理机制进 build 前置（防 C2660） | OPEN | 未实现，人工删除 |
| H3 | JIT post-build salvage 机制（rc!=0 时回收 fresh 产物） | DONE | 653dcfbf9 |
| H4 | NuGet real-user env 要求（APPDATA/ProgramFiles 不能剥） | DONE | 归档备注，非代码改动 |

## 逐项复核

### H3 · JIT salvage（DONE）
- build.py 新增 `_finalize_jit_copy`：当 TPG 子进程 rc!=0 但已产出
  `build_jit/RelWithDebInfo/chaos_entry.exe` + 全新 `aot-core-ir.jdata` 时，回收进
  `native_dir` 而非回退 06-20 stale entry-jit.exe。
- 验证：隔离调用选中 08-25 build 产物复制成功；py_compile + verification unit 8 passed；
  完整标准管道重跑 sec2 → JIT 06:48 fresh 23.9MB → aot/jit 均 1287/1287 → Pipeline complete passed。

### H4 · NuGet real-user env（DONE）
- harness 剥离 APPDATA/ProgramFiles → NuGet `XPlatMachineWideSetting` 的 `Path.Combine(null,...)`
  崩溃（"value cannot be null (Parameter path1)"）。
- spawn 补回 real-user env（USERPROFILE/APPDATA/HOME/ProgramFiles 等）后标准 combination
  /dotnet build 0 errors。属环境安全边界要求，归档即足够，无需代码改动。

### H1 · SDK lib 指纹校验（OPEN）
- 本轮 LNK2019：chunk 本地 codegen/lib 的 lib 为 06-15/06-19 旧 9 参签名，
  SDK sdk/windows-x64-reference 为 08-24 新 10 参；scriban/interop_stubs 按新签名调用 → unresolved external。
- 现状：build.py 靠 deps-mtime 失效 + 人工同步兜底；hephaestus cache key 不含 chunk 本地 lib，
  指纹校验未实现。
- **建议实现**（后续）：TPG emit 时对 chunk codegen/lib 与 SDK lib 做 mtime/SHA-256 指纹比对，
  不一致则在 build 前置强制 refresh。

### H2 · 陈旧 pageN.cpp 清理（OPEN）
- 本轮 x509 C2660：chunk 本地 subjects/ 残留 06-19 旧 pageN.cpp（0 参旧签名）被
  cmake `file(GLOB subjects/*.cpp)` 编入 fresh build，与新版 1 参声明冲突 → C2660。
- 人工删除陈旧 pageN.cpp 后 fresh 重建通过（274/274）。
- **建议实现**：build.py 在 subjects 重建后，删除 `subjects/*.generated.pageN.cpp` 中
  mtime 早于本次 codegen 的分页文件，杜绝 glob 误编。

## 结论
- crypto 三 chunk（sec/sec-2/x509）标准 combination 路径 fresh 3/3 passed，aot/jit 精确对齐，
  12 项缺口无存活项——归档主结论成立。
- 剩余待办为 H1/H2 两项**防御性加固**（版本指纹 + 陈旧分页清理），不影响当前正确性，
  归入后续 CI 硬化，非阻塞。

## 架构映射

- 涉及层：Python（verification/stages/build.py）+ TPG（CppProjectEmitter）。
- 无跨层 write_text 违规：Python 层改动仅碰 .py/.md。