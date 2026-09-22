# CI 红灯治理 — 交接文档

> 日期：2026-09-22
> 主检出 main：`abd3ec4fd`（PR #55 合并点）
> 本轮 PR：#49–#55（7 个）
> 文档目的：**接续者从"第 11 条红线"开始**，其余均已闭环

---

## 一、快速上手：三条命令看清全局

```bash
# 1. 有没有注册坏的 workflow（判据：name == path）
gh api repos/PolarisWang/booming-il2cpp/actions/workflows --jq '.workflows[]|select(.name==.path)'

# 2. 各 workflow 在 main 上的最新结论
gh api "repos/PolarisWang/booming-il2cpp/actions/runs?per_page=60&branch=main" \
  --jq '[.workflow_runs[]]|sort_by(.created_at)|reverse|unique_by(.path)|.[]|"\(.conclusion // .status)  \(.created_at[5:16])  \(.path|split("/")|last)"'

# 3. 某次失败的具体步骤与日志
gh api repos/PolarisWang/booming-il2cpp/actions/runs/<RUN_ID>/jobs \
  --jq '.jobs[]|{name,conclusion,failed:[.steps[]|select(.conclusion=="failure")|.name]}'
```

> ⚠️ 仓库的 main 是**受保护分支**，`git push origin main` 会被拒。
> 一律走 PR。若本地 main 落后，用 `git update-ref refs/heads/main <sha> <old-sha>` 快进（不要 checkout，主检出常有其他会话的 WIP）。

---

## 二、已闭环的 10 条红灯

| # | 症状 | 根因 | PR |
|:--|:-----|:-----|:---|
| 1 | `foundation-dll-regression` Configure CMake 失败 | Linux 分支漏传 `-DROADMAP0_PRESET_TARGET` → `CMakeLists.txt:925` FATAL_ERROR | #50 |
| 2 | `jit-baseline` Restore 失败 `NU1301` | `testing/nuget.config` 声明本地源 `_packages\`，该目录被 gitignore 且 CI 不生成 | #50 |
| 3 | `ci.yml` step7：7 个 `test_bootstrap_*` LNK2019 | `WITHOUT_CODEGEN_STUB` 抑制默认桩，而 bootstrap 桩缺 `kChaosExceptionTypes` | #51 |
| 4 | `ci.yml` step8：`ci-run-all.sh` 秒退 | 脚本指向 `9951957db` 已删除的旧验证管线路径 | #51 |
| 5 | `codegen-regression` step9：89 个 snapshot 失败 | 漏 `Build FixtureAssembly` 步骤 | #51 |
| 6 | `codegen-regression` step9：残留 85 个 mismatch | 基线落后于 `pps2-4*` 链接性改写（`static constexpr`→`extern const`） | #52 |
| 7 | `foundation-dll-regression` Build：`numa.h` not found | 缺 `libnuma-dev` 安装步骤 | #53 |
| 8 | `jit-baseline` Configure：`Unknown CMake command "chaos_compile_only_smoke"` | `CMakeLists.txt` 的 `linux-x64-reference` 分支漏 `include(cmake/chaos_native_test.cmake)` | #53 |
| 9 | `ci-framework.yml` 每次 push 零-job 失败 | 孤儿 workflow（无调用方），其调用方已删。**病因未完全确证** | #54 |
| 10 | `release.yml` 每次 push 零-job 失败（381 次） | `uses:` 字段含 `${{ vars.* }}` → 注册期无法解析 | #55 |

### 值得反复用的一条判据（#10 的副产品）

**`name == path` ⟺ 该 workflow 注册失败**，后果是每次 push 零-job 失败。
病因之一：**`uses:` 不能含 `${{ vars.* }}`**（`runs-on`/`with:` 里的无害）。

详见记忆：`gha-uses-cannot-contain-vars-expression`、`workflow-registration-name-equals-path`。

---

## 三、🔴 未解决：第 11 条红线

> **2026-09-22 更新（PR #56 后）：** 前三层已修复并合并（见 §3.1）；第 11 条已收敛为 **单个残留 target**，根因悬在「INTERFACE 目标不在 `--start-group` 内」与「疑似自含环」两个候选之间，**未钉死**。接续者从这里开始。

### 症状

`foundation-dll-regression` → `foundation-dll (ubuntu-latest)` → **`Build Native Libraries`** 失败。

```
/usr/bin/ld: undefined reference to `chaos::il2cpp::diagnostics::DbgShouldStopAtCurrentPosition(int)'
/usr/bin/ld: undefined reference to `chaos::il2cpp::diagnostics::DbgClearFrameSnapshot()'
/usr/bin/ld: undefined reference to `chaos::il2cpp::method_table::PopulateMethodTableFromHotpatch()'
/usr/bin/ld: undefined reference to `ChaosBCryptOpenAlgorithmProvider'
... (40 条)
```

- **原 4 个失败 target**：`test_gc_bit_utils`、`test_gc_profile_stats`、`test_jit_il_smoke`、`test_jit_native` —— **本轮已全绿**（见 §3.1）
- **当前唯一残留**：`chaos_method_replacement_smoke`（`tests/contracts/native/hot-update/`），同样的 ~40 条 undefined reference
- 三组符号：`diagnostics::Dbg*`、`method_table::Populate*/Resolve*`、`ChaosBCrypt*`

**注意**：本表「4 个 target 失败 / 28 个 target 成功」已被推翻 —— 真实情况是**大多数 target 成功、极少数失败**，且失败逐层暴露（先链接、再编译）。测试到 22% 才撞上唯一残留。

### 3.1 本轮已修复并合并（PR #56）

| commit | 修复 | CI 验证 |
|---|---|---|
| `d7d7be4eb` | `test_gc_profile_stats` → `LIBS ${CHAOS_GC_LIBS}`（全量，含 interpreter/bootstrap/jit 等） | ✅ `Built target test_gc_profile_stats` |
| `597242b13` | `foundation-dll-regression.yml` paths 补 `tests/**`/`cmake/**` —— 改测试 CMake 会触发 Linux 验证 | —（基础设施） |
| `ded6772dd` | `jit_unwind_test.cpp` 的 Win64-only `using` 加 `#if defined(_WIN64)` 守卫 | ✅ `Built target test_jit_unwind` |
| `39c1c152d` | `jit_seh_handler_internal.cpp` 自旋锁非 MSVC 分支用 `__atomic_exchange_n`（同 test 文件两处） | ✅ `Built target test_jit_seh_handler_internal` |
| （附带） | workflow 加 `permissions: {issues: write, pull-requests: write}` —— PR 评论不再假失败 | ✅ `Post PR Comment` success |

### 3.2 已证伪/已澄清（勿重复）

| 假设 | 证伪依据 |
|:-----|:---------|
| `linux_platform_stubs.cpp` 不在 CMake | 在 `runtime-core/CMakeLists.txt:341` 的 Linux 分支 |
| 该文件未被编译 | CI 日志确认已编译（`linux_platform_stubs.cpp.o`） |
| 符号定义缺失 | 该文件确实定义了 `Dbg*`；`crypto_stubs_posix.cpp:226-347` 定义了 `ChaosBCrypt*` |
| 命名空间/签名不匹配 | 逐个核对，一致 |
| `chaos_runtime_core` 未参与链接 | 在链接列表中，且 `libchaos_debugger.a` 也被构建了 |
| 静态库单遍扫描顺序问题（全 target 通用） | **部分成立但不充分**：`test_gc_profile_stats` 确因残缺 LIBS 失败，但 `chaos_method_replacement_smoke` 用**全量默认** LIBS 仍失败 |
| 平台分支遗漏 | `CMakeLists.txt` 的 `linux-x64-reference` 分支完整 |
| `--start-group 内不包含 INTERFACE 展开` | **候选根因但未钉死** —— 能解释 5 个案例中的 5 个，但解释不了对照：`chaos_hotupdate_verification_test` 用**相同**默认 INTERFACE 配置却通过 |
| 仅 Windows 专属 target 被误注册 | `jit_unwind_test`/`jit_seh_handler_internal` 确实是 Windows 专属代码在 Linux 编译（**本轮修掉**）；但 `method_replacement_smoke` 是平台中立源码 |

### 3.3 🔑 下一步：一次性取证（强烈建议，替代继续猜）

**取 `build/native/.../tests/contracts/native/hot-update/CMakeFiles/chaos_method_replacement_smoke.dir/link.txt` 的实际链接命令行**，与 `chaos_hotupdate_verification_test.dir/link.txt`（**通过**的同类 target）对比。两者**唯一有意义差异**应在：
- `--start-group` 的边界（INTERFACE `chaos_test_libs_v0` 是否被展开进 group）
- 或 `collect2` 传给 `ld` 的库顺序

**判读**：
- 失败者 group 内缺 `chaos_interpreter`/`chaos_bootstrap` → 根因= INTERFACE 目标不透传给 start-group，修法=工厂里把 `_libs`（INTERFACE）展开成真实库列表再拼 group（**我一度实现过，但由于解释不了对照案例而回退**）
- 失败者与通过者 group 边界**一致** → 根因在目标自身的 TU 引用（谁先引用不了解释不了），下一步看 `nm -u` 对比两 target 对象

**此诊断已在 workflow 里存在**：`Diagnose missing symbols` 步骤（`if: failure()`，nm 判读 `libchaos_runtime_core.a` 中三组符号，**已确认全部在库内** —— 见 run `35729846929`）。把 link.txt 的 log 也加进去一行即可复用。

**对比过状态**：该 workflow 历史 `success=0`（id 289258742，300 次运行全 failure），link 失败自 09-10 起每次 main push 都在 —— 属「从未在 Linux 跑通过」而非新回归（下层编译错误一直遮蔽到链接）。

### 备选：确认这是否是"一直如此"

`foundation-dll-regression` 在 Linux 上**历史上的实际执行次数可能极少**（它的失败长期被上游步骤遮蔽）。
可查该 workflow 是否在**更早的 commit**（如 09-10 之前）成功跑完过 `Build Native Libraries`，
以判断这是**新回归**还是**从未工作过**。

---

## 四、另一类遗留（低危，未处理）

### 4.1 `.github/workflows/triggers/` 从不生效

GitHub **不扫描 `.github/workflows/` 的子目录**，所以 `triggers/{pr,release,nightly}.yml` 全部未被注册。

- **后果**：PR 上不跑 `pipeline.yml`（后者只有 `on: push` + `workflow_call` + `workflow_dispatch`）
- **但不必修**：已有 7 个独立 workflow 覆盖 PR 触发（`aot-regression`/`arm64-jit-ci`/`ci`/`codegen-regression`/`foundation-dll-regression`/`hybrid-regression`/`jit-baseline`），启用 `triggers/` 会导致**双重触发**
- **结论**：这是架构演进的遗留物，建议**清理并说明**，而非启用

附带：`.github/pipelines/pr.yml` 注释指向 `.github/workflows/stages/<name>.yml`，而实际 stage 文件在**顶层** —— 目录重构后注释/路径未同步。

### 4.2 3 个僵尸 workflow 条目（已 disable）

`release-build.yml`、`probe-copy-release.yml`、`probe-c-uses.yml` —— 我实验的副产物，
均 `state=disabled_manually`，**不影响 main**。

**清理机制（实测）**：
- 文件在 default branch 上被删 → 条目清理 ✓
- 删分支 ✗ / 改文件 ✗
- 兜底：`PUT /repos/{o}/{r}/actions/workflows/{id}/disable`

### 4.3 `hybrid-regression.yml` 的 paths 指向不存在的文件

其 `pull_request.paths` 含 4 个**在 git 全历史中从不存在的** `.cpp`：

```
testing/src/native/jit/test_hybrid_e2e.cpp / test_hybrid_mode.cpp / test_hybrid_race.cpp / test_jit_hybrid_mode.cpp
```

该 workflow 因此**从未被触发过**。需先搞清 hybrid 测试的真实归属再修。

---

## 五、贯穿本轮的根因线索：`0f661d621`

多条红灯（#2、#4、#9）同源于这个 commit：

```
0f661d621  "fix: add missing #include <cstdio> in pal_eh_posix.cpp"
```

**它的 message 只提一个 `#include`，实际替换了 5570 个文件、删除 1071 个文件（含 28 个 workflow）。**

来历记录在 `c128020d6`：一次 `--force-push` 导致 main 状态丢失，之后从 `ci-r7-clean` 分支"恢复"，混入了版本不匹配的文件。

**这类"恢复式"提交的风险是隐性且长期的** —— 每修一层才露出下一层。
若要彻底清理，建议核对 `c128020d6` 恢复的 33 个文件是否都与当时的 main 兼容。

---

## 六、方法论教训（本轮最贵的经验）

**我从"读代码猜病因"切换到"构造受控实验"，是本轮唯一的转折点。**

- `release.yml` 的病态，我**先后 5 次从文件内容推测**（YAML 语法 / job 结构 / 行尾 CRLF / 编码 / 陈旧注册条目+改名），**全部被证伪**
- 最终靠 **A/B/C 三组探针**（唯一变量 = `vars.` 的位置）**一轮定位**

**另有两类反复出现的自我陷阱**，接续时请警惕：

1. **把环境 artifact 当证据**：`wc -c` 报 18003 vs blob 17634，我一度当成"文件被污染"，
   实为 `core.autocrlf=true` 的行尾转换（369 行 → +369 字节）。**核对用 `git hash-object`，别用 `wc -c`。**
2. **读错版本**：本地主检出 ≠ CI 跑的 commit。**一律 `git show <sha>:<path>`。**

---

## 七、工作纪律备忘

- **禁止 `git stash`**（项目红线）
- 提交前核对暂存区：`git diff --cached --name-only`
- **绝不 `git add -A`** —— `tmp_v7/`、`tmp_verify/` 是**已跟踪**文件且含大量 autocrlf 空白差异
- worktree 用 `git worktree add`，退出用 `ExitWorktree`；`rm` 报 busy 说明有并发会话
- 三轮修不好同一问题 → 停下来取证，别继续猜
