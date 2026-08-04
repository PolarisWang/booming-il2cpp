# Stage A：Value Type 栈上编译支持

## 1. 现状

当前 AOT 解释栈（eval stack）所有值统一为 `CHAOS_IL2CPP_INTPTR`（8 字节指针）：

```cpp
// 当前：push/pop 都是 8 字节
CHAOS_IL2CPP_INTPTR _s0;   // 栈槽
_s0 = ...;                   // 推入值
auto val = _s0;             // 弹出值
```

对于 >8 字节 value type（`Vector128<Int32>`=16字节、`ReadOnlyMemory<byte>`=16字节、`Decimal`=16字节），当前通过堆分配绕行：

```cpp
// 当前对大 value type 的处理：
auto* heap_ptr = malloc(16);  // 堆分配 16 字节
*heap_ptr = value;            // 拷贝值到堆
_s0 = (CHAOS_IL2CPP_INTPTR)heap_ptr;  // 存指针到栈
```

这种方式带来：
1. **堆分配开销** — 每次 push 大 value type 都要 malloc/free
2. **间接访问** — 读写需要 dereference 指针
3. **值语义丢失** — `ldobj` / `stobj` 无法直接操作

## 2. 改造目标

支持 eval stack 上的定长 value type 直接存储，消除堆分配和间接访问：

```cpp
// 改造后：大 value type 直接存到栈
uint8_t _s0[16];            // 16 字节栈槽
std::memcpy(_s0, &value, 16); // 直接拷贝值
// 或使用 union 支持不同类型
union {
    CHAOS_IL2CPP_INTPTR as_ptr;
    Vector128Payload as_v128;  // 16 字节对齐
    DecimalPayload as_decimal; // 16 字节对齐
} _s0;
```

## 3. 影响范围

| 模块 | 改动 | 影响 |
|------|------|------|
| StructuredIR | 栈槽定义、push/pop/dup 操作 | 核心 IR 结构变更 |
| LinearEmitter | 栈操作代码生成 | 发射逻辑变更 |
| ExceptionEmission | eval stack 操作 | 兼容多大小 |
| AbiInvocation | 参数传递、返回值处理 | 多 slot 传参 |
| GcSlotMap | GC 跟踪槽 | 大 value type 无 GC 指针 |

## 4. 实现步骤

### Step 1：IR 层支持（3-5 天）

```csharp
// 当前：栈槽 = CHAOS_IL2CPP_INTPTR
// 目标：栈槽 = ISlotValue 接口
interface ISlotValue {
    int Size { get; }          // 字节数
    string CppType { get; }    // C++ 类型名
    string ToCppExpr(string slotName);  // 读取表达式
}

sealed class IntPtrSlot : ISlotValue {
    int Size => 8;
    string CppType => "CHAOS_IL2CPP_INTPTR";
}

sealed class MultiSlotValue : ISlotValue {
    int Size { get; }
    string CppType { get; }
}
```

### Step 2：Emit 层支持（3-5 天）

```csharp
// 当前每个 push 都 push_intptr(value)
// 改造后按 size 区分：
void PushValue(ISlotValue value) {
    if (value.Size <= 8) PushIntPtr(value);
    else PushMultiSlot(value);  // 多槽或 union
}
```

### Step 3：Vector128/256 适配（2 天）

使用 stage 2 的能力，为 Vector128/256 生成正确的栈操作。

### Step 4：ReadOnlyMemory<T> 适配（3-5 天）

使 `ReadOnlyMemory<byte>` 能在栈上传递，从而让 `JsonDocument::Parse` 可编译。

## 5. 时间线

| Step | 内容 | 工作量 |
|------|------|--------|
| 1 | IR 层支持 | 3-5 天 |
| 2 | Emit 层支持 | 3-5 天 |
| 3 | Vector128/256 适配 | 2 天 |
| 4 | ReadOnlyMemory<T> 适配 | 3-5 天 |
| 5 | 测试 + 回归验证 | 3 天 |
| **总计** | | **~3 周** |

## 6. 风险和缓解

| 风险 | 概率 | 缓解 |
|------|------|------|
| 栈槽 size 变动导致 GC 槽偏移错误 | 中 | 增加 GcSlotMap 验证断言 |
| push/pop 操作数量不匹配 | 低 | 结构化 IR 的深度追踪可自动验证 |
| Abi 传递多 slot 值需要 caller/callee 同步修改 | 中 | 增加 ABI 版本号校验 |
| 性能回退 | 低 | 每个 step 后跑 benchmark 验证 |

## 7. 依赖

- 需要理解 `StructuredSlotContext`、`LinearSlotContext`、`EvalStack` 的完整交互
- 需要熟悉 `EmitLinearCallTarget`、`EmitExternalRuntimeTableDispatch` 的调用约定
- 需要 `ReadOnlyMemory<T>` 的内存布局信息
