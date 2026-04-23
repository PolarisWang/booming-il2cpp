from tests.tooling.run.tui_test_support import *


class TestTuiUnifiedProjectFlow(TuiUnifiedMenuTestSupport):
    def test_run_project_submenu_routes_generate_all_entry_through_aggregate_flow(self) -> None:
        tui_module = load_tui_module()
        manifest = {"groups": [], "commands": []}
        generate_all_entry = tui_module.MenuEntry(
            "生成工程",
            {"id": "generate-project-all", "title": "生成所有 Solution（当前宿主）"},
            "all",
            ["generate", "project", "all"],
        )
        fake_terminal = object()

        with patch.object(tui_module, "build_project_menu_entries", return_value=[generate_all_entry]):
            with patch.object(tui_module, "_run_menu_selection", return_value=generate_all_entry):
                with patch.object(
                    tui_module,
                    "run_generate_project_all_flow",
                    return_value=["generate", "project", "all", "--host", "windows"],
                ) as run_generate_project_all_flow:
                    argv = tui_module.run_project_submenu(manifest, "windows", terminal=fake_terminal)

        self.assertEqual(["generate", "project", "all", "--host", "windows"], argv)
        run_generate_project_all_flow.assert_called_once_with("windows", terminal=fake_terminal)

    def test_run_generate_project_all_flow_supports_refresh_generated(self) -> None:
        tui_module = load_tui_module()
        refresh_entry = tui_module.MenuEntry(
            "刷新策略",
            {"id": "project-all-refresh-generated", "title": "先刷新全部 generated", "refresh_generated": True},
            "refresh",
            [],
        )
        fake_terminal = object()

        with patch.object(tui_module, "_run_menu_selection", return_value=refresh_entry):
            with patch.object(tui_module, "run_confirmation_screen", return_value=True):
                argv = tui_module.run_generate_project_all_flow("windows", terminal=fake_terminal)

        self.assertEqual(["generate", "project", "all", "--host", "windows", "--refresh-generated"], argv)

    def test_run_project_subject_flow_collects_specific_matrix_variant_and_refresh(self) -> None:
        tui_module = load_tui_module()
        fake_terminal = object()
        subject_record = make_project_subject_record()
        matrix_id = str(dict(subject_record["manifest"])["defaultMatrix"])

        selections = [
            tui_module.MenuEntry(
                "Subjects",
                {"id": "subject-option", "title": str(subject_record["subject_id"]), "subject_id": str(subject_record["subject_id"])},
                str(subject_record["subject_id"]),
                [],
            ),
            tui_module.MenuEntry("范围", {"id": "scope-specific", "title": "指定目标", "scope": "specific"}, "specific", []),
            tui_module.MenuEntry("Matrices", {"id": f"matrix-option:{matrix_id}", "title": matrix_id, "matrix_id": matrix_id}, matrix_id, []),
            tui_module.MenuEntry("Variant", {"id": "variant-profile", "title": "PROFILE", "variant": "PROFILE"}, "PROFILE", []),
            tui_module.MenuEntry("Generated", {"id": "refresh-generated", "title": "刷新 generated", "refresh_generated": True}, "refresh", []),
            tui_module.MenuEntry("Open", {"id": "open-generated-native", "title": "打开 generated native project", "open_native_target": "generated"}, "generated", []),
        ]

        with patch.object(tui_module, "_subject_records_for_host", return_value=[subject_record]):
            with patch.object(tui_module, "_run_menu_selection", side_effect=selections):
                with patch.object(tui_module, "run_confirmation_screen", return_value=True):
                    argv = tui_module.run_project_subject_flow("generate-project-subject", "windows", terminal=fake_terminal)

        self.assertEqual(
            [
                "generate",
                "project",
                "subject",
                "--id",
                f"subject/{subject_record['subject_id']}",
                "--matrix",
                matrix_id,
                "--variant",
                "PROFILE",
                "--refresh-generated",
                "--open-native-target",
                "generated",
            ],
            argv,
        )

    def test_run_project_subject_flow_build_collects_generated_native_target(self) -> None:
        tui_module = load_tui_module()
        fake_terminal = object()
        subject_record = make_project_subject_record(subject_id="FixtureProjectBuildSubject")
        matrix_id = str(dict(subject_record["manifest"])["defaultMatrix"])

        selections = [
            tui_module.MenuEntry(
                "Subjects",
                {"id": "subject-option", "title": str(subject_record["subject_id"]), "subject_id": str(subject_record["subject_id"])},
                str(subject_record["subject_id"]),
                [],
            ),
            tui_module.MenuEntry("范围", {"id": "scope-specific", "title": "指定目标", "scope": "specific"}, "specific", []),
            tui_module.MenuEntry("Matrices", {"id": f"matrix-option:{matrix_id}", "title": matrix_id, "matrix_id": matrix_id}, matrix_id, []),
            tui_module.MenuEntry("Build", {"id": "build-generated-native", "title": "构建 generated native project", "native_target": "generated"}, "generated", []),
        ]

        with patch.object(tui_module, "_subject_records_for_host", return_value=[subject_record]):
            with patch.object(tui_module, "_run_menu_selection", side_effect=selections):
                with patch.object(tui_module, "run_confirmation_screen", return_value=True):
                    argv = tui_module.run_project_subject_flow("build-project-subject", "windows", terminal=fake_terminal)

        self.assertEqual(
            [
                "build",
                "project",
                "subject",
                "--id",
                f"subject/{subject_record['subject_id']}",
                "--matrix",
                matrix_id,
                "--native-target",
                "generated",
            ],
            argv,
        )

    def test_run_project_core_flow_collects_specific_target(self) -> None:
        tui_module = load_tui_module()
        fake_terminal = object()
        selections = [
            tui_module.MenuEntry("范围", {"id": "scope-specific", "title": "指定目标", "scope": "specific"}, "specific", []),
            tui_module.MenuEntry("Targets", {"id": "target-option", "title": "linux-x64", "target_id": "linux-x64"}, "linux-x64", []),
        ]

        with patch.object(tui_module, "_run_menu_selection", side_effect=selections):
            with patch.object(tui_module, "run_confirmation_screen", return_value=True):
                argv = tui_module.run_project_core_flow("build-project-core", "windows", terminal=fake_terminal)

        self.assertEqual(["build", "project", "core", "--host", "windows", "--target", "linux-x64"], argv)
