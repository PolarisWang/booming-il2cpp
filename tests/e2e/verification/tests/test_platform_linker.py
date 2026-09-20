"""Unit tests for verification/platform/ — linker capabilities and the registry.

The property under test is small but load-bearing: the linker must both
*tolerate* duplicate symbols and keep the **first** definition, because that is
what lets source order decide whether the patch or the sentinel wins.  A
platform whose linker cannot do that must report ``duplicate_symbol_flag=None``
so the stage skips (or picks another strategy) rather than producing a build
with no patches that reports success.
"""

from __future__ import annotations

import sys
from pathlib import Path

_HERE = Path(__file__).resolve()
_VERIFY = _HERE.parents[1]
for _p in (_VERIFY, _VERIFY.parent):
    if str(_p) not in sys.path:
        sys.path.insert(0, str(_p))

from verification.platforms.linker import LinkerProfile, linker_for_platform  # noqa: E402
from verification.platforms.registry import PLATFORM_REGISTRY, spec_for_current_platform  # noqa: E402
from verification.platforms.patch_link import (  # noqa: E402
    DistinctLinkStrategy,
    PatchLinkUnsupported,
    select_strategy,
    verify_patch_objects_linked,
)


class TestLinkerProfile:
    def test_msvc_profile_carries_force_multiple(self):
        p = LinkerProfile("msvc", "/FORCE:MULTIPLE")
        assert p.supports_distinct_linking
        assert p.duplicate_symbol_flag == "/FORCE:MULTIPLE"

    def test_none_flag_means_distinct_linking_unavailable(self):
        assert not LinkerProfile("apple-ld", None).supports_distinct_linking

    def test_linux_uses_allow_multiple_definition(self):
        p = linker_for_platform("linux")
        assert p.name == "gnu-ld"
        # The flag is the GNU ld spelling of the same capability MSVC exposes.
        assert p.duplicate_symbol_flag == "-Wl,--allow-multiple-definition"
        assert p.supports_distinct_linking

    def test_darwin_reports_no_duplicate_tolerance(self):
        # Apple's ld has no equivalent; claiming a flag would produce a link error
        # at best and a sentinel-winning build at worst.
        p = linker_for_platform("darwin")
        assert p.name == "apple-ld"
        assert not p.supports_distinct_linking

    def test_unknown_platform_has_no_flag(self):
        assert not linker_for_platform("plan9").supports_distinct_linking


class TestRegistry:
    def test_every_entry_key_matches_its_spec_name(self):
        for key, spec in PLATFORM_REGISTRY.items():
            assert spec.name == key, f"registry key {key!r} disagrees with spec name {spec.name!r}"

    def test_linux_prefers_distinct_link_then_weak_symbol(self):
        spec = PLATFORM_REGISTRY["linux"]
        assert spec.patch_link_strategies[0] == "distinct_link"
        assert "weak_symbol" in spec.patch_link_strategies

    def test_platform_without_duplicate_support_does_not_offer_distinct_link(self):
        """A platform must not advertise a strategy its linker cannot run."""
        spec = PLATFORM_REGISTRY["darwin"]
        assert "distinct_link" not in spec.patch_link_strategies

    def test_unknown_platform_yields_empty_strategy_list(self):
        spec = spec_for_current_platform("plan9")
        assert spec.patch_link_strategies == ()
        assert spec.name.startswith("unknown:")

    def test_prefix_lookup_handles_legacy_platform_strings(self):
        # Old Pythons reported "linux2"/"linux3"; the registry is keyed on the
        # stable stem so those keep working.
        assert spec_for_current_platform("linux2").name == "linux"


class TestStrategySelection:
    def test_selects_distinct_link_on_linux(self):
        spec = PLATFORM_REGISTRY["linux"]
        s = select_strategy(spec.patch_link_strategies, spec.layout, spec.linker)
        assert isinstance(s, DistinctLinkStrategy)

    def test_raises_when_nothing_is_available(self):
        spec = PLATFORM_REGISTRY["darwin"]  # only offers weak_symbol, not implemented
        try:
            select_strategy(spec.patch_link_strategies, spec.layout, spec.linker)
        except PatchLinkUnsupported:
            return
        raise AssertionError("expected PatchLinkUnsupported for a platform offering no usable strategy")

    def test_strategy_flags_track_the_linker(self):
        s = DistinctLinkStrategy()
        assert s.link_flags(LinkerProfile("msvc", "/FORCE:MULTIPLE")) == ["/FORCE:MULTIPLE"]
        assert s.link_flags(LinkerProfile("apple-ld", None)) == []


class TestPatchObjectVerification:
    def test_detects_object_present(self, tmp_path: Path):
        from verification.platforms.layout import NinjaLayout

        layout = NinjaLayout()
        obj_dir = layout.object_dir(tmp_path, "RelWithDebInfo")
        obj_dir.mkdir(parents=True)
        (obj_dir / "patch-host-arrays.cpp.o").write_text("")

        assert verify_patch_objects_linked(layout, tmp_path, "RelWithDebInfo", "patch-host-arra")

    def test_reports_absent_object(self, tmp_path: Path):
        """The 'CMakeLists surgery silently did nothing' case must not read as success."""
        from verification.platforms.layout import NinjaLayout

        assert not verify_patch_objects_linked(NinjaLayout(), tmp_path, "RelWithDebInfo", "patch-host-arra")
