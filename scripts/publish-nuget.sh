#!/usr/bin/env bash
# publish-nuget.sh — push the chaos-il2cpp nupkg to NuGet.org.
#
# Usage:
#   ./scripts/publish-nuget.sh [version]          # push to NuGet.org
#   ./scripts/publish-nuget.sh [version] --dry-run # show what would happen
#
# Prerequisites:
#   - NUGET_API_KEY environment variable set, OR
#   - dotnet nuget setapikey <key> --source https://api.nuget.org/v3/index.json
#
# The nupkg must already exist at artifacts/release/tool/chaos-il2cpp.<version>.nupkg
# (built by scripts/build-tool-package.sh).
#
# Shared constants from release-config.sh (RC_*).

set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$REPO_ROOT"
source "$REPO_ROOT/scripts/release-config.sh"

DRY_RUN=0
VERSION="${1:-}"
for a in "$@"; do
    case "$a" in --dry-run) DRY_RUN=1;; esac
done

if [ -z "$VERSION" ]; then
    VERSION=$(cat "$RC_VERSION_FILE" 2>/dev/null || echo "0.0.0")
fi

NUPKG="$RC_ARTIFACTS_BASE/release/tool/chaos-il2cpp.${VERSION}.nupkg"
SNUPKG="$RC_ARTIFACTS_BASE/release/tool/chaos-il2cpp.${VERSION}.snupkg"
NUGET_SOURCE="https://api.nuget.org/v3/index.json"

echo "=== publish-nuget: version=$VERSION ==="

# ── 1. Verify nupkg exists ────────────────────────────────────────────────
if [ ! -f "$NUPKG" ]; then
    echo "  ERROR: nupkg not found at $NUPKG" >&2
    echo "  Run scripts/build-tool-package.sh $VERSION first." >&2
    exit 1
fi
echo "  nupkg: $NUPKG ($(du -h "$NUPKG" | cut -f1))"

# ── 2. Resolve API key ────────────────────────────────────────────────────
API_KEY="${NUGET_API_KEY:-}"
if [ -z "$API_KEY" ]; then
    # Try reading from the NuGet config store
    API_KEY=$(dotnet nuget getapikey "$NUGET_SOURCE" 2>/dev/null || echo "")
fi
if [ -z "$API_KEY" ]; then
    echo "  ERROR: no NuGet API key found. Set NUGET_API_KEY env var or run:" >&2
    echo "    dotnet nuget setapikey <your-key> --source $NUGET_SOURCE" >&2
    exit 1
fi
echo "  API key: ${API_KEY:0:4}...${API_KEY: -4} (len=${#API_KEY})"

# ── 3. Push ────────────────────────────────────────────────────────────────
echo "  Pushing to NuGet.org..."
if [ "$DRY_RUN" -eq 1 ]; then
    echo "  [dry-run] would push: $NUPKG"
    echo "  [dry-run] would push: $SNUPKG"
else
    dotnet nuget push "$NUPKG" \
        --api-key "$API_KEY" \
        --source "$NUGET_SOURCE" \
        --symbol-api-key "$API_KEY" \
        --symbol-source "$NUGET_SOURCE" \
        --timeout 300 2>&1 | tail -5
    echo "  Push complete"
fi

echo ""
echo "== NuGet publish done."
echo "   Install:  dotnet tool install --global chaos-il2cpp"
echo "   Version:  $VERSION"