from tests.tooling.run.tui_test_support import *


class TestTuiLegacyRouting(LegacyTuiTestSupport):
    def test_resolve_entry_argv_supports_dynamic_capability_target(self) -> None:
        manifest_module = load_manifest_module()
        tui_module = load_tui_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        capability_entry = next(
            entry for entry in tui_module.build_inspect_menu_entries(manifest, "windows") if entry.command["id"] == "capability"
        )

        argv = tui_module.resolve_entry_argv(capability_entry, prompt_value_provider=lambda prompt: "bootstrap")

        self.assertEqual(["capability", "bootstrap"], argv)

    def test_resolve_entry_argv_supports_dynamic_test_suite_target(self) -> None:
        manifest_module = load_manifest_module()
        tui_module = load_tui_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)
        smoke_spec = select_public_suite_spec(
            "chaos_tui_dynamic_suite_target",
            host_platform="windows",
            family="smoke",
            required_stages=["all"],
        )

        test_entry = next(
            entry for entry in tui_module.build_menu_entries(manifest, "windows") if entry.syntax == "test"
        )

        answers = iter(["suite", f"{smoke_spec['family']} {smoke_spec['suite']}"])
        argv = tui_module.resolve_entry_argv(test_entry, prompt_value_provider=lambda prompt: next(answers))

        self.assertEqual(["test", str(smoke_spec["family"]), str(smoke_spec["suite"])], argv)

    def test_resolve_entry_argv_supports_dynamic_test_family_all_target(self) -> None:
        manifest_module = load_manifest_module()
        tui_module = load_tui_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        test_entry = next(
            entry for entry in tui_module.build_menu_entries(manifest, "windows") if entry.syntax == "test"
        )

        answers = iter(["family-all", "smoke"])
        argv = tui_module.resolve_entry_argv(test_entry, prompt_value_provider=lambda prompt: next(answers))

        self.assertEqual(["test", "smoke", "all"], argv)

    def test_resolve_entry_argv_supports_test_all_without_prompt(self) -> None:
        manifest_module = load_manifest_module()
        tui_module = load_tui_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        test_entry = next(
            entry for entry in tui_module.build_menu_entries(manifest, "windows") if entry.syntax == "test"
        )

        argv = tui_module.resolve_entry_argv(test_entry, prompt_value_provider=lambda prompt: "all")

        self.assertEqual(["test", "all"], argv)

    def test_resolve_entry_argv_supports_test_list_from_unified_test_menu(self) -> None:
        manifest_module = load_manifest_module()
        tui_module = load_tui_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        test_entry = next(
            entry for entry in tui_module.build_menu_entries(manifest, "windows") if entry.syntax == "test"
        )

        answers = iter(["list", "smoke"])
        argv = tui_module.resolve_entry_argv(test_entry, prompt_value_provider=lambda prompt: next(answers))

        self.assertEqual(["test", "list", "smoke"], argv)

    def test_resolve_entry_argv_supports_test_watch_from_unified_test_menu(self) -> None:
        manifest_module = load_manifest_module()
        tui_module = load_tui_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        test_entry = next(
            entry for entry in tui_module.build_menu_entries(manifest, "windows") if entry.syntax == "test"
        )

        argv = tui_module.resolve_entry_argv(test_entry, prompt_value_provider=lambda prompt: "watch")

        self.assertEqual(["test", "watch"], argv)

    def test_resolve_entry_argv_supports_test_summary_from_unified_test_menu(self) -> None:
        manifest_module = load_manifest_module()
        tui_module = load_tui_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        test_entry = next(
            entry for entry in tui_module.build_menu_entries(manifest, "windows") if entry.syntax == "test"
        )

        argv = tui_module.resolve_entry_argv(test_entry, prompt_value_provider=lambda prompt: "summary")

        self.assertEqual(["test", "summary"], argv)

    def test_run_fullscreen_menu_routes_test_entry_into_second_level_menu(self) -> None:
        tui_module = load_tui_module()
        test_entry = tui_module.MenuEntry(
            group_title="Validate",
            command={"id": "test-menu", "title": "Open the unified test center"},
            syntax="test",
            argv=["test"],
        )

        class FakeTerminal:
            def __init__(self) -> None:
                self.screens: list[str] = []
                self.keys = iter(["enter"])

            def __enter__(self):
                return self

            def __exit__(self, exc_type, exc, tb) -> None:
                return None

            def render(self, screen: str) -> None:
                self.screens.append(screen)

            def read_key(self) -> str:
                return next(self.keys)

        fake_terminal = FakeTerminal()

        with patch.object(tui_module, "build_menu_entries", return_value=[test_entry]):
            with patch.object(tui_module, "_TerminalSession", return_value=fake_terminal):
                with patch.object(tui_module, "run_test_submenu", return_value=["test", "all"]) as run_test_submenu:
                    argv = tui_module.run_fullscreen_menu({"groups": [], "commands": []}, "windows")

        self.assertEqual(["test", "all"], argv)
        run_test_submenu.assert_called_once()
        self.assertTrue(any("工作区控制中心" in screen for screen in fake_terminal.screens))

    def test_run_fullscreen_menu_routes_prepare_entry_into_second_level_menu(self) -> None:
        tui_module = load_tui_module()
        prepare_entry = tui_module.MenuEntry(
            group_title="Start",
            command={"id": "prepare-menu", "title": "Check and prepare the local host environment"},
            syntax="prepare",
            argv=["prepare"],
        )

        class FakeTerminal:
            def __init__(self) -> None:
                self.screens: list[str] = []
                self.keys = iter(["enter"])

            def __enter__(self):
                return self

            def __exit__(self, exc_type, exc, tb) -> None:
                return None

            def render(self, screen: str) -> None:
                self.screens.append(screen)

            def read_key(self) -> str:
                return next(self.keys)

        fake_terminal = FakeTerminal()

        with patch.object(tui_module, "build_menu_entries", return_value=[prepare_entry]):
            with patch.object(tui_module, "_TerminalSession", return_value=fake_terminal):
                with patch.object(tui_module, "run_section_submenu", return_value=["doctor"]) as run_section_submenu:
                    argv = tui_module.run_fullscreen_menu({"groups": [], "commands": []}, "windows")

        self.assertEqual(["doctor"], argv)
        run_section_submenu.assert_called_once_with("prepare-menu", {"groups": [], "commands": []}, "windows", terminal=fake_terminal)

    def test_run_inline_menu_reopens_last_selected_section_and_item(self) -> None:
        tui_module = load_tui_module()
        test_entry = tui_module.MenuEntry(
            group_title="Validate",
            command={"id": "test-menu", "title": "Open the unified test center"},
            syntax="test",
            argv=["test"],
        )

        class FakeTerminal:
            def __init__(self) -> None:
                self.screens: list[str] = []
                self.keys = iter([])

            def __enter__(self):
                return self

            def __exit__(self, exc_type, exc, tb) -> None:
                return None

            def render(self, screen: str) -> None:
                self.screens.append(screen)

            def read_key(self) -> str:
                return next(self.keys)

        fake_terminal = FakeTerminal()
        menu_state = tui_module.MenuState(
            primary_command_id="test-menu",
            active_section_command_id="test-menu",
            section_selection_command_id="test-all",
        )

        with patch.object(tui_module, "build_menu_entries", return_value=[test_entry]):
            with patch.object(tui_module, "_InlineTerminalSession", return_value=fake_terminal):
                with patch.object(tui_module, "run_test_submenu", return_value=["test", "all"]) as run_test_submenu:
                    argv = tui_module.run_inline_menu({"groups": [], "commands": []}, "windows", menu_state=menu_state)

        self.assertEqual(["test", "all"], argv)
        run_test_submenu.assert_called_once_with(
            {"groups": [], "commands": []},
            "windows",
            terminal=fake_terminal,
            menu_state=menu_state,
        )
        self.assertEqual([], fake_terminal.screens)
