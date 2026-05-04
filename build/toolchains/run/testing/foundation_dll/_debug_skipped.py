"""Investigate threading-tasks-primitives: run full pipeline and capture all errors."""
from __future__ import annotations

import json
import subprocess
import sys
import shutil
from pathlib import Path

_HERE = Path(__file__).resolve().parent
_REPO_ROOT = _HERE.parents[4]
family_slug = "threading-tasks-primitives"
entrypoint_dir = _REPO_ROOT / "verification" / "foundation-dll" / "System.Private.CoreLib" / family_slug / "il2cpp_dist" / "entrypoint"
dll_path = str(entrypoint_dir / "build-output" / "ThreadingTasksPrimitivesNativeEntry.dll")
entry_pt = "ThreadingTasksPrimitivesNativeEntry/ThreadingTasksPrimitivesNativeEntry::Run:System.Int32(System.Int32)"

# Run convert
print("=== Step 1: Convert ===")
manifest = {
    "source": {"type": "managed-dlls", "assemblies": [dll_path], "entry": entry_pt}
}
with open(entrypoint_dir / "subject.manifest.json", "w", encoding="utf-8") as f:
    json.dump(manifest, f, indent=2)
closure_sp = entrypoint_dir / "closure-sp"
if closure_sp.exists():
    shutil.rmtree(closure_sp)
closure_sp.mkdir(parents=True, exist_ok=True)

subprocess.run(
    ["dotnet", "run", "--no-build", "--project", str(_REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver"),
     "--", "convert", str(entrypoint_dir), "--output", str(closure_sp)],
    capture_output=True, text=True, timeout=180,
)

ir_path = closure_sp / "analysis" / "aot-core-ir.json"
with open(ir_path, encoding="utf-8") as f:
    ir = json.load(f)
methods = ir.get("methods", [])

print(f"All {len(methods)} methods in IR:")
# Find the Method2 body to see what Task.Delay looks like
for m in methods:
    mid = m.get("methodSubjectId", m.get("name", "?"))
    print(f"\n=== {mid} ===")
    if mid.endswith("Method2"):
        print(f"  isStatic: {m.get('isStatic')}")
        print(f"  returnType: {m.get('returnType')}")
        for inst in m.get("instructions", []):
            op = inst.get("op", "")
            if op == "call":
                print(f"  CALL: {inst.get('callee', '?')}")
            elif op in ("ldtoken", "newobj"):
                print(f"  {op}: {inst}")

# Trim
print("\n=== Step 2: Trim ===")
subprocess.run(
    [sys.executable, str(_HERE / "trim_aot_core_ir.py"),
     "--input", str(ir_path), "--entry-prefix", "ThreadingTasksPrimitivesNativeEntry", "--output", str(ir_path)],
    capture_output=True, text=True, timeout=120,
)
with open(ir_path, encoding="utf-8") as f:
    trimmed = json.load(f)
print(f"After trim: {len(trimmed.get('methods', []))} methods")

# Emit with error capture
print("\n=== Step 3: Emit ===")
genuine_out = _REPO_ROOT / "verification" / "foundation-dll" / "System.Private.CoreLib" / family_slug / "il2cpp_dist" / "genuine"
genuine_out.mkdir(parents=True, exist_ok=True)
emit = subprocess.run(
    ["dotnet", "run", "--no-build", "--project", str(_REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver"),
     "--", "emit-native-aot", str(closure_sp / "analysis"), str(genuine_out)],
    capture_output=True, text=True, timeout=180,
)
print(f"Emit rc={emit.returncode}")
if emit.returncode != 0:
    # Search for NotSupportedException
    stderr = emit.stderr.splitlines()
    for i, line in enumerate(stderr):
        if "NotSupportedException" in line or "native-aot lowering does not support" in line:
            print(f"  EMIT FAIL: {stderr[i+1] if i+1 < len(stderr) else line}")
    # Count unique "does not support" errors
    errors = set()
    for i, line in enumerate(stderr):
        if "does not support unresolved call target" in line:
            target = line.split("'")[1] if "'" in line else line
            errors.add(target)
    print(f"  Unique unresolved targets ({len(errors)}):")
    for e in sorted(errors):
        print(f"    {e}")
else:
    cpp = genuine_out / "generated" / "native-aot.generated.cpp"
    print(f"Emit OK: {cpp.stat().st_size} bytes")
