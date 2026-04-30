import json, re, sys
sys.path.insert(0, 'D:/agent/booming-il2cpp/build')

EMITTED_FUNCTIONS = {
    "chaos_reflection_get_type_name_value_from_handle": {
        "type": "type-name",
        "families": ["reflection/type", "reflection/member-complete"],
        "coverage": "Type.Name via switch on type handle",
    },
    "chaos_reflection_get_type_metadata_token_from_handle": {
        "type": "type-metadata-token",
        "families": ["reflection/type", "reflection/member-complete"],
        "coverage": "Type.MetadataToken via switch",
    },
    "chaos_reflection_create_type_value": {
        "type": "type-creation",
        "families": ["reflection/type"],
        "coverage": "typeof() -> System.Type object allocation",
    },
    "chaos_reflection_get_method_name_value_from_handle": {
        "type": "method-name",
        "families": ["reflection/member-complete"],
        "coverage": "MethodInfo.Name via switch",
    },
    "chaos_reflection_get_method_metadata_token_from_handle": {
        "type": "method-metadata-token",
        "families": ["reflection/member-complete"],
        "coverage": "MethodInfo.MetadataToken via switch",
    },
    "chaos_reflection_get_declaring_type_handle_from_type_handle": {
        "type": "declaring-type",
        "families": ["reflection/type", "reflection/member-complete"],
        "coverage": "Nested type parent handle lookup",
    },
    "chaos_reflection_get_member_name": {
        "type": "member-name-dispatch",
        "families": ["reflection/member-complete"],
        "coverage": "Type/Method/Field -> Name dispatch by header type_id",
    },
    "chaos_reflection_get_declaring_type": {
        "type": "declaring-type-dispatch",
        "families": ["reflection/type", "reflection/member-complete"],
        "coverage": "Type/Method/Field -> DeclaringType by header type_id",
    },
    "chaos_reflection_get_metadata_token": {
        "type": "metadata-token-dispatch",
        "families": ["reflection/member-complete"],
        "coverage": "Type/Method/Field -> MetadataToken by header type_id",
    },
    "chaos_reflection_get_parameter_name": {
        "type": "parameter-name",
        "families": ["reflection/parameters"],
        "coverage": "ParameterInfo.Name field access",
    },
    "chaos_reflection_get_field": {
        "type": "field-lookup",
        "families": ["reflection/type", "reflection/member-complete"],
        "coverage": "Type.GetField(name) -> FieldInfo with name switch",
    },
    "chaos_reflection_get_generic_type_definition": {
        "type": "generic-type-definition",
        "families": ["reflection/generics"],
        "coverage": "Type.GetGenericTypeDefinition()",
    },
    "chaos_reflection_get_generic_arguments": {
        "type": "generic-arguments",
        "families": ["reflection/generics"],
        "coverage": "Type.GetGenericArguments() with array",
    },
    "chaos_reflection_get_constructors": {
        "type": "constructor-enumeration",
        "families": ["reflection/type", "reflection/activation"],
        "coverage": "Type.GetConstructors(BindingFlags) with array",
    },
    "chaos_reflection_get_parameters": {
        "type": "parameter-enumeration",
        "families": ["reflection/parameters"],
        "coverage": "MethodBase.GetParameters() with per-method switch",
    },
    "chaos_reflection_get_assembly": {
        "type": "assembly-lookup",
        "families": ["reflection/assembly"],
        "coverage": "Type.Assembly -> Assembly object by type handle",
    },
    "chaos_reflection_resolve_type_handle": {
        "type": "type-handle-resolution",
        "families": ["reflection/assembly", "reflection/type"],
        "coverage": "Assembly+TypeName -> TypeHandle (string compare)",
    },
    "chaos_reflection_get_type_from_assembly": {
        "type": "assembly-get-type",
        "families": ["reflection/assembly"],
        "coverage": "Assembly.GetType(name)",
    },
    "chaos_reflection_get_type_by_name": {
        "type": "type-get-type",
        "families": ["reflection/type"],
        "coverage": "Type.GetType(name)",
    },
}

DETECTION_FUNCTIONS = {
    "IsTypeReflectionHelperSubjectId": {
        "triggers_on": [
            "Type.GetTypeFromHandle(RuntimeTypeHandle)",
            "Type.get_TypeHandle",
            "Type.GetField(string)",
            "Type.GetMethod(string)",
            "Type.GetGenericArguments",
            "MemberInfo.get_DeclaringType",
            "Type.GetMethod(string,BindingFlags)",
            "Type.get_Assembly",
            "Type.GetGenericTypeDefinition",
            "Type.GetConstructors(BindingFlags)",
        ],
        "emission_flag": "flag2",
        "enables": 15,
    },
    "IsAssemblyReflectionHelperSubjectId": {
        "triggers_on": [
            "Type.get_Assembly",
            "Assembly.GetType(string)",
            "Type.GetType(string)",
            "Assembly.GetName",
            "AssemblyName.get_Name",
        ],
        "emission_flag": "flag3",
        "enables": 4,
    },
    "IsReflectionMemberHelperSubjectId": {
        "triggers_on": [
            "Type.GetMethod(string)",
            "Type.GetMethod(string,BindingFlags)",
            "MethodBase.GetParameters",
            "MemberInfo.get_Name",
            "ParameterInfo.get_Name",
            "MethodBase.get_MethodHandle",
            "MethodBase.Invoke(object,object[])",
        ],
        "emission_flag": "flag4",
        "enables": 0,  # gates conditional blocks, not standalone helpers
    },
}

from collections import defaultdict
family_coverage = defaultdict(list)
for func_name, info in EMITTED_FUNCTIONS.items():
    for fam in info["families"]:
        family_coverage[fam].append(func_name)

print("=" * 60)
print("Phase 0c: ReflectionObjectEmission Coverage Audit")
print("=" * 60)
print()
print(f"Emitted helper functions: {len(EMITTED_FUNCTIONS)}")
print(f"Detection functions: {len(DETECTION_FUNCTIONS)}")
print(f"Taxonomy families reachable: {len(family_coverage)}")
print()

for fam in sorted(family_coverage.keys()):
    funcs = family_coverage[fam]
    print(f"  [{fam}] {len(funcs)} helpers")
    for fn in sorted(funcs):
        info = EMITTED_FUNCTIONS[fn]
        print(f"    + {fn}")
        print(f"      {info['coverage']}")

print()
print("Detection gates:")
for func_name, info in DETECTION_FUNCTIONS.items():
    print(f"  {func_name} ({info['emission_flag']}): {len(info['triggers_on'])} triggers, enables {info['enables']} helpers")
    for t in info["triggers_on"]:
        print(f"    - {t}")

print()
print("Coverage Gaps:")
gaps = [
    ("Type.IsEnum/IsValueType/IsAbstract/IsSealed/IsInterface/IsArray",
     "NO dedicated helper - no type attribute query emission"),
    ("Type.FullName/Namespace/AssemblyQualifiedName",
     "Partial - get_type_name_value_from_handle returns short name only"),
    ("MethodBase.Invoke(object,object[])",
     "No dedicated emit helper but handled by ExternalRuntimeHelpers bridge pattern"),
    ("FieldInfo.GetValue/SetValue",
     "No dedicated emit helper but handled by runtime_abi.h bridge functions"),
    ("Activator.CreateInstance",
     "No dedicated emit helper - needs constructor resolution path"),
    ("Type.GetMethods/GetProperties/GetEvents",
     "NOT emitted - only GetConstructors exists for member enumeration"),
    ("BindingFlags filtering in GetConstructors",
     "NOT implemented - BindingFlags arg is ignored in emitted switch"),
    ("Module reflection",
     "NOT emitted - no Assembly.GetModule or Module support"),
    ("EventInfo reflection",
     "NOT emitted - no EventInfo emission at all"),
]
for gap, note in gaps:
    print(f"  ! {gap}")
    print(f"    {note}")

print()
print("Summary:")
print(f"  Implemented helpers: {len(EMITTED_FUNCTIONS)}")
print(f"  Detection gates: {len(DETECTION_FUNCTIONS)}")
print(f"  Families partially covered: {len(family_coverage)}")
print(f"  Coverage gaps: {len(gaps)}")
