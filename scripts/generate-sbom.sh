#!/usr/bin/env bash
# generate-sbom.sh — generate a CycloneDX-format SBOM for a release artifact set.
#
# Records the project version (from VERSION), every file in the target directory
# with its SHA256 hash + size + content type, and the git commit it was built from,
# into a CycloneDX 1.5 JSON document. This satisfies the minimal supply-chain
# provenance requirement for product-grade releases (no external tool dependency).
#
# Usage:
#   ./scripts/generate-sbom.sh <artifacts-dir> [version] [output_file]
#
#   <artifacts-dir> : directory containing the release artifacts (recursively scanned)
#   [version]       : override the VERSION-file value (else read from VERSION)
#   [output_file]   : default <artifacts-dir>/sbom.cyclonedx.json
#
# Output: CycloneDX 1.5 JSON (SPDX license expression on metadata for the project).

set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
# shellcheck source=release-config.sh
# shellcheck disable=SC1091
source "$REPO_ROOT/scripts/release-config.sh" 2>/dev/null || true

SBOM_NAME="${RC_SBOM_FILENAME:-sbom.cyclonedx.json}"
SBOM_SPEC="${RC_SBOM_SPEC:-1.5}"

if [ $# -lt 1 ]; then
    echo "Usage: $0 <artifacts-dir> [version] [output_file]" >&2
    exit 1
fi

ARTIFACTS_DIR="$1"
VERSION_OVERRIDE="${2:-}"
OUTPUT_FILE="${3:-"$ARTIFACTS_DIR/$SBOM_NAME"}"

if [ ! -d "$ARTIFACTS_DIR" ]; then
    echo "Error: artifacts directory not found: $ARTIFACTS_DIR" >&2
    exit 1
fi

# Resolve version: override > VERSION file > git describe
VERSION="$VERSION_OVERRIDE"
if [ -z "$VERSION" ] && [ -f "$REPO_ROOT/VERSION" ]; then
    VERSION=$(tr -d '[:space:]' < "$REPO_ROOT/VERSION")
fi
if [ -z "$VERSION" ]; then
    VERSION=$(git -C "$REPO_ROOT" describe --tags --always 2>/dev/null || echo "unknown")
fi

# Resolve git commit + timestamp at build source
GIT_COMMIT=$(git -C "$REPO_ROOT" rev-parse --short HEAD 2>/dev/null || echo "unknown")
GIT_BRANCH=$(git -C "$REPO_ROOT" rev-parse --abbrev-ref HEAD 2>/dev/null || echo "unknown")

echo "== generate-sbom: $ARTIFACTS_DIR (version=$VERSION, commit=$GIT_COMMIT)"

# Resolve a working python interpreter (python3 may be a broken Windows Store alias).
PY=""
for cand in python python3 py; do
    if command -v "$cand" >/dev/null 2>&1 && "$cand" -c 'import sys' >/dev/null 2>&1; then PY="$cand"; break; fi
done
if [ -z "$PY" ]; then echo "Error: no working python interpreter found" >&2; exit 1; fi

# Build the file inventory via python (avoids fragile JSON escaping in bash).
"$PY" - "$ARTIFACTS_DIR" "$VERSION" "$GIT_COMMIT" "$GIT_BRANCH" "$OUTPUT_FILE" "$SBOM_NAME" <<'PYEOF'
import hashlib, json, os, sys, time, uuid

artifacts_dir, version, commit, branch, output_file, sbom_name = (sys.argv[1:6] + [""])[:6]
# sbom_name passed separately (avoid self-reference on the exact filename)
import hashlib

def sha256_of(path):
    h = hashlib.sha256()
    with open(path, 'rb') as f:
        for chunk in iter(lambda: f.read(65536), b''):
            h.update(chunk)
    return h.hexdigest()

components = []
for root, dirs, files in os.walk(artifacts_dir):
    dirs[:] = [d for d in dirs if d not in ('.git',)]
    for name in files:
        full = os.path.join(root, name)
        if name in ('sbom.cyclonedx.json', 'SHA256SUMS', sbom_name):
            continue
        rel = os.path.relpath(full, artifacts_dir).replace('\\', '/')
        size = os.path.getsize(full)
        if size == 0:
            continue
        components.append({
            "type": "library",
            "name": name,
            "version": version,
            "bom-ref": rel,
            "properties": [{"name": "path", "value": rel}, {"name": "size", "value": str(size)}],
            "hashes": [{
                "alg": "SHA-256",
                "content": sha256_of(full),
            }],
        })

timestamp = time.strftime('%Y-%m-%dT%H:%M:%SZ', time.gmtime())

# Build a deterministic UUID v5 (SHA-1 namespace) from the artifact dir + version.
# CycloneDX requires serialNumber to be urn:uuid:<UUID>; use UUID v5 for
# reproducibility (same inputs → same serialNumber across re-runs).
ns = uuid.NAMESPACE_DNS
serial_uuid = uuid.uuid5(ns, f"chaos-il2cpp-release-{artifacts_dir}-{version}")

sbom = {
    "bomFormat": "CycloneDX",
    "specVersion": "1.5",
    "serialNumber": "urn:uuid:" + str(serial_uuid),
    "version": 1,
    "metadata": {
        "timestamp": timestamp,
        "component": {
            "type": "application",
            "name": "chaos-il2cpp",
            "version": version,
            "licenses": [{"license": {"id": "MIT"}}] if os.path.exists(os.path.join(os.path.dirname(os.path.dirname(artifacts_dir)), "LICENSE")) else [],
        },
        "properties": [
            {"name": "git.commit", "value": commit},
            {"name": "git.branch", "value": branch},
            {"name": "toolchain", "value": "chaos-il2cpp release pipeline"},
        ],
    },
    "components": components,
}

with open(output_file, 'w', encoding='utf-8') as f:
    json.dump(sbom, f, indent=2)
print(f"  wrote {len(components)} components to {output_file}")
PYEOF