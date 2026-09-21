# 步 3 设计：codegen 生成「异常类型 → TypeInfo」表

> 日期：2026-09-21

## 目标

让 `RaiseManagedException("System.ObjectDisposedException")` 能拿到正确的
`TypeInfo*`，从而建造**带正确运行时类型**的异常对象（而非 null）。

## 关键约束（已逐条验证）

| # | 约束 | 证据 |
|:-:|:-----|:-----|
| 1 | `RaiseManagedException` 的 **149 个调用点在手写 C++**（非 codegen 产物） | `runtime_stubs/*.cpp` |
| 2 | 这些 stub **编译期不知道 chunk 闭包** | 属 `chaos_runtime_core` 库 |
| 3 | `chaos_mt_*` 是**TU 私有符号**（无 extern） | 生成体实测 |
| 4 | `GcLayoutRegistry` 只有 layout，**不存 TypeInfo 指针** | `gc_layout.h:178` |
| 5 | 但 **codegen ↔ runtime 已有跨 TU 表约定** | `kChaosExternalRuntimeSubjects`（`chaos_runtime_host.h:52`） |

## 设计

### 复用语约束 5 的既有模式

**codegen 侧**（生成体，与 `chaos_mt_*` 同 TU）：
```cpp
struct ChaosExceptionTypeEntry { const char* name; const TypeInfo* type_info; };
extern "C" const ChaosExceptionTypeEntry kChaosExceptionTypes[] = {
    { "System.ObjectDisposedException",   chaos_mt_..._ObjectDisposedException.AsTypeInfoHot() },
    { "System.ArgumentNullException",     chaos_mt_..._ArgumentNullException.AsTypeInfoHot() },
    { "System.InvalidOperationException", chaos_mt_..._InvalidOperationException.AsTypeInfoHot() },
    ...
};
extern "C" const int32_t kChaosExceptionTypeCount = N;
```

**runtime 侧**（`exception_helpers.cpp`）：
```cpp
extern "C" const ChaosExceptionTypeEntry kChaosExceptionTypes[];
extern "C" const int32_t kChaosExceptionTypeCount;

TypeInfoHandle ResolveTypeByName(const char* name) {
    // 1. 先查这张表（编译期符号，最可靠）
    for (int i = 0; i < kChaosExceptionTypeCount; i++)
        if (strcmp(kChaosExceptionTypes[i].name, name) == 0)
            return /* 由 TypeInfo* 得到 handle */;
    // 2. 回退到现有的模块扫描（保持兼容）
    ...
}
```

### 表的内容从哪来

**codegen 已知**：IR 里的 **catch 类型**（`region.CatchTypeSubjectId`）——
实测 114+48+32+25+5+3+1 = 覆盖所需异常。

**加上层 1 已收集的基类链**（`SystemException`/`Exception`）。

### 为什么这样能「还原 C# 真实表现」

| C# 行为 | 本方案 |
|:--------|:-------|
| 异常类型**编译期**确定 | ✅ 表由 codegen 编译期生成 |
| 对象带正确运行时类型 | ✅ `AsTypeInfoHot()` |
| 继承链可匹配 | ✅ **层 1 已修** |

## 与「镜像」方案的关系

**步 3 替代了步 2**：
- 步 2（镜像）是 `ResolveTypeByName` 的**一种实现手段**
- 步 3 用**编译期符号表**这个更可靠的手段
- **不需要** metadata token（约束 2 的跨域障碍消失）

## 工作量

| 项 | 改动 |
|:---|:-----|
| codegen | 新增一张表的发射（~50 行） |
| runtime | `ResolveTypeByName` 加表查找（~20 行） |
| 头文件 | `ChaosExceptionTypeEntry` 定义 + extern 声明 |
| 验证 | 读产物确认表内容 + 跑 chunk 看 62 项是否下降 |

**中等规模，但路径清晰。**

## 待确认

- 表的发射位置：应与其他 `kChaos*` 表同处（`Methods.ModuleData` 或 `Methods.cs:1358` 附近）
- `AsTypeInfoHot()` 对异常类型是否可用（需 MethodTable 有定义 —— 层 1 已补）
