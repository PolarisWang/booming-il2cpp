# 设计：FieldInfo.GetValue/SetValue 的真实读写

> 日期：2026-09-13 ｜ 状态：待实施
> 触发：reflection chunk fact 失败项（GetValue 返回 0 而非字段真实值）

---

## 1. 问题

`abi_reflection.cpp:97` 的 `FieldGetValue` 实现：

```cpp
const auto* ti = *static_cast<const TypeInfoHot* const*>(object_instance);
const auto header_size = HeaderSizeFromFlags(ti != nullptr ? ti->flags : 0);
CHAOS_IL2CPP_MEMCPY(out_value,
    static_cast<const unsigned char*>(object_instance) + header_size,
    out_value_size);
```

两个缺陷：
1. **忽略字段偏移**——它总是拷贝「对象头之后第一个字段」的内容，
   无论请求的是哪个字段
2. **静态字段直接失败**——`object_instance == nullptr` 时返回
   `INVALID_ARGUMENT`，而静态字段根本不需要实例

## 2. 架构约束（实测）

| 事实 | 影响 |
|---|---|
| `ReflectionQueryFieldDescriptor` **无 offset 字段** | 运行时无法从 descriptor 得到偏移 |
| codegen 用 `chaos_object->field_X` 直接访问字段 | 偏移由 C++ 编译器解析，**未物化为数字** |
| `GcTypeLayout.pointer_offsets[]` 只含 **GC 指针字段** | 不能直接复用于普通字段 |
| `kReflFields_*` 与类型 struct **在同一 TU**（`native-aot.generated.cpp`） | `offsetof` 可用 |
| descriptor 是 `constexpr` 静态表 | `offsetof` 是编译期常量 → 可放入 |

## 3. 方案

### 方案 A：descriptor 增加 `field_offset`（推荐）

在 `ReflectionQueryFieldDescriptor` **末尾**追加字段：

```cpp
CHAOS_IL2CPP_UINT32 field_offset;   // offsetof(TypeSymbol, member); static 字段为 0
```

- 遵循既有 ABI 约定（`flags` 的注释反复强调「must be last for ABI compat」，
  新字段改为最后，旧字段位置不变 —— 与 R1.1 引入 flags 时同一做法）
- codegen 在 Scriban 模板中发射 `offsetof({{ type_group.type_symbol }}, {{ field.member_name }})`
- 静态字段：`flags & kFieldFlagIsStatic` 时用 `chaos_static_*` 全局符号寻址

**运行时**：
```cpp
if (flags & kFieldFlagIsStatic) {
    // 静态字段：从注册的静态存储读取
    memcpy(out_value, static_storage_for(field), out_value_size);
} else {
    if (object_instance == nullptr) return INVALID_ARGUMENT;
    memcpy(out_value, (const char*)object_instance + field->field_offset, out_value_size);
}
```

**成本**：descriptor ABI 变更（末尾追加，向后兼容）+ Scriban 模板 + 运行时实现
**风险**：中（需确认全量重建后所有 chunk 的 descriptor 一致）

### 方案 B：扩展 GcTypeLayout 承载任意字段偏移

把 `GcTypeLayout` 从「仅 GC 指针」扩展为「全部字段」，反射经
`GcLayoutRegistry` 按 (stable_id, 字段名) 查询。

**成本**：改 GC 核心结构 + 注册协议；**风险**：高（GC 是 P0 路径）

### 方案 C：只修静态字段（缩小范围）

静态字段不需要偏移——只需按 `subject_id` 找到对应的 `chaos_static_*` 符号。
实例字段维持现状（返回 0）。

**成本**：低；**收益**：部分（只覆盖静态字段）

---

## 4. 建议

**方案 A**。理由：
1. 修正的是**根因**（偏移信息缺失），而非绕过
2. ABI 变更遵循既有约定（末尾追加），有 R1.1 先例
3. 不触碰 GC 核心（方案 B 的风险不可接受）
4. 一次改动同时覆盖实例字段与静态字段

## 5. 关键技术前提（已验证）

`offsetof` 对**继承来的字段**可用 —— 已在实际生成物中找到用例：

```cpp
// MulticastDelegate 继承 Delegate，但直接对基类字段取偏移
static constexpr CHAOS_IL2CPP_UINT16 kGcOffsets_..._MulticastDelegate[] = {
    offsetof(chaos_type_..._MulticastDelegate, chaos_delegate_target),
    offsetof(chaos_type_..._MulticastDelegate, chaos_delegate_invocation_list)};
```

来源：`artifacts/.../System.Collections.Immutable/.../native-aot.generated.cpp:1123`。
与 `GcTypeLayout.cs:77` 注释相互印证。方案 A 的模板发射可完全沿用该模式。
- 实例字段写入（SetValue）需要写屏障（P1 内存安全）；本设计只覆盖读路径，
  写路径单独立项

## 6. 非目标

- 属性 setter 语义修正（`PropertyInfo.GetSetMethod` 对只读属性应返 null）——独立小改
- 值类型字段的装箱语义——超出本次范围
