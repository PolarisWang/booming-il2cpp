# 异常Shape翻译表

## 5 种 EH Shape

| Shape | 结构 | 翻译模式 |
|-------|------|---------|
| CatchOnly | `try { } catch(T) { }` | 直接 `try { } catch(T&) { }` |
| FilterOnly | `try { } filter { } catch { }` | `try { } catch(...) { if(filter) ... else throw; }` |
| FinallyOnly | `try { } finally { }` | `try { } catch(...) { finally_code; throw; }` + normal finally 路径 |
| CatchAndFinally | `try { } catch(T) { } finally { }` | catch 块 + finally 展开 |
| FilterAndFinally | `try { } filter { } catch { } finally { }` | filter + finally 组合 |

## 约束

| 约束 | 说明 |
|------|------|
| Filter 不分配 | Filter 子句中不允许托管内存分配 |
| 异常对象桥接 | 通过 `raise_managed_exception` 抛托管异常 |
| C++/托管边界 | C++ 异常在托管边界处转换为托管异常 |
| 多层嵌套 | 嵌套 try/catch 展开为嵌套 try 块 |

## 实现位置

- EH shape 识别：CodeGen 层（`NativeAotLoweringPlanner.Exceptions.cs`）
- C++ 生成：Emitter 层（`NativeAotLoweringPlanner.Emit_Exceptions.cs`）
- Runtime support：`RuntimeSupport.Exception.cs`
