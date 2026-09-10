# 第 5 段：Verify、Trace Contract 与验证资产组织

## verify-roadmap-0

`verify-roadmap-0` 是全量硬 gate，但按 host OS 分子矩阵执行：

- Windows 开发机：`Windows + Android + Linux(cross-compile/packaging)`
- macOS 开发机：`macOS + iOS + Linux(cross-compile/packaging)`

Reference desktop 上还叠加高阶 ABI/bridge contract 的 end-to-end 语义验证。

## 验证资产的三层结构

### 1. 正式 contract 资产

- schema/example/snapshot
- native contract pack

### 2. 验证 contract

- native compile-only smoke
- trace / event contract

### 3. 运行期输出

- `artifacts/` 中的 build 输出
- 日志
- 运行期快照副本
- diff 输出

## Warmup Trace / Event Contract

### 定位

warmup trace / event log 进入 `Roadmap 0` 的验证 contract，但不提升为与核心 `ABI/bridge` 同等级的长期核心 contract。

### 位置

- schema：`tests/contracts/trace/schema/`
- 权威 snapshot：`tests/contracts/trace/snapshots/`

### 格式

采用结构化 JSON。

### 字段

至少包含：

- `eventName`
- `phase`
- `subjectKind`
- `subjectId`
- `order`
- `status`

### 枚举

`subjectKind`：

- `type`
- `method`
- `field`
- `property`
- `event`
- `parameter`
- `genericContext`
- `delegate`
- `assembly`

`phase`：

- `requested`
- `started`
- `completed`
- `failed`
- `skipped`
- `cached`
- `replayed`

`status`：

- `ok`
- `error`
- `skipped`
- `cached`

### 命名风格

`eventName` 采用命名空间风格：

- 默认三段：`<domain>.<subject>.<action>`
- 必要时两段：`<domain>.<action>`

`domain` 集合包含：

- `warmup`
- `metadata`
- `reflection`
- `generic`
- `delegate`
- `bootstrap`
- `interop`

### SubjectId

`subjectId` 采用规范化、人读签名式字符串。
至少包含：

- 程序集
- 类型
- 成员签名

在泛型场景下还需带上实例化形态。

## Lazy Warmup 验证

最后一段 warmup 采用全 lazy 策略：

- startup 只建立最小壳
- class world / reflection / generic / delegate 预热按访问路径触发

为了验证这套 lazy 行为：

- 验证不仅看最终结果
- 还要检查触发顺序
- 因此必须暴露 warmup trace / event log

## 结论

这套验证组织使 `Roadmap 0` 的 gate 不再只是“能编、能启动”，而是把部分运行期初始化行为也收敛成可比对、可 snapshot、可 diff 的验证 contract。
