"""Unit tests for the pre-push review runner's verdict parser.

TARGET: .githooks/review_runner.py::parse_verdict

Verifies the fail-closed contract:
  - a well-formed block/allow verdict is parsed as-is
  - trailing noise / multi-line output resolves to the LAST JSON line
  - non-JSON / empty output FAILS CLOSED to verdict=block (never let a severe
    issue slip through on a parse error)
"""
from __future__ import annotations

import importlib.util
import sys
import unittest
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]
RUNNER_PATH = REPO_ROOT / ".githooks" / "review_runner.py"

_loader = importlib.util.spec_from_file_location("review_runner", str(RUNNER_PATH))
_module = importlib.util.module_from_spec(_loader)
_loader.loader.exec_module(_module)


class ParseVerdictTests(unittest.TestCase):

    def test_block_verdict_parsed(self) -> None:
        raw = '{"verdict":"block","severity":"L1","issue_count":2,"issues":[{"file":"a.c","msg":"x"}]}'
        v = _module.parse_verdict(raw)
        self.assertEqual(v["verdict"], "block")
        self.assertEqual(v["severity"], "L1")
        self.assertEqual(v["issue_count"], 2)

    def test_allow_verdict_parsed(self) -> None:
        raw = '{"verdict":"allow","severity":"L4","issue_count":0,"issues":[]}'
        v = _module.parse_verdict(raw)
        self.assertEqual(v["verdict"], "allow")
        self.assertEqual(v["severity"], "L4")

    def test_last_json_line_wins_with_noise(self) -> None:
        # model may emit a preamble line then the JSON verdict line
        raw = 'noise here\n{"verdict":"block","severity":"L2","issue_count":1,"issues":[]}  '
        v = _module.parse_verdict(raw)
        self.assertEqual(v["verdict"], "block")
        self.assertEqual(v["severity"], "L2")

    def test_non_json_fails_closed(self) -> None:
        v = _module.parse_verdict("the model did not return JSON at all")
        self.assertEqual(v["verdict"], "block")
        self.assertEqual(v["severity"], "L1")

    def test_empty_fails_closed(self) -> None:
        v = _module.parse_verdict("")
        self.assertEqual(v["verdict"], "block")
        self.assertEqual(v["severity"], "L1")

    def test_blank_lines_only_fails_closed(self) -> None:
        v = _module.parse_verdict("\n\n  \n")
        self.assertEqual(v["verdict"], "block")


if __name__ == "__main__":
    unittest.main()
