#!/usr/bin/env bash
set -euo pipefail

script_dir="$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)"
if command -v python3 >/dev/null 2>&1; then
    exec python3 "$script_dir/verify-runtime-baseline.py" "$@"
fi
if command -v python >/dev/null 2>&1; then
    exec python "$script_dir/verify-runtime-baseline.py" "$@"
fi

echo "Python 3 is required to run verify-roadmap-0.sh" >&2
exit 127
