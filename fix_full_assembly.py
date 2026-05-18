#!/usr/bin/env python3
"""Fix: skip abstract/interface methods in full-assembly reachability."""
import re

path = r"D:\agent\booming-il2cpp\src\managed\Chaos.IL2CPP.CodeGen\NativeAotLoweringPlanner.cs"
with open(path, "r", encoding="utf-8") as f:
    content = f.read()

old = '''        // In full-assembly mode (no entry point), all methods are AOT-reachable
        // since any method may be invoked via the RunNativeAot dispatch table
        // by the runtime harness (entry.exe loop over kAotMethodCount).
        if (fullAssemblyMode)
        {
            foreach (var m in methodsForLowering)
                aotReachableSubjectIds.Add(m.SubjectId);
        }'''

new = '''        // In full-assembly mode (no entry point), all methods are AOT-reachable
        // since any method may be invoked via the RunNativeAot dispatch table
        // by the runtime harness (entry.exe loop over kAotMethodCount).
        // Skip abstract/interface methods with 0 instructions — they have no
        // IL body to translate and get unreachable stubs instead.
        if (fullAssemblyMode)
        {
            foreach (var m in methodsForLowering)
            {
                if (m.Instructions.Count > 0)
                    aotReachableSubjectIds.Add(m.SubjectId);
            }
        }'''

if old in content:
    content = content.replace(old, new)
    with open(path, "w", encoding="utf-8") as f:
        f.write(content)
    print("OK: fix applied")
else:
    print("FAIL: old string not found — check whitespace")
    # Print hex of the relevant area for debugging
    idx = content.find("fullAssemblyMode")
    if idx > 0:
        snippet = content[idx-50:idx+200]
        print(repr(snippet))
