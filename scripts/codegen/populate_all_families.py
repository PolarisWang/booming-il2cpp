import json

LEDGER_PATH = "D:/agent/booming-il2cpp/verification/projections/foundation-dll-audit/capability-family-ledger.json"

with open(LEDGER_PATH, encoding="utf-8") as f:
    data = json.load(f)

for dll in data["dlls"]:
    if dll["assemblyName"] != "System.Private.CoreLib":
        continue
    families = {fam["familyId"]: fam for fam in dll["families"]}

    # --- reflection/assembly (~22 methods) ---
    assembly_methods = [
        "System.Private.CoreLib/System.Reflection.Assembly::GetExecutingAssembly:System.Reflection.Assembly()",
        "System.Private.CoreLib/System.Reflection.Assembly::GetCallingAssembly:System.Reflection.Assembly()",
        "System.Private.CoreLib/System.Reflection.Assembly::GetEntryAssembly:System.Reflection.Assembly()",
        "System.Private.CoreLib/System.Reflection.Assembly::GetName:System.Reflection.AssemblyName()",
        "System.Private.CoreLib/System.Reflection.Assembly::get_FullName:System.String()",
        "System.Private.CoreLib/System.Reflection.Assembly::get_Location:System.String()",
        "System.Private.CoreLib/System.Reflection.Assembly::get_ImageRuntimeVersion:System.String()",
        "System.Private.CoreLib/System.Reflection.Assembly::GetTypes:System.Type[]()",
        "System.Private.CoreLib/System.Reflection.Assembly::GetExportedTypes:System.Type[]()",
        "System.Private.CoreLib/System.Reflection.Assembly::GetForwardedTypes:System.Type[]()",
        "System.Private.CoreLib/System.Reflection.Assembly::GetType:System.Type(System.String)",
        "System.Private.CoreLib/System.Reflection.Assembly::GetType:System.Type(System.String,System.Boolean)",
        "System.Private.CoreLib/System.Reflection.Assembly::GetModule:System.Reflection.Module(System.String)",
        "System.Private.CoreLib/System.Reflection.Assembly::GetModules:System.Reflection.Module[]()",
        "System.Private.CoreLib/System.Reflection.Assembly::GetManifestResourceStream:System.IO.Stream(System.String)",
        "System.Private.CoreLib/System.Reflection.Assembly::GetManifestResourceNames:System.String[]()",
        "System.Private.CoreLib/System.Reflection.Assembly::get_EntryPoint:System.Reflection.MethodInfo()",
        "System.Private.CoreLib/System.Reflection.AssemblyName::get_Name:System.String()",
        "System.Private.CoreLib/System.Reflection.AssemblyName::get_Version:System.Version()",
        "System.Private.CoreLib/System.Reflection.AssemblyName::get_CultureInfo:System.Globalization.CultureInfo()",
        "System.Private.CoreLib/System.Type::get_Assembly:System.Reflection.Assembly()",
        "System.Private.CoreLib/System.Type::GetType:System.Type(System.String)",
    ]
    af = families.get("family/System.Private.CoreLib/reflection/assembly")
    af["methodSubjectIds"] = assembly_methods
    af["methodCount"] = len(assembly_methods)

    # --- reflection/member-complete (~20 methods) ---
    member_complete = [
        "System.Private.CoreLib/System.Reflection.MemberInfo::get_Name:System.String()",
        "System.Private.CoreLib/System.Reflection.MemberInfo::get_MemberType:System.Reflection.MemberTypes()",
        "System.Private.CoreLib/System.Reflection.MemberInfo::get_DeclaringType:System.Type()",
        "System.Private.CoreLib/System.Reflection.MemberInfo::get_ReflectedType:System.Type()",
        "System.Private.CoreLib/System.Reflection.MemberInfo::get_MetadataToken:System.Int32()",
        "System.Private.CoreLib/System.Reflection.MethodInfo::get_ReturnType:System.Type()",
        "System.Private.CoreLib/System.Reflection.MethodInfo::GetBaseDefinition:System.Reflection.MethodInfo()",
        "System.Private.CoreLib/System.Reflection.MethodInfo::get_MethodHandle:System.RuntimeMethodHandle()",
        "System.Private.CoreLib/System.Reflection.MethodBase::get_CallingConvention:System.Reflection.CallingConventions()",
        "System.Private.CoreLib/System.Reflection.MethodBase::get_IsPublic:System.Boolean()",
        "System.Private.CoreLib/System.Reflection.MethodBase::get_IsStatic:System.Boolean()",
        "System.Private.CoreLib/System.Reflection.MethodBase::get_IsVirtual:System.Boolean()",
        "System.Private.CoreLib/System.Reflection.FieldInfo::get_FieldType:System.Type()",
        "System.Private.CoreLib/System.Reflection.FieldInfo::get_IsStatic:System.Boolean()",
        "System.Private.CoreLib/System.Reflection.FieldInfo::get_IsInitOnly:System.Boolean()",
        "System.Private.CoreLib/System.Reflection.FieldInfo::get_IsLiteral:System.Boolean()",
        "System.Private.CoreLib/System.Reflection.PropertyInfo::get_PropertyType:System.Type()",
        "System.Private.CoreLib/System.Reflection.PropertyInfo::get_CanRead:System.Boolean()",
        "System.Private.CoreLib/System.Reflection.PropertyInfo::get_CanWrite:System.Boolean()",
        "System.Private.CoreLib/System.Reflection.PropertyInfo::GetIndexParameters:System.Reflection.ParameterInfo[]()",
    ]
    mf = families.get("family/System.Private.CoreLib/reflection/member-complete")
    mf["methodSubjectIds"] = member_complete
    mf["methodCount"] = len(member_complete)

    # --- reflection/generics (~12 methods) ---
    generics_methods = [
        "System.Private.CoreLib/System.Type::GetGenericTypeDefinition:System.Type()",
        "System.Private.CoreLib/System.Type::MakeGenericType:System.Type(System.Type[])",
        "System.Private.CoreLib/System.Type::GetGenericArguments:System.Type[]()",
        "System.Private.CoreLib/System.Type::GetGenericParameterConstraints:System.Type[]()",
        "System.Private.CoreLib/System.Type::get_GenericParameterPosition:System.Int32()",
        "System.Private.CoreLib/System.Type::ContainsGenericParameters:System.Boolean()",
        "System.Private.CoreLib/System.Type::get_IsGenericType:System.Boolean()",
        "System.Private.CoreLib/System.Type::get_IsGenericTypeDefinition:System.Boolean()",
        "System.Private.CoreLib/System.Type::get_IsConstructedGenericType:System.Boolean()",
        "System.Private.CoreLib/System.Type::MakeArrayType:System.Type()",
        "System.Private.CoreLib/System.Type::MakeByRefType:System.Type()",
        "System.Private.CoreLib/System.Type::MakePointerType:System.Type()",
    ]
    gf = families.get("family/System.Private.CoreLib/reflection/generics")
    gf["methodSubjectIds"] = generics_methods
    gf["methodCount"] = len(generics_methods)

    # --- reflection/parameters (~10 methods) ---
    params_methods = [
        "System.Private.CoreLib/System.Reflection.ParameterInfo::get_Name:System.String()",
        "System.Private.CoreLib/System.Reflection.ParameterInfo::get_ParameterType:System.Type()",
        "System.Private.CoreLib/System.Reflection.ParameterInfo::get_DefaultValue:System.Object()",
        "System.Private.CoreLib/System.Reflection.ParameterInfo::get_RawDefaultValue:System.Object()",
        "System.Private.CoreLib/System.Reflection.ParameterInfo::get_HasDefaultValue:System.Boolean()",
        "System.Private.CoreLib/System.Reflection.ParameterInfo::get_Attributes:System.Reflection.ParameterAttributes()",
        "System.Private.CoreLib/System.Reflection.ParameterInfo::get_Position:System.Int32()",
        "System.Private.CoreLib/System.Reflection.ParameterInfo::GetRequiredCustomModifiers:System.Type[]()",
        "System.Private.CoreLib/System.Reflection.ParameterInfo::GetOptionalCustomModifiers:System.Type[]()",
        "System.Private.CoreLib/System.Reflection.MethodBase::GetParameters:System.Reflection.ParameterInfo[]()",
    ]
    pf = families.get("family/System.Private.CoreLib/reflection/parameters")
    pf["methodSubjectIds"] = params_methods
    pf["methodCount"] = len(params_methods)

    # --- reflection/module (~6 methods) ---
    module_methods = [
        "System.Private.CoreLib/System.Reflection.Module::get_FullyQualifiedName:System.String()",
        "System.Private.CoreLib/System.Reflection.Module::get_Name:System.String()",
        "System.Private.CoreLib/System.Reflection.Module::get_Assembly:System.Reflection.Assembly()",
        "System.Private.CoreLib/System.Reflection.Module::GetType:System.Type(System.String)",
        "System.Private.CoreLib/System.Reflection.Module::GetTypes:System.Type[]()",
        "System.Private.CoreLib/System.Reflection.Module::GetCustomAttributes:System.Object[](System.Type)",
    ]
    mof = families.get("family/System.Private.CoreLib/reflection/module")
    mof["methodSubjectIds"] = module_methods
    mof["methodCount"] = len(module_methods)

    # --- reflection/binding (~12 methods) ---
    binding_methods = [
        "System.Private.CoreLib/System.Type::GetMethod:System.Reflection.MethodInfo(System.String)",
        "System.Private.CoreLib/System.Type::GetMethod:System.Reflection.MethodInfo(System.String,System.Reflection.BindingFlags)",
        "System.Private.CoreLib/System.Type::GetMethod:System.Reflection.MethodInfo(System.String,System.Type[])",
        "System.Private.CoreLib/System.Type::GetField:System.Reflection.FieldInfo(System.String)",
        "System.Private.CoreLib/System.Type::GetField:System.Reflection.FieldInfo(System.String,System.Reflection.BindingFlags)",
        "System.Private.CoreLib/System.Type::GetProperty:System.Reflection.PropertyInfo(System.String)",
        "System.Private.CoreLib/System.Type::GetProperty:System.Reflection.PropertyInfo(System.String,System.Reflection.BindingFlags)",
        "System.Private.CoreLib/System.Type::GetProperty:System.Reflection.PropertyInfo(System.String,System.Type[])",
        "System.Private.CoreLib/System.Type::GetEvent:System.Reflection.EventInfo(System.String)",
        "System.Private.CoreLib/System.Type::GetEvent:System.Reflection.EventInfo(System.String,System.Reflection.BindingFlags)",
        "System.Private.CoreLib/System.Reflection.MethodBase::Invoke:System.Object(System.Object,System.Object[])",
        "System.Private.CoreLib/System.Reflection.MethodBase::Invoke:System.Object(System.Object,System.Reflection.BindingFlags,System.Reflection.Binder,System.Object[],System.Globalization.CultureInfo)",
    ]
    bf = families.get("family/System.Private.CoreLib/reflection/binding")
    bf["methodSubjectIds"] = binding_methods
    bf["methodCount"] = len(binding_methods)

    # --- reflection/activation (~8 methods) ---
    activation_methods = [
        "System.Private.CoreLib/System.Activator::CreateInstance:System.Object(System.Type)",
        "System.Private.CoreLib/System.Activator::CreateInstance:System.Object(System.Type,System.Object[])",
        "System.Private.CoreLib/System.Activator::CreateInstance:T()",
        "System.Private.CoreLib/System.Activator::CreateInstance:System.Object(System.Type,System.Reflection.BindingFlags,System.Reflection.Binder,System.Object[],System.Globalization.CultureInfo)",
        "System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32)",
        "System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32,System.Int32)",
        "System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32[])",
        "System.Private.CoreLib/System.Reflection.ConstructorInfo::Invoke:System.Object(System.Object[])",
    ]
    acf = families.get("family/System.Private.CoreLib/reflection/activation")
    acf["methodSubjectIds"] = activation_methods
    acf["methodCount"] = len(activation_methods)

    for fam_id in ["reflection/assembly", "reflection/member-complete", "reflection/generics",
                   "reflection/parameters", "reflection/module", "reflection/binding", "reflection/activation"]:
        full_id = f"family/System.Private.CoreLib/{fam_id}"
        f = families[full_id]
        print(f"{full_id}: {f['methodCount']} methods")

    break

with open(LEDGER_PATH, "w", encoding="utf-8") as f:
    json.dump(data, f, ensure_ascii=False, indent=2)
print("All families populated")
