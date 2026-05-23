# T0-2: vtable_registry 隔离测试 — 实现计划

> **面向执行 Agent：** 使用 dev-executing-plans 执行本计划。

**目标：** 为 vtable_registry 子系统编写独立隔离单元测试

**架构：** 直接构造 VTableSlot[] + TypeVTable 静态实例，调用公开 API 验证。使用 shared domain fixture，破坏性操作放末尾。

**技术栈：** GoogleTest, vtable_registry.h

**架构审核模式：** normal

**设计文档：** n/a（继承父 roadmap 设计）

**问题清零来源：** parent-roadmap

**计划来源：** roadmap-child-auto

---

## 文件清单

| 文件 | 操作 | 职责 |
|------|------|------|
| `testing/src/native/runtime-core/vtable_registry_test.cpp` | 新建 | 所有 vtable_registry 隔离测试 |
| `testing/src/native/runtime-core/CMakeLists.txt` | 修改 | 添加 `vtable_registry_test` 测试目标 |

## 执行步骤

1. 创建 `vtable_registry_test.cpp`，包含以下测试组：

   **Fixture:**
   - 共享 domain（SetUpTestSuite/TearDownTestSuite，复用 type_registry_test 模式）
   - AOT stub 符号定义
   - 辅助函数 CreateTestVTable 构造 TypeVTable 实例

   **测试用例（预计 20+）：**

   ### 注册/查找
   - RegisterAndFindByToken — 注册后通过 TryGetTypeVTable(token) 找到
   - RegisterAndFindByStableId — 注册后通过 TryGetTypeVTableByStableId(id) 找到
   - RegisterNullVTable — RegisterTypeVTable(nullptr) 返回 false
   - RegisterZeroToken — type_token=0 返回 false
   - RegisterIdempotent — 重复注册同 token 返回 true

   ### 扁平 vtable 数组
   - RegisterVTableArray — 注册并查找
   - FindVTableLength — 验证长度
   - RegisterVTableArrayGuard — null/zero 参数不崩溃

   ### Codegen VTable 注册
   - RegisterCodegenVTable — 构造 VTableDescriptorV0，注册后查找

   ### HotUpdate VTable 注册
   - RegisterHotUpdateVTable — 基本注册
   - HotUpdateWithParent — 父类 vtable 存在时注册，验证继承
   - HotUpdateWithOverrides — 应用覆写后验证指针正确

   ### 运行时接口追加
   - RegisterRuntimeInterface — 追加接口并验证
   - RegisterRuntimeInterfaceIdempotent — 重复添加相同 iface_stable_id 不重复
   - RegisterRuntimeInterfaceNotFoundToken — 不存在的 token 返回 false

   ### 继承链解析
   - ResolveVirtualMethodDirect — 直接命中
   - ResolveVirtualMethodInheritanceDepth2 — 深度 2 继承链走行
   - ResolveVirtualMethodInheritanceDepth3 — 深度 3 继承链走行
   - ResolveVirtualMethodNotFound — 不存在的 method_token 返回 nullptr

   ### 接口偏移量查找
   - FindInterfaceOffset — AOT iface_map 命中
   - FindInterfaceOffsetRuntime — runtime_iface_map 命中
   - FindInterfaceOffsetNotFound — 不存在返回 UINT32_MAX

   ### 统一更新
   - UpdateVTableSlot — 更新后 ResolveVirtualMethodPointer 返回新指针
   - UpdateVTableSlotEpoch — epoch 递增

   ### 其他
   - BuildRuntimeVTable — 从基类拷贝 vtable 数组
   - FindMethodPointerByMethodToken — 查找第一个匹配指针
   - GetRegisteredVTableCount — 注册计数
   - UnregisterTypeVTable — 注销后查找返回 nullptr
   - UnregisterTypeVTableByStableId — 按 stable_id 注销

2. 修改 `testing/src/native/runtime-core/CMakeLists.txt` 添加测试目标

3. 构建并运行验证：`cmake --build --preset debug --target vtable_registry_test && ctest --preset debug -R vtable_registry_test --output-on-failure`

4. 归档子任务并更新父 roadmap
