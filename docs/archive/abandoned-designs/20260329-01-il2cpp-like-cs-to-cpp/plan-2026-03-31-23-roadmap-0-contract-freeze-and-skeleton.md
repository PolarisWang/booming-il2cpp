# Roadmap 0：契约冻结与工程骨架 实现计划

> **For agentic workers:** REQUIRED: Use booming-code:subagent-driven-development (if subagents available) or booming-code:executing-plans to implement this plan. Steps use checkbox (`- [ ]`) syntax for tracking.

**目标：** 在当前几乎空白的仓库中，建立能支撑后续 Roadmap 1-4 开发的工程骨架、契约文档、schema、ABI/header、移动端验证骨架和 smoke 输入资产。

**架构：** Roadmap 0 不实现真正的 IL 分析、AOT 闭包或 Runtime Core 语义，而是先冻结后续开发要共享的边界。核心产物分成三组：契约正文、验证夹具、自动化入口。执行时坚持“先文档和 schema，后 header 和 smoke，再做本地验证脚本”，避免空目录驱动开发。

**技术栈：** Markdown、JSON Schema、C/C++ 头文件、CMake、PowerShell、.NET SDK、Android NDK、Xcode/iOS toolchain 描述、快照测试与 compile-only smoke。

---

## 总体约束

- 本计划仅覆盖 `Roadmap 0`
- 本阶段禁止提前实现：
  - 真正的程序集加载器
  - 真正的 Typed IL IR 生成器
  - 真正的 AOT 分析器
  - 真正的 Runtime Core 行为
- 本阶段必须产出可执行的本地验证入口，但允许验证层级停留在：
  - schema round-trip
  - compile-only
  - smoke input build
  - Android/iOS build description 与启动骨架
- 所有 generated C++ 与 runtime 的交互，在本阶段只冻结 surface，不冻结具体实现

## 目标交付物清单

Roadmap 0 完成后，仓库至少应新增以下类型的正式产物：

1. 仓库骨架与职责索引
2. artifact schema pack
3. runtime ABI v0
4. codegen bridge API v0
5. preserve/descriptor v0
6. reference desktop + Android/iOS 验证矩阵
7. smoke 输入项目
8. 本地验证脚本与 compile-only smoke

## Chunk 1: 仓库骨架与契约索引

### 任务 1：建立顶层工程骨架与职责文档

**文件：**
- 创建：`frontend/README.md`
- 创建：`analysis/README.md`
- 创建：`runtime/README.md`
- 创建：`codegen/README.md`
- 创建：`build/README.md`
- 创建：`tests/README.md`
- 创建：`wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/roadmap-0/README.md`
- 创建：`wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/roadmap-0/ownership-map.md`

- [ ] **步骤 1：创建顶层目录结构**

运行：`New-Item -ItemType Directory frontend,analysis,runtime,codegen,build,tests -Force`
预期：六个目录都被创建，重复执行不报错。

- [ ] **步骤 2：为六个顶层目录创建最小 README**

要求每个 README 明确：
- 本目录负责什么
- 不负责什么
- 与其它目录的边界

预期：任何新加入的开发者只看 README 就能知道文件该放哪里。

- [ ] **步骤 3：创建 Roadmap 0 架构索引页**

`wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/roadmap-0/README.md` 至少包含：

```md
# Roadmap 0 Contract Pack

- repo skeleton
- artifact schemas
- runtime ABI v0
- codegen bridge v0
- preserve format v0
- validation matrix
- smoke assets
```

- [ ] **步骤 4：创建 ownership map**

`wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/roadmap-0/ownership-map.md` 明确：
- `frontend/` 未来归前端与 IR 轨
- `analysis/` 未来归 AOT 与 manifest 轨
- `runtime/` 未来归 runtime 轨
- `codegen/` 未来归 codegen 轨
- `build/` 未来归 toolchain / packaging 轨
- `tests/` 归验证轨

- [ ] **步骤 5：检查目录与文档落盘**

运行：`Get-ChildItem frontend,analysis,runtime,codegen,build,tests,wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/roadmap-0`
预期：所有目标目录和 README / md 文件存在。

### 任务 2：建立契约索引、依赖顺序与 Gate 对照表

**文件：**
- 创建：`wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/roadmap-0/contract-index.md`
- 创建：`wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/roadmap-0/gates-and-artifacts.md`

- [ ] **步骤 1：列出所有正式契约产物**

`contract-index.md` 需要覆盖：
- `metadata-graph`
- `typed-il-ir`
- `aot-manifest`
- `code-registration`
- `metadata-registration`
- runtime ABI v0
- codegen bridge v0
- preserve/descriptor v0

- [ ] **步骤 2：给每个契约定义 owner 与消费者**

要求写清楚：
- owner 是谁
- 谁依赖它
- 哪些 Roadmap 会消费它

- [ ] **步骤 3：把 Gate 1 / Gate 3 的验证项映射到产物**

`gates-and-artifacts.md` 至少包含一张表，列出：
- 产物
- 验证命令
- 通过标准
- 对应 Gate

- [ ] **步骤 4：人工复查索引是否覆盖所有 Roadmap 0 范围**

检查项：
- 是否漏掉 Android/iOS 验证矩阵
- 是否漏掉 smoke 资产
- 是否漏掉 descriptor/preserve 格式

## Chunk 2: Artifact Schema Pack

### 任务 3：定义核心 JSON Schema 与示例产物

**文件：**
- 创建：`analysis/contracts/schemas/metadata-graph.schema.json`
- 创建：`analysis/contracts/schemas/typed-il-ir.schema.json`
- 创建：`analysis/contracts/schemas/aot-manifest.schema.json`
- 创建：`analysis/contracts/examples/metadata-graph.min.json`
- 创建：`analysis/contracts/examples/typed-il-ir.min.json`
- 创建：`analysis/contracts/examples/aot-manifest.min.json`
- 创建：`wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/roadmap-0/artifact-schemas-v0.md`
- 测试：`tests/contracts/schema/metadata-graph.snapshot.json`
- 测试：`tests/contracts/schema/typed-il-ir.snapshot.json`
- 测试：`tests/contracts/schema/aot-manifest.snapshot.json`

- [ ] **步骤 1：创建 schema 目录与 examples 目录**

运行：`New-Item -ItemType Directory analysis/contracts/schemas,analysis/contracts/examples,tests/contracts/schema -Force`
预期：三个目录创建成功。

- [ ] **步骤 2：先写 `metadata-graph.schema.json` 的骨架**

最小必须字段：

```json
{
  "formatVersion": "v0",
  "assemblies": [],
  "types": [],
  "methods": []
}
```

预期：结构能表达程序集、类型、方法三个根集合。

- [ ] **步骤 3：再写 `typed-il-ir.schema.json` 的骨架**

最小必须字段：

```json
{
  "formatVersion": "v0",
  "method": {},
  "basicBlocks": [],
  "exceptionRegions": []
}
```

- [ ] **步骤 4：再写 `aot-manifest.schema.json` 的骨架**

最小必须字段：

```json
{
  "formatVersion": "v0",
  "roots": [],
  "reachableMethods": [],
  "genericInstantiations": [],
  "preservedMembers": []
}
```

- [ ] **步骤 5：为三个 schema 各写一个最小 example**

要求每个 example 都是可读的人工维护样例，而不是随机生成。

- [ ] **步骤 6：把 example 复制为首版 snapshot**

运行：`Copy-Item analysis/contracts/examples/*.json tests/contracts/schema/ -Force`
预期：snapshot 与 example 初始一致。

- [ ] **步骤 7：编写 `artifact-schemas-v0.md`**

文档至少写清：
- 每个 schema 的目的
- 字段稳定性
- 哪些字段允许后续追加
- 哪些字段一旦发布就不能随意改名

### 任务 4：定义 registration 与 preserve schema

**文件：**
- 创建：`analysis/contracts/schemas/code-registration.schema.json`
- 创建：`analysis/contracts/schemas/metadata-registration.schema.json`
- 创建：`analysis/contracts/schemas/preserve-descriptor.schema.json`
- 创建：`analysis/contracts/examples/code-registration.min.json`
- 创建：`analysis/contracts/examples/metadata-registration.min.json`
- 创建：`analysis/contracts/examples/preserve-descriptor.min.json`
- 创建：`wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/roadmap-0/preserve-format-v0.md`
- 测试：`tests/contracts/schema/code-registration.snapshot.json`
- 测试：`tests/contracts/schema/metadata-registration.snapshot.json`
- 测试：`tests/contracts/schema/preserve-descriptor.snapshot.json`

- [ ] **步骤 1：定义 code registration schema**

最小必须字段：
- modules
- methodPointers
- invokerKinds
- reversePInvokeExports

- [ ] **步骤 2：定义 metadata registration schema**

最小必须字段：
- types
- methods
- fields
- stringLiterals

- [ ] **步骤 3：定义 preserve descriptor schema**

最小必须字段：
- targetAssembly
- targetType
- preserve
- reason

- [ ] **步骤 4：为三个 schema 写 example 与 snapshot**

要求 example 体现：
- 一个模块
- 一个类型
- 一个被反射保留的成员

- [ ] **步骤 5：在 `preserve-format-v0.md` 里写清 priority 规则**

至少说明：
- 用户手工 descriptor 优先级
- 内建 profile 与自动推导如何合并
- `reason` 字段必须可追溯

- [ ] **步骤 6：执行 schema 文件存在性检查**

运行：`Get-ChildItem analysis/contracts/schemas,analysis/contracts/examples,tests/contracts/schema`
预期：六类 schema 与对应 example/snapshot 都可见。

## Chunk 3: Runtime ABI v0 与 Codegen Bridge v0

### 任务 5：定义 runtime ABI v0 surface

**文件：**
- 创建：`runtime/abi/include/boom_runtime_abi_v0.h`
- 创建：`runtime/abi/README.md`
- 创建：`wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/roadmap-0/runtime-abi-v0.md`
- 测试：`tests/contracts/abi/abi_surface_smoke.cpp`
- 测试：`tests/contracts/abi/CMakeLists.txt`

- [ ] **步骤 1：创建 ABI 目录**

运行：`New-Item -ItemType Directory runtime/abi/include,tests/contracts/abi -Force`
预期：ABI 与测试目录创建成功。

- [ ] **步骤 2：起草 `boom_runtime_abi_v0.h`**

最小函数族必须覆盖：

```cpp
struct BoomRuntimeApiV0;
struct BoomTypeInfo;
struct BoomObject;
struct BoomMethodInfo;

BoomRuntimeApiV0* boom_runtime_get_api_v0();
BoomObject* boom_gc_alloc_object(BoomTypeInfo* type);
void boom_runtime_class_init(BoomTypeInfo* type);
void boom_thread_attach();
void boom_thread_detach();
```

- [ ] **步骤 3：在 `runtime-abi-v0.md` 中写清“稳定边界”和“非稳定边界”**

必须明确：
- generated code 不得直接碰 runtime 内部结构体
- 允许以后在 `BoomRuntimeApiV0` 末尾扩展函数表
- 不允许在 v0 内随意重排已发布函数签名

- [ ] **步骤 4：编写 compile-only smoke**

`abi_surface_smoke.cpp` 至少要：
- include `boom_runtime_abi_v0.h`
- 声明一个最小调用点
- 在没有真实实现时也能完成编译

- [ ] **步骤 5：编写最小 CMakeLists**

运行：`cmake -S tests/contracts/abi -B out/roadmap0-abi-smoke -G Ninja`
预期：configure 成功。

- [ ] **步骤 6：构建 compile-only smoke**

运行：`cmake --build out/roadmap0-abi-smoke`
预期：目标编译通过，不要求链接真实 runtime。

### 任务 6：定义 codegen bridge v0 surface

**文件：**
- 创建：`codegen/bridge/include/boom_codegen_bridge_v0.h`
- 创建：`codegen/bridge/README.md`
- 创建：`wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/roadmap-0/codegen-bridge-v0.md`
- 测试：`tests/contracts/codegen/bridge_surface_smoke.cpp`
- 测试：`tests/contracts/codegen/CMakeLists.txt`

- [ ] **步骤 1：创建 bridge 目录**

运行：`New-Item -ItemType Directory codegen/bridge/include,tests/contracts/codegen -Force`
预期：bridge 与测试目录创建成功。

- [ ] **步骤 2：起草 `boom_codegen_bridge_v0.h`**

最小 helper 族必须覆盖：

```cpp
void boom_codegen_register(const void* code_registration,
                           const void* metadata_registration,
                           const void* codegen_options);
const void* boom_codegen_initialize_runtime_metadata(uint32_t token);
void* boom_codegen_resolve_icall(const char* signature);
void* boom_codegen_box_value(BoomTypeInfo* type, const void* data);
```

- [ ] **步骤 3：在 `codegen-bridge-v0.md` 中写清 helper 分类**

必须至少分成：
- registration
- metadata usage
- field/static access
- boxing/unboxing
- delegate / invoke helpers
- exception boundary helpers

- [ ] **步骤 4：编写 compile-only bridge smoke**

要求 smoke 能证明：
- generated stub 只需要 include bridge header
- 无需 include runtime 私有头

- [ ] **步骤 5：配置并构建 codegen smoke**

运行：`cmake -S tests/contracts/codegen -B out/roadmap0-codegen-smoke -G Ninja`
运行：`cmake --build out/roadmap0-codegen-smoke`
预期：configure 与 build 均通过。

- [ ] **步骤 6：交叉检查 ABI 与 bridge 是否职责重复**

检查项：
- `boom_runtime_*` 不应被 copy 到 bridge
- `boom_codegen_*` 不应反向暴露 runtime internals

## Chunk 4: 构建工具链与移动端验证骨架

### 任务 7：建立 reference desktop + Android/iOS toolchain 描述

**文件：**
- 创建：`CMakePresets.json`
- 创建：`build/toolchains/reference-desktop.cmake`
- 创建：`build/toolchains/android-arm64.cmake`
- 创建：`build/toolchains/ios-arm64.cmake`
- 创建：`wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/roadmap-0/toolchain-matrix-v0.md`
- 测试：`build/scripts/probe-desktop-toolchain.ps1`
- 测试：`build/scripts/probe-android-toolchain.ps1`
- 测试：`build/scripts/probe-ios-toolchain.sh`

- [ ] **步骤 1：建立 toolchain 目录与脚本目录**

运行：`New-Item -ItemType Directory build/toolchains,build/scripts -Force`
预期：目录创建成功。

- [ ] **步骤 2：编写 `reference-desktop.cmake`**

要求至少覆盖：
- 编译器家族
- C++ 标准
- 输出目录约定
- compile-only smoke 的通用选项

- [ ] **步骤 3：编写 `android-arm64.cmake`**

要求至少约定：
- `ANDROID_ABI=arm64-v8a`
- `ANDROID_PLATFORM`
- NDK toolchain 入口变量

- [ ] **步骤 4：编写 `ios-arm64.cmake`**

要求至少约定：
- `CMAKE_SYSTEM_NAME=iOS`
- `CMAKE_OSX_ARCHITECTURES=arm64`
- SDK / simulator / device 差异说明

- [ ] **步骤 5：编写 `CMakePresets.json`**

要求包含：
- `desktop-smoke`
- `android-arm64-smoke`
- `ios-arm64-smoke`

- [ ] **步骤 6：编写三份 toolchain probe 脚本**

预期输出：
- 工具链是否存在
- 关键环境变量
- 推荐下一步命令

### 任务 8：建立 Android/iOS smoke harness 形态与验证矩阵

**文件：**
- 创建：`tests/mobile/android-smoke/README.md`
- 创建：`tests/mobile/android-smoke/CMakeLists.txt`
- 创建：`tests/mobile/android-smoke/smoke_entry.cpp`
- 创建：`tests/mobile/ios-smoke/README.md`
- 创建：`tests/mobile/ios-smoke/CMakeLists.txt`
- 创建：`tests/mobile/ios-smoke/smoke_entry.mm`
- 创建：`wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/roadmap-0/mobile-validation-matrix-v0.md`

- [ ] **步骤 1：创建移动端 smoke 目录**

运行：`New-Item -ItemType Directory tests/mobile/android-smoke,tests/mobile/ios-smoke -Force`
预期：目录创建成功。

- [ ] **步骤 2：起草 Android smoke harness**

要求 README 写明：
- 当前阶段只验证 compile/link/startup shape
- 未来 Roadmap 4 再接真实 host app

- [ ] **步骤 3：起草 iOS smoke harness**

要求 README 写明：
- simulator 与 device 的区别
- `.mm` 入口只是为了保留 ObjC++ 桥接可能性

- [ ] **步骤 4：编写 `mobile-validation-matrix-v0.md`**

矩阵至少包含：
- desktop reference
- Android arm64 compile
- Android startup smoke
- iOS arm64 compile
- iOS startup smoke

- [ ] **步骤 5：为 Android smoke 跑 configure**

运行：
`cmake -S tests/mobile/android-smoke -B out/roadmap0-android-smoke -G Ninja -C build/toolchains/android-arm64.cmake`

预期：在工具链存在时 configure 成功；工具链不存在时输出明确错误。

- [ ] **步骤 6：为 iOS smoke 跑 configure**

运行：
`cmake -S tests/mobile/ios-smoke -B out/roadmap0-ios-smoke -GXcode -C build/toolchains/ios-arm64.cmake`

预期：在 macOS + Xcode 环境中 configure 成功；非该环境时探针脚本能给出清晰阻塞原因。

## Chunk 5: Smoke 输入资产与 Roadmap 0 本地验证入口

### 任务 9：建立五个 smoke 输入项目

**文件：**
- 创建：`tests/smoke/README.md`
- 创建：`tests/smoke/input/HelloWorld/HelloWorld.csproj`
- 创建：`tests/smoke/input/HelloWorld/Program.cs`
- 创建：`tests/smoke/input/GenericEcho/GenericEcho.csproj`
- 创建：`tests/smoke/input/GenericEcho/Program.cs`
- 创建：`tests/smoke/input/ReflectionLite/ReflectionLite.csproj`
- 创建：`tests/smoke/input/ReflectionLite/Program.cs`
- 创建：`tests/smoke/input/PInvokeLite/PInvokeLite.csproj`
- 创建：`tests/smoke/input/PInvokeLite/Program.cs`
- 创建：`tests/smoke/input/HostEmbeddingLite/HostEmbeddingLite.csproj`
- 创建：`tests/smoke/input/HostEmbeddingLite/Program.cs`

- [ ] **步骤 1：创建 smoke 输入目录**

运行：`New-Item -ItemType Directory tests/smoke/input/HelloWorld,tests/smoke/input/GenericEcho,tests/smoke/input/ReflectionLite,tests/smoke/input/PInvokeLite,tests/smoke/input/HostEmbeddingLite -Force`
预期：五个输入目录创建成功。

- [ ] **步骤 2：编写 `HelloWorld`**

用途：验证最小程序集输入。

- [ ] **步骤 3：编写 `GenericEcho`**

用途：验证后续 generic shape 与 manifest roots 的最小样例。

- [ ] **步骤 4：编写 `ReflectionLite`**

用途：验证保留规则和 metadata example 的基础输入。

- [ ] **步骤 5：编写 `PInvokeLite`**

用途：验证后续 interop 边界所需的最小程序集输入。

- [ ] **步骤 6：编写 `HostEmbeddingLite`**

用途：验证宿主嵌入生命周期的最小托管入口。

- [ ] **步骤 7：逐个 build 五个 smoke 项目**

运行：
- `dotnet build tests/smoke/input/HelloWorld/HelloWorld.csproj -c Release`
- `dotnet build tests/smoke/input/GenericEcho/GenericEcho.csproj -c Release`
- `dotnet build tests/smoke/input/ReflectionLite/ReflectionLite.csproj -c Release`
- `dotnet build tests/smoke/input/PInvokeLite/PInvokeLite.csproj -c Release`
- `dotnet build tests/smoke/input/HostEmbeddingLite/HostEmbeddingLite.csproj -c Release`

预期：全部编译通过，并生成可供后续 Roadmap 1 使用的程序集输入。

### 任务 10：建立 Roadmap 0 一键验证入口

**文件：**
- 创建：`build/scripts/verify-roadmap-0.ps1`
- 创建：`build/scripts/verify-roadmap-0.sh`
- 创建：`wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/roadmap-0/local-verification.md`

- [ ] **步骤 1：编写 PowerShell 验证脚本**

要求顺序执行：
- 文件存在性检查
- schema/example/snapshot 检查
- ABI smoke configure/build
- codegen smoke configure/build
- 五个 smoke 项目 `dotnet build`

- [ ] **步骤 2：编写 shell 版本验证脚本**

要求与 PowerShell 版同构，方便 Linux/macOS 开发环境复用。

- [ ] **步骤 3：编写 `local-verification.md`**

文档至少写清：
- 前置依赖
- 桌面平台验证命令
- Android toolchain 验证命令
- iOS toolchain 验证命令
- 失败时去哪里看

- [ ] **步骤 4：运行 PowerShell 验证脚本**

运行：`pwsh ./build/scripts/verify-roadmap-0.ps1`
预期：在缺少 Android/iOS 工具链时，脚本对移动端部分给出 `SKIP with reason`，而不是整体失败。

- [ ] **步骤 5：确认 Gate 1 / Gate 3 的 Roadmap 0 子集已可本地执行**

检查项：
- schema 能看
- header 能编
- smoke 输入能 build
- Android/iOS 有 probe 和 smoke 入口

## 计划注意事项

- 执行本计划时，每完成一个 Chunk，先用 `@booming-code:requesting-code-review` 做计划/实现检查，再进入下一个 Chunk。
- 在任何声称“Roadmap 0 完成”之前，必须补跑 `@booming-code:verification-before-completion`。
- 不要在本计划执行过程中偷跑到 Roadmap 1/2/3 的真实实现。
- 不要让 Android/iOS 在本阶段演变成“必须真机语义通过”的阻塞条件。
- compile-only 与 smoke-only 失败信息必须可读，否则它们对后续阶段没有价值。

## 建议执行顺序

1. `Chunk 1`：把目录、索引和 ownership map 定下来
2. `Chunk 2`：把 schema 和 example/snapshot 定下来
3. `Chunk 3`：把 ABI v0 与 bridge v0 定下来
4. `Chunk 4`：把 toolchain 与移动端 smoke 骨架补齐
5. `Chunk 5`：把 smoke 输入和一键验证入口接上

## 完成标准

满足以下全部条件时，才算 `Roadmap 0` 达标：

- 新工程结构已落盘，职责边界清晰
- schema、example、snapshot、doc 成套存在
- runtime ABI v0 与 codegen bridge v0 已有正式 header 和 compile-only smoke
- preserve/descriptor 格式与 registration 产物已被正式建模
- `HelloWorld`、`GenericEcho`、`ReflectionLite`、`PInvokeLite`、`HostEmbeddingLite` 都可 build
- Android/iOS 至少已有 probe、toolchain 描述和 smoke harness 骨架
- `verify-roadmap-0` 能在桌面环境下跑通主线检查，并对移动端给出可解释的 skip/通过结果

## 执行交接

计划已完成并保存到 `docs/booming/2026-03-29-il2cpp-like-cs-to-cpp/plan-2026-03-31-23-roadmap-0-contract-freeze-and-skeleton.md`。准备好执行了吗？
