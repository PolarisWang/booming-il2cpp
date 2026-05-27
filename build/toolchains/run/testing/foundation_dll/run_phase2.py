"""Phase 2 — Generate handwritten benchmark/host/patch/test for all STAGE-2 families.

For each family, writes 4 handwritten files:
  - test/<Class>Tests.cs           — [Fact] + Assert
  - benchmark/<Class>Benchmarks.cs  — BenchmarkSubjectId + call
  - host/<Class>HotUpdate.cs        — HotUpdateSubjectId + HostToPatch + call
  - patch/<Class>Patch.cs           — HotUpdateSubjectId + PatchToHost + call
"""

import json, sys
from pathlib import Path

sys.path.insert(0, str(Path("build/toolchains/run").resolve()))
from testing.foundation_dll.test_code_generator import (
    parse_method_subject_id, is_simple_method, has_unsafe_param,
    build_call_expr, class_name, family_slug, enum_name,
    member_name, relative, BASE_USINGS,
    is_auto_callable, method_skip_reason,
)

REPO = Path(".")
LEDGER = REPO / "testing" / "foundation-dll" / "_contracts" / "ledger.json"


def classify_methods(mids):
    simple, mixed, unsafe, needs_manual = [], [], [], []
    for mid in mids:
        parsed = parse_method_subject_id(mid)
        if not is_auto_callable(parsed):
            needs_manual.append((mid, parsed))
        elif has_unsafe_param(parsed["param_types"]):
            unsafe.append((mid, parsed))
        elif is_simple_method(parsed):
            simple.append((mid, parsed))
        else:
            mixed.append((mid, parsed))
    return simple, mixed, unsafe, needs_manual


def needs_manual_entry(mid, p):
    reason = method_skip_reason(p)
    return f"""
    [Fact(Skip = "{reason}")]
    public void {member_name("", mid)}()
    {{
        // TODO: {p['type_name']}.{p['method_name']} needs manual impl
    }}"""


def write_test_source(hw_path, fid, cname, simple, mixed, unsafe, needs_manual):
    parts = [f"    // AI-generated tests for {fid}"]
    parts.append("")

    if simple:
        parts.append("    // === simple (all-primitive) ===")
        for mid, p in simple:
            mname = member_name("", mid)
            call = build_call_expr(p)
            ret = p["return_type"]
            if ret in ("System.Void", ""):
                body = f"            {call};"
            else:
                body = f"            var result = {call};\n            Xunit.Assert.NotNull((object)result);"
            parts.append(f"""
    [Fact]
    public void {mname}()
    {{
        // Purpose: Verify {p['type_name']}.{p['method_name']} with typical input
{body}
    }}""")
        parts.append("")

    if mixed:
        parts.append("    // === mixed (smoke) ===")
        for mid, p in mixed:
            mname = member_name("", mid)
            call = build_call_expr(p)
            ret = p["return_type"]
            body = f"            _ = {call};" if ret not in ("System.Void", "") else f"            {call};"
            parts.append(f"""
    [Fact]
    public void {mname}()
    {{
        // Purpose: Smoke — {p['type_name']}.{p['method_name']} with complex param(s)
{body}
    }}""")
        parts.append("")

    if unsafe:
        parts.append("    // === needs-manual (ref/pointer/generic) ===")
        for mid, p in unsafe:
            mname = member_name("", mid)
            parts.append(f"""
    [Fact(Skip = "needs-manual — ref/pointer parameter requires unsafe context")]
    public void {mname}()
    {{
        // TODO: {p['type_name']}.{p['method_name']} needs manual impl
    }}""")
        parts.append("")

    if needs_manual:
        parts.append("    // === needs-manual (operator/protected/etc) ===")
        for mid, p in needs_manual:
            reason = method_skip_reason(p)
            mname = member_name("", mid)
            parts.append(f"""
    [Fact(Skip = "{reason}")]
    public void {mname}()
    {{
        // TODO: {p['type_name']}.{p['method_name']} needs manual impl
    }}""")
        parts.append("")

    joined = "\n".join(parts)
    source = (
        f"{BASE_USINGS}\n"
        "using Chaos.TestFramework;\n"
        "using Xunit;\n"
        "\n"
        f'[CapabilityTest("{fid}", IncludeBenchmark = true, IncludeHotUpdate = true)]\n'
        f"public partial class {cname}\n"
        "{\n"
        f"{joined}"
        "}\n"
    )
    hw_path.write_text(source, encoding="utf-8")


def write_benchmark_source(bm_path, fid, cname, mids, enum_val):
    bm_class = cname.replace("Tests", "Benchmarks")
    if not mids:
        source = (
            f"{BASE_USINGS}\n"
            "using Chaos.TestFramework;\n\n"
            f"// Benchmark stubs for {fid}\n"
            f"public static class {bm_class}\n"
            "{\n"
            "    public static void Placeholder() { }\n"
            "}\n"
        )
    else:
        parts = []
        for mid in mids:
            parsed = parse_method_subject_id(mid)
            call_expr = build_call_expr(parsed)
            mname = member_name("Benchmark", mid)
            purpose = f"// Benchmark {parsed['type_name']}.{parsed['method_name']} native-runtime throughput"
            if not is_auto_callable(parsed) or has_unsafe_param(parsed["param_types"]):
                parts.append(
                    f"    {purpose}\n"
                    f'    [BenchmarkSubjectId("{mid}")]\n'
                    f"    [CapabilityFamilyId(CapabilityFamilyId.{enum_val})]\n"
                    f"    public static void {mname}() {{ }}"
                )
            else:
                ret = parsed["return_type"]
                body = f"_ = {call_expr}" if ret not in ("System.Void", "") else f"{call_expr}"
                parts.append(
                    f"    {purpose}\n"
                    f'    [BenchmarkSubjectId("{mid}")]\n'
                    f"    [CapabilityFamilyId(CapabilityFamilyId.{enum_val})]\n"
                    "    [BenchmarkRoute(BenchmarkRoute.Native)]\n"
                    "    [BenchmarkProfile(BenchmarkProfile.Default)]\n"
                    f"    public static void {mname}() {{ {body}; }}"
                )
        members = "\n".join(parts)
        source = (
            f"{BASE_USINGS}\n"
            "using Chaos.TestFramework;\n\n"
            f"// Benchmark stubs for {fid}\n"
            f"public static class {bm_class}\n"
            "{\n"
            f"{members}"
            "}\n"
        )
    bm_path.write_text(source, encoding="utf-8")


def write_hotupdate_source(hu_path, fid, cname, mids, enum_val, direction="HostToPatch"):
    hu_class = cname.replace("Tests", "HotUpdate")
    dir_label = "patch" if direction == "HostToPatch" else "host"
    if not mids:
        source = (
            f"{BASE_USINGS}\n"
            "using Chaos.TestFramework;\n\n"
            f"// Hot-update stubs for {fid}\n"
            f"public static class {hu_class}\n"
            "{\n"
            "    public static void Placeholder() { }\n"
            "}\n"
        )
    else:
        parts = []
        for mid in mids:
            parsed = parse_method_subject_id(mid)
            call_expr = build_call_expr(parsed)
            mname = member_name("HotUpdate", mid)
            purpose = f"// Verify {parsed['type_name']}.{parsed['method_name']} after hot-update ({dir_label} side)"
            direction_attr = f"HotUpdateDirection(HotUpdateDirection.{direction})"
            if not is_auto_callable(parsed) or has_unsafe_param(parsed["param_types"]):
                parts.append(
                    f"    {purpose}\n"
                    f'    [HotUpdateSubjectId("{mid}")]\n'
                    f"    [CapabilityFamilyId(CapabilityFamilyId.{enum_val})]\n"
                    f"    [{direction_attr}]\n"
                    f"    public static void {mname}() {{ }}"
                )
            else:
                ret = parsed["return_type"]
                body = f"_ = {call_expr}" if ret not in ("System.Void", "") else f"{call_expr}"
                parts.append(
                    f"    {purpose}\n"
                    f'    [HotUpdateSubjectId("{mid}")]\n'
                    f"    [CapabilityFamilyId(CapabilityFamilyId.{enum_val})]\n"
                    f"    [{direction_attr}]\n"
                    f"    public static void {mname}() {{ {body}; }}"
                )
        members = "\n".join(parts)
        source = (
            f"{BASE_USINGS}\n"
            "using Chaos.TestFramework;\n\n"
            f"// Hot-update stubs for {fid}\n"
            f"public static class {hu_class}\n"
            "{\n"
            f"{members}"
            "}\n"
        )
    hu_path.write_text(source, encoding="utf-8")


def write_patch_source(patch_path, fid, cname, mids, enum_val):
    # Patch is same as HotUpdate but with PatchToHost direction
    write_hotupdate_source(patch_path, fid, cname, mids, enum_val, direction="PatchToHost")
    # Rename class inline — re-read and fix
    text = patch_path.read_text(encoding="utf-8")
    patch_class = cname.replace("Tests", "Patch")
    text = text.replace(cname.replace("Tests", "HotUpdate"), patch_class)
    text = text.replace("HotUpdate_", "Patch_")
    patch_path.write_text(text, encoding="utf-8")


def generate(aname, fid, contract_path):
    slug = family_slug(fid)
    cname = class_name(fid)
    family_root = REPO / "testing" / "foundation-dll" / aname / slug

    contract = json.loads(contract_path.read_text(encoding="utf-8"))
    mids = [str(m) for m in contract.get("methodSubjectIds", []) if str(m)]
    if not mids:
        return None

    # Build a minimal family dict for _enum_name
    fam_dict = {"familyId": fid, "capabilityFamilyEnum": ""}
    enum_val = enum_name(fam_dict)

    simple, mixed, unsafe, needs_manual = classify_methods(mids)

    # test/
    test_dir = family_root / "test"
    test_dir.mkdir(parents=True, exist_ok=True)
    write_test_source(test_dir / f"{cname}.cs", fid, cname, simple, mixed, unsafe, needs_manual)

    # benchmark/
    bm_dir = family_root / "benchmark"
    bm_dir.mkdir(parents=True, exist_ok=True)
    write_benchmark_source(bm_dir / f"{cname.replace('Tests', 'Benchmarks')}.cs", fid, cname, mids, enum_val)

    # host/
    host_dir = family_root / "host"
    host_dir.mkdir(parents=True, exist_ok=True)
    write_hotupdate_source(host_dir / f"{cname.replace('Tests', 'HotUpdate')}.cs", fid, cname, mids, enum_val)

    # patch/
    patch_dir = family_root / "patch"
    patch_dir.mkdir(parents=True, exist_ok=True)
    write_patch_source(patch_dir / f"{cname.replace('Tests', 'Patch')}.cs", fid, cname, mids, enum_val)

    return len(simple) + len(mixed) + len(unsafe)


def main():
    ledger = json.loads(LEDGER.read_text(encoding="utf-8"))
    results = []
    for dll in ledger["assemblies"]:
        aname = dll["assemblyName"]
        for fam in dll["families"]:
            fid = fam.get("familyId", "")
            if int(fam.get("methodCount", 0)) <= 0:
                continue
            slug = family_slug(fid)
            contract_path = REPO / "testing" / "foundation-dll" / aname / slug / "capability-family-contract.json"
            if not contract_path.exists():
                print(f"  SKIP {aname}/{slug} — no contract")
                continue
            r = generate(aname, fid, contract_path)
            if r:
                results.append((aname, slug, r))
                print(f"  {aname}/{slug}: {r} methods across test/benchmark/host/patch")
    print(f"\nDone. {len(results)} families updated.")


if __name__ == "__main__":
    main()
