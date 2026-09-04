# Configuration Map — 配置集中化地图

> 最后更新：2026-09-04
> 对应 Phase 1-3 配置集中化完成状态

## 设计原则

**技术栈内单一真源**（Single Source of Truth per stack）。bash、GitHub Actions、CMake 三套技术栈无法合并，各自维护一个 SSOT 文件，其他文件从该源读取。

```
bash 脚本层          → release-config.sh
GitHub Actions CI   → GitHub repo vars (Settings → Actions → Variables)
CMake 构建           → CMakePresets.json + cmake/*.cmake
版本号               → VERSION (特殊:由 release_bump.sh 同步到 CMakeLists + Directory.Build.props)
```

---

## 1. Bash 发布脚本 — `scripts/release-config.sh`

### 来源
被 `release.sh`、`release_bump.sh`、`build-tool-package.sh`、`publish-nuget.sh`、`generate-*.sh` 等 `source`。

### 常量清单

| 常量 | 值 | 用途 | 消费者 |
|------|-----|------|--------|
| `RC_VERSION_FILE` | `VERSION` | 版本文件路径 | build-tool, publish-nuget |
| `RC_SEMVER_RE` | `^[0-9]+...` | SemVer 校验正则 | release.sh init |
| `RC_TAG_PREFIX` | `v` | 标签前缀 | release_bump.sh |
| `RC_RELEASE_BRANCH_PREFIX` | `release/` | 分支命名 | release.sh |
| `RC_REPO` | `PolarisWang/booming-il2cpp` | 仓库 slug | release.sh, notify-feishu |
| `RC_REPO_URL` | `https://github.com/...` | 仓库 URL base | generate-release-notes |
| `RC_DOTNET_BUILD_TIMEOUT` | `120000` ms | csproj 构建超时 | (未直接引用) |
| `RC_DOTNET_TEST_TIMEOUT` | `300` s | dotnet test 超时 | release.sh verify |
| `RC_TPG_TIMEOUT` | `1800000` ms | TPG 超时 | (未直接引用) |
| `RC_CI_TIMEOUT` | `120` s | hygiene 门禁 | (未直接引用) |
| `RC_PRESET_WIN_X64` | `windows-x64-reference` | Windows 预设名 | build-tool-package |
| `RC_PRESET_LINUX_X64` | `linux-x64-profile` | Linux 预设名 | (未使用，C# 代码引用) |
| `RC_PRESET_LINUX_X64_BUILD` | `linux-x64-native` | Linux 构建预设 | (未使用) |
| `RC_PRESET_LINUX_ARM64` | `linux-arm64-profile` | ARM64 预设 | (未使用) |
| `RC_PRESET_OSX_X64` | `osx-x64-profile` | macOS 预设 | (未使用) |
| `RC_PRESET_OSX_ARM64` | `osx-arm64-profile` | macOS ARM 预设 | (未使用) |
| `RC_ARTIFACTS_BASE` | `$REPO_ROOT/artifacts` | 产物根目录 | release.sh, build-tool, publish-nuget |
| `RC_RELEASE_DIR` | `$RC_ARTIFACTS_BASE/release` | 发布产物目录 | (派生) |
| `RC_STUB_MAX_BYTES` | `8` | 空 stub 最大字节 | (未直接引用) |
| `RC_SBOM_SPEC` | `1.5` | CycloneDX 版本 | generate-sbom.sh |
| `RC_SBOM_FILENAME` | `sbom.cyclonedx.json` | SBOM 文件名 | generate-sbom.sh |
| `RC_CHECKSUM_FILENAME` | `SHA256SUMS` | 校验和文件名 | generate-checksums.sh |
| `RC_FEISHU_SCRIPT` | `scripts/notify-feishu.sh` | 飞书通知脚本 | (未直接引用，历史) |

### 修改方式
```bash
vim scripts/release-config.sh   # 改值
bash scripts/release.sh --dry-run init 0.3.0   # 验证
```

---

## 2. GitHub repo vars — CI 工作流配置

### 来源
GitHub 仓库 → Settings → Secrets and variables → Actions → Variables。
通过 `${{ vars.X }}` 在 workflow 中引用，提供 `|| 'default'` 回退（fork 友好）。

### 已创建变量（10 个）

| 变量 | 值 | 用途 | 引用工作流 |
|------|-----|------|-----------|
| `DOTNET_VERSION` | `8.0.x` | .NET SDK 版本 | release.yml |
| `RUNNER_WIN` | `windows-latest` | Windows runner | release.yml |
| `RUNNER_LINUX` | `ubuntu-latest` | Linux runner | release.yml |
| `CHECKOUT_DEPTH` | `0` | `actions/checkout` fetch-depth | release.yml |
| `CHECKOUT_SUBMODULES` | `recursive` | `actions/checkout` submodules | release.yml |
| `PRESET_WIN_X64` | `windows-x64-reference` | Windows cmake preset | release.yml |
| `PRESET_LINUX_X64` | `linux-x64-native` | Linux cmake preset | (待用) |
| `ACTIONS_CHECKOUT_VER` | `v4` | `actions/checkout@` 版本 | release.yml |
| `ACTIONS_SETUP_DOTNET_VER` | `v4` | `actions/setup-dotnet@` 版本 | release.yml |
| `ACTIONS_UPLOAD_ARTIFACT_VER` | `v4` | `actions/upload-artifact@` 版本 | release.yml |

### 修改方式
```bash
# 本地 CLI
gh variable set DOTNET_VERSION --repo PolarisWang/booming-il2cpp --body "9.0.x"

# 或 GitHub UI: Settings → Secrets and variables → Actions → Variables
```

### 待扩展
当前只有 `release.yml` 试点引用。后续新 pipeline workflow 或 stage 需要时，直接读这些 vars，不再硬编码。

---

## 3. CMake / 构建配置

### 3a. CMakePresets.json — 预设定义
**文件**：`CMakePresets.json`

14 个 configurePreset，每个定义：
- `generator`（Visual Studio 17 2022 / Ninja / Xcode）
- `binaryDir`（`artifacts/presets/<name>`）
- `toolchainFile`（`build/toolchains/<name>.cmake`）
- `cacheVariables`（`ROADMAP0_PRESET_TARGET`、`CHAOS_IL2CPP_EXEC_PROFILE` 等）

### 3b. CMakeLists.txt — 构建路由
**文件**：`CMakeLists.txt`

`ROADMAP0_PRESET_TARGET` 分支路由（各 preset 的 arm64 JIT 测试套件已抽到共享 `cmake/arm64-jit-test.cmake`，CMakeLists 显著瘦身）：
```
windows-x64-reference   → 全平台构建（含测试）
macos-reference         → macOS 构建
android-arm64-smoke     → Android 测试
ios-arm64-packaging     → iOS 打包
linux-x64-packaging     → Linux 打包
linux-arm64-native      → ARM64 全构建 (含 JIT 测试)
linux-arm64-smoke       → ARM64 精简构建 (含 JIT 测试)
linux-x64-native        → Linux x64 构建
linux-x64-reference     → (与 linux-x64-native 同分支)
```

### 3c. 共享 CMake 模块

| 文件 | 行数 | 用途 |
|------|------|------|
| `cmake/config-tier.cmake` | 31 | debug/profile/ship 配置等级映射 |
| `cmake/chaos_native_test.cmake` | — | `chaos_native_add_test` 函数定义 |
| `cmake/arm64-jit-test.cmake` | 192 | ARM64 JIT 测试套件（native+smoke 共用） |

### 3d. 工具链文件
`build/toolchains/windows-x64-reference.cmake`、`linux-x64-native.cmake` 等—每个定义一个 `ROADMAP0_TARGET_NAME` + 编译器 + 系统 + 处理器。

### 修改方式
```bash
# 改预设
vim CMakePresets.json
# 改构建路由或添加共享模块
vim CMakeLists.txt
vim cmake/arm64-jit-test.cmake
# 改工具链
vim build/toolchains/linux-x64-native.cmake
```

---

## 4. 版本号（特殊三源）

### 来源与同步
```
VERSION  (SSOT)
  ├── CMakeLists.txt          (set(CHAOS_VERSION "..."))
  └── Directory.Build.props   (<Version>...</Version>)
```

**强制同步工具**：`scripts/release_bump.sh`（唯一合法修改方式）

**强制一致性门禁**：`scripts/cleanliness/check_release_governance.py`（pre-commit + CI 硬阻断）

### 修改方式
```bash
# 唯一合法方式
scripts/release_bump.sh 0.3.0
# 同步到三源
scripts/release_bump.sh 0.3.0 --tag
# 验证一致性
scripts/cleanliness/check_release_governance.py --gate
```

**禁止**：手动编辑 `VERSION`、`CMakeLists.txt` 或 `Directory.Build.props` 中的版本号。

---

## 5. 飞书通知密钥

不存储在代码仓库中。通过 GitHub secrets 注入：

| secret | 用途 | 设置位置 |
|--------|------|---------|
| `FEISHU_WEBHOOK` | 飞书自定义机器人 webhook URL | GitHub → Settings → Secrets |
| `FEISHU_SECRET` | 签名密钥 | GitHub → Settings → Secrets |
| `NUGET_API_KEY` | NuGet.org API key（未设置） | GitHub → Settings → Secrets |

---

## 6. 修改建议

| 场景 | 改哪里 |
|------|--------|
| 改 .NET SDK 版本 | `gh variable set DOTNET_VERSION` + 更新 C# 项目文件 |
| 改 runner 镜像 | `gh variable set RUNNER_WIN` |
| 改预设名 | CMakePresets.json + release-config.sh + CMakeLists 分支（三处同步） |
| 改发布产物路径 | `release-config.sh`→`RC_ARTIFACTS_BASE` |
| 改仓库 slug | `release-config.sh`→`RC_REPO` |
| 改飞书 webhook | GitHub Secrets：`FEISHU_WEBHOOK` |
| 改版本号 | `scripts/release_bump.sh` |
| 改 CMake 分支 | `CMakeLists.txt` + 对应 preset + toolchain |
| 改超时 | `release-config.sh` 中各 RC_*_TIMEOUT |
| 改 actions 版本 | `gh variable set ACTIONS_CHECKOUT_VER` |