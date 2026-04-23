from tests.tooling.run.tui_test_support import *


class TestTuiLegacyMenuEntries(LegacyTuiTestSupport):
    def test_build_menu_entries_only_includes_visible_commands_for_host(self) -> None:
        manifest_module = load_manifest_module()
        tui_module = load_tui_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        entries = tui_module.build_menu_entries(manifest, "windows")
        syntaxes = [entry.syntax for entry in entries]

        self.assertEqual(["prepare", "build", "test", "clean", "inspect"], syntaxes)
        self.assertEqual(
            ["prepare-menu", "build-menu", "test-menu", "clean-menu", "inspect-menu"],
            [entry.command["id"] for entry in entries],
        )
        self.assertEqual(["环境准备", "构建产物", "测试验证", "清理维护", "信息查看"], [entry.group_title for entry in entries])
        self.assertIn("工作区", entries[0].command["title"])
        self.assertIn("运行时契约", entries[1].command["title"])
        self.assertIn("统一测试入口", entries[2].command["title"])
        self.assertIn("缓存构建结果", entries[3].command["title"])
        self.assertIn("公开测试目录", entries[4].command["title"])

    def test_jump_group_moves_between_neighbor_groups(self) -> None:
        manifest_module = load_manifest_module()
        tui_module = load_tui_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        entries = tui_module.build_menu_entries(manifest, "windows")
        prepare_index = next(index for index, entry in enumerate(entries) if entry.syntax == "prepare")

        build_index = tui_module.jump_group(entries, prepare_index, direction=1)
        test_and_verify_index = tui_module.jump_group(entries, build_index, direction=1)
        build_index_again = tui_module.jump_group(entries, test_and_verify_index, direction=-1)

        self.assertEqual("构建产物", entries[build_index].group_title)
        self.assertEqual("测试验证", entries[test_and_verify_index].group_title)
        self.assertEqual(build_index, build_index_again)

    def test_build_prepare_menu_entries_provides_curated_second_level_menu(self) -> None:
        manifest_module = load_manifest_module()
        tui_module = load_tui_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        entries = tui_module.build_prepare_menu_entries(manifest, "windows")

        self.assertEqual(
            ["doctor", "prepare", "prepare-smoke", "prepare-android-host", "prepare-workflow-runtime-baseline-windows", "menu-back"],
            [entry.command["id"] for entry in entries],
        )
        self.assertEqual(["基础准备", "基础准备", "场景准备", "场景准备", "返回上级"], [entry.group_title for entry in entries])
        self.assertEqual(["doctor", "setup", "smoke", "android-host", "runtime-baseline", "back"], [entry.syntax for entry in entries])
        self.assertIn("检查并初始化", entries[1].command["title"])
        self.assertEqual("back", entries[-1].syntax)

    def test_build_clean_menu_entries_provides_curated_second_level_menu(self) -> None:
        manifest_module = load_manifest_module()
        tui_module = load_tui_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        entries = tui_module.build_clean_menu_entries(manifest, "windows")

        self.assertEqual(["all", "smoke", "android-host", "runtime-baseline", "back"], [entry.syntax for entry in entries])
        self.assertIn("全部托管输出", entries[0].command["title"])
        self.assertEqual("menu-back", entries[-1].command["id"])

    def test_build_inspect_menu_entries_provides_curated_second_level_menu(self) -> None:
        manifest_module = load_manifest_module()
        tui_module = load_tui_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        entries = tui_module.build_inspect_menu_entries(manifest, "windows")

        self.assertEqual(
            ["help", "capability", "list", "test-list", "menu-back"],
            [entry.command["id"] for entry in entries],
        )
        self.assertEqual(["help", "capability", "catalog", "tests", "back"], [entry.syntax for entry in entries])
        self.assertIn("公开测试套件", entries[3].command["title"])

    def test_build_test_menu_entries_provides_curated_second_level_menu(self) -> None:
        manifest_module = load_manifest_module()
        tui_module = load_tui_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        entries = tui_module.build_test_menu_entries(manifest, "windows")

        self.assertEqual(
            ["test-all", "test-family-all", "test-family-suite", "test-watch", "test-summary", "test-list", "menu-back"],
            [entry.command["id"] for entry in entries],
        )
        self.assertEqual(["快速开始", "按范围执行", "按范围执行", "查看结果", "查看结果", "查看结果", "返回上级"], [entry.group_title for entry in entries])
        self.assertIn("默认测试矩阵", next(entry.command["title"] for entry in entries if entry.command["id"] == "test-all"))
        self.assertIn("事件时间线", next(entry.command["title"] for entry in entries if entry.command["id"] == "test-watch"))
        self.assertEqual("back", entries[-1].syntax)
        self.assertIn("返回上一级", entries[-1].command["title"])

    def test_menu_help_texts_clarify_navigation_expectations(self) -> None:
        tui_module = load_tui_module()

        self.assertIn("方向键移动", tui_module.PRIMARY_MENU_HELP)
        self.assertIn("q/Esc 退出", tui_module.PRIMARY_MENU_HELP)
        self.assertIn("方向键选择", tui_module.TEST_MENU_HELP)
        self.assertIn("Back 返回上级", tui_module.TEST_MENU_HELP)
        self.assertIn("q/Esc 返回主菜单", tui_module.TEST_MENU_HELP)
        self.assertIn("方向键选择", tui_module.SECTION_MENU_HELP)
        self.assertIn("Back 返回上级", tui_module.SECTION_MENU_HELP)
        self.assertIn("q/Esc 返回主菜单", tui_module.SECTION_MENU_HELP)

    def test_build_build_menu_entries_provides_curated_second_level_menu(self) -> None:
        manifest_module = load_manifest_module()
        tui_module = load_tui_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        entries = tui_module.build_build_menu_entries(manifest, "windows")

        self.assertEqual(
            [
                "build-all",
                "build-native-contract-abi",
                "build-native-contract-bridge",
                "build-preset-windows-x64-reference",
                "build-platform-android-arm64-smoke",
                "build-platform-linux-x64-packaging",
                "menu-back",
            ],
            [entry.command["id"] for entry in entries],
        )
        self.assertEqual(["all", "abi", "bridge", "windows", "android", "linux", "back"], [entry.syntax for entry in entries])
        self.assertIn("全量构建", entries[0].command["title"])
        self.assertIn("Windows", entries[3].command["title"])
        self.assertEqual("校验 Android 启动 smoke 路由", entries[4].command["title"])
        self.assertEqual("校验 Linux 打包路由", entries[5].command["title"])
