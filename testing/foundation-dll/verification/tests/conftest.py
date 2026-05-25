"""Shared fixtures for verification pipeline unit tests."""
from __future__ import annotations

import sys
from pathlib import Path

import pytest

# Ensure verification/ package is on sys.path for unqualified imports
_VERIFICATION = Path(__file__).resolve().parent.parent
if str(_VERIFICATION) not in sys.path:
    sys.path.insert(0, str(_VERIFICATION))

from verification.orchestration.context import FamilyContext, StageResult


@pytest.fixture
def mock_ctx() -> FamilyContext:
    return FamilyContext(
        slug="test-family",
        assembly="Test.Assembly",
        family_dir=Path("/tmp/test-family"),
        mode="standard",
    )


@pytest.fixture
def mock_ctx_strict() -> FamilyContext:
    return FamilyContext(
        slug="test-family-strict",
        assembly="Test.Assembly",
        family_dir=Path("/tmp/test-family-strict"),
        mode="strict",
    )


@pytest.fixture
def passed_stage() -> StageResult:
    return StageResult(stage="fact", status="passed", summary="18/18 passed")


@pytest.fixture
def failed_stage() -> StageResult:
    return StageResult(stage="codegen", status="failed", summary="Build error")


@pytest.fixture
def skipped_stage() -> StageResult:
    return StageResult(stage="benchmark", status="skipped", summary="Skipped by config")


@pytest.fixture
def sample_stages(passed_stage, failed_stage, skipped_stage) -> dict[str, StageResult]:
    return {
        "preflight": passed_stage,
        "codegen": failed_stage,
        "fact": passed_stage,
        "audit": passed_stage,
        "benchmark": skipped_stage,
    }
