import json

chunk_dir = "tests/e2e/translation/System.Private.CoreLib/chunks/numerics"
meta = json.load(open(f"{chunk_dir}/managed/subjects/subjects.metadata.json"))

partition = json.load(open("tests/e2e/translation/System.Private.CoreLib/_dll/namespace-partition.json"))
numerics_chunk = [c for c in partition["chunks"] if c["slug"] == "numerics"][0]
chunk_methods = set(m["methodSubjectId"] for m in numerics_chunk["methods"])
covered = set(m["methodSubjectId"] for m in meta["methods"])
missing = chunk_methods - covered

intrinsic_bases = {"Vector2", "Vector3", "Vector4", "Matrix4x4", "Plane", "Quaternion",
                   "Vector", "Matrix", "Vector128", "Vector256", "Vector64",
                   "BitOperations", "Matrix3x2"}

cats = {"hw_intrinsic": 0, "fixable": []}
for m in sorted(missing):
    type_part = m.split("::")[0] if "::" in m else m.split("/")[-1]
    base = type_part.rsplit(".", 1)[-1] if "." in type_part else type_part
    base = base.split("`")[0]
    if base in intrinsic_bases:
        cats["hw_intrinsic"] += 1
    else:
        cats["fixable"].append(m)

print(f"numerics gap analysis:")
print(f"  total missing: {len(missing)}")
print(f"  hardware intrinsics (cannot probe): {cats['hw_intrinsic']}")
print(f"  potentially fixable: {len(cats['fixable'])}")
if cats["fixable"]:
    print()
    print("fixable methods:")
    for m in cats["fixable"]:
        print(f"  {m}")
