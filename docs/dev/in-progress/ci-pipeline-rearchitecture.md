# 方案A 完整设计：GitHub Actions 原生可复用流水线架构

## 核心理念

```
┌─ 测试行为定义 (Stage Definitions) ──────────────────────┐
│  gc-unit.yml, gc-stress.yml, codegen-unit.yml, ...       │
│  "我知道怎么跑一个给定的测试"                                │
│  通过 workflow_call 暴露: os, timeout, flags, thresholds  │
└──────────────────────────────────────────────────────────┘
            ▲ 被选择                                      ▲ 被调用
            │                                              │
┌─ 管线配置 (Pipeline Profile) ───────┐  ┌─ 调度器 ───────────┐
│  pipelines/pr.yml                    │  │  pipeline.yml      │
│  pipelines/nightly.yml               │  │  读取 profile →     │
│  pipelines/release.yml               │  │  矩阵展开每个 stage  │
│  这里写: 哪个 stage 在哪个 profile    │  │  并行/串行执行        │
│  里启用、超时、矩阵、门禁软硬           │  │  聚合报告            │
└──────────────────────────────────────┘  └────────────────────┘
```

**3 层 + 1 核心规则**：

| 层 | 职责 | 你关心的配置 |
|----|------|-------------|
| **Stage 定义** `.github/workflows/stages/*.yml` | 一个测试行为如何执行 | 不再改。只定义一次执行方式 |
| **Pipeline 配置** `.github/pipelines/{profile}.yml` | 哪些 stage 在哪个 profile 里跑 | ✅ **你改这里**。决定"gc-stress 开在 release 不开在 PR" |
| **调度器** `.github/workflows/pipeline.yml` | 读取配置→调度执行→汇总报告 | 不再改。稳定基础设施 |

---

## 一、Stage 定义（What — 测试行为）

每个 stage 是一个 standalone `reusable workflow`。通过 `workflow_call` 暴露参数给调用方。

### 文件结构

```
.github/workflows/
  pipeline.yml              ← 调度器（唯一）
  stages/
    gc-unit.yml             ← ctest -R chaos_gc_|test_gc_ -LE benchmark|stress|soak
    gc-stress.yml           ← ctest -L "stress|soak"
    gc-perf.yml             ← ctest -L "benchmark" -R gc_
    codegen-unit.yml        ← dotnet test unit/codegen
    codegen-snapshot.yml    ← dotnet test unit/snapshot
    driver-unit.yml         ← dotnet test unit/driver
    jit-unit.yml            ← ctest -R test_jit_ 
    jit-baseline.yml        ← ctest + compare-baseline.py
    native-integration.yml  ← test_driver --layer integration
    foundation-smoke.yml    ← ci_smoke.py --mode=smoke
    foundation-full.yml     ← ci_smoke.py --mode=full
    foundation-extended.yml ← ci_smoke.py --mode=extended
    hygiene.yml             ← chaos_hygiene.py --ci + clang-format
    style-csharp.yml        ← dotnet format --verify
    clang-tidy.yml          ← clang-tidy analysis
    ubsan.yml               ← ctest -R (ubsan preset)
    arm64-jit.yml           ← ctest -L arm64 (QEMU)
    publish-smoke.yml       ← publish-smoke.py
    release-verify.yml      ← release.sh verify
    secret-scan.yml         ← gitleaks
    codeql.yml              ← CodeQL analysis
```

### 一个 stage 定义示例

```yaml
# .github/workflows/stages/gc-unit.yml
# 测试行为: 运行 GC 确定性单元测试 + 可选 per-PR stress
# 暴露参数: os, timeout, stress_scale, heap_verify
name: gc-unit
on:
  workflow_call:
    inputs:
      os:                    {required: false, type: string,  default: windows-latest}
      timeout:               {required: false, type: number,  default: 60}
      stress_scale:          {required: false, type: number,  default: 0}
      heap_verify:           {required: false, type: number,  default: 2}
      cmake_preset:          {required: false, type: string,  default: windows-x64-reference}
      skip_known_failures:   {required: false, type: boolean, default: false}
    secrets:
      GH_TOKEN:              {required: false}

jobs:
  gc-unit:
    runs-on: ${{ inputs.os }}
    timeout-minutes: ${{ inputs.timeout }}
    steps:
      - uses: actions/checkout@v4
        with:
          submodules: recursive

      - name: Configure
        run: cmake --preset ${{ inputs.cmake_preset }}

      - name: Build
        run: cmake --build --preset ${{ inputs.cmake_preset }}

      - name: GC unit tests
        run: |
          ctest --test-dir artifacts/presets/${{ inputs.cmake_preset }} \
            -R "chaos_gc_|test_gc_" \
            -LE "benchmark|stress|soak" \
            -E "test_gc_young_collector" \
            --timeout 600
        env:
          HEAP_VERIFY: ${{ inputs.heap_verify }}

      - name: Per-PR stress (SCALE=${{ inputs.stress_scale }})
        if: ${{ inputs.stress_scale > 0 }}
        run: |
          ctest --test-dir artifacts/presets/${{ inputs.cmake_preset }} \
            -R "chaos_gc_region_barrier_stress_test" \
            --timeout 600
        env:
          CHAOS_IL2CPP_STRESS_SCALE: ${{ inputs.stress_scale }}
```

### 第二个示例：gc-stress（你关心的"部署到 release 但不部署到 PR"）

```yaml
# .github/workflows/stages/gc-stress.yml
name: gc-stress
on:
  workflow_call:
    inputs:
      os:        {required: false, type: string,  default: windows-latest}
      timeout:   {required: false, type: number,  default: 3600}
      stress_scale: {required: false, type: number, default: 100}
      update_baseline: {required: false, type: boolean, default: false}
    secrets:
      GH_TOKEN:  {required: false}

jobs:
  gc-stress:
    runs-on: ${{ inputs.os }}
    timeout-minutes: ${{ inputs.timeout }}
    steps:
      - uses: actions/checkout@v4
      - run: cmake --preset windows-x64-reference
      - run: cmake --build --preset windows-x64-reference

      - name: GC stress/soak/benchmark tiers
        run: |
          python tests/runner/test_driver.py \
            --layer integration --group contracts-native \
            --stress-only --cases --json gc-stress-report.json

      - name: Update baseline
        if: ${{ inputs.update_baseline && github.event_name == 'workflow_dispatch' }}
        run: |
          python tests/runner/test_driver.py --stress-only --json /tmp/baseline.json
          cp /tmp/baseline.json .github/baselines/gc-stress-metrics.json
```

### 第三个示例：jit-baseline（你关心的"只部署到 nightly"）

```yaml
# .github/workflows/stages/jit-baseline.yml
name: jit-baseline
on:
  workflow_call:
    inputs:
      os:           {required: false, type: string,  default: ubuntu-latest}
      timeout:      {required: false, type: number,  default: 1800}
      update_baseline: {required: false, type: boolean, default: false}

jobs:
  jit-baseline:
    runs-on: ${{ inputs.os }}
    timeout-minutes: ${{ inputs.timeout }}
    steps:
      - uses: actions/checkout@v4
      - run: cmake --preset linux-x64-native
      - run: cmake --build --preset linux-x64-native

      - name: JIT benchmarks
        run: |
          ctest --test-dir artifacts/presets/linux-x64-native \
            -R test_jit_ -L benchmark \
            --output-on-failure

      - name: Compare against baseline
        uses: ./.github/workflows/stages/jit-baseline.yml/compare  # 内部调用
        with:
          baseline: .github/baselines/jit-metrics.json
```

---

## 二、Pipeline 配置（Where/When — 部署策略）

放在 `.github/pipelines/*.yml`。每个文件定义**一个管线**及其包含的 stage。

### 配置 Schema

```yaml
# .github/pipelines/pr.yml
# 管线：PR 验证
# 属于你的：决定哪些 stage 在 PR 上跑
profile: pr
trigger: pull_request                # 映射到 GitHub Actions trigger
paths:
  include: ["src/**", "tests/**", "CMakeLists.txt"]
  exclude: ["docs/**", "scripts/**"]

stages:
  # 基础门禁（每个 PR 必跑）
  hygiene:
    enabled: true
    os: ubuntu-latest
    timeout: 10

  style-csharp:
    enabled: true
    os: ubuntu-latest
    timeout: 10

  # 单元测试（每个 PR 必跑）
  gc-unit:
    enabled: true
    os: windows-latest
    timeout: 30
    with:
      stress_scale: 50       # 轻量 stress 在 PR 上跑
      heap_verify: 2

  codegen-unit:
    enabled: true
    os: ubuntu-latest
    timeout: 10

  # e2e smoke（每个 PR 必跑）
  foundation-smoke:
    enabled: true
    os: ubuntu-latest
    timeout: 120

  # ← 这里不写 gc-stress → PR 不跑它
  # ← 这里不写 jit-baseline → PR 不跑它
```

### nightly 配置示例

```yaml
# .github/pipelines/nightly.yml
profile: nightly
trigger: schedule
schedule: "7 4 * * *"          # cron 表达式

stages:
  # 基础门禁
  hygiene:
    enabled: true
    os: ubuntu-latest
    timeout: 10

  # GC 全家桶
  gc-unit:
    enabled: true
    os: windows-latest
    timeout: 60
    with:
      stress_scale: 0
      heap_verify: 2

  gc-stress:                     # 只在 nightly 跑
    enabled: true
    os: windows-latest
    timeout: 3600
    with:
      stress_scale: 100
      update_baseline: false

  gc-perf:                       # 只在 nightly 跑
    enabled: true
    os: windows-latest
    timeout: 600

  # 验证管线
  foundation-extended:           # 全量 e2e（smoke 在 PR 跑，full 在 nightly 跑）
    enabled: true
    os: ubuntu-latest
    timeout: 1800

  jit-baseline:                  # 只在 nightly 跑
    enabled: true
    os: ubuntu-latest
    timeout: 1800
```

### release 配置示例

```yaml
# .github/pipelines/release.yml
profile: release
trigger: tag
tag_pattern: "v*"                 # 匹配 tag push

stages:
  # 发布前验证
  release-verify:
    enabled: true
    os: ubuntu-latest
    timeout: 600
    needs: []                     # 无依赖，最先跑

  # GC 全量（包括 stress）
  gc-unit:
    enabled: true
    os: windows-latest
    timeout: 60
    with:
      stress_scale: 50            # release 也跑 stress，但不是全集
      heap_verify: 2

  gc-stress:                      # ✅ release 也跑 gc-stress
    enabled: true
    os: windows-latest
    timeout: 3600
    with:
      stress_scale: 100

  # 全量 e2e 验证
  foundation-extended:
    enabled: true
    os: ubuntu-latest
    timeout: 1800

  # 发布
  publish:
    enabled: true
    os: ubuntu-latest
    timeout: 600
    needs: [release-verify, gc-unit, gc-stress, foundation-extended]  # 发布需要前面都绿
```

---

## 三、调度器设计（pipeline.yml）

唯一一个 workflow 入口。16 个 stage 的调度器。

```yaml
# .github/workflows/pipeline.yml
name: Pipeline
on:
  workflow_call:
    inputs:
      profile:
        required: true
        type: string
      ref:
        required: false
        type: string
        default: ${{ github.ref }}

jobs:
  # ── Job 1: 解析配置文件 ──
  resolve:
    runs-on: ubuntu-latest
    outputs:
      matrix: ${{ steps.parse.outputs.matrix }}
    steps:
      - uses: actions/checkout@v4
        with:
          ref: ${{ inputs.ref }}

      - id: parse
        run: |
          # 读取 .github/pipelines/${{ inputs.profile }}.yml
          # 提取 enabled stages 为 JSON matrix
          MATRIX=$(python -c "
import yaml, json, sys
with open('.github/pipelines/${{ inputs.profile }}.yml') as f:
    cfg = yaml.safe_load(f)
stages = {k: v for k, v in cfg.get('stages', {}).items() if v.get('enabled', False)}
print(json.dumps({'stage': list(stages.keys()), 'config': stages}))
")
          echo "matrix=$MATRIX" >> $GITHUB_OUTPUT
          echo "profile=${{ inputs.profile }}" >> $GITHUB_OUTPUT

  # ── Job 2: 展开每个 stage ──
  run-stage:
    needs: [resolve]
    strategy:
      matrix:
        stage: ${{ fromJson(needs.resolve.outputs.matrix).stage }}
      fail-fast: false
    uses: .github/workflows/stages/${{ matrix.stage }}.yml@${{ inputs.ref }}
    with:
      os: ${{ fromJson(needs.resolve.outputs.matrix).config[matrix.stage].os || 'ubuntu-latest' }}
      timeout: ${{ fromJson(needs.resolve.outputs.matrix).config[matrix.stage].timeout || 60 }}
      # 透传 with 中的自定义参数到 stage
      ${{ fromJson(needs.resolve.outputs.matrix).config[matrix.stage].with || {} }}
    secrets: inherit
```

### 各 profile 的触发入口

```yaml
# .github/workflows/triggers/pr.yml
# 触发入口：PR → 跑 pr profile
name: PR Pipeline
on: pull_request

jobs:
  pr-pipeline:
    uses: .github/workflows/pipeline.yml@main
    with:
      profile: pr
```

```yaml
# .github/workflows/triggers/nightly.yml
# 触发入口：schedule → 跑 nightly profile
name: Nightly Pipeline
on:
  schedule:
    - cron: "7 4 * * *"
  workflow_dispatch:

jobs:
  nightly-pipeline:
    uses: .github/workflows/pipeline.yml@main
    with:
      profile: nightly
```

```yaml
# .github/workflows/triggers/release.yml
# 触发入口：tag push → 跑 release profile
name: Release Pipeline
on:
  push:
    tags:
      - "v*"
  workflow_dispatch:
    inputs:
      tag:
        required: true
        type: string

jobs:
  release-pipeline:
    uses: .github/workflows/pipeline.yml@main
    with:
      profile: release
      ref: ${{ github.event.inputs.tag || github.ref }}
```

---

## 四、你关心的 3 个场景在配置层的表达

### 场景 1：GC 压力测试，只部署在 release pipeline，不部署在 PR

```yaml
# .github/pipelines/pr.yml
stages:
  gc-stress: {enabled: false}    # ← 不开

# .github/pipelines/release.yml
stages:
  gc-stress:
    enabled: true
    os: windows-latest
    timeout: 3600
    with:
      stress_scale: 100
      update_baseline: false
```

### 场景 2：GC 单元测试，同时部署在 CI + 发布流程

```yaml
# .github/pipelines/pr.yml
stages:
  gc-unit:
    enabled: true
    os: windows-latest
    timeout: 30
    with:
      stress_scale: 50

# .github/pipelines/release.yml
stages:
  gc-unit:
    enabled: true
    os: windows-latest
    timeout: 60
    with:
      stress_scale: 100
```

### 场景 3：性能测试，只部署在 nightly build

```yaml
# .github/pipelines/pr.yml
stages:
  jit-baseline: {enabled: false}    # ← 不开

# .github/pipelines/nightly.yml
stages:
  jit-baseline:
    enabled: true
    os: ubuntu-latest
    timeout: 1800
    with:
      update_baseline: false

# .github/pipelines/release.yml
stages:
  jit-baseline: {enabled: false}    # ← 不开
```

---

## 五、复杂性：stage 依赖关系（needs）

有些 stage 之间存在依赖（`publish` 需要 `release-verify` 和 `gc-stress` 都绿）。在配置中通过 `needs` 表达：

```yaml
stages:
  release-verify:
    enabled: true
    needs: []                     # 无依赖
    timeout: 600

  gc-unit:
    enabled: true
    needs: [release-verify]        # 等 verify 过了再跑

  gc-stress:
    enabled: true
    needs: [release-verify]        # 可以并行跑，不依赖 gc-unit

  foundation-extended:
    enabled: true
    needs: [release-verify]        # 并行

  publish:
    enabled: true
    needs: [release-verify, gc-unit, gc-stress, foundation-extended]  # 全绿才发布
```

调度器在 `resolve` job 中解析 `needs` 图，分阶段展开 matrix：

```
Phase 0: [release-verify]
Phase 1: [gc-unit, gc-stress, foundation-extended]  ← 并行
Phase 2: [publish]
```

这需要 pipeline.yml 里做依赖图拓扑排序来分阶段触发。GAN 原生 matrix 不支持跨 job 的 `needs` 自动推导——需要调度器用 `needs` 字段在 Phase 间展开，而不是一次全 matrix。

---

## 六、迁移路线

### Phase 1: 概念验证（1 天）

| 步骤 | 产出 |
|------|------|
| 1. 建立 `.github/workflows/stages/gc-unit.yml` | 抽取 gc-ci.yml 的 GC 单元测试步骤为可复用 workflow |
| 2. 建立 `.github/workflows/stages/codegen-unit.yml` | 抽取 codegen-regression.yml 的单元测试步骤 |
| 3. 建立 `.github/workflows/stages/hygiene.yml` | 抽取 hygiene-gate.yml |
| 4. 建立 `.github/pipelines/pr.yml` | 配置 pr profile 包含这 3 个 stage |
| 5. 建立 `.github/workflows/triggers/pr.yml` | 触发入口 |
| 6. 验证 | 创建一个 PR，确认 3 个 stage 都跑 |

### Phase 2: 主力拆解（5 天，21→1 调度器 + 20 stage 定义）

逐个拆解剩余的 18 个 workflow：

| 批次 | workflow | stage 文件 | 天数 |
|------|----------|-----------|:----:|
| 1 | gc-ci, gc-linux-x64-ci, gc-stress-nightly | `gc-unit`, `gc-stress`, `gc-perf`, `gc-tsan` | 1 |
| 2 | codegen-regression, aot-regression, hybrid-regression | `codegen-snapshot`, `aot-smoke`, `jit-unit` | 1 |
| 3 | jit-baseline, arm64-jit-ci | `jit-baseline`, `arm64-jit` | 0.5 |
| 4 | foundation-dll-regression, ci.yml | `foundation-smoke`, `foundation-full`, `foundation-extended` | 1 |
| 5 | clang-tidy, ubsan, managed-csharp-style, secret-scan, codeql, docs | 各 1 个 stage | 0.5 |
| 6 | native-nightly, notify | `native-integration`, `notify` | 0.5 |

### Phase 3: 配置驱动（2 天）

| 步骤 | 产出 |
|------|------|
| 1. 建立 `.github/pipelines/nightly.yml` | 配置 nightly profile，包含 gc-stress + jit-baseline + foundation-extended |
| 2. 建立 `.github/workflows/triggers/nightly.yml` | 触发入口 |
| 3. 建立 `.github/pipelines/release.yml` | 配置 release profile，包含 gc-stress + foundation-extended + publish |
| 4. 建立 `.github/workflows/triggers/release.yml` | 触发入口 |
| 5. 验证 3 个场景 | 确认 PR 不跑 gc-stress，release 跑 gc-stress，nightly 跑 jit-baseline |

### Phase 4: 进阶编排（3 天）

| 步骤 | 产出 |
|------|------|
| 1. 调度器 needs 解析 | 根据配置的 `needs` 做拓扑排序，分阶段展开 matrix |
| 2. pre-flight 跳过 | 根据 path filter 智能跳过无关 stage |
| 3. 时长预测 | 根据历史运行时长自动调整 timeout |
| 4. 聚合报告 | 所有 stage 的 status 汇总为一个 dashboard 报告 |

---

## 七、文件清单与改造量估算

| 路径 | 类型 | 文件数 | 行数 |
|------|------|:------:|:----:|
| `.github/workflows/stages/*.yml` | 新增 | ~20 | ~400 |
| `.github/workflows/pipeline.yml` | 新增 | 1 | ~80 |
| `.github/workflows/triggers/*.yml` | 新增 | ~5 | ~30 |
| `.github/pipelines/*.yml` | 新增 | ~5 | ~150 |
| `.github/workflows/*.yml`（旧） | 删除 | 21 | -~2000 |
| **总计** | | **~52** | **~660** |

---

## 八、你下一步需要定的事

| 问题 | 选项 |
|------|------|
| **1. 配置字段是否覆盖你的需求？** | 配置 schema 的 `enabled/os/timeout/with/needs` 字段是否足够？缺什么？ |
| **2. 阶段数量** | 20 个 stage 是否太多？要不要合并一些（如 foundation-{smoke,full,extended} 合并为一个带 mode 参数）？ |
| **3. 先做 Phase 1 概念验证？** | 抽 3 个 stage 试跑，看效果再继续？还是直接 Phase 2 拆全部？ |
| **4. 依赖图复杂度** | 你的 release pipeline 是否有复杂的 stage 依赖（如 publish 需要前面 5 个都绿）？还是简单顺序？ |