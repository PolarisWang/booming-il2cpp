---
task_id: 20260419-08-scriban-generator-formalization
title: Scriban Generator Formalization
task_type: plan
lifecycle_status: hanging
phase: hanging
created_at: 2026-04-19 22:45:00 +08:00
updated_at: 2026-04-21 23:45:00 +08:00
latest_stop_point: 鐢ㄦ埛瑕佹眰鏆傚仠 20260419 涓荤嚎锛沗Chaos.IL2CPP.Generator` cutover 淇濇寔鍦ㄥ凡瀹屾垚鏂规纭銆佸皻鏈紑濮嬫寮?TDD/cutover 鐨勭姸鎬?current_dir: docs/dev/hanging/20260419-08-scriban-generator-formalization
parent_task_id: 20260417-02-il2cpp-core-structure-and-scriban-governance-roadmap
source_relation: naming-and-cutover-followup
brainstorm_doc: docs/dev/in-progress/20260419-08-scriban-generator-formalization/brainstorm-v1-01.md
design_doc: docs/dev/in-progress/20260419-08-scriban-generator-formalization/design-v1-01.md
plan_doc: docs/dev/in-progress/20260419-08-scriban-generator-formalization/plan-v1-01.md
active: false
---

## 2026-04-21 鎸傝捣

- 鐢ㄦ埛鏄庣‘瑕佹眰鍏堟殏鍋?`20260419-*` 鐩稿叧浠诲姟銆?- 褰撳墠淇濈暀鐨勭粨璁轰笉鍙橈細
  - canonical 鍚嶇О浠嶄负 `Chaos.IL2CPP.Generator`
  - 鏂规浠嶄负鍗曟鍘熷湴 cutover锛屼笉淇濈暀闀挎湡 alias 鎴栧弻杞ㄥ伐绋?- 鏈鎸傝捣涓嶆墽琛屾祴璇曞垏鎹紝涔熶笉鏀?source / docs / wiki銆?
## 涓嬩竴姝?
- 绛夊緟鐢ㄦ埛鎭㈠鏈换鍔°€?- 鎭㈠鍚庢寜鏃㈠畾 `plan-v1-01.md`锛屽厛琛?RED 娴嬭瘯锛屽啀鎵ц鍘熷湴閲嶅懡鍚嶅拰 authority/wiki 鏀跺彛銆?
## Purpose

鎶婂綋鍓嶅凡缁忔帴鍏ュ苟瀹為檯浣跨敤鐨?vendored `Scriban` 姝ｅ紡绾冲叆 IL2CPP 鏍稿績鐢熸垚灞傛不鐞嗭紝骞跺皢鐜版湁 `Chaos.IL2CPP.CodeGen` 鍘熷湴閲嶅懡鍚嶄负 `Chaos.IL2CPP.Generator`銆?
## Current Conclusion

- `Scriban` 涓嶆槸鈥滄湭鏉ュ啀鎺ュ叆鈥濈殑鐘舵€侊紝鑰屾槸宸茬粡閫氳繃 `src/managed/Chaos.IL2CPP.CodeGen/Chaos.IL2CPP.CodeGen.csproj` 鐩存帴寮曠敤 `third_party/scriban/src/Scriban/Scriban.csproj`銆?- 褰撳墠浠撳簱鐨勭湡瀹為棶棰樹笉鏄€滃浣曞紩鍏?Scriban鈥濓紝鑰屾槸鈥滃浣曟妸宸茬粡瀛樺湪鐨勪复鏃?灞€閮ㄦ帴鍏ユ寮忔敹鍙ｄ负鏍稿績灞傛爣鍑嗙粍浠垛€濄€?- 鏈换鍔￠噰鐢ㄥ崟娆?cutover锛氫笉鏂板缓骞惰鐨?`Generator` 澹冲伐绋嬶紝鑰屾槸鎶婄幇鏈?`Chaos.IL2CPP.CodeGen` 鍘熷湴 cutover 涓?`Chaos.IL2CPP.Generator`銆?- canonical 鍚嶇О缁熶竴閲囩敤 `Chaos.IL2CPP.Generator`锛屼笉寮曞叆 `CHaos.*` 鍒嗘敮銆?
## Governance Intake

- capabilityFamily: n/a
- capabilityItem: scriban-generator-formalization
- ownerSubjectId: n/a
- proofRequired: false
- benchmarkRequired: false
- hotupdateImpact: None
- formalVerificationObjects: []
- requiredGates: collector -> registry -> workspace

## Key Findings

- 婧愮爜灞備緷璧栫偣锛?  - `Chaos.IL2CPP.Driver`
  - `Chaos.IL2CPP.Pipeline`
  - `DriverEntry.cs` 鐨?`using Chaos.IL2CPP.CodeGen;`
- 娴嬭瘯灞傜‖缂栫爜鐐癸細
  - `tests/unit/run/test_repo_layout.py`
  - `tests/unit/planning/test_project_workspace.py`
  - `tests/support.py`
  - 澶ч噺 `tests/unit/compatibility/**` 鐩存帴寮曠敤 `src/managed/Chaos.IL2CPP.CodeGen/**`
  - `docs/architecture/subject-test-framework-v1/INDEX.md`
  - `docs/architecture/verification-v1/spec.md`
  - `docs/architecture/runtime-baseline/repo-layout.md`
  - `docs/architecture/runtime-baseline/ownership-map.md`
  - `wiki/04-宸ュ叿涓庨泦鎴?il2cpp-core-structure-and-scriban-governance.md`

## Next Step

- 鎸?`plan-v1-01.md` 鎵ц鍗曟 cutover锛?  - 鍏堣ˉ/淇敼娴嬭瘯锛岄攣瀹?`Generator` 涓?canonical 鍚嶇О涓?Python 鐢熸垚鍏ュ彛
  - 鍘熷湴閲嶅懡鍚?`Chaos.IL2CPP.CodeGen` -> `Chaos.IL2CPP.Generator`
  - 淇濇寔 vendored `Scriban` 鐨?`ProjectReference`
  - 鍚屾鏇存柊 source/tests/authority docs/wiki
  - 涓嶄繚鐣欓暱鏈?alias 鎴栧弻杞ㄥ伐绋?
