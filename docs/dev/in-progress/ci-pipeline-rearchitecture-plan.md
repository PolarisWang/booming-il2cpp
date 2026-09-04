# CI Pipeline 重构 — 完整开发计划

## 分类：Architectural — 架构级 / 全流程

**已确认的设计**：方案A（GitHub Actions 原生可复用），3 层架构（Stage 定义 / Pipeline 配置 / 调度器），3 个 Profile（pr / nightly / release），20 个 Stage 定义。

**执行路径**：roadmap（多阶段、多步、可验证）

---

## 任务总览

| 阶段 | 内容 | 预估时间 | 产出文件数 | 关键门禁 |
|------|------|---------|:----------:|---------|
| Phase 0 | 准备工作：建立目录结构 + 基础设施 | 0.5 天 | ~5 | stages/ 目录、triggers/ 目录、pipelines/ 目录 |
| Phase 1 | 核心 3 个 Stage 概念验证 | 1 天 | ~8 | PR 跑 3 个 stage，不跑旧 workflow |
| Phase 2 | 全量 20 个 Stage 定义 | 5 天 | ~20 | 每个 stage 独立可调用 |
| Phase 3 | 3 个 Profile 配置 + 触发入口 | 2 天 | ~8 | pr/nightly/release 各跑对应 stage 集 |
| Phase 4 | 旧 21 个 workflow 下线 | 1 天 | -21 | 无旧 workflow 残留引用 |
| Phase 5 | 依赖图 + 聚合报告 + 进阶 | 3 天 | ~5 | needs 拓扑排序正确 |
| **总计** | | **~12.5 天** | **~25 新增，-21 删除** | |

---

## Phase 0：准备工作（0.5 天）

### 文件结构

```
.github/
  workflows/
    pipeline.yml                         ← 调度器（新增）
    triggers/
      pr.yml                             ← PR 触发入口（新增）
      nightly.yml                        ← nightly 触发入口（新增）
      release.yml                        ← release 触发入口（新增）
    stages/
      (Phase 1 填充)
  pipelines/
    pr.yml                               ← PR 配置（新增）
    nightly.yml                          ← nightly 配置（新增）
    release.yml                          ← release 配置（新增）
```

### 任务清单

| # | 任务 | 文件 | 验收标准 |
|---|------|------|---------|
| 0.1 | 创建目录结构 | `mkdir -p .github/workflows/stages/ .github/workflows/triggers/ .github/pipelines/` | 目录存在 |
| 0.2 | 写调度器 pipeline.yml 框架 | `.github/workflows/pipeline.yml` | 能接收 `profile` 输入，解析配置文件，输出 matrix |
| 0.3 | 写配置 Schema | `.github/pipelines/README.md` | 文档化配置字段 |
| 0.4 | 验证 GHA `workflow_call` 可调用 | 用 echo-only stage 测试 | 调度器可调用 stage |

---

## Phase 1：概念验证（1 天）

### 目标

抽取 3 个高频 stage 为可复用 workflow，用 `pipeline.yml --profile pr` 替代 PR 上的 3 个旧 workflow。

### 任务清单

| # | 任务 | 文件 | 验收标准 |
|---|------|------|---------|
| 1.1 | 抽取 `gc-unit` stage | `.github/workflows/stages/gc-unit.yml` | 从 `gc-ci.yml` 提取 ctest 步骤，暴露 `os/timeout/stress_scale/heap_verify` 参数 |
| 1.2 | 抽取 `codegen-unit` stage | `.github/workflows/stages/codegen-unit.yml` | 从 `codegen-regression.yml` 提取 dotnet test 步骤 |
| 1.3 | 抽取 `hygiene` stage | `.github/workflows/stages/hygiene.yml` | 从 `hygiene-gate.yml` 提取 chaos_hygiene + clang-format |
| 1.4 | 写 pr 配置 | `.github/pipelines/pr.yml` | 包含 3 个 stage，各自参数 |
| 1.5 | 写 PR 触发入口 | `.github/workflows/triggers/pr.yml` | 触发 `pipeline.yml` |
| 1.6 | 旧 workflow 失活 | 在 `gc-ci.yml`/`codegen-regression.yml`/`hygiene-gate.yml` 加 `if: false` | 旧 workflow 不跑，新 pipeline 跑 |
| 1.7 | 验证 | 创建一个空 PR | 确认 3 个 stage 正常跑、绿、报告正确 |

### 验收标准

- [ ] 一个 PR 触发后，pipeline.yml 读取 pr.yml 配置，展开 3 个 stage
- [ ] 每个 stage 的 job 名称后缀为 stage 名称（可区分）
- [ ] 3 个 stage 全部绿，PR 门禁通过
- [ ] 旧 3 个 workflow 不触发

---

## Phase 2：全量 Stage 定义（5 天）

### 权威 Stage 清单（job 粒度）— 遗漏检查后修正

> ⚠️ 关键发现：一个 workflow 内的**多个独立 job 是不同测试行为**，应拆成多个可独立调度的 stage。
> gc-ci.yml 的 3 个 job(gc-unit / gc-server-smoke / gc-asan) 用不同 preset(普通/ServerGC/ASan)，是 3 个独立门禁。

| Stage | 来源(workflow:job) | 说明/矩阵 | 独立原因 | 预估 |
|-------|-------------------|----------|---------|:----:|
| `build-gen` | ci.yml:verify 前段 / codegen:aot-codegen | dotnet build Generator/TPG/ATG + cmake SDK | **通用构建前置**，被多数 stage 共享 → 公共 job | 0.5d |
| | | | | |
| `gc-unit` | gc-ci.yml:gc-unit | ctest -R chaos_gc_\|test_gc_ -LE b\|s\|soak + stress SCALE=50 | 常规 GC 单测 | ✅P1 |
| `gc-server-smoke` | gc-ci.yml:gc-server-smoke | GC_SERVER=ON preset + server stress | **独立 preset（ServerGC）** | 0.5d |
| `gc-asan` | gc-ci.yml:gc-asan | ASAN preset | **独立 sanitizer** | 0.5d |
| `gc-linux-tsan` | gc-linux-x64-ci.yml:gc-linux-tsan | linux TSAN | **独立 OS+sanitizer** | 0.5d |
| `gc-stress` | gc-stress-nightly.yml:gc-stress | test_driver --stress-only | nightly 压测 | 0.5d |
| `gc-perf` | gc-stress-nightly.yml 内 | collect-gc-metrics + 对比 | 性能，advisory | 0.25d |
| | | | | |
| `codegen-unit` | codegen-regression:jit-regression | dotnet codegen tests | 码生成单测 | ✅P1 |
| `metadata-writer-unit` | codegen-regression 内 | dotnet MetadataWriter 单测 | **独立被测模块** | 0.25d |
| `snapshot-unit` | codegen-regression + foundation-dll | dotnet snapshot tests | 码生成快照 | 0.25d |
| `aot-smoke` | aot-regression:aot-codegen | aot-smoke.py | AOT 冒烟 | 0.25d |
| `jit-unit` | codegen-regression / hybrid | ctest test_common / test_jit | JIT 单测 | 0.25d |
| `jit-baseline` | jit-baseline.yml:jit-baseline | collect-jit-metrics + 对比 | 性能回归门禁 | 0.5d |
| `hybrid-jit` | hybrid-regression:hybrid-regression | WS2 self-hosted ctest | 混合 JIT | 0.25d |
| `arm64-jit` | arm64-jit-ci:arm64-jit | QEMU aarch64 | 跨架构 | 0.25d |
| | | | | |
| `foundation-verify` | ci.yml:verify | build Tools + 3 SDK + 小批 translation | foundation 构建验证 | 0.5d |
| `foundation-dll` | foundation-dll-regression:foundation-dll | ci_smoke.py（mode: smoke/full/extended 通过 input 选） | **1 stage + mode 参数**，勿拆 3 | 1d |
| `native-integration` | native-nightly:native-nightly | test_driver --layer integration | native CTest 层 | 0.5d |
| | | | | |
| `ubsan` | ubsan-ci:native-ubsan | linux UBSan ctest | sanitizer | 0.25d |
| `clang-tidy` | clang-tidy:clang-tidy | P0 filter 分析 | 静态分析（advisory？） | 0.25d |
| `style-csharp` | managed-csharp-style:dotnet-format | dotnet format | 风格 | 0.25d |
| `hygiene` | hygiene-gate:hygiene | chaos_hygiene + clang-format | 仓库卫生 | ✅P1 |
| `secret-scan` | secret-scan:scan | gitleaks | 安全 | 0.25d |
| `codeql` | codeql:analyze | CodeQL | 安全（advisory） | 0.25d |
| `docs-build` | docs.yml:build | doxygen | 文档 | 0.25d |
| `docs-deploy` | docs.yml:deploy | GitHub Pages deploy | **Pages 部署**（漏了） | 0.25d |
| | | | | |
| `publish-smoke` | codegen-regression（if push） | publish-smoke.py | **仅 push/release profile** | 0.25d |
| `release-verify` | release.sh verify | 5-gate 发布预检 | 发布门禁 | 0.5d |
| `release-notes` | ?（本地 release.sh 段） | 生成 notes/changelog | 发布产物 | 0.25d |
| `build-windows` | release.yml:build-windows | SDK lib RelWithDebInfo | 发布构建 | 0.5d |
| `build-linux` | release.yml:build-linux (disabled) | Linux lib | **已知 disabled** | 关闭 |
| `checksums` | release.yml:checksums | SHA256SUMS/SBOM | 发布审计（漏了） | 0.25d |
| `release-publish` | release.yml:publish + release.sh | gh release / nuget | 最终发布 | 0.5d |

> **clean 计数**：约 28 个可复用 stage（不含 disabled build-linux、notify、ci-run helper）。
> 比原 25 个清单多出的关键修正：
>   - gc-ci 拆 3（加 gc-server-smoke、gc-asan）
>   - 加 metadata-writer-unit（codegen 内部独立模块）
>   - 加 build-gen（所有 native/CI 共享的构建前置）
>   - foundation 合 1 个 stage + mode 参数（勿拆 smoke/full/extended 3 个）
>   - ci.yml verify 是其特有 foundation-verify（含 3-SDK），与 foundation-dll 不同
>   - release 细分为 version/build-windows/checksums/publish（build-linux disabled 关闭）
>   - docs 拆 build 与 deploy
>   - 加 publish-smoke（仅 push/release）

### Release pipeline 现在的真实依赖

release 现在是 `release.yml`（GitHub tag 触发 6 个 job）+ `release.sh`（本地状态机：verify→publish 双轨）。重构后 release 由 **profile: release**（GitHub tag → 调度器 → 跑 foundation-dll extended + release-verify 等）驱动，替代 release.yml 的 6 job；release.sh 保留独立 verify。这需要在重构时**明确 release 双轨归属**（见下一节），是本重构最大的语义决策点。

### 每个 Stage 定义的模板

```yaml
# .github/workflows/stages/<stage-name>.yml
name: <stage-name>
on:
  workflow_call:
    inputs:
      os:      {required: false, type: string,  default: <default-os>}
      timeout: {required: false, type: number,  default: <default-timeout>}
      # 额外的参数看具体 stage 需求
    secrets:
      GH_TOKEN:  {required: false}

jobs:
  <stage-name>:
    runs-on: ${{ inputs.os }}
    timeout-minutes: ${{ inputs.timeout }}
    steps:
      - uses: actions/checkout@v4
        with:
          submodules: recursive

      # ... 提取自旧 workflow 的步骤 ...
```

### 验收标准

- [ ] 25 个 stage 定义文件全部存在，语法正确（`actionlint` 通过）
- [ ] 每个 stage 可独立通过 `workflow_dispatch` 调用
- [ ] 每个 stage 的步骤来自旧 workflow，两步不差

---

## Phase 3：3 个 Profile 配置 + 触发入口（2 天）

### 任务清单

| # | 任务 | 文件 | 验收标准 |
|---|------|------|---------|
| 3.1 | 写 pr 配置 | `.github/pipelines/pr.yml` | 包含：hygiene, style-csharp, gc-unit(stress=50), codegen-unit, jit-unit, foundation-smoke, secret-scan, clang-tidy |
| 3.2 | 写 nightly 配置 | `.github/pipelines/nightly.yml` | 包含：所有 25 个 stage（含 gc-stress, gc-perf, jit-baseline, foundation-extended, native-integration） |
| 3.3 | 写 release 配置 | `.github/pipelines/release.yml` | 包含：release-verify, gc-unit, gc-stress, foundation-extended, publish |
| 3.4 | 写 PR 触发入口 | `.github/workflows/triggers/pr.yml` | trigger: pull_request, 调用 pipeline.yml --profile pr |
| 3.5 | 写 nightly 触发入口 | `.github/workflows/triggers/nightly.yml` | trigger: schedule + workflow_dispatch, 调用 pipeline.yml --profile nightly |
| 3.6 | 写 release 触发入口 | `.github/workflows/triggers/release.yml` | trigger: tag push v*, 调用 pipeline.yml --profile release |
| 3.7 | 验证 3 个场景 | 见下 | |

### 你的 3 个场景验证

| 场景 | 验证方法 |
|------|---------|
| GC 压力测试只在 release 跑，不在 PR 跑 | `pr.yml` 无 `gc-stress`；`release.yml` 有 `gc-stress`。触发 PR → 不跑；触发 tag → 跑 |
| GC 单元测试同时在 CI + release 跑 | `pr.yml` 和 `release.yml` 都有 `gc-unit`。触发 PR 和 tag 都跑 |
| 性能测试只在 nightly 跑 | `pr.yml` 和 `release.yml` 无 `jit-baseline`；`nightly.yml` 有。触发 PR/tag → 不跑；schedule/nightly → 跑 |

### 验收标准

- [ ] 3 个 profile 配置在创建 PR 后各自触发正确的 stage 集
- [ ] 3 个场景验证通过

---

## Phase 4：旧 21 个 workflow 下线（1 天）

### 任务清单

| # | 任务 | 旧文件 | 风险 |
|---|------|--------|------|
| 4.1 | 逐一确认每个 stage 定义已覆盖旧 workflow 的全部步骤 | 全部 21 个 | 高（漏步骤=门禁丢失） |
| 4.2 | 删除旧 workflow | `rm .github/workflows/{gc-ci,codegen-regression,...,hygiene-gate}.yml` | 中 |
| 4.3 | 更新 `notify.yml` 的 watcher 列表 | `.github/workflows/notify.yml` | 低 |
| 4.4 | 更新 `release.yml` | `.github/workflows/release.yml` | 低（release 现在改由 trigger/release.yml 处理） |

### 验收标准

- [ ] 旧 21 个 workflow 文件从仓库删除
- [ ] 所有 PR/nightly/release 触发全部走新 pipeline
- [ ] notify.yml 仍能正确通知失败

---

## Phase 5：进阶编排（3 天）

### 任务清单

| # | 任务 | 文件 | 验收标准 |
|---|------|------|---------|
| 5.1 | 调度器支持依赖图解析 | `pipeline.yml` resolve job | 根据配置的 `needs` 做拓扑排序，分阶段展开 matrix |
| 5.2 | path filter 智能跳过 | `pipeline.yml` | 无相关 path 变更的 stage 自动跳过 |
| 5.3 | 聚合报告 | 新增 `.github/workflows/stages/report.yml` | 所有 stage 的 status 汇总为单一 JSON + 文本报告 |
| 5.4 | 历史时长预测 | 可选 | 调度器根据历史数据自动调整 timeout |

---

## 风险清单

| 风险 | 等级 | 缓解措施 |
|------|------|---------|
| GHA `workflow_call` 不支持跨 repo 的 `needs` 自动推导（必须手动写） | 中 | 调度器在 `resolve` job 中输出 `needs` 矩阵，用 `if: ${{ contains(...) }}` 模拟 |
| `workflow_call` 最多 10 个 `inputs`，部分 stage 需要更多参数 | 低 | 用 `with` 嵌套 JSON 对象传递复合参数 |
| 并行 stage 数超过 GHA 矩阵限制（256） | 低 | 最多 25 个 stage，远低于限制 |
| 旧 workflow 下线后，`notify.yml` 监听的工作流名称变了 | 中 | 同步更新 `notify.yml` 的 `workflow_run` 列表 |
| 迁移过程中 CI 门禁中断 | 中 | Phase 1 保持旧 workflow 并行运行（`if: false`），Phase 2-4 逐步切换 |

---

## 资源估算

| 角色 | 投入 | 说明 |
|------|------|------|
| 开发者 | ~12.5 天 | 全量实施 |
| CI 验证 | 每次 PR 自动触发 | 每个 Phase 合并后通过 PR 验证 |
| 总代码变更 | ~25 新增文件 / -21 删除文件 / ~660 行净增 | |

---

## 下一步

这份计划需要你确认后才能开始执行。需要你拍板的有：

1. **Phase 1 的概念验证 stage 集**：gc-unit + codegen-unit + hygiene 覆盖你想要的证明范围吗？还是想换/加 stage？
2. **Phase 2 的 25 个 stage 清单**：有没有遗漏或多余的？
3. **迁移策略**：Phase 1 保持旧 workflow 并行（`if: false`）还是直接删旧换新？
4. **优先级**：先做 Phase 1 验证设计，再全量拆解？还是直接全量拆解？
---

## 执行状态 (2026-09-04)

| Phase | 状态 | commit | 验证 |
|-------|------|--------|------|
| Phase 0 目录+调度器+v1 dispatcher | ✅ | d635868ac (spike) | yaml 绿 |
| Phase 1 概念验证(gc-unit/hygiene/codegen 3 stage) | ✅ | d635868ac / merge | 26 文件 yaml 绿 |
| Phase 2 全量 26 stage + pipeline + 3 profile | ✅ | 3e9ecaa67 | 全 32 文件 yaml 绿 |
| Phase 3 degate 18 旧 workload | ✅ | 5f6e2357c | 18/18 degate + yaml 绿; release/notify 保留 |
| Phase 3 on:push 自触发入口 | ✅ | 386f26246 | Pipeline on push main(codegen paths) |
| Phase 4 依赖拓扑/report/path-skip | 🔶 部分(仅浅层 marker) | pending | GitHub 语义需真绿观察;已加 finish marker + TODO |

### Phase 4 决策(诚实)
- GitHub 原生已 render per-run 各 job status → 自造 report job 价值低,不 push 复杂版。
- per-profile DAG(如 publish⊃foundation) 与 per-stage path-skip 依赖 dispatch 语义
  (skipped-job in needs = success) 的真 GitHub 运行确认, 否则本地无法证明 → 标 TODO,
  首绿后观察调优。
- finish job: 低风险 workflow 级显式 gate(needs [resolve]), 缺失 profile→resolve fail→红。
