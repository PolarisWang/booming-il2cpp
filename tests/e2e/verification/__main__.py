"""Unified CLI entry point for foundation-dll verification pipeline.

Usage:
    cd testing/foundation-dll

    # Run a single chunk
    python -m verification --chunk numerics
    python -m verification --all-chunks

    # Specific stages
    python -m verification --chunk io-2 --stages coverage-audit
"""

from __future__ import annotations

import sys
from pathlib import Path


def main() -> None:
    """Delegate to chunk_pipeline CLI."""
    pkg_root = Path(__file__).resolve().parent.parent
    if str(pkg_root) not in sys.path:
        sys.path.insert(0, str(pkg_root))

    from verification.chunk_pipeline import main as chunk_main
    sys.exit(chunk_main())


if __name__ == "__main__":
    main()
