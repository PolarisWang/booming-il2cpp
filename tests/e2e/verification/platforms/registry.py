"""Platform registry — the single place a new platform is declared.

Adding a platform should mean adding one entry here and (only if the platform
needs a fundamentally different linking approach) one strategy in
``patch_link.py``.  Nothing in the stages should branch on ``sys.platform``.
"""

from __future__ import annotations

import sys
from dataclasses import dataclass, field

from .layout import BuildTreeLayout, MsBuildLayout, NinjaLayout
from .linker import LinkerProfile, linker_for_platform


@dataclass(frozen=True)
class PlatformSpec:
    """Everything the stages need to know about one platform."""

    name: str
    layout: BuildTreeLayout
    linker: LinkerProfile
    # Ordered strategy preference for making patch symbols win.  The stage takes
    # the first strategy whose `is_available()` holds.  Ordering matters: a
    # strategy that genuinely guarantees link order beats one that only avoids
    # duplicates.
    patch_link_strategies: tuple[str, ...] = field(default_factory=tuple)


def _spec(name: str, layout: BuildTreeLayout, linker: LinkerProfile, strategies: tuple[str, ...]) -> PlatformSpec:
    return PlatformSpec(name=name, layout=layout, linker=linker, patch_link_strategies=strategies)


# ── Registry ────────────────────────────────────────────────────────────────
# Keys are `sys.platform` prefixes so the lookup below can match "linux2"-style
# legacy values without another branch.
PLATFORM_REGISTRY: dict[str, PlatformSpec] = {
    "win32": _spec(
        "win32",
        MsBuildLayout(),
        LinkerProfile("msvc", "/FORCE:MULTIPLE"),
        ("distinct_link",),
    ),
    "linux": _spec(
        "linux",
        NinjaLayout(),
        LinkerProfile("gnu-ld", "-Wl,--allow-multiple-definition"),
        # distinct_link is preferred (it is what the nightly has always done on
        # Windows, so the semantics are already proven); weak_symbol is the
        # fallback for a linker that cannot tolerate duplicates.
        ("distinct_link", "weak_symbol"),
    ),
    "darwin": _spec(
        "darwin",
        NinjaLayout(),
        LinkerProfile("apple-ld", None),
        # No duplicate-symbol tolerance on Apple's linker, so distinct linking is
        # not offered at all.
        ("weak_symbol",),
    ),
}


def spec_for_current_platform(platform: str | None = None) -> PlatformSpec:
    """Look up the spec for ``platform`` (defaults to the running platform).

    Falls back to a conservative spec rather than raising: a platform we have
    not described yet should produce a clear "no strategy available" message
    from the caller, not an import-time crash.
    """
    plat = sys.platform if platform is None else platform

    if plat in PLATFORM_REGISTRY:
        return PLATFORM_REGISTRY[plat]

    # Prefix match handles legacy values like "linux2"/"linux3" and keeps the
    # registry keyed on stable names.
    for key, spec in PLATFORM_REGISTRY.items():
        if plat.startswith(key):
            return spec

    return PlatformSpec(
        name=f"unknown:{plat}",
        layout=NinjaLayout(),
        linker=linker_for_platform(plat),
        patch_link_strategies=(),
    )
