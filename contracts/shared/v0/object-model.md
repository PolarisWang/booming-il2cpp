# Shared Object Model v0

## 目标

冻结当前 proof runtime 已经实现、且 AOT 与热更都必须共同理解的对象布局事实。

## v0 原则

- v0 只冻结当前代码里已经存在、可验证的 proof layout。
- 任何尚未在 `runtime_core.cpp` 落地的 GC header、sync block、压缩指针策略，都不在 v0 保证面内。

## ObjectHeader

当前 proof runtime 的普通对象头如下：

```cpp
struct ObjectHeader {
    TypeInfoHandle type;
    unsigned char field_storage[sizeof(void*) * 4u];
};
```

冻结点：

- 第一个 machine word 是 `TypeInfoHandle`，作为 type info slot。
- 普通对象内联保留 `sizeof(void*) * 4` 的 field storage proof 区。
- v0 没有独立 sync block 字段。
- v0 没有单独暴露 GC bits 字段。

## StringObjectHeader

当前 proof runtime 的字符串对象由 `string_new_utf8(...)` 构造：

```cpp
struct StringObjectHeader {
    TypeInfoHandle type;
    uintptr_t byte_count;
};
```

冻结点：

- header 后跟零结尾 UTF-8 字节序列。
- `byte_count` 表示 UTF-8 payload 长度，不含结尾 `\0`。
- v0 仍是 proof 级 UTF-8 存储，不宣称 CLR 完整 `UTF-16 string object` 语义。

## ArrayHeader

```cpp
struct ArrayHeader {
    TypeInfoHandle element_type;
    uintptr_t length;
};
```

冻结点：

- header 后紧跟 `length` 个元素槽位。
- 当前 proof runtime 的数组槽位按 `void*` reference storage 分配。

## BoxedValueHeader

```cpp
struct BoxedValueHeader {
    TypeInfoHandle type;
    uintptr_t byte_count;
};
```

冻结点：

- header 后紧跟 inline value bytes。
- `byte_count` 是被装箱值的原始字节大小。

## 兼容性说明

- 如果后续要把 GC bits、sync block 或更完整 string layout 放进对象头，必须明确升级版本，而不是默默修改 v0 定义。

## 事实来源

- `src/native/runtime-core/runtime_core.cpp`
