# C-P3-2: V5 语义等价性验证 + V6 内存/GC 正确性验证

> 父任务: 20260523-codegen-industrialization
> source_task_id: C-P3-2
> source_relation: roadmap-child
> 创建日期: 2026-05-23
> 上游设计: 父 roadmap brainstorm-v1-01.md（已确认）
> 计划来源: roadmap-child-auto

---

## Phase

`completed`

## Type

`plan`

## 设计摘要

### 边界拍板

- **目标**: 建立 codegen 语义等价性验证框架（V5）+ 内存/GC 正确性验证（V6）
- **范围**: `testing/foundation-dll/_core/python/` 验证管线 + `testing/config/foundation-dll/pipeline-config.yaml`
- **非目标**: 不修改 codegen 生成逻辑、不修改 GC 运行时、不修改 CI/CD 配置
- **冲突域**: `testing/`

### V5 方案

1. **激活 asm_compare 阶段** — 从 pipeline-config.yaml migrationSkips 移除，适配新框架路径
2. **fact AOT vs fact JIT 交叉验证** — 新增 stage: 比较 AOT 和 JIT 的 fact 结果是否一致
3. **IL→C++ 语义模式验证** — 新增 stage: 对关键 IL 模式（arithmetic、call、branch）验证生成 C++ 使用预期 lowering

### V6 方案

1. **GC stress 集成** — 新增 stage: codegen 完成后运行 GC stress 测试（T4 代码路径）
2. **GC slot map 正确性** — 新增 principle check: 验证 T4 生成代码的精确 GC slot map 完整性
3. **Write barrier 验证** — 新增 principle check: 验证所有托管引用赋值使用 CHAOS_IL2CPP_WRITE_BARRIER

### Authority 决策

- 属于 Thread C (HT) Phase 3 任务
- 依赖 C-P3-1（已完成）
- 不影响 Thread A (托管 C#) 或 Thread B (原生 codegen)

### 三优先级权衡结论

| 维度 | V5 | V6 |
|------|-----|-----|
| P1 性能 | 验证框架不影响生成代码 | GC 正确性保障防止性能退化 |
| P2 架构 | 补充验证管线缺失环节 | 补充 GC 安全验证能力 |
| P3 HotUpdate | 确保热更后语义一致 | 确保热更后 GC 安全 |

无优先级冲突。

---

## 问题清零

```yaml
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
clearance_source: parent-roadmap
```

## 入口

`dev-writing-plans`（继承父 roadmap 上下文，最小 preflight 后直接执行）

## 最近摘要

2026-05-23: C-P3-2 V5+V6 完成。共实现 5 项验证增强：

**V5 语义等价性验证:**
1. **asm_compare 阶段激活** — 从 pipeline-config.yaml migrationSkips 移除，加入 defaultStages，JIT vs AOT 指令级对比全面启用
2. **fact AOT vs JIT 交叉验证** — 新增 `fact_cross_verify` 阶段，对比 AOT 和 JIT fact 结果，发现差异即标记语义分歧
3. **IL→C++ 语义模式检查** — 新增 4 项 principle check（lowering density、arithmetic locality、branch directness、stub contamination），集成到 audit 阶段

**V6 内存/GC 正确性验证:**
4. **GC stress 阶段** — 新增 `gc_stress` 阶段（默认跳过），运行 GC stress 测试集
5. **GC slot map + write barrier 检查** — 新增 2 项 principle check（gc_slot_map integrity、write barrier usage），集成到 audit 阶段

完成文件:
- 新建 4 个 Python 模块: `fact_cross_verify.py`, `semantic_pattern_check.py`, `gc_stress_stage.py`, `gc_correctness_check.py`
- 修改 2 个文件: `audit_runner.py`（集成 V5+V6 checks）、`orchestrator.py`（注册 2 个新阶段）
- 修改 1 个配置: `pipeline-config.yaml`（激活 asm_compare、添加 gc_stress 配置）

## 下一步

归档完成。回写父 roadmap。C-P3-3 (D3 Performance baseline) 为剩余 Thread C 任务。
