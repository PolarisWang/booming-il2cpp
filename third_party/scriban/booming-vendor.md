# scriban vendor metadata

## 来源

- 上游项目：`scriban/scriban`（https://github.com/scriban/scriban）
- 当前目录内容为 vendored snapshot，路径：`third_party/scriban`
- 上游版本：v7.2.6（对应 NuGet PackageReference 版本）

## 许可证

- `BSD-2-Clause`
- 依据：`third_party/scriban/src/Scriban/Scriban.props` 中 `PackageLicenseExpression` 与 `third_party/scriban/license.txt`

## 用途

- 作为 .NET 模板/脚本引擎依赖，用于文本模板与脚本能力。

## 版本 / commit

- 上游 tag：`v7.2.6`（https://github.com/scriban/scriban/releases/tag/v7.2.6）
- 本仓库导入锚点 commit：`d46a2c6e9`（`chore: migrate scriban into third_party and align roadmap0 docs`）
- 初始导入后本地修改清单（`d46a2c6e9..HEAD`）：
  - `src/Scriban/Scriban.props`：调整 TargetFrameworks 为 net8.0-only，添加 AOT 兼容性开关
  - `src/Scriban.DocGen/Program.cs`：修复 README.md 大小写（文档生成，不影响运行时）
  - `src/Directory.Build.props`：构建配置调整
  - `README.md` / `site/`：文档大小写修正
  - `booming-vendor.md` / `AGENTS.md`：仓库元数据
  - 核心引擎（`src/Scriban/*.cs` 引擎代码）**未修改**
