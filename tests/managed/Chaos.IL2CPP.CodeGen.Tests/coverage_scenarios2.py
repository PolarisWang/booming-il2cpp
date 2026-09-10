"""Calculate coverage with various exclusion strategies."""
import xml.etree.ElementTree as ET
from collections import defaultdict
import sys

tree = ET.parse(sys.argv[1])
root = tree.getroot()
pkg = root.find('.//package[@name="Chaos.IL2CPP.Generator"]')
target = 'CodeGen'

file_data = defaultdict(lambda: {"c": 0, "t": 0})

for cls in pkg.findall('.//class'):
    fn = cls.get('filename', '')
    if target not in fn:
        continue
    fn_norm = fn.replace('\\', '/')
    lines = cls.findall('.//line')
    if not lines:
        continue
    covered = sum(1 for l in lines if int(l.get('hits', '0')) > 0)
    t = len(lines)
    file_data[fn_norm]["c"] += covered
    file_data[fn_norm]["t"] += t

# Scenario A: All source code (exclude obj/ only)
total_a = sum(d["t"] for fn, d in file_data.items() if '/obj/' not in fn)
covered_a = sum(d["c"] for fn, d in file_data.items() if '/obj/' not in fn)
print(f"A) Exclude obj/ only:           {covered_a}/{total_a} = {covered_a/total_a*100:.1f}%")

# Scenario B: Exclude obj/ + ReferenceProof/
total_b = sum(d["t"] for fn, d in file_data.items()
              if '/obj/' not in fn and '/ReferenceProof/' not in fn)
covered_b = sum(d["c"] for fn, d in file_data.items()
                if '/obj/' not in fn and '/ReferenceProof/' not in fn)
print(f"B) Exclude obj/ + ReferenceProof: {covered_b}/{total_b} = {covered_b/total_b*100:.1f}%")

# Scenario C: Exclude obj/ + ReferenceProof/ + RuntimeSkeleton
total_c = sum(d["t"] for fn, d in file_data.items()
              if '/obj/' not in fn and '/ReferenceProof/' not in fn
              and 'RuntimeSkeleton' not in fn
              and 'NativeReferenceProofEmitter' not in fn
              and 'NativeReferenceLoweringPlanner' not in fn)
covered_c = sum(d["c"] for fn, d in file_data.items()
                if '/obj/' not in fn and '/ReferenceProof/' not in fn
                and 'RuntimeSkeleton' not in fn
                and 'NativeReferenceProofEmitter' not in fn
                and 'NativeReferenceLoweringPlanner' not in fn)
print(f"C) Exclude obj/ + ReferenceProof + NativeReference*: {covered_c}/{total_c} = {covered_c/total_c*100:.1f}%")

# Scenario D: What was the previous setup (exclude by file patterns from csproj)
# Match the patterns that were in the original csproj
import fnmatch
excluded_patterns = [
    '**/ReferenceProof/*',
    '**/NativeReferenceProof*',
    '**/RuntimeSkeleton*',
    '**/GenericCapabilityMatrix*',
    '**/NativeAotEmitter*',
    '**/PatchData*',
    '**/Pluginization/*',
    '**/BuildSystem/*',
    '**/CodeGenRuntimeProviderCatalog*',
    '**/GeneratedLoweringPlanPluginMetadata*',
    '**/GeneratedRuntimeProviderPluginMetadata*',
    '**/GeneratedRuntimeSkeletonFamilyPluginMetadata*',
    '**/GeneratedTemplateBundlePluginMetadata*',
    '**/GeneratedTranslationUnitRenderPlan*',
    '**/Lowering/GeneratedTranslationUnitRenderPlan*',
]

def is_excluded(fn_norm):
    for pat in excluded_patterns:
        if fnmatch.fnmatch(fn_norm, pat):
            return True
    return False

total_d = sum(d["t"] for fn, d in file_data.items() if '/obj/' not in fn and not is_excluded(fn))
covered_d = sum(d["c"] for fn, d in file_data.items() if '/obj/' not in fn and not is_excluded(fn))
print(f"D) Original csproj exclusions:    {covered_d}/{total_d} = {covered_d/total_d*100:.1f}%")

# Scenario E: Exclude only the truly generated/skeleton files
# (ReferenceProof is generated code, rest is actual logic)
total_e = sum(d["t"] for fn, d in file_data.items()
              if '/obj/' not in fn and '/ReferenceProof/' not in fn
              and '/PatchData' not in fn)
covered_e = sum(d["c"] for fn, d in file_data.items()
                if '/obj/' not in fn and '/ReferenceProof/' not in fn
                and '/PatchData' not in fn)
print(f"E) Exclude obj/ + ReferenceProof + PatchData: {covered_e}/{total_e} = {covered_e/total_e*100:.1f}%")
