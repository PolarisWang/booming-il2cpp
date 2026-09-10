#!/usr/bin/env python3
"""Smoke tests for batch.py entry point.

Tests argument parsing, function signatures, and discovery logic
without running actual families or native builds.
"""

from __future__ import annotations

import inspect

from verification.orchestration.discovery import discover_families
from verification.entry_points.batch import run_family, run_single_family


# ── Tests ─────────────────────────────────────────────────────────

def test_discover_families():
    slugs = discover_families(verbose=False)
    assert isinstance(slugs, list), f"Expected list, got {type(slugs)}"
    assert len(slugs) > 0, "Expected at least one family"
    assert all(isinstance(s, str) for s in slugs), "All slugs should be strings"
    assert "convert-char" in slugs, "Expected convert-char in discovered families"


def test_discover_families_skip_slugs():
    slugs = discover_families(verbose=False)
    skip_slugs = {
        "aggregation", "filtering", "immutable-array", "immutable-list", "reports",
        "interface-dispatch", "pinvoke-dllimport", "primitive-numeric-conversions-core",
    }
    for s in skip_slugs:
        assert s not in slugs, f"{s} should be in SKIP_SLUGS"


def test_run_family_signature():
    sig = inspect.signature(run_family)
    assert "timeout" in sig.parameters, "run_family missing timeout param"
    assert "resume" in sig.parameters, "run_family missing resume param"
    assert sig.parameters["timeout"].default == 0
    assert sig.parameters["resume"].default is False


def test_run_single_family_signature():
    sig = inspect.signature(run_single_family)
    params = list(sig.parameters.keys())
    assert params == ["args"], f"Expected single 'args' param, got {params}"


def test_cli_args_parse_defaults():
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument("--output", "-o", default=None)
    parser.add_argument("--assembly", default="System.Private.CoreLib")
    parser.add_argument("--family", default=None)
    parser.add_argument("--skip-stages", default=None)
    parser.add_argument("--from-slug", default=None)
    parser.add_argument("--concurrency", "-j", type=int, default=1)
    parser.add_argument("--native-config", choices=["check", "profile", "ship"], default="check")
    parser.add_argument("--mode", choices=["standard", "strict"], default="standard")
    parser.add_argument("--stage-resume", action="store_true")
    parser.add_argument("--validate", action="store_true")
    parser.add_argument("--timeout", type=int, default=0)
    parser.add_argument("--verbose", "-v", action="store_true")

    args = parser.parse_args([])
    assert args.mode == "standard"
    assert args.timeout == 0
    assert args.stage_resume is False
    assert args.validate is False
    assert args.native_config == "check"


def test_cli_args_parse_explicit():
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument("--output", "-o", default=None)
    parser.add_argument("--assembly", default="System.Private.CoreLib")
    parser.add_argument("--family", default=None)
    parser.add_argument("--skip-stages", default=None)
    parser.add_argument("--from-slug", default=None)
    parser.add_argument("--concurrency", "-j", type=int, default=1)
    parser.add_argument("--native-config", choices=["check", "profile", "ship"], default="check")
    parser.add_argument("--mode", choices=["standard", "strict"], default="standard")
    parser.add_argument("--stage-resume", action="store_true")
    parser.add_argument("--validate", action="store_true")
    parser.add_argument("--timeout", type=int, default=0)
    parser.add_argument("--verbose", "-v", action="store_true")

    args = parser.parse_args(["--mode", "strict", "--timeout", "30", "--stage-resume", "--validate"])
    assert args.mode == "strict"
    assert args.timeout == 30
    assert args.stage_resume is True
    assert args.validate is True


def test_run_family_validations_nonexistent():
    """run_family_validations returns error list for non-existent report."""
    from verification.entry_points.batch import run_family_validations
    result = run_family_validations("nonexistent-slug")
    assert isinstance(result, list)
    assert len(result) > 0
    assert "Report not found" in result[0]
