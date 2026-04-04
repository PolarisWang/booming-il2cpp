#!/usr/bin/env bash
set -euo pipefail

script_dir="$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)"
exec pwsh "$script_dir/compare-warmup-trace.ps1" "$@"
