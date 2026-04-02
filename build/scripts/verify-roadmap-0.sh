#!/usr/bin/env bash
set -euo pipefail

script_dir="$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)"
exec pwsh "$script_dir/verify-roadmap-0.ps1" "$@"
