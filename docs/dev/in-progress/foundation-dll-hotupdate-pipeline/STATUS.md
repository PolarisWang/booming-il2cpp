# STATUS

## Phase
`brainstorming` → 设计已更新（统一 return-value 模式），等待分流到 plan

## 边界拍板
- HotUpdate Subject 覆盖全部方法（不只是值类型）
- Void 方法用 sentinel 值（42L / 142L）
- ATG 两次调用（baseline + `--patch-mode`）
- 第二次 TPG 不需要（PatchDataExtractor subjectOnly）
- Hotupdate stage 默认纳入 pipeline
- Patch data 通过外部文件传入 entry.exe（方案 A）

## Authority 决策
- AutoTestGenerator 负责 Subject_N 生成（baseline + patch）
- PatchDataExtractor 负责 .patchdata 生成（已有 subjectOnly 模式，直接复用）
- TPG 只修改 RuntimeEntry.cpp.scriban（增加外部 patch data 加载能力）
- Pipeline Python 层组合各步骤

## 当前结论
设计已定，方案已收敛，blocking_questions 已清零。

## 风险评估摘要
- PatchDataExtractor subjectOnly 模式兼容性：低风险，需 E2E 验证
- ResultToLong 精度损失：低风险，浮点类型考虑用 BitConverter 方案
- ATG 第二次调用时间累积（21 chunks × ~30s）：中风险，可接受（hotupdate 默认执行）
- 外部文件加载 patch data 生命周期：低风险，ApplyPatchFromMemory 已复制数据

## 三优先级权衡结论
- P1 性能：无影响（hotupdate 可选 stage，不拖慢默认流程）
- P2 架构：一致（复用已有工具，不引入新层）
- P3 热更：直接验证（目标）

## blocking_questions
`[]`

## question_clearance
`cleared`

## clearance_confirmed_by_user
待确认——当前文档供审查

## 关键文档
- `design-v1-01.md`

## 修改清单（预估）

| 文件 | 改动 | 规模 |
|------|------|------|
| `AutoTestGenerator/Program.cs` | 新增 `--patch-mode` flag + patch value 生成 | ~50 行 |
| `AutoTestGenerator/TestEmitter.cs` | HotUpdate Subject 改为 return long | ~30 行 |
| `AutoTestGenerator/ValueGenerator.cs` | 新增 `GetPatchReturnExpression` + `ResultToLong` | ~40 行 |
| `AutoTestGenerator/Models.cs` | 可能需扩展 `GeneratedSubject` | ~5 行 |
| `TestProject.RuntimeEntry.cpp.scriban` | 新增 `--patch-data` + `ApplyHotpatchFromFile` | ~40 行 |
| `verification/stages/hotupdate_chunk.py` | 重写：ATG patch pass + PatchDataExtractor + 运行 | ~80 行 |
| `verification/orchestration/context.py` | 可能需新增配置字段 | ~10 行 |
| `verification/chunk_pipeline.py` | 默认 stages 加入 hotupdate | ~5 行 |

## 下一步入口
用户审查设计后，分流到 `roadmap`（如需分阶段）或 `writing-plans`（直接进入实现计划）。
