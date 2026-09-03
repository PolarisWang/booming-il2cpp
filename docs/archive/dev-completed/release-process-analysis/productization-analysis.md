# 发布 IL2CPP 产品化 — 方案分析与深度设计

> 日期：2026-09-01
> 域：发布流程 + 翻译路径 + 构建 + 集成
> 状态：brainstorm 阶段

---

## 1. 核心认知重构

### 用户的核心诉求

> **"用户真正拿到的是 il2cpp 的产物"**

这句话改变了发布的目标定位。当前发布流程（包括上一轮的 `release.sh` + GitHub Release）以"发布 HelloWorld 应用"为终点。但产品正确形态应该是：

**chaos-il2cpp 是一个 IL2CPP 工具链产品**，用户拿到的应该是一个 **可安装、可执行的 SDK/工具链包**，而 HelloWorld 发布只是"验证这个 SDK 能正常工作"的 smoke test 环节。

### 现状盘点（来自代码探查）

| 维度 | 现状 | 差距 |
|------|------|------|
| **产物形态** | `chaos-sdk/` CMake 项目（头文件+libs+cmake） | 未打包成可分发的格式 |
| **CLI** | `.csproj` 编译出的 Exe，只能 `dotnet run`/直接跑二进制 | 不能 `dotnet global tool` 安装 |
| **分发** | 无 NuGet 包、无 dotnet tool | 用户无法 `dotnet tool install chaos-il2cpp` |
| **预构建 SDK** | 56 MB（14 个 .lib）驻留在 `tests/e2e/translation/sdk/` | SDK 与测试代码耦合 |
| **平台覆盖** | 12 个 CMake 预设，覆盖 Win/macOS/Linux/Android/iOS | 无发布侧跨平台打包 |
| **版本一致性** | `release_bump.sh` + `check_release_governance` | 只映射到 VERSION/CMake/props，未映射到产物版本 |
| **验证** | HelloWorld `publish --mode app` 跑通 | 仅作为 smoke，未成为"SDK 正确性 gate"的正式一环 |

---

## 2. 目标形态（建立坐标系）

```
用户视角的完整链条：

  dotnet tool install chaos-il2cpp          # 1. 安装工具链（dotnet global tool / NuGet）
        │
        ▼
  chaos-il2cpp publish HelloWorld.csproj    # 2. 使用工具链发布应用
        │        │  └──── HelloWorld 发布只是"用户用工具链做一件事"的例子
        │        └──────── 工具链本身：IL→C++ 翻译 + SDK 发射 + 原生构建
        ▼
  chaos-sdk/ + entry.exe                     # 3. 产物：可链接的 SDK + 原生可执行文件
        │
        ▼
  HelloWorld/... 用户用这款 SDK 构建自己的东西

真正的"IL2CPP 产物" = 工具链（可安装）+ SDK（可链接）
"HelloWorld 发布"      = 验证工具链能把一个 .NET 应用变成本地原生应用
```

---

## 3. 三个方案（含深度分析）

### 方案 A：dotnet global tool（推荐）

**核心**：把 `Chaos.IL2CPP.Driver` 做成 `dotnet global tool`，用户 `dotnet tool install chaos-il2cpp` 后用 `chaos-il2cpp publish ...`。

**改动点**：
1. `Chaos.IL2CPP.Driver.csproj` 加 `<PackAsTool>true</PackAsTool>` + `<ToolCommandName>chaos-il2cpp</ToolCommandName>` + `<PackageId>chaos-il2cpp</PackageId>`
2. 创建 NuGet 包（`dotnet pack`）→ 产物 `.nupkg`
3. `release-flow` 把 nupkg 纳入发布产物（进 GitHub Release + NuGet.org 或私有 feed）
4. 工具在 `%USERPROFILE%\.dotnet\tools\` 安装，带一个 `chaos-il2cpp` 命令
5. 工具运行时需要定位 SDK 原生 libs —— **这是最大挑战**：需要把 56MB SDK 打进 NuGet 包（作为 `tools/` 内容），或工具在首次运行时下载/定位 SDK

```
chaos-il2cpp.nupkg
├── tools/
│   └── chaos-il2cpp.dll          # CLI
├── sdk/
│   ├── windows-x64/              # 按 RID 分发的预构建 .lib
│   ├── linux-x64/
│   └── ...                       # 5 个平台
└── headers/                      # 公共头文件
```

**优点**：
- 符合 .NET 生态惯例，用户熟悉
- `dotnet tool install` + `dotnet global tool trust` 即可安装
- 天然支持多 RID（不同平台的 `.lib`）

**缺点/挑战**：
- SDK（56MB+.lib）打进 NuGet 包会导致包体积大
- 工具运行时定位本地原生工具链（VS 的 cl.exe，确保 MSVC 版本匹配）复杂——SdkEmitter 已有 MSVC 版本匹配逻辑
- `dotnet tool` 的 RID 粒度可能不够（需要每个平台单独安装）

---

### 方案 B：CMake Package（纯 SDK）

**核心**：把 `chaos-sdk/` 完整地打包成可分发格式（zip/tar.gz），用户下载后解压，用 `find_package(chaos)` 链接。HelloWorld 作为 SDK 的验证 project。

**改动点**：
1. 把 SDK 从 `tests/e2e/translation/sdk/` 迁出到独立产物根（如 `artifacts/sdk-<platform>/`）
2. `release-flow` 为每个平台压缩成 `chaos-sdk-<platform>-<version>.zip`（含 checksums + SBOM）
3. 提供 `chaos-il2cpp` 独立二进制（不依赖 `dotnet run`）
4. HelloWorld 变成一个 `examples/HelloWorld/CMakeLists.txt`，用 `find_package(chaos)` 演示

```
chaos-sdk-windows-x64-0.1.1.zip
├── include/                    # 头文件
├── lib/                        # .lib / .a
├── chaos-config.cmake          # find_package 入口
├── runtime_stubs/              # 存根源码
└── CMakePresets.json
```

**优点**：
- SDK 与测试解耦，独立可分发
- 用户只需 zip + 头文件 + libs
- 跨平台天然（每平台一个 zip）

**缺点/挑战**：
- 用户要手动解压 + 配置 CMake，体验不如 `dotnet tool`
- 没有统一的"工具链命令"，用户可能不知道如何从 .NET 项目出发

---

### 方案 C：单文件 CLI + 独立 SDK 捆绑

**核心**：`chaos-il2cpp` 编译为原生可执行文件（或 self-contained .NET），与 SDK 捆绑在同一个 zip。用户解压得到一个 `bin/chaos-il2cpp` + `sdk/`，可直接命令。

**改动点**：
1. `dotnet publish -r <rid> --self-contained` 得单一可执行文件 `chaos-il2cpp`
2. 捆绑 SDK 到一个 self-contained release 包
3. `release-flow` 打包成 `chaos-il2cpp-<platform>-<version>.tar.gz`

**优点**：
- 用户不需要装 .NET SDK（self-contained）
- 单一产物，体验好
- 离线可用

**缺点/挑战**：
- self-contained 体积大（~100MB+）
- 仍然需要 CMake + 原生编译器才能构建用户应用（发布工具本身不解决这点）

---

## 4. 关键架构决策（需要你定的）

无论选哪个方案，有几个横切决策必须定：

### D1：SDK 原生 libs 的定位方式
- **内嵌**：打进 NuGet/tool 包（方案 A 默认，包大）
- **预编译按平台分发**：每平台一个 SDK 包（方案 B/C）
- **按需生成**：工具首次运行时从某个源下载对应平台 SDK（复杂但有弹性）

### D2：CLI 与 SDK 的关系
- CLI 本身不含 SDK，SDK 是独立下载/安装（解耦，推荐）
- CLI 捆绑一份默认 SDK（开箱即用，但大）

### D3：多少平台作为首发
- 仅 Windows x64（当前实际验证过的）—— 推荐先锁定
- Windows + Linux x64 —— 覆盖 CI 主流
- 全部 5 平台 —— Android/iOS 需要移动 host 证据，暂不适合进发布

### D4：HelloWorld 在发布流程中的位置
- 方案一：独立 `smoke` 事件，发布后手动跑（现状）
- 方案二：作为 GitHub `release.yml` 的一个 job，自动跑（推荐）：发布 SDK → 立即用刚发布的 SDK publish HelloWorld → 运行 → 断言 exit 0 → 通过才标记 Release 成功。把 HelloWorld 变成 SDK 正确性的自动化 gate

---

## 5. 推荐组合（我的建议）

**主推：方案 A（dotnet global tool）+ D2 解耦 + D3 只 Windows 首发 + D4 自动化 gate**

理由：
1. **用户拿到的是工具链**：`dotnet tool install chaos-il2cpp` + `chaos-il2cpp publish app.csproj` → 这是最自然的"IL2CPP 工具链"使用方式，与 Unity 的 il2cpp 心智对齐
2. **HelloWorld 成为 gate**：发布 SDK 后，CI 自动用该 SDK publish HelloWorld 并断言 exit 0，确保"用户拿到能用"
3. **SDK 解耦**：CLI（工具）+ 独立 SDK 包，体积可控，各平台独立发布
4. **符合 .NET 生态**：NuGet 是 .NET 工具链的标准分发渠道

**分阶段落地**：
- Phase 1（1-2 天）：CLI 打包为 dotnet tool + Windows x64 SDK 独立 zip
- Phase 2（1 天）：release.yml 增加 HelloWorld 自动化 smoke gate
- Phase 3（远期）：NuGet.org 发布、多平台

---

## 6. 需要你确认的开放问题

| # | 问题 | 我的倾向 |
|---|------|---------|
| Q1 | 选哪个主方案？A / B / C / 组合 | A（dotnet tool）+ D2 解耦 |
| Q2 | SDK 原生 libs 内嵌还是独立分发？ | 独立（每平台一个 SDK 包） |
| Q3 | 首发平台范围？ | 仅 Windows x64 |
| Q4 | HelloWorld 是否作为自动化 gate？ | 是（release.yml 加 job） |
| Q5 | 分发渠道？NuGet.org 还是只是 GitHub Release？ | 先 GitHub Release，NuGet 后补 |
| Q6 | CLI 是否 self-contained（免装 .NET SDK）？ | 先 framework-dependent（需用户 .NET 8），self-contained 后补 |

请针对 Q1-Q6 给出你的选择或修改意见，我再据此深化成可执行 plan。
