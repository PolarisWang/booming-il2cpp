# Chunk 5：平台矩阵、Harness 与 Verify Gate

## 目标

把 host-specific matrix、reference desktop gate、Android/iOS/Linux 平台强度和 `verify-roadmap-0` 脚本真正串起来。

### 任务 1：建立 toolchain 描述与平台矩阵文档

**文件：**
- 创建：`CMakePresets.json`
- 创建：`build/toolchains/windows-x64-reference.cmake`
- 创建：`build/toolchains/macos-reference.cmake`
- 创建：`build/toolchains/android-arm64.cmake`
- 创建：`build/toolchains/ios-arm64.cmake`
- 创建：`build/toolchains/linux-x64.cmake`
- 创建：`wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/roadmap-0/toolchain-matrix.md`
- 创建：`wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/roadmap-0/reference-desktop-gates.md`

- [ ] **步骤 1：为 5 类 target 写 toolchain 描述**

要求覆盖：
- Windows x64 reference
- macOS reference
- Android arm64
- iOS arm64
- Linux x64 cross-compile

- [ ] **步骤 2：写 `CMakePresets.json`**

至少包含：
- Windows reference preset
- macOS reference preset
- Android smoke preset
- iOS packaging preset
- Linux packaging preset

- [ ] **步骤 3：写 `toolchain-matrix.md` 与 `reference-desktop-gates.md`**

明确：
- host-specific matrix
- 双 reference desktop
- Windows 主线基准集
- macOS 同语义子集
- Android/iOS/Linux 各自 gate 强度

- [ ] **步骤 4：检查 preset 是否可枚举**

运行：
```powershell
cmake --list-presets
```

预期：新预设可见。

### 任务 2：建立平台 harness 与 reference desktop gate 外壳

**文件：**
- 创建：`tests/platform/android-smoke/README.md`
- 创建：`tests/platform/android-smoke/CMakeLists.txt`
- 创建：`tests/platform/android-smoke/smoke_entry.cpp`
- 创建：`tests/platform/ios-smoke/README.md`
- 创建：`tests/platform/ios-smoke/CMakeLists.txt`
- 创建：`tests/platform/ios-smoke/smoke_entry.mm`
- 创建：`tests/reference-desktop/windows/README.md`
- 创建：`tests/reference-desktop/macos/README.md`

- [ ] **步骤 1：创建平台 harness 目录**

运行：
```powershell
New-Item -ItemType Directory tests/platform/android-smoke,tests/platform/ios-smoke,tests/reference-desktop/windows,tests/reference-desktop/macos -Force
```

- [ ] **步骤 2：实现 Android 真启动 smoke 外壳**

要求：
- 能表达真实 startup smoke 路径
- 输出进入 `artifacts/`

- [ ] **步骤 3：实现 iOS compile/link/packaging 外壳**

要求：
- 不要求 `Roadmap 0` 完整 iOS runtime 语义
- 但必须表达完整 packaging path

- [ ] **步骤 4：为 Windows/macOS reference desktop 写 gate 说明**

说明：
- Windows 走高阶主线基准集
- macOS 走接近同范围的高阶子集
- 平台能力相关 `icall` 只做到 basic-path

### 任务 3：把 trace contract 接到 lazy warmup 验证

**文件：**
- 创建：`tests/contracts/trace/compare-warmup-trace.ps1`
- 创建：`tests/contracts/trace/compare-warmup-trace.sh`
- 修改：`tests/smoke/input/HostEmbeddingLite/Program.cs`
- 修改：`tests/reference-desktop/windows/README.md`
- 修改：`tests/reference-desktop/macos/README.md`

- [ ] **步骤 1：编写 trace 比较脚本**

要求：
- 读取 `tests/contracts/trace/snapshots/*.json`
- 对比运行期 trace 输出
- 验证 `eventName/phase/subjectKind/subjectId/order/status`

- [ ] **步骤 2：在 `HostEmbeddingLite` 中固定 lazy warmup 触发路径**

要求：
- 能按稳定顺序触发 class world / reflection / generic / delegate warmup
- 能导出结构化 JSON trace

- [ ] **步骤 3：把 Windows/macOS reference desktop 的 trace 比较方式写进 README**

要求：
- 写清触发场景
- 写清对比命令
- 写清哪些 `icall` 差异允许留白

### 任务 4：实现 `verify-roadmap-0`

**文件：**
- 创建：`build/scripts/verify-roadmap-0.ps1`
- 创建：`build/scripts/verify-roadmap-0.sh`
- 创建：`wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/roadmap-0/local-verification.md`

- [ ] **步骤 1：让 PowerShell 版支持 host-specific matrix**

要求：
- 支持 `-HostProfile windows`
- 支持 `-HostProfile macos`
- 调用顺序固定

- [ ] **步骤 2：把以下检查串入 PowerShell 版**

必须拆成 `common` 与 `host-specific` 两组：

`common`：
- schema/example/snapshot 解析检查
- trace schema/snapshot 检查
- native ABI smoke
- native bridge smoke
- 5 个 smoke 项目 build
- Linux cross-compile/packaging

`HostProfile=windows`：
- Windows reference desktop gate
- Android 真启动 smoke

`HostProfile=macos`：
- macOS reference desktop gate
- iOS compile/link/packaging

- [ ] **步骤 3：实现 shell 版脚本**

要求：
- 与 PowerShell 版保持相同的 `common + host-specific` 调度结构
- 方便 macOS host 复用

- [ ] **步骤 4：写 `local-verification.md`**

至少说明：
- Windows host 怎么跑
- macOS host 怎么跑
- 哪些检查属于 `common`
- 哪些检查属于 `windows` / `macos` profile 专属
- Android 需要哪些前置
- iOS 需要哪些前置
- Linux cross-compile/packaging 如何进入矩阵
- trace diff 输出在 `artifacts/` 的哪里

- [ ] **步骤 5：在 Windows profile 下跑一次主线**

运行：
```powershell
pwsh ./build/scripts/verify-roadmap-0.ps1 -HostProfile windows
```

预期：
- Windows reference desktop gate 跑通
- Android startup smoke 被调度
- Linux packaging gate 被调度

- [ ] **步骤 6：在 macOS profile 下跑一次主线**

运行：
```bash
pwsh ./build/scripts/verify-roadmap-0.ps1 -HostProfile macos
```

预期：
- macOS reference desktop gate 跑通
- iOS packaging gate 被调度
- Linux packaging gate 被调度

- [ ] **步骤 7：提交当前 chunk**

运行：
```bash
git add CMakePresets.json build/toolchains build/scripts tests/platform tests/reference-desktop tests/contracts/trace wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/roadmap-0
git commit -m "feat: add roadmap0 platform gates and verify pipeline"
```

预期：提交包含平台矩阵、harness、trace compare 和 verify 脚本。
