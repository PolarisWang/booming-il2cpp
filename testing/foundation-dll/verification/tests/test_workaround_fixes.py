"""Verify all workaround fixes — syntax, unit tests, config consistency."""
import json
import sys
from pathlib import Path

sys.path.insert(0, "testing/foundation-dll")  # makes verification. importable

passed = 0
failed = 0


def check(name, condition, detail=""):
    global passed, failed
    if condition:
        passed += 1
        print(f"  PASS: {name}")
    else:
        failed += 1
        print(f"  FAIL: {name} {detail}")


# ── 1. Syntax check all modified files ──
print("=== Syntax check ===")
import py_compile
for f in [
    "testing/foundation-dll/verification/stages/build.py",
    "testing/foundation-dll/verification/stages/fact_chunk.py",
    "testing/foundation-dll/verification/stages/aggregate.py",
    "testing/foundation-dll/verification/stages/hephaestus_cache.py",
    "testing/foundation-dll/verification/stages/benchmark_chunk.py",
    "testing/foundation-dll/verification/reporting/dashboard.py",
    "testing/foundation-dll/verification/analysis/code_size_tracker.py",
    ".ai/skills/tooling/learning/skill_learn.py",
]:
    try:
        py_compile.compile(f, doraise=True)
        check(f, True)
    except py_compile.PyCompileError as e:
        check(f, False, str(e))


# ── 2. chunk.json config loading ──
print("\n=== chunk.json config loading ===")
from verification.stages.build import _load_chunk_config, _get_additional_assemblies
from verification.stages.fact_chunk import _is_jit_enabled

# Test missing chunk.json
config = _load_chunk_config(Path("/nonexistent"))
check("_load_chunk_config missing dir returns {}", config == {})

# Test valid chunk.json
import tempfile, os
with tempfile.TemporaryDirectory() as tmp:
    p = Path(tmp) / "chunk.json"
    p.write_text('{"additionalAssemblies": ["A.dll"], "jitEnabled": true, "slug": "test"}')
    config = _load_chunk_config(Path(tmp))
    check("_load_chunk_config reads valid json", config.get("additionalAssemblies") == ["A.dll"])
    check("_get_additional_assemblies returns list", _get_additional_assemblies(Path(tmp)) == ["A.dll"])
    check("_is_jit_enabled returns true", _is_jit_enabled(Path(tmp)) is True)

    # Test corrupt chunk.json
    p.write_text("not json")
    config = _load_chunk_config(Path(tmp))
    check("_load_chunk_config corrupt returns {}", config == {})

    # Test jitEnabled = false
    p.write_text('{"jitEnabled": false}')
    check("_is_jit_enabled false", _is_jit_enabled(Path(tmp)) is False)

    # Test jitEnabled missing
    p.write_text('{"slug": "test"}')
    check("_is_jit_enabled missing defaults to false", _is_jit_enabled(Path(tmp)) is False)


# ── 3. Created chunk.json files exist and are valid ──
print("\n=== chunk.json file consistency ===")
chunk_configs = [
    ("System.Private.CoreLib/security-cryptography",     "testing/foundation-dll/System.Private.CoreLib/chunks/security-cryptography/chunk.json"),
    ("System.Security.Cryptography/security-cryptography","testing/foundation-dll/System.Security.Cryptography/chunks/security-cryptography/chunk.json"),
    ("System.Linq/global-ns",                            "testing/foundation-dll/System.Linq/chunks/global-ns/chunk.json"),
    ("System.Linq.Expressions/global-ns",                "testing/foundation-dll/System.Linq.Expressions/chunks/global-ns/chunk.json"),
]
for label, path_str in chunk_configs:
    p = Path(path_str)
    if p.exists():
        try:
            data = json.loads(p.read_text(encoding="utf-8"))
            check(f"{label} chunk.json valid JSON", True)
            if "cryptography" in label or "CoreLib" in label:
                check(f"{label} has additionalAssemblies", "additionalAssemblies" in data)
            elif "Linq" in label:
                check(f"{label} has jitEnabled", data.get("jitEnabled") is True)
        except (json.JSONDecodeError, OSError) as e:
            check(f"{label} chunk.json", False, str(e))
    else:
        # CoreLib is gitignored, so its chunk.json won't exist in git
        if "CoreLib" in label:
            check(f"{label} chunk.json (gitignored, expected missing)", True)
        else:
            check(f"{label} chunk.json", False, "MISSING")


# ── 4. Verify no hardcoded crypto slug matching in build.py ──
print("\n=== No stale slug matching ===")
with open("testing/foundation-dll/verification/stages/build.py", encoding="utf-8") as f:
    content = f.read()
# The new code should have the slug match inside 'else:' fallback, not as primary
has_slug_fallback = 'any(x in ctx.slug for x in ("security-cryptography", "x509"))' in content
has_config_path = '_get_additional_assemblies(ctx.chunk_dir)' in content
check("config-driven is primary path", has_config_path)
check("slug fallback still present (for gitignored chunks)", has_slug_fallback)
# Verify config-driven path comes before slug fallback
idx_config = content.index("if config_assemblies:")
idx_slug = content.index('any(x in ctx.slug')
check("config-driven path comes before slug fallback", idx_config < idx_slug)


# ── 5. Verify hephaestus cache changes ──
print("\n=== Hephaestus cache ===")
with open("testing/foundation-dll/verification/stages/hephaestus_cache.py", encoding="utf-8") as f:
    content = f.read()
check("manifest corrupt prints ERROR (not WARNING)",
      'ERROR: Corrupt manifest' in content)
check("prune cleans stale entries",
      'entry.status == "stale"' in content)


# ── 6. Verify benchmark calibration WARNING ──
print("\n=== Benchmark calibration ===")
with open("testing/foundation-dll/verification/stages/benchmark_chunk.py", encoding="utf-8") as f:
    content = f.read()
check("probe fail calls _log_calibration_failure",
      '_log_calibration_failure' in content)
check("all-fast calls _log_calibration_failure",
      content.count('_log_calibration_failure') >= 2)


# ── 7. Verify DOTNET_ROOT has no hardcoded paths ──
print("\n=== DOTNET_ROOT ===")
with open("testing/foundation-dll/verification/stages/build.py", encoding="utf-8") as f:
    content = f.read()
check("no hardcoded /usr/share/dotnet paths",
      "/usr/share/dotnet" not in content)
check("dotnet --info fallback present",
      'dotnet", "--info"' in content or '"dotnet", "--info"' in content)


# ── 8. Verify aggregate meta-mismatch is hard error ──
print("\n=== Aggregate meta-mismatch ===")
with open("testing/foundation-dll/verification/stages/aggregate.py", encoding="utf-8") as f:
    content = f.read()
check("meta-mismatch sets status=error",
      'aggregate_status = "error"' in content)
check("no ratio-based thresholds",
      'ratio < 0.5' not in content)
check("any mismatch is error (not partial)",
      'aggregate_status = "partial"' not in content)


# ── 9. Verify value_suspicious appends to errors ──
print("\n=== Value suspicious ===")
with open("testing/foundation-dll/verification/stages/fact_chunk.py", encoding="utf-8") as f:
    content = f.read()
check("value_suspicious appends to errors",
      'errors.append(f"{value_warnings}' in content)
check("value_suspicious no longer sets partial",
      'Demoting status to partial:.*negative values' not in content)


# ── 10. Verify JIT config-driven check ──
print("\n=== JIT config-driven ===")
with open("testing/foundation-dll/verification/stages/fact_chunk.py", encoding="utf-8") as f:
    content = f.read()
check("_is_jit_enabled exists", "_is_jit_enabled" in content)
check("jitEnabled error if exe missing",
      'jit_enabled in chunk.json but entry-jit.exe not found' in content)


# ── Summary ──
print(f"\n=== {passed} passed, {failed} failed ===")
sys.exit(0 if failed == 0 else 1)
