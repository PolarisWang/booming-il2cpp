# Foundation-dll 项目结构重构 — Roadmap v1-01

## 1. 目标

将 foundation-dll 从当前分散的 5-project-per-family 结构重构为统一的源码/构建分离架构。managed 源码集中在一个 family 目录下，`.csproj`/`.sln` 统一输出到 `solution/`，cmake 项目统一输出到 `solution_native/`，il2cpp 产出到 `il2cpp_dist/`，实现完整 il2cpp 翻译链路和 native cmake 测试 exe 构建。

## 2. 范围边界

- **范围内**: generator 脚本改造、文件迁移与清理、il2cpp 完整链路、native cmake 构建、wiki 更新
- **范围外**: 非 foundation-dll 的其他 verification 体系不改；现有 handwrite 测试逻辑不改（仅迁移位置）

## 3. 非目标

- 不改动 Chaos.IL2CPP.CodeGen 内部的 il2cpp 翻译引擎
- 不改动现有 test 断言逻辑和 benchmark 测量逻辑
- 不改动 ledger JSON 格式和 gap analyzer

## 4. 阶段列表

### Phase 1 — Generator 改造（输出新目录结构）

- **goal**: 改造所有 Python generator，使其输出到新的目录结构（src/、src/patch/、managed_test/、solution/ 等）
- **exit_criteria**: 跑 generator 后，新目录结构下的文件全部生成，`dotnet build solution/FoundationDllTranslationSolution.sln` 0 error
- **deliverables**: 修改后的 `test_code_generator.py`、`execution_project_generator.py`、`execution_solution_generator.py`、`native_codegen_generator.py`

### Phase 2 — 文件迁移与清理

- **goal**: 将现有文件迁移到新目录结构位置，删除旧目录，更新 .gitignore
- **exit_criteria**: git status 显示所有文件在新位置，旧目录全部删除，build 通过
- **deliverables**: 迁移后的 .cs/.csproj 文件，更新后的 .gitignore

### Phase 3 — 完整 il2cpp 链路

- **goal**: 从 managed DLL 出发，走完整 il2cpp 翻译链路，输出 native-aot.generated.cpp 到 il2cpp_dist/
- **exit_criteria**: il2cpp_dist/ 下生成 native-aot.generated.cpp + metadata.h + runtime_helper_shapes.h，且可通过 cmake 编译
- **deliverables**: il2cpp pipeline 集成代码，输出文件到 il2cpp_dist/

### Phase 4 — Native cmake 构建

- **goal**: 为 solution_native/ 生成完整 cmake 项目，test/benchmark/hotupdate exe 可编译执行
- **exit_criteria**: cmake --build solution_native/ 产出 test/benchmark/hotupdate exe
- **deliverables**: solution_native/ 下的 CMakeLists.txt，native_test/ 下的 C++ 源码

### Phase 5 — Wiki 更新

- **goal**: 更新 il2cpp 核心翻译路径参考和 foundation-dll 测试流程文档
- **exit_criteria**: wiki 文档覆盖新架构下的完整测试流程
- **deliverables**: 更新后的 wiki 文档

## 5. 每阶段完成定义

### Phase 1 — Generator 改造

| 字段 | 内容 |
|------|------|
| goal | 改造 generator 输出新目录结构 |
| exit_criteria | generator 跑完新目录结构正确，`dotnet build solution/*.sln` 0 error |
| deliverables | 修改后的 4 个 generator Python 脚本 |
| dependencies | 无（从零改造） |
| resolved_decisions | 目录角色划分、csproj 模板、sln 结构已在 brainstorm 确认 |
| watch_items | 生成代码的 correctness 需要 build 验证 |

### Phase 2 — 文件迁移与清理

| 字段 | 内容 |
|------|------|
| goal | 迁移现有文件到新位置，删除旧目录 |
| exit_criteria | git status clean，build 通过 |
| deliverables | 迁移后文件布局，更新后的 .gitignore |
| dependencies | Phase 1（需要 generator 先能生成新结构） |
| resolved_decisions | handwrite 文件保留在新位置 |
| watch_items | 大量文件删除需小心 git 历史 |

### Phase 3 — 完整 il2cpp 链路

| 字段 | 内容 |
|------|------|
| goal | managed DLL → il2cpp → native-aot.generated.cpp |
| exit_criteria | il2cpp_dist/ 下生成完整产出，可 cmake 编译 |
| deliverables | il2cpp pipeline 集成，il2cpp_dist/ 输出 |
| dependencies | Phase 2（需要 managed DLL 已在新位置可编译） |
| resolved_decisions | il2cpp_dist/ 不提交 git |
| watch_items | CI 环境需要 dotnet + C++ 工具链 |

### Phase 4 — Native cmake 构建

| 字段 | 内容 |
|------|------|
| goal | solution_native/ cmake 项目，test/benchmark/hotupdate exe |
| exit_criteria | cmake 编译产出 3 个 exe，运行返回预期结果 |
| deliverables | solution_native/CMakeLists.txt，native_test/ 下 C++ 源码 |
| dependencies | Phase 3（需要 il2cpp_dist/ 输出的 lib） |
| resolved_decisions | cmake target 命名约定 |
| watch_items | hotupdate exe 需要额外的 link 依赖 |

### Phase 5 — Wiki 更新

| 字段 | 内容 |
|------|------|
| goal | 更新架构文档和测试流程文档 |
| exit_criteria | wiki 覆盖新架构完整流程 |
| deliverables | 更新后的 wiki 文档 |
| dependencies | Phase 1-4（需要架构稳定后写文档） |
| resolved_decisions | — |
| watch_items | — |

## 6. 子任务映射

| task_id | phase | status | owner | purpose | depends_on | batch_id | requirements | deliverables | exit_criteria | conflict_scope | estimated_effort |
|---------|-------|--------|-------|---------|------------|----------|-------------|-------------|--------------|---------------|-----------------|
| gen-scripts | P1 | planned | main | 改造 4 个 generator 脚本输出新目录结构 | — | — | test_code_generator.py + execution_project_generator.py + execution_solution_generator.py + native_codegen_generator.py 全部适配 | 修改后的脚本 | generator 跑完新结构正确，build 0 error | build/toolchains/run/testing/foundation_dll/ | 3h |
| migrate-files | P2 | planned | main | 迁移文件到新位置，删除旧目录 | gen-scripts | — | 迁移所有 .cs 文件，更新 .gitignore | 新文件布局 | git status clean，build 通过 | verification/foundation-dll/ | 2h |
| il2cpp-link | P3 | planned | main | managed DLL → il2cpp → native-aot.generated.cpp | migrate-files | — | 集成完整 il2cpp 链路 | il2cpp_dist/ 输出 | 编译通过 | src/managed/ + verification/ | 4h |
| native-cmake | P4 | planned | main | solution_native/ cmake 项目 | il2cpp-link | — | 生成 cmake 项目，编译 test/benchmark/hotupdate exe | CMakeLists.txt + exe | cmake --build 成功产生 exe | verification/ | 3h |
| wiki-update | P5 | planned | main | 更新架构和测试流程文档 | native-cmake | — | 更新 wiki 中的相关文档 | wiki 文档更新 | 文档覆盖新架构 | wiki/ | 1h |

## 7. 依赖

```
gen-scripts → migrate-files → il2cpp-link → native-cmake → wiki-update
```

全串行依赖链，每个阶段必须等前一个完成。

## 8. 风险

| 风险 | 概率 | 影响 | 缓解措施 |
|------|------|------|---------|
| Generator 输出的 csproj 路径引用不对 | 中 | 高 | 先跑一个 family 验证引用路径正确性 |
| il2cpp 完整链路集成复杂，pipeline 需要改造 | 高 | 高 | 先做最小 MVP（一个 family），再推广 |
| 文件迁移导致 git 历史断裂 | 低 | 中 | 用 git mv 保留历史 |
| solution_native/ 需要跨平台 cmake 支持 | 中 | 中 | 先支持 Windows/msbuild，后续扩展 |

## 9. 备选路径

- 如果 Phase 3（il2cpp 链路）过于复杂，可降级为先保留现有 RuntimeSkeleton stub，il2cpp 链路放在下一轮
- 如果 Phase 4（native cmake）出现跨平台问题，可先只支持 Windows

## 10. 当前建议推进顺序

串行推进：gen-scripts → migrate-files → il2cpp-link → native-cmake → wiki-update

## 11. 子任务执行策略

- `child_execution_mode`: auto
- `auto_continue`: true
- `auto_stop_policy: blocking-only`
- `dispatch_model`: sequential
- `recommended_next_child`: gen-scripts
