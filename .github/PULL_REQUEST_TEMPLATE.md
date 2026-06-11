# Pull Request

## 描述

<!-- 请简要描述此 PR 的内容和动机 -->

## 四层架构检查清单

<!-- 在提交前完成所有适用项的检查 -->

- [ ] **层归属确认**: 此 PR 的修改集中在 `Codegen` / `TPG` / `ATG` / `Python` 中的**单个层**
- [ ] **Python 不生成 C++**: Python 层没有新的 `write_text` 调用写入 `.cpp`/`.h` 文件
- [ ] **如果有跨层写入**:
  - [ ] 已添加 `# BOUNDARY_OVERRIDE: <ticket-link>` 标注
  - [ ] 已添加 `# Reason:` 和 `# Expires:` 
  - [ ] 在 PR 描述中说明了为什么不能通过正确层解决
- [ ] **Codegen 产出自包含**: `.generated.cpp`/`.generated.h` 不依赖 Python 后处理
- [ ] **Scriban 模板**修改已在 `.scriban` 文件中完成，不是 Python f-string

## 测试验证

- [ ] 本地验证通过:
  - [ ] 相关 chunk 的 `build` + `fact` 通过
- [ ] 如果有缓存影响：
  - [ ] 已确认 Hephaestus cache key 会因 codegen/TPG 变更自动失效
- [ ] 如果是修复 bug:
  - [ ] 根因记录: `root_cause` / `fix_strategy` / `regression_check`

## 相关 Issue

<!-- 关联的 Issue 链接 -->
