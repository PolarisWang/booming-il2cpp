"""PreToolUse hook: 强制 Expert 加载验证 (A+B+方案4)

方案4: Bash 无豁免。只读 Bash 放行，管理 Bash (.claude/) 放行，
域操作 Bash 必须匹配 loaded_expert。

流程:
1. 用户发消息 → Agent 输出分类声明 + "→ 加载 dev-xxx-expert"
2. echo "..." > .claude/.classified (hook 验证格式)
3. Agent 通过 Skill 工具加载 Expert → hook 自动写入 .loaded_expert
4. 后续 Edit/Write/Bash 域文件 → hook 检查 loaded_expert 是否匹配
"""

import os
import re
import subprocess
import sys
from pathlib import Path


_RESOLVED_REPO_ROOT: Path | None = None

def _get_repo_root() -> Path | None:
    global _RESOLVED_REPO_ROOT
    if _RESOLVED_REPO_ROOT is not None:
        return _RESOLVED_REPO_ROOT
    try:
        script_dir = Path(__file__).resolve().parent
        output = subprocess.run(
            ["git", "-C", str(script_dir), "rev-parse", "--show-toplevel"],
            capture_output=True, text=True, timeout=5,
        )
        root = output.stdout.strip()
        _RESOLVED_REPO_ROOT = Path(root).resolve() if root else None
        return _RESOLVED_REPO_ROOT
    except Exception:
        return None


_claude_dir = Path(__file__).resolve().parent.parent.parent / ".claude"
if not _claude_dir.exists():
    repo_root = _get_repo_root()
    if repo_root:
        _claude_dir = repo_root / ".claude"
    else:
        _claude_dir = Path.cwd() / ".claude"

flag_file = _claude_dir / ".classified"
loaded_expert_file = _claude_dir / ".loaded_expert"

tool_name = os.environ.get("CLAUDE_TOOL_NAME", sys.argv[1] if len(sys.argv) > 1 else "unknown")


# ═══════════════════════════════════════════════════════════════════
# 文件路径 → Expert 映射表
# ═══════════════════════════════════════════════════════════════════
FILE_TO_EXPERT: list[tuple[str, str]] = [
    # GC 必须先于 runtime-core（gc/ 是 runtime-core/ 的子路径）
    ("src/native/runtime-core/gc/",          "dev-il2cpp-gc-expert"),
    ("src/native/runtime-core/",             "dev-il2cpp-runtime-expert"),
    ("src/native/interpreter/",              "dev-il2cpp-runtime-expert"),
    ("src/native/bootstrap/",                "dev-il2cpp-runtime-expert"),
    ("src/native/support/",                  "dev-il2cpp-runtime-expert"),
    ("src/native/jit/",                      "dev-il2cpp-jit-expert"),
    ("src/native/pal/",                      "dev-il2cpp-platform-expert"),
    ("CMakePresets.json",                    "dev-il2cpp-platform-expert"),
    ("build/toolchains/",                    "dev-il2cpp-platform-expert"),
    (".github/workflows/",                   "dev-il2cpp-platform-expert"),
    ("src/managed/Chaos.IL2CPP.Generator/",  "dev-il2cpp-codegen-expert"),
    ("testing/foundation-dll/",              "dev-il2cpp-fact-verification-expert"),
    ("testing/foundation-dll/verification/stages/build.py", "dev-il2cpp-build-fixer"),
    ("src/tools/Chaos.IL2CPP.Tools.AutoTestGenerator/",  "dev-il2cpp-build-fixer"),
    ("src/tools/Chaos.IL2CPP.Tools.TestProjectGenerator/", "dev-il2cpp-build-fixer"),
    ("src/native/hot-update/",               "dev-il2cpp-hotupdate-expert"),
    # build/ 映射到对应源域的 Expert（构建产物对应源码域）
    ("build/native/",                        "dev-il2cpp-runtime-expert"),
    ("build/native-profile/",                "dev-il2cpp-runtime-expert"),
]


def get_required_expert(file_path: str) -> str | None:
    normalized = file_path.replace("\\", "/")
    for prefix, expert in FILE_TO_EXPERT:
        if normalized.startswith(prefix):
            return expert
    return None


def get_loaded_expert() -> str | None:
    if loaded_expert_file.exists():
        expert = loaded_expert_file.read_text(encoding="utf-8", errors="replace").strip()
        return expert if expert else None
    return None


# ═══════════════════════════════════════════════════════════════════
# 只读工具豁免
# ═══════════════════════════════════════════════════════════════════
ALLOWED_TOOLS = {"Read", "Grep", "Glob"}
if tool_name in ALLOWED_TOOLS:
    sys.exit(0)


# ═══════════════════════════════════════════════════════════════════
# Bash 处理 — 三档过滤
# ═══════════════════════════════════════════════════════════════════
if tool_name == "Bash":
    cmd = " ".join(sys.argv[1:]) if len(sys.argv) > 1 else ""

    # 第一档: 管理 Bash — 只操作 .claude/ 标记文件，不检查
    MANAGEMENT_PATTERNS = [
        r'echo\s+.*>.*\.claude/',
        r'echo\s+.*>.*\.loaded_expert',
        r'echo\s+.*>.*\.classified',
        r'rm\s+-f.*\.claude/',
        r'touch\s+.*\.claude/',
    ]
    for pat in MANAGEMENT_PATTERNS:
        if re.search(pat, cmd):
            sys.exit(0)

    # 第二档: 只读 Bash — 不影响源文件，不检查
    READONLY_BASH_WORDS = {
        "ls", "pwd", "which", "type", "date", "printf",
        "cat", "head", "tail", "wc", "sort", "uniq", "cut",
        "file", "du", "df", "ps", "env", "which",
        "git status", "git log", "git diff", "git show", "git branch",
        "git ls-files", "git ls-tree",
    }
    first_word = cmd.strip().split()[0] if cmd.strip() else ""
    if first_word in READONLY_BASH_WORDS:
        sys.exit(0)

    # 第三档: 域操作 Bash — 检查 loaded_expert 是否匹配
    # 从命令中提取文件/目录路径参数
    cmd_paths = re.findall(r'(?:^|\s)(?:cd\s+)?([a-zA-Z]:/[^\s"\']+|\.\.?/[^\s"\']+)', cmd)
    cmd_paths += re.findall(r'(?:^|\s)(?:cd\s+)?([a-zA-Z]:\\[^\s"\']+)', cmd)

    # 处理 cd X && Y 模式 — 提取 X 下的所有路径
    cd_match = re.search(r'cd\s+([^\s&;|]+)\s*(?:&&|;)', cmd)
    if cd_match:
        cd_base = cd_match.group(1).replace("\\", "/")
        cmd_paths.append(cd_base + "/")

    required_experts: set[str] = set()
    for p in cmd_paths:
        expert = get_required_expert(p)
        if expert:
            required_experts.add(expert)

    if required_experts:
        loaded = get_loaded_expert()
        if loaded is None or loaded not in required_experts:
            expert_list = " 或 ".join(required_experts)
            print(file=sys.stderr)
            print(f"  ⚠️  此 Bash 操作涉及域文件，需要先加载对应 Expert", file=sys.stderr)
            print(f"  需加载: {expert_list}", file=sys.stderr)
            print(f"  当前已加载: {loaded or '无'}", file=sys.stderr)
            print(f"  请先执行: Skill(\"{list(required_experts)[0]}\")", file=sys.stderr)
            print(file=sys.stderr)
            sys.exit(1)

    # 无域路径匹配 → 放行（可能是工具命令、第三方脚本等）
    sys.exit(0)


# ═══════════════════════════════════════════════════════════════════
# 分类声明 + Expert 声明验证（Edit/Write/Skill/Workflow）
# ═══════════════════════════════════════════════════════════════════
if not flag_file.exists():
    print(file=sys.stderr)
    print("  ⚠️  分类声明未输出！", file=sys.stderr)
    print(file=sys.stderr)
    sys.exit(1)

classification = flag_file.read_text(encoding="utf-8", errors="replace").strip()
if not classification:
    print("  ⚠️  分类文件为空！", file=sys.stderr)
    sys.exit(1)

pattern = r'本轮任务涉及\s+(.+?)\s*，\s*(\w+)\s*操作，\s*第\s*(\d+)\s*轮\s*→\s*加载\s+(dev-\S+)'
m = re.match(pattern, classification)
if not m:
    print(f"  ⚠️  分类声明缺少 Expert 加载声明", file=sys.stderr)
    print("  格式: 本轮任务涉及 CodeGen(4) ，fix 操作，第 1 轮 → 加载 dev-il2cpp-codegen-expert", file=sys.stderr)
    sys.exit(1)

domain_part = m.group(1)
action = m.group(2)
round_str = m.group(3)
declared_expert = m.group(4)

VALID_ACTIONS = {"read", "fix", "build", "verify", "plan"}
if action not in VALID_ACTIONS:
    print(f"  ⚠️  无效 action '{action}'", file=sys.stderr)
    sys.exit(1)

domain_nums = re.findall(r'\((\d+)\)', domain_part)
for dn in domain_nums:
    num = int(dn)
    if num < 1 or num > 8:
        print(f"  ⚠️  无效域编号 {num}", file=sys.stderr)
        sys.exit(1)

round_num = int(round_str)
if round_num < 1:
    print(f"  ⚠️  无效轮次 {round_num}", file=sys.stderr)
    sys.exit(1)

# ═══════════════════════════════════════════════════════════════════
# Skill 工具 → 自动写入 .loaded_expert
# ═══════════════════════════════════════════════════════════════════
if tool_name == "Skill":
    skill_arg = ""
    for i, arg in enumerate(sys.argv):
        if arg == "--skill" and i + 1 < len(sys.argv):
            skill_arg = sys.argv[i + 1]
            break
    if skill_arg and skill_arg.startswith("dev-"):
        loaded_expert_file.write_text(skill_arg + "\n", encoding="utf-8")
    sys.exit(0)

# ═══════════════════════════════════════════════════════════════════
# Edit/Write → 检查 Expert 是否已加载
# ═══════════════════════════════════════════════════════════════════
if tool_name in ("Edit", "Write"):
    file_path = ""
    for i, arg in enumerate(sys.argv):
        if arg == "--file-path" and i + 1 < len(sys.argv):
            file_path = sys.argv[i + 1]
            break
    if not file_path:
        file_path = os.environ.get("CLAUDE_TOOL_FILE_PATH", "")
    required = get_required_expert(file_path)
    if required is not None:
        loaded = get_loaded_expert()
        if loaded != required:
            print(file=sys.stderr)
            print(f"  ⚠️  编辑域文件需要先加载 {required}", file=sys.stderr)
            print(f"  当前已加载: {loaded or '无'}", file=sys.stderr)
            sys.exit(1)

# ═══════════════════════════════════════════════════════════════════
# Sub-agent guard
# ═══════════════════════════════════════════════════════════════════
subagent_file = _claude_dir / ".subagent"
if tool_name == "Skill" and subagent_file.exists():
    subagent = subagent_file.read_text(encoding="utf-8", errors="replace").strip()
    print(f"  ⚠️  已在子 Agent '{subagent}' 中", file=sys.stderr)
    sys.exit(1)

sys.exit(0)
