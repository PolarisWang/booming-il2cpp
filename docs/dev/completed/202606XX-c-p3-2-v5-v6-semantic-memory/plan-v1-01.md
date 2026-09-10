# C-P3-2: V5 语义等价性 + V6 内存/GC 正确性 — 实现计划

> **面向执行:** 直接实现。步骤使用复选框（`- [ ]`）语法跟踪。
> **确认方式:** 每个步骤完成后验证构建通过 + 测试无回归。

**目标:** 在 foundation-dll verification pipeline 中补充语义等价性和 GC 正确性验证能力。

**架构:** 在现有 13-stage pipeline 基础上：
- V5: 激活被迁移跳过的 asm_compare 阶段 + 新增 fact AOT vs JIT 交叉验证 stage + 新增 principle check
- V6: 新增 GC stress stage + GC slot map principle check + write barrier principle check

**技术栈:** Python (验证管线), GoogleTest (GC 测试), pipeline-config.yaml

**架构审核模式:** normal

**结构告警重点:** 文件职责与拆分 — 新增 stage 放在 `testing/foundation-dll/_core/python/` 中独立文件

**设计文档:** 父 roadmap brainstorm-v1-01.md（已在 STATUS.md 中提取摘要）

**问题清零来源:** parent-roadmap（clearance_confirmed_by_user: true）

**计划来源:** roadmap-child-auto

**预期知识沉淀:** n/a（验证框架本身不是长期知识）

**收尾约束:** 执行完成后必须进入"结构告警与架构审视 -> 测试通过 -> 归档 completed -> 回写父任务"固定链路。

---

## 修改文件清单

| 操作 | 文件 | 用途 |
|------|------|------|
| 修改 | `testing/config/foundation-dll/pipeline-config.yaml` | 取消 asm_compare migration skip |
| 修改 | `testing/foundation-dll/_core/python/orchestrator.py` | 注册新 stages |
| 新增 | `testing/foundation-dll/_core/python/fact_cross_verify.py` | V5: fact AOT vs JIT 交叉验证 stage |
| 新增 | `testing/foundation-dll/_core/python/semantic_pattern_check.py` | V5: IL→C++ 语义模式 principle check |
| 修改 | `testing/foundation-dll/_core/python/audit_runner.py` | V5: 集成语义模式 check |
| 新增 | `testing/foundation-dll/_core/python/gc_stress_stage.py` | V6: GC stress stage |
| 新增 | `testing/foundation-dll/_core/python/gc_slot_map_check.py` | V6: GC slot map principle check |
| 修改 | `testing/foundation-dll/_core/python/models.py` | 新 stage 所需的数据模型 |
| 修改 | `testing/config/foundation-dll/pipeline-config.yaml` | V6: 新增 GC stress stage 配置 |

---

## 步骤

### Phase 1: V5 asm_compare 激活

- [ ] **1.1** — 修改 `pipeline-config.yaml`: 从 migrationSkips 移除 `asm_compare: true`
- [ ] **1.2** — 验证 `asm_compare_runner.py` 在新框架路径下能正确找到旧模块
- [ ] **1.3** — 跑一次 family verify 确认 asm_compare 阶段通过

### Phase 2: V5 fact AOT vs JIT 交叉验证

- [ ] **2.1** — 创建 `fact_cross_verify.py`: 
  - stages 中读取 `fact` 和 `fact_jit` 结果
  - 对比两者的 `passed/total` 是否一致
  - 如果不一致 → status=failed + 记录差值
  - 如果一致 → status=passed
  - 如果 fact_jit 不存在或被跳过 → status=skipped
- [ ] **2.2** — 注册到 `orchestrator.py` STAGES 列表（放在 fact_jit 之后、audit 之前）
- [ ] **2.3** — 跑一次 family verify 确认新 stage 通过

### Phase 3: V5 IL→C++ 语义模式验证 (principle check)

- [ ] **3.1** — 创建 `semantic_pattern_check.py`:
  - 扫描生成 C++ 中关键 IL 模式的 lowering 质量
  - 检查点:
    - arithmetic 操作使用本地计算（`_s0.i4_0 + _s0.i4_1`）而非运行时调用
    - call 操作直接调用目标函数而非通过解释器 fallback
    - conditional branch 使用真实 Jcc 指令而非仿真路径
  - 返回 ALIGNED/CONCERN/VIOLATION
- [ ] **3.2** — 集成到 `audit_runner.py` 作为额外的 principle check
- [ ] **3.3** — 跑一次 family verify 确认新 check 运行

### Phase 4: V6 GC stress 集成

- [ ] **4.1** — 创建 `gc_stress_stage.py`:
  - 在 codegen 完成后，运行 GC stress 测试集
  - 执行 `chaos_gc_stress_test` 和 `chaos_bgc_stress_test`
  - 解析输出: "N scenarios, M passed, F failed" 
  - 如果有任何 failed → status=failed
  - 如果二进制不存在 → status=skipped
- [ ] **4.2** — 注册到 `orchestrator.py` STAGES 列表（放在 audit 之后）
- [ ] **4.3** — 添加到 `pipeline-config.yaml` timeouts 和 migrationSkips（默认跳过，按需开启）
- [ ] **4.4** — 跑一次验证配置正确性

### Phase 5: V6 GC slot map + write barrier checks (principle checks)

- [ ] **5.1** — 创建 `gc_slot_map_check.py`:
  - 扫描 T4 生成代码的 GC slot map 元数据
  - 验证所有包含 ObjectRef vreg 的方法有非空 gc_slot_map
  - 验证 slot map 编码正确（GPR/Stack kind 合法）
  - 返回 ALIGNED/CONCERN/VIOLATION
- [ ] **5.2** — 集成到 `audit_runner.py`
- [ ] **5.3** — 创建 write barrier check（集成到 gc_slot_map_check.py 或独立文件）:
  - 扫描生成 C++ 中托管引用 `StFld` 操作
  - 验证使用了 `CHAOS_IL2CPP_WRITE_BARRIER` 宏
- [ ] **5.4** — 跑一次验证

### Phase 6: 收尾

- [ ] **6.1** — 结构告警与架构审视: 确认文件职责清晰、无重复逻辑
- [ ] **6.2** — 最终验证: 跑一次 foundation-dll verify-family 确认无回归
- [ ] **6.3** — 更新当前 STATUS.md 为 `completed`
- [ ] **6.4** — 物理归档到 `docs/dev/completed/`
- [ ] **6.5** — 回写父 roadmap STATUS.md

---

## 验证命令

```bash
# V5: 验证 asm_compare 激活
cd D:/agent/booming-il2cpp
python -m testing.foundation_dll._core.python.cli verify-family --family convert-char --assembly System.Private.CoreLib

# V6: 验证 GC stress stage
python -m testing.foundation_dll._core.python.cli verify-family --family convert-char --assembly System.Private.CoreLib --no-skip-gc-stress

# 完整管线验证
python -m testing.foundation_dll._core.python.cli verify-family --family convert-char --assembly System.Private.CoreLib --mode strict

# 构建验证 (native)
cmake --build build/native --config RelWithDebInfo
```
