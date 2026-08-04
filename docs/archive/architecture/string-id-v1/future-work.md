# String ID V1 — Future Work

## 摘要

本文件记录 `Stable String ID` 方案中明确推迟到后续迭代的 4 项工作，附带前置依赖、入场条件和预估风险。这些条目由 `string-id-v1` spec 统一跟踪 lifecycle。

## 关联 Authority

- **主 spec**：`docs/archive/architecture/string-id-v1/INDEX.md`（待创建）
- **实施 plan**：`C:\Users\haochuan.wang\.claude\plans\serene-gliding-milner.md`
- **准入规则**：`wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/unsupported-feature-policy.md`

---

## 1. InterpolatedStringHandler 深度优化

**现状**：`DefaultInterpolatedStringHandler` 通过多次 `ConcatStringPair`（每次两两拼接）实现。每次 concat 产生临时 `new char[N+M]` buffer，最终 `Intern` 产生 StringId。

**目标**：改为 `StringId[]`（或 utf8 片段累积），一次性拼接 → `Intern`，消除中间的 char[] 分配和多次 hash 查找。

**收益**：消除 N-1 次临时 char[] 分配（N = 插值片段数），减少 N-1 次 `Intern` hash 计算。

**前置依赖**：
- Phase B concat 链路稳定（Step 10-11 完成）
- `DefaultInterpolatedStringHandler` 的 codegen pattern 可被识别为 handler 模式

**风险**：
- handler 的 `AppendFormatted<T>` 泛型分支可能涉及物化，需要 type-aware 处理
- `ToStringAndClear` 返回后 handler 可复用，状态清理逻辑需保持正确

---

## 2. 字符串 GC 集成

**现状**：物化后的 `chaos_SR_String` 通过 `new` 分配手动管理，跟随 domain 生命周期。项目当前无 GC 子系统。

**目标**：将物化对象接入 GC，同时保持 StringId 本身无 GC 开销。

**关键工作**：
- 物化缓存（`chaos_SR_String.string_id` 反向引用）改用 `GCHandle` 锁定，防止 GC 移动
- GC 扫描时识别物化对象：`string_id != 0` 表示已物化，需扫描 `utf8_data` 指针
- StringId → 物化对象的正向引用不阻止 GC 回收（weak semantics）

**前置依赖**：
- 项目 GC 子系统上线
- `chaos_string_materialize`（Step 14）稳定

**风险**：
- 物化对象被 GC 回收后，后续对同一 StringId 的 `chaos_string_materialize` 调用需重新物化（逻辑正确但可能抖动）
- GC 移动 `utf8_data` 指向的 char[] 时，需更新 `chaos_SR_String.utf8_data` 指针

---

## 3. 物化对象回收

**现状**：物化对象跟随 domain 生命周期，domain unload 时批量清理。AOT domain 永驻内存，其物化对象永不释放。

**目标**：引入精细化的物化对象生命周期管理。

**关键工作**：
- 引用计数或 weak reference 追踪物化对象是否仍被引用
- 无引用时主动回收物化对象（归还堆内存 + char[] 内存）
- 避免长期运行的 AOT domain 中物化对象堆积导致内存泄漏

**前置依赖**：
- 物化路径（Step 14）稳定
- GC 集成完成（#2）

**风险**：
- 计数字段本身增加了 `chaos_SR_String` 对象体积
- 回收后再次需要同一 StringId 时重新物化，可能导致抖动（需保留回收阈值/策略）

---

## 4. ARM64/移动端平台适配

**现状**：仅在设计层面验证了 tag bit（bit 63）在 ARM64 用户态地址空间的安全性。所有开发和测试在 Windows x64 环境进行。

**目标**：确认并调优 StringId 方案在 ARM64 iOS/Android 上的正确性和性能。

**关键工作**：
- ARM64 iOS/Android 实测 `static_assert`：`CHAOS_STRING_ID_TAG` 不落入 mmap 返回的地址范围
- 确认 kernel 地址空间布局（Android 内核配置差异可能影响用户态地址位数）
- 移动端 `malloc`/`new` 行为差异下的 performance tuning
- String table 的 AOT 段访问模式在移动端 cache hierarchy 下的效率

**前置依赖**：
- Phase C 全部完成（Step 14-17）
- iOS/Android CI 构建环境就绪

**风险**：
- Android 某些厂商定制内核可能使用非标准用户态地址空间布局
- 移动端内存带宽更低，`Resolve` 的 cache miss 惩罚相对更大
