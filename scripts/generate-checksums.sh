#!/usr/bin/env bash
# generate-checksums.sh — generate SHA256 checksums for all files in a release directory.
#
# Usage:
#   ./scripts/generate-checksums.sh <directory> [output_file]
#
# If output_file is omitted, writes to <directory>/SHA256SUMS.
#
# Output format: standard SHA256SUMS (hash  filename)
# One file per line, sorted by filename for reproducibility.
#
# Shared constants from release-config.sh: RC_CHECKSUM_FILENAME

set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
# shellcheck source=release-config.sh
# shellcheck disable=SC1091
source "$REPO_ROOT/scripts/release-config.sh" 2>/dev/null || true

CHECKSUM_NAME="${RC_CHECKSUM_FILENAME:-SHA256SUMS}"

if [ $# -lt 1 ]; then
    echo "Usage: $0 <directory> [output_file]" >&2
    exit 1
fi

TARGET_DIR="$1"
OUTPUT_FILE="${2:-"$TARGET_DIR/$CHECKSUM_NAME"}"

if [ ! -d "$TARGET_DIR" ]; then
    echo "Error: directory not found: $TARGET_DIR" >&2
    exit 1
fi

echo "== generate-checksums: $TARGET_DIR → $OUTPUT_FILE"

# Use python for portable SHA256 computation (avoid Windows path issues with sha256sum).
python -c "
import hashlib, os, sys

target = os.path.abspath(sys.argv[1])
output = os.path.abspath(sys.argv[2]) if len(sys.argv) > 2 else os.path.join(target, 'SHA256SUMS')

entries = []
for root, dirs, files in os.walk(target):
    for name in sorted(files):
        if name == 'SHA256SUMS':
            continue
        full = os.path.join(root, name)
        rel = os.path.relpath(full, target).replace('\\\\', '/')
        h = hashlib.sha256()
        with open(full, 'rb') as f:
            for chunk in iter(lambda: f.read(65536), b''):
                h.update(chunk)
        entries.append((h.hexdigest(), rel))

with open(output, 'w', encoding='utf-8') as f:
    for digest, rel in entries:
        f.write(f'{digest}  {rel}\n')
print(f'  wrote {len(entries)} checksums to {output}')
" "$TARGET_DIR" "$OUTPUT_FILE"