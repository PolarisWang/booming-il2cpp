from __future__ import annotations

import sys
from dataclasses import dataclass
from pathlib import Path

try:
    from ..core.common import write_json
    from .suite_manifest import SuiteManifest, load_suite_manifest
except ImportError:
    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    from core.common import write_json
    from testing.suite_manifest import SuiteManifest, load_suite_manifest


FAMILY_ROOTS = ["unit", "integration", "contract", "smoke", "gate", "workflow", "perf"]


@dataclass(frozen=True)
class InvalidSuite:
    family: str
    suite_id: str
    manifest_path: Path
    error: str

    @property
    def suite_key(self) -> str:
        return f"{self.family}/{self.suite_id}"

    def to_dict(self) -> dict[str, str]:
        return {
            "id": self.suite_key,
            "family": self.family,
            "suiteId": self.suite_id,
            "manifestPath": str(self.manifest_path),
            "error": self.error,
        }


@dataclass(frozen=True)
class CatalogIndex:
    host_platform: str
    valid_suites: list[SuiteManifest]
    invalid_suites: list[InvalidSuite]
    cache_path: Path

    @property
    def valid_suite_ids(self) -> list[str]:
        return sorted(suite.suite_key for suite in self.valid_suites)

    @property
    def invalid_suite_ids(self) -> list[str]:
        return sorted(suite.suite_key for suite in self.invalid_suites)

    @property
    def family_names(self) -> list[str]:
        families = {suite.family for suite in self.valid_suites}
        families.update(suite.family for suite in self.invalid_suites)
        return sorted(families)

    def to_dict(self) -> dict[str, object]:
        return {
            "hostPlatform": self.host_platform,
            "validSuites": [suite.to_dict() for suite in self.valid_suites],
            "invalidSuites": [suite.to_dict() for suite in self.invalid_suites],
        }


def _is_host_compatible(suite: SuiteManifest, host_platform: str) -> bool:
    return suite.supported_hosts is None or host_platform in suite.supported_hosts


def scan_catalog(repo_root: Path, host_platform: str, strict: bool = False) -> CatalogIndex:
    valid_suites: list[SuiteManifest] = []
    invalid_suites: list[InvalidSuite] = []

    tests_root = repo_root / "tests"
    for family in FAMILY_ROOTS:
        family_root = tests_root / family
        if not family_root.is_dir():
            continue

        for suite_dir in sorted(path for path in family_root.iterdir() if path.is_dir()):
            manifest_path = suite_dir / "suite.manifest.json"
            if not manifest_path.is_file():
                continue

            try:
                suite = load_suite_manifest(manifest_path)
            except ValueError as error:
                invalid_suites.append(
                    InvalidSuite(
                        family=family,
                        suite_id=suite_dir.name,
                        manifest_path=manifest_path,
                        error=str(error),
                    )
                )
                continue

            if _is_host_compatible(suite, host_platform):
                valid_suites.append(suite)

    if strict and invalid_suites:
        raise ValueError("catalog scan found invalid suites")

    cache_path = repo_root / "artifacts" / "tests" / "catalog" / host_platform / "index.json"
    index = CatalogIndex(
        host_platform=host_platform,
        valid_suites=valid_suites,
        invalid_suites=invalid_suites,
        cache_path=cache_path,
    )
    write_json(cache_path, index.to_dict())
    return index
