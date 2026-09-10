"""Unified path setup — called only by entry_points to register the verification package."""
import sys
from pathlib import Path

_HERE = Path(__file__).resolve().parent  # verification/
if str(_HERE) not in sys.path:
    sys.path.insert(0, str(_HERE))
