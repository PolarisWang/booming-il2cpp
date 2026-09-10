---
task_id: clib-iw-e1
lifecycle_status: completed
phase: E
parent_task_id: commonlib-industrialization
source_relation: roadmap-child
---

# clib-iw-e1 — Windows CI Pipeline Integration

## 完成证据

- CI config modified: `.github/workflows/codegen-regression.yml`
  - Added `src/native/common/**` and `testing/src/native/**` to trigger paths
  - Added "Run Common Native Tests" step (ctest -R "test_common")
  - Renamed workflow to "Codegen & Common Regression"
- Local verification: all 30 common tests pass
- YAML structure valid
- arch_review: ok — minimal change, no duplication, no abstraction issues
