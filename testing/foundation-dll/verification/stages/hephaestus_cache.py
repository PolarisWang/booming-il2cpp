"""Hephaestus Lib — Layered build cache for foundation-dll verification.

Hephaestus caches the output of IL2CPP codegen + native build at the chunk level.
When a chunk's input DLLs haven't changed, the cached entry.exe is reused,
saving ~500s of codegen time per chunk.

Cache key: SHA-256 hash of (assembly DLL + all dependency DLLs + metadata).
Cache entries are stored under <foundation-dll>/.hephaestus-cache/ with a
manifest.json tracking all entries.

Usage:
    from verification.stages.hephaestus_cache import HephaestusCache
    cache = HephaestusCache(foundation_dir)

    # Before build
    cache_key = cache.compute_key(subjects_dll, metadata_path, chunk_slug)
    if cache_key:
        entry = cache.lookup(cache_key)
        if entry:
            # Cache hit — restore build output
            entry.restore_to(native_dir)
            return

    # After build
    cache.store(cache_key, native_dir, duration_ms=...)
"""

from __future__ import annotations

import hashlib
import json
import os
import shutil
import subprocess
import sys
import time
from dataclasses import dataclass, field, asdict
from pathlib import Path
from typing import Any


# ── Constants ────────────────────────────────────────────────────────────────

CACHE_DIR_NAME = ".hephaestus-cache"
MANIFEST_NAME = "manifest.json"
DEFAULT_MAX_ENTRIES = 50  # LRU eviction threshold
CACHE_VERSION = 1


# ── Data types ───────────────────────────────────────────────────────────────

@dataclass
class CacheEntry:
    """A single cache entry in the Hephaestus manifest.

    Fields:
        cache_key: Unique key derived from input hash (e.g. "System.Collections/global-ns/abc123def456").
        assembly: Assembly name (e.g. "System.Collections").
        chunk_slug: Chunk slug (e.g. "global-ns").
        input_hash: SHA-256 hex digest of all input DLLs + metadata.
        cached_at: ISO-8601 timestamp when this entry was created.
        duration_ms: Build duration in milliseconds (for analytics).
        commit_hash: Git commit hash at time of caching.
        entry_exe_size: Size of entry.exe in bytes.
        entry_count: Number of files stored in this cache entry.
        status: "valid", "stale", or "invalid".
    """
    cache_key: str
    assembly: str
    chunk_slug: str
    input_hash: str
    cached_at: str = ""
    duration_ms: int = 0
    commit_hash: str = ""
    entry_exe_size: int = 0
    entry_count: int = 0
    status: str = "valid"

    def to_dict(self) -> dict[str, Any]:
        return asdict(self)

    @staticmethod
    def from_dict(d: dict[str, Any]) -> CacheEntry:
        return CacheEntry(**d)


@dataclass
class HephaestusManifest:
    """The manifest file for the Hephaestus cache.

    Fields:
        version: Schema version for forward compatibility.
        max_entries: Maximum number of cache entries before LRU eviction.
        entries: List of CacheEntry objects, newest first.
    """
    version: int = CACHE_VERSION
    max_entries: int = DEFAULT_MAX_ENTRIES
    entries: list[CacheEntry] = field(default_factory=list)

    def to_dict(self) -> dict[str, Any]:
        return asdict(self)

    @staticmethod
    def from_dict(d: dict[str, Any]) -> HephaestusManifest:
        entries = [CacheEntry.from_dict(e) for e in d.get("entries", [])]
        return HephaestusManifest(
            version=d.get("version", CACHE_VERSION),
            max_entries=d.get("max_entries", DEFAULT_MAX_ENTRIES),
            entries=entries,
        )


# ── Hash computation ─────────────────────────────────────────────────────────

def _sha256_file(path: Path) -> str:
    """Compute SHA-256 of a file's contents."""
    h = hashlib.sha256()
    with open(path, "rb") as f:
        while True:
            chunk = f.read(65536)
            if not chunk:
                break
            h.update(chunk)
    return h.hexdigest()


def _get_git_commit_hash(project_root: Path | None = None) -> str:
    """Get current git commit hash (short form)."""
    try:
        cmd = ["git", "rev-parse", "--short", "HEAD"]
        if project_root:
            cmd.append(f"--git-dir={project_root / '.git'}")
        result = subprocess.run(
            cmd, capture_output=True, text=True, timeout=10
        )
        if result.returncode == 0:
            return result.stdout.strip()
    except (subprocess.TimeoutExpired, OSError):
        pass
    return "unknown"


def compute_input_hash(
    subjects_dll: Path,
    metadata_path: Path,
    assembly: str,
    additional_dlls: list[Path] | None = None,
) -> str:
    """Compute the input hash for cache key derivation.

    Hashes:
    1. The subjects DLL (CombinedSubjects.dll)
    2. The metadata file (subjects.metadata.json)
    3. Any additional dependency DLLs (CoreLib, System.Runtime, etc.)

    Returns a hexadecimal SHA-256 digest.
    """
    h = hashlib.sha256()

    # Update with assembly name for namespace isolation
    h.update(assembly.encode("utf-8"))

    # Subjects DLL
    if subjects_dll.exists():
        h.update(_sha256_file(subjects_dll).encode("utf-8"))

    # Metadata
    if metadata_path.exists():
        h.update(_sha256_file(metadata_path).encode("utf-8"))

    # Additional DLLs (runtime dependencies)
    for dll in (additional_dlls or []):
        if dll.exists():
            h.update(dll.name.encode("utf-8"))
            h.update(_sha256_file(dll).encode("utf-8"))

    return h.hexdigest()


# ── Cache manager ────────────────────────────────────────────────────────────

class HephaestusCache:
    """Manages the Hephaestus Lib build cache.

    The cache lives at <foundation_dir>/.hephaestus-cache/ and contains:
      manifest.json        — metadata for all cache entries
      <cache_key>/         — per-entry directory with build artifacts

    Each cache entry stores the entire native/ build directory (entry.exe,
    subjects/, codegen/, etc.).
    """

    def __init__(
        self,
        foundation_dir: Path,
        max_entries: int = DEFAULT_MAX_ENTRIES,
        verbose: bool = False,
    ):
        self._foundation_dir = Path(foundation_dir).resolve()
        self._cache_dir = self._foundation_dir / CACHE_DIR_NAME
        self._manifest_path = self._cache_dir / MANIFEST_NAME
        self._max_entries = max_entries
        self._verbose = verbose
        self._manifest = self._load_manifest()

    # ── Public API ──

    def lookup(self, cache_key: str) -> CacheEntry | None:
        """Look up a cache entry by key.

        Returns the CacheEntry if found and valid, None otherwise.
        Does NOT verify that the cached files still exist on disk.
        """
        entry = self._manifest.entries_by_key.get(cache_key)
        if entry is None:
            return None
        if entry.status != "valid":
            return None
        return entry

    def is_cache_hit(self, cache_key: str) -> bool:
        """Check if a valid cache entry exists for the given key."""
        entry = self.lookup(cache_key)
        if entry is None:
            return False
        # Verify the cached directory still exists
        entry_dir = self._entry_dir(cache_key)
        if not entry_dir.is_dir():
            if self._verbose:
                print(f"  [hephaestus] Cache entry {cache_key[:24]}... directory missing, marking stale")
            self._mark_stale(cache_key)
            return False
        return True

    def restore_to(self, cache_key: str, target_dir: Path) -> bool:
        """Restore a cached build to the target directory.

        Copies all files from the cache entry to target_dir.
        target_dir is expected to be the chunk's native/ directory.
        Returns True on success.
        """
        entry = self.lookup(cache_key)
        if entry is None:
            return False

        src = self._entry_dir(cache_key)
        if not src.is_dir():
            self._mark_stale(cache_key)
            return False

        if self._verbose:
            print(f"  [hephaestus] Restoring cached build: {cache_key[:24]}... -> {target_dir}")

        # Ensure target exists
        target_dir.mkdir(parents=True, exist_ok=True)

        # Copy cached files to target
        for item in src.iterdir():
            dest = target_dir / item.name
            if item.is_dir():
                if dest.is_dir():
                    shutil.rmtree(dest)
                shutil.copytree(item, dest, symlinks=True)
            else:
                shutil.copy2(item, dest)

        # Touch the entry to update LRU order
        self._touch_entry(cache_key)
        return True

    def store(
        self,
        cache_key: str,
        source_dir: Path,
        assembly: str,
        chunk_slug: str,
        input_hash: str,
        duration_ms: int = 0,
    ) -> CacheEntry:
        """Store a build output in the cache.

        Copies the contents of source_dir into the cache, creates a manifest
        entry, and prunes old entries if the cache exceeds max_entries.
        """
        if self._verbose:
            print(f"  [hephaestus] Caching build: {cache_key[:24]}... ({source_dir})")

        # Remove stale entry if exists
        self._remove_entry(cache_key)

        # Copy build artifacts to cache
        dest = self._entry_dir(cache_key)
        dest.mkdir(parents=True, exist_ok=True)
        for item in source_dir.iterdir():
            d = dest / item.name
            if item.is_dir():
                if d.is_dir():
                    shutil.rmtree(d)
                shutil.copytree(item, d, symlinks=True)
            else:
                shutil.copy2(item, d)

        # Compute metadata
        entry_exe = source_dir / "entry.exe"
        exe_size = entry_exe.stat().st_size if entry_exe.exists() else 0
        entry_count = sum(1 for _ in dest.rglob("*"))

        entry = CacheEntry(
            cache_key=cache_key,
            assembly=assembly,
            chunk_slug=chunk_slug,
            input_hash=input_hash,
            cached_at=time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime()),
            duration_ms=duration_ms,
            commit_hash=_get_git_commit_hash(self._foundation_dir),
            entry_exe_size=exe_size,
            entry_count=entry_count,
            status="valid",
        )

        # Add to manifest
        self._manifest.entries.insert(0, entry)
        self._prune_old_entries()
        self._save_manifest()

        if self._verbose:
            print(f"  [hephaestus] Cached: {exe_size} bytes, {entry_count} files")
        return entry

    def compute_key(self, input_hash: str, assembly: str, chunk_slug: str) -> str:
        """Derive a cache key from assembly, chunk slug, input hash, and platform.

        Format: "{assembly}/{chunk_slug}/{platform}/{input_hash[:16]}"
        The platform prefix prevents cross-platform cache contamination
        (e.g. Windows-generated verification_dispatch.generated.cpp uses
        __try/__except which can't compile on Linux with GCC).
        """
        short_hash = input_hash[:16]
        platform = sys.platform  # e.g. 'linux', 'win32', 'darwin'
        return f"{assembly}/{chunk_slug}/{platform}/{short_hash}"

    def invalidate_assembly(self, assembly: str) -> int:
        """Invalidate all cache entries for a given assembly.

        Returns the number of entries invalidated.
        Useful when intentionally rebuilding an assembly's chunks.
        """
        count = 0
        for entry in self._manifest.entries:
            if entry.assembly == assembly and entry.status == "valid":
                entry.status = "stale"
                count += 1
        if count > 0:
            self._save_manifest()
            if self._verbose:
                print(f"  [hephaestus] Invalidated {count} entries for {assembly}")
        return count

    def get_stats(self) -> dict[str, Any]:
        """Return cache statistics for reporting."""
        valid = [e for e in self._manifest.entries if e.status == "valid"]
        total_size = 0
        for e in valid:
            d = self._entry_dir(e.cache_key)
            if d.is_dir():
                total_size += sum(
                    f.stat().st_size for f in d.rglob("*") if f.is_file()
                )
        return {
            "cache_dir": str(self._cache_dir),
            "total_entries": len(self._manifest.entries),
            "valid_entries": len(valid),
            "stale_entries": len(self._manifest.entries) - len(valid),
            "total_size_bytes": total_size,
            "max_entries": self._max_entries,
        }

    # ── Internal helpers ──

    def _entry_dir(self, cache_key: str) -> Path:
        """Path to the storage directory for a cache entry."""
        # Sanitize cache key for filesystem use
        safe = cache_key.replace("/", "_")
        return self._cache_dir / safe

    def _load_manifest(self) -> HephaestusManifest:
        """Load the manifest from disk, or create empty."""
        if self._manifest_path.exists():
            try:
                data = json.loads(self._manifest_path.read_text(encoding="utf-8"))
                manifest = HephaestusManifest.from_dict(data)
                # Build lookup index
                manifest.entries_by_key = {
                    e.cache_key: e for e in manifest.entries
                }
                return manifest
            except (json.JSONDecodeError, OSError):
                if self._verbose:
                    print(f"  [hephaestus] WARNING: Corrupt manifest, starting fresh")
        manifest = HephaestusManifest(max_entries=self._max_entries)
        manifest.entries_by_key = {}
        return manifest

    def _save_manifest(self) -> None:
        """Write manifest to disk."""
        self._cache_dir.mkdir(parents=True, exist_ok=True)
        self._manifest.entries_by_key = {
            e.cache_key: e for e in self._manifest.entries
        }
        # Write atomically: write to temp, then rename
        tmp = self._manifest_path.with_suffix(".tmp.json")
        tmp.write_text(
            json.dumps(self._manifest.to_dict(), indent=2, ensure_ascii=False),
            encoding="utf-8",
        )
        tmp.replace(self._manifest_path)

    def _prune_old_entries(self) -> None:
        """Remove oldest entries if we exceed max_entries."""
        valid = [e for e in self._manifest.entries if e.status == "valid"]
        if len(valid) <= self._max_entries:
            return

        # Sort by cached_at (oldest first) and remove excess
        valid.sort(key=lambda e: e.cached_at)
        to_remove = valid[: len(valid) - self._max_entries]
        for entry in to_remove:
            self._remove_entry(entry.cache_key)

    def _remove_entry(self, cache_key: str) -> None:
        """Remove a cache entry from disk and manifest."""
        # Remove from disk
        entry_dir = self._entry_dir(cache_key)
        if entry_dir.is_dir():
            shutil.rmtree(entry_dir)

        # Remove from manifest
        self._manifest.entries = [
            e for e in self._manifest.entries if e.cache_key != cache_key
        ]
        self._manifest.entries_by_key.pop(cache_key, None)

    def _mark_stale(self, cache_key: str) -> None:
        """Mark a cache entry as stale (directory missing)."""
        entry = self._manifest.entries_by_key.get(cache_key)
        if entry and entry.status == "valid":
            entry.status = "stale"
            self._save_manifest()

    def _touch_entry(self, cache_key: str) -> None:
        """Update LRU order by moving entry to front."""
        entry = self._manifest.entries_by_key.get(cache_key)
        if entry is None:
            return
        # Move to front (most recently used)
        self._manifest.entries = [
            e for e in self._manifest.entries if e.cache_key != cache_key
        ]
        self._manifest.entries.insert(0, entry)


# ── CLI entry point ──────────────────────────────────────────────────────────

def _print_stats(cache: HephaestusCache) -> None:
    """Print cache statistics."""
    stats = cache.get_stats()
    print(f"Hephaestus Cache Statistics:")
    print(f"  Cache dir:    {stats['cache_dir']}")
    print(f"  Valid entries: {stats['valid_entries']}")
    print(f"  Stale entries: {stats['stale_entries']}")
    print(f"  Total size:   {stats['total_size_bytes'] / (1024*1024):.1f} MB")
    print(f"  Max entries:  {stats['max_entries']}")


def cli_main() -> int:
    """CLI entry point for cache management commands."""
    import argparse

    parser = argparse.ArgumentParser(
        description="Hephaestus Lib — build cache management",
    )
    parser.add_argument(
        "--foundation-dir",
        default=None,
        help="Foundation DLL directory (default: auto-detect from CWD)",
    )
    parser.add_argument(
        "--verbose", "-v", action="store_true", help="Verbose output"
    )

    subparsers = parser.add_subparsers(dest="command", help="Command")

    # ── stats ──
    subparsers.add_parser("stats", help="Show cache statistics")

    # ── clear ──
    subparsers.add_parser("clear", help="Clear all cache entries")

    # ── invalidate ──
    inv = subparsers.add_parser(
        "invalidate", help="Invalidate entries for an assembly"
    )
    inv.add_argument("assembly", help="Assembly name to invalidate")

    # ── prune ──
    prn = subparsers.add_parser("prune", help="Prune stale entries")
    prn.add_argument(
        "--max-entries", type=int, default=DEFAULT_MAX_ENTRIES,
        help="Maximum entries to keep",
    )

    args = parser.parse_args()

    # Detect foundation directory
    if args.foundation_dir:
        foundation_dir = Path(args.foundation_dir)
    else:
        # Walk up from CWD to find testing/foundation-dll/
        cwd = Path.cwd().resolve()
        for parent in [cwd] + list(cwd.parents):
            if (parent / "verification").is_dir() and (parent / "config").is_dir():
                foundation_dir = parent
                break
        else:
            print("ERROR: Could not detect foundation-dll directory. Use --foundation-dir")
            return 1

    cache = HephaestusCache(
        foundation_dir, verbose=args.verbose
    )

    if args.command == "stats" or args.command is None:
        _print_stats(cache)
        return 0

    elif args.command == "clear":
        # Remove entire cache directory
        cache_dir = cache._cache_dir
        if cache_dir.is_dir():
            shutil.rmtree(cache_dir)
            print(f"Cleared cache: {cache_dir}")
        else:
            print("Cache is already empty")
        return 0

    elif args.command == "invalidate":
        count = cache.invalidate_assembly(args.assembly)
        print(f"Invalidated {count} entries for {args.assembly}")
        return 0

    elif args.command == "prune":
        cache._max_entries = args.max_entries
        cache._prune_old_entries()
        cache._save_manifest()
        print(f"Pruned cache to max {args.max_entries} entries")
        _print_stats(cache)
        return 0

    else:
        parser.print_help()
        return 1


if __name__ == "__main__":
    sys.exit(cli_main())
