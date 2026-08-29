# 编码参考（非规则，需要时查阅）

## 四层架构红线明细

| 层 | 路径 | 允许产出 | 红线 |
|----|------|---------|------|
| **ATG** | `src/tools/Chaos.IL2CPP.Tools.AutoTestGenerator/` | `.cs`, `.csproj`, `.json` | 不得生成 C++ |
| **Codegen** | `src/managed/Chaos.IL2CPP.Generator/` | `.generated.cpp`, `.generated.h`, `.json` | 产出必须自包含 |
| **TPG** | `src/tools/Chaos.IL2CPP.Tools.TestProjectGenerator/` | `.cpp`, `.h` (via Scriban), `.cmake` | 不得修改 `.generated.*` |
| **Python** | `tests/e2e/verification/` | `.py`, `.json`, `.yaml` | 不得 `write_text` 写入 `.cpp`/`.h` |

跨层写入必须加 `# BOUNDARY_OVERRIDE: issues/NNN` + `Reason:` + `Expires:`。

## 统一内存分配细则

1. **禁止全局 `operator new/delete` 重载** — 污染引擎分配行为
2. **codegen 输出必须用 `CHAOS_IL2CPP_` 宏**（`CHAOS_IL2CPP_NEW_GC` / `CHAOS_IL2CPP_MALLOC`）
3. **禁止跨域分配/free 不匹配** — GC 域→GC 回收，Domain 域→heap->Destroy()，Raw 域→free
4. 新分配策略通过 `GcAllocate`/`GcAllocateAtomic` 内部切换，对 codegen 透明

详见 `wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/17-统一内存分配体系.md`。

## 编码规范 wiki 链接

| 规范 | wiki 路径 |
|------|----------|
| **Native 编码规范 v1**（命名空间/Include/extern C/命名/注释/格式/内存/宏） | `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/native-coding-conventions-v1.md` |
| Native 调试/LOG/PROFILE_SCOPE | `wiki/03-功能模块/06-il2cpp核心架构/24-DAP调试器使用说明.md` + 上述 v1 §5 日志节 |
| `unordered_map` 选型 | 见 v1 规范 §5 内存/宏 + `wiki/04-工具与集成/chaos-sdk-usage.md` |
| SDK/TPG 输出边界（四层架构红线） | `wiki/01-项目总览/codegen-tpg-python-module-boundaries.md` |