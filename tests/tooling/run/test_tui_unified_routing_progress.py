from tests.tooling.run.tui_test_support import *


class TestTuiUnifiedRoutingProgress(TuiUnifiedMenuTestSupport):
    def test_resolve_entry_argv_supports_suite_subject_module_and_system(self) -> None:
        manifest_module = load_manifest_module()
        tui_module = load_tui_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)
        test_entry = next(entry for entry in tui_module.build_menu_entries(manifest, "windows") if entry.syntax == "test")
        smoke_spec = select_public_suite_spec(
            "chaos_tui_unified_suite_selector",
            host_platform="windows",
            family="smoke",
            required_stages=["all"],
        )

        suite_answers = iter(["suite", f"{smoke_spec['family']} {smoke_spec['suite']}"])
        suite_argv = tui_module.resolve_entry_argv(test_entry, prompt_value_provider=lambda prompt: next(suite_answers))
        self.assertEqual(
            ["test", "suite", "--family", str(smoke_spec["family"]), "--suite", str(smoke_spec["suite"])],
            suite_argv,
        )

        subject_id = "FixtureSubject"
        subject_answers = iter(["subject", subject_id])
        subject_argv = tui_module.resolve_entry_argv(test_entry, prompt_value_provider=lambda prompt: next(subject_answers))
        self.assertEqual(["test", "subject", "--subject", subject_id], subject_argv)

        module_answers = iter(["module", "managed-smoke basic"])
        module_argv = tui_module.resolve_entry_argv(test_entry, prompt_value_provider=lambda prompt: next(module_answers))
        self.assertEqual(["test", "module", "--module", "managed-smoke", "--profile", "basic"], module_argv)

        system_answers = iter(["system", "hosted-runtime-smoke"])
        system_argv = tui_module.resolve_entry_argv(test_entry, prompt_value_provider=lambda prompt: next(system_answers))
        self.assertEqual(["test", "system", "--scenario", "hosted-runtime-smoke"], system_argv)

    def test_resolve_entry_argv_supports_registry_and_result_actions(self) -> None:
        manifest_module = load_manifest_module()
        tui_module = load_tui_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)
        test_entry = next(entry for entry in tui_module.build_menu_entries(manifest, "windows") if entry.syntax == "test")

        self.assertEqual(["test", "all"], tui_module.resolve_entry_argv(test_entry, prompt_value_provider=lambda prompt: "all"))
        self.assertEqual(
            ["test", "registry", "list"],
            tui_module.resolve_entry_argv(test_entry, prompt_value_provider=lambda prompt: "registry-list"),
        )
        self.assertEqual(
            ["test", "registry", "refresh"],
            tui_module.resolve_entry_argv(test_entry, prompt_value_provider=lambda prompt: "registry-refresh"),
        )
        self.assertEqual(
            ["test", "registry", "check-consistency"],
            tui_module.resolve_entry_argv(test_entry, prompt_value_provider=lambda prompt: "registry-check"),
        )
        self.assertEqual(["test", "watch"], tui_module.resolve_entry_argv(test_entry, prompt_value_provider=lambda prompt: "watch"))
        self.assertEqual(["test", "summary"], tui_module.resolve_entry_argv(test_entry, prompt_value_provider=lambda prompt: "summary"))
        self.assertIsNone(
            tui_module.resolve_entry_argv(test_entry, prompt_value_provider=lambda prompt: "verify"),
        )

    def test_direct_registry_entry_resolves_to_public_command(self) -> None:
        manifest_module = load_manifest_module()
        tui_module = load_tui_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        entry = next(
            item for item in tui_module.build_test_menu_entries(manifest, "windows") if item.command["id"] == "test-registry-check-consistency"
        )

        self.assertEqual(["test", "registry", "check-consistency"], tui_module.resolve_entry_argv(entry))

    def test_render_test_progress_screen_renders_phase_results(self) -> None:
        tui_module = load_tui_module()

        screen = tui_module.render_test_progress_screen(
            [
                {"eventType": "session-start", "payload": {"command": "test system --id system/runtime-baseline-macos"}},
                {"eventType": "progress", "payload": {"completedUnits": 1, "totalUnits": 2}},
                {
                    "eventType": "final-summary",
                    "payload": {
                        "finalStatus": "ok",
                        "phaseResults": [
                            {"phaseId": "code", "status": "ok"},
                            {"phaseId": "module", "status": "ok"},
                            {"phaseId": "system", "status": "ok"},
                        ],
                    },
                },
            ],
            repo_root=REPO_ROOT,
        )

        self.assertIn("Phases:", screen)
        self.assertIn("ok: code", screen)
        self.assertIn("ok: module", screen)
        self.assertIn("ok: system", screen)

    def test_render_test_progress_screen_highlights_subject_summary(self) -> None:
        tui_module = load_tui_module()
        subject_id = "FixtureSubject"
        matrix_id = "windows-native-check"
        run_id = "fixture-run-summary-001"
        summary_path = f"artifacts/subjects/{subject_id}/runs/{run_id}/subject-report/summary.json"
        matrix_report_path = f"artifacts/subjects/{subject_id}/runs/{run_id}/matrices/{matrix_id}/pipeline-report/report.json"

        screen = tui_module.render_test_progress_screen(
            [
                {"eventType": "session-start", "payload": {"command": f"test subject --id subject/{subject_id}"}},
                {
                    "eventType": "final-summary",
                    "payload": {
                        "finalStatus": "ok",
                        "subjectResults": [
                            {
                                "subjectId": subject_id,
                                "status": "ok",
                                "subjectSummaryPath": summary_path,
                            }
                        ],
                        "artifacts": [
                            summary_path,
                            matrix_report_path,
                        ],
                    },
                },
            ],
            repo_root=REPO_ROOT,
        )

        self.assertIn("Subjects:", screen)
        self.assertIn(f"ok: {subject_id}", screen)
        self.assertIn("\x1b[1;36mSubject summary:\x1b[0m", screen)
        subject_summary_uri = (REPO_ROOT / summary_path).resolve().as_uri()
        self.assertIn(
            f"\x1b]8;;{subject_summary_uri}\x1b\\{summary_path}\x1b]8;;\x1b\\",
            screen,
        )

    def test_render_test_progress_screen_renders_batch_counts_current_and_failures(self) -> None:
        tui_module = load_tui_module()

        screen = tui_module.render_test_progress_screen(
            [
                {"eventType": "session-start", "payload": {"command": "test all"}},
                {
                    "eventType": "stage-start",
                    "payload": {
                        "completedUnits": 1,
                        "totalUnits": 3,
                        "activeUnit": "contract/trace-schema",
                        "activeUnitContext": {
                            "id": "contract/trace-schema",
                            "type": "suite",
                            "family": "contract",
                            "level": "code",
                            "primaryModuleId": "trace-export",
                        },
                    },
                },
                {
                    "eventType": "final-summary",
                    "payload": {
                        "finalStatus": "fail",
                        "plannedCounts": {"totalUnits": 3, "suiteCount": 2, "subjectCount": 1},
                        "countsByType": {
                            "suite": {"total": 2, "ok": 1, "fail": 1, "skip": 0, "aborted": 0},
                            "subject": {"total": 1, "ok": 0, "fail": 0, "skip": 0, "aborted": 1},
                        },
                        "countsByFamily": {
                            "smoke": {"total": 1, "ok": 1, "fail": 0, "skip": 0, "aborted": 0},
                            "contract": {"total": 1, "ok": 0, "fail": 1, "skip": 0, "aborted": 0},
                        },
                        "failureItems": [
                            {
                                "id": "contract/trace-schema",
                                "type": "suite",
                                "status": "fail",
                                "rerunCommand": "run test contract trace-schema",
                            }
                        ],
                    },
                },
            ],
            repo_root=REPO_ROOT,
        )

        self.assertIn("Batch:", screen)
        self.assertIn("Planned: 3 = 2 suites + 1 subjects", screen)
        self.assertIn("Families: contract 1 | smoke 1", screen)
        self.assertIn("Counts:", screen)
        self.assertIn("suite: total 2 | ok 1 | fail 1 | skip 0 | aborted 0", screen)
        self.assertIn("subject: total 1 | ok 0 | fail 0 | skip 0 | aborted 1", screen)
        self.assertIn("Current:", screen)
        self.assertIn("family=contract | level=code | module=trace-export", screen)
        self.assertIn("Failures:", screen)
        self.assertIn("rerun: run test contract trace-schema", screen)
