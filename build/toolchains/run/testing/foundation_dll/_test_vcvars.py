"""Test different vcvars_prefix execution methods."""
from __future__ import annotations

import subprocess
from pathlib import Path

VCVARS = r'"C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat"'
CMD = VCVARS + ' x64 >/dev/null 2>/dev/null && cl.exe --version 2>&1'

print('=== Method 1: shell=True ===')
r1 = subprocess.run(CMD, shell=True, capture_output=True, text=True, timeout=30)
print(f'rc={r1.returncode}')
if r1.returncode == 0:
    print(r1.stdout[:300])
else:
    print(f'stderr: {r1.stderr[:200]}')

print()
print('=== Method 2: cmd.exe /c direct ===')
r2 = subprocess.run(['cmd.exe', '/c', CMD], capture_output=True, text=True, timeout=30)
print(f'rc={r2.returncode}')
if r2.returncode == 0:
    print(r2.stdout[:300])
else:
    print(f'stdout: {r2.stdout[:200]}')
    print(f'stderr: {r2.stderr[:200]}')
