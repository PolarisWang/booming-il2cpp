from __future__ import annotations

import hashlib
import re
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Iterable

try:
    from ..suite_manifest import SuiteManifest
except ImportError:
    sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
    from suite_manifest import SuiteManifest


_NON_ALNUM_RE = re.compile(r"[^a-z0-9]+")


@dataclass(frozen=True)
class DiscoveredCase:
    case_id: str
    source_id: str
    source_name: str


def normalize_case_id(value: str) -> str:
    normalized = _NON_ALNUM_RE.sub("-", value.strip().lower()).strip("-")
    return normalized or "main"


def _collision_suffix(source_id: str) -> str:
    return hashlib.sha1(source_id.encode("utf-8")).hexdigest()[:4]


def finalize_discovered_cases(raw_cases: Iterable[tuple[str, str]]) -> list[DiscoveredCase]:
    counts: dict[str, int] = {}
    discovered: list[DiscoveredCase] = []

    for source_id, source_name in raw_cases:
        base_case_id = normalize_case_id(source_name)
        occurrence = counts.get(base_case_id, 0)
        counts[base_case_id] = occurrence + 1
        case_id = base_case_id if occurrence == 0 else f"{base_case_id}-{_collision_suffix(source_id)}"
        discovered.append(DiscoveredCase(case_id=case_id, source_id=source_id, source_name=source_name))

    return discovered


class BaseAdapter:
    runner_type = ""

    def __init__(self, suite: SuiteManifest) -> None:
        self.suite = suite

    def discover_cases(self) -> list[DiscoveredCase]:
        return finalize_discovered_cases([("main", "main")])

    def build(self) -> None:
        raise NotImplementedError

    def run(self) -> None:
        raise NotImplementedError

    def report(self) -> None:
        raise NotImplementedError
