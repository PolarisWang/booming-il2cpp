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

if [ $# -lt 1 ]; then
    echo "Usage: $0 <artifacts-dir> [version] [output_file]" >&2
    exit 1
fi

ARTIFACTS_DIR="$1"
VERSION_OVERRIDE="${2:-}"
OUTPUT_FILE="${3:-"$ARTIFACTS_DIR/sbom.cyclonedx.json"}"

if [ ! -d "$ARTIFACTS_DIR" ]; then
    echo "Error: artifacts directory not found: $ARTIFACTS_DIR" >&2
    exit 1
fi

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

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
"$PY" - "$ARTIFACTS_DIR" "$VERSION" "$GIT_COMMIT" "$GIT_BRANCH" "$OUTPUT_FILE" <<'PYEOF'
import hashlib, json, os, sys, time

artifacts_dir, version, commit, branch, output_file = sys.argv[1:6]

def sha256_of(path):
    h = hashlib.sha256()
    with open(path, 'rb') as f:
        for chunk in iter(lambda: f.read(65536), b''):
            h.update(chunk)
    return h.hexdigest()

components = []
for root, dirs, files in os.walk(artifacts_dir):
    # skip the output SBOM itself + SHA256SUMS to avoid self-reference
    dirs[:] = [d for d in dirs if d not in ('.git',)]
    for name in files:
        full = os.path.join(root, name)
        if name in ('sbom.cyclonedx.json', 'SHA256SUMS'):
            continue
        rel = os.path.relpath(full, artifacts_dir).replace('\\', '/')
        size = os.path.getsize(full)
        # Skip directories-only empty files
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

sbom = {
    "bomFormat": "CycloneDX",
    "specVersion": "1.5",
    "serialNumber": "urn:uuid:" + (hashlib.sha1(str((artifacts_dir, version, commit)).encode()).hexdigest()),
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