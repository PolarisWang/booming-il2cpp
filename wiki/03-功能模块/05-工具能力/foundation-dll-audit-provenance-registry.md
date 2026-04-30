# Foundation DLL Audit 数据来源注册表

> 由 `docs/dev/scripts/generate-provenance-registry.py` 自动生成 baseline。
> 人工审查确认后提交 commit。后续改 dashboard 代码后须 diff 检查。

## 1. 函数清单

### generator

| 函数 | 行号 | 参数 | 调用函数 | 返回的 dict keys |
|------|------|------|---------|-----------------|
| _string | L81 | value | str |  |
| _list | L85 | value | isinstance, isinstance, list, list |  |
| _relative | L95 | repo_root, path |  |  |
| _normalized | L102 | text | str |  |
| _safe_display_text | L106 | value, fallback | _string, sum, any, ord |  |
| _family_slug | L119 | family_id | len, str, str |  |
| _root_relative_prefix | L126 | repo_root, output_path | Path, len, _relative |  |
| _manifest_path | L132 | repo_root |  |  |
| has_foundation_dll_audit_manifest | L136 | repo_root | _manifest_path |  |
| _load_program_manifest | L140 | repo_root | _manifest_path, _normalized, _normalized, _normalized, _normalized... |  |
| _load_capability_ledger | L163 | repo_root |  |  |
| _root_relative_link | L170 | root_prefix, path_text | _normalized |  |
| _build_ledger_lookup | L177 | ledger | list |  |
| _parse_markdown_table_rows | L184 | roadmap_text | len |  |
| _task_status_path | L205 | repo_root, task_id |  |  |
| _extract_path_references | L218 | repo_root, markdown_path | set, _normalized |  |
| _classify_artifact_kind | L234 | path_text | _normalized |  |
| _artifact_role | L249 | project_code, path_text | _normalized |  |
| _artifact_record | L264 | repo_root, assembly_name, project_code, path_text | _classify_artifact_kind, Path, _normalized, _artifact_role | artifactKind, assemblyName, displayName, exists, linkTargetType, path, projectCode, required, role |
| _project_artifacts | L282 | repo_root | list, any, any, _artifact_record, _normalized... |  |
| _dll_state | L317 | assembly_entry | _string, dict, _string, _string, _string... |  |
| _compute_capability_closure | L333 | families | len, sum, sum, sum, sum... | closedFamilies, closurePercent, excludedFamilies, inProgressFamilies, platformBlockedFamilies, totalFamilies, waivedFamilies |
| _compute_gate_progress | L352 | families | dict, round, _string | passedGates, progressPercent, totalRequiredGates |
| _compute_waiver_summary | L376 | families | list, _string | activeWaivers, expiredWaivers, totalExclusions, totalPlatformBlocked, totalWaivers |
| _auto_derive_family | L414 | assembly_name, projects, project_templates | _string, _string, _string | closureStatus, denominatorStatus, description, displayName, familyId, implementationFamilies, methodCount, sourceGroups, verificationGates |
| _validate_ledger_dll | L444 | entry | list, set, _string, _string, _string... |  |
| _derive_dll_denominator_status | L471 | ledger_entry, families, family_source | _string, _string, _string |  |
| _render_progress_bar | L483 | label, numerator, denominator, pct | min, escape, escape |  |
| _render_mini_bar | L493 | pct, numerator, denominator | min, escape |  |
| _collect_support_refs | L506 | repo_root | list, _string, _relative, _normalized |  |
| _evidence_task_ids | L521 | assembly_entry | _string, _string, _list, _string, _string... |  |
| _display_phase | L531 | program_manifest, assembly_entry | _string, _string, _string |  |
| _display_roadmap_task_id | L539 | program_manifest, assembly_entry | _string, _string, _string |  |
| _ordered_assembly_entries | L547 | program_manifest | sorted, enumerate, _string, dict, list... |  |
| _build_projects | L576 | repo_root | list, _string, _evidence_task_ids, list, _collect_support_refs... |  |
| _roadmap_rows | L706 | repo_root, roadmap_path_text | _parse_markdown_table_rows, Path |  |
| _active_task_ids | L713 | repo_root | set, set, set |  |
| build_foundation_dll_audit_payload | L724 | repo_root | _load_program_manifest, _roadmap_rows, _active_task_ids, _load_capability_ledger, _build_ledger_lookup... | artifactIndex, dllMatrix, dlls, familyVerification, familyVerificationClaims, program, truthContracts |
| _dashboard_link | L1046 | path_text | _normalized, escape, escape |  |
| _local_link | L1053 | path_text, label | _normalized, escape, escape |  |
| _status_class | L1058 | value | _normalized, str |  |
| _status_badge | L1063 | value | _string, _status_class, escape |  |
| _dll_detail_relative_path | L1068 | assembly_name | _string |  |
| _dll_json_relative_path | L1072 | assembly_name | _string |  |
| _dashboard_styles | L1076 |  |  |  |
| _project_evidence_count | L1505 | project | len, list |  |
| _project_support_count | L1509 | project | len, list |  |
| _dll_evidence_count | L1513 | dll | sum, _project_evidence_count, list |  |
| _dll_support_count | L1517 | dll | sum, _project_support_count, list |  |
| _render_project_card | L1521 | project | _string, escape, list, list, escape... |  |
| _render_source_links_block | L1560 | sl | _string, _normalized, len, len, escape... |  |
| _gate_header_tooltip | L1600 | gate_code |  |  |
| _render_benchmark_speedup_cell | L1644 | benchmark_proof | _family_slug, int, int, _render_generic_gate_progress_cell, escape... |  |
| _render_benchmark_detail_section | L1679 | family | _string, _family_slug, _string, dict, list... |  |
| _render_benchmark_comparison_section | L1724 | gate_proof | int, int, list, escape, escape... |  |
| _family_has_active_gates | L1742 | gates | _string |  |
| _render_generic_gate_progress_cell | L1753 | family | _string, int, int, _render_mini_bar, list... |  |
| _render_family_table | L1804 | families | enumerate, _string, _string, _family_slug, _string... |  |
| _render_waiver_table | L1868 | families | list, escape, _status_badge, escape, escape... |  |
| _render_native_proof_progress_cell | L1890 | family | _render_mini_bar, list, list, list, list... |  |
| _short_method_subject_id | L1953 | subject_id | _string |  |
| _native_proof_tooltip_script | L1967 |  |  |  |
| _render_dll_detail_page | L2011 | dll | list, _string, list, dict, dict... |  |
| _render_dashboard | L2109 | payload | dict, dict, list, list, any... |  |
| _summary_markdown | L2291 | payload | dict, dict, list, _dll_detail_relative_path, str |  |
| _render_artifact_index_page | L2316 | payload | list, escape, dict, _dashboard_link, escape... |  |
| _write_projection_bundle | L2358 | repo_root, payload | write_json, write_json, write_json, write_json, write_json... |  |
| write_foundation_dll_audit_outputs | L2407 | repo_root | build_foundation_dll_audit_payload, bool, _write_projection_bundle, list, _summary_markdown... | artifacts, dashboardPath, docsDashboardPath, docsOutputRoot, outputRoot, program, reportSummaryPath |

### kernel

| 函数 | 行号 | 参数 | 调用函数 | 返回的 dict keys |
|------|------|------|---------|-----------------|
| _string | L9 | value | str |  |
| to_dict | L55 | self | asdict |  |
| _try_read_json | L59 | path | isinstance |  |
| _coverage_json_for_artifact_path | L69 | repo_root, artifact_path | _try_read_json |  |
| _extract_run_id | L75 | path | _string, len, str |  |
| build_native_proof_facts | L83 | projects | list, _string, _string, VerificationFact, _extract_run_id... |  |
| evaluate_native_proof | L106 | repo_root, family, claim | VerificationClaim, build_native_proof_facts, VerificationEvaluation, _string, round... |  |
| evaluate_generic_gate | L204 | family | list, VerificationEvaluation, _string, list, len... |  |
| evaluate_test_code | L250 | family | dict, int, _string, VerificationEvaluation, _string... |  |
| build_family_verification_snapshot | L279 | repo_root | dict, _string, to_dict, to_dict, to_dict... | assemblyName, families, ownerSubjectId, schemaVersion |

### claims

| 函数 | 行号 | 参数 | 调用函数 | 返回的 dict keys |
|------|------|------|---------|-----------------|
| _string | L9 | value | str |  |
| _normalized | L13 | text | str |  |
| _extract_run_id | L17 | path | _string |  |
| _try_read_json | L24 | path | isinstance |  |
| _native_reference_plan_candidates | L34 | repo_root, artifact_path | _normalized |  |
| _load_latest_method_universe | L47 | repo_root, projects | sorted, list, _string, _string, _extract_run_id... |  |
| _family_slug | L73 | family_id | len, _string, _string |  |
| _load_truth_method_subject_ids | L80 | repo_root | _try_read_json, dict, _string, _family_slug, list... |  |
| _append_gate_claim | L104 | claims | _string, _string, _string, dict |  |
| build_family_verification_claims_snapshot | L132 | repo_root | _load_latest_method_universe, _string, _load_truth_method_subject_ids, _append_gate_claim, _append_gate_claim... | assemblyName, claims, methodUniverseArtifactPaths, ownerSubjectId, schemaVersion |

### gap

| 函数 | 行号 | 参数 | 调用函数 | 返回的 dict keys |
|------|------|------|---------|-----------------|
| _read_json | L35 | path |  |  |
| _write_json | L39 | path, payload |  |  |
| _family_slug | L44 | family_id | len, str, str |  |
| _family_test_dir | L51 | repo_root, assembly_name, family_id | _family_slug |  |
| _derive_test_code_status | L55 | repo_root, assembly_name, family | str, int, _family_test_dir, dict, str | action, allMethodsCovered, emittedMethodCount, requestedMethodCount, testCodeStatus, testProjectPath |
| _matches_scope | L86 | assembly_name, family_id, scope |  |  |
| _existing_ledger_paths | L92 | repo_root |  |  |
| _family_truth_root | L100 | repo_root, assembly_name, family_id | _family_slug |  |
| _resolve_method_subject_ids | L104 | repo_root, assembly_name, family | _read_json, list, str, _family_truth_root, _read_json... |  |
| _discover_native_proof_projects | L133 | repo_root |  |  |
| _select_primary_ledger_path | L148 | repo_root |  |  |
| _analyze_payload | L155 | repo_root, payload | list, str, list, str, _derive_test_code_status... |  |
| analyze_gaps | L310 | repo_root | _select_primary_ledger_path, _read_json, _analyze_payload, _existing_ledger_paths, _read_json... | dllCount, families, familyCount, generatedArtifacts, ledgerPath, scope, statusCounts |

## 2. 字段来源映射表

| 字段 | 出现工件 | 定义函数 | 数据来源链路 | 枚举值 |
|------|---------|---------|------------|--------|
| `authoritySnapshot` | program.json | (直接赋值) | (待补充) | (待补充) |
| `authoritySnapshot.denominatorStatus` | program.json | generator/_auto_derive_family [L414] | (待补充) | (待补充) |
| `authoritySnapshot.denominatorStrategy` | program.json | (直接赋值) | (待补充) | (待补充) |
| `authoritySnapshot.ledgerVersion` | program.json | (直接赋值) | (待补充) | (待补充) |
| `authoritySnapshot.originalAuthorityDigest` | program.json | (直接赋值) | (待补充) | (待补充) |
| `authoritySnapshot.originalAuthorityDigest.inputManifestVersion` | program.json | (直接赋值) | (待补充) | (待补充) |
| `authoritySnapshot.originalAuthorityDigest.nativeizationPlanVersion` | program.json | (直接赋值) | (待补充) | (待补充) |
| `authoritySnapshot.originalAuthorityDigest.semanticLedgerVersion` | program.json | (直接赋值) | (待补充) | (待补充) |
| `authoritySnapshot.originalAuthorityDigest.surfaceLedgerVersion` | program.json | (直接赋值) | (待补充) | (待补充) |
| `authoritySnapshot.snapshotAt` | program.json | (直接赋值) | (待补充) | (待补充) |
| `authoritySnapshot.snapshotId` | program.json | (直接赋值) | (待补充) | (待补充) |
| `designPath` | program.json | (直接赋值) | (待补充) | (待补充) |
| `programId` | program.json | (直接赋值) | (待补充) | (待补充) |
| `roadmapPath` | program.json | (直接赋值) | (待补充) | (待补充) |
| `schemaVersion` | program.json | kernel/build_family_verification_snapshot [L279]; claims/build_family_verification_claims_snapshot [L132] | (待补充) | (待补充) |
| `scopeAssemblies` | program.json | (直接赋值) | (待补充) | (待补充) |
| `statusPath` | program.json | (直接赋值) | (待补充) | (待补充) |
| `subjectEntry` | program.json | (直接赋值) | (待补充) | (待补充) |
| `summary` | program.json | (直接赋值) | (待补充) | (待补充) |
| `summary.activeAssembly` | program.json | (直接赋值) | (待补充) | (待补充) |
| `summary.blockedCount` | program.json | (直接赋值) | (待补充) | (待补充) |
| `summary.capabilityClosure` | program.json | (直接赋值) | (待补充) | (待补充) |
| `summary.capabilityClosure.closedFamilies` | program.json | generator/_compute_capability_closure [L333] | (待补充) | (待补充) |
| `summary.capabilityClosure.closurePercent` | program.json | generator/_compute_capability_closure [L333] | (待补充) | (待补充) |
| `summary.capabilityClosure.excludedFamilies` | program.json | generator/_compute_capability_closure [L333] | (待补充) | (待补充) |
| `summary.capabilityClosure.inProgressFamilies` | program.json | generator/_compute_capability_closure [L333] | (待补充) | (待补充) |
| `summary.capabilityClosure.platformBlockedFamilies` | program.json | generator/_compute_capability_closure [L333] | (待补充) | (待补充) |
| `summary.capabilityClosure.totalFamilies` | program.json | generator/_compute_capability_closure [L333] | (待补充) | (待补充) |
| `summary.capabilityClosure.waivedFamilies` | program.json | generator/_compute_capability_closure [L333] | (待补充) | (待补充) |
| `summary.completedCount` | program.json | (直接赋值) | (待补充) | (待补充) |
| `summary.dllCompletion` | program.json | (直接赋值) | (待补充) | (待补充) |
| `summary.dllCompletion.blockedDllCount` | program.json | (直接赋值) | (待补充) | (待补充) |
| `summary.dllCompletion.completedDllCount` | program.json | (直接赋值) | (待补充) | (待补充) |
| `summary.dllCompletion.inProgressDllCount` | program.json | (直接赋值) | (待补充) | (待补充) |
| `summary.dllCompletion.notStartedDllCount` | program.json | (直接赋值) | (待补充) | (待补充) |
| `summary.dllCount` | program.json | gap/analyze_gaps [L310] | (待补充) | (待补充) |
| `summary.inProgressCount` | program.json | (直接赋值) | (待补充) | (待补充) |
| `summary.notStartedCount` | program.json | (直接赋值) | (待补充) | (待补充) |
| `summary.progressPercent` | program.json | generator/_compute_gate_progress [L352] | (待补充) | (待补充) |
| `summary.workflowProgress` | program.json | (直接赋值) | (待补充) | (待补充) |
| `summary.workflowProgress.passedGates` | program.json | generator/_compute_gate_progress [L352] | (待补充) | (待补充) |
| `summary.workflowProgress.progressPercent` | program.json | generator/_compute_gate_progress [L352] | (待补充) | (待补充) |
| `summary.workflowProgress.totalRequiredGates` | program.json | generator/_compute_gate_progress [L352] | (待补充) | (待补充) |
| `title` | program.json | (直接赋值) | (待补充) | (待补充) |
| `rows` | dll-matrix.json | (直接赋值) | (待补充) | (待补充) |
| `rows[].assemblyName` | dll-matrix.json | generator/_artifact_record [L264]; kernel/build_family_verification_snapshot [L279]; claims/build_family_verification_claims_snapshot [L132] | (待补充) | (待补充) |
| `rows[].audit-input-and-ledger` | dll-matrix.json | (直接赋值) | (待补充) | (待补充) |
| `rows[].benchmark` | dll-matrix.json | (直接赋值) | (待补充) | (待补充) |
| `rows[].capabilityClosure` | dll-matrix.json | (直接赋值) | (待补充) | (待补充) |
| `rows[].capabilityClosure.closedFamilies` | dll-matrix.json | generator/_compute_capability_closure [L333] | (待补充) | (待补充) |
| `rows[].capabilityClosure.closurePercent` | dll-matrix.json | generator/_compute_capability_closure [L333] | (待补充) | (待补充) |
| `rows[].capabilityClosure.excludedFamilies` | dll-matrix.json | generator/_compute_capability_closure [L333] | (待补充) | (待补充) |
| `rows[].capabilityClosure.inProgressFamilies` | dll-matrix.json | generator/_compute_capability_closure [L333] | (待补充) | (待补充) |
| `rows[].capabilityClosure.platformBlockedFamilies` | dll-matrix.json | generator/_compute_capability_closure [L333] | (待补充) | (待补充) |
| `rows[].capabilityClosure.totalFamilies` | dll-matrix.json | generator/_compute_capability_closure [L333] | (待补充) | (待补充) |
| `rows[].capabilityClosure.waivedFamilies` | dll-matrix.json | generator/_compute_capability_closure [L333] | (待补充) | (待补充) |
| `rows[].codegen-review` | dll-matrix.json | (直接赋值) | (待补充) | (待补充) |
| `rows[].completion-certification` | dll-matrix.json | (直接赋值) | (待补充) | (待补充) |
| `rows[].currentProject` | dll-matrix.json | (直接赋值) | (待补充) | (待补充) |
| `rows[].denominatorStatus` | dll-matrix.json | generator/_auto_derive_family [L414] | (待补充) | (待补充) |
| `rows[].dllState` | dll-matrix.json | (直接赋值) | (待补充) | (待补充) |
| `rows[].hotupdate-proof` | dll-matrix.json | (直接赋值) | (待补充) | (待补充) |
| `rows[].managed-proof` | dll-matrix.json | (直接赋值) | (待补充) | (待补充) |
| `rows[].native-proof` | dll-matrix.json | (直接赋值) | (待补充) | (待补充) |
| `rows[].orderIndex` | dll-matrix.json | (直接赋值) | (待补充) | (待补充) |
| `rows[].riskTags` | dll-matrix.json | (直接赋值) | (待补充) | (待补充) |
| `rows[].workflowProgress` | dll-matrix.json | (直接赋值) | (待补充) | (待补充) |
| `rows[].workflowProgress.passedGates` | dll-matrix.json | generator/_compute_gate_progress [L352] | (待补充) | (待补充) |
| `rows[].workflowProgress.progressPercent` | dll-matrix.json | generator/_compute_gate_progress [L352] | (待补充) | (待补充) |
| `rows[].workflowProgress.totalRequiredGates` | dll-matrix.json | generator/_compute_gate_progress [L352] | (待补充) | (待补充) |
| `assemblies` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].assemblyName` | family-verification.json | generator/_artifact_record [L264]; kernel/build_family_verification_snapshot [L279]; claims/build_family_verification_claims_snapshot [L132] | (待补充) | (待补充) |
| `assemblies[].families` | family-verification.json | kernel/build_family_verification_snapshot [L279]; gap/analyze_gaps [L310] | (待补充) | (待补充) |
| `assemblies[].families[].auditInputProof` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].auditInputProof.caseItems` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].auditInputProof.caseSectionLabel` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].auditInputProof.denominator` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].auditInputProof.evidence` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].auditInputProof.numerator` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].auditInputProof.progressPercent` | family-verification.json | generator/_compute_gate_progress [L352] | (待补充) | (待补充) |
| `assemblies[].families[].auditInputProof.status` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].authorityRefs` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].benchmarkProof` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].benchmarkProof.averageSpeedupPercent` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].benchmarkProof.caseItems` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].benchmarkProof.caseItems[].detail` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].benchmarkProof.caseItems[].memberName` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].benchmarkProof.caseSectionLabel` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].benchmarkProof.comparisonMethodResults` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].benchmarkProof.comparisonMethodResults[].managedElapsedMs` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].benchmarkProof.comparisonMethodResults[].methodSubjectId` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].benchmarkProof.comparisonMethodResults[].nativeElapsedMs` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].benchmarkProof.comparisonMethodResults[].speedupPercent` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].benchmarkProof.comparisonMethodResults[].status` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].benchmarkProof.denominator` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].benchmarkProof.evidence` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].benchmarkProof.managedFasterCount` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].benchmarkProof.methodDetails` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].benchmarkProof.nativeFasterCount` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].benchmarkProof.numerator` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].benchmarkProof.progressPercent` | family-verification.json | generator/_compute_gate_progress [L352] | (待补充) | (待补充) |
| `assemblies[].families[].benchmarkProof.reason` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].benchmarkProof.runs` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].benchmarkProof.status` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].capabilityFamilyEnum` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].closureStatus` | family-verification.json | generator/_auto_derive_family [L414] | (待补充) | (待补充) |
| `assemblies[].families[].codegenReviewProof` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].codegenReviewProof.caseItems` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].codegenReviewProof.caseSectionLabel` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].codegenReviewProof.denominator` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].codegenReviewProof.evidence` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].codegenReviewProof.numerator` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].codegenReviewProof.progressPercent` | family-verification.json | generator/_compute_gate_progress [L352] | (待补充) | (待补充) |
| `assemblies[].families[].codegenReviewProof.status` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].confirmationNote` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].denominatorStatus` | family-verification.json | generator/_auto_derive_family [L414] | (待补充) | (待补充) |
| `assemblies[].families[].derivationBasis` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].description` | family-verification.json | generator/_auto_derive_family [L414] | (待补充) | (待补充) |
| `assemblies[].families[].displayName` | family-verification.json | generator/_artifact_record [L264]; generator/_auto_derive_family [L414] | (待补充) | (待补充) |
| `assemblies[].families[].familyId` | family-verification.json | generator/_auto_derive_family [L414] | (待补充) | (待补充) |
| `assemblies[].families[].hotupdateProof` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].hotupdateProof.caseItems` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].hotupdateProof.caseItems[].detail` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].hotupdateProof.caseItems[].memberName` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].hotupdateProof.caseSectionLabel` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].hotupdateProof.denominator` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].hotupdateProof.evidence` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].hotupdateProof.evidence[].artifactKind` | family-verification.json | generator/_artifact_record [L264] | (待补充) | (待补充) |
| `assemblies[].families[].hotupdateProof.evidence[].label` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].hotupdateProof.evidence[].linkTargetType` | family-verification.json | generator/_artifact_record [L264] | (待补充) | (待补充) |
| `assemblies[].families[].hotupdateProof.evidence[].path` | family-verification.json | generator/_artifact_record [L264] | (待补充) | (待补充) |
| `assemblies[].families[].hotupdateProof.failedMethodCount` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].hotupdateProof.methodDetails` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].hotupdateProof.numerator` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].hotupdateProof.passedMethodCount` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].hotupdateProof.progressPercent` | family-verification.json | generator/_compute_gate_progress [L352] | (待补充) | (待补充) |
| `assemblies[].families[].hotupdateProof.reason` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].hotupdateProof.runs` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].hotupdateProof.runs[].evidence` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].hotupdateProof.runs[].evidence[].artifactKind` | family-verification.json | generator/_artifact_record [L264] | (待补充) | (待补充) |
| `assemblies[].families[].hotupdateProof.runs[].evidence[].label` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].hotupdateProof.runs[].evidence[].linkTargetType` | family-verification.json | generator/_artifact_record [L264] | (待补充) | (待补充) |
| `assemblies[].families[].hotupdateProof.runs[].evidence[].path` | family-verification.json | generator/_artifact_record [L264] | (待补充) | (待补充) |
| `assemblies[].families[].hotupdateProof.runs[].runId` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].hotupdateProof.runs[].status` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].hotupdateProof.status` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].hotupdateProof.unmatchedMethodCount` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].hotupdateProof.verificationMethodResults` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].hotupdateProof.verificationMethodResults[].expectedPatchedValue` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].hotupdateProof.verificationMethodResults[].methodSubjectId` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].hotupdateProof.verificationMethodResults[].methodToken` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].hotupdateProof.verificationMethodResults[].originalReturnValue` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].hotupdateProof.verificationMethodResults[].patchedReturnValue` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].hotupdateProof.verificationMethodResults[].revertVerified` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].hotupdateProof.verificationMethodResults[].status` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].implementationFamilies` | family-verification.json | generator/_auto_derive_family [L414] | (待补充) | (待补充) |
| `assemblies[].families[].implementationFamilies[].displayName` | family-verification.json | generator/_artifact_record [L264]; generator/_auto_derive_family [L414] | (待补充) | (待补充) |
| `assemblies[].families[].implementationFamilies[].familyId` | family-verification.json | generator/_auto_derive_family [L414] | (待补充) | (待补充) |
| `assemblies[].families[].implementationFamilies[].handlerName` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].managedProof` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].managedProof.caseItems` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].managedProof.caseItems[].detail` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].managedProof.caseItems[].memberName` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].managedProof.caseSectionLabel` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].managedProof.denominator` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].managedProof.evidence` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].managedProof.methodDetails` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].managedProof.numerator` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].managedProof.progressPercent` | family-verification.json | generator/_compute_gate_progress [L352] | (待补充) | (待补充) |
| `assemblies[].families[].managedProof.reason` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].managedProof.runs` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].managedProof.status` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].methodCount` | family-verification.json | generator/_auto_derive_family [L414] | (待补充) | (待补充) |
| `assemblies[].families[].nativeProof` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].nativeProof.benchmarkCases` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].nativeProof.benchmarkCases[].annotationDigest` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].nativeProof.benchmarkCases[].caseKind` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].nativeProof.benchmarkCases[].className` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].nativeProof.benchmarkCases[].familyId` | family-verification.json | generator/_auto_derive_family [L414] | (待补充) | (待补充) |
| `assemblies[].families[].nativeProof.benchmarkCases[].memberName` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].nativeProof.benchmarkCases[].methodSubjectId` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].nativeProof.benchmarkCases[].profileCode` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].nativeProof.benchmarkCases[].projectPath` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].nativeProof.benchmarkCases[].routeCode` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].nativeProof.benchmarkCases[].sourceFile` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].nativeProof.denominator` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].nativeProof.evidence` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].nativeProof.evidence[].artifactKind` | family-verification.json | generator/_artifact_record [L264] | (待补充) | (待补充) |
| `assemblies[].families[].nativeProof.evidence[].label` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].nativeProof.evidence[].linkTargetType` | family-verification.json | generator/_artifact_record [L264] | (待补充) | (待补充) |
| `assemblies[].families[].nativeProof.evidence[].path` | family-verification.json | generator/_artifact_record [L264] | (待补充) | (待补充) |
| `assemblies[].families[].nativeProof.hotupdateCases` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].nativeProof.hotupdateCases[].annotationDigest` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].nativeProof.hotupdateCases[].caseKind` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].nativeProof.hotupdateCases[].className` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].nativeProof.hotupdateCases[].direction` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].nativeProof.hotupdateCases[].familyId` | family-verification.json | generator/_auto_derive_family [L414] | (待补充) | (待补充) |
| `assemblies[].families[].nativeProof.hotupdateCases[].memberName` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].nativeProof.hotupdateCases[].methodSubjectId` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].nativeProof.hotupdateCases[].projectPath` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].nativeProof.hotupdateCases[].sourceFile` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].nativeProof.methodDetails` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].nativeProof.methodDetails[].covered` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].nativeProof.methodDetails[].subjectId` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].nativeProof.numerator` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].nativeProof.progressPercent` | family-verification.json | generator/_compute_gate_progress [L352] | (待补充) | (待补充) |
| `assemblies[].families[].nativeProof.reason` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].nativeProof.runs` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].nativeProof.runs[].evidence` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].nativeProof.runs[].evidence[].artifactKind` | family-verification.json | generator/_artifact_record [L264] | (待补充) | (待补充) |
| `assemblies[].families[].nativeProof.runs[].evidence[].label` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].nativeProof.runs[].evidence[].linkTargetType` | family-verification.json | generator/_artifact_record [L264] | (待补充) | (待补充) |
| `assemblies[].families[].nativeProof.runs[].evidence[].path` | family-verification.json | generator/_artifact_record [L264] | (待补充) | (待补充) |
| `assemblies[].families[].nativeProof.runs[].runId` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].nativeProof.runs[].status` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].nativeProof.status` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].nativeProof.testCases` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].nativeProof.testCases[].annotationDigest` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].nativeProof.testCases[].caseKind` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].nativeProof.testCases[].className` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].nativeProof.testCases[].familyId` | family-verification.json | generator/_auto_derive_family [L414] | (待补充) | (待补充) |
| `assemblies[].families[].nativeProof.testCases[].memberName` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].nativeProof.testCases[].methodSubjectId` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].nativeProof.testCases[].profileCode` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].nativeProof.testCases[].projectPath` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].nativeProof.testCases[].routeCode` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].nativeProof.testCases[].sourceFile` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].reviewBundle` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].reviewBundle.assemblyName` | family-verification.json | generator/_artifact_record [L264]; kernel/build_family_verification_snapshot [L279]; claims/build_family_verification_claims_snapshot [L132] | (待补充) | (待补充) |
| `assemblies[].families[].reviewBundle.displayName` | family-verification.json | generator/_artifact_record [L264]; generator/_auto_derive_family [L414] | (待补充) | (待补充) |
| `assemblies[].families[].reviewBundle.familyId` | family-verification.json | generator/_auto_derive_family [L414] | (待补充) | (待补充) |
| `assemblies[].families[].reviewBundle.generatedArtifacts` | family-verification.json | gap/analyze_gaps [L310] | (待补充) | (待补充) |
| `assemblies[].families[].reviewBundle.notesPath` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].reviewBundle.schemaVersion` | family-verification.json | kernel/build_family_verification_snapshot [L279]; claims/build_family_verification_claims_snapshot [L132] | (待补充) | (待补充) |
| `assemblies[].families[].reviewBundle.solutionPath` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].sourceGroups` | family-verification.json | generator/_auto_derive_family [L414] | (待补充) | (待补充) |
| `assemblies[].families[].sourceGroups[].groupId` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].sourceGroups[].methodCount` | family-verification.json | generator/_auto_derive_family [L414] | (待补充) | (待补充) |
| `assemblies[].families[].sourceGroups[].typeNames` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].synthesisStatus` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].testCode` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].testCode.action` | family-verification.json | gap/_derive_test_code_status [L55] | (待补充) | (待补充) |
| `assemblies[].families[].testCode.allMethodsCovered` | family-verification.json | gap/_derive_test_code_status [L55] | (待补充) | (待补充) |
| `assemblies[].families[].testCode.emittedMethodCount` | family-verification.json | gap/_derive_test_code_status [L55] | (待补充) | (待补充) |
| `assemblies[].families[].testCode.requestedMethodCount` | family-verification.json | gap/_derive_test_code_status [L55] | (待补充) | (待补充) |
| `assemblies[].families[].testCode.testCodeStatus` | family-verification.json | gap/_derive_test_code_status [L55] | (待补充) | (待补充) |
| `assemblies[].families[].testCode.testProjectPath` | family-verification.json | gap/_derive_test_code_status [L55] | (待补充) | (待补充) |
| `assemblies[].families[].testCodeProof` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].testCodeProof.denominator` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].testCodeProof.evidence` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].testCodeProof.evidence[].artifactKind` | family-verification.json | generator/_artifact_record [L264] | (待补充) | (待补充) |
| `assemblies[].families[].testCodeProof.evidence[].label` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].testCodeProof.evidence[].linkTargetType` | family-verification.json | generator/_artifact_record [L264] | (待补充) | (待补充) |
| `assemblies[].families[].testCodeProof.evidence[].path` | family-verification.json | generator/_artifact_record [L264] | (待补充) | (待补充) |
| `assemblies[].families[].testCodeProof.methodDetails` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].testCodeProof.numerator` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].testCodeProof.progressPercent` | family-verification.json | generator/_compute_gate_progress [L352] | (待补充) | (待补充) |
| `assemblies[].families[].testCodeProof.reason` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].testCodeProof.runs` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].testCodeProof.status` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].verificationGates` | family-verification.json | generator/_auto_derive_family [L414] | (待补充) | (待补充) |
| `assemblies[].families[].verificationGates.audit-input-and-ledger` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].verificationGates.benchmark` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].verificationGates.codegen-review` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].verificationGates.completion-certification` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].verificationGates.hotupdate-proof` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].verificationGates.managed-proof` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].families[].verificationGates.native-proof` | family-verification.json | (直接赋值) | (待补充) | (待补充) |
| `assemblies[].ownerSubjectId` | family-verification.json | kernel/build_family_verification_snapshot [L279]; claims/build_family_verification_claims_snapshot [L132] | (待补充) | (待补充) |
| `assemblies[].schemaVersion` | family-verification.json | kernel/build_family_verification_snapshot [L279]; claims/build_family_verification_claims_snapshot [L132] | (待补充) | (待补充) |
| `assemblyName` | dll-detail.json | generator/_artifact_record [L264]; kernel/build_family_verification_snapshot [L279]; claims/build_family_verification_claims_snapshot [L132] | (待补充) | (待补充) |
| `blockingReason` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityClosure` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityClosure.closedFamilies` | dll-detail.json | generator/_compute_capability_closure [L333] | (待补充) | (待补充) |
| `capabilityClosure.closurePercent` | dll-detail.json | generator/_compute_capability_closure [L333] | (待补充) | (待补充) |
| `capabilityClosure.excludedFamilies` | dll-detail.json | generator/_compute_capability_closure [L333] | (待补充) | (待补充) |
| `capabilityClosure.inProgressFamilies` | dll-detail.json | generator/_compute_capability_closure [L333] | (待补充) | (待补充) |
| `capabilityClosure.platformBlockedFamilies` | dll-detail.json | generator/_compute_capability_closure [L333] | (待补充) | (待补充) |
| `capabilityClosure.totalFamilies` | dll-detail.json | generator/_compute_capability_closure [L333] | (待补充) | (待补充) |
| `capabilityClosure.waivedFamilies` | dll-detail.json | generator/_compute_capability_closure [L333] | (待补充) | (待补充) |
| `capabilityFamilies` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].auditInputProof` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].auditInputProof.caseItems` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].auditInputProof.caseSectionLabel` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].auditInputProof.denominator` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].auditInputProof.evidence` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].auditInputProof.numerator` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].auditInputProof.progressPercent` | dll-detail.json | generator/_compute_gate_progress [L352] | (待补充) | (待补充) |
| `capabilityFamilies[].auditInputProof.status` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].authorityRefs` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].benchmarkProof` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].benchmarkProof.caseItems` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].benchmarkProof.caseSectionLabel` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].benchmarkProof.denominator` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].benchmarkProof.evidence` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].benchmarkProof.methodDetails` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].benchmarkProof.numerator` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].benchmarkProof.progressPercent` | dll-detail.json | generator/_compute_gate_progress [L352] | (待补充) | (待补充) |
| `capabilityFamilies[].benchmarkProof.reason` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].benchmarkProof.runs` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].benchmarkProof.status` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].closureStatus` | dll-detail.json | generator/_auto_derive_family [L414] | (待补充) | (待补充) |
| `capabilityFamilies[].codegenReviewProof` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].codegenReviewProof.caseItems` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].codegenReviewProof.caseSectionLabel` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].codegenReviewProof.denominator` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].codegenReviewProof.evidence` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].codegenReviewProof.numerator` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].codegenReviewProof.progressPercent` | dll-detail.json | generator/_compute_gate_progress [L352] | (待补充) | (待补充) |
| `capabilityFamilies[].codegenReviewProof.status` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].confirmationNote` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].denominatorStatus` | dll-detail.json | generator/_auto_derive_family [L414] | (待补充) | (待补充) |
| `capabilityFamilies[].derivationBasis` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].description` | dll-detail.json | generator/_auto_derive_family [L414] | (待补充) | (待补充) |
| `capabilityFamilies[].displayName` | dll-detail.json | generator/_artifact_record [L264]; generator/_auto_derive_family [L414] | (待补充) | (待补充) |
| `capabilityFamilies[].familyId` | dll-detail.json | generator/_auto_derive_family [L414] | (待补充) | (待补充) |
| `capabilityFamilies[].hotupdateProof` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].hotupdateProof.caseItems` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].hotupdateProof.caseSectionLabel` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].hotupdateProof.denominator` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].hotupdateProof.evidence` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].hotupdateProof.methodDetails` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].hotupdateProof.numerator` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].hotupdateProof.progressPercent` | dll-detail.json | generator/_compute_gate_progress [L352] | (待补充) | (待补充) |
| `capabilityFamilies[].hotupdateProof.reason` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].hotupdateProof.runs` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].hotupdateProof.status` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].implementationFamilies` | dll-detail.json | generator/_auto_derive_family [L414] | (待补充) | (待补充) |
| `capabilityFamilies[].managedProof` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].managedProof.caseItems` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].managedProof.caseSectionLabel` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].managedProof.denominator` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].managedProof.evidence` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].managedProof.methodDetails` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].managedProof.numerator` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].managedProof.progressPercent` | dll-detail.json | generator/_compute_gate_progress [L352] | (待补充) | (待补充) |
| `capabilityFamilies[].managedProof.reason` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].managedProof.runs` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].managedProof.status` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].methodCount` | dll-detail.json | generator/_auto_derive_family [L414] | (待补充) | (待补充) |
| `capabilityFamilies[].nativeProof` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].nativeProof.benchmarkCases` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].nativeProof.denominator` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].nativeProof.evidence` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].nativeProof.hotupdateCases` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].nativeProof.methodDetails` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].nativeProof.numerator` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].nativeProof.progressPercent` | dll-detail.json | generator/_compute_gate_progress [L352] | (待补充) | (待补充) |
| `capabilityFamilies[].nativeProof.reason` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].nativeProof.runs` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].nativeProof.status` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].nativeProof.testCases` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].reviewBundle` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].sourceGroups` | dll-detail.json | generator/_auto_derive_family [L414] | (待补充) | (待补充) |
| `capabilityFamilies[].synthesisStatus` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].testCode` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].testCode.action` | dll-detail.json | gap/_derive_test_code_status [L55] | (待补充) | (待补充) |
| `capabilityFamilies[].testCode.allMethodsCovered` | dll-detail.json | gap/_derive_test_code_status [L55] | (待补充) | (待补充) |
| `capabilityFamilies[].testCode.emittedMethodCount` | dll-detail.json | gap/_derive_test_code_status [L55] | (待补充) | (待补充) |
| `capabilityFamilies[].testCode.requestedMethodCount` | dll-detail.json | gap/_derive_test_code_status [L55] | (待补充) | (待补充) |
| `capabilityFamilies[].testCode.testCodeStatus` | dll-detail.json | gap/_derive_test_code_status [L55] | (待补充) | (待补充) |
| `capabilityFamilies[].testCode.testProjectPath` | dll-detail.json | gap/_derive_test_code_status [L55] | (待补充) | (待补充) |
| `capabilityFamilies[].testCodeProof` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].testCodeProof.denominator` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].testCodeProof.evidence` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].testCodeProof.methodDetails` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].testCodeProof.numerator` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].testCodeProof.progressPercent` | dll-detail.json | generator/_compute_gate_progress [L352] | (待补充) | (待补充) |
| `capabilityFamilies[].testCodeProof.reason` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].testCodeProof.runs` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].testCodeProof.status` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].verificationGates` | dll-detail.json | generator/_auto_derive_family [L414] | (待补充) | (待补充) |
| `capabilityFamilies[].verificationGates.audit-input-and-ledger` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].verificationGates.benchmark` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].verificationGates.codegen-review` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].verificationGates.hotupdate-proof` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].verificationGates.managed-proof` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `capabilityFamilies[].verificationGates.native-proof` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `currentProject` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `denominatorStatus` | dll-detail.json | generator/_auto_derive_family [L414] | (待补充) | (待补充) |
| `dllState` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationClaims` | dll-detail.json | generator/build_foundation_dll_audit_payload [L724] | (待补充) | (待补充) |
| `familyVerificationClaims.assemblyName` | dll-detail.json | generator/_artifact_record [L264]; kernel/build_family_verification_snapshot [L279]; claims/build_family_verification_claims_snapshot [L132] | (待补充) | (待补充) |
| `familyVerificationClaims.claims` | dll-detail.json | claims/build_family_verification_claims_snapshot [L132] | (待补充) | (待补充) |
| `familyVerificationClaims.claims[].authorityRefs` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationClaims.claims[].claimId` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationClaims.claims[].denominator` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationClaims.claims[].derivationRule` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationClaims.claims[].familyId` | dll-detail.json | generator/_auto_derive_family [L414] | (待补充) | (待补充) |
| `familyVerificationClaims.claims[].gateCode` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationClaims.claims[].methodSubjectIds` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationClaims.claims[].ownerSubjectId` | dll-detail.json | kernel/build_family_verification_snapshot [L279]; claims/build_family_verification_claims_snapshot [L132] | (待补充) | (待补充) |
| `familyVerificationClaims.claims[].required` | dll-detail.json | generator/_artifact_record [L264] | (待补充) | (待补充) |
| `familyVerificationClaims.claims[].scope` | dll-detail.json | gap/analyze_gaps [L310] | (待补充) | (待补充) |
| `familyVerificationClaims.methodUniverseArtifactPaths` | dll-detail.json | claims/build_family_verification_claims_snapshot [L132] | (待补充) | (待补充) |
| `familyVerificationClaims.ownerSubjectId` | dll-detail.json | kernel/build_family_verification_snapshot [L279]; claims/build_family_verification_claims_snapshot [L132] | (待补充) | (待补充) |
| `familyVerificationClaims.schemaVersion` | dll-detail.json | kernel/build_family_verification_snapshot [L279]; claims/build_family_verification_claims_snapshot [L132] | (待补充) | (待补充) |
| `familyVerificationSnapshot` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.assemblyName` | dll-detail.json | generator/_artifact_record [L264]; kernel/build_family_verification_snapshot [L279]; claims/build_family_verification_claims_snapshot [L132] | (待补充) | (待补充) |
| `familyVerificationSnapshot.families` | dll-detail.json | kernel/build_family_verification_snapshot [L279]; gap/analyze_gaps [L310] | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].auditInputProof` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].auditInputProof.caseItems` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].auditInputProof.caseSectionLabel` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].auditInputProof.denominator` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].auditInputProof.evidence` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].auditInputProof.numerator` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].auditInputProof.progressPercent` | dll-detail.json | generator/_compute_gate_progress [L352] | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].auditInputProof.status` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].authorityRefs` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].benchmarkProof` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].benchmarkProof.caseItems` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].benchmarkProof.caseSectionLabel` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].benchmarkProof.denominator` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].benchmarkProof.evidence` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].benchmarkProof.methodDetails` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].benchmarkProof.numerator` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].benchmarkProof.progressPercent` | dll-detail.json | generator/_compute_gate_progress [L352] | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].benchmarkProof.reason` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].benchmarkProof.runs` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].benchmarkProof.status` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].closureStatus` | dll-detail.json | generator/_auto_derive_family [L414] | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].codegenReviewProof` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].codegenReviewProof.caseItems` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].codegenReviewProof.caseSectionLabel` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].codegenReviewProof.denominator` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].codegenReviewProof.evidence` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].codegenReviewProof.numerator` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].codegenReviewProof.progressPercent` | dll-detail.json | generator/_compute_gate_progress [L352] | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].codegenReviewProof.status` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].confirmationNote` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].denominatorStatus` | dll-detail.json | generator/_auto_derive_family [L414] | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].derivationBasis` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].description` | dll-detail.json | generator/_auto_derive_family [L414] | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].displayName` | dll-detail.json | generator/_artifact_record [L264]; generator/_auto_derive_family [L414] | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].familyId` | dll-detail.json | generator/_auto_derive_family [L414] | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].hotupdateProof` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].hotupdateProof.caseItems` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].hotupdateProof.caseSectionLabel` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].hotupdateProof.denominator` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].hotupdateProof.evidence` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].hotupdateProof.methodDetails` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].hotupdateProof.numerator` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].hotupdateProof.progressPercent` | dll-detail.json | generator/_compute_gate_progress [L352] | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].hotupdateProof.reason` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].hotupdateProof.runs` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].hotupdateProof.status` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].implementationFamilies` | dll-detail.json | generator/_auto_derive_family [L414] | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].managedProof` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].managedProof.caseItems` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].managedProof.caseSectionLabel` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].managedProof.denominator` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].managedProof.evidence` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].managedProof.methodDetails` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].managedProof.numerator` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].managedProof.progressPercent` | dll-detail.json | generator/_compute_gate_progress [L352] | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].managedProof.reason` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].managedProof.runs` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].managedProof.status` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].methodCount` | dll-detail.json | generator/_auto_derive_family [L414] | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].nativeProof` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].nativeProof.benchmarkCases` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].nativeProof.denominator` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].nativeProof.evidence` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].nativeProof.hotupdateCases` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].nativeProof.methodDetails` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].nativeProof.numerator` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].nativeProof.progressPercent` | dll-detail.json | generator/_compute_gate_progress [L352] | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].nativeProof.reason` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].nativeProof.runs` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].nativeProof.status` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].nativeProof.testCases` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].reviewBundle` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].sourceGroups` | dll-detail.json | generator/_auto_derive_family [L414] | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].synthesisStatus` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].testCode` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].testCode.action` | dll-detail.json | gap/_derive_test_code_status [L55] | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].testCode.allMethodsCovered` | dll-detail.json | gap/_derive_test_code_status [L55] | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].testCode.emittedMethodCount` | dll-detail.json | gap/_derive_test_code_status [L55] | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].testCode.requestedMethodCount` | dll-detail.json | gap/_derive_test_code_status [L55] | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].testCode.testCodeStatus` | dll-detail.json | gap/_derive_test_code_status [L55] | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].testCode.testProjectPath` | dll-detail.json | gap/_derive_test_code_status [L55] | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].testCodeProof` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].testCodeProof.denominator` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].testCodeProof.evidence` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].testCodeProof.methodDetails` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].testCodeProof.numerator` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].testCodeProof.progressPercent` | dll-detail.json | generator/_compute_gate_progress [L352] | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].testCodeProof.reason` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].testCodeProof.runs` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].testCodeProof.status` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].verificationGates` | dll-detail.json | generator/_auto_derive_family [L414] | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].verificationGates.audit-input-and-ledger` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].verificationGates.benchmark` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].verificationGates.codegen-review` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].verificationGates.hotupdate-proof` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].verificationGates.managed-proof` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.families[].verificationGates.native-proof` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `familyVerificationSnapshot.ownerSubjectId` | dll-detail.json | kernel/build_family_verification_snapshot [L279]; claims/build_family_verification_claims_snapshot [L132] | (待补充) | (待补充) |
| `familyVerificationSnapshot.schemaVersion` | dll-detail.json | kernel/build_family_verification_snapshot [L279]; claims/build_family_verification_claims_snapshot [L132] | (待补充) | (待补充) |
| `orderIndex` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `phase` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `projects` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `projects[].artifacts` | dll-detail.json | generator/write_foundation_dll_audit_outputs [L2407] | (待补充) | (待补充) |
| `projects[].blockers` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `projects[].completionRule` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `projects[].displayName` | dll-detail.json | generator/_artifact_record [L264]; generator/_auto_derive_family [L414] | (待补充) | (待补充) |
| `projects[].executionState` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `projects[].policyState` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `projects[].projectCode` | dll-detail.json | generator/_artifact_record [L264] | (待补充) | (待补充) |
| `projects[].supportRefs` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `projects[].updatedAt` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `projects[].verificationMethod` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `projects[].verificationTarget` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `riskTags` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `roadmapTaskId` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `sourceLinks` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `sourceLinks.evidence` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `sourceLinks.generatedCode` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `sourceLinks.subjectSource` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `sourceLinks.verificationSource` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `truthContracts` | dll-detail.json | generator/build_foundation_dll_audit_payload [L724] | (待补充) | (待补充) |
| `truthContracts.capabilityFamilyVerificationContracts` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `truthContracts.capabilityFamilyVerificationContracts[].assemblyName` | dll-detail.json | generator/_artifact_record [L264]; kernel/build_family_verification_snapshot [L279]; claims/build_family_verification_claims_snapshot [L132] | (待补充) | (待补充) |
| `truthContracts.capabilityFamilyVerificationContracts[].displayName` | dll-detail.json | generator/_artifact_record [L264]; generator/_auto_derive_family [L414] | (待补充) | (待补充) |
| `truthContracts.capabilityFamilyVerificationContracts[].familyId` | dll-detail.json | generator/_auto_derive_family [L414] | (待补充) | (待补充) |
| `truthContracts.capabilityFamilyVerificationContracts[].functionalObligation` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `truthContracts.capabilityFamilyVerificationContracts[].functionalObligation.required` | dll-detail.json | generator/_artifact_record [L264] | (待补充) | (待补充) |
| `truthContracts.capabilityFamilyVerificationContracts[].hotupdateFunctionalObligation` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `truthContracts.capabilityFamilyVerificationContracts[].hotupdateFunctionalObligation.required` | dll-detail.json | generator/_artifact_record [L264] | (待补充) | (待补充) |
| `truthContracts.capabilityFamilyVerificationContracts[].hotupdatePerformanceObligation` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `truthContracts.capabilityFamilyVerificationContracts[].hotupdatePerformanceObligation.required` | dll-detail.json | generator/_artifact_record [L264] | (待补充) | (待补充) |
| `truthContracts.capabilityFamilyVerificationContracts[].methodSubjectCount` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `truthContracts.capabilityFamilyVerificationContracts[].methodSubjectIds` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `truthContracts.capabilityFamilyVerificationContracts[].ownerSubjectId` | dll-detail.json | kernel/build_family_verification_snapshot [L279]; claims/build_family_verification_claims_snapshot [L132] | (待补充) | (待补充) |
| `truthContracts.capabilityFamilyVerificationContracts[].performanceObligation` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `truthContracts.capabilityFamilyVerificationContracts[].performanceObligation.required` | dll-detail.json | generator/_artifact_record [L264] | (待补充) | (待补充) |
| `truthContracts.capabilityFamilyVerificationContracts[].reviewBundleRequired` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `truthContracts.dllCapabilityManifest` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `truthContracts.dllCapabilityManifest.assemblyName` | dll-detail.json | generator/_artifact_record [L264]; kernel/build_family_verification_snapshot [L279]; claims/build_family_verification_claims_snapshot [L132] | (待补充) | (待补充) |
| `truthContracts.dllCapabilityManifest.capabilityFamilies` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `truthContracts.dllCapabilityManifest.capabilityFamilyCount` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `truthContracts.dllCapabilityManifest.methodUniverseCount` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `truthContracts.dllCapabilityManifest.ownerSubjectId` | dll-detail.json | kernel/build_family_verification_snapshot [L279]; claims/build_family_verification_claims_snapshot [L132] | (待补充) | (待补充) |
| `truthContracts.methodCapabilityContracts` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `truthContracts.schemaVersion` | dll-detail.json | kernel/build_family_verification_snapshot [L279]; claims/build_family_verification_claims_snapshot [L132] | (待补充) | (待补充) |
| `waiverSummary` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `waiverSummary.activeWaivers` | dll-detail.json | generator/_compute_waiver_summary [L376] | (待补充) | (待补充) |
| `waiverSummary.expiredWaivers` | dll-detail.json | generator/_compute_waiver_summary [L376] | (待补充) | (待补充) |
| `waiverSummary.totalExclusions` | dll-detail.json | generator/_compute_waiver_summary [L376] | (待补充) | (待补充) |
| `waiverSummary.totalPlatformBlocked` | dll-detail.json | generator/_compute_waiver_summary [L376] | (待补充) | (待补充) |
| `waiverSummary.totalWaivers` | dll-detail.json | generator/_compute_waiver_summary [L376] | (待补充) | (待补充) |
| `workflowProgress` | dll-detail.json | (直接赋值) | (待补充) | (待补充) |
| `workflowProgress.passedGates` | dll-detail.json | generator/_compute_gate_progress [L352] | (待补充) | (待补充) |
| `workflowProgress.progressPercent` | dll-detail.json | generator/_compute_gate_progress [L352] | (待补充) | (待补充) |
| `workflowProgress.totalRequiredGates` | dll-detail.json | generator/_compute_gate_progress [L352] | (待补充) | (待补充) |

---
*基线版本 v1，生成时间: (请填写日期)*
