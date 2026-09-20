"""platform — build-system and linker abstractions for the verification stages.

Why this package exists
-----------------------
The hotupdate stage performs a *distinct linking* trick: `patch-host-arrays.cpp`
and `runtime-patchdata.cpp` both define the `kPatchDataHost*` symbols, and the
one whose object file the linker sees **first** wins.  Making that work requires
three things that differ per platform:

  1. where the build system puts intermediate objects and the executable
     (`layout.py`),
  2. whether the linker can be told to tolerate the duplicate symbols, and that
     it keeps the first (`linker.py`),
  3. how to guarantee the patch object precedes the sentinel (`patch_link.py`).

Historically all three were hardcoded to MSVC + the Visual Studio generator:
paths like ``chaos_entry.dir/RelWithDebInfo/chaos_entry.exe``, a
``_msvc_toolchain_available()`` guard, and a post-hoc check that parsed the
*MSBuild* build log to infer compilation order.  On any other platform the
stage simply skipped — so hotupdate coverage silently did not exist on Linux,
and the chunk could never reach a full pass.

The abstractions split those three concerns so that adding a platform is a
registry entry rather than a new branch.  See
``wiki/06-测试验证/模块/平台抽象层.md``.
"""

from .layout import BuildTreeLayout, MsBuildLayout, NinjaLayout, layout_for_platform
from .linker import LinkerProfile, linker_for_platform
from .patch_link import (
    BuildOutcome,
    PatchLinkStrategy,
    PatchLinkUnsupported,
    select_strategy,
    verify_patch_active,
    verify_patch_objects_linked,
)
from .registry import PLATFORM_REGISTRY, PlatformSpec, spec_for_current_platform

__all__ = [
    "BuildTreeLayout",
    "MsBuildLayout",
    "NinjaLayout",
    "layout_for_platform",
    "LinkerProfile",
    "linker_for_platform",
    "PlatformSpec",
    "PLATFORM_REGISTRY",
    "spec_for_current_platform",
    "PatchLinkStrategy",
    "PatchLinkUnsupported",
    "BuildOutcome",
    "select_strategy",
    "verify_patch_active",
    "verify_patch_objects_linked",
]
