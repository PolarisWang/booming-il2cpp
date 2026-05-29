"""pytest configuration for smoke tests in testing/tests/."""
from __future__ import annotations

import sys
from pathlib import Path

_HERE = Path(__file__).resolve().parent
_FOUNDATION_DLL = _HERE.parent / "foundation-dll"
if str(_FOUNDATION_DLL) not in sys.path:
    sys.path.insert(0, str(_FOUNDATION_DLL))
