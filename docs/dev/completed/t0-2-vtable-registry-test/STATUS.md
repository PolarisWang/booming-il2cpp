# T0-2: vtable_registry 隔离测试 — 完成

> **parent_task_id**: 20260523-typesystem-vtable-industrialization
> **source_task_id**: t0-2-vtable-registry-test
> **source_relation**: roadmap-child
> **创建日期**: 2026-05-23
> **更新日期**: 2026-05-23
> **task_type**: plan
> **phase**: completed
> **lifecycle_status**: completed

## 完成证据

- **测试文件**: testing/src/native/runtime-core/vtable_registry_test.cpp
- **构建配置**: testing/src/native/runtime-core/CMakeLists.txt（新增 vtable_registry_test 目标）
- **测试结果**: 47/47 ✅（ctest Debug 配置，0.03 秒）
- **覆盖率**: type_registry_test + vtable_registry_test 共 59 个测试全部通过

### 测试覆盖清单

| 功能 | 测试数 | 覆盖场景 |
|------|--------|---------|
| RegisterTypeVTable | 7 | 基本注册、token-only、null/zero/null slots、幂等性 |
| TryGetTypeVTable / ByStableId | 3 | 存在/不存在/零参数 |
| RegisterVTableArray | 3 | 注册查找、null/zero 守卫、不存在 |
| RegisterCodegenVTable | 3 | 基本注册、null、零 token |
| RegisterHotUpdateVTable | 4 | 基本、带父类+覆写、零 token、幂等性 |
| RegisterTypeVTableRuntimeInterface | 4 | 追加、幂等性、找不到 token、零参数 |
| chaos_find_interface_offset | 3 | AOT iface_map、未找到、零参数 |
| ResolveVirtualMethodPointer | 6 | 直接命中、深度2、深度3、独立解析、未找到、零 token |
| BuildRuntimeVTable | 4 | 基类拷贝、幂等性、零 ID、找不到基类 |
| UpdateVTableSlotByMethodToken | 3 | 更新验证、零参数、epoch 传播（TCVC 失效） |
| FindMethodPointerByMethodToken | 3 | 存在、零参数、未找到 |
| GetRegisteredVTableCount | 1 | 注册计数增减 |
| RuntimeInterfaceEpochPropagation | 1 | IOC epoch 验证 |
| UnregisterTypeVTable | 4 | 按 token 注销、按 stable_id 注销、零 token、不存在 |

### 注意事项

- 使用 `RegisterHotUpdateVTable`（内部 CHAOS_IL2CPP_MALLOC 全量堆分配）测试 UnregisterTypeVTable
- 静态 TypeVTable 注册后不调用 UnregisterTypeVTable（会尝试 free 静态内存）
- 测试文件使用了 MSVC 兼容的静态数组（避免 vector initializer_list 问题）

## 完成证据卡

- arch_review: ok
- verification:
  - canonicalCommand: cmake --build build/testing --target vtable_registry_test --config Debug && ctest -C Debug -R vtable_registry_test
  - formalObject: testing/src/native/runtime-core/vtable_registry_test.cpp
  - consolePath: n/a
- test_result: passed (47/47)
- wiki: n/a
- next: update parent roadmap → T0-3
