# Bootstrap 模块架构

## 两阶段初始化

### 为什么需要两阶段？

1. **分离注册与就绪** — RegisterCodegen 存储 codegen 产出的指针表（无运行时依赖），BootstrapRuntime 执行需要运行时基础设施的初始化（SEH、GC slot maps、vtable）
2. **hotupdate 兼容** — hotupdate 补丁通过 ApplyPatchFromMemory 在 BootstrapRuntime 之后独立注册，两阶段保证了 AOT 主模块与补丁模块的初始化隔离
3. **测试友好** — 测试 fixture 可以构造合成数据调用 RegisterCodegen，验证指针表填充，再独立验证 BootstrapRuntime 的行为

### 状态机

```
未初始化 → RegisterCodegen → is_registered=true → BootstrapRuntime → is_bootstrapped=true
    ↑                                                                           │
    └──────────────────────── ApplyPatchFromMemory ──────────────────────────────┘
```

BootstrapRuntime 之前的 ResolveIcall 返回 nullptr（通过 is_bootstrapped 守卫）。

## CodegenBridgeV0

15 函数指针的 ABI vtable，以 C 符号 `chaos_codegen_get_bridge_v0()` 导出。

- 作为 AOT 主模块和运行时的唯一契约接口
- 所有 15 个函数指针在编译期确定（constexpr），无动态注册开销
- 使用时无需 `dlopen` 或符号查找 — 直接调用 bridge→function()

## 文件拆分策略

Phase D 将原单文件 bootstrap.cpp (1084 行) 按职责拆分为 4 个 TU：

- **bootstrap.cpp**: 初始化编排 + ABI 入口 + virtual method 解析，保留匿名 namespace 包裹内部符号
- **bootstrap_delegates.cpp**: 自包含的 delegate 子系统（GC 分配不可变链表的 Combine/Remove/Invoke）
- **bootstrap_icall.cpp**: 自包含的 icall 解析（~30 个 strstr 前缀匹配）
- **bootstrap_internal.h**: 跨 TU 共享声明（DelegateInstance、DecodeOpaqueToken、子系统入口点）

关键约束：匿名 namespace 中的符号不能跨 TU 引用。bootstrap.cpp 中的匿名 namespace 包裹了内部全局状态和帮助函数；`DecodeOpaqueToken` 被移出匿名 namespace 以支持跨 TU 使用。

## Delegate 实现策略

- 不可变链表节点，GC 分配（CRAG old-gen 可扫描堆）
- Combine = 克隆左链 + 共享右链（O(|left|)），最小化分配
- Remove = 克隆前缀跳过匹配节点 + 共享后缀
- Invoke = 遍历链，每个节点按 target_instance 选择 closed/static 调用约定

## Virtual Method 3-Tier 解析

ResolveVirtualMethod 遵循三阶回退链：

1. **vtable registry** — 运行时注册的 per-type vtables（最快路径）
2. **UnresolvedVirtualCall 表** — codegen 输出的预解析虚方法（编译期已知）
3. **返回 declared_method** — 直接调用 fallback

## Icall 解析策略

- 使用 `std::strstr` 做子串匹配（非精确比较）
- ~30 个已知 icall 字符串常量，匹配顺序有依赖关系（长签名在前，短签名在后）
- 字符串中逗号后**无空格**（`"System.Delegate,System.Delegate"`），与 C# 参数格式化不同

## Hotupdate 交互点

- FindMethodPointerByToken → method_replacement::Resolve(method_token) 提供 hotupdate 重定向
- BootstrapRuntime 通过 SetAotBridge 将 bridge 注入 HotpatchNameRegistry
- interpreter 的 ResolveSubjectId 通过 bridge→resolve_*_by_token 查询

## 测试架构

7 个测试目标使用共享的 BootstrapTestFixture（bootstrap_test_fixture.h）。

通过 RegisterMinimal()/BootstrapMinimal() 辅助方法，测试代码无需关心底层数据结构细节。BootstrapRuntime 只能成功执行一次（全局状态），因此所有测试被组织为独立的 test executable，每个在自己的进程中运行。
