---
task_id: 20260417-01-test-process-and-skill-governance
title: Test Process And Skill Governance
task_type: roadmap
lifecycle_status: completed
phase: completed
created_at: 2026-04-17 00:00:00 +08:00
updated_at: 2026-04-17 06:47:45 +08:00
latest_stop_point: roadmap completed; unified verification mainline now closes on Sdk + Runtime + collection + manifest, and Phase 7 terminology cleanup is finished
current_dir: docs/dev/completed/20260417-01-test-process-and-skill-governance
parent_task_id:
source_task_id:
source_relation:
roadmap_or_plan: docs/dev/completed/20260417-01-test-process-and-skill-governance/roadmap-v1-01.md
active: false
---

## 鍏抽敭鏂囨。

- `docs/dev/completed/20260417-01-test-process-and-skill-governance/brainstorm-v1-01.md`
- `docs/dev/completed/20260417-01-test-process-and-skill-governance/design-v1-01.md`
- `docs/dev/completed/20260417-01-test-process-and-skill-governance/roadmap-v1-01.md`
- `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/subject-test-framework-v1/INDEX.md`
- `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/verification-v1/spec.md`

## 瀹屾垚缁撹

- 宸插喕缁撶粺涓€娴嬭瘯涓荤嚎锛歚managed solution -> dotnet 8 collection analysis -> collection files -> managed test project -> native project -> native test project -> hotupdate patch project + hotupdate test host project`
- 宸茬‘璁ら噰鐢ㄦ柟妗?A锛屽苟灏?`Chaos.TestFramework.Annotation` 鏀瑰悕涓?`Chaos.TestFramework.Sdk`
- 宸茬‘璁?`Assert` 灞炰簬 `Sdk`锛宍Runtime` 灞炰簬鎵ц瀹夸富闈?
- 宸茬‘璁?native / hotupdate 涓撴湁缁戝畾缁х画閫氳繃 manifest 鍒嗗眰锛屼笉杩涘叆 collection file

## 鏈疆鏀跺彛

- 宸插畬鎴?`workspace_declared_catalog.py -> workspace_declared_collection.py` 閲嶅懡鍚嶏紝骞跺悓姝?`benchmark.py`銆乣benchmark_dashboard_generator.py`銆乣subject_workers.py`銆乣test_subject_workers.py` 涓庡搴斿崟娴嬫枃浠跺懡鍚嶃€?
- 宸叉竻鎺夊綋鍓嶄唬鐮佷笌娴嬭瘯閲屾畫鐣欑殑 `workspace catalog` / `declared catalog` 鐢ㄦ埛鍙鏂囨锛岀粺涓€鍒?`collection` 鍙ｅ緞銆?
- HotUpdate proof / benchmark host 宸茬ǔ瀹氳蛋 `collection + binding manifest` 鐪熼摼璺紝`PerfHarness` 涓嶅啀鍥為€€鍒版棫鍙嶅皠鎵弿璺緞銆?
- 鐜拌 architecture / wiki / skill 鍧囧凡瀵归綈 `Sdk + Runtime + collection + manifest` 涓荤嚎锛屾湰浠诲姟鏃犻澶?wiki 寰呰ˉ銆?

## 宸查獙璇?

- `python -m pytest tests/unit/planning/test_project_workspace.py tests/unit/planning/test_generated_managed_hosts.py tests/unit/planning/test_generated_hotupdate_hosts.py tests/unit/execution/test_subject_workers.py tests/unit/performance/test_workspace_declared_collection.py tests/unit/performance/test_benchmark_command.py tests/unit/performance/test_benchmark_dashboard_generator.py tests/unit/performance/test_subject_workers_perf.py tests/tooling/run/test_subject_command.py tests/integration/registry/test_declared_metadata_discovery.py tests/integration/run/test_perf_harness_hotupdate_benchmark_host.py tests/unit/compatibility/test_phase5_hotupdate_host_pack_declared_proof.py -q`
  - `134 passed`
- `python -u build/toolchains/run/run.py test declared-benchmark --id "declared-benchmark/HotUpdateHostPack::HotUpdateHostPack::HotUpdateHostPack.HotUpdateLoadBenchmarkEntry::RunWorkload()" --json`
  - `final-summary.status = ok`
- `python -u build/toolchains/run/run.py test declared-unit-test --id 'declared-unit-test/HotUpdateHostPack::HotUpdateHostPack::HotUpdateHostPack.HotUpdateSkeletonProofEntry::Run()' --json`
  - `final-summary.status = ok`

## 鍓╀綑椋庨櫓 / 浠诲姟澶栭樆濉?

- `SolutionCorePack` 浠嶆棤娉曞畬鎴愬叏閲?workspace regenerate锛涢樆濉炵偣涓嶅湪娴嬭瘯妗嗘灦锛岃€屽湪 native proof emission锛?
  - `unsupported managed lowering plan kind 'generic-analysis-only'`
  - 璋冪敤鏍堣惤鍦?`Chaos.IL2CPP.CodeGen.NativeReferenceProofEmitter.ValidateManagedLoweringPlan(...)`
- 褰撳墠娴嬭瘯娌荤悊 roadmap 宸叉棤鍓╀綑闃诲锛涗笂闈㈢殑 `SolutionCorePack` 闂缁х画鐣欏湪鐙珛 AOT roadmap 涓鐞嗐€?

## 涓嬩竴姝?

- 鏈换鍔″凡褰掓。瀹屾垚銆?
- 鑻ュ悗缁缁х画鎺ㄨ繘 `SolutionCorePack` native proof emission锛岃杞洖 `20260416-01-aot-core-ir-gap-audit-and-next-expansion-roadmap` 涓荤嚎澶勭悊銆?
