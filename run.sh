#!/usr/bin/env bash
set -euo pipefail

script_dir="$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)"
repo_root="${BOOM_RUN_REPO_ROOT:-$script_dir}"
manifest_path="${BOOM_RUN_RUNTIME_MANIFEST:-$repo_root/build/toolchains/run/runtime_manifest.json}"
runtime_script="$repo_root/build/toolchains/run/runtime.py"
run_script="$repo_root/build/toolchains/run/run.py"

bootstrap_python="${BOOM_RUN_BOOTSTRAP_PYTHON:-}"
if [[ -z "$bootstrap_python" ]]; then
  if command -v python3 >/dev/null 2>&1; then
    bootstrap_python="$(command -v python3)"
  elif command -v python >/dev/null 2>&1; then
    bootstrap_python="$(command -v python)"
  else
    bootstrap_python=""
  fi
fi

detect_host_platform_id() {
  local system arch
  system="$(uname -s | tr '[:upper:]' '[:lower:]')"
  arch="$(uname -m | tr '[:upper:]' '[:lower:]')"

  case "$system" in
    mingw*|msys*|cygwin*|windows_nt*)
      case "$arch" in
        x86_64|amd64) echo "windows-x64" ;;
        arm64|aarch64) echo "windows-arm64" ;;
      esac
      ;;
    darwin)
      case "$arch" in
        x86_64|amd64) echo "macos-x64" ;;
        arm64|aarch64) echo "macos-arm64" ;;
      esac
      ;;
    linux)
      case "$arch" in
        x86_64|amd64) echo "linux-x64" ;;
        arm64|aarch64) echo "linux-arm64" ;;
      esac
      ;;
  esac
}

host_platform_family() {
  case "$1" in
    windows*) echo "windows" ;;
    macos*) echo "macos" ;;
    linux*) echo "linux" ;;
    *) echo "${1:-unknown}" ;;
  esac
}

json_escape() {
  printf '%s' "$1" | sed -e 's/\\/\\\\/g' -e 's/"/\\"/g' -e ':a;N;$!ba;s/\r/\\r/g;s/\n/\\n/g'
}

normalize_response() {
  printf '%s' "$1" | tr '[:upper:]' '[:lower:]'
}

has_json_flag() {
  local arg
  for arg in "$@"; do
    if [[ "$arg" == "--json" ]]; then
      return 0
    fi
  done

  return 1
}

render_command_text() {
  local filtered=()
  local arg

  for arg in "$@"; do
    if [[ "$arg" != "--json" ]]; then
      filtered+=("$arg")
    fi
  done

  if [[ ${#filtered[@]} -eq 0 ]]; then
    echo "help"
    return 0
  fi

  local IFS=' '
  echo "${filtered[*]}"
}

emit_json_error() {
  local command_text="$1"
  local host_platform="$2"
  local message="$3"

  printf '{\n'
  printf '  "command": "%s",\n' "$(json_escape "$command_text")"
  printf '  "status": "error",\n'
  printf '  "hostPlatform": "%s",\n' "$(json_escape "$host_platform")"
  printf '  "target": null,\n'
  printf '  "durationMs": 0,\n'
  printf '  "checks": [],\n'
  printf '  "errors": ["%s"]\n' "$(json_escape "$message")"
  printf '}\n'
}

probe_runtime() {
  if [[ -z "$bootstrap_python" ]]; then
    return 1
  fi

  "$bootstrap_python" "$runtime_script" probe --repo-root "$repo_root" --manifest "$manifest_path"
}

runtime_python_from_probe_json() {
  local probe_json="$1"

  if [[ -z "$probe_json" || -z "$bootstrap_python" ]]; then
    return 1
  fi

  PROBE_JSON="$probe_json" "$bootstrap_python" - <<'PY'
import json
import os
print(json.loads(os.environ["PROBE_JSON"])["pythonPath"])
PY
}

json_requested=0
if has_json_flag "$@"; then
  json_requested=1
fi

command_text="$(render_command_text "$@")"
host_platform_id="$(detect_host_platform_id)"
host_platform="$(host_platform_family "$host_platform_id")"

if [[ "${1-}" == "bootstrap" ]]; then
  if [[ " $* " != *" --yes "* ]]; then
    force_interactive="${BOOM_RUN_FORCE_INTERACTIVE:-}"
    if [[ "$force_interactive" == "0" || ! -t 0 ]]; then
      if [[ "$json_requested" -eq 1 ]]; then
        emit_json_error "$command_text" "$host_platform" "non-interactive bootstrap requires 'run bootstrap --yes'"
      else
        echo "non-interactive bootstrap requires 'run bootstrap --yes'" >&2
      fi
      exit 1
    fi

    echo "Python runtime is not installed. Bootstrap now? [y/N]"
    response="${BOOM_RUN_CONFIRM_RESPONSE:-}"
    if [[ -z "$response" ]]; then
      read -r response
    fi

    case "$(normalize_response "$response")" in
      y|yes|1|true) ;;
      *)
        if [[ "$json_requested" -eq 1 ]]; then
          emit_json_error "$command_text" "$host_platform" "bootstrap cancelled"
        else
          echo "bootstrap cancelled" >&2
        fi
        exit 1
        ;;
    esac
  fi

  probe_json="$(probe_runtime || true)"
  runtime_python="$(runtime_python_from_probe_json "$probe_json" || true)"
  if [[ -n "$runtime_python" && -f "$runtime_python" ]]; then

    if [[ "$json_requested" -eq 1 ]]; then
      exec "$runtime_python" "$runtime_script" bootstrap --repo-root "$repo_root" --manifest "$manifest_path" --json
    fi

    echo "Python runtime already installed."
    exit 0
  fi

  if [[ -z "$bootstrap_python" ]]; then
    if [[ "$json_requested" -eq 1 ]]; then
      emit_json_error "$command_text" "$host_platform" "system Python was not found. Install Python temporarily or pre-seed the cached runtime before bootstrapping."
    else
      echo "system Python was not found. Install Python temporarily or pre-seed the cached runtime before bootstrapping." >&2
    fi
    exit 1
  fi

  if [[ "$json_requested" -eq 1 ]]; then
    exec "$bootstrap_python" "$runtime_script" bootstrap --repo-root "$repo_root" --manifest "$manifest_path" --json
  fi

  "$bootstrap_python" "$runtime_script" bootstrap --repo-root "$repo_root" --manifest "$manifest_path"
  exit $?
fi

probe_json="$(probe_runtime || true)"
runtime_python="$(runtime_python_from_probe_json "$probe_json" || true)"
if [[ -z "$runtime_python" || ! -f "$runtime_python" ]]; then
  force_interactive="${BOOM_RUN_FORCE_INTERACTIVE:-}"
  if [[ "$force_interactive" == "0" || ! -t 0 ]]; then
    if [[ "$json_requested" -eq 1 ]]; then
      emit_json_error "$command_text" "$host_platform" "Python runtime is not installed. Run 'run bootstrap --yes' first."
    else
      echo "Python runtime is not installed. Run 'run bootstrap --yes' first." >&2
    fi
    exit 1
  fi

  echo "Python runtime is not installed. Bootstrap now? [y/N]"
  response="${BOOM_RUN_CONFIRM_RESPONSE:-}"
  if [[ -z "$response" ]]; then
    read -r response
  fi

  case "$(normalize_response "$response")" in
    y|yes|1|true)
      if [[ -z "$bootstrap_python" ]]; then
        if [[ "$json_requested" -eq 1 ]]; then
          emit_json_error "$command_text" "$host_platform" "system Python was not found. Install Python temporarily or pre-seed the cached runtime before bootstrapping."
        else
          echo "system Python was not found. Install Python temporarily or pre-seed the cached runtime before bootstrapping." >&2
        fi
        exit 1
      fi
      "$bootstrap_python" "$runtime_script" bootstrap --repo-root "$repo_root" --manifest "$manifest_path"
      ;;
    *)
      if [[ "$json_requested" -eq 1 ]]; then
        emit_json_error "$command_text" "$host_platform" "bootstrap cancelled"
      else
        echo "bootstrap cancelled" >&2
      fi
      exit 1
      ;;
  esac

  probe_json="$(probe_runtime)"
  runtime_python="$(runtime_python_from_probe_json "$probe_json")"
fi

if [[ ! -f "$run_script" ]]; then
  echo "runtime bootstrap is ready, but the command layer is not implemented yet." >&2
  exit 1
fi

exec "$runtime_python" "$run_script" "$@"
