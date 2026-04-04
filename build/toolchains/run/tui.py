from __future__ import annotations

import os
import shutil
import sys
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


@dataclass(frozen=True)
class MenuEntry:
    group_title: str
    command: dict[str, Any]
    syntax: str
    argv: list[str]


def build_menu_entries(manifest: dict[str, Any], host_platform: str) -> list[MenuEntry]:
    entries: list[MenuEntry] = []
    for group in manifest["groups"]:
        title = group["title"]
        commands = [
            command
            for command in manifest_module.list_commands_by_group(manifest, title, host_platform)
            if command.get("show_in_menu", True)
        ]
        for command in commands:
            entries.append(
                MenuEntry(
                    group_title=title,
                    command=command,
                    syntax=manifest_module.command_syntax(command),
                    argv=manifest_module.command_argv(command),
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

    selection = 0
    with _TerminalSession() as terminal:
        while True:
            terminal.render(render_menu_screen(entries, selection))
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
                break

    return resolve_entry_argv(entries[selection])


def render_menu_screen(entries: list[MenuEntry], selection: int) -> str:
    width, height = shutil.get_terminal_size(fallback=(100, 30))
    rows = _build_rows(entries)
    selected_row = _find_selected_row(rows, selection)
    visible_body_height = max(6, height - 5)
    scroll_top = _compute_scroll_top(selected_row, visible_body_height, len(rows))
    visible_rows = rows[scroll_top : scroll_top + visible_body_height]
    syntax_width = max(18, min(42, width // 3))

    header = [
        _trim("Unified Run Menu", width),
        _trim("Use Up/Down to move, Enter to run, PgUp/PgDn to jump groups, Home/End to jump, q/Esc to exit.", width),
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


def render_test_progress_screen(events: list[dict[str, Any]]) -> str:
    command = ""
    progress_text = "0%"
    warnings: list[str] = []
    artifacts: list[str] = []
    final_status = "running"

    for event in events:
        event_type = event.get("eventType")
        payload = event.get("payload") or {}

        if event_type == "session-start":
            command = str(payload.get("command") or command)
            continue

        if event_type == "progress":
            completed = payload.get("completedUnits")
            total = payload.get("totalUnits")
            if isinstance(completed, int) and isinstance(total, int) and total > 0:
                progress_text = f"{int((completed / total) * 100)}%"
            continue

        if event_type == "warning":
            message = payload.get("message")
            if message:
                warnings.append(str(message))
            continue

        if event_type == "artifact":
            path = payload.get("path")
            if path:
                artifacts.append(str(path))
            continue

        if event_type == "final-summary":
            final_status = str(payload.get("finalStatus") or final_status)

    lines = [
        "Unified Test Progress",
        f"Command: {command or '-'}",
        f"Progress: {progress_text}",
        f"Status: {final_status}",
    ]

    if warnings:
        lines.append("Warnings:")
        lines.extend(warnings)

    if artifacts:
        lines.append("Artifacts:")
        lines.extend(artifacts)

    return "\n".join(lines) + "\n"


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
