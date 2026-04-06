from __future__ import annotations

import re
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
SCANNED_PATHS = [
    REPO_ROOT / "contracts" / "docs",
    REPO_ROOT / "contracts" / "native",
    REPO_ROOT / "build" / "scripts",
    REPO_ROOT / "build" / "toolchains" / "run",
    REPO_ROOT / "run.cmd",
    REPO_ROOT / "run.ps1",
    REPO_ROOT / "run.sh",
    REPO_ROOT / "src" / "native",
    REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen",
    REPO_ROOT / "tests" / "contract",
    REPO_ROOT / "tests" / "contracts" / "native",
    REPO_ROOT / "tests" / "integration" / "run",
    REPO_ROOT / "tests" / "tooling" / "run",
    REPO_ROOT / "tests" / "platform",
    REPO_ROOT / "tests" / "gate",
    REPO_ROOT / "tests" / "proof" / "native-reference",
    REPO_ROOT / "tests" / "smoke" / "input",
    REPO_ROOT / "tests" / "unit" / "run",
    REPO_ROOT / "docs" / "architecture" / "roadmap-0",
    REPO_ROOT / "docs" / "dev" / "in-progress" / "20260405-01-il2cpp-reboot-after-abandoned-roadmap",
    REPO_ROOT / "docs" / "dev" / "in-progress" / "20260406-02-stage-4-native-bootstrap-and-reference-proof",
]
TEXT_SUFFIXES = {
    ".cmd",
    ".cpp",
    ".cs",
    ".csproj",
    ".cxx",
    ".h",
    ".hpp",
    ".json",
    ".md",
    ".mm",
    ".ps1",
    ".py",
    ".scriban",
    ".sh",
    ".txt",
}
FORBIDDEN_PATTERN = re.compile(r"\bBOOM_|\bboom_|\bboom::|\bBoom\.")


class NativePrefixNamingTests(unittest.TestCase):
    def test_live_native_and_runner_surfaces_use_chaos_prefixes(self) -> None:
        violations: list[str] = []

        for scanned_path in SCANNED_PATHS:
            self.assertTrue(scanned_path.exists(), msg=f"missing scan root: {scanned_path}")

            if scanned_path.is_file():
                candidate_paths = [scanned_path]
            else:
                candidate_paths = scanned_path.rglob("*")

            for path in candidate_paths:
                if not path.is_file() or path.suffix.lower() not in TEXT_SUFFIXES:
                    continue
                if path == Path(__file__):
                    continue

                text = path.read_text(encoding="utf-8")
                for match in FORBIDDEN_PATTERN.finditer(text):
                    line_no = text.count("\n", 0, match.start()) + 1
                    violations.append(f"{path.relative_to(REPO_ROOT)}:{line_no}:{match.group(0)}")

        self.assertEqual([], violations)


if __name__ == "__main__":
    unittest.main()

