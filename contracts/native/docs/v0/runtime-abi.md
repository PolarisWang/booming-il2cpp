# Runtime ABI v0

## 定位

- `boom_runtime_get_abi_v0()` 返回进程级函数表，供 bridge、host embedding、手写 smoke 和少量平台 glue 统一使用。
- `Runtime ABI` 是低层基础操作面，只承诺稳定的句柄、生命周期与基础内存/反射入口，不承诺高层生成代码语义。
- `v0` 的目标是冻结后续实现必须对齐的边界，而不是提前实现完整 runtime core。

## 调用约定

- 所有导出入口都使用 `BOOM_RUNTIME_ABI_EXPORT` 与 `BOOM_RUNTIME_ABI_CALL`。
- 宿主应先调用 `boom_runtime_get_abi_v0()`，再缓存返回的 `BoomRuntimeAbiV0*`。
- `BoomRuntimeState*` 表示进程级 runtime 状态；`BoomThreadState*` 表示当前线程的附着状态。
- 所有 `*Handle` 都是不透明句柄，只能通过 ABI/bridge 传递，不能解引用、序列化或假设布局。

## 句柄集合

| 句柄 | 含义 | 典型来源 |
| --- | --- | --- |
| `BoomTypeInfoHandle` | 托管类型元数据句柄 | `image_find_type` |
| `BoomMethodInfoHandle` | 方法句柄 | `type_find_method` |
| `BoomFieldInfoHandle` | 字段句柄 | `type_find_field` |
| `BoomPropertyInfoHandle` | 属性句柄 | `type_find_property` |
| `BoomEventInfoHandle` | 事件句柄 | `type_find_event` |
| `BoomParameterInfoHandle` | 参数句柄 | `method_get_parameter` |
| `BoomGenericContextHandle` | 泛型上下文句柄 | `method_get_generic_context` |
| `BoomAssemblyHandle` | 程序集句柄 | 上层注册或宿主枚举 |
| `BoomImageHandle` | 程序集镜像句柄 | `assembly_get_image` |
| `BoomExceptionHandle` | 托管异常对象句柄 | `method_invoke` 或宿主保留 |

## 错误模型

- `Runtime ABI` 的错误边界比 bridge 更底层、更显式。调用方必须检查状态码、空指针或输出异常句柄，不能假设失败会被自动提升成高层异常。
- 查询类入口遵循“状态码或空指针”模型：
  - `runtime_init`、`thread_attach`、`class_init`、`field_get_value`、`field_set_value`、`method_invoke` 返回 `BoomRuntimeStatus`
  - `assembly_get_image`、`image_find_type`、`type_find_method`、`type_find_field`、`type_find_property`、`type_find_event`、`method_get_parameter`、`method_get_generic_context` 在无法解析时返回空句柄
- 分配类入口 `object_new`、`array_new`、`string_new_utf8` 失败时返回空指针；宿主必须把空返回值视为显式失败，而不是继续向下执行。
- 托管异常不会通过隐式 side effect 抛出。需要由 `raise_managed_exception` 显式触发，或者由 `method_invoke` 通过 `out_exception` 返回。
- 高阶句柄在 `v0` 中按完整语义承诺处理，不是占位符；后续实现必须保证句柄可稳定驱动字段访问、方法调用和反射查询。

## 函数面划分

### 生命周期

| 入口 | 参数语义 | 返回/约束 |
| --- | --- | --- |
| `runtime_init` | `init_params` 描述宿主名、runtime tag 与初始化 flag；`config` 可选提供分配器；`out_runtime_state` 返回进程级状态 | 成功返回 `BOOM_RUNTIME_STATUS_OK` 并写出 `runtime_state` |
| `runtime_shutdown` | 销毁 `runtime_init` 返回的进程级状态 | 无返回值；调用方负责保证线程已分离 |
| `thread_attach` | 将当前原生线程附着到指定 runtime | 成功后写出 `BoomThreadState*` |
| `thread_detach` | 解除当前线程与 runtime 的绑定 | 无返回值；仅用于已成功附着的线程 |

### 分配与类初始化

| 入口 | 参数语义 | 返回/约束 |
| --- | --- | --- |
| `object_new` | 按 `type` 分配单对象实例 | 成功返回对象指针，失败返回空 |
| `array_new` | 按元素类型和长度分配数组 | 成功返回数组对象指针，失败返回空 |
| `string_new_utf8` | 使用 UTF-8 字节创建托管字符串 | 成功返回字符串对象指针，失败返回空 |
| `class_init` | 触发类型静态初始化或确保类已准备好 | 返回显式状态码 |

### GC handle 与异常

| 入口 | 参数语义 | 返回/约束 |
| --- | --- | --- |
| `gc_handle_new` | 为对象创建稳定 handle；`pinned=true` 表示请求 pin | 成功返回非零 `BoomGCHandle` |
| `gc_handle_free` | 释放先前分配的 handle | 允许对宿主缓存的 handle 做显式清理 |
| `raise_managed_exception` | 把给定异常句柄提升到当前线程的托管异常边界 | 只负责显式触发，不返回状态 |

### 句柄驱动访问

| 入口 | 参数语义 | 返回/约束 |
| --- | --- | --- |
| `field_get_value` | 通过字段句柄把对象或静态字段拷贝到 `out_value` | `out_value_size` 必须与宿主缓冲区匹配 |
| `field_set_value` | 通过字段句柄把原生值写回对象或静态字段 | `value_size` 必须匹配字段布局约定 |
| `method_invoke` | 使用句柄、实例指针和 `argv/argc` 调用方法 | 语义失败通过 `out_exception` 返回异常句柄 |

### 反射查询

| 入口 | 参数语义 | 返回/约束 |
| --- | --- | --- |
| `assembly_get_image` | 从程序集句柄取镜像句柄 | 查无结果返回空 |
| `image_find_type` | 通过命名空间和类型名查找类型 | 仅负责解析，不做类初始化 |
| `type_find_method` | 按名称和参数个数查找方法 | 查无结果返回空 |
| `type_find_field` | 按名称查找字段 | 查无结果返回空 |
| `type_find_property` | 按名称查找属性 | 查无结果返回空 |
| `type_find_event` | 按名称查找事件 | 查无结果返回空 |
| `method_get_parameter` | 按序号读取参数句柄 | 越界返回空 |
| `method_get_generic_context` | 读取方法的泛型上下文句柄 | 非泛型场景可返回空 |

## 与 Bridge 的边界

- `Runtime ABI` 只暴露低层基础操作，不承担 `generated C++` 的高层辅助语义。
- `virtual invoke`、`delegate`、`icall resolve` 等生成代码最常用的高层帮助函数不属于 ABI，而属于 bridge。
- ABI 的职责是让宿主和 bridge 拿到稳定基础元件；bridge 再把这些元件拼成对生成代码更友好的调用面。
