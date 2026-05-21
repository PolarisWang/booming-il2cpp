"""Quick runner for project-test pipeline — sets up paths correctly."""
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))       # testing/project-test/
sys.path.insert(0, str(Path(__file__).resolve().parent.parent))  # testing/

from _core.python.cli import main
main()
