# Chunk 2：Schema Pack 与 Trace Contract

## 目标

冻结正式 schema/example/snapshot，并把 warmup trace 作为验证 contract 单独成组落盘。

### 任务 1：建立 schema pack 骨架与 example

**文件：**
- 创建：`analysis/contracts/schemas/metadata-graph.schema.json`
- 创建：`analysis/contracts/schemas/typed-il-ir.schema.json`
- 创建：`analysis/contracts/schemas/aot-manifest.schema.json`
- 创建：`analysis/contracts/schemas/code-registration.schema.json`
- 创建：`analysis/contracts/schemas/metadata-registration.schema.json`
- 创建：`analysis/contracts/schemas/preserve-descriptor.schema.json`
- 创建：`analysis/contracts/examples/metadata-graph.min.json`
- 创建：`analysis/contracts/examples/typed-il-ir.min.json`
- 创建：`analysis/contracts/examples/aot-manifest.min.json`
- 创建：`analysis/contracts/examples/code-registration.min.json`
- 创建：`analysis/contracts/examples/metadata-registration.min.json`
- 创建：`analysis/contracts/examples/preserve-descriptor.min.json`
- 创建：`tests/contracts/schema/metadata-graph.snapshot.json`
- 创建：`tests/contracts/schema/typed-il-ir.snapshot.json`
- 创建：`tests/contracts/schema/aot-manifest.snapshot.json`
- 创建：`tests/contracts/schema/code-registration.snapshot.json`
- 创建：`tests/contracts/schema/metadata-registration.snapshot.json`
- 创建：`tests/contracts/schema/preserve-descriptor.snapshot.json`
- 创建：`wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/roadmap-0/schema-pack-v0.md`

- [ ] **步骤 1：创建 schema/example/snapshot 目录**

运行：
```powershell
New-Item -ItemType Directory analysis/contracts/schemas,analysis/contracts/examples,tests/contracts/schema -Force
```

- [ ] **步骤 2：编写 6 个 schema 的最小稳定骨架**

要求：
- 顶层 `formatVersion` 固定为 `v0`
- `metadata-graph` 和 `typed-il-ir` 只冻结最小稳定骨架，不冻结全部细字段
- registration / preserve schema 要包含最小必须字段

示例骨架：
```json
{
  "formatVersion": "v0"
}
```

- [ ] **步骤 3：为每个 schema 编写一个人维护 example**

要求：
- example 是可读最小样例
- 不使用随机生成数据
- example 能体现对应 contract 的意图

- [ ] **步骤 4：把 example 转成权威 snapshot**

要求：
- snapshot 与 example 分离存放
- snapshot 代表当前 canonical baseline

- [ ] **步骤 5：写 `schema-pack-v0.md`**

至少写清：
- 每个 schema 的目的
- 哪些字段是最小冻结
- 哪些字段允许追加
- 哪些字段名禁止随意改动

- [ ] **步骤 6：运行 JSON 解析检查**

运行：
```powershell
Get-ChildItem analysis/contracts/schemas/*.json,analysis/contracts/examples/*.json,tests/contracts/schema/*.json | ForEach-Object { Get-Content $_ | ConvertFrom-Json | Out-Null }
```

预期：所有 JSON 都能被解析。

### 任务 2：建立 warmup trace verification contract

**文件：**
- 创建：`tests/contracts/trace/schema/warmup-trace.schema.json`
- 创建：`tests/contracts/trace/snapshots/windows-warmup-trace.snapshot.json`
- 创建：`tests/contracts/trace/snapshots/macos-warmup-trace.snapshot.json`
- 创建：`wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/roadmap-0/trace-contract-v0.md`

- [ ] **步骤 1：创建 trace 目录**

运行：
```powershell
New-Item -ItemType Directory tests/contracts/trace/schema,tests/contracts/trace/snapshots -Force
```

- [ ] **步骤 2：编写 warmup trace schema**

至少包含字段：
- `eventName`
- `phase`
- `subjectKind`
- `subjectId`
- `order`
- `status`

并约束：
- `phase` = `requested|started|completed|failed|skipped|cached|replayed`
- `status` = `ok|error|skipped|cached`
- `subjectKind` = `type|method|field|property|event|parameter|genericContext|delegate|assembly`

- [ ] **步骤 3：把命名规则和 subjectId 规则写进 schema 注释或文档**

要求：
- `eventName` 默认三段 `<domain>.<subject>.<action>`
- 必要时两段 `<domain>.<action>`
- `domain` 允许：`warmup|metadata|reflection|generic|delegate|bootstrap|interop`
- `subjectId` 采用人读签名式，至少包含程序集 / 类型 / 成员签名，泛型场景带实例化形态

- [ ] **步骤 4：编写 Windows/macOS 权威 trace snapshot**

要求：
- 至少各有一份 sample
- 覆盖 lazy warmup 路径
- 覆盖 `cached` 或 `replayed` 一类路径中的至少一种

- [ ] **步骤 5：写 `trace-contract-v0.md`**

说明：
- 这组 trace 是验证 contract，不是核心 ABI/bridge contract
- 用途是验证 lazy warmup 顺序与最终结果

- [ ] **步骤 6：运行 trace schema 解析检查**

运行：
```powershell
Get-ChildItem tests/contracts/trace/schema/*.json,tests/contracts/trace/snapshots/*.json | ForEach-Object { Get-Content $_ | ConvertFrom-Json | Out-Null }
```

预期：schema 与 snapshot 都可解析。

- [ ] **步骤 7：提交当前 chunk**

运行：
```bash
git add analysis/contracts tests/contracts/schema tests/contracts/trace wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/roadmap-0
git commit -m "docs: add roadmap0 schema and trace contracts"
```

预期：提交只包含 schema/example/snapshot/trace contract 资产。
