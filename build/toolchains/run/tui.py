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
    from core import manifest as manifest_module
    from subject import project_workspace as project_workspace_module
    from testing import public_specs as public_specs_module
    from testing import registry as registry_module
    from testing import subjects as subjects_module
else:
    from .core import manifest as manifest_module
    from .subject import project_workspace as project_workspace_module
    from .testing import public_specs as public_specs_module
    from .testing import registry as registry_module
    from .testing import subjects as subjects_module

if os.name == "nt":
    import ctypes
    import msvcrt
else:
    import select
    import termios
    import tty


_WINDOWS_VT_READY: bool | None = None
REPO_ROOT = Path(__file__).resolve().parents[3]
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
        "id": "project-menu",
        "group_title": "工程 / IDE",
        "syntax": "project",
        "title": "生成或构建可供 VS / IDE 打开的 subject 与 core 工程工作区",
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
    batch_lines: list[str]
    count_lines: list[str]
    current_lines: list[str]
    failure_lines: list[str]
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
    ordered_items: list[tuple[str, str | None, str, str]] = [
        ("基础准备", "doctor", "doctor", "只检查当前本机工具链与运行条件"),
        ("基础准备", "prepare", "setup", "检查并初始化当前主机环境（推荐）"),
        ("场景准备", "prepare-smoke", "smoke", "准备 smoke 测试所需的托管运行环境"),
    ]
    if host_platform == "windows":
        ordered_items.append(("场景准备", "prepare-android-host", "android-host", "准备 Android SDK / NDK / adb / emulator 缓存"))
    ordered_items.append(
        (
            "场景准备",
            f"prepare-workflow-runtime-baseline-{host_platform}",
            "runtime-baseline",
            "准备当前主机的 runtime-baseline 工作流环境",
        )
    )
    return _build_curated_submenu_entries(
        manifest,
        host_platform,
        ordered_items,
    )


def build_project_menu_entries(manifest: dict[str, Any], host_platform: str) -> list[MenuEntry]:
    return [
        MenuEntry(
            "生成工程",
            {
                "id": "generate-project-all",
                "title": "生成所有 Solution（当前宿主）",
                "details": _project_command_details("generate-project-all", host_platform),
            },
            "all",
            ["generate", "project", "all"],
        ),
        MenuEntry(
            "生成工程",
            {
                "id": "generate-project-subject",
                "title": "生成 Subject 调试工程",
                "details": _project_command_details("generate-project-subject", host_platform),
            },
            "subject",
            ["generate", "project", "subject"],
        ),
        MenuEntry(
            "生成工程",
            {
                "id": "generate-project-core",
                "title": "生成 IL2CPP Core 调试工程",
                "details": _project_command_details("generate-project-core", host_platform),
            },
            "core",
            ["generate", "project", "core"],
        ),
        MenuEntry(
            "构建工程",
            {
                "id": "build-project-subject",
                "title": "构建 Subject 已生成工程",
                "details": _project_command_details("build-project-subject", host_platform),
            },
            "subject-build",
            ["build", "project", "subject"],
        ),
        MenuEntry(
            "构建工程",
            {
                "id": "build-project-core",
                "title": "构建 IL2CPP Core 已生成工程",
                "details": _project_command_details("build-project-core", host_platform),
            },
            "core-build",
            ["build", "project", "core"],
        ),
        MenuEntry(
            "返回上级",
            dict(MENU_BACK_COMMAND),
            "back",
            [],
        ),
    ]


def build_clean_menu_entries(manifest: dict[str, Any], host_platform: str) -> list[MenuEntry]:
    ordered_items: list[tuple[str, str | None, str, str]] = [
        ("清理范围", "clean", "all", "清理统一入口产生的全部托管输出与缓存产物"),
        ("清理范围", "clean-smoke", "smoke", "只清理 smoke 测试相关输出"),
    ]
    if host_platform == "windows":
        ordered_items.append(("清理范围", "clean-android-host", "android-host", "清理 Android SDK / NDK / adb / emulator 缓存"))
    ordered_items.append(
        (
            "清理范围",
            f"clean-workflow-runtime-baseline-{host_platform}",
            "runtime-baseline",
            "清理当前主机的 runtime-baseline 工作流输出",
        )
    )
    return _build_curated_submenu_entries(
        manifest,
        host_platform,
        ordered_items,
    )


def _project_command_details(command_id: str, host_platform: str) -> dict[str, str]:
    core_targets = ", ".join(_core_target_ids(host_platform)) or "-"
    if command_id == "generate-project-all":
        return {
            "summary": "为当前宿主生成全部 subject/core solution 与对应 native project/workspace。",
            "outputs": "verification/workspaces/verification-all.manifest.json | verification/workspaces/verification-all.generation.report.json | verification/workspaces/subjects/<subject-id>/ | verification/workspaces/core/<host>/",
            "target_scope": "当前宿主支持的全部 subject 与全部 core target",
            "host_support": f"{host_platform}: {_subject_count_for_host(host_platform)} subjects | {core_targets}",
            "command": f"run generate project all --host {host_platform} [--refresh-generated]",
            "notes": "重操作入口；subject 默认按全部支持目标生成 native project 并写入 solution。",
        }
    if command_id == "generate-project-subject":
        return {
            "summary": "生成 Subject 的 solution 与 native 调试工程。",
            "outputs": "verification/workspaces/subjects/<subject-id>/<subject-id>.sln | verification/workspaces/subjects/<subject-id>/native/<matrix-id>/",
            "target_scope": "默认目标 / 指定目标 / 全部支持目标",
            "host_support": "从 subject.manifest.json 动态读取当前宿主可用 matrices",
            "command": "run generate project subject --id subject/<subject-id> [--matrix <matrix-id>] [--all-targets] [--variant CHECK|PROFILE|SHIP] [--refresh-generated] [--open-native-target generated|proof]",
            "notes": "Windows Subject 会同时生成 generated/proof 两个 native project；可显式选择默认打开哪个 native project。",
        }
    if command_id == "build-project-subject":
        return {
            "summary": "构建 Subject 已生成的 managed solution 与 native workspace。",
            "outputs": "verification/workspaces/subjects/<subject-id>/build.report.json | verification/workspaces/subjects/<subject-id>/workspace.manifest.json",
            "target_scope": "默认目标 / 指定目标 / 全部支持目标",
            "host_support": "基于已生成的 workspace.manifest.json 读取可构建 matrices",
            "command": "run build project subject --id subject/<subject-id> [--matrix <matrix-id>] [--all-targets] [--native-target generated|proof]",
            "notes": "只消费现有工作区；不会刷新 generated。Windows Subject 可显式选择构建 generated 或 proof native project。",
        }
    if command_id == "generate-project-core":
        return {
            "summary": "生成 IL2CPP Core 的 managed solution 与 native 调试工作区。",
            "outputs": "verification/workspaces/core/<host>/chaos-il2cpp-core.sln | verification/workspaces/core/<host>/native/<target-id>/",
            "target_scope": "默认目标 / 指定目标 / 全部支持目标",
            "host_support": core_targets,
            "command": f"run generate project core --host {host_platform} [--target <target-id>] [--all-targets]",
            "notes": "native 侧输出仍视为 workspace，不与 managed .sln 混称。",
        }
    return {
        "summary": "构建 IL2CPP Core 已生成的 managed solution 与 native workspace。",
        "outputs": "verification/workspaces/core/<host>/build.report.json | verification/workspaces/core/<host>/workspace.manifest.json",
        "target_scope": "默认目标 / 指定目标 / 全部支持目标",
        "host_support": core_targets,
        "command": f"run build project core --host {host_platform} [--target <target-id>] [--all-targets]",
        "notes": "构建前应已存在 core workspace.manifest.json。",
    }


def _host_platform_family(platform_id: str) -> str:
    if platform_id.startswith("windows"):
        return "windows"
    if platform_id.startswith("macos"):
        return "macos"
    if platform_id.startswith("linux"):
        return "linux"
    return platform_id


def _core_target_ids(host_platform: str) -> list[str]:
    return [
        target_id
        for target_id in project_workspace_module.CORE_TARGET_ORDER
        if host_platform in set(project_workspace_module.CORE_TARGET_SPECS[target_id]["hosts"])
    ]


def _subject_records_for_host(host_platform: str) -> list[dict[str, Any]]:
    records: list[dict[str, Any]] = []
    for manifest_path in subjects_module.discover_subject_manifests(REPO_ROOT):
        try:
            manifest = subjects_module.load_subject_manifest_file(manifest_path)
        except Exception:
            continue
        subject_id = str(manifest.get("subjectId") or manifest_path.parent.name)
        matrices = [
            dict(matrix)
            for matrix in list(manifest.get("environmentMatrices") or [])
            if _host_platform_family(str(dict(matrix.get("executionContext") or {}).get("hostPlatform") or "")) == host_platform
        ]
        if not matrices:
            continue
        records.append(
            {
                "subject_id": subject_id,
                "title": str(manifest.get("displayName") or subject_id),
                "manifest": manifest,
                "matrices": matrices,
            }
        )
    return records


def _subject_count_for_host(host_platform: str) -> int:
    return len(_subject_records_for_host(host_platform))


def _subject_matrix_target_platform(matrix: dict[str, Any]) -> str:
    return str(dict(matrix.get("executionContext") or {}).get("targetPlatform") or "")


def _subject_matrices_support_dual_native_projects(matrices: list[dict[str, Any]]) -> bool:
    return any(_subject_matrix_target_platform(matrix) == "windows-x64" for matrix in matrices)


def _build_choice_entries(group_title: str, options: list[dict[str, Any]]) -> list[MenuEntry]:
    entries = [
        MenuEntry(
            group_title,
            dict(option["command"]),
            str(option["syntax"]),
            list(option.get("argv") or []),
        )
        for option in options
    ]
    entries.append(MenuEntry("返回上级", dict(MENU_BACK_COMMAND), "back", []))
    return entries


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
        MenuEntry("Quick Start", {"id": "test-all", "title": "Run host default batch (suites + subjects)"}, "all", ["test", "all"]),
        MenuEntry("Selectors", {"id": "test-suite", "title": "Run a suite object"}, "suite", ["test"]),
        MenuEntry("Selectors", {"id": "test-subject", "title": "Run a subject object"}, "subject", ["test"]),
        MenuEntry("Selectors", {"id": "test-module", "title": "Run a module verification object"}, "module", ["test"]),
        MenuEntry("Selectors", {"id": "test-system", "title": "Run a system validation object"}, "system", ["test"]),
        MenuEntry("Registry", {"id": "test-registry-list", "title": "Browse registered test objects"}, "registry-list", ["test", "registry", "list"]),
        MenuEntry("Registry", {"id": "test-registry-refresh", "title": "Refresh registry snapshots"}, "registry-refresh", ["test", "registry", "refresh"]),
        MenuEntry("Registry", {"id": "test-registry-check-consistency", "title": "Check registry/wiki consistency"}, "registry-check", ["test", "registry", "check-consistency"]),
        MenuEntry("Results", {"id": "test-watch", "title": "Show the latest test event timeline"}, "watch", ["test", "watch"]),
        MenuEntry("Results", {"id": "test-summary", "title": "Show the latest aggregated summary"}, "summary", ["test", "summary"]),
        MenuEntry("Results", {"id": "verify-verification-v1", "title": "Refresh verification-v1 formal outputs"}, "verify", ["verify", "verification-v1"]),
        MenuEntry("Back", {**MENU_BACK_COMMAND, "title": "Back to previous menu"}, "back", []),
    ]


def build_test_subject_menu_entries(host_platform: str) -> list[MenuEntry]:
    index = registry_module.scan_registry(
        REPO_ROOT,
        host_platform=host_platform,
        public_suite_specs=public_specs_module.PUBLIC_TEST_SPECS,
    )
    entries = [
        MenuEntry(
            "Subjects",
            {
                "id": f"test-subject-target:{subject['id']}",
                "title": str(subject.get("displayName") or subject.get("subjectId") or subject["id"]),
                "targetObjectId": str(subject["id"]),
            },
            str(subject.get("subjectId") or subject["id"]),
            ["test", "subject", "--id", str(subject["id"])],
        )
        for subject in sorted(index.subjects, key=lambda item: str(item["id"]))
    ]
    entries.append(MenuEntry("Back", {**MENU_BACK_COMMAND, "title": "Back to previous menu"}, "back", []))
    return entries


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
            "Test mode (suite/subject/module/system/all/registry-list/registry-refresh/registry-check/watch/summary/verify), leave blank to cancel: "
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
            value = prompt_value_provider("Enter subject id, for example: SolutionCorePack: ").strip()
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
        if mode == "verify":
            return ["verify", "verification-v1"]
        return None

    if command_id == "test-suite":
        value = prompt_value_provider("Enter family and suite, for example: smoke HelloWorld: ").strip()
        parts = value.split()
        if len(parts) != 2:
            return None
        return ["test", "suite", "--family", parts[0], "--suite", parts[1]]

    if command_id == "test-subject":
        value = prompt_value_provider("Enter subject id, for example: SolutionCorePack: ").strip()
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
    elif section_command_id == "project-menu":
        project_kwargs = {"terminal": terminal, "menu_state": menu_state}
        return run_project_submenu(manifest, host_platform, **project_kwargs)
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


def _run_choice_menu(
    entries: list[MenuEntry],
    *,
    title: str,
    terminal: "_TerminalSession | _InlineTerminalSession",
) -> MenuEntry | None:
    selected_entry = _run_menu_selection(
        terminal,
        entries,
        title=title,
        help_text=SECTION_MENU_HELP,
    )
    if selected_entry is None or selected_entry.command["id"] == "menu-back":
        return None
    return selected_entry


def run_project_submenu(
    manifest: dict[str, Any],
    host_platform: str,
    *,
    terminal: "_TerminalSession | _InlineTerminalSession" | None = None,
    menu_state: MenuState | None = None,
) -> list[str] | None:
    entries = build_project_menu_entries(manifest, host_platform)
    initial_selection = 0
    if menu_state is not None and menu_state.active_section_command_id == "project-menu":
        initial_selection = _selection_index_for_command(entries, menu_state.section_selection_command_id)

    active_terminal = terminal
    created_terminal = False
    if active_terminal is None:
        active_terminal = _TerminalSession()
        created_terminal = True

    def _run_loop(current_terminal: "_TerminalSession | _InlineTerminalSession") -> list[str] | None:
        nonlocal initial_selection
        while True:
            selected_entry = _run_menu_selection(
                current_terminal,
                entries,
                title="工程 / IDE",
                help_text=SECTION_MENU_HELP,
                initial_selection=initial_selection,
            )
            if selected_entry is None or selected_entry.command["id"] == "menu-back":
                return None
            if menu_state is not None:
                menu_state.primary_command_id = "project-menu"
                menu_state.active_section_command_id = "project-menu"
                menu_state.section_selection_command_id = selected_entry.command["id"]

            command_id = selected_entry.command["id"]
            if command_id == "generate-project-all":
                argv = run_generate_project_all_flow(host_platform, terminal=current_terminal)
            elif command_id in {"generate-project-subject", "build-project-subject"}:
                argv = run_project_subject_flow(command_id, host_platform, terminal=current_terminal)
            elif command_id in {"generate-project-core", "build-project-core"}:
                argv = run_project_core_flow(command_id, host_platform, terminal=current_terminal)
            else:
                argv = resolve_entry_argv(selected_entry)
            if argv is not None:
                return argv
            initial_selection = _selection_index_for_command(entries, selected_entry.command["id"])

    if created_terminal:
        with active_terminal as current_terminal:
            return _run_loop(current_terminal)
    return _run_loop(active_terminal)


def run_generate_project_all_flow(
    host_platform: str,
    *,
    terminal: "_TerminalSession | _InlineTerminalSession",
) -> list[str] | None:
    refresh_entry = _run_choice_menu(
        _build_choice_entries(
            "刷新策略",
            [
                {
                    "command": {
                        "id": "project-all-use-generated",
                        "title": "使用现有 generated 生成全部 Solution",
                        "refresh_generated": False,
                    },
                    "syntax": "reuse",
                },
                {
                    "command": {
                        "id": "project-all-refresh-generated",
                        "title": "先刷新全部 generated 再生成全部 Solution",
                        "refresh_generated": True,
                    },
                    "syntax": "refresh",
                },
            ],
        ),
        title="生成所有 Solution",
        terminal=terminal,
    )
    if refresh_entry is None:
        return None

    argv = ["generate", "project", "all", "--host", host_platform]
    if refresh_entry.command.get("refresh_generated"):
        argv.append("--refresh-generated")
    if not run_confirmation_screen(
        "确认生成所有 Solution",
        [
            "输出根: verification/workspaces/",
            f"当前宿主: {host_platform}",
            f"刷新 generated: {'yes' if refresh_entry.command.get('refresh_generated') else 'no'}",
            f"执行命令: {' '.join(argv)}",
        ],
        terminal=terminal,
    ):
        return None
    return argv


def run_project_subject_flow(
    command_id: str,
    host_platform: str,
    *,
    terminal: "_TerminalSession | _InlineTerminalSession",
) -> list[str] | None:
    is_generate = command_id == "generate-project-subject"
    subject_records = _subject_records_for_host(host_platform)
    subject_entry = _run_choice_menu(
        _build_choice_entries(
            "Subjects",
            [
                {
                    "command": {
                        "id": f"subject-option:{record['subject_id']}",
                        "title": str(record["title"]),
                        "subject_id": str(record["subject_id"]),
                    },
                    "syntax": str(record["subject_id"]),
                }
                for record in subject_records
            ],
        ),
        title="选择 Subject",
        terminal=terminal,
    )
    if subject_entry is None:
        return None

    selected_record = next(
        (record for record in subject_records if str(record["subject_id"]) == str(subject_entry.command["subject_id"])),
        None,
    )
    if selected_record is None:
        raise RuntimeError(f"subject not found for project flow: {subject_entry.command['subject_id']}")

    scope_entry = _run_choice_menu(
        _build_choice_entries(
            "范围",
            [
                {"command": {"id": "scope-default", "title": "默认目标", "scope": "default"}, "syntax": "default"},
                {"command": {"id": "scope-specific", "title": "指定目标", "scope": "specific"}, "syntax": "specific"},
                {"command": {"id": "scope-all-targets", "title": "全部支持目标", "scope": "all-targets"}, "syntax": "all-targets"},
            ],
        ),
        title="选择目标范围",
        terminal=terminal,
    )
    if scope_entry is None:
        return None

    argv = [
        "generate" if is_generate else "build",
        "project",
        "subject",
        "--id",
        f"subject/{subject_entry.command['subject_id']}",
    ]
    selected_matrix_id = ""
    scope = str(scope_entry.command["scope"])
    selected_matrices_for_flow: list[dict[str, Any]] = []
    if scope == "specific":
        matrix_entry = _run_choice_menu(
            _build_choice_entries(
                "Matrices",
                [
                    {
                        "command": {
                            "id": f"matrix-option:{matrix['matrixId']}",
                            "title": str(matrix.get("matrixId") or ""),
                            "matrix_id": str(matrix.get("matrixId") or ""),
                        },
                        "syntax": str(matrix.get("matrixId") or ""),
                    }
                    for matrix in list(selected_record["matrices"])
                ],
            ),
            title="选择 Matrix",
            terminal=terminal,
        )
        if matrix_entry is None:
            return None
        selected_matrix_id = str(matrix_entry.command["matrix_id"])
        argv.extend(["--matrix", selected_matrix_id])
        selected_matrices_for_flow = [
            matrix for matrix in list(selected_record["matrices"]) if str(matrix.get("matrixId") or "") == selected_matrix_id
        ]
    elif scope == "all-targets":
        argv.append("--all-targets")
        selected_matrices_for_flow = [dict(matrix) for matrix in list(selected_record["matrices"])]
    else:
        default_matrix_id = str(dict(selected_record.get("manifest") or {}).get("defaultMatrix") or "")
        selected_matrices_for_flow = [
            dict(matrix)
            for matrix in list(selected_record["matrices"])
            if not default_matrix_id or str(matrix.get("matrixId") or "") == default_matrix_id
        ]
        if not selected_matrices_for_flow and list(selected_record["matrices"]):
            selected_matrices_for_flow = [dict(list(selected_record["matrices"])[0])]

    selected_variant = ""
    refresh_generated = False
    selected_open_native_target = ""
    selected_build_native_target = ""
    if is_generate:
        variant_entry = _run_choice_menu(
            _build_choice_entries(
                "Variant",
                [
                    {"command": {"id": "variant-check", "title": "CHECK", "variant": "CHECK"}, "syntax": "CHECK"},
                    {"command": {"id": "variant-profile", "title": "PROFILE", "variant": "PROFILE"}, "syntax": "PROFILE"},
                    {"command": {"id": "variant-ship", "title": "SHIP", "variant": "SHIP"}, "syntax": "SHIP"},
                ],
            ),
            title="选择 Variant",
            terminal=terminal,
        )
        if variant_entry is None:
            return None
        selected_variant = str(variant_entry.command["variant"])
        argv.extend(["--variant", selected_variant])

        refresh_entry = _run_choice_menu(
            _build_choice_entries(
                "Generated",
                [
                    {
                        "command": {
                            "id": "use-generated",
                            "title": "使用现有 generated",
                            "refresh_generated": False,
                        },
                        "syntax": "reuse",
                    },
                    {
                        "command": {
                            "id": "refresh-generated",
                            "title": "刷新 generated 后再生成",
                            "refresh_generated": True,
                        },
                        "syntax": "refresh",
                    },
                ],
            ),
            title="选择 Generated 刷新策略",
            terminal=terminal,
        )
        if refresh_entry is None:
            return None
        refresh_generated = bool(refresh_entry.command.get("refresh_generated"))
        if refresh_generated:
            argv.append("--refresh-generated")
        if _subject_matrices_support_dual_native_projects(selected_matrices_for_flow):
            open_entry = _run_choice_menu(
                _build_choice_entries(
                    "Open",
                    [
                        {
                            "command": {
                                "id": "open-proof-native",
                                "title": "打开 proof native project",
                                "open_native_target": "proof",
                            },
                            "syntax": "proof",
                        },
                        {
                            "command": {
                                "id": "open-generated-native",
                                "title": "打开 generated native project",
                                "open_native_target": "generated",
                            },
                            "syntax": "generated",
                        },
                    ],
                ),
                title="选择默认打开的 Native Project",
                terminal=terminal,
            )
            if open_entry is None:
                return None
            selected_open_native_target = str(open_entry.command["open_native_target"])
            argv.extend(["--open-native-target", selected_open_native_target])
    elif _subject_matrices_support_dual_native_projects(selected_matrices_for_flow):
        build_entry = _run_choice_menu(
            _build_choice_entries(
                "Build",
                [
                    {
                        "command": {
                            "id": "build-proof-native",
                            "title": "构建 proof native project",
                            "native_target": "proof",
                        },
                        "syntax": "proof",
                    },
                    {
                        "command": {
                            "id": "build-generated-native",
                            "title": "构建 generated native project",
                            "native_target": "generated",
                        },
                        "syntax": "generated",
                    },
                ],
            ),
            title="选择要构建的 Native Project",
            terminal=terminal,
        )
        if build_entry is None:
            return None
        selected_build_native_target = str(build_entry.command["native_target"])
        argv.extend(["--native-target", selected_build_native_target])

    if not run_confirmation_screen(
        "确认 Subject 工程命令",
        [
            f"Subject: {subject_entry.command['subject_id']}",
            f"目标范围: {scope}",
            f"Matrix: {selected_matrix_id or '(default)'}",
            f"Variant: {selected_variant or '(build only)'}",
            f"刷新 generated: {'yes' if refresh_generated else 'no'}",
            f"默认打开 native project: {selected_open_native_target or '(default)'}",
            f"构建 native project: {selected_build_native_target or '(default)'}",
            f"执行命令: {' '.join(argv)}",
        ],
        terminal=terminal,
    ):
        return None
    return argv


def run_project_core_flow(
    command_id: str,
    host_platform: str,
    *,
    terminal: "_TerminalSession | _InlineTerminalSession",
) -> list[str] | None:
    scope_entry = _run_choice_menu(
        _build_choice_entries(
            "范围",
            [
                {"command": {"id": "scope-default", "title": "默认目标", "scope": "default"}, "syntax": "default"},
                {"command": {"id": "scope-specific", "title": "指定目标", "scope": "specific"}, "syntax": "specific"},
                {"command": {"id": "scope-all-targets", "title": "全部支持目标", "scope": "all-targets"}, "syntax": "all-targets"},
            ],
        ),
        title="选择 Core 目标范围",
        terminal=terminal,
    )
    if scope_entry is None:
        return None

    argv = [
        "generate" if command_id == "generate-project-core" else "build",
        "project",
        "core",
        "--host",
        host_platform,
    ]
    scope = str(scope_entry.command["scope"])
    selected_target_id = ""
    if scope == "specific":
        target_entry = _run_choice_menu(
            _build_choice_entries(
                "Targets",
                [
                    {
                        "command": {
                            "id": f"target-option:{target_id}",
                            "title": target_id,
                            "target_id": target_id,
                        },
                        "syntax": target_id,
                    }
                    for target_id in _core_target_ids(host_platform)
                ],
            ),
            title="选择 Core Target",
            terminal=terminal,
        )
        if target_entry is None:
            return None
        selected_target_id = str(target_entry.command["target_id"])
        argv.extend(["--target", selected_target_id])
    elif scope == "all-targets":
        argv.append("--all-targets")

    if not run_confirmation_screen(
        "确认 Core 工程命令",
        [
            f"当前宿主: {host_platform}",
            f"目标范围: {scope}",
            f"Target: {selected_target_id or '(default)'}",
            f"执行命令: {' '.join(argv)}",
        ],
        terminal=terminal,
    ):
        return None
    return argv


def run_confirmation_screen(
    title: str,
    lines: list[str],
    *,
    terminal: "_TerminalSession | _InlineTerminalSession",
) -> bool:
    while True:
        screen_lines = [title, "", *lines, "", "Enter 确认，Esc/Q 返回。"]
        terminal.render("\n".join(screen_lines) + "\n")
        key = terminal.read_key()
        if key == "enter":
            return True
        if key in {"escape", "quit"}:
            return False


def run_test_submenu(
    manifest: dict[str, Any],
    host_platform: str,
    *,
    terminal: "_TerminalSession" | None = None,
    menu_state: MenuState | None = None,
) -> list[str] | None:
    entries = build_test_menu_entries(manifest, host_platform)
    initial_selection = 0
    if menu_state is not None and menu_state.active_section_command_id == "test-menu":
        initial_selection = _selection_index_for_command(entries, menu_state.section_selection_command_id)

    active_terminal = terminal
    created_terminal = False
    if active_terminal is None:
        active_terminal = _TerminalSession()
        created_terminal = True

    def _run_loop(current_terminal: "_TerminalSession") -> list[str] | None:
        nonlocal initial_selection
        while True:
            selected_entry = _run_menu_selection(
                current_terminal,
                entries,
                title="统一测试入口",
                help_text=TEST_MENU_HELP,
                initial_selection=initial_selection,
            )
            if selected_entry is None or selected_entry.command["id"] == "menu-back":
                return None
            if selected_entry.command["id"] == "test-subject":
                if menu_state is not None:
                    menu_state.primary_command_id = "test-menu"
                    menu_state.active_section_command_id = "test-menu"
                    menu_state.section_selection_command_id = selected_entry.command["id"]
                argv = run_test_subject_submenu(manifest, host_platform, terminal=current_terminal, menu_state=menu_state)
                if argv is not None:
                    return argv
                initial_selection = _selection_index_for_command(entries, selected_entry.command["id"])
                continue
            if selected_entry.command["id"] == "test-all":
                if menu_state is not None:
                    menu_state.primary_command_id = "test-menu"
                    menu_state.active_section_command_id = "test-menu"
                    menu_state.section_selection_command_id = selected_entry.command["id"]
                argv = run_test_all_preview(host_platform, terminal=current_terminal)
                if argv is not None:
                    return argv
                initial_selection = _selection_index_for_command(entries, selected_entry.command["id"])
                continue
            if menu_state is not None:
                menu_state.primary_command_id = "test-menu"
                menu_state.active_section_command_id = "test-menu"
                menu_state.section_selection_command_id = selected_entry.command["id"]
            return resolve_entry_argv(selected_entry)

    if created_terminal:
        with active_terminal as current_terminal:
            return _run_loop(current_terminal)
    return _run_loop(active_terminal)


def run_test_subject_submenu(
    manifest: dict[str, Any],
    host_platform: str,
    *,
    terminal: "_TerminalSession" | None = None,
    menu_state: MenuState | None = None,
) -> list[str] | None:
    del manifest
    return _run_submenu(
        build_test_subject_menu_entries(host_platform),
        title="Select subject target",
        help_text=SECTION_MENU_HELP,
        terminal=terminal,
        section_command_id=None,
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


def _build_test_all_preview_payload(host_platform: str) -> dict[str, Any]:
    public_suites = public_specs_module.list_public_test_suites(host_platform)
    index = registry_module.scan_registry(
        REPO_ROOT,
        host_platform=host_platform,
        public_suite_specs=public_specs_module.PUBLIC_TEST_SPECS,
    )
    counts_by_family: dict[str, dict[str, int]] = {}
    for suite in public_suites:
        family = str(suite.get("family") or "unknown")
        bucket = counts_by_family.setdefault(family, {"total": 0})
        bucket["total"] += 1

    subject_preview = [
        str(subject.get("displayName") or subject.get("subjectId") or subject.get("id") or "").removeprefix("subject/")
        for subject in sorted(index.subjects, key=lambda item: str(item.get("id") or ""))
    ]
    return {
        "hostPlatform": host_platform,
        "plannedCounts": {
            "totalUnits": len(public_suites) + len(index.subjects),
            "suiteCount": len(public_suites),
            "subjectCount": len(index.subjects),
        },
        "countsByFamily": counts_by_family,
        "subjectPreview": subject_preview[:5],
        "outputPaths": {
            "summaryPath": "artifacts/logs/tests/<run-id>/summary.json",
            "eventsPath": "artifacts/logs/tests/<run-id>/events.jsonl",
            "consolePath": "artifacts/logs/tests/<run-id>/console.log",
        },
    }


def render_test_all_preview_screen(preview: dict[str, Any]) -> str:
    planned_line = _format_planned_counts(preview.get("plannedCounts"))
    family_line = _format_family_totals(preview.get("countsByFamily"))
    subject_preview = [str(item) for item in list(preview.get("subjectPreview") or []) if str(item)]
    output_paths = dict(preview.get("outputPaths") or {})

    lines = [
        "Unified Test Batch Preview",
        f"Host: {preview.get('hostPlatform', '-')}",
    ]
    if planned_line:
        lines.append(planned_line)
    if family_line:
        lines.append(f"Families: {family_line}")
    lines.append(f"Subjects: {' | '.join(subject_preview) if subject_preview else '-'}")
    lines.append("")
    lines.append("Outputs:")
    for key in ("summaryPath", "eventsPath", "consolePath"):
        value = output_paths.get(key)
        if value:
            lines.append(str(value))
    lines.append("")
    lines.append("Enter to run. Esc/Q to return.")
    return "\n".join(lines) + "\n"


def run_test_all_preview(
    host_platform: str,
    *,
    terminal: "_TerminalSession" | None = None,
) -> list[str] | None:
    preview = _build_test_all_preview_payload(host_platform)
    active_terminal = terminal
    created_terminal = False
    if active_terminal is None:
        active_terminal = _TerminalSession()
        created_terminal = True

    def _run_loop(current_terminal: "_TerminalSession") -> list[str] | None:
        while True:
            current_terminal.render(render_test_all_preview_screen(preview))
            key = current_terminal.read_key()
            if key == "enter":
                return ["test", "all"]
            if key in {"escape", "quit"}:
                return None

    if created_terminal:
        with active_terminal as current_terminal:
            return _run_loop(current_terminal)
    return _run_loop(active_terminal)


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
    detail_lines = _build_menu_detail_lines(entries[selection].command if entries else {})
    if fullscreen:
        visible_body_height = max(6, height - 5 - len(detail_lines) - (1 if detail_lines else 0))
    else:
        visible_body_height = min(len(rows), max(6, min(14, height - 5 - len(detail_lines) - (1 if detail_lines else 0))))
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
    if detail_lines:
        lines.append("")
        lines.extend(_trim(detail_line, width) for detail_line in detail_lines)
    if fullscreen:
        while len(lines) < height:
            lines.append("")
        return "\x1b[2J\x1b[H" + "\n".join(lines[:height])
    return "\n".join(lines)


def _build_menu_detail_lines(command: dict[str, Any]) -> list[str]:
    details = dict(command.get("details") or {})
    if not details:
        return []

    lines = [_style_workspace_heading("选中项说明")]
    ordered_fields = [
        ("简介", "summary"),
        ("输出", "outputs"),
        ("目标范围", "target_scope"),
        ("当前宿主支持", "host_support"),
        ("执行命令", "command"),
        ("备注", "notes"),
    ]
    for label, key in ordered_fields:
        value = details.get(key)
        if value:
            lines.append(f"{label}: {value}")
    return lines


def render_test_progress_screen(events: list[dict[str, Any]], repo_root: Path | None = None) -> str:
    view = build_test_progress_view(events, repo_root=repo_root)
    lines = [
        "Unified Test Progress",
        f"Command: {view.command or '-'}",
        f"Status: {view.final_status}",
        f"Progress: {_green(view.progress_text)}",
    ]
    if view.batch_lines:
        lines.extend(["", "Batch:"])
        lines.extend(view.batch_lines)
    if view.count_lines:
        lines.extend(["", "Counts:"])
        lines.extend(view.count_lines)
    if view.current_lines:
        lines.extend(["", "Current:"])
        lines.extend(view.current_lines)
    if view.failure_lines:
        lines.extend(["", "Failures:"])
        lines.extend(view.failure_lines)

    lines.extend(["", "Timeline:"])
    lines.extend(view.history_lines or ["[  0%] waiting for test events"])

    if view.phase_lines:
        lines.append("")
        lines.append("Phases:")
        lines.extend(view.phase_lines)
    if view.subject_lines:
        lines.append("")
        lines.append("Subjects:")
        lines.extend(view.subject_lines)

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
    batch_lines: list[str] = []
    count_lines: list[str] = []
    current_lines: list[str] = []
    failure_lines: list[str] = []
    history_lines: list[str] = []
    phase_lines: list[str] = []
    subject_lines: list[str] = []
    important_lines: list[str] = []
    collected_artifacts: list[str] = []
    errors: list[str] = []
    seen_history: set[str] = set()
    seen_important: set[tuple[str, str]] = set()
    seen_artifacts: set[str] = set()
    active_unit = ""
    active_unit_context: dict[str, Any] = {}

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
            active_unit = str(payload.get("activeUnit") or "")
            active_unit_context = dict(payload.get("activeUnitContext") or {})
            if active_unit:
                _append_unique(
                    history_lines,
                    seen_history,
                    f"[{_format_progress_label(progress_text)}] run    {active_unit}",
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
            batch_lines = _build_batch_lines(payload)
            count_lines = _build_count_lines(payload)
            current_lines = _build_current_lines(active_unit, active_unit_context)
            failure_lines = _build_failure_lines(payload)
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
        batch_lines=batch_lines,
        count_lines=count_lines,
        current_lines=current_lines,
        failure_lines=failure_lines,
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


def _format_planned_counts(planned_counts: Any) -> str:
    counts = dict(planned_counts or {})
    total_units = int(counts.get("totalUnits", 0))
    suite_count = int(counts.get("suiteCount", 0))
    subject_count = int(counts.get("subjectCount", 0))
    if total_units <= 0 and suite_count <= 0 and subject_count <= 0:
        return ""
    return f"Planned: {total_units} = {suite_count} suites + {subject_count} subjects"


def _format_family_totals(counts_by_family: Any) -> str:
    families = dict(counts_by_family or {})
    parts = [f"{family} {int(dict(families[family] or {}).get('total', 0))}" for family in sorted(families)]
    return " | ".join(parts)


def _build_batch_lines(payload: dict[str, Any]) -> list[str]:
    lines: list[str] = []
    planned_line = _format_planned_counts(payload.get("plannedCounts"))
    if planned_line:
        lines.append(planned_line)
    family_line = _format_family_totals(payload.get("countsByFamily"))
    if family_line:
        lines.append(f"Families: {family_line}")
    return lines


def _build_count_lines(payload: dict[str, Any]) -> list[str]:
    counts_by_type = dict(payload.get("countsByType") or {})
    ordered_types = [item_type for item_type in ("suite", "subject") if item_type in counts_by_type]
    ordered_types.extend(item_type for item_type in counts_by_type if item_type not in {"suite", "subject"})
    lines: list[str] = []
    for item_type in ordered_types:
        counts = dict(counts_by_type.get(item_type) or {})
        lines.append(
            f"{item_type}: total {counts.get('total', 0)} | ok {counts.get('ok', 0)} | "
            f"fail {counts.get('fail', 0)} | skip {counts.get('skip', 0)} | aborted {counts.get('aborted', 0)}"
        )
    return lines


def _build_current_lines(active_unit: str, active_unit_context: dict[str, Any]) -> list[str]:
    if not active_unit and not active_unit_context:
        return []
    lines: list[str] = []
    if active_unit:
        lines.append(f"id: {active_unit}")
    if str(active_unit_context.get("type") or "") == "subject":
        parts = [
            f"subject={active_unit_context.get('subjectId') or '-'}",
            f"goal={active_unit_context.get('goalId') or '-'}",
            f"matrix={active_unit_context.get('matrixId') or '-'}",
        ]
        lines.append(" | ".join(parts))
    elif active_unit_context:
        parts = [
            f"family={active_unit_context.get('family') or '-'}",
            f"level={active_unit_context.get('level') or '-'}",
            f"module={active_unit_context.get('primaryModuleId') or '-'}",
        ]
        lines.append(" | ".join(parts))
    return lines


def _build_failure_lines(payload: dict[str, Any]) -> list[str]:
    lines: list[str] = []
    for failure_item in list(payload.get("failureItems") or []):
        lines.append(
            f"{failure_item.get('type', '-')}: {failure_item.get('status', '-')} | {failure_item.get('id', '-')}"
        )
        if failure_item.get("rerunCommand"):
            lines.append(f"rerun: {failure_item['rerunCommand']}")
        if failure_item.get("reportPath"):
            lines.append(f"report: {failure_item['reportPath']}")
        if failure_item.get("subjectSummaryPath"):
            lines.append(f"summary: {failure_item['subjectSummaryPath']}")
    return lines


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
