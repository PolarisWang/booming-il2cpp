# T2-8a: EEClass AOT 验证测试

> **parent_task_id**: 20260523-typesystem-vtable-industrialization
> **source_task_id**: roadmap-v1-01.md
> **source_relation**: roadmap-child
> **lifecycle_status**: completed
> **phase**: completed

## Scope

验证 EEClass AOT 路径 Ensure*Filled 行为正确：
- AOT 类型 EnsureEEClass 直接引用 constexpr ReflectionQuery*Descriptor 数组（零拷贝）
- EnsureMethodsFilled / EnsureFieldsFilled / EnsurePropertiesFilled / EnsureEventsFilled 对 AOT 类型是无操作（filled 已为 true）
- 验证通过后，AOT 冷路径反射数据可安全使用

## Inputs

- `src/native/runtime-core/eeclass.cpp` — EnsureEEClass + Ensure*Filled 实现
- `src/native/runtime-core/eeclass.h` — EEClass + LazyArray 定义
- `src/native/runtime-core/module_registry.h/cpp` — 模块注册 API
- `src/native/runtime-core/reflection_query_model.h` — 描述符类型定义
- `src/native/common/chaos/type_info.h` — MethodTable 结构定义

## Expected Outputs

- `testing/src/native/runtime-core/eeclass_test.cpp` — 单元测试文件
- 测试目标注册到 CMakeLists.txt

## Exit Criteria ✅

- [x] eeclass_test 编译通过
- [x] 全部 9 个测试通过：
  1. AOTPath — EnsureEEClass 成功链接 constexpr descriptor 数据
  2. Idempotent — 重复调用幂等
  3. NullMethodTable — null 返回 false
  4. GetEEClassReturnsNullForNoColdDelta — cold_delta==0 返回 nullptr
  5. GetEEClassNullMT — null MT 返回 nullptr
  6. DynamicTypePath — 未注册类型走动态路径（空 EEClass）
  7. EnsureMethodsFilledNoopForAOT — AOT 类型 Ensure*Filled 不改变数据
  8. EnsureFilledNoopForDynamic — 动态类型 Ensure*Filled 只设 filled=true
  9. EnsureFilledNullEEClass — null 不崩溃

## Terminal Notes

实现要点：
- AOT 路径 `FindDescriptorByMethodTable` 通过 module_registry type_info_ptrs 反向查找 constexpr descriptor
- EEClass 通过 `cold_delta`（uint32_t 字节偏移）关联 MethodTable
- **关键发现**: MethodTable 和 EEClass 必须在同一 heap（domain heap）分配，否则 x64 上指针差可能 >4GB 导致 uint32_t 截断崩溃
- 测试中 `DynamicTypePath` 和 `EnsureFilledNoopForDynamic` 使用 domain-heap 分配的 MethodTable（非栈/静态）
- AOT constexpr descriptor 数据直接引用（零拷贝），不分配 domain 内存
- Events LazyArray 未被 EnsureEEClass 填充（descriptor 无 events 字段）

修改文件：
- `testing/src/native/runtime-core/eeclass_test.cpp` — 9 个测试用例
- `testing/src/native/runtime-core/CMakeLists.txt` — 注册 eeclass_test 目标
