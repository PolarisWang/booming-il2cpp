from __future__ import annotations

from typing import Any

ANSI_GREEN = "\x1b[32m"
ANSI_BOLD_YELLOW = "\x1b[1;33m"
ANSI_BOLD_CYAN = "\x1b[1;36m"
ANSI_BRIGHT_WHITE = "\x1b[97m"
ANSI_RESET = "\x1b[0m"

GROUP_TITLE_ALIASES = {
    "Quick Start": "快速开始",
    "Build": "构建产物",
    "Test And Verify": "测试验证",
    "Environment": "环境准备",
    "Inspect": "信息查看",
}

COMMAND_TITLE_ALIASES = {
    "list": "浏览当前可用能力列表",
    "doctor": "检查当前本机可用工具链",
    "help": "查看统一入口帮助",
    "capability": "查看单个能力说明",
    "bootstrap": "安装缓存 Python 运行时",
    "prepare": "准备当前主机的可复用环境",
    "prepare-smoke": "准备 smoke 测试环境",
    "prepare-verify-roadmap-0-windows": "准备 Windows roadmap-0 验证环境",
    "prepare-verify-roadmap-0-macos": "准备 macOS roadmap-0 验证环境",
    "clean": "清理统一入口产生的全部输出",
    "clean-smoke": "清理 smoke 测试输出",
    "clean-verify-roadmap0-windows": "清理 Windows roadmap-0 验证输出",
    "clean-verify-roadmap0-macos": "清理 macOS roadmap-0 验证输出",
    "build-all": "执行当前主机推荐的全量构建",
    "build-native-contract-abi": "构建原生 ABI 契约目标",
    "build-native-contract-bridge": "构建原生 bridge 契约目标",
    "build-preset-windows-x64-reference": "构建 Windows 参考预设",
    "build-preset-macos-reference": "构建 macOS 参考预设",
    "build-platform-android-arm64-smoke": "校验 Android 启动 smoke 路由",
    "build-platform-ios-arm64-packaging": "校验 iOS 打包路由",
    "build-platform-linux-x64-packaging": "校验 Linux 打包路由",
    "test-family-suite": "运行一个公开测试套件",
    "test-family-all": "按测试族批量执行公开测试",
    "test-all": "执行默认公开测试矩阵",
    "test-list": "浏览公开测试套件列表",
}


def render_menu(manifest: dict[str, Any], visible_commands_by_group: dict[str, list[dict[str, Any]]]) -> str:
    lines = [f"{ANSI_BOLD_YELLOW}统一运行菜单{ANSI_RESET}", ""]

    for group in manifest["groups"]:
        title = group["title"]
        commands = visible_commands_by_group.get(title, [])
        if not commands:
            continue

        lines.append(f"{ANSI_BOLD_CYAN}{GROUP_TITLE_ALIASES.get(title, title)}{ANSI_RESET}")
        for command in commands:
            command_title = _style_command_title(COMMAND_TITLE_ALIASES.get(command["id"], command["title"]))
            lines.append(f"  {ANSI_GREEN}{command['id']}{ANSI_RESET}: {command_title}")
        lines.append("")

    return "\n".join(lines).rstrip() + "\n"


def _style_command_title(text: str) -> str:
    if _contains_cjk(text):
        return f"{ANSI_BRIGHT_WHITE}{text}{ANSI_RESET}"
    return f"{ANSI_BRIGHT_WHITE}{text}{ANSI_RESET}"


def _contains_cjk(text: str) -> bool:
    return any("\u4e00" <= character <= "\u9fff" for character in text)
