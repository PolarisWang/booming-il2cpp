"""Patch-link strategies — how the patch symbols win over the sentinel.

Background
----------
`patch-host-arrays.cpp` (real host arrays) and `runtime-patchdata.cpp` (a
sentinel claiming zero patches) both define `kPatchDataHost*`.  Exactly one may
win, and it must be the patch file, or hotupdate silently runs with no patches
and reports a pass it did not earn.

The historical approach was: reorder the generated CMakeLists so the patch file
is listed first, build, then **parse the build log** to infer which file was
compiled first and fail if the order came out wrong.  The log parsing was the
fragile part — it only worked for the MSBuild generator's output format, so the
whole stage was skipped on any other platform (and with it, all Linux hotupdate
coverage).

The insight that removes it: **CMake already preserves the order of sources in
`add_executable`**, on every generator we use (measured on Ninja and Unix
Makefiles; the Visual Studio generator's ordering is additionally checked by the
existing Windows path).  Since the linker keeps the first definition, listing
the patch file first *is* the guarantee — there is nothing left to discover by
reading logs.

What replaces the log check
---------------------------
A check that the patch actually took effect at **runtime** (see
``verify_patch_active``).  That is strictly stronger than inferring compile
order: it observes the outcome the order was supposed to produce, and it is
generator-independent.
"""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
from typing import Protocol

from .layout import BuildTreeLayout
from .linker import LinkerProfile


class PatchLinkUnsupported(Exception):
    """No available strategy can guarantee the patch symbols win.

    Distinct from a build failure: the stage treats this as a legitimate skip
    only when *no* strategy exists for the platform (e.g. an unknown toolchain).
    If a strategy was selected and then failed, that is a failure.
    """


@dataclass(frozen=True)
class BuildOutcome:
    ok: bool
    message: str


class PatchLinkStrategy(Protocol):
    name: str

    def is_available(self, layout: BuildTreeLayout, linker: LinkerProfile) -> bool: ...

    def cmake_reorder(self, cmake_text: str, patch_stem: str, sentinel_stem: str) -> str | None:
        """Return patched CMakeLists text, or None if this strategy needs none."""
        ...

    def link_flags(self, linker: LinkerProfile) -> list[str]: ...


class DistinctLinkStrategy:
    """Two files define the same symbols; order decides, the linker tollerates it.

    Requires a linker that keeps the first definition (MSVC, GNU ld, lld).
    """

    name = "distinct_link"

    def is_available(self, layout: BuildTreeLayout, linker: LinkerProfile) -> bool:
        return linker.supports_distinct_linking

    def cmake_reorder(self, cmake_text: str, patch_stem: str, sentinel_stem: str) -> str | None:
        # The caller owns the actual text surgery (it needs the generated file's
        # exact shape); this strategy only declares that reordering is the lever.
        return None

    def link_flags(self, linker: LinkerProfile) -> list[str]:
        return [linker.duplicate_symbol_flag] if linker.duplicate_symbol_flag else []


class WeakSymbolStrategy:
    """Mark the sentinel definitions weak so the patch's strong ones win.

    Available wherever the compiler honours ``__attribute__((weak))`` (GCC and
    Clang, so Linux, Android, and — with a caveat — Darwin).  It needs no linker
    flag and no source ordering, which makes it the natural fallback when the
    linker cannot tolerate duplicates.

    Not implemented yet: it requires template changes (the sentinel is emitted
    from `TestProject.RuntimePatchdata.cpp.scriban`) and the result is
    equivalent to distinct linking, so it is only worth building when a platform
    actually needs it.
    """

    name = "weak_symbol"

    def is_available(self, layout: BuildTreeLayout, linker: LinkerProfile) -> bool:
        return False  # see docstring — interface only for now

    def cmake_reorder(self, cmake_text: str, patch_stem: str, sentinel_stem: str) -> str | None:
        return None

    def link_flags(self, linker: LinkerProfile) -> list[str]:
        return []


_STRATEGIES: dict[str, PatchLinkStrategy] = {
    DistinctLinkStrategy.name: DistinctLinkStrategy(),
    WeakSymbolStrategy.name: WeakSymbolStrategy(),
}


def select_strategy(
    preference: tuple[str, ...], layout: BuildTreeLayout, linker: LinkerProfile
) -> PatchLinkStrategy:
    """First preferred strategy that is available for this toolchain."""
    for name in preference:
        strategy = _STRATEGIES.get(name)
        if strategy is not None and strategy.is_available(layout, linker):
            return strategy
    raise PatchLinkUnsupported(
        f"no patch-link strategy available for linker={linker.name!r} "
        f"(preference={preference!r})"
    )


def verify_patch_objects_linked(layout: BuildTreeLayout, build_dir: Path, config: str, patch_stem: str) -> bool:
    """Whether the patch file contributed an object to the link.

    Weak on its own — it proves the file was compiled, not that its symbols won.
    It exists to catch the "CMakeLists surgery silently did nothing" case, which
    would otherwise look identical to success.  `verify_patch_active` is the
    real check.
    """
    import glob as _glob

    return bool(_glob.glob(layout.object_glob(build_dir, config, patch_stem)))


def verify_patch_active(hotupdate_data: dict, expected_patch_count: int = 0) -> tuple[bool, str]:
    """Confirm hotupdate actually applied patches, from the run's parsed output.

    Replaces the old compile-order log parsing.  The property that matters is
    not "which object linked first" but "did the patch become active" — and the
    hotupdate stage already runs the binary (with ``--patch-data``) and parses
    exactly that document, so this inspects the result instead of running the
    binary a second time.

    Why not run the binary here: a bare ``--hotupdate`` has no patch data to
    apply, so it would report zero patches on a perfectly good build.  The
    check must read the same run the stage already trusts.

    ``expected_patch_count==0`` means "any non-zero count is acceptable"; pass a
    real expectation when the caller knows how many entries were generated.
    """
    if not hotupdate_data:
        return False, "no parsed --hotupdate payload"

    patched = hotupdate_data.get("patchedFact")
    if patched is None:
        return False, (
            "payload has no 'patchedFact' — the run did not reach the patch "
            "phase, so patch activation is unverified"
        )

    count = len(patched) if isinstance(patched, list) else int(patched or 0)
    if count <= 0:
        return False, (
            f"hotupdate applied 0 patches (expected {expected_patch_count or '>0'}) — "
            "the sentinel won the link, so this build has no patches"
        )
    if expected_patch_count > 0 and count < expected_patch_count:
        return False, (
            f"hotupdate applied {count} patches, fewer than the "
            f"{expected_patch_count} generated — partial patch data"
        )
    return True, f"hotupdate active: {count} patched entries"
