from __future__ import annotations

from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[5]
SDK_SOURCE_PATH = REPO_ROOT / "src" / "reference" / "Chaos.TestFramework.Sdk" / "Chaos.TestFramework.Sdk.cs"


def test_test_framework_sdk_contains_foundation_dll_subject_annotations() -> None:
    source = SDK_SOURCE_PATH.read_text(encoding="utf-8")

    assert "MethodSubjectIdAttribute" in source
    assert "BenchmarkSubjectIdAttribute" in source
    assert "HotUpdateSubjectIdAttribute" in source
    assert "CapabilityFamilyIdAttribute" in source
    assert "VerificationRouteAttribute" in source
    assert "BenchmarkRouteAttribute" in source
    assert "BenchmarkProfileAttribute" in source
    assert "HotUpdateDirectionAttribute" in source
