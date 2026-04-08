from __future__ import annotations

import os
import shutil
import sys
import unicodedata
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Callable

if __package__ in (None, ""):
    sys.path.insert(0, str(Path(__file__).resolve().parent))
    import manifest as manifest_module
else:
    from . import manifest as manifest_module

if os.name == "nt":
    import ctypes
    import msvcrt
else:
    import select
    import termios
    import tty


_WINDOWS_VT_READY: bool | None = None
ANSI_GREEN = "\x1b[32m"
ANSI_BOLD_YELLOW = "\x1b[1;33m"
ANSI_BOLD_CYAN = "\x1b[1;36m"
ANSI_BRIGHT_WHITE = "\x1b[97m"
ANSI_BOLD_BRIGHT_WHITE = "\x1b[1;97m"
ANSI_BRIGHT_BLACK = "\x1b[90m"
ANSI_RESET = "\x1b[0m"


@dataclass(frozen=True)
class MenuEntry:
    group_title: str
    command: dict[str, Any]
    syntax: str
    argv: list[str]


@dataclass
class MenuState:
    primary_command_id: str | None = None
    active_section_command_id: str | None = None
    section_selection_command_id: str | None = None


TEST_MENU_COMMAND_IDS = {
    "test-all",
    "test-suite",
    "test-subject",
    "test-module",
    "test-system",
    "test-pipeline",
    "test-registry-list",
    "test-registry-refresh",
    "test-registry-check-consistency",
    "test-watch",
    "test-summary",
}
PRIMARY_MENU_ENTRIES = [
    {
        "id": "prepare-menu",
        "group_title": "环境准备",
        "syntax": "prepare",
        "title": "准备当前工作区与本机环境，先把运行基础打通",
    },
    {
        "id": "build-menu",
        "group_title": "构建产物",
        "syntax": "build",
        "title": "构建运行时契约、主机预设与平台校验产物",
    },
    {
        "id": "test-menu",
        "group_title": "测试验证",
        "syntax": "test",
        "title": "进入统一测试入口，查看进度、摘要与测试结果",
    },
    {
        "id": "clean-menu",
        "group_title": "清理维护",
        "syntax": "clean",
        "title": "清理托管输出、临时产物与缓存构建结果",
    },
    {
        "id": "inspect-menu",
        "group_title": "信息查看",
        "syntax": "inspect",
        "title": "查看帮助、能力说明与公开测试目录",
    },
]
MENU_BACK_COMMAND = {"id": "menu-back", "title": "返回主菜单"}
PRIMARY_MENU_HELP = "方向键移动，Enter 执行，PgUp/PgDn 切换分组，Home/End 跳转，q/Esc 退出。"
TEST_MENU_HELP = "方向键选择，Enter 继续，Back 返回上级，q/Esc 返回主菜单。"
SECTION_MENU_HELP = "方向键选择，Enter 继续，Back 返回上级，q/Esc 返回主菜单。"


@dataclass(frozen=True)
class TestProgressView:
    command: str
    progress_text: str
    final_status: str
    history_lines: list[str]
    phase_lines: list[str]
    subject_lines: list[str]
    important_lines: list[str]
    artifact_lines: list[str]
    artifact_count: int
    errors: list[str]


@dataclass(frozen=True)
class OperationProgressView:
    command: str
    progress_text: str
    final_status: str
    history_lines: list[str]
    important_lines: list[str]
    artifact_lines: list[str]
    artifact_count: int
    errors: list[str]


def build_menu_entries(manifest: dict[str, Any], host_platform: str) -> list[MenuEntry]:
    del manifest
    del host_platform
    return [
        MenuEntry(
            group_title=item["group_title"],
            command={"id": item["id"], "title": item["title"]},
            syntax=item["syntax"],
            argv=[item["syntax"]],
        )
        for item in PRIMARY_MENU_ENTRIES
    ]


def build_prepare_menu_entries(manifest: dict[str, Any], host_platform: str) -> list[MenuEntry]:
    return _build_curated_submenu_entries(
        manifest,
        host_platform,
        [
            ("基础准备", "doctor", "doctor", "只检查当前本机工具链与运行条件"),
            ("基础准备", "prepare", "setup", "检查并初始化当前主机环境（推荐）"),
            ("场景准备", "prepare-smoke", "smoke", "准备 smoke 测试所需的托管运行环境"),
            (
                "场景准备",
                f"prepare-workflow-runtime-baseline-{host_platform}",
                "runtime-baseline",
                "准备当前主机的 runtime-baseline 工作流环境",
            ),
        ],
    )


def build_clean_menu_entries(manifest: dict[str, Any], host_platform: str) -> list[MenuEntry]:
    return _build_curated_submenu_entries(
        manifest,
        host_platform,
        [
            ("清理范围", "clean", "all", "清理统一入口产生的全部托管输出与缓存产物"),
            ("清理范围", "clean-smoke", "smoke", "只清理 smoke 测试相关输出"),
            ("清理范围", f"clean-workflow-runtime-baseline-{host_platform}", "runtime-baseline", "清理当前主机的 runtime-baseline 工作流输出"),
        ],
    )


def build_build_menu_entries(manifest: dict[str, Any], host_platform: str) -> list[MenuEntry]:
    preset_by_host = {
        "windows": ("build-preset-windows-x64-reference", "windows", "构建当前主机的 Windows 参考预设"),
        "macos": ("build-preset-macos-reference", "macos", "构建当前主机的 macOS 参考预设"),
    }
    preset_command_id, preset_syntax, preset_title = preset_by_host.get(
        host_platform,
        (None, "reference", "构建当前主机参考预设"),
    )
    return _build_curated_submenu_entries(
        manifest,
        host_platform,
        [
            ("快速构建", "build-all", "all", "执行当前主机推荐的全量构建批次"),
            ("契约构建", "build-native-contract-abi", "abi", "构建原生 ABI 契约 smoke 目标"),
            ("契约构建", "build-native-contract-bridge", "bridge", "构建原生 bridge 契约 smoke 目标"),
            ("主机参考", preset_command_id, preset_syntax, preset_title),
            ("平台校验", "build-platform-android-arm64-smoke", "android", "校验 Android 启动 smoke 路由"),
            ("平台校验", "build-platform-ios-arm64-packaging", "ios", "校验 iOS 打包路由"),
            ("平台校验", "build-platform-linux-x64-packaging", "linux", "校验 Linux 打包路由"),
        ],
    )


def build_inspect_menu_entries(manifest: dict[str, Any], host_platform: str) -> list[MenuEntry]:
    return _build_curated_submenu_entries(
        manifest,
        host_platform,
        [
            ("帮助参考", "help", "help", "查看统一入口帮助与常用命令语法"),
            ("帮助参考", "capability", "capability", "查看单个能力的详细说明"),
            ("帮助参考", "list", "catalog", "浏览当前可用能力目录"),
            ("测试目录", "test-list", "tests", "浏览当前主机可用的公开测试套件"),
        ],
    )


def build_test_menu_entries(manifest: dict[str, Any], host_platform: str) -> list[MenuEntry]:
    del manifest
    del host_platform
    return [
        MenuEntry(
            group_title="快速开始",
            command={"id": "test-all", "title": "运行默认测试矩阵，适合先做一轮完整健康检查"},
            syntax="all",
            argv=["test", "all"],
        ),
        MenuEntry(
            group_title="按范围执行",
            command={"id": "test-family-all", "title": "按测试族批量执行，适合做一轮定向回归"},
            syntax="family",
            argv=["test"],
        ),
        MenuEntry(
            group_title="按范围执行",
            command={"id": "test-family-suite", "title": "只运行一个测试套件，适合快速验证单点改动"},
            syntax="suite",
            argv=["test"],
        ),
        MenuEntry(
            group_title="查看结果",
            command={"id": "test-watch", "title": "查看当前或最近一次运行的事件时间线"},
            syntax="watch",
            argv=["test", "watch"],
        ),
        MenuEntry(
            group_title="查看结果",
            command={"id": "test-summary", "title": "查看最近一次聚合摘要和套件结果"},
            syntax="summary",
            argv=["test", "summary"],
        ),
        MenuEntry(
            group_title="查看结果",
            command={"id": "test-list", "title": "浏览当前主机可用的公开测试套件"},
            syntax="list",
            argv=["test", "list"],
        ),
        MenuEntry(
            group_title="返回上级",
            command={**MENU_BACK_COMMAND, "title": "返回上一级菜单"},
            syntax="back",
            argv=[],
        ),
    ]


def _build_submenu_entries(
    manifest: dict[str, Any],
    host_platform: str,
    ordered_ids: list[tuple[str, str | None]],
) -> list[MenuEntry]:
    entries: list[MenuEntry] = []
    for group_title, command_id in ordered_ids:
        if not command_id:
            continue
        command = manifest_module.find_command(manifest, command_id, host_platform)
        if command is None or not command.get("show_in_menu", True):
            continue
        entries.append(
            MenuEntry(
                group_title=group_title,
                command=command,
                syntax=manifest_module.command_syntax(command),
                argv=manifest_module.command_argv(command),
            )
        )
    entries.append(
        MenuEntry(
            group_title="返回上级",
            command=dict(MENU_BACK_COMMAND),
            syntax="back",
            argv=[],
        )
    )
    return entries


def _build_curated_submenu_entries(
    manifest: dict[str, Any],
    host_platform: str,
    ordered_items: list[tuple[str, str | None, str, str]],
) -> list[MenuEntry]:
    entries: list[MenuEntry] = []
    for group_title, command_id, syntax, title in ordered_items:
        if not command_id:
            continue
        command = manifest_module.find_command(manifest, command_id, host_platform)
        if command is None or not command.get("show_in_menu", True):
            continue
        entries.append(
            MenuEntry(
                group_title=group_title,
                command={**command, "title": title},
                syntax=syntax,
                argv=manifest_module.command_argv(command),
            )
        )
    entries.append(
        MenuEntry(
            group_title="返回上级",
            command=dict(MENU_BACK_COMMAND),
            syntax="back",
            argv=[],
        )
    )
    return entries


def jump_group(entries: list[MenuEntry], current_index: int, direction: int) -> int:
    if not entries:
        return 0

    starts = [0]
    for index in range(1, len(entries)):
        if entries[index].group_title != entries[index - 1].group_title:
            starts.append(index)

    if direction > 0:
        for index in starts:
            if index > current_index:
                return index
        return starts[-1]

    current_group_start = max(index for index in starts if index <= current_index)
    if current_group_start < current_index:
        return current_group_start

    previous = [index for index in starts if index < current_group_start]
    return previous[-1] if previous else starts[0]


def resolve_entry_argv(
    entry: MenuEntry,
    prompt_value_provider: Callable[[str], str] | None = None,
) -> list[str] | None:
    prompt_value_provider = prompt_value_provider or input
    command_id = entry.command["id"]

    if command_id == "capability":
        value = prompt_value_provider("输入 capability id，留空取消：").strip()
        if not value:
            return None
        return ["capability", value]

    if command_id == "test-menu":
        mode = prompt_value_provider("测试模式（suite/family-all/all/list/watch/summary），留空取消：").strip().lower()
        if not mode:
            return None
        if mode == "suite":
            value = prompt_value_provider("输入测试族和套件名，例如：smoke HelloWorld：").strip()
            parts = value.split()
            if len(parts) != 2:
                return None
            return ["test", parts[0], parts[1]]
        if mode == "family-all":
            value = prompt_value_provider("输入测试族，留空取消：").strip()
            if not value:
                return None
            return ["test", value, "all"]
        if mode == "all":
            return ["test", "all"]
        if mode == "list":
            value = prompt_value_provider("可选输入测试族，留空表示全部：").strip()
            return ["test", "list", value] if value else ["test", "list"]
        if mode == "watch":
            return ["test", "watch"]
        if mode == "summary":
            return ["test", "summary"]
        return None

    if command_id == "test-family-suite":
        value = prompt_value_provider("输入测试族和套件名，例如：smoke HelloWorld：").strip()
        parts = value.split()
        if len(parts) != 2:
            return None
        return ["test", parts[0], parts[1]]

    if command_id == "test-family-all":
        value = prompt_value_provider("输入测试族，留空取消：").strip()
        if not value:
            return None
        return ["test", value, "all"]

    if command_id == "test-all":
        return ["test", "all"]

    if command_id == "test-list":
        value = prompt_value_provider("可选输入测试族，留空表示全部：").strip()
        return ["test", "list", value] if value else ["test", "list"]

    return list(entry.argv)


def build_inspect_menu_entries(manifest: dict[str, Any], host_platform: str) -> list[MenuEntry]:
    return _build_curated_submenu_entries(
        manifest,
        host_platform,
        [
            ("Inspect", "help", "help", "Show unified entrypoint help and common command syntax"),
            ("Inspect", "capability", "capability", "Show details for one capability"),
            ("Inspect", "list", "catalog", "Browse the visible capability catalog"),
            ("Tests", "test-registry-list", "tests", "Browse the current host's registered test objects"),
        ],
    )


def build_test_menu_entries(manifest: dict[str, Any], host_platform: str) -> list[MenuEntry]:
    del manifest
    del host_platform
    return [
        MenuEntry("Quick Start", {"id": "test-all", "title": "Run the default unified test matrix"}, "all", ["test", "all"]),
        MenuEntry("Selectors", {"id": "test-suite", "title": "Run a suite object"}, "suite", ["test"]),
        MenuEntry("Selectors", {"id": "test-subject", "title": "Run a subject object"}, "subject", ["test"]),
        MenuEntry("Selectors", {"id": "test-module", "title": "Run a module verification object"}, "module", ["test"]),
        MenuEntry("Selectors", {"id": "test-system", "title": "Run a system validation object"}, "system", ["test"]),
        MenuEntry("Selectors", {"id": "test-pipeline", "title": "Run a test pipeline object"}, "pipeline", ["test"]),
        MenuEntry("Registry", {"id": "test-registry-list", "title": "Browse registered test objects"}, "registry-list", ["test", "registry", "list"]),
        MenuEntry("Registry", {"id": "test-registry-refresh", "title": "Refresh registry snapshots"}, "registry-refresh", ["test", "registry", "refresh"]),
        MenuEntry("Registry", {"id": "test-registry-check-consistency", "title": "Check registry/wiki consistency"}, "registry-check", ["test", "registry", "check-consistency"]),
        MenuEntry("Results", {"id": "test-watch", "title": "Show the latest test event timeline"}, "watch", ["test", "watch"]),
        MenuEntry("Results", {"id": "test-summary", "title": "Show the latest aggregated summary"}, "summary", ["test", "summary"]),
        MenuEntry("Back", {**MENU_BACK_COMMAND, "title": "Back to previous menu"}, "back", []),
    ]


def resolve_entry_argv(
    entry: MenuEntry,
    prompt_value_provider: Callable[[str], str] | None = None,
) -> list[str] | None:
    prompt_value_provider = prompt_value_provider or input
    command_id = entry.command["id"]

    if command_id == "capability":
        value = prompt_value_provider("Enter capability id, leave blank to cancel: ").strip()
        if not value:
            return None
        return ["capability", value]

    if command_id == "test-menu":
        mode = prompt_value_provider(
            "Test mode (suite/subject/module/system/pipeline/all/registry-list/registry-refresh/registry-check/watch/summary), leave blank to cancel: "
        ).strip().lower()
        if not mode:
            return None
        if mode == "suite":
            value = prompt_value_provider("Enter family and suite, for example: smoke HelloWorld: ").strip()
            parts = value.split()
            if len(parts) != 2:
                return None
            return ["test", "suite", "--family", parts[0], "--suite", parts[1]]
        if mode == "subject":
            value = prompt_value_provider("Enter subject id, for example: HelloWorldObject: ").strip()
            if not value:
                return None
            return ["test", "subject", "--subject", value]
        if mode == "module":
            value = prompt_value_provider("Enter module and profile, for example: managed-smoke basic: ").strip()
            parts = value.split()
            if len(parts) != 2:
                return None
            return ["test", "module", "--module", parts[0], "--profile", parts[1]]
        if mode == "system":
            value = prompt_value_provider("Enter system scenario id, for example: hosted-runtime-smoke: ").strip()
            if not value:
                return None
            return ["test", "system", "--scenario", value]
        if mode == "pipeline":
            value = prompt_value_provider("Enter pipeline id, for example: completion-runtime-core: ").strip()
            if not value:
                return None
            return ["test", "pipeline", "--pipeline", value]
        if mode == "all":
            return ["test", "all"]
        if mode == "registry-list":
            return ["test", "registry", "list"]
        if mode == "registry-refresh":
            return ["test", "registry", "refresh"]
        if mode == "registry-check":
            return ["test", "registry", "check-consistency"]
        if mode == "watch":
            return ["test", "watch"]
        if mode == "summary":
            return ["test", "summary"]
        return None

    if command_id == "test-suite":
        value = prompt_value_provider("Enter family and suite, for example: smoke HelloWorld: ").strip()
        parts = value.split()
        if len(parts) != 2:
            return None
        return ["test", "suite", "--family", parts[0], "--suite", parts[1]]

    if command_id == "test-subject":
        value = prompt_value_provider("Enter subject id, for example: HelloWorldObject: ").strip()
        if not value:
            return None
        return ["test", "subject", "--subject", value]

    if command_id == "test-module":
        value = prompt_value_provider("Enter module and profile, for example: managed-smoke basic: ").strip()
        parts = value.split()
        if len(parts) != 2:
            return None
        return ["test", "module", "--module", parts[0], "--profile", parts[1]]

    if command_id == "test-system":
        value = prompt_value_provider("Enter system scenario id, for example: hosted-runtime-smoke: ").strip()
        if not value:
            return None
        return ["test", "system", "--scenario", value]

    if command_id == "test-pipeline":
        value = prompt_value_provider("Enter pipeline id, for example: completion-runtime-core: ").strip()
        if not value:
            return None
        return ["test", "pipeline", "--pipeline", value]

    if command_id == "test-all":
        return ["test", "all"]
    if command_id == "test-registry-list":
        return ["test", "registry", "list"]
    if command_id == "test-registry-refresh":
        return ["test", "registry", "refresh"]
    if command_id == "test-registry-check-consistency":
        return ["test", "registry", "check-consistency"]
    if command_id == "test-watch":
        return ["test", "watch"]
    if command_id == "test-summary":
        return ["test", "summary"]

    return list(entry.argv)


def supports_fullscreen_tui() -> bool:
    if not _stream_is_tty("stdin") or not _stream_is_tty("stdout"):
        return False

    return _enable_virtual_terminal_output()


def run_fullscreen_menu(
    manifest: dict[str, Any],
    host_platform: str,
    *,
    menu_state: MenuState | None = None,
) -> list[str] | None:
    entries = build_menu_entries(manifest, host_platform)
    if not entries:
        return None

    with _TerminalSession() as terminal:
        return _run_primary_menu(manifest, host_platform, terminal, menu_state=menu_state)


def run_inline_menu(
    manifest: dict[str, Any],
    host_platform: str,
    *,
    menu_state: MenuState | None = None,
) -> list[str] | None:
    entries = build_menu_entries(manifest, host_platform)
    if not entries:
        return None

    with _InlineTerminalSession() as terminal:
        return _run_primary_menu(manifest, host_platform, terminal, menu_state=menu_state)


def _run_primary_menu(
    manifest: dict[str, Any],
    host_platform: str,
    terminal: "_TerminalSession | _InlineTerminalSession",
    *,
    menu_state: MenuState | None = None,
) -> list[str] | None:
    entries = build_menu_entries(manifest, host_platform)
    if menu_state is not None and menu_state.active_section_command_id is not None:
        submenu_kwargs: dict[str, Any] = {"terminal": terminal}
        submenu_kwargs["menu_state"] = menu_state
        submenu_argv = run_section_submenu(
            menu_state.active_section_command_id,
            manifest,
            host_platform,
            **submenu_kwargs,
        )
        if submenu_argv is not None:
            return submenu_argv

    selection = _selection_index_for_command(entries, menu_state.primary_command_id if menu_state is not None else None)
    while True:
        selected_entry = _run_menu_selection(
            terminal,
            entries,
            title="工作区控制中心",
            help_text=PRIMARY_MENU_HELP,
            initial_selection=selection,
        )
        if selected_entry is None:
            return None
        selection = _selection_index_for_command(entries, selected_entry.command["id"])
        if menu_state is not None:
            menu_state.primary_command_id = selected_entry.command["id"]
        if not selected_entry.command["id"].endswith("-menu"):
            return resolve_entry_argv(selected_entry)

        submenu_kwargs = {"terminal": terminal}
        if menu_state is not None:
            submenu_kwargs["menu_state"] = menu_state
        submenu_argv = run_section_submenu(
            selected_entry.command["id"],
            manifest,
            host_platform,
            **submenu_kwargs,
        )
        if submenu_argv is not None:
            return submenu_argv


def run_section_submenu(
    section_command_id: str,
    manifest: dict[str, Any],
    host_platform: str,
    *,
    terminal: "_TerminalSession" | None = None,
    menu_state: MenuState | None = None,
) -> list[str] | None:
    if section_command_id == "prepare-menu":
        entries = build_prepare_menu_entries(manifest, host_platform)
        title = "准备中心"
        help_text = SECTION_MENU_HELP
    elif section_command_id == "build-menu":
        entries = build_build_menu_entries(manifest, host_platform)
        title = "构建中心"
        help_text = SECTION_MENU_HELP
    elif section_command_id == "test-menu":
        test_kwargs = {"terminal": terminal}
        if menu_state is not None:
            test_kwargs["menu_state"] = menu_state
        return run_test_submenu(manifest, host_platform, **test_kwargs)
    elif section_command_id == "clean-menu":
        entries = build_clean_menu_entries(manifest, host_platform)
        title = "清理中心"
        help_text = SECTION_MENU_HELP
    elif section_command_id == "inspect-menu":
        entries = build_inspect_menu_entries(manifest, host_platform)
        title = "查看中心"
        help_text = SECTION_MENU_HELP
    else:
        return None

    return _run_submenu(
        entries,
        title=title,
        help_text=help_text,
        terminal=terminal,
        section_command_id=section_command_id,
        menu_state=menu_state,
    )


def run_test_submenu(
    manifest: dict[str, Any],
    host_platform: str,
    *,
    terminal: "_TerminalSession" | None = None,
    menu_state: MenuState | None = None,
) -> list[str] | None:
    entries = build_test_menu_entries(manifest, host_platform)
    return _run_submenu(
        entries,
        title="统一测试入口",
        help_text=TEST_MENU_HELP,
        terminal=terminal,
        section_command_id="test-menu",
        menu_state=menu_state,
    )


def _run_submenu(
    entries: list[MenuEntry],
    *,
    title: str,
    help_text: str,
    terminal: "_TerminalSession" | None = None,
    section_command_id: str | None = None,
    menu_state: MenuState | None = None,
) -> list[str] | None:
    if not entries:
        return None

    initial_selection = 0
    if (
        menu_state is not None
        and section_command_id is not None
        and menu_state.active_section_command_id == section_command_id
    ):
        initial_selection = _selection_index_for_command(entries, menu_state.section_selection_command_id)

    if terminal is not None:
        selected_entry = _run_menu_selection(
            terminal,
            entries,
            title=title,
            help_text=help_text,
            initial_selection=initial_selection,
        )
        if selected_entry is None or selected_entry.command["id"] == "menu-back":
            return None
        if menu_state is not None:
            menu_state.primary_command_id = section_command_id
            menu_state.active_section_command_id = section_command_id
            menu_state.section_selection_command_id = selected_entry.command["id"]
        return resolve_entry_argv(selected_entry)

    with _TerminalSession() as session:
        selected_entry = _run_menu_selection(
            session,
            entries,
            title=title,
            help_text=help_text,
            initial_selection=initial_selection,
        )
    if selected_entry is None or selected_entry.command["id"] == "menu-back":
        return None
    if menu_state is not None:
        menu_state.primary_command_id = section_command_id
        menu_state.active_section_command_id = section_command_id
        menu_state.section_selection_command_id = selected_entry.command["id"]
    return resolve_entry_argv(selected_entry)


def _run_menu_selection(
    terminal: "_TerminalSession | _InlineTerminalSession",
    entries: list[MenuEntry],
    *,
    title: str,
    help_text: str,
    initial_selection: int = 0,
) -> MenuEntry | None:
    selection = max(0, min(initial_selection, len(entries) - 1))
    while True:
        terminal.render(
            render_menu_screen(
                entries,
                selection,
                title=title,
                help_text=help_text,
                fullscreen=getattr(terminal, "fullscreen", True),
            )
        )
        key = terminal.read_key()

        if key in {"escape", "quit"}:
            return None
        if key == "up":
            selection = max(0, selection - 1)
            continue
        if key == "down":
            selection = min(len(entries) - 1, selection + 1)
            continue
        if key == "home":
            selection = 0
            continue
        if key == "end":
            selection = len(entries) - 1
            continue
        if key == "page_up":
            selection = jump_group(entries, selection, direction=-1)
            continue
        if key == "page_down":
            selection = jump_group(entries, selection, direction=1)
            continue
        if key == "enter":
            return entries[selection]


def render_menu_screen(
    entries: list[MenuEntry],
    selection: int,
    *,
    title: str = "Unified Run Menu",
    help_text: str = PRIMARY_MENU_HELP,
    fullscreen: bool = True,
) -> str:
    width, height = shutil.get_terminal_size(fallback=(100, 30))
    rows = _build_rows(entries)
    selected_row = _find_selected_row(rows, selection)
    if fullscreen:
        visible_body_height = max(6, height - 5)
    else:
        visible_body_height = min(len(rows), max(6, min(14, height - 5)))
    scroll_top = _compute_scroll_top(selected_row, visible_body_height, len(rows))
    visible_rows = rows[scroll_top : scroll_top + visible_body_height]
    syntax_width = max(18, min(42, width // 3))

    header = [
        _style_menu_screen_title(_trim(title, width)),
        _style_menu_help(_trim(help_text, width)),
        _style_menu_counter(_trim(f"{selection + 1}/{len(entries)}", width)),
        "",
    ]
    if not fullscreen:
        header = _build_inline_workspace_header(width) + header

    body: list[str] = []
    for row in visible_rows:
        if row["entry_index"] is None:
            body.append(_style_group_heading(_trim(row["text"], width)))
            continue

        entry = entries[row["entry_index"]]
        prefix = "> " if row["entry_index"] == selection else "  "
        syntax = _pad(_trim(entry.syntax, max(10, syntax_width)), syntax_width)
        title = _trim(entry.command["title"], max(0, width - len(prefix) - syntax_width - 2))
        line = _trim(f"{prefix}{syntax}  {title}", width)
        if row["entry_index"] == selection:
            body.append(f"\x1b[1;7m{_pad(line, width)}\x1b[0m")
        else:
            body.append(f"{prefix}{_style_menu_syntax(syntax)}  {_style_menu_command_title(title)}")

    lines = header + body
    if fullscreen:
        while len(lines) < height:
            lines.append("")
        return "\x1b[2J\x1b[H" + "\n".join(lines[:height])
    return "\n".join(lines)


def render_test_progress_screen(events: list[dict[str, Any]], repo_root: Path | None = None) -> str:
    view = build_test_progress_view(events, repo_root=repo_root)
    lines = [
        "Unified Test Progress",
        f"Command: {view.command or '-'}",
        f"Status: {view.final_status}",
        f"Progress: {_green(view.progress_text)}",
        "",
        "Timeline:",
    ]
    lines.extend(view.history_lines or ["[  0%] waiting for test events"])

    if view.important_lines:
        lines.append("")
    if view.phase_lines:
        lines.append("Phases:")
        lines.extend(view.phase_lines)
    if view.subject_lines:
        if not view.phase_lines:
            lines.append("")
        lines.append("Subjects:")
        lines.extend(view.subject_lines)

    if view.important_lines:
        if not view.phase_lines and not view.subject_lines:
            lines.append("")
        else:
            lines.append("")
        lines.append(_highlight_heading("Important outputs:"))
        lines.extend(view.important_lines)

    if view.artifact_lines:
        lines.append("")
        lines.append(_highlight_label(f"Artifacts ({view.artifact_count}):"))
        lines.extend(view.artifact_lines)

    if view.errors:
        lines.append("")
        lines.append("Errors:")
        lines.extend(view.errors)

    return "\n".join(lines) + "\n"


def render_operation_progress_screen(events: list[dict[str, Any]], repo_root: Path | None = None) -> str:
    view = build_operation_progress_view(events, repo_root=repo_root)
    lines = [
        "Unified Run Progress",
        f"Command: {view.command or '-'}",
        f"Status: {view.final_status}",
        f"Progress: {_green(view.progress_text)}",
        "",
        "Timeline:",
    ]
    lines.extend(view.history_lines or ["[  0%] waiting for run events"])

    if view.important_lines:
        lines.append("")
        lines.append(_highlight_heading("Important outputs:"))
        lines.extend(view.important_lines)

    if view.artifact_lines:
        lines.append("")
        lines.append(_highlight_label(f"Artifacts ({view.artifact_count}):"))
        lines.extend(view.artifact_lines)

    if view.errors:
        lines.append("")
        lines.append("Errors:")
        lines.extend(view.errors)

    return "\n".join(lines) + "\n"


def build_test_progress_view(events: list[dict[str, Any]], repo_root: Path | None = None) -> TestProgressView:
    command = ""
    progress_text = "0%"
    final_status = "running"
    history_lines: list[str] = []
    phase_lines: list[str] = []
    subject_lines: list[str] = []
    important_lines: list[str] = []
    collected_artifacts: list[str] = []
    errors: list[str] = []
    seen_history: set[str] = set()
    seen_important: set[tuple[str, str]] = set()
    seen_artifacts: set[str] = set()

    for event in events:
        event_type = str(event.get("eventType") or "")
        payload = event.get("payload") or {}
        progress_text = _resolve_progress_text(progress_text, payload)

        if event_type == "session-start":
            command = str(payload.get("command") or command)
            _append_unique(history_lines, seen_history, f"[{_format_progress_label(progress_text)}] start  {command or '-'}")
            continue

        if event_type == "progress":
            active_unit = payload.get("activeUnit")
            suite_status = payload.get("suiteStatus")
            if active_unit and suite_status:
                _append_unique(
                    history_lines,
                    seen_history,
                    f"[{_format_progress_label(progress_text)}] {str(suite_status):<6} {str(active_unit)}",
                )
            elif active_unit:
                _append_unique(
                    history_lines,
                    seen_history,
                    f"[{_format_progress_label(progress_text)}] queued {str(active_unit)}",
                )
            continue

        if event_type == "stage-start":
            active_unit = payload.get("activeUnit")
            if active_unit:
                _append_unique(
                    history_lines,
                    seen_history,
                    f"[{_format_progress_label(progress_text)}] run    {str(active_unit)}",
                )
            continue

        if event_type == "warning":
            message = payload.get("message")
            if message:
                _append_unique(
                    history_lines,
                    seen_history,
                    f"[{_format_progress_label(progress_text)}] warn   {str(message)}",
                )
            continue

        if event_type == "artifact":
            path = payload.get("path")
            if path:
                _append_unique(
                    history_lines,
                    seen_history,
                    f"[{_format_progress_label(progress_text)}] file   {str(path)}",
                )
                _append_grouped_artifact_values(collected_artifacts, seen_artifacts, str(path))
            continue

        if event_type == "final-summary":
            final_status = str(payload.get("finalStatus") or final_status)
            _append_unique(history_lines, seen_history, f"[{_format_progress_label(progress_text)}] done   {final_status}")
            phase_lines = _build_phase_result_lines(payload)
            subject_lines = _build_subject_result_lines(payload)
            errors.extend(str(error) for error in list(payload.get("errors") or []))
            important_lines.extend(build_test_report_highlight_lines(payload, repo_root=repo_root, seen=seen_important))
            for artifact in list(payload.get("artifacts") or []):
                _append_grouped_artifact_values(collected_artifacts, seen_artifacts, artifact)

    artifact_lines = build_artifact_lines({"artifacts": collected_artifacts}, repo_root=repo_root)

    return TestProgressView(
        command=command,
        progress_text=progress_text,
        final_status=final_status,
        history_lines=history_lines,
        phase_lines=phase_lines,
        subject_lines=subject_lines,
        important_lines=important_lines,
        artifact_lines=artifact_lines,
        artifact_count=len(seen_artifacts),
        errors=errors,
    )


def build_operation_progress_view(events: list[dict[str, Any]], repo_root: Path | None = None) -> OperationProgressView:
    command = ""
    progress_text = "0%"
    final_status = "running"
    history_lines: list[str] = []
    important_lines: list[str] = []
    collected_artifacts: list[str] = []
    errors: list[str] = []
    seen_history: set[str] = set()
    seen_important: set[tuple[str, str]] = set()
    seen_artifacts: set[str] = set()

    for event in events:
        event_type = str(event.get("eventType") or "")
        payload = event.get("payload") or {}
        progress_text = _resolve_progress_text(progress_text, payload)

        if event_type == "session-start":
            command = str(payload.get("command") or command)
            _append_unique(history_lines, seen_history, f"[{_format_progress_label(progress_text)}] start  {command or '-'}")
            continue

        if event_type == "progress":
            active_unit = payload.get("activeUnit")
            step_status = payload.get("suiteStatus")
            if active_unit and step_status:
                _append_unique(
                    history_lines,
                    seen_history,
                    f"[{_format_progress_label(progress_text)}] {str(step_status):<6} {str(active_unit)}",
                )
            elif active_unit:
                _append_unique(
                    history_lines,
                    seen_history,
                    f"[{_format_progress_label(progress_text)}] queued {str(active_unit)}",
                )
            continue

        if event_type == "stage-start":
            active_unit = payload.get("activeUnit")
            if active_unit:
                _append_unique(
                    history_lines,
                    seen_history,
                    f"[{_format_progress_label(progress_text)}] run    {str(active_unit)}",
                )
            continue

        if event_type == "warning":
            message = payload.get("message")
            if message:
                _append_unique(
                    history_lines,
                    seen_history,
                    f"[{_format_progress_label(progress_text)}] warn   {str(message)}",
                )
            continue

        if event_type == "artifact":
            path = payload.get("path")
            if path:
                _append_unique(
                    history_lines,
                    seen_history,
                    f"[{_format_progress_label(progress_text)}] file   {str(path)}",
                )
                _append_grouped_artifact_values(collected_artifacts, seen_artifacts, str(path))
            continue

        if event_type == "final-summary":
            final_status = str(payload.get("finalStatus") or final_status)
            _append_unique(history_lines, seen_history, f"[{_format_progress_label(progress_text)}] done   {final_status}")
            errors.extend(str(error) for error in list(payload.get("errors") or []))
            important_lines.extend(build_operation_highlight_lines(payload, repo_root=repo_root, seen=seen_important))
            for artifact in list(payload.get("artifacts") or []):
                _append_grouped_artifact_values(collected_artifacts, seen_artifacts, artifact)

    artifact_lines = build_artifact_lines({"artifacts": collected_artifacts}, repo_root=repo_root)

    return OperationProgressView(
        command=command,
        progress_text=progress_text,
        final_status=final_status,
        history_lines=history_lines,
        important_lines=important_lines,
        artifact_lines=artifact_lines,
        artifact_count=len(seen_artifacts),
        errors=errors,
    )


def _build_phase_result_lines(payload: dict[str, Any]) -> list[str]:
    phase_results = list(payload.get("phaseResults") or [])
    return [f"{phase.get('status', '-')}: {phase.get('phaseId', '-')}" for phase in phase_results]


def _build_subject_result_lines(payload: dict[str, Any]) -> list[str]:
    subject_results = list(payload.get("subjectResults") or [])
    return [f"{subject.get('status', '-')}: {subject.get('subjectId', '-')}" for subject in subject_results]


def render_test_report_highlights(payload: dict[str, Any], repo_root: Path | None = None) -> str:
    important_lines = build_test_report_highlight_lines(payload, repo_root=repo_root)
    artifact_lines = build_artifact_lines(payload, repo_root=repo_root)
    if not important_lines and not artifact_lines:
        return ""
    lines = [_highlight_heading("Important outputs:")]
    lines.extend(important_lines)
    if artifact_lines:
        lines.append("")
        artifact_count = len(list(payload.get("artifacts") or []))
        lines.append(_highlight_label(f"Artifacts ({artifact_count}):"))
        lines.extend(artifact_lines)
    return "\n".join(lines) + "\n"


def render_operation_report_highlights(payload: dict[str, Any], repo_root: Path | None = None) -> str:
    important_lines = build_operation_highlight_lines(payload, repo_root=repo_root)
    artifact_lines = build_artifact_lines(payload, repo_root=repo_root)
    if not important_lines and not artifact_lines:
        return ""
    lines = [_highlight_heading("Important outputs:")]
    lines.extend(important_lines)
    if artifact_lines:
        lines.append("")
        artifact_count = len(list(payload.get("artifacts") or []))
        lines.append(_highlight_label(f"Artifacts ({artifact_count}):"))
        lines.extend(artifact_lines)
    return "\n".join(lines) + "\n"


def build_test_report_highlight_lines(
    payload: dict[str, Any],
    *,
    repo_root: Path | None = None,
    seen: set[tuple[str, str]] | None = None,
) -> list[str]:
    lines: list[str] = []
    seen = seen or set()
    _append_important_line(lines, seen, "Test report", payload.get("summaryPath"), repo_root=repo_root)
    _append_important_line(lines, seen, "Event stream", payload.get("eventsPath"), repo_root=repo_root)
    _append_important_line(lines, seen, "Session record", payload.get("sessionPath"), repo_root=repo_root)
    _append_important_line(lines, seen, "Console log", payload.get("consolePath"), repo_root=repo_root)
    _append_important_line(lines, seen, "Performance / telemetry", payload.get("telemetryPath"), repo_root=repo_root)
    for subject_result in list(payload.get("subjectResults") or []):
        if not isinstance(subject_result, dict):
            continue
        _append_important_line(
            lines,
            seen,
            "Subject summary",
            subject_result.get("subjectSummaryPath"),
            repo_root=repo_root,
        )
    return lines


def build_operation_highlight_lines(
    payload: dict[str, Any],
    *,
    repo_root: Path | None = None,
    seen: set[tuple[str, str]] | None = None,
) -> list[str]:
    lines: list[str] = []
    seen = seen or set()
    _append_important_line(lines, seen, "Run summary", payload.get("summaryPath"), repo_root=repo_root)
    _append_important_line(lines, seen, "Event stream", payload.get("eventsPath"), repo_root=repo_root)
    _append_important_line(lines, seen, "Console log", payload.get("consolePath"), repo_root=repo_root)
    _append_important_line(lines, seen, "Performance / telemetry", payload.get("telemetryPath"), repo_root=repo_root)
    for item in list(payload.get("importantOutputs") or []):
        if not isinstance(item, dict):
            continue
        label = str(item.get("label") or "").strip()
        path = item.get("path")
        if label and path:
            _append_important_line(lines, seen, label, path, repo_root=repo_root)
    return lines


def build_artifact_lines(
    payload: dict[str, Any],
    *,
    repo_root: Path | None = None,
    seen: set[str] | None = None,
) -> list[str]:
    grouped: dict[str, list[str]] = {
        "Smoke binaries": [],
        "Trace outputs": [],
        "Verify outputs": [],
        "Other artifacts": [],
    }
    seen = seen or set()
    for artifact in list(payload.get("artifacts") or []):
        _append_grouped_artifact(grouped, seen, artifact)

    lines: list[str] = []
    for group_name in ("Smoke binaries", "Trace outputs", "Verify outputs", "Other artifacts"):
        values = grouped[group_name]
        if not values:
            continue
        lines.append(_highlight_label(f"{group_name} ({len(values)}):"))
        for index, value in enumerate(values, start=1):
            lines.append(f"  {index}. {_terminal_link(value, repo_root)}")
    return lines


def _append_grouped_artifact_values(lines: list[str], seen: set[str], value: Any) -> None:
    if not value:
        return
    text = str(value)
    if text in seen:
        return
    seen.add(text)
    lines.append(text)


def _resolve_progress_text(current: str, payload: dict[str, Any]) -> str:
    completed = payload.get("completedUnits")
    total = payload.get("totalUnits")
    if isinstance(completed, int) and isinstance(total, int) and total > 0:
        return f"{int((completed / total) * 100)}%"
    return current


def _format_progress_label(progress_text: str) -> str:
    return _green(f"{progress_text:>4}")


def _green(text: str) -> str:
    return f"{ANSI_GREEN}{text}{ANSI_RESET}"


def _style_menu_screen_title(text: str) -> str:
    return f"{ANSI_BOLD_YELLOW}{text}{ANSI_RESET}"


def _style_menu_help(text: str) -> str:
    return f"{ANSI_BRIGHT_BLACK}{text}{ANSI_RESET}"


def _style_menu_counter(text: str) -> str:
    return f"{ANSI_GREEN}{text}{ANSI_RESET}"


def _style_group_heading(text: str) -> str:
    return f"{ANSI_BOLD_CYAN}{text}{ANSI_RESET}"


def _style_menu_syntax(text: str) -> str:
    return f"{ANSI_BOLD_CYAN}{text}{ANSI_RESET}"


def _style_menu_command_title(text: str) -> str:
    if _contains_cjk(text):
        return f"{ANSI_BOLD_BRIGHT_WHITE}{text}{ANSI_RESET}"
    return f"{ANSI_BOLD_BRIGHT_WHITE}{text}{ANSI_RESET}"


def _style_menu_divider(text: str) -> str:
    return f"{ANSI_BRIGHT_BLACK}{text}{ANSI_RESET}"


def _style_workspace_heading(text: str) -> str:
    return f"{ANSI_BOLD_CYAN}{text}{ANSI_RESET}"


def _build_inline_workspace_header(width: int) -> list[str]:
    divider = _trim("------------------------------------", max(24, width))
    return [
        "",
        _style_menu_divider(divider),
        "",
    ]


def _highlight_heading(text: str) -> str:
    return f"{ANSI_BOLD_YELLOW}{text}{ANSI_RESET}"


def _highlight_label(text: str) -> str:
    return f"{ANSI_BOLD_CYAN}{text}{ANSI_RESET}"


def _contains_cjk(text: str) -> bool:
    return any("\u4e00" <= character <= "\u9fff" for character in text)


def _append_unique(lines: list[str], seen: set[str], line: str) -> None:
    if line in seen:
        return
    seen.add(line)
    lines.append(line)


def _append_important_line(
    lines: list[str],
    seen: set[tuple[str, str]],
    label: str,
    value: Any,
    *,
    repo_root: Path | None = None,
) -> None:
    if not value:
        return
    text = str(value)
    key = (label, text)
    if key in seen:
        return
    seen.add(key)
    lines.append(f"{ANSI_BOLD_CYAN}{label}:{ANSI_RESET} {_terminal_link(text, repo_root)}")


def _append_grouped_artifact(
    grouped: dict[str, list[str]],
    seen: set[str],
    value: Any,
) -> None:
    if not value:
        return
    text = str(value)
    if text in seen:
        return
    seen.add(text)
    if "/artifacts/smoke/bin/" in text or text.startswith("artifacts/smoke/bin/"):
        grouped["Smoke binaries"].append(text)
        return
    if "/artifacts/run/trace/" in text or text.startswith("artifacts/run/trace/") or "warmup-trace" in text:
        grouped["Trace outputs"].append(text)
        return
    if (
        "/artifacts/verify-runtime-baseline/" in text
        or text.startswith("artifacts/verify-runtime-baseline/")
        or "/artifacts/verify-roadmap-0/" in text
        or text.startswith("artifacts/verify-roadmap-0/")
    ):
        grouped["Verify outputs"].append(text)
        return
    grouped["Other artifacts"].append(text)


def _terminal_link(text: str, repo_root: Path | None) -> str:
    uri = _path_to_uri(text, repo_root)
    if uri is None:
        return text
    return f"\x1b]8;;{uri}\x1b\\{text}\x1b]8;;\x1b\\"


def _path_to_uri(text: str, repo_root: Path | None) -> str | None:
    candidate = Path(text)
    if not candidate.is_absolute():
        if repo_root is None:
            return None
        candidate = (repo_root / candidate).resolve()
    else:
        candidate = candidate.resolve()
    return candidate.as_uri()


def _build_rows(entries: list[MenuEntry]) -> list[dict[str, Any]]:
    rows: list[dict[str, Any]] = []
    current_group: str | None = None
    for index, entry in enumerate(entries):
        if entry.group_title != current_group:
            rows.append({"entry_index": None, "text": entry.group_title})
            current_group = entry.group_title
        rows.append({"entry_index": index, "text": entry.syntax})
    return rows


def _compute_scroll_top(selected_row: int, body_height: int, row_count: int) -> int:
    if row_count <= body_height:
        return 0

    top = max(0, selected_row - (body_height // 2))
    return min(top, row_count - body_height)


def _find_selected_row(rows: list[dict[str, Any]], selection: int) -> int:
    for index, row in enumerate(rows):
        if row["entry_index"] == selection:
            return index
    return 0


def _selection_index_for_command(entries: list[MenuEntry], command_id: str | None) -> int:
    if not command_id:
        return 0
    for index, entry in enumerate(entries):
        if entry.command["id"] == command_id:
            return index
    return 0


def _trim(text: str, width: int) -> str:
    if width <= 0:
        return ""
    if _display_width(text) <= width:
        return text
    if width <= 3:
        return _slice_display_width(text, width)
    return _slice_display_width(text, width - 3) + "..."


def _pad(text: str, width: int) -> str:
    text_width = _display_width(text)
    if text_width >= width:
        return text
    return text + (" " * (width - text_width))


def _slice_display_width(text: str, width: int) -> str:
    if width <= 0:
        return ""

    parts: list[str] = []
    consumed = 0
    for character in text:
        char_width = _char_display_width(character)
        if consumed + char_width > width:
            break
        parts.append(character)
        consumed += char_width
    return "".join(parts)


def _display_width(text: str) -> int:
    return sum(_char_display_width(character) for character in text)


def _char_display_width(character: str) -> int:
    if not character:
        return 0
    if unicodedata.combining(character):
        return 0
    if unicodedata.east_asian_width(character) in {"W", "F"}:
        return 2
    return 1


def _stream_is_tty(name: str) -> bool:
    stream = getattr(sys, name, None)
    isatty = getattr(stream, "isatty", None)
    if not callable(isatty):
        return False

    try:
        return bool(isatty())
    except OSError:
        return False


def _enable_virtual_terminal_output() -> bool:
    global _WINDOWS_VT_READY
    if _WINDOWS_VT_READY is not None:
        return _WINDOWS_VT_READY

    if os.name != "nt":
        _WINDOWS_VT_READY = True
        return True

    kernel32 = ctypes.windll.kernel32
    handle = kernel32.GetStdHandle(-11)
    if handle == 0 or handle == -1:
        _WINDOWS_VT_READY = False
        return False

    mode = ctypes.c_uint()
    if kernel32.GetConsoleMode(handle, ctypes.byref(mode)) == 0:
        _WINDOWS_VT_READY = False
        return False

    if kernel32.SetConsoleMode(handle, mode.value | 0x0004) == 0:
        _WINDOWS_VT_READY = False
        return False

    _WINDOWS_VT_READY = True
    return True


class _TerminalSession:
    fullscreen = True

    def __init__(self) -> None:
        self._stdin_fd: int | None = None
        self._saved_mode: Any = None

    def __enter__(self) -> "_TerminalSession":
        if os.name != "nt":
            self._stdin_fd = sys.stdin.fileno()
            self._saved_mode = termios.tcgetattr(self._stdin_fd)
            tty.setraw(self._stdin_fd)

        self._write("\x1b[?1049h\x1b[?25l\x1b[2J\x1b[H")
        return self

    def __exit__(self, exc_type, exc, tb) -> None:
        self._write("\x1b[0m\x1b[?25h\x1b[?1049l")
        if os.name != "nt" and self._stdin_fd is not None and self._saved_mode is not None:
            termios.tcsetattr(self._stdin_fd, termios.TCSADRAIN, self._saved_mode)

    def render(self, screen: str) -> None:
        self._write(screen)

    def read_key(self) -> str:
        if os.name == "nt":
            return _read_windows_key()
        return _read_posix_key(self._stdin_fd)

    @staticmethod
    def _write(text: str) -> None:
        if os.name != "nt":
            text = text.replace("\r\n", "\n").replace("\n", "\r\n")
        sys.stdout.write(text)
        sys.stdout.flush()


class _InlineTerminalSession:
    fullscreen = False

    def __init__(self) -> None:
        self._stdin_fd: int | None = None
        self._saved_mode: Any = None
        self._rendered_lines = 0

    def __enter__(self) -> "_InlineTerminalSession":
        if os.name != "nt":
            self._stdin_fd = sys.stdin.fileno()
            self._saved_mode = termios.tcgetattr(self._stdin_fd)
            tty.setraw(self._stdin_fd)

        self._write("\x1b[?25l")
        return self

    def __exit__(self, exc_type, exc, tb) -> None:
        self._clear_rendered_region()
        self._write("\x1b[0m\x1b[?25h")
        if os.name != "nt" and self._stdin_fd is not None and self._saved_mode is not None:
            termios.tcsetattr(self._stdin_fd, termios.TCSADRAIN, self._saved_mode)

    def render(self, screen: str) -> None:
        lines = screen.splitlines() or [""]
        self._clear_rendered_region()
        self._write(screen)
        self._rendered_lines = len(lines)

    def read_key(self) -> str:
        if os.name == "nt":
            return _read_windows_key()
        return _read_posix_key(self._stdin_fd)

    def _clear_rendered_region(self) -> None:
        if self._rendered_lines <= 0:
            return
        self._write("\r")
        if self._rendered_lines > 1:
            self._write(f"\x1b[{self._rendered_lines - 1}F")
        self._write("\x1b[J")
        self._rendered_lines = 0

    @staticmethod
    def _write(text: str) -> None:
        if os.name != "nt":
            text = text.replace("\r\n", "\n").replace("\n", "\r\n")
        sys.stdout.write(text)
        sys.stdout.flush()


def _read_windows_key() -> str:
    first = msvcrt.getwch()
    if first in ("\x00", "\xe0"):
        second = msvcrt.getwch()
        return {
            "H": "up",
            "P": "down",
            "I": "page_up",
            "Q": "page_down",
            "G": "home",
            "O": "end",
        }.get(second, "unknown")

    return {
        "\r": "enter",
        "\x1b": "escape",
        "q": "quit",
        "Q": "quit",
        "k": "up",
        "K": "up",
        "j": "down",
        "J": "down",
    }.get(first, "unknown")


def _read_posix_key(stdin_fd: int | None) -> str:
    if stdin_fd is None:
        return "quit"

    first = os.read(stdin_fd, 1)
    if first in {b"\r", b"\n"}:
        return "enter"
    if first in {b"q", b"Q"}:
        return "quit"
    if first in {b"k", b"K"}:
        return "up"
    if first in {b"j", b"J"}:
        return "down"
    if first != b"\x1b":
        return "unknown"

    if not select.select([stdin_fd], [], [], 0.05)[0]:
        return "escape"

    second = os.read(stdin_fd, 1)
    if second != b"[":
        return "escape"

    third = os.read(stdin_fd, 1)
    if third == b"A":
        return "up"
    if third == b"B":
        return "down"
    if third == b"H":
        return "home"
    if third == b"F":
        return "end"
    if third in {b"5", b"6"} and select.select([stdin_fd], [], [], 0.01)[0]:
        fourth = os.read(stdin_fd, 1)
        if fourth == b"~":
            return "page_up" if third == b"5" else "page_down"
    return "unknown"
