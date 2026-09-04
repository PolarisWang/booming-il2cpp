#!/bin/env bash
# ===============================================================================
# release-config.sh — single source of truth for release pipeline constants.
#
# Sourced by scripts/release.sh and the generate-*.sh helpers so that no hardcoded
# version / branch / timeout / php-version appears twice. Editing these constants
# updates every consumer.
#
# Not intended to be executed directly.
# ===============================================================================

# ── SemVer / versioning ───────────────────────────────────────────────────────
# Path to the version file (single source of truth, consumed by release_bump.sh).
export RC_VERSION_FILE="${RC_VERSION_FILE:-$REPO_ROOT/VERSION}"

# Version regex — must reject anything that is not MAJOR.MINOR.PATCH[-suffix].
export RC_SEMVER_RE="^[0-9]+\.[0-9]+\.[0-9]+(-[0-9A-Za-z.-]+)?$"

# ── Release branch / tag naming ───────────────────────────────────────────────
# Tag prefix used for releases, e.g. v0.1.1. Derived from version.
export RC_TAG_PREFIX="v"
# Release branch prefix + suffix, e.g. release/0.1.x.
export RC_RELEASE_BRANCH_PREFIX="release/"
export RC_RELEASE_BRANCH_SUFFIX=".x"

# ── Timeouts (seconds) ─────────────────────────────────────────────────────────
export RC_DOTNET_BUILD_TIMEOUT="${RC_DOTNET_BUILD_TIMEOUT:-120000}"   # ms; csproj build
export RC_TPG_TIMEOUT="${RC_TPG_TIMEOUT:-1800000}"                    # ms; TPG generate-dll
export RC_CI_TIMEOUT="${RC_CI_TIMEOUT:-120}"                          # s; hygiene gate

# ── SDK preset names ──────────────────────────────────────────────────────────
# Map host OS+arch → CMake preset dir. Mirrors ConvertService.ResolveNativePreset.
# Used by release.sh SDK build + generate-sbom / publish manifest.
export RC_PRESET_WIN_X64="windows-x64-reference"
export RC_PRESET_LINUX_X64="linux-x64-profile"
export RC_PRESET_LINUX_X64_BUILD="linux-x64-native"
export RC_PRESET_LINUX_ARM64="linux-arm64-profile"
export RC_PRESET_OSX_X64="osx-x64-profile"
export RC_PRESET_OSX_ARM64="osx-arm64-profile"

# ── Release directory layout ──────────────────────────────────────────────────
# Where release artifacts are staged under artifacts/.
export RC_ARTIFACTS_BASE="$REPO_ROOT/artifacts"
export RC_RELEASE_DIR="$RC_ARTIFACTS_BASE/release"

# ── SDK library archive sizes / checks ────────────────────────────────────────
# Empty archives are exactly 8 bytes: `!<arch>\n`. Any lib <= this after the
# real-lib copy is a leftover stub and means the link will fail.
export RC_STUB_MAX_BYTES="${RC_STUB_MAX_BYTES:-8}"

# ── SBOM ──────────────────────────────────────────────────────────────────────
# CycloneDX spec version emitted by generate-sbom.sh.
export RC_SBOM_SPEC="1.5"
export RC_SBOM_FILENAME="sbom.cyclonedx.json"

# ── Checksums ─────────────────────────────────────────────────────────────────
export RC_CHECKSUM_FILENAME="SHA256SUMS"

# ── Feishu notification (optional; set FEISHU_WEBHOOK in env to enable) ──────
# Custom bot webhook URL: https://open.feishu.cn/open-apis/bot/v2/hook/<token>
# Optional signing secret (FEISHU_SECRET) if the bot has signature verification.
export RC_FEISHU_SCRIPT="$REPO_ROOT/scripts/notify-feishu.sh"