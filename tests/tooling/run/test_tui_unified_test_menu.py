from tests.tooling.run.tui_test_support import *


class TestTuiUnifiedTestMenu(TuiUnifiedMenuTestSupport):
    def test_build_test_menu_entries_uses_new_unified_objects(self) -> None:
        manifest_module = load_manifest_module()
        tui_module = load_tui_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        entries = tui_module.build_test_menu_entries(manifest, "windows")

        self.assertEqual(
            [
                "test-all",
                "test-suite",
                "test-subject",
                "test-module",
                "test-system",
                "test-registry-list",
                "test-registry-refresh",
                "test-registry-check-consistency",
                "test-watch",
                "test-summary",
                "menu-back",
            ],
            [entry.command["id"] for entry in entries],
        )
        self.assertEqual(
            ["Quick Start", "Selectors", "Selectors", "Selectors", "Selectors", "Registry", "Registry", "Registry", "Results", "Results", "Back"],
            [entry.group_title for entry in entries],
        )

    def test_build_test_menu_entries_describes_test_all_as_suite_and_subject_batch(self) -> None:
        manifest_module = load_manifest_module()
        tui_module = load_tui_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        entries = tui_module.build_test_menu_entries(manifest, "windows")

        title = next(entry.command["title"] for entry in entries if entry.command["id"] == "test-all")
        self.assertIn("suites + subjects", title)

    def test_build_test_subject_menu_entries_lists_registered_subjects(self) -> None:
        tui_module = load_tui_module()
        subject_id = "FixtureSubject"

        with patch.object(
            tui_module.registry_module,
            "scan_registry",
            return_value=make_subject_registry_index(tui_module, subject_id),
        ):
            entries = tui_module.build_test_subject_menu_entries("windows")

        subject_entry = next(entry for entry in entries if entry.syntax == subject_id)
        self.assertEqual(["test", "subject", "--id", f"subject/{subject_id}"], subject_entry.argv)
        self.assertEqual(f"subject/{subject_id}", subject_entry.command["targetObjectId"])
        self.assertEqual("menu-back", entries[-1].command["id"])

    def test_render_test_all_preview_screen_summarizes_batch(self) -> None:
        tui_module = load_tui_module()
        subject_id = "FixtureSubject"

        screen = tui_module.render_test_all_preview_screen(
            {
                "hostPlatform": "windows",
                "plannedCounts": {"totalUnits": 3, "suiteCount": 2, "subjectCount": 1},
                "countsByFamily": {
                    "smoke": {"total": 1},
                    "contract": {"total": 1},
                },
                "subjectPreview": [subject_id],
                "outputPaths": {
                    "summaryPath": "artifacts/logs/tests/<run-id>/summary.json",
                    "eventsPath": "artifacts/logs/tests/<run-id>/events.jsonl",
                    "consolePath": "artifacts/logs/tests/<run-id>/console.log",
                },
            }
        )

        self.assertIn("Unified Test Batch Preview", screen)
        self.assertIn("Host: windows", screen)
        self.assertIn("Planned: 3 = 2 suites + 1 subjects", screen)
        self.assertIn("Families: contract 1 | smoke 1", screen)
        self.assertIn(f"Subjects: {subject_id}", screen)
        self.assertIn("artifacts/logs/tests/<run-id>/summary.json", screen)

    def test_run_test_submenu_routes_subject_entry_to_third_level_subject_menu(self) -> None:
        tui_module = load_tui_module()
        manifest = {"groups": [], "commands": []}
        subject_entry = tui_module.MenuEntry(
            "Selectors",
            {"id": "test-subject", "title": "Run a subject object"},
            "subject",
            ["test"],
        )
        fake_terminal = object()

        with patch.object(tui_module, "build_test_menu_entries", return_value=[subject_entry]):
            with patch.object(tui_module, "_run_menu_selection", return_value=subject_entry):
                with patch.object(
                    tui_module,
                    "run_test_subject_submenu",
                    return_value=["test", "subject", "--id", "subject/FixtureSubject"],
                ) as run_test_subject_submenu:
                    argv = tui_module.run_test_submenu(manifest, "windows", terminal=fake_terminal)

        self.assertEqual(["test", "subject", "--id", "subject/FixtureSubject"], argv)
        run_test_subject_submenu.assert_called_once_with(
            manifest,
            "windows",
            terminal=fake_terminal,
            menu_state=None,
        )

    def test_run_test_submenu_routes_test_all_entry_through_preview(self) -> None:
        tui_module = load_tui_module()
        manifest = {"groups": [], "commands": []}
        test_all_entry = tui_module.MenuEntry(
            "Quick Start",
            {"id": "test-all", "title": "Run host default batch (suites + subjects)"},
            "all",
            ["test", "all"],
        )
        fake_terminal = object()

        with patch.object(tui_module, "build_test_menu_entries", return_value=[test_all_entry]):
            with patch.object(tui_module, "_run_menu_selection", return_value=test_all_entry):
                with patch.object(
                    tui_module,
                    "run_test_all_preview",
                    return_value=["test", "all"],
                ) as run_test_all_preview:
                    argv = tui_module.run_test_submenu(manifest, "windows", terminal=fake_terminal)

        self.assertEqual(["test", "all"], argv)
        run_test_all_preview.assert_called_once_with("windows", terminal=fake_terminal)
