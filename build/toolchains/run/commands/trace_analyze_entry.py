"""Entry point for trace-analyze command.

Usage:
  python -m build.toolchains.run.commands.trace_analyze_entry [--session N] [--stage X] [--json]

Equivalent to:
  run trace-analyze [args...]
"""

from __future__ import annotations

import sys
from pathlib import Path

_HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(_HERE.parent))

from commands.trace_analyze import handle, _find_trace_sessions

if __name__ == "__main__":
    repo_root = Path(__file__).resolve().parents[4]
    opts = {}

    args = sys.argv[1:]
    i = 0
    while i < len(args):
        if args[i] == "--session" and i + 1 < len(args):
            opts["session"] = int(args[i + 1])
            i += 2
        elif args[i] == "--stage" and i + 1 < len(args):
            opts["stage"] = args[i + 1]
            i += 2
        elif args[i] == "--op" and i + 1 < len(args):
            opts["op"] = args[i + 1]
            i += 2
        elif args[i] == "--lang" and i + 1 < len(args):
            opts["lang"] = args[i + 1]
            i += 2
        elif args[i] == "--json":
            opts["json"] = True
            i += 1
        elif args[i] == "--list":
            sessions = _find_trace_sessions(repo_root)
            from datetime import datetime
            for idx, s in enumerate(sessions):
                dt = datetime.fromtimestamp(s["mtime"]).strftime("%Y-%m-%d %H:%M:%S")
                print(f"  [{idx}] {s['session_id']}  ({dt})")
            sys.exit(0)
        else:
            i += 1

    result = handle(repo_root, "windows", "trace-analyze", opts)
    print(result.text)
    sys.exit(0 if result.status == "ok" else 1)
