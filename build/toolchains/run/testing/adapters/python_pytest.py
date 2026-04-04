from __future__ import annotations

try:
    from .base import BaseAdapter, finalize_discovered_cases
except ImportError:
    from base import BaseAdapter, finalize_discovered_cases


class PythonPytestAdapter(BaseAdapter):
    runner_type = "python-pytest"

    def discover_cases(self):
        case_sources = self.suite.runner_config.get("caseSources", [])
        if not case_sources:
            return finalize_discovered_cases([("main", "main")])
        return finalize_discovered_cases(
            [
                (str(item.get("sourceId", item.get("sourceName", "main"))), str(item.get("sourceName", item.get("sourceId", "main"))))
                for item in case_sources
            ]
        )
