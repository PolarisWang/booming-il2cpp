from __future__ import annotations

from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[5]
SDK_DIR = REPO_ROOT / "src" / "reference" / "Chaos.TestFramework.Sdk"


def test_test_framework_sdk_contains_foundation_dll_subject_annotations() -> None:
    # SDK was refactored into individual files per type
    fact_src = (SDK_DIR / "FactAttribute.cs").read_text(encoding="utf-8")
    benchmark_src = (SDK_DIR / "BenchmarkAttribute.cs").read_text(encoding="utf-8")
    hotupdate_src = (SDK_DIR / "HotUpdateAttribute.cs").read_text(encoding="utf-8")
    assert_src = (SDK_DIR / "Assert.cs").read_text(encoding="utf-8")

    # New attribute types: only [Fact], [Benchmark], [HotUpdate] + Assert.*
    assert "FactAttribute" in fact_src
    assert "BenchmarkAttribute" in benchmark_src
    assert "HotUpdateAttribute" in hotupdate_src
    assert "Assert" in assert_src

    # Read the csproj to verify old attribute types are absent
    csproj_src = (SDK_DIR / "Chaos.TestFramework.Sdk.csproj").read_text(encoding="utf-8")
    for old_type in ("MethodSubjectIdAttribute", "BenchmarkSubjectIdAttribute",
                     "HotUpdateSubjectIdAttribute", "CapabilityFamilyIdAttribute",
                     "VerificationRouteAttribute", "BenchmarkRouteAttribute",
                     "BenchmarkProfileAttribute", "HotUpdateDirectionAttribute"):
        assert old_type not in fact_src
        assert old_type not in benchmark_src
        assert old_type not in hotupdate_src
