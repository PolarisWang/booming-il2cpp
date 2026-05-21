# Subject Generated Test Workspace And Unified Test Entry 设计

Date: 2026-04-14
Status: pending-review

## 1. 目标

把当前 subject 工程的生成、测试发现、测试宿主和统一入口整理成一条清晰主线：

- `subject` 只保留 solution/source 和 attribute 声明。
- `ChaosUnitTest` / `ChaosBenchmark` 成为 proof / benchmark 的唯一声明源。
- 每个 `subject` 都能生成四类工程：
  - managed projects
  - managed test projects
  - native projects
  - native test projects
- `run generate/build/test` 统一消费同一份 workspace manifest。
- 历史手写注册、历史 proof host、历史兼容字段和旧入口被系统性删除。

## 2. 设计原则

### 2.1 单一真源

- 测试声明真源：C# attribute
- 执行工作区真源：`workspace.manifest.json`
- 测试对象索引真源：registry / compiled catalog

### 2.2 入口统一，不强行 native-only

- proof / benchmark 统一为“generated host 驱动”的执行模型。
- engineering validation / engineering workload 继续保留 pipeline 语义。
- CLI 层统一，不把所有对象都伪装成 native executable test。

### 2.3 旧逻辑必须可删除

- 新架构中的每个新增 contract，都要能替代至少一类旧逻辑。
- roadmap 末尾必须落到“删掉旧入口、删掉旧字段、删掉旧测试壳”。

## 3. 现有链路与可复用部分

### 3.1 可直接复用

- attribute 定义：
  - `src/reference/Chaos.TestFramework/Chaos.TestFramework.cs`
- declaration discovery：
  - `src/managed/Chaos.IL2CPP.DeclarationDiscovery/Program.cs`
- declared catalog 构建：
  - `build/toolchains/run/testing/compiled_catalog.py`
- registry 骨架：
  - `build/toolchains/run/testing/registry.py`
- unified project command：
  - `build/toolchains/run/commands/project.py`
- unified test command：
  - `build/toolchains/run/commands/test.py`

### 3.2 需要重构

- `project_workspace.py`
  - 从“managed + native 混合临时模型”升级到“四类工程 v2”
- `test.py` / registry / executor
  - 从“对象分发 + 历史入口混合执行”升级到“workspace manifest + generated host 优先”
- benchmark dashboard
  - 从“历史 case / 历史 record 聚合”升级到“declared benchmark identity + managed baseline 对比”

## 4. 目标架构

### 4.1 Subject Workspace Model v2

每个 subject 在 `solutions/subjects/<SubjectId>/` 下生成：

```text
solutions/subjects/<SubjectId>/
  workspace.manifest.json
  <SubjectId>.sln
  managed/
    ...
  managed-tests/
    <SubjectId>.ProofHost.csproj
    <SubjectId>.BenchmarkHost.csproj
    Generated/
      ChaosGeneratedDeclaredTests.g.cs
      ChaosGeneratedDeclaredBenchmarks.g.cs
      declared-tests.catalog.json
  native/
    <matrix-id>/
      ...
  native-tests/
    <matrix-id>/
      proof/
      benchmark/
      ...
```

四类工程的一等模型定义：

| 工程类型 | 作用 | 粒度 | 生成来源 |
| --- | --- | --- | --- |
| managed projects | subject 原始 solution / project 集合 | subject | `subjects/<id>/source` |
| managed test projects | 自动生成的 proof / benchmark managed host | subject | declared catalog |
| native projects | 由 IL2CPP 生成并配置的 native 工程 | matrix | managed source 翻译结果 |
| native test projects | 由 generated managed test host 翻译并包装出的 native test host | matrix | managed test host 翻译结果 |

### 4.2 Workspace Manifest v2

目标是让 `generate/build/test` 全部围绕同一份 manifest 工作。

建议字段：

```json
{
  "workspaceVersion": 2,
  "kind": "subject-workspace",
  "subjectId": "subject/SolutionCorePack",
  "managedSolutionPath": "solutions/subjects/.../<Subject>.sln",
  "managedProjects": [],
  "managedTestProjects": [
    {
      "kind": "proof-host",
      "projectPath": "solutions/subjects/.../managed-tests/<Subject>.ProofHost.csproj",
      "catalogPath": "solutions/subjects/.../managed-tests/Generated/declared-tests.catalog.json"
    },
    {
      "kind": "benchmark-host",
      "projectPath": "solutions/subjects/.../managed-tests/<Subject>.BenchmarkHost.csproj",
      "catalogPath": "solutions/subjects/.../managed-tests/Generated/declared-tests.catalog.json"
    }
  ],
  "nativeProjects": [],
  "nativeTestProjects": [],
  "matrices": []
}
```

旧字段处理策略：

- 保留一个阶段的兼容读取可以接受。
- 新写入一律只写 v2 字段。
- Phase 5 删除旧字段和旧读取分支。

### 4.3 声明发现与生成链路

目标链路如下：

```text
subject source solution
  -> build managed assemblies
  -> declaration discovery
  -> declared catalog
  -> generate managed proof/benchmark host
  -> build managed test host
  -> translate test host to native
  -> generate native proof/benchmark host project
```

#### 4.3.1 declared catalog

复用现有 `compiled_catalog.py` 的发现能力，但把输出升级为“可直接驱动 host generation”的产物。

catalog 至少包含：

- stableId
- assemblyName
- declaringType
- methodName
- methodSignature
- alias
- kind
- category / capability family / capability item / archetype / requires
- benchmark 专属字段：metrics / modes / warmup / iteration / invocation
- `entryIndex`
  - 供 runner 内部用数值索引调度，避免继续扩张 string 协议面

#### 4.3.2 generated managed host

不再手写 proof / benchmark 注册代码，而是生成：

- `ChaosGeneratedDeclaredTests.g.cs`
- `ChaosGeneratedDeclaredBenchmarks.g.cs`

这两个生成文件负责：

- 元数据表
- `stableId -> entryIndex` 映射
- `entryIndex -> wrapper invoke` 映射
- 统一 runner 调度入口

建议 v1 的 declared method 契约先收敛为：

- `public static void Method()`
- `public static int Method()`
- benchmark 为 `public static void Method()`

不在 v1 直接引入实例方法、参数注入、异步返回值等复杂签名。

### 4.4 Native Test Host 模型

不建议 native 侧重新做一套 attribute 解释器或手写注册器。

推荐方案：

1. managed proof / benchmark host 先生成并可在 .NET 下运行。
2. IL2CPP 把这些 generated host 作为普通 managed project 一起翻译。
3. native test project 只负责：
   - 薄的启动壳
   - 入口参数解析
   - 调用 generated host 对应的 native 入口

优点：

- managed / native 共用同一份 dispatch 逻辑。
- proof / benchmark identity 完全一致。
- 删除手工注册的路径最直接。

### 4.5 统一 CLI / Registry 模型

#### 4.5.1 生成与构建

- `run generate project subject --id subject/<id>`
  - 生成 workspace v2、declared catalog、managed test host、native/native-test project
- `run build project subject --id subject/<id> --component all|managed|managed-tests|native|native-tests`
  - 基于 workspace manifest 选择构建范围

#### 4.5.2 测试执行

- `run test subject --id subject/<id> --family proof|benchmark --mode managed|native|all`
- `run test declared-unit-test --id declared-unit-test/<stableId>`
- `run test declared-benchmark --id declared-benchmark/<stableId>`

调度原则：

- registry 负责“找对象”
- workspace manifest 负责“找宿主”
- generated host 负责“执行 entry”

### 4.6 engineering validation / workload 的位置

它们不应该被强行等同为 declared proof / benchmark。

设计判断：

- engineering validation / workload 继续是 registry object。
- 统一入口继续通过 `run test ...` 调度它们。
- 但它们的产物、链接和菜单展示都要挂到同一套 workspace manifest / report spine 上。

这样不会把 pipeline 语义打碎，同时 CLI 视角依旧统一。

## 5. 旧逻辑清理设计

### 5.1 必删对象

1. 手写 proof / benchmark 注册代码
2. `project_workspace.py` 中只服务旧 proof target 的临时字段和逻辑分支
3. 旧 native proof target 命名和路径别名
4. registry / selector 中只为历史 subject 入口存在的兼容壳
5. tests 中直接绑定历史 subject 目录或历史 phase 命名的断言

### 5.2 允许短期存在的 shim

- v1 -> v2 manifest 读取兼容
- CLI 旧参数到新参数的映射
- dashboard 旧 record 到 declared identity 的一次性归并

这些 shim 都必须：

- 标注为阶段性逻辑
- 在 roadmap 中有明确删除相位

## 6. 目录与职责边界

### 6.1 `subjects/`

只保留：

- source solution / project
- subject manifest
- 与业务验证本体直接相关的 source files

不再长期保留：

- 手写 test runner
- 手写 benchmark registry
- 仅为旧 CLI / 旧 pipeline 服务的 glue code

### 6.2 `solutions/subjects/`

只保留生成结果和 IDE 打开入口，不反向成为真源。

### 6.3 `tests/`

测试目标改为验证通用 contract，而不是绑定具体旧 subject 壳：

- declaration discovery contract
- workspace manifest v2 contract
- generated host contract
- unified command contract
- dashboard aggregation contract

## 7. 验收标准

### 7.1 架构验收

- 任一新增 subject，只写 solution/source + attribute，即可生成四类工程。
- proof / benchmark 不再需要手工注册。
- CLI 和 registry 都能基于同一份 workspace manifest 找到执行宿主。

### 7.2 清理验收

- 旧 proof / benchmark 注册入口从仓库中删除。
- 旧 workspace manifest 临时字段和兼容路径删除或显式标记为最后一个过渡版本。
- tests 中针对历史 subject / phase 命名的耦合断言被替换。

### 7.3 运行验收

- declared unit test 在 managed / native 两侧使用相同 stableId 运行。
- benchmark 至少能稳定产出：
  - managed baseline
  - native result
  - case-level identity
- dashboard 以 managed baseline 为主基线展示对比结果。

## 8. 风险

### 风险 1：把入口统一误做成 native-only

- 缓解：
  - 在 design 和 roadmap 中显式区分“统一入口”和“统一物理宿主”。

### 风险 2：generated host 契约过宽，导致迁移停滞

- 缓解：
  - v1 先限制 declared method 签名。

### 风险 3：双轨迁移时间过长，旧逻辑删不掉

- 缓解：
  - roadmap 末尾单独设 legacy deletion phase。
  - 每个 phase 都要求替代至少一类旧逻辑。

### 风险 4：benchmark 数据继续缺失

- 缓解：
  - benchmark host 与 dashboard 改造作为单独 phase，不把它当“后补工作”。

## 9. 非目标

- 不在这次设计里把 engineering validation 全部改写为 attribute。
- 不在 v1 支持任意复杂方法签名的 declared test。
- 不为了兼容历史入口而继续扩大 string 协议。
- 不把 `solutions/subjects` 变成新的手工维护真源。
