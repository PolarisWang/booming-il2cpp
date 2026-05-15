# 异常Shape翻译表

## 5 种 EH Shape

| Shape | 结构 | CPP_THROW 翻译 | SETJMP 翻译 |
|-------|------|---------------|-------------|
| CatchOnly | `try { } catch(T) { }` | `try { } catch(T&) { }` | `setjmp == 0 ? try体 : { type_check → handler / re-raise }` |
| FilterOnly | `try { } filter { } catch { }` | `try { } catch(...) { if(filter) ... else throw; }` | `setjmp != 0 → { filter_expr; if (!match) pop+re-raise; handler }` |
| FinallyOnly | `try { } finally { }` | `{ auto guard = make_finally_guard(...); try体 }` | `setjmp == 0 ? try : { finally体; re-raise }` |
| CatchAndFinally | `try { } catch(T) { } finally { }` | catch 块 + RAII finally 守卫 | setjmp catch + finally + re-raise |
| FilterAndFinally | `try { } filter { } catch { } finally { }` | filter + finally 组合 | setjmp filter + finally + re-raise |

## 约束

| 约束 | 说明 |
|------|------|
| Filter 不分配 | Filter 子句中不允许托管内存分配 |
| 统一异常投递 | 通过 `chaos_raise_exception` 统一 API（编译时展开为 C++ throw 或 longjmp） |
| 平台自动选择 | `config.h` 根据目标平台编译时选择 EH 模式（iOS/Android → SETJMP，桌面 → CPP_THROW） |
| 多层嵌套 | 嵌套 try/catch 展开为嵌套 try/setjmp 块 |

## 双路径架构

从 v1.2 起，codegen 为每个异常处理方法生成双路径代码：

```cpp
#if defined(CHAOS_IL2CPP_EH_CPP_THROW)
    try { ... }
    catch (const chaos_managed_exception& e) { ... }
#else
    {
        auto* _chaos_jmp = push_exception_jmp_buf();
        if (setjmp(*_chaos_jmp) == 0) { try体 }
        else { /* 从 g_chaos_exception_obj 读取异常 */ }
        pop_exception_jmp_buf();
    }
#endif
```

## SETJMP 模式详细模式

| 模式 | codegen 模板 | 说明 |
|------|-------------|------|
| TryCatch | `push_jmp → setjmp==0 ? try : { type_check → handler / pop+re-raise } → pop_jmp` | 类型不匹配时先 pop 再 re-raise 避免 jmp_buf 栈泄漏 |
| TryFinally | `push_jmp → bool caught; setjmp==0 ? try : { caught=true; finally } → pop_jmp → if(caught) re-raise` | finally 体执行后 re-raise 保持异常传播 |
| TryFilter | `push_jmp → setjmp==0 ? try : { push(exception) → filter → endfilter→0 ? pop+re-raise : handler } → pop_jmp` | filter 语义通过 endfilter 返回值区分 |

## 实现位置

- EH shape 识别：CodeGen 层（`NativeAotLoweringPlanner.Exceptions.cs`）
- C++ 生成（双路径）：`NativeAotLoweringPlanner.StructuredIR.cs`（StructuredIR 路径）
- C++ 生成（双路径）：`NativeAotLoweringPlanner.ExceptionEmission.cs`（Flat goto 路径）
- Runtime support：`exception_jmp.h`（SETJMP TLS 栈 + uniform API）
- Runtime support：`finally.h`（CPP_THROW RAII finally 守卫）
- Runtime support：`exception_api.cpp`（元数据 ABI 函数）