"""Run Phase 1 auto-generate directly, bypassing CLI manifest routing."""
import sys
from pathlib import Path

REPO_ROOT = Path("D:/agent/booming-il2cpp")
sys.path.insert(0, str(REPO_ROOT / "build" / "toolchains" / "run"))

from testing.foundation_dll import gap_analyzer

result = gap_analyzer.analyze_gaps(
    REPO_ROOT,
    scope="all",
    update_ledger=False,
    auto_generate=True,
)

print(f"DLL count: {result['dllCount']}")
print(f"Family count: {result['familyCount']}")
print(f"Status counts: {result['statusCounts']}")
generated = result.get("generatedArtifacts", [])
print(f"Generated artifacts: {len(generated)}")
for a in generated:
    print(f"  {a}")
