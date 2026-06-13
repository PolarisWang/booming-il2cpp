# Interpreter Hotpatch 性能优化 — 方案对比与权衡 (brainstorm v1)

## 总览

基于 postPatchNsPerOp 性能数据，hotpatch 路径慢的核心原因是 interpreter 逐条解释 IL。本次优化覆盖 fast_dispatch.cpp (FastExecute) 中所有 handler + dispatch loop，目标是缩小 HU-AOT 与 NET8 的差距。

### 三个优先级约束
1. **性能最优（P1）** — 所有方案以降低 postPatchNsPerOp 为首要目标
2. **方案完美性（P2）** — 架构保持清晰，不引入技术债
3. **热更适配（P3）** — 优化对 hotpatch 后的 interpreter 路径友好

---

## P0: Quick Wins (预期 2-3 天)

### P0-A: Box type_token 设置

#### 现状
```cpp
// Handle_Box (fast_dispatch.cpp:824)
auto* boxed = AcquireBoxedObject();
boxed->fields.resize(1);
boxed->fields[0] = frame.PopIV();
frame.PushObj(boxed);
// type_token 未设置! 默认为 0
```

`InterpreterObject::type_token` 在 `Handle_Box` 中从未设置，导致后续 `callvirt` (如 `ToString()`, `GetHashCode()`) 的 MIC 检查时 receiver_token 为 0，永远无法命中 MIC → 每次都走 `ResolveVirtualMethodPointer` (~2000ns)。

#### 方案对比

| 方案 | 描述 | 性能 | 架构 | 维护 | 热更 |
|------|------|------|------|------|------|
| **A1: IR instr 传 token** | IR lowering 时 box 指令记录 type_token → immediate_i4 | ★★★ | ★★★ | ★★★ | ★★★ |
| A2: Handle_Box 查 metadata | 在 Handle_Box 内部通过 stack tag 反向推导 type | ★ | ★★ | ★★ | ★★ |
| A3: 在 AcquireBoxedObject 返回值设 | 在 `AcquireBoxedObject` 中设置默认值 | — | — | — | — 不可行，无 token 信息 |

**推荐: A1**

Handle_Box 的 `IRInstruction` 有一个 `immediate_i4` 字段，IR lowering 时已知 box 的目标类型 token。在 `fast_dispatch.cpp:831` 加一行即可：

```cpp
boxed->type_token = static_cast<uint32_t>(instr.immediate_i4);
```

**改动量**: 1 行 C++
**影响范围**: 所有 box 操作
**预期收益**: 
- Convert::ToChar 等装箱频繁的方法: **2-5x** 提升 (MIC 从永远 miss 变为命中)
- Enum::ToString/Format: 1.5-2x 提升
- 全量评估: postPatchNsPerOp 越高的方法收益越大

#### 风险
- 极低。immediate_i4 已经是合法的值，IR lowering 只是在 box 指令中填充它
- 如果 IR lowering 在某些路径上没填 type_token → type_token 仍为 0 → 和现状一样，不会更差

---

### P0-B: 补齐缺失 opcode dispatch (104-110)

#### 现状
```cpp
// ir_opcodes.h 定义了 120 个 opcode (0-110)
// 但 switch 只处理了 0-103 (到 StFldBarrier=103)
// LdElemNoChk=104, StElemNoChk=105, LdElemANoChk=106 全部走 Handle_Unsupported
```

这些 NoChk 变体是 IR lowering 时有意生成的，用于已确认安全的数组访问（bounds check 已由前面的代码完成）。但因为 switch 没处理，任何用了这些 opcode 的方法都触发 fallback → InterpreterVM (慢 10-100x)。

#### 方案对比

| 方案 | 描述 | 性能 | 架构 | 维护 | 热更 |
|------|------|------|------|------|------|
| **B1: 直接添加 case** | `case 104: Handle_LdElem(...); break;` | ★★★ | ★★★ | ★★★ | ★★★ |
| B2: 宏生成 | 用宏统一生成带/不带 check 的 case | ★★ | ★★★ | ★★★ | ★★ |

**推荐: B1**

最直接：LdElemNoChk = 去除 bounds check 的 LdElem，StElemNoChk = 去除 bounds check 的 StElem，LdElemANoChk = LdElemA 的 NoChk 版。

```cpp
// 在 Handle_LdElem 后添加
case 104: Handle_LdElemNoChk(frame, instrs[frame.pc]); break;
case 105: Handle_StElemNoChk(frame, instrs[frame.pc]); break; 
case 106: Handle_LdElemANoChk(frame, instrs[frame.pc]); break;
```

`Handle_LdElemNoChk` = `Handle_LdElem` 去掉 null/bounds check。
`Handle_StElemNoChk` = `Handle_StElem` 去掉 null/bounds check + resize fallback。
`Handle_LdElemANoChk` = address-of 模式，直接返回 `&arr->elements[index]`。

**改动量**: ~30 行 C++
**预期收益**: 
- 涉及数组密集操作的方法 (Enum::GetValues, Array::Copy): **2-10x** (从 fallback 回到 fast dispatch)
- 全量评估: 因这些方法之前 fallback 到 InterpreterVM，提升极端显著

#### 风险
- 低。NoChk 语义就是 caller 保证 bounds → 如果 IR lowering 有 bug 则越界访问不被捕获。但 IR lowering 通常在前面的代码已经做了 check。

---

## P1: Medium Investment (预期 1-2 周)

### P1-A: Typed Array Fast Path

#### 现状
```cpp
// Handle_NewArr (fast_dispatch.cpp:892)
auto* arr = static_cast<interpreter::ArrayStorage*>(...);
::new (arr) interpreter::ArrayStorage();
arr->elements.resize(len);  // O(n): constructs len InterpreterValues (17+ bytes each)
```

所有数组都是 `vector<InterpreterValue>`，即使 `int[]` 也是 16 字节一个 slot。len=100 时 resize 构造 100 个 InterpreterValue（每个 ~800-1200 cycles）。对于原始类型的数组（int[], byte[], float[]），这是巨大的浪费。

同时 Handle_LdElem/Handle_StElem 也通过 PushIV/PopIV 做 InterpreterValue deep copy。

#### 方案对比

| 方案 | 描述 | 性能 | 架构 | 维护 | 热更 |
|------|------|------|------|------|------|
| **A1: flat buffer per element type** | ArrayStorage 中存 raw buffer + element_size + element_tag，按类型存取 | ★★★ | ★★ | ★★ | ★★★ |
| A2: SmallArrayStorage<256> inline | 小数组 inline buffer + 大数组回退到 vector | ★ | ★★ | ★★ | ★★ |
| A3: 只在原始类型上用 flat buffer | IROpCode::NewArr 携带 element_type_tag，根据 tag 选择存储方式 | ★★★ | ★★★ | ★★★ | ★★★ |

**推荐: A3 — IR lowering 传递 element type，fast_dispatch 分路径**

#### 详细设计

**Step 1: IRInstruction 添加字段**
```cpp
// IRInstruction 新增
uint8_t array_elem_tag = 0xFF;  // ValueTag for array elements (0xFF = unknown/object)
```

IR lowering 时，已知数组的 element type：
- `int[]` → `array_elem_tag = Int32`
- `byte[]` → `array_elem_tag = Int32` (byte 用 int32 表示)
- `string[]` → `array_elem_tag = ObjectRef`
- 未知 → `0xFF` (现有行为)

**Step 2: ArrayStorage 添加 flat buffer 支持**
```cpp
struct ArrayStorage {
    uint32_t type_token = 0;
    uint8_t elem_tag = 0xFF;       // ValueTag of elements
    uint32_t elem_size = 16;       // sizeof(InterpreterValue) default
    void* flat_data = nullptr;     // raw buffer for primitive types
    uint32_t flat_capacity = 0;
    CHAOS_IL2CPP_VECTOR(interpreter::InterpreterValue) elements;  // existing, for object arrays
};
```

**Step 3: Handle_NewArr 分路径**
```cpp
static void Handle_NewArr(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    uint32_t len = static_cast<uint32_t>(frame.stack[--frame.sp]);
    
    uint8_t elem_tag = instr.array_elem_tag;
    bool is_primitive = (elem_tag != 0xFF && elem_tag <= Float64);
    
    if (is_primitive) {
        // Flat buffer path: one malloc for raw data
        uint32_t es = (elem_tag == Int64 || elem_tag == Float64) ? 8 : 4;
        auto* storage = AllocArrayStorage(...);
        storage->elem_tag = elem_tag;
        storage->elem_size = es;
        storage->flat_data = CHAOS_IL2CPP_MALLOC(len * es);
        storage->flat_capacity = len;
        // memset zero (safely uninitialized for int/byte)
        std::memset(storage->flat_data, 0, len * es);
        frame.PushObj(storage);
    } else {
        // Existing path for object arrays
        ...
    }
    ++frame.pc;
}
```

**Step 4: Handle_LdElem/StElem 分路径**
```cpp
static void Handle_LdElem(FastFrame& frame, const interpreter::IRInstruction&) noexcept {
    uint32_t index = frame.stack[--frame.sp];
    auto* arr = reinterpret_cast<ArrayStorage*>(frame.stack[--frame.sp]);
    
    if (arr == nullptr || index >= arr->Size()) { PushNull; return; }
    
    if (arr->flat_data != nullptr) {
        // Primitive: load from flat buffer directly
        switch (arr->elem_tag) {
        case Int32:  frame.PushI32(static_cast<int32_t*>(arr->flat_data)[index]); return;
        case Int64:  frame.PushI64(static_cast<int64_t*>(arr->flat_data)[index]); return;
        case Float32: frame.PushF32(static_cast<float*>(arr->flat_data)[index]); return;
        case Float64: frame.PushF64(static_cast<double*>(arr->flat_data)[index]); return;
        }
    }
    // Existing InterpreterValue path
    frame.PushIV(arr->elements[index]);
}
```

**改动量**: ~100 行 C++ (IRInstruction 1字段 + ArrayStorage 3字段 + Handle_NewArr/LdElem/StElem 分路径)
**预期收益**:
- 原始类型数组创建: **10-50x** (O(1) malloc + memset 替代 O(n) vector 构造)
- 原始类型数组读写: **5-10x** (直接内存读写 vs InterpreterValue deep copy)
- 受影响方法: Enum::GetValues, Array::Copy, Buffer::Memcpy 等
- Enum::GetValues (创建 int[]): 现 postPatchNs ≈ 650ns → 预计 <100ns

#### 风险
- 中。需确保 flat_data 在 GC scan 时不被误扫 (原始类型数组不含 GC 引用)
- 回退：如果 elem_tag 未知或为 ObjectRef，走现有路径
- 需 DCHECK/assert 确保 flat_data 与 elements 互斥

---

### P1-B: 扩大 direct_fn 覆盖

#### 现状
```cpp
// Handle_Call 有三个路径:
// 1. direct_fn != null → 直接调用 (最快, ~2ns overhead)
// 2. MIC cache hit → call_cache direct_ptr (较快, ~3 atomics + call)
// 3. DoRaw → InterpreterDispatchRaw (~2000ns)
```

当前 `direct_fn` (AotDirectDispatch) 只在 pipeline 的 `_patch_bypass_0xC0000409.py` 阶段为已知 subjectId 预填。大量运行时库调用 (如 `System.String::Format`, `System.Array::Copy`) 没有 direct_fn，走 DoRaw 路径。

#### 方案对比

| 方案 | 描述 | 性能 | 架构 | 维护 | 热更 |
|------|------|------|------|------|------|
| **B1: IR lowering 阶段自动解析** | IR lowering 时尝试 ResolveMethodTable，成功则填 direct_fn | ★★★ | ★★★ | ★★ | ★★★ |
| B2: 运行时 lazy resolution + cache | 首次 DoRaw 后缓存 direct_ptr 到 call_cache | ★★ | ★★ | ★★ | ★★ |
| B3: kAotDirectFnTable 新增自动发现 | pipeline 自动扫所有被调方法，加入 AotDirectFnTable | ★★★ | ★★ | ★ | ★★★ |

**推荐: B1 — IR lowering 阶段自动解析**

IR lowering 时已经获取了 call_target (MethodInfoHandle)。尝试通过 `ResolveMethodTable` 获取 native 函数指针，成功则写入 `instr.direct_fn`。

```cpp
// IR lowering (ir_reg_alloc.cpp 或 IR builder)
if (instr.call_target != nullptr) {
    void* direct = chaos::il2cpp::interpreter::TryResolveDirectFn(instr.call_target);
    if (direct != nullptr) {
        instr.direct_fn = direct;
        instr.direct_ret_tag = /* resolve return type tag */;
    }
}
```

需要 `TryResolveDirectFn` 函数:
- 查 `kAotDirectFnTable` (现有)
- 尝试 `ResolveMethodTable` (通用解析)
- 对 non-virtual instance 方法通过静态类型 resolve
- 若成功，标记 is_patched 检测

**改动量**: ~50 行 C++ (TryResolveDirectFn + IR lowering 集成)
**预期收益**:
- 常见的运行时库调用 (String::Format, Array::Copy 等): 从 ~2000ns DoRaw → ~2ns direct call
- 受影响方法: String::Replace(302ns), Array::Copy(310ns), BitConverter::*, Math::* 等
- 预期 **10-100x** 提升

#### 风险
- 中。需处理 patched 方法: 如果 native 方法被 hotpatch，direct_fn 必须更新或标记 `is_patched`
- 需处理 virtual method: 只能 resolve 非 virtual 或 sealed 方法
- 回退：resolve 失败则 direct_fn 保持 null，走现有路径

---

## P2: Architecture-level (预期月级)

### P2-A: NewObj TLS Pool

#### 现状
```cpp
// Handle_NewObj (fast_dispatch.cpp:877)
auto* storage = static_cast<interpreter::InterpreterObject*>(
    CHAOS_IL2CPP_DOMAIN_CURRENT_ALLOCATE(sizeof(interpreter::InterpreterObject)));
::new (storage) interpreter::InterpreterObject();
frame.TrackPool(storage, DomainFreeInterpreterObject);
```

Box 有 TLS pool (kBoxPoolSize=8)，但 NewObj 没有。每个 `new T()` 或 `new Tuple(...)` 都走 `CHAOS_IL2CPP_DOMAIN_CURRENT_ALLOCATE` (heap alloc)。对于小对象，alloc 开销 > 对象初始化。

#### 方案对比

| 方案 | 描述 | 性能 | 架构 | 维护 | 热更 |
|------|------|------|------|------|------|
| **A1: 通用 TLS pool (2-3 种大小)** | 按 InterpreterObject + SmallFieldArray 大小池化 | ★★★ | ★★ | ★★ | ★★★ |
| A2: size-bucketed TLS pool | 64/128/256 bytes 三个 bucket | ★★★ | ★★ | ★ | ★★★ |
| A3: 只池化无 field 小对象 | field_count ≤ 2 的走 pool，其余 alloc | ★★ | ★★★ | ★★★ | ★★ |

**推荐: A1 — 通用 TLS pool**

扩展现有 `tls_box_pool` 机制:
```cpp
static constexpr uint32_t kObjectPoolSize = 16;
static thread_local void* tls_obj_pool[kObjectPoolSize];
static thread_local uint32_t tls_obj_pool_count = 0;

static void* AcquireObject() noexcept {
    if (tls_obj_pool_count > 0) {
        --tls_obj_pool_count;
        void* p = tls_obj_pool[tls_obj_pool_count];
        tls_obj_pool[tls_obj_pool_count] = nullptr;
        return p;
    }
    return CHAOS_IL2CPP_DOMAIN_CURRENT_ALLOCATE(sizeof(interpreter::InterpreterObject));
}

static void ReturnObjectToPool(void* p) noexcept {
    if (tls_obj_pool_count < kObjectPoolSize) {
        tls_obj_pool[tls_obj_pool_count++] = p;
    } else {
        static_cast<interpreter::InterpreterObject*>(p)->~InterpreterObject();
        CHAOS_IL2CPP_DOMAIN_CURRENT_FREE(p);
    }
}
```

**改动量**: ~40 行 C++
**预期收益**:
- 小对象创建: **3-10x** (TLS pool hit 无锁，CHAOS_IL2CPP_DOMAIN_CURRENT_ALLOCATE 有锁)
- 受影响: ConstructorInfo::Invoke(2656ns), new Tuple/KeyValuePair 等
- 在 reflection-heavy 场景 (reflection-binding, reflection-field-property): 预期 **1.5-2x**

#### 风险
- 低。pool 对象在 Return 时需要 `~InterpreterObject()`。但 `SmallFieldArray` 的析构需正确处理。
- 需和 DomainFree 配合: pool 中的对象在 domain 关闭时必须清理。

---

### P2-B: Scalar Field Access Bypass InterpreterValue Deep Copy

#### 现状
```cpp
// Handle_LdFld (fast_dispatch.cpp:915)
frame.PushIV(storage->fields[idx]);  // Deep copy InterpreterValue
// Handle_StFld (fast_dispatch.cpp:933)
storage->fields[idx] = val;         // Deep-copy assignment (可能 malloc/free)
```

每个 `ldfld`/`stfld` 都做 full InterpreterValue deep copy。对于 int32/float 等 scalar 类型 (占所有 field 操作的 >80%)，deep copy 完全没必要——直接读写 8 字节即可。

#### 方案对比

| 方案 | 描述 | 性能 | 架构 | 维护 | 热更 |
|------|------|------|------|------|------|
| **B1: PushI32/PushI64 替代 PushIV** | Handle_LdFld 中根据 tag 选择 PushI32/PushI64 | ★★★ | ★★★ | ★★★ | ★★★ |
| B2: InterpreterValue 改为 tagged union 不 deep copy struct | 改 InterpreterValue 内部数据结构 | ★ | ★ | ★ | ★ |
| B3: SmallFieldArray 存 raw bits | field_array 直接存 uint64_t + tag，不经过 InterpreterValue | ★★ | ★★ | ★ | ★★ |

**推荐: B1 — 按 tag 分路径**

```cpp
static void Handle_LdFld(FastFrame& frame, const interpreter::IRInstruction& instr) noexcept {
    void* obj = frame.PopObj();
    if (obj == nullptr) { frame.PushNull(); ++frame.pc; return; }
    auto* storage = static_cast<interpreter::InterpreterObject*>(obj);
    uint32_t idx = static_cast<uint32_t>(instr.field_offset);
    
    auto& field = storage->fields[idx];
    switch (field.tag) {
    case ValueTag::Int32:  frame.PushI32(field.i32); break;
    case ValueTag::Int64:  frame.PushI64(field.i64); break;
    case ValueTag::Float32: frame.PushF32(field.f32); break;
    case ValueTag::Float64: frame.PushF64(field.f64); break;
    case ValueTag::ObjectRef: frame.PushObj(field.obj); break;
    default:               frame.PushIV(field); break;  // Struct or Unknown → deep copy
    }
    ++frame.pc;
}
```

`Handle_StFld` 同理，先检查 val 的 tag，对 scalar 类型直接赋值跳过 deep copy：
```cpp
auto tag = static_cast<ValueTag>(frame.stack_tags[frame.sp - 2]); // val tag
if (tag <= ValueTag::Float64) {
    // Scalar: direct write, no deep copy
    storage->fields[idx].tag = tag;
    storage->fields[idx].i64 = frame.stack[frame.sp - 2];  // raw copy
} else {
    storage->fields[idx] = val;  // existing deep copy path
}
```

**改动量**: ~30 行 C++
**预期收益**:
- Scalar field 读写: **5-20x** (消除 InterpreterValue deep copy 的 malloc/free)
- Struct field 无变化 (仍走 deep copy)
- 受影响: PropertyInfo::GetValue/SetValue, FieldInfo::GetValue/SetValue 等 reflection 操作
- reflection-field-property 34 个方法: 预期 **2-5x** 提升

#### 风险
- 低。严格按 tag 分路径，scalar → 直接读写，non-scalar → 现有 deep copy 路径。
- 注意 ObjectRef 的 GC barrier 仍需保留 (StFld 的 SATB + dirty card)。

---

### P2-C: Dispatch Loop Overhead 削减

#### 现状
每条指令固定开销:
1. `g_fast_op_freq[op_val]++` — TLS 递增 (profile-only, 发布可关)
2. Debugger breakpoint 检查
3. OSR backedge 检测
4. 每 64 条指令 thread abort 检查

#### 方案对比

| 方案 | 描述 | 性能 | 架构 | 维护 | 热更 |
|------|------|------|------|------|------|
| **C1: #ifdef 条件编译** | PROFILE/SHIP 配置去掉 histogram + debugger | ★★★ | ★★★ | ★★★ | ★★★ |
| C2: 循环展开 + 批量指令处理 | 每次迭代处理 4 条指令后再检查 thread abort | ★★ | ★★ | ★ | ★★ |
| C3: 直接线程化 (threaded code) | 用 label pointers 数组替代 switch | ★★ | ★ | ★ | ★★ |

**推荐: C1 — 条件编译**

```cpp
// Non-profile builds: skip histogram
#if CHAOS_IL2CPP_PROFILE_ENABLED
    ++g_fast_op_freq[op_val];
#endif
```

Debugger 检查同理:
```cpp
#if CHAOS_IL2CPP_DEBUGGER_ENABLED
    if (CHAOS_UNLIKELY(DbgShouldPause(...))) { ... }
#endif
```

**改动量**: ~10 行 C++
**预期收益**:
- PROFILE/SHIP 构建每条指令节省 ~3-5 cycles
- ~5-10% 整体 interpreter 吞吐提升

#### 风险
- 极低。纯条件编译，运行时行为完全一致。

---

## 各方案影响范围与 cumulative 收益

### 影响覆盖率

| 方案 | 影响操作 | 占所有 HU 方法比例 | 单独预期提升 |
|------|---------|-------------------|-------------|
| P0-A: Box type_token | box + 后续 callvirt | 30-40% (reflection/enum/convert) | 2-5x |
| P0-B: NoChk opcodes | 数组操作 fallback 消除 | 5-10% | 2-10x |
| P1-A: Typed Array | newarr + ldelem/stelem 原始类型 | 15-20% | 5-50x |
| P1-B: 扩大 direct_fn | call 避开 DoRaw | 40-50% | 10-100x |
| P2-A: NewObj pool | newobj 小对象 | 20-30% | 3-10x |
| P2-B: Scalar field bypass | ldfld/stfld scalar | 30-40% | 5-20x |
| P2-C: Dispatch overhead | 所有指令 | 100% | 5-10% |

### Cumulative 预期效果

| 优化组合 | 预期 postPatchNsPerOp 降低 | 典型场景 |
|---------|--------------------------|---------|
| P0 全部 | 1.5-3x | Convert::ToChar |
| P0 + P1 | 5-20x | Enum::GetValues, String::Replace |
| P0 + P1 + P2 | 10-50x | reflection-field-property, ConstructorInfo::Invoke |

### 性能差异基准 (预期)

以 convert-char 为例 (现 postPatch 3462ns/op):
- 仅 P0 (type_token): ~1500ns (2.3x)
- P0 + P1-B (direct_fn for Convert::ToChar): ~100ns (35x)
- P0 + P1-B + P2-B (scalar field bypass): ~80ns (43x)
- 最终 vs NET8(25ns): 仍慢 3.2x — 已进入 3x 阈值内

---

## 实施路径

### Phase 1: P0 (2-3 days)
1. P0-A: Box type_token — 1 line change
2. P0-B: NoChk opcodes — 30 lines + 3 new handlers
→ 立即 benchmark 验证: convert-char, enum-parsing, reflection-field-property

### Phase 2: P1 (1-2 weeks)
3. P1-B: 扩大 direct_fn — 50 lines (IR lowering)
4. P1-A: Typed array — 100 lines (IRInstruction + ArrayStorage + handlers)
→ benchmark 验证: enum-parsing, reflection-binding, type-runtime-handles

### Phase 3: P2 (2-4 weeks)
5. P2-A: NewObj pool — 40 lines
6. P2-B: Scalar field bypass — 30 lines
7. P2-C: Dispatch overhead — 10 lines
→ benchmark 验证: 全量 family

---

## 验证方法

每个优化点独立验证:
1. 修改前: `entry.exe --hotupdate-and-benchmark N 100000` 记录 postPatchNsPerOp
2. 修改后: 同一命令对比
3. Dashboard 重新生成: `python _postprocess.py`
4. 关注的 families: convert-char, reflection-field-property, reflection-binding, enum-parsing, type-runtime-handles

## blocking_questions
1. P1-A typed array: `ArrayStorage.flat_data` 在 GC scan 时是否会被误扫为对象引用？ → 需在 GC scan 路径排除 elem_tag 为 primitive 的 flat_data
2. P1-B direct_fn: 被 hotpatch 后的方法如何使 direct_fn 失效？ → 需 patch_generation 检测或 direct_fn == patched_entry_ptr

## watch_items
- P1-A typed array: SmallFieldArray 的 resize 逻辑对 flat_data 不适用，需确保二者互斥
- P2-A NewObj pool: 返回 pool 前需确保 InterpreterObject 被正确重置
