# EH 模式错配修复 — 终止记录（2026-09-22）

> **状态**: 未修复，按 CLAUDE.md「三次修复规则」终止盲试
> **worktree**: `reflection-eh-mode-fix`（已清理，无残留改动）
> **需合并到**: `docs/dev/in-progress/reflection-production-readiness/STATUS.md` 追加

---

## 一、已确证的事实（可信，有证据）

1. **现象**：reflection chunk `preAssertionRaise=31`，这些 subject 全部
   `caught=true` / `assertFailed=false` —— 异常逃逸，非断言失败。

2. **逃逸机制**：`entry.exe` 直跑 31 处 `[SEH-FAULT] code=0xe0000001`
   （`kChaosManagedExceptionCode`）。原生 `chaos_raise_exception()` 经
   **SEH `RaiseException`** 抛出。

3. **宏展开实测**（用探针 TU 直接编译验证）：
   `CHAOS_IL2CPP_TARGET_PLATFORM_WINDOWS` 与 `CHAOS_IL2CPP_EH_WIN32_SEH`
   **均已正确定义** —— `config.h:44-50` 有 `_WIN32` 回退检测。
   **早前「平台宏未传递 → 走了 CPP_THROW 分支」的判断是错的，已推翻。**

4. **真正的不一致**：`NativeAotLoweringPlanner.MethodEmission.cs:499`（原文）
   在 subject 外层硬编码发射 C++ `try` / `catch (const chaos_managed_exception&)`
   / `throw;`，**绕过 `CHAOS_EH_TRY` / `CHAOS_EH_CATCH_BEGIN` /
   `CHAOS_EH_RETHROW` 宏**；而 IL 异常区域（`StructuredIR.Emit.cs`）用的是宏。
   SEH 异常无法被 C++ catch 接住，必然逃逸。

## 二、未能解释的阻塞矛盾（核心）

按上述结论修改 `MethodEmission.cs` 改用宏后，**产物仍为 79 个硬编码
catch、0 个宏**。为此做了穷尽验证：

| 验证手段 | 结果 |
|---|---|
| 源码改动 | ✅ 已改 |
| 所有 Generator.dll 副本含新代码（UTF-16LE 验证） | ✅ 确认 |
| pipeline 路径解析指向本 worktree（`_worktree_root()` 实测） | ✅ 确认 |
| codegen 每次执行 | ✅ 1120 methods lowered |
| 产物 marker（`/* MARKER_WRAP_SITE_ACTIVE */`） | ❌ 0 次 |
| **三层文件探针**（`File.AppendAllText` 绝对路径，绕过 stdout/stderr 捕获） | ❌ **文件从未生成** |

探针覆盖 `BuildMethodSourceSafe`（入口）→ `BuildMethodSource` →
`EmitManagedMethod` 全链（`Methods.ModuleData.Helpers.cs` +
`MethodEmission.cs`），**三层全部零触发**，而产物持续正常生成。

**推论**：codegen 执行了，但**不经过这条发射链**。存在尚未定位的第二条
发射路径，或进程实际加载的程序集与磁盘不一致。

## 三、本轮犯的错误（供后续避免）

1. **建 worktree 前未查并发会话状态** —— 全程与另一会话争用同一批
   artifacts，验证数据多次不可信。
2. **用 ASCII 搜 C# 字符串（必然为 0）** —— 项目记忆明确记录过
   `UTF-16LE 字节验证` 坑，仍然踩了，并据此下过「改动没编译进去」的错误结论。
3. **未尽早使用运行时探针**（文件写入），长期停留在二进制/产物层推断。
4. **把「产物里有 C++ catch」推断为「走 CPP_THROW 分支」** ——
   实测证明宏是对的，该推断错误，导致 A 方案（补平台宏）整轮作废。
5. **pipeline 失败才打印 stderr**（`build.py` 的 `[TPG:err]` 分支），
   成功时 stderr 被丢弃 —— 前两轮诊断输出其实一直在，只是没被捕获。

## 四、下一步建议

- 重新立项时**先定位第二条发射路径**。建议探针点：
  `NativeAotEmitter.GenerateFromArtifacts`（`NativeAotEmitter.cs:55`）或
  TPG 的 `ConvertToCppHandler.Run`（`ConvertToCppHandler.cs:136`）入口，
  确认实际调用的发射器类型与程序集来源。
- 或由了解该 codegen 结构的人直接指出主体发射路径。
- **不要在未解决该矛盾前继续改 `MethodEmission.cs`** —— 已验证无效。

## 五、worktree 清理确认

- 临时探针（`[SAFE]` / `[EMIT]` / `[WRAP?]` / `MARKER_WRAP_SITE_ACTIVE`）已全部移除
- `MethodEmission.cs` / `Methods.ModuleData.Helpers.cs` 已 `git checkout` 还原
- 临时文件（`chk.py` / `clean_diag.py` / `probe_eh.cpp` / `tmp_probe.bat` / `eh_diag.txt`）已删除
- `git status` 干净
