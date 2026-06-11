# 23 — LCAC 跨程序集编译决策

> 来源：`MEMORY.md > lcac-complete.md`

## 决策

LCAC (Layered Cross-Assembly Compilation) 替代 BridgeAOT，作为跨程序集编译的唯一方案。

## 背景

BridgeAOT 的 IL 解码异常导致堆损坏，且不支持增量编译。LCAC 用 Demeter Table O(1) 间接调度替代 BridgeAOT 的 IL 解码方案。

## 方案要点

- **Phase 1**: 禁用 BridgeAOT + Demeter Table 补全
- **Phase 2**: 清理 bridge 残留代码 (BridgeAotCompiler.cs + IlBytecodeDecoder.cs 删除)
- **Phase 3**: Hephaestus Lib 构建缓存 (SHA-256 LRU cache)
- **Phase 4**: Gold Direct Link 热点编译 (PGO + direct calls)
- **Phase 5**: Wiki 文档更新 + 风险评估报告
- **Phase 6**: Cryptography Runtime Stubs (BCrypt RNG/SHA/HMAC) +80 fixes

## 约束

- 支持任意数量 DLL（BridgeAOT 受限于 IL 解码稳定性）
- 修复清单包括 C3861/C2995/C2712/C2589/C2059/C2065 等全部 MSVC 兼容性问题
- 65 个 crypto 方法 (DSA/ECDiffieHellman/ECDsa) 需要完整 native 加密库，不在 LCAC 范围内

## TRIGGER

涉及跨程序集编译、新增 DLL 支持、或需要理解 LCAC 翻译路径时。
