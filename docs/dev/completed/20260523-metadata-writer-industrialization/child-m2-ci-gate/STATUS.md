# M2 — CI 门禁 + 覆盖率门禁

> **task_id**: m2-ci-gate
> **parent_task_id**: 20260523-metadata-writer-industrialization
> **source_task_id**: 20260523-metadata-writer-industrialization
> **source_relation**: roadmap-child
> **clearance_source**: parent-roadmap
> **phase**: completed
> **entry_skill**: dev-executing-plans

blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true

## Scope

CI 门禁 + 覆盖率门禁：将 MetadataWriter 测试纳入 CI workflow，配置 coverlet 基线。

## Changes

### CI workflow (codegen-regression.yml)
- Added `testing/src/managed/**` to PR/push trigger paths
- Added `dotnet restore testing/src/managed/Chaos.IL2CPP.Tests.MetadataWriter/`
- Added "Build MetadataWriter Tests" step
- Added "Run MetadataWriter Tests" step
- Added "MetadataWriter Coverage Collection" step (coverlet with cobertura format)
- Added "Upload MetadataWriter Coverage Report" step

### coverlet.runsettings
- Created `testing/src/managed/Chaos.IL2CPP.Tests.MetadataWriter/coverlet.runsettings`
- Line coverage threshold: 70%
- Excludes auto-generated `obj/` files

### coverlet.msbuild reference
- Added `coverlet.msbuild` v6.0.0 package reference to the test csproj

## Verification

- `dotnet restore + build` — 0 errors, 0 warnings
- `dotnet test` — 24/24 passed
- Coverlet coverage collection — cobertura output generated successfully

## Exit Criteria

- [x] MetadataWriter 测试在 CI workflow 中自动触发 (codegen-regression.yml)
- [x] coverlet 基线已建立
- [x] PR 修改 MetadataWriterStage.cs 触发 CI（已有 path trigger）
- [x] 覆盖率阈值配置生效
