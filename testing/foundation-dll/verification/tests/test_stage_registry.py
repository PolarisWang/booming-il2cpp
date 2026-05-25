"""Tests for stages/__init__.py — stage registry and lookup."""

from verification.stages import lookup_stage, STAGE_REGISTRY, get_stage_names, get_default_stages


class TestStageRegistry:
    def test_has_expected_stages(self):
        names = get_stage_names()
        for expected in ("preflight", "codegen", "jit_codegen", "fact",
                         "audit", "asm_compare", "benchmark", "hotupdate",
                         "hotupdate_aot_benchmark", "hotupdate_jit_fact"):
            assert expected in names, f"Missing stage: {expected}"

    def test_default_stages_not_empty(self):
        assert len(get_default_stages()) > 0

    def test_all_registry_entries_have_valid_functions(self):
        for name, func_name, _ in STAGE_REGISTRY:
            fn = lookup_stage(name)
            assert fn is not None, f"lookup_stage({name!r}) returned None, expected {func_name}"

    def test_lookup_unknown_stage(self):
        assert lookup_stage("nonexistent") is None

    def test_lookup_hotupdate_jit_fact_uses_hotupdate_module(self):
        """R4 regression: run_hotupdate_jit_fact must resolve from 'hotupdate' module, not 'run_hotupdate_jit'."""
        fn = lookup_stage("hotupdate_jit_fact")
        assert fn is not None
        assert fn.__name__ == "run_hotupdate_jit_fact"
        assert "hotupdate" in fn.__module__, f"Expected hotupdate module, got {fn.__module__}"

    def test_lookup_preflight(self):
        fn = lookup_stage("preflight")
        assert fn is not None
        assert fn.__name__ == "run_preflight"
