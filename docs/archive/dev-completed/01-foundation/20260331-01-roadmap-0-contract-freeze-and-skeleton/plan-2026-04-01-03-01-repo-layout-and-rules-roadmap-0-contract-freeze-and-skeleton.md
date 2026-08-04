# Chunk 1：仓库骨架、第三方规则与产物目录

## 目标

先把仓库骨架、第三方依赖规则、产物目录规则和 host-specific verify 矩阵框架落盘，给后续 chunk 一个稳定落点。

### 任务 1：创建顶层目录与 README 骨架

**文件：**
- 创建：`frontend/README.md`
- 创建：`analysis/README.md`
- 创建：`runtime/README.md`
- 创建：`codegen/README.md`
- 创建：`build/README.md`
- 创建：`tests/README.md`
- 创建：`contracts/README.md`
- 创建：`wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/roadmap-0/README.md`
- 创建：`wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/roadmap-0/repo-layout.md`
- 创建：`wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/roadmap-0/ownership-map.md`

- [ ] **步骤 1：创建顶层目录结构**

运行：
```powershell
New-Item -ItemType Directory frontend,analysis,runtime,codegen,build,tests,contracts,wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/roadmap-0 -Force
```

预期：目录全部存在，重复执行不报错。

- [ ] **步骤 2：为每个顶层目录写最小 README**

要求每个 README 至少包含：
- 本目录负责什么
- 本目录不负责什么
- 它与其它目录的边界

可复用骨架：
```md
# <dir>

## 负责

- ...

## 不负责

- ...

## 与其它目录的边界

- ...
```

- [ ] **步骤 3：写 `wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/roadmap-0/README.md`**

至少列出：
- repo skeleton
- schema pack
- native contract pack
- smoke inputs
- trace contract
- host-specific verify matrix

- [ ] **步骤 4：写 `repo-layout.md` 和 `ownership-map.md`**

要求：
- 把 `frontend/analysis/runtime/codegen/build/tests/contracts/third_party/artifacts` 的责任写清
- 给未来工作轨一个明确归属说明

- [ ] **步骤 5：检查目录与文档落盘**

运行：
```powershell
rg --files frontend analysis runtime codegen build tests contracts wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/roadmap-0
```

预期：所有 README 和架构文档都能被列出。

### 任务 2：建立 `third_party/` 与 `artifacts/` 规则

**文件：**
- 创建：`third_party/README.md`
- 创建：`third_party/INDEX.md`
- 创建：`artifacts/README.md`
- 创建：`artifacts/.gitignore`
- 修改：`.gitignore`

- [ ] **步骤 1：创建 `third_party/` 和 `artifacts/`**

运行：
```powershell
New-Item -ItemType Directory third_party,artifacts -Force
```

预期：两个目录存在。

- [ ] **步骤 2：写 `third_party/README.md`**

必须明确：
- 这里只放 vendored 源码
- 不放工具链、下载缓存、构建产物
- 默认接入方式是手动 vendor snapshot
- 每个依赖目录必须自带 `README.md`

- [ ] **步骤 3：写 `third_party/INDEX.md`**

先建立空表头，字段固定为：
- 名称
- 版本 / commit
- 许可证
- 用途
- 目录路径

- [ ] **步骤 4：写 `artifacts/README.md` 与 `artifacts/.gitignore`**

要求说明：
- `artifacts/` 收口 build 目录、日志、打包产物、运行期快照副本与 diff 输出
- 权威 snapshot 不放这里

`.gitignore` 至少包含：
```gitignore
*
!.gitignore
!README.md
```

- [ ] **步骤 5：把 `artifacts/` 与 `third_party/` 规则补进仓库根 `.gitignore`**

运行检查：
```powershell
Get-Content .gitignore
```

预期：不会误提交 `artifacts/` 下的运行输出。

### 任务 3：建立 host-specific verify matrix 框架文档

**文件：**
- 创建：`wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/roadmap-0/host-verify-matrix.md`
- 创建：`wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/roadmap-0/platform-gates.md`

- [ ] **步骤 1：写 `host-verify-matrix.md`**

明确：
- Windows host 跑 `Windows + Android + Linux(cross-compile/packaging)`
- macOS host 跑 `macOS + iOS + Linux(cross-compile/packaging)`

- [ ] **步骤 2：写 `platform-gates.md`**

明确：
- 双 reference desktop：`Windows x64 + macOS`
- Windows 是高阶 contract 主线
- macOS 跟同语义子集
- Android 真启动
- iOS compile/link/packaging
- Linux cross-compile/packaging

- [ ] **步骤 3：交叉检查这些文档与设计入口一致**

运行：
```powershell
rg -n "Windows|macOS|Android|iOS|Linux|reference desktop" docs/booming/2026-03-31-roadmap-0-contract-freeze-and-skeleton/design-2026-04-01-03-*.md wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/roadmap-0/*.md
```

预期：术语和矩阵定义不冲突。

- [ ] **步骤 4：提交当前 chunk**

运行：
```bash
git add frontend analysis runtime codegen build tests contracts third_party artifacts wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/roadmap-0 .gitignore
git commit -m "chore: scaffold roadmap0 repo layout and rules"
```

预期：提交只包含骨架、README、矩阵与规则文档。
