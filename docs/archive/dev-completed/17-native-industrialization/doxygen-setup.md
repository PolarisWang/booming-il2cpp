# Native C++ API 文档体系（Doxygen 方案）

> 任务：为 native 模块设计 Doxygen API 文档生成方案（T-B5 / P3）。
> 日期：2026-09-02
> 域：构建(7) / API 文档（P3）
> 对标基准：production il2cpp 工程门禁体系（核心合约头文件 Doxygen 注释 + 自动生成）
> 关联文档：`docs/dev/in-progress/native-industrialization/roadmap-v1-01.md`、`design-v1-01.md`、`DISPATCH.md`

---

## 1. 目标与非目标

### 1.1 目标

1. 建立**标准化注释规范**，区分「强制 `///` + Doxygen 标记」的合约头 与「推荐 `///`」的内部头。
2. 产出 **Doxyfile 配置**（INPUT、OUTPUT、EXTRACT_ALL、GENERATE_HTML 等），可本地 / CI 复现。
3. 提供 **函数 / 类 / 枚举 / 宏** 的 Doxygen 注释模板。
4. 分阶段覆盖：**P0 API 合约 -> P1 基础库 -> P2 运行时**。
5. 打通 **CI 自动生成 + GH Pages 发布**，push 到 `main` 即更新，可选项 PR preview。

### 1.2 非目标

- **不改动任何 `src/` 或 `contracts/` 源码**（本文档产出时已通过只读勘察确认，不写回源码）。
- 不引入运行时 / 编译期开销（Doxygen 是文档期工具，遵循「P1 性能 > P2 架构 > P3 热更」全局优先级，工具链不碰运行时）。
- 不在本阶段强制 `runtime-core/` 全量注释（P2 可选），避免阻塞一次性合入。

---

## 2. 当前注释现状评估（已实测勘察）

### 2.1 核心合约头 `contracts/native/v0/`（7 个文件）

| 文件 | 主线注释风格 | Doxygen 标记现状 |
|------|-------------|-----------------|
| `runtime_abi.h` | `/* */` 块注释 + `--` 分隔线 | 无 Doxygen 标记，全靠 `/* */` 段注释 |
| `abi_manifest.h` | 顶部 `//`；函数区已用 `///` | 函数已用 `///`（未带 `@param`/`@return`），宏/结构体用 `//` |
| `codegen_bridge.h` | 混用：`/* */` 长块 + `///` | 部分 `///`（如 GcSlotMapV0、chaos_string_id_t），`@a` 标记偶现 |
| `patch_data.h` | 顶部 `//` 长注释块 + `//` 分段 | 无 Doxygen，结构体字段靠尾部 `//` 行内注释 |
| `register_ir.h` | 顶部 `//` + `//` 分段 | 无 Doxygen |
| `unified_metadata.h` | 顶部 `//`（中文）+ `//` 分段 | 无 Doxygen（部分中文注释） |
| `runtime_instantiation.h` | `/* */` 块 + `--` 分隔线 | 无正式 `///`，多用 `/* */` 块 |

**结论**：`abi_manifest.h` 与 `codegen_bridge.h` 已出现 `///` 雏形（函数 / 个别结构体），但**未统一、未带 `@param/@return` 等标记**。其余 5 个文件为纯 `/* */` 或 `//`。这构成「合约头必须用 `///` + Doxygen 标记」的改造起点，但属于**注释风格增补**，Doxygen 对 `/* */` 也能识别，只是不做结构化提取，无解析风险。

**关键发现**：合约头在**结构体字段**上大量使用**单行行内注释**（如 register_ir.h、runtime_abi.h、codegen_bridge.h），这些字段级注释是 ABI 布局契约的核心（bit 偏移、编码含义），是 Doxygen 结构化文档最有价值的地方——模板需覆盖「结构体字段」这一层。

### 2.2 内部基础库 `src/native/common/chaos/`（27 个 .h）

| 文件 | 风格 |
|------|------|
| `format.h` | 已大量用 `///`（函数级）——可作为「基础库样板」 |
| `common.h` | `//` 头部块 + `/// g_chaos_fail_hook` |
| `log.h` | `//` 巨型头部架构说明 + `//` 宏，无 `///` |
| `checked_conv.h` | 纯函数、零注释 |
| `span.h`、`finally.h`、`compiler_hints.h`、`type_info.h` 等 | `//` 为主，函数极少 `///` |

**结论**：基础库注释覆盖**参差不齐**。`format.h`（短、纯函数、已用 `///`）应作为「内部头样板」，可用于模板示范。多数文件为「文件级 `//` 架构注释 + 函数级零注释」或「宏 `//` 段」。

### 2.3 现状总体评价

- **无任何 Doxyfile / doxygen 集成**（勘察确认：无 Doxyfile、无 doxygen CMake、`.github/workflows/` 17 个 workflow 均无 docs 任务）。
- **注释密度**：合约头**文件级 / 结构体级注释已很详尽**（架构意图、bit 布局、编码），但**非结构化**（`//`、`/* */`、行内）。
- **函数级注释**：合约头函数表（RuntimeAbiV0 的成员函数）**未逐一 Doxygen**；基础库参差。
- **混合中英文**：unified_metadata.h 用中文注释，本文档规范给出兼容策略（见 4.2）。

---

## 3. Doxyfile 配置方案

### 3.1 路径规划

建议在仓库根新建 `docs/native-api/` 目录承载 Doxygen 工程配置与产物管理：

```
docs/native-api/
|- Doxyfile              # 主配置（INPUT 指向真实源码，不在源码内塞配置）
|- .gitignore            # 忽略 build/ 产物（或复用根 .gitignore）
|- README.md             # 本地重生成说明（可选）
```

**为什么不把 Doxyfile 放 contracts/native/v0/**：Doxygen INPUT 是**多目录**（合约 + 基础库 + 运行时），配置应按「生成工程」维度组织，而非贴在某源码目录。

### 3.2 核心配置项（关键值）

```
# -- 输入 / 输出 --------------------------------------------
INPUT                       = contracts/native/v0 \
                              src/native/common/chaos \
                              src/native/runtime-core      # P2 阶段按需开启
RECURSIVE                   = YES
OUTPUT_DIRECTORY            = docs/native-api/build

# -- 覆盖率 -------------------------------------------------
EXTRACT_ALL                 = YES
EXTRACT_PRIVATE             = NO
EXTRACT_STATIC              = YES
EXTRACT_LOCAL_CLASSES       = NO
HIDE_UNDOC_MEMBERS          = NO       # 即使无注释也列出成员，便于审查覆盖缺口
HIDE_UNDOC_CLASSES          = NO

# -- 语言 / C++ 识别 ----------------------------------------
CASE_SENSE_NAMES            = NO
ENABLE_PREPROCESSING        = YES
MACRO_EXPANSION             = YES
EXPAND_ONLY_PREDEF          = YES      # 避免展开过多宏导致噪声
# 合约头大量使用 CHAOS_RUNTIME_ABI_EXPORT / CHAOS_RUNTIME_ABI_CALL 等
# 宏包裹函数指针，需将其定义为空以让 Doxygen 解析出真实签名：
PREDEFINED                 = "CHAOS_RUNTIME_ABI_EXPORT=" \
                             "CHAOS_RUNTIME_ABI_CALL=__cdecl" \
                             "CHAOS_IL2CPP_INTPTR=intptr_t" \
                             "CHAOS_IL2CPP_UINT32=uint32_t" \
                             "CHAOS_IL2CPP_INT32=int32_t" \
                             "CHAOS_IL2CPP_UINT16=uint16_t" \
                             "CHAOS_IL2CPP_UINT8=uint8_t" \
                             "CHAOS_IL2CPP_INT64=int64_t" \
                             "CHAOS_IL2CPP_UINT64=uint64_t" \
                             "CHAOS_IL2CPP_SIZE=size_t"

# -- 输出格式 ----------------------------------------------
GENERATE_HTML               = YES
GENERATE_LATEX              = NO
GENERATE_XML                = YES      # 供未来 CI 覆盖检查
GENERATE_TREEVIEW           = YES
SEARCHENGINE                = YES
USE_MDFILE_AS_MAINPAGE      = docs/native-api/index.md

# -- GCC / C++11 解析（避免 ./ 噪声）-----------------------
JAVADOC_AUTOBRIEF           = NO
QT_AUTOBRIEF                = NO
MULTILINE_CPP_IS_BRIEF      = NO
OPTIMIZE_OUTPUT_FOR_C       = NO
MARKDOWN_SUPPORT            = YES
```

### 3.3 需重点预定义的 ABI 宏

合约头依赖大量「类型别名宏」和「装饰宏」，**不预定义会导致 Doxygen 解析出残缺签名**（如 `void* (CHAOS_RUNTIME_ABI_CALL* object_new)(...)` 里的 `CHAOS_RUNTIME_ABI_CALL`，以及 `CHAOS_IL2CPP_*` 类型别名）。这是 Doxyfile 配置里**最易踩坑**的点，已在 `PREDEFINED` 中覆盖主要别名。落地时需在生成后人工抽查 `RuntimeAbiV0`、`CodegenBridgeV0` 的 HTML 是否呈现真实函数指针签名。

### 3.4 本地重生成

```bash
# 依赖：doxygen 已安装
doxygen docs/native-api/Doxyfile
# 产物：docs/native-api/build/html/index.html
```

---

## 4. 注释规范

### 4.1 强制层级（合约 vs 内部）

| 层级 | 目录 | 要求 |
|------|------|------|
| **强令牌（必须）** | `contracts/native/v0/` | **所有 API**（类型、函数、宏、结构体字段）都必须用 `///` + Doxygen 标记 |
| **推荐** | `src/native/common/chaos/` | 建议用 `///`；可先用 `//` 段注释，后续 SLA 收敛到 `///` |
| **可选** | `src/native/runtime-core/` | 仅公共入口 / 复杂子系统用 `///`，P2 再全面铺开 |

**理由**：合约头是「跨 DLL / 跨语言（C 与 C++）编译」的 ABI 事实来源，是最需要稳定、可浏览、可检索文档的一层；runtime-core 是内部实现，文档价值密度低且改造面大。

### 4.2 语言

- **标准建议**：统一使用**英文**首句 + 必要中文补充（参考现有 codegen_bridge.h 大量英文 `///`、unified_metadata.h 中文）。
- 规范给出**兼容策略**：Doxygen 对中英文 Markdown 均支持。为避免 ABI 层跨团队契约语义歧义，**合约头建议首句英文 + 关键参数可中文**；不强改存量中文。
- 最终一致性由 styleguide 术语表（stable_id、method_token、carrier 等既有词）维持，避免一词多译。

### 4.3 Sphinx/Breathe 或 Doxygen 直出？

**本方案选 Doxygen 直出 HTML**（不引入 Breathe/Sphinx），理由：
- 仓库已有大量 Markdown 文档，Sphinx 路由会引入 Python 依赖链（breathe、doxyrest），CI 复杂度上升。
- 纯 C/C++ ABI 合约文档，Doxygen HTML + treeview + search 已足够，无需 reStructuredText 换皮。

### 4.4 注释模板

#### 4.4.1 函数模板（合约头函数表 / 基础库函数）

```cpp
/// 一句话功能（动词开头，说明「做什么」，不含「怎么做」）。
///
/// 补充语义：可空返回 / 所有权 / 并发语义 / 失败行为。
///
/// @param  arg_a     参数含义（单位 / 编码 / 边界）。
/// @param  out_arg   输出参数；入参可空表示跳过该输出。
/// @return 成功返回 CHAOS_RUNTIME_STATUS_OK；
///         失败返回对应错误码。
/// @attention 调用方不得 free 返回的字符串；由 bridge 持有。
/// @note   本成员仅在 ABI v3+ 可用（检索时需先读 abi_version）。
```

**对照示例 — RuntimeAbiV0.field_get_value**（当前无注释，改造后）：

```cpp
/// 读取指定字段的值到调用方缓冲区。
///
/// @param  runtime_state   运行时状态（来自 runtime_init）。
/// @param  thread_state    当前线程状态（来自 thread_attach）。
/// @param  field           字段句柄（来自 type_find_field）。
/// @param  object_instance 实例指针（静态字段传 NULL）。
/// @param  out_value       输出缓冲区（由调用方分配）。
/// @param  out_value_size  输出缓冲区大小（字节）。
/// @return CHAOS_RUNTIME_STATUS_OK 或错误码。
RuntimeStatus (CHAOS_RUNTIME_ABI_CALL* field_get_value)(...);
```

**对照示例 — format_int32**（当前仅一句 `///`，扩展为带 param/return）：

```cpp
/// 将 int32 值格式化为字符串（替换 snprintf + char[64] 模式）。
///
/// @param  value  要格式化的整数。
/// @return 格式化后的 std::string。
```

**对照示例 — PatchData_String**（当前无 `///`，改造后）：

```cpp
/// 从字符串堆中返回给定偏移量的 C 字符串。
///
/// @param  hdr    已验证的 .patchdata 头部指针。
/// @param  offset 字符串堆内的偏移量（0 返回空字符串）。
/// @return 指向字符串堆内部的指针（调用方不可 free）。
static inline const char* PatchData_String(const PatchDataHeader* hdr, uint32_t offset);
```

#### 4.4.2 类 / 结构体模板（含字段）

```cpp
/// ABI 契约：描述 .patchdata 的固定头部布局。v3 = 132 字节。
///
/// 字段偏移的兼容性说明见 @ref patch_data.h 文件注释。
typedef struct PatchDataHeader {
    uint32_t magic;              ///< "PADT" 魔数（小端）。
    uint32_t version;            ///< 当前 3。
    uint32_t header_size;        ///< 必须 == sizeof(PatchDataHeader）。
    // ...
} PatchDataHeader;
```

**关键规范点**：合约结构体的**行内单行注释统一升级为 `///<` 行内 Doxygen**，这样 Doxygen 才能把字段纳入结构体的属性表。这是当前 register_ir.h / runtime_abi.h / codegen_bridge.h 最大的缺失区（现用尾部 `//` 或 `/* */`）。

#### 4.4.3 枚举 / 值常量模板

```cpp
/// 载体类型编码（对应 AotCoreIr 的 AbiCarrierKind）。
enum {
    CHAOS_ABI_CARRIER_INT32 = 1,   ///< 32 位有符号整型直接传递。
    // ...
};
```

或对 enum class：

```cpp
/// Runtime/CoreCLR 对齐的字节容器宽度。
enum class SpanStorageKind : CHAOS_IL2CPP_UINT8 {
    kArray     = 1,   ///< 由数组承载。
    kRawInt32  = 2,   ///< 由裸 int32 段承载。
};
```

#### 4.4.4 宏模板

```cpp
/// 对 (flags) 进行「是否已热更」位测试。
/// 供生成代码与 PatchLoader 共用，语义与其自述一致。
#define kHotpatchActive  (1u << 0)
```

### 4.5 禁止项（styleguide 红线）

- **不**用 `///<` 反写在已带 `@param` 的函数（函数用 `@param`，**字段**才用 `///<`）。
- **不**用 `///` 写「实现细节」——实现放函数体，`///` 只记录契约语义。
- **不**用 `\brief \param` 的 backslash 形式，统一 `@`（合约头 codegen_bridge.h 已用 `@a`，朝 `@` 对齐）。

---

## 5. 分阶段覆盖计划

| 阶段 | 范围 | 交付物 | 门禁 |
|------|------|--------|------|
| **P0 API 合约** | contracts/native/v0/（7 文件，全部 API 注释） | 逐文件改为 `///` + 模板，字段行内 `///<` | 7 文件 100% 覆盖（EXTRACT + HIDE_UNDOC 审查） |
| **P1 基础库** | src/native/common/chaos/（27 .h） | 以 format.h 为样板，优先 status.h/native_types.h 等公共类型 | 函数级覆盖率目标 >=50% 起步，渐进到 80% |
| **P2 运行时（可选）** | src/native/runtime-core/ | 仅公共入口 / 复杂子系统 | 视 P2 模块化重构进度开启 |

**注入方式**：P0 与 P1 通过在**同一次 PR** 里 `///` 化（改注释 + 改文档，不涉运行时逻辑）。仅改注释 PR 不进 clang-tidy 红线区（HeaderFilterRegex 不含 contracts/，见第 6 节），可低风险提交。

---

## 6. Doxygen 与 clang-tidy 的兼容性

### 6.1 现状分析

- `src/native/.clang-tidy` 的 Checks 是 `*` 风格，但**未显式打开** `clang-diagnostic-comment` / `-Wdocumentation`。当前生效的相关检查主要是 functional（clang-analyzer-*、bugprone-*、performance-*、modernize-*、readability-*、cppcoreguidelines-*、concurrency-*、misc-*、llvm-*）。
- **readability- 组内**注释相关主要是 clang-diagnostic-*（即 -Wcomment 族）；因未开 `-Wdocumentation`，`///<` 或错误链接通常不触发告警。
- **HeaderFilterRegex: 'src/native/.*\.h$' 只匹配 src/native/ 下的头文件**，**contracts/native/v0/ 不在其中**，合约头的注释改动不会被 clang-tidy 分析（显著降低 P0 注释化风险）。

### 6.2 风险点与缓解

| 风险 | 说明 | 缓解 |
|------|------|------|
| -Wcomment / -Wdocumentation 触发 | 若未来在 src/native/ 启用带 -Wdocumentation 的 clang-tidy 检查 | 1) P0 合约头不在 HeaderFilterRegex；2) 模板规避嵌套 `*/`；3) 必要时显式 -Wno-documentation 隔离 |
| 注释改动误伤 WarningsAsErrors | .clang-tidy 的 WarningsAsErrors 集中在分析型，无 comment 类 | 不改 WarningsAsErrors 行为 |
| clang-format 对注释重排 | hygiene-gate 对 src/native/*.{cpp,h} 做格式校验，contracts/ 不在校验 glob | P0 改 contracts/ 不受格式门禁约束；src/native/ 的 `///` 注释需保持 clang-format 风格（列对齐） |
| Doxygen 解析出残签名 | ABI 宏未预定义会丢失函数指针签名 | 见 3.3 PREDEFINED；生成后抽查 RuntimeAbiV0/CodegenBridgeV0 |

**结论**：Doxygen 注释与当前 clang-tidy 配置**兼容**。唯一需留意的长期项是「若未来启用 -Wdocumentation，需把注释质量纳入 clang-tidy」，届时在 src/native/.clang-tidy 增加对应 check 并做一次存量 `///` 注释合规化即可——本方案已把注释模板定义为严格文档注释规范，理论上开 -Wdocumentation 也能过。

---

## 7. CI 发布方案（GH Pages）

### 7.1 工作流文件 `.github/workflows/docs.yml`

```yaml
name: docs

on:
  push:
    branches: [main]
    paths:
      - 'contracts/native/**'
      - 'src/native/common/**'
      - 'src/native/runtime-core/**'
      - 'docs/native-api/**'
      - '.clang-tidy'
      - 'CMakePresets.json'
  workflow_dispatch: {}        # 手动重生成

permissions:
  contents: write              # 写 gh-pages 分支
  pages: write                 # GH Pages 部署
  id-token: write

concurrency:
  group: docs
  cancel-in-progress: true

jobs:
  build-docs:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4

      # 方案 A（推荐）：直接 apt 装 doxygen，不依赖 build 树。
      - name: Install Doxygen
        run: |
          sudo apt-get update -qq
          sudo apt-get install -y -qq doxygen graphviz

      - name: Generate API docs
        run: doxygen docs/native-api/Doxyfile

      - name: Upload Pages artifact
        uses: actions/upload-pages-artifact@v3
        with:
          path: docs/native-api/build/html

  deploy:
    needs: build-docs
    runs-on: ubuntu-latest
    environment:
      name: github-pages
      url: ${{ steps.deployment.outputs.page_url }}
    steps:
      - id: deployment
        uses: actions/deploy-pages@v4
```

> 备选部署方式 B：用 peaceiris/actions-gh-pages@v4 直接 push 到 gh-pages 分支（若仓库偏好「gh-pages 来源」而非 GitHub Actions 源）。二选一，避免同时配两套。

### 7.2 触发策略

- **每次 push 到 main** 且 paths 命中源码 / 文档 / Doxyfile 变更时重建（避免无关 commit 白跑）。
- `workflow_dispatch` 提供手动重建入口。
- **可选 PR preview**：在 PR 上加 job，生成到 github-pages 的**临时预览**（用 deploy-pages 的 preview 或 artifact 上传 + comment 链接）。**建议先不加预览**，砍到最小可用（main 发布 + 手动），预览作为后续增强。

### 7.3 集成注意

- Pages 源设置为 **GitHub Actions**（repo Settings -> Pages -> Source: GitHub Actions），否则 `actions/deploy-pages` 无效。
- `docs/native-api/build/` 加入 .gitignore，避免产物入库。
- 文档站点 URL 写入 docs/INDEX.md 或 README 的「API 文档」入口。

---

## 8. 风险与缓解

| 风险 | 等级 | 缓解 |
|------|------|------|
| 注释化 PR 大而杂（7+27 文件） | 中 | 按 P0->P1->P2 分批，每批独立 PR + review；锁 contracts/ 无 clang-tidy 干扰 |
| Doxygen 对 ABI 宏解析出残签名 | 中 | PREDEFINED 兜底 + 生成后人工抽查两个 V0 函数表 |
| 注释漂移（代码改了注释没跟） | 中 | CI 用 GENERATE_XML 提取覆盖，后续可加「未文档化成员/API」的增量门禁 |
| clang-format 对 src/native/ `///` 重排 | 低 | hygiene-gate 仅校验 src/native/*.{cpp,h}；`///` 注释按列对齐即可，不改逻辑 |
| 未来开 -Wdocumentation 触发告警 | 低 | 模板已按严格文档注释规范；届时一次性合规化 |
| 中文/英文混用导致文档风格不统一 | 低 | 术语表 + 明确首句规则（不做强制回译），降低摩擦 |
| GH Pages 源配置错误 / 权限 | 低 | Settings 显式设 GitHub Actions 源；permissions.pages/contents 齐全 |

---

## 9. 落地 Checklist（建议执行顺序）

1. 建 docs/native-api/Doxyfile + CMakeLists 可选 option(CHAOS_IL2CPP_DOCS OFF) + .gitignore。
2. 本地跑通 doxygen docs/native-api/Doxyfile，抽查 RuntimeAbiV0 / CodegenBridgeV0 / RegisterInstruction 签名正确。
3. **P0**：逐文件把 contracts/native/v0/ 7 文件转为 `///` + 模板，字段行内 `///<`。
4. 建 .github/workflows/docs.yml + 启 GH Pages（GitHub Actions 源）。
5. **P1**：src/native/common/chaos/ 以 format.h 为样板铺 `///`，优先公共类型。
6. **P2（可选）**：runtime-core 公共入口 `///`。
7. 渐进式覆盖门禁（可选）：GENERATE_XML 落库 + 增量检查。

> 本文档为设计产出，**未修改任何源码文件**。
