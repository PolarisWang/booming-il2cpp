"""Runner script for batch verification with unbuffered output."""
import os, sys

# Add paths
REPO = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
for p in [
    os.path.join(REPO, "testing", "foundation-dll"),
    os.path.join(REPO, "testing", "foundation-dll", "verification"),
    os.path.join(REPO, "build", "toolchains", "run", "testing", "foundation_dll"),
    os.path.join(REPO, "build", "toolchains", "run"),
]:
    if p not in sys.path:
        sys.path.insert(0, p)

sys.stdout = os.fdopen(sys.stdout.fileno(), 'w', 1)  # line-buffered
sys.stderr = os.fdopen(sys.stderr.fileno(), 'w', 1)  # line-buffered

from verification.entry_points.batch import main
main()
