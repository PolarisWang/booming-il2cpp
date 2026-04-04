from __future__ import annotations

import os
import shutil
import sys
from dataclasses import dataclass
from pathlib import Path
from urllib.parse import quote
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
ANSI_RESET = "\x1b[0m"


@dataclass(frozen=True)
class MenuEntry:
    group_title: str
    command: dict[str, Any]
    syntax: str
    argv: list[str]


TEST_MENU_COMMAND_IDS = {"test-family-suite", "test-family-all", "test-all", "test-list", "test-watch", "test-summary"}
PRIMARY_MENU_ENTRIES = [
    {
        "id": "prepare-menu",
        "group_title": "Prepare",
        "syntax": "prepare",
        "title": "Prepare this workspace and local host environment",
    },
    {
        "id": "build-menu",
        "group_title": "Build",
        "syntax": "build",
        "title": "Build runtime contracts, reference presets, and platform routing targets",
    },
    {
        "id": "test-menu",
        "group_title": "Test",
        "syntax": "test",
        "title": "Run the unified test center with live progress and reports",
    },
    {
        "id": "clean-menu",
        "group_title": "Clean",
        "syntax": "clean",
        "title": "Clean managed outputs, temporary artifacts, and cached build products",
    },
    {
        "id": "inspect-menu",
        "group_title": "Inspect",
        "syntax": "inspect",
        "title": "Inspect help, capabilities, and the public test catalog",
    },
]
MENU_BACK_COMMAND = {"id": "menu-back", "title": "Return to the main run menu"}
PRIMARY_MENU_HELP = "Use Up/Down to move, Enter to run, PgUp/PgDn to jump groups, Home/End to jump, q/Esc to exit."
TEST_MENU_HELP = "Use Up/Down to move, Enter to continue, q/Esc to return to the main run menu."
SECTION_MENU_HELP = "Use Up/Down to move, Enter to continue, q/Esc to return to the main run menu."


@dataclass(frozen=True)
class TestProgressView:
    command: str
    progress_text: str
    final_status: str
    history_lines: list[str]
    important_lines: list[str]
    artifact_lines: list[str]
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
            ("Core", "doctor", "doctor", "Check the local toolchain before running anything else"),
            ("Core", "prepare", "host", "Prepare the reusable host environment for everyday work"),
            ("Core", "bootstrap", "python", "Bootstrap the cached Python runtime required by the wrapper"),
            ("Targets", "prepare-smoke", "smoke", "Prepare the managed smoke environment for quick validation"),
            (
                "Targets",
                f"prepare-verify-roadmap-0-{host_platform}",
                "roadmap-0",
                "Prepare the roadmap-0 verification environment for this host",
            ),
        ],
    )


def build_build_menu_entries(manifest: dict[str, Any], host_platform: str) -> list[MenuEntry]:
    preset_by_host = {
        "windows": "build-preset-windows-x64-reference",
        "macos": "build-preset-macos-reference",
    }
    return _build_curated_submenu_entries(
        manifest,
        host_platform,
        [
            ("Contracts", "build-native-contract-abi", "abi", "Build the native ABI contract smoke target"),
            ("Contracts", "build-native-contract-bridge", "bridge", "Build the native bridge contract smoke target"),
            ("Presets", preset_by_host.get(host_platform), "reference", "Build the reference preset for this host"),
            ("Platform Routing", "build-platform-android-arm64-smoke", "android", "Validate Android platform routing"),
            ("Platform Routing", "build-platform-ios-arm64-packaging", "ios", "Validate iOS platform routing"),
            ("Platform Routing", "build-platform-linux-x64-packaging", "linux", "Validate Linux platform routing"),
        ],
    )


def build_clean_menu_entries(manifest: dict[str, Any], host_platform: str) -> list[MenuEntry]:
    return _build_curated_submenu_entries(
        manifest,
        host_platform,
        [
            ("Scopes", "clean", "all", "Clean managed outputs, temporary artifacts, and cached build products"),
            ("Scopes", "clean-smoke", "smoke", "Clean the smoke-test outputs only"),
            ("Scopes", f"clean-verify-roadmap0-{host_platform}", "roadmap-0", "Clean roadmap-0 verification outputs for this host"),
        ],
    )


def build_inspect_menu_entries(manifest: dict[str, Any], host_platform: str) -> list[MenuEntry]:
    return _build_curated_submenu_entries(
        manifest,
        host_platform,
        [
            ("Reference", "help", "help", "Show the unified command help and quick syntax guide"),
            ("Reference", "capability", "capability", "Describe one capability in detail"),
            ("Reference", "list", "catalog", "Browse the available capability catalog"),
            ("Testing", "test-list", "tests", "Browse the public test suites available on this host"),
        ],
    )


def build_test_menu_entries(manifest: dict[str, Any], host_platform: str) -> list[MenuEntry]:
    del manifest
    del host_platform
    return [
        MenuEntry(
            group_title="Execute",
            command={"id": "test-all", "title": "Run the full test center matrix with live progress and end-of-run reports"},
            syntax="all",
            argv=["test", "all"],
        ),
        MenuEntry(
            group_title="Execute",
            command={"id": "test-family-all", "title": "Run every suite in one family when you want a focused regression pass"},
            syntax="family",
            argv=["test"],
        ),
        MenuEntry(
            group_title="Execute",
            command={"id": "test-family-suite", "title": "Run one named suite for the fastest targeted validation loop"},
            syntax="suite",
            argv=["test"],
        ),
        MenuEntry(
            group_title="Observe",
            command={"id": "test-watch", "title": "Watch the active or most recent run as a readable event timeline"},
            syntax="watch",
            argv=["test", "watch"],
        ),
        MenuEntry(
            group_title="Observe",
            command={"id": "test-summary", "title": "Open the latest aggregated summary with suite-level outcomes"},
            syntax="summary",
            argv=["test", "summary"],
        ),
        MenuEntry(
            group_title="Observe",
            command={"id": "test-list", "title": "Browse the public suite catalog available on this host"},
            syntax="list",
            argv=["test", "list"],
        ),
        MenuEntry(
            group_title="Navigation",
            command=dict(MENU_BACK_COMMAND),
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
            group_title="Navigation",
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
            group_title="Navigation",
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
        value = prompt_value_provider("Capability id (blank to cancel): ").strip()
        if not value:
            return None
        return ["capability", value]

    if command_id == "test-menu":
        mode = prompt_value_provider("Test mode (suite/family-all/all/list/watch/summary, blank to cancel): ").strip().lower()
        if not mode:
            return None
        if mode == "suite":
            value = prompt_value_provider("Test family and suite (for example: smoke HelloWorld): ").strip()
            parts = value.split()
            if len(parts) != 2:
                return None
            return ["test", parts[0], parts[1]]
        if mode == "family-all":
            value = prompt_value_provider("Test family (blank to cancel): ").strip()
            if not value:
                return None
            return ["test", value, "all"]
        if mode == "all":
            return ["test", "all"]
        if mode == "list":
            value = prompt_value_provider("Optional test family (blank for all): ").strip()
            return ["test", "list", value] if value else ["test", "list"]
        if mode == "watch":
            return ["test", "watch"]
        if mode == "summary":
            return ["test", "summary"]
        return None

    if command_id == "test-family-suite":
        value = prompt_value_provider("Test family and suite (for example: smoke HelloWorld): ").strip()
        parts = value.split()
        if len(parts) != 2:
            return None
        return ["test", parts[0], parts[1]]

    if command_id == "test-family-all":
        value = prompt_value_provider("Test family (blank to cancel): ").strip()
        if not value:
            return None
        return ["test", value, "all"]

    if command_id == "test-all":
        return ["test", "all"]

    if command_id == "test-list":
        value = prompt_value_provider("Optional test family (blank for all): ").strip()
        return ["test", "list", value] if value else ["test", "list"]

    return list(entry.argv)


def supports_fullscreen_tui() -> bool:
    if not _stream_is_tty("stdin") or not _stream_is_tty("stdout"):
        return False

    return _enable_virtual_terminal_output()


def run_fullscreen_menu(manifest: dict[str, Any], host_platform: str) -> list[str] | None:
    entries = build_menu_entries(manifest, host_platform)
    if not entries:
        return None

    with _TerminalSession() as terminal:
        while True:
            selected_entry = _run_menu_selection(
                terminal,
                entries,
                title="Workspace Control Center",
                help_text=PRIMARY_MENU_HELP,
            )
            if selected_entry is None:
                return None
            if not selected_entry.command["id"].endswith("-menu"):
                return resolve_entry_argv(selected_entry)

            submenu_argv = run_section_submenu(selected_entry.command["id"], manifest, host_platform, terminal=terminal)
            if submenu_argv is not None:
                return submenu_argv


def run_section_submenu(
    section_command_id: str,
    manifest: dict[str, Any],
    host_platform: str,
    *,
    terminal: "_TerminalSession" | None = None,
) -> list[str] | None:
    if section_command_id == "prepare-menu":
        entries = build_prepare_menu_entries(manifest, host_platform)
        title = "Prepare Center"
        help_text = SECTION_MENU_HELP
    elif section_command_id == "build-menu":
        entries = build_build_menu_entries(manifest, host_platform)
        title = "Build Center"
        help_text = SECTION_MENU_HELP
    elif section_command_id == "test-menu":
        return run_test_submenu(manifest, host_platform, terminal=terminal)
    elif section_command_id == "clean-menu":
        entries = build_clean_menu_entries(manifest, host_platform)
        title = "Clean Center"
        help_text = SECTION_MENU_HELP
    elif section_command_id == "inspect-menu":
        entries = build_inspect_menu_entries(manifest, host_platform)
        title = "Inspect Center"
        help_text = SECTION_MENU_HELP
    else:
        return None

    return _run_submenu(entries, title=title, help_text=help_text, terminal=terminal)


def run_test_submenu(
    manifest: dict[str, Any],
    host_platform: str,
    *,
    terminal: "_TerminalSession" | None = None,
) -> list[str] | None:
    entries = build_test_menu_entries(manifest, host_platform)
    return _run_submenu(entries, title="Unified Test Menu", help_text=TEST_MENU_HELP, terminal=terminal)


def _run_submenu(
    entries: list[MenuEntry],
    *,
    title: str,
    help_text: str,
    terminal: "_TerminalSession" | None = None,
) -> list[str] | None:
    if not entries:
        return None

    if terminal is not None:
        selected_entry = _run_menu_selection(
            terminal,
            entries,
            title=title,
            help_text=help_text,
        )
        if selected_entry is None or selected_entry.command["id"] == "menu-back":
            return None
        return resolve_entry_argv(selected_entry)

    with _TerminalSession() as session:
        selected_entry = _run_menu_selection(
            session,
            entries,
            title=title,
            help_text=help_text,
        )
    if selected_entry is None or selected_entry.command["id"] == "menu-back":
        return None
    return resolve_entry_argv(selected_entry)


def _run_menu_selection(
    terminal: "_TerminalSession",
    entries: list[MenuEntry],
    *,
    title: str,
    help_text: str,
) -> MenuEntry | None:
    selection = 0
    while True:
        terminal.render(render_menu_screen(entries, selection, title=title, help_text=help_text))
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
) -> str:
    width, height = shutil.get_terminal_size(fallback=(100, 30))
    rows = _build_rows(entries)
    selected_row = _find_selected_row(rows, selection)
    visible_body_height = max(6, height - 5)
    scroll_top = _compute_scroll_top(selected_row, visible_body_height, len(rows))
    visible_rows = rows[scroll_top : scroll_top + visible_body_height]
    syntax_width = max(18, min(42, width // 3))

    header = [
        _trim(title, width),
        _trim(help_text, width),
        _trim(f"{selection + 1}/{len(entries)}", width),
        "",
    ]

    body: list[str] = []
    for row in visible_rows:
        if row["entry_index"] is None:
            body.append(_trim(row["text"], width))
            continue

        entry = entries[row["entry_index"]]
        prefix = "> " if row["entry_index"] == selection else "  "
        syntax = _pad(_trim(entry.syntax, max(10, syntax_width)), syntax_width)
        title = _trim(entry.command["title"], max(0, width - len(prefix) - syntax_width - 2))
        line = _trim(f"{prefix}{syntax}  {title}", width)
        if row["entry_index"] == selection:
            body.append(f"\x1b[7m{_pad(line, width)}\x1b[0m")
        else:
            body.append(line)

    lines = header + body
    while len(lines) < height:
        lines.append("")

    return "\x1b[2J\x1b[H" + "\n".join(lines[:height])


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
        lines.append(_highlight_heading("Important outputs:"))
        lines.extend(view.important_lines)

    if view.artifact_lines:
        lines.append("")
        lines.append(_highlight_label(f"Artifacts ({len(view.artifact_lines)}):"))
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
    important_lines: list[str] = []
    artifact_lines: list[str] = []
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
                _append_artifact_line(artifact_lines, seen_artifacts, str(path), repo_root=repo_root)
            continue

        if event_type == "final-summary":
            final_status = str(payload.get("finalStatus") or final_status)
            _append_unique(history_lines, seen_history, f"[{_format_progress_label(progress_text)}] done   {final_status}")
            errors.extend(str(error) for error in list(payload.get("errors") or []))
            important_lines.extend(build_test_report_highlight_lines(payload, repo_root=repo_root, seen=seen_important))
            artifact_lines.extend(build_artifact_lines(payload, repo_root=repo_root, seen=seen_artifacts))

    return TestProgressView(
        command=command,
        progress_text=progress_text,
        final_status=final_status,
        history_lines=history_lines,
        important_lines=important_lines,
        artifact_lines=artifact_lines,
        errors=errors,
    )


def render_test_report_highlights(payload: dict[str, Any], repo_root: Path | None = None) -> str:
    important_lines = build_test_report_highlight_lines(payload, repo_root=repo_root)
    artifact_lines = build_artifact_lines(payload, repo_root=repo_root)
    if not important_lines and not artifact_lines:
        return ""
    lines = [_highlight_heading("Important outputs:")]
    lines.extend(important_lines)
    if artifact_lines:
        lines.append("")
        lines.append(_highlight_label(f"Artifacts ({len(artifact_lines)}):"))
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
    return lines


def build_artifact_lines(
    payload: dict[str, Any],
    *,
    repo_root: Path | None = None,
    seen: set[str] | None = None,
) -> list[str]:
    lines: list[str] = []
    seen = seen or set()
    for artifact in list(payload.get("artifacts") or []):
        _append_artifact_line(lines, seen, artifact, repo_root=repo_root)
    return lines


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


def _highlight_heading(text: str) -> str:
    return f"{ANSI_BOLD_YELLOW}{text}{ANSI_RESET}"


def _highlight_label(text: str) -> str:
    return f"{ANSI_BOLD_CYAN}{text}{ANSI_RESET}"


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


def _append_artifact_line(
    lines: list[str],
    seen: set[str],
    value: Any,
    *,
    repo_root: Path | None = None,
) -> None:
    if not value:
        return
    text = str(value)
    if text in seen:
        return
    seen.add(text)
    lines.append(f"  {len(lines) + 1}. {_terminal_link(text, repo_root)}")


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
    return f"file://{quote(candidate.as_posix())}"


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


def _trim(text: str, width: int) -> str:
    if width <= 0:
        return ""
    if len(text) <= width:
        return text
    if width <= 3:
        return text[:width]
    return text[: width - 3] + "..."


def _pad(text: str, width: int) -> str:
    if len(text) >= width:
        return text
    return text + (" " * (width - len(text)))


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
