from __future__ import annotations

try:
    from .base import BaseAdapter, finalize_discovered_cases
except ImportError:
    from base import BaseAdapter, finalize_discovered_cases


class NativeCmakeAdapter(BaseAdapter):
    runner_type = "native-cmake"

    def discover_cases(self):
        return finalize_discovered_cases([("main", "main")])
