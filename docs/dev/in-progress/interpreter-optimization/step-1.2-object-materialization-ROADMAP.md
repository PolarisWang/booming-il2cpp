# 1.2 对象物化专项 — 可执行 Roadmap（下个会话接续入口）

> **性质**：将 `step-1.2-materialization-scope.md` 的 8 注入点调研转成可分段执行的专项计划。
> **状态**：独立专项（跨会话）。当前会话已安全落地 P0-二期 barrier guards(c606783cc)，为其铺路。

## 目标
让解释器沙箱对象模型（InterpreterObject/ArrayStorage/BoxedValue）与 runtime-core 真实 GC 对象（Il2CppObject/ArrayHeader/BoxedValueHeader）在 3 处跨边界出入口正确互转（物化/反物化），消除"裸指针当 GC 对象""非GC存储注册GC root""撤barrier后漏根"三类内存安全问题（deepdive 谷#1）。

## 前置依赖（memory P0-二期，方案1+3）
- **已做**：撤 g_static_fields 反向 GC-root 注册（e6ea531e7）；非GC容器 barrier 用 chaos_is_gc_pointer 守卫跳过（c606783cc）。
- **关键事实**：解释器槽位确实能持真实 GC 指针（interpreter_vm.cpp:1085 from_obj 直传），撤 barrier 后必须保证"持真实GC对象时仍被 GC 可见" → 需要物化 or handle。

## 分阶段（每步独立 commit + GC-stress 强回归）

### 1.2-a：对象判别机制（为物化铺路 + 断言护栏）
- **goal**：可靠判别"解释器对象 vs 真实 GC 对象"，不破坏 offset0/MIC（`type_token`/`frame_type` 占 offset 0 用于分派）。
- **做法（推荐）**：不要加新头字段。用 `chaos_is_gc_pointer(ptr)`（地址>=g_heap_base 判定，gc_api.h）区分——解释器 MALLOC 对象地址在 g_heap_base 下，真实 GC 对象在上。
- **护栏**：`from_obj`(interpreter_vm.cpp:1085) 处加 debug 断言：若槽将持 GC 指针，标注/登记到 GC 可达（过渡用 GcRoot/handle 表），否则 assert fail —— 防"撤barrier 后住真实GC对象漏根"。
- **验证**：GC-stress（穿插真实 GC 分配 + 解释器 from_obj 返回真实对象）+ 断言触发的单测。

### 1.2-b：参数方向物化（解释器→真实 GC object）
- **goal**：解释器把沙箱对象传给 AOT 时，物化为真实 GC 对象再传（消除裸指针互不改读错）。
- **8 注入点（参数向 4 处，step-1.2-materialization-scope.md §一）**：precache_dispatch_raw.cpp:199-209/277-288、fast_dispatch_call.inc:208-212、ir_reg_alloc.cpp:1949-1951/2017-2019、interpreter_dispatch.cpp:103-107。
- **物化 API**：`abi->object_new(type_token→TypeInfoHandle, ...)` + `field_set_value` 拷字段；type_token→TypeInfoHandle 经 token_resolver.cpp:142-150（resolve_type_by_token）。
- **数组**：最大硬墙（ArrayStorage vector vs 原生连续内存），单列 1.2-d。

### 1.2-c：返回方向反物化（真实 GC object → 解释器可读）
- **goal**：解释器拿到 native 返回的真实 Il2CppObject* 后能正确读字段/元素。
- **8 注入点（返回向 4 处）**：precache_dispatch_raw.cpp:384-389、interpreter_dispatch.cpp:211-216、fast_dispatch_call.inc:616-618/691-700、ir_reg_alloc.cpp:1984-1986/2042-2044。
- **难点**：反物化需在边界新建解释器对象拷贝，或标记类型让解释器按真实 GC 布局读（反向 bridge）。此步依赖 1.2-a 判别。

### 1.2-d：数组/box 特例
- **数组**：解释器 ref-array = vector<InterpreterValue> vs 真实 chaos_managed_array(header+连续) —— 物化/反物化需重建连续内存。
- **box**：解释器 BoxedValue(MALLOC+type_token) vs 真实 BoxedValueHeader(GcAllocate+TypeInfoHandle) —— 统一走 BoxValueObject/UnboxValueObject。

## 强回归门（每步）
- chaos_interpreter + chaos_pal 编译；解释器 smoke/new_features/integration/entry 4/4。
- **GC-stress**：并发 GC + 解释器长循环 + 真实对象分配/返回往返 —— 无悬挂/漏根/崩溃。
- unit/snapshot：OVERALL: OK。
- 跨 AOT 对象往返单测：传解释器对象给 AOT 方法 → 正确；AOT 返回真实对象 → 解释器正确读。

## 关键文件集
- 边界：`instantiation/{precache_dispatch_raw,interpreter_dispatch,interpret_method_call}.cpp` + `interpreter/fast_dispatch_call.inc` + `ir_reg_alloc.cpp`
- 判别/GC：`gc_api.h` chaos_is_gc_pointer、`token_resolver.cpp`、`interpreter_vm.cpp` from_obj/NewObj/NewArr
- 对象模型：`interpreter_vm.h`（InterpreterObject/ArrayStorage）、`runtime_abi.h`（object_new/field_set）

## 收尾
1.2-a → b → c → d 完成后：解释器与 AOT 对象可正确互通，keep-native 依赖减少，谷#1 闭环。届时可回做 4.1（ldtoken → 真实 RuntimeTypeHandle，现被对象模型阻塞）。
