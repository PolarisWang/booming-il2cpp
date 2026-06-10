# Crypto Interpreter Routing — 现状分析报告

> 生成时间: 2026-06-10
> 分析范围: testing/foundation-dll/System.Security.Cryptography/
> 分析方法: 搜索 ChaosExternalRuntimeFallback 引用 + 统计 generated code 中的 IL 数据嵌入

## 1. 总览

| 指标 | 数值 |
|------|------|
| 外部运行时表总条目 | **1,753** |
| 已嵌入 IL 数据的方法 | **164** (21 + 143) |
| 其中 crypto 相关 IL 数据 | **143** (全部在 x509 块) |
| 仍在走 fallback 的 crypto 方法 | **~834** |
| Interpreter 路由覆盖率(crypto) | **~14.6%** |

## 2. Chunk 级别详情

| Chunk | 总 Subjects | IL 数据 | Crypto Subjects | Crypto IL | 覆盖率 |
|-------|------------|---------|----------------|-----------|--------|
| `security-cryptography` | 1,753 | 21 | 408 | **0** | **0%** |
| `security-cryptography-2` | 同 chunk 拆分 | 21 | 408 | **0** | **0%** |
| `x509certificates` | 161 | 144 | 161 | **143** | **88.8%** |

## 3. 关键发现

### 3.1 Core Crypto (AES/RSA/SHA/TripleDES 等) 全部未路由
`security-cryptography` 块中的 **408 个 crypto 方法** 全部走 `ChaosExternalRuntimeFallback`。这些包括:
- `System.Security.Cryptography.Aes*`
- `System.Security.Cryptography.RSA*`
- `System.Security.Cryptography.SHA*`
- `System.Security.Cryptography.TripleDES*`
- `System.Security.Cryptography.DSA*`
- `System.Security.Cryptography.HMACSHA*`
- `System.Security.Cryptography.PBKDF2*`
- 等等

### 3.2 x509Certificates 已部分路由
`x509certificates` 块的 **143 / 161 个方法** 已嵌入 IL 数据 (剩余的 18 个可能在 Subjects 数组以外)。覆盖方法包括:
- `X509Certificate2` 相关方法 (Import/Export/Verify)
- `CertificateRequest` 相关方法
- `X509Chain`、`X509Store`、`X509ExtensionCollection` 等

### 3.3 基础设施已就绪
近期提交 (053f5994, ab12a877) 已添加:
- `AllManagedMethods` 数据收集 — 从 CodeGenStage 收集所有 managed 方法
- `BuildCryptoAotIrCode` — 生成 IL 嵌入 C++ 代码
- Header decls 和 data table 结构
- `s_il_*` 数组 + `ChaosIlDataEntry` 表结构 (21 个非 crypto 示例已嵌入)

### 3.4 与设计目标对比
设计文档 (24-Crypto解释器路由方案决策.md) 规划的 **360 个 crypto 方法**，目前仅覆盖 **143 个 x509 方法**，剩余约 **217 个 core crypto 方法** 仍待路由。

## 4. 建议下一步

| 优先级 | 任务 | 涉及域 |
|--------|------|--------|
| P0 | 将 core crypto 方法(408 个)纳入 `BuildCryptoAotIrCode` 数据收集范围 | CodeGen |
| P0 | 确保 core crypto 的 IL 数据正确发射到 `native-aot.generated.cpp` | CodeGen |
| P1 | 验证 x509 的 143 个方法是否正常通过 interpreter 执行 | 运行时 |
| P1 | 修复 x509 剩余 18 个未覆盖方法的 fallback | CodeGen + 运行时 |
| P2 | 端到端验证: foundation-dll 全量 crypto 测试通过 interpreter 路由 | 测试 |

## 5. 文件清单

| 文件 | 说明 |
|------|------|
| `src/managed/Chaos.IL2CPP.Generator/RuntimeSupport/NativeAotLoweringPlanner.ExternalRuntimeHelpers.cs` | 外部 runtime helper 定义 + IL 数据收集 |
| `src/native/runtime-core/runtime_stubs/interop_stubs.h/.cpp` | ChaosExternalRuntimeFallback 实现 |
| `testing/.../security-cryptography/native/subjects/native-aot.generated.cpp` | core crypto 生成代码 (408 subjects, 0 IL data) |
| `testing/.../system-security-cryptography-x509certificates/.../native-aot.generated.cpp` | x509 生成代码 (143/161 IL data) |
