from tests.tooling.run.tui_test_support import *


class TestTuiUnifiedSections(TuiUnifiedMenuTestSupport):
    def test_build_menu_entries_only_includes_primary_sections(self) -> None:
        manifest_module = load_manifest_module()
        tui_module = load_tui_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        entries = tui_module.build_menu_entries(manifest, "windows")

        self.assertEqual(["prepare", "project", "build", "test", "clean", "inspect"], [entry.syntax for entry in entries])
        self.assertEqual(
            ["prepare-menu", "project-menu", "build-menu", "test-menu", "clean-menu", "inspect-menu"],
            [entry.command["id"] for entry in entries],
        )
        self.assertTrue(all(entry.command["title"] for entry in entries))

    def test_build_prepare_menu_entries_exposes_android_host_bootstrap_on_windows(self) -> None:
        manifest_module = load_manifest_module()
        tui_module = load_tui_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        entries = tui_module.build_prepare_menu_entries(manifest, "windows")

        self.assertIn("prepare-android-host", [entry.command["id"] for entry in entries])
        android_entry = next(entry for entry in entries if entry.command["id"] == "prepare-android-host")
        self.assertEqual("android-host", android_entry.syntax)
        self.assertIn("Android", android_entry.command["title"])

    def test_build_project_menu_entries_exposes_generate_and_build_actions(self) -> None:
        manifest_module = load_manifest_module()
        tui_module = load_tui_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        entries = tui_module.build_project_menu_entries(manifest, "windows")

        self.assertEqual(
            [
                "generate-project-all",
                "generate-project-subject",
                "generate-project-core",
                "build-project-subject",
                "build-project-core",
                "menu-back",
            ],
            [entry.command["id"] for entry in entries],
        )
        self.assertEqual(
            ["all", "subject", "core", "subject-build", "core-build", "back"],
            [entry.syntax for entry in entries],
        )
        self.assertIn("生成所有 Solution", entries[0].command["title"])
        self.assertIn("Subject 调试工程", entries[1].command["title"])
        self.assertIn("IL2CPP Core 调试工程", entries[2].command["title"])

    def test_run_section_submenu_routes_project_menu_to_project_flow(self) -> None:
        tui_module = load_tui_module()
        manifest = {"groups": [], "commands": []}
        fake_terminal = object()

        with patch.object(
            tui_module,
            "run_project_submenu",
            return_value=["generate", "project", "all", "--host", "windows"],
        ) as run_project_submenu:
            argv = tui_module.run_section_submenu("project-menu", manifest, "windows", terminal=fake_terminal)

        self.assertEqual(["generate", "project", "all", "--host", "windows"], argv)
        run_project_submenu.assert_called_once_with(
            manifest,
            "windows",
            terminal=fake_terminal,
            menu_state=None,
        )

    def test_render_menu_screen_shows_project_detail_block(self) -> None:
        manifest_module = load_manifest_module()
        tui_module = load_tui_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        entries = tui_module.build_project_menu_entries(manifest, "windows")
        screen = tui_module.render_menu_screen(
            entries,
            0,
            title="工程 / IDE",
            help_text="方向键选择，Enter 继续，Back 返回上级，q/Esc 返回主菜单。",
            fullscreen=False,
        )

        self.assertIn("简介:", screen)
        self.assertIn("输出:", screen)
        self.assertIn("目标范围:", screen)
        self.assertIn("当前宿主支持:", screen)
        self.assertIn("执行命令:", screen)
        self.assertIn("备注:", screen)
        self.assertIn("solutions/manifest.json", screen)

    def test_build_inspect_menu_entries_points_to_registry_listing(self) -> None:
        manifest_module = load_manifest_module()
        tui_module = load_tui_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        entries = tui_module.build_inspect_menu_entries(manifest, "windows")

        self.assertEqual(
            ["help", "capability", "list", "test-registry-list", "menu-back"],
            [entry.command["id"] for entry in entries],
        )
        self.assertEqual(["help", "capability", "catalog", "tests", "back"], [entry.syntax for entry in entries])
