using System.IO;
using System.Reflection;
using System.Reflection.Metadata;
using System.Reflection.Metadata.Ecma335;
using System.Reflection.PortableExecutable;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    private StaticInitializationSupportModel _staticInitializationSupport = StaticInitializationSupportModel.Empty;

    private sealed record StaticInitializationAction(
        string FieldSubjectId,
        string ConstructedTypeSubjectId,
        string ConstructorSubjectId,
        bool ElideConstructorCall);

    private sealed record StaticInitializationPlan(
        string TypeSubjectId,
        IReadOnlyList<StaticInitializationAction> Actions);

    private sealed record StaticInitializationSupportModel(
        IReadOnlyDictionary<string, StaticInitializationPlan> PlansByTypeSubjectId,
        IReadOnlyDictionary<string, string> DeclaringTypeByStaticFieldSubjectId,
        IReadOnlySet<string> RequiredExternalRuntimeHelperSubjectIds)
    {
        public static readonly StaticInitializationSupportModel Empty = new(
            new Dictionary<string, StaticInitializationPlan>(StringComparer.Ordinal),
            new Dictionary<string, string>(StringComparer.Ordinal),
            new HashSet<string>(StringComparer.Ordinal));
    }

    private StaticInitializationSupportModel BuildStaticInitializationSupportModel(
        IReadOnlyList<AotCoreIrMethodArtifact> reachableMethods,
        ManagedClosureManifestArtifact closureManifest)
    {
        var candidateTypeSubjectIds = new HashSet<string>(reachableMethods.Count, StringComparer.Ordinal);
        var declaringTypeByStaticFieldSubjectId = new Dictionary<string, string>(StringComparer.Ordinal);

        foreach (var method in reachableMethods)
        {
            if (method.IsStatic &&
                !string.IsNullOrEmpty(method.Identity.DeclaringTypeSubjectId))
            {
                candidateTypeSubjectIds.Add(method.Identity.DeclaringTypeSubjectId);
            }

            foreach (var instruction in method.Instructions)
            {
                if (instruction.TargetReference?.Kind != AotCoreIrReferenceKind.Field ||
                    instruction.RuntimeServiceKind is not (
                        AotCoreIrRuntimeServiceKind.LoadStaticField or
                        AotCoreIrRuntimeServiceKind.StoreStaticField))
                {
                    continue;
                }

                var fieldSubjectId = instruction.TargetReference.SubjectId;
                var declaringTypeSubjectId = GetRequiredDeclaringTypeSubjectId(instruction.TargetReference);
                candidateTypeSubjectIds.Add(declaringTypeSubjectId);
                declaringTypeByStaticFieldSubjectId[fieldSubjectId] = declaringTypeSubjectId;
            }
        }

        if (candidateTypeSubjectIds.Count == 0)
        {
            return StaticInitializationSupportModel.Empty;
        }

        // Read .cctor IL directly from PE metadata instead of loading full managed models.
        // This avoids the ~3s overhead of LoaderStage.LoadMultiple() which loads all closure
        // assemblies into a full managed model when we only need a few cctor method bodies.
        var peReaders = new Dictionary<string, (PEReader Pe, MetadataReader Md)>(StringComparer.OrdinalIgnoreCase);
        try
        {
            foreach (var kvp in _closureAssemblyPathByName)
            {
                try
                {
                    var stream = new FileStream(kvp.Value, FileMode.Open, FileAccess.Read, FileShare.Read);
                    var pe = new PEReader(stream);
                    peReaders[kvp.Key] = (pe, pe.GetMetadataReader());
                }
                catch
                {
                    // Skip assemblies that can't be opened
                }
            }

            var reachableMethodSubjectIds = reachableMethods
                .Select(method => method.SubjectId)
                .ToHashSet(StringComparer.Ordinal);
            var plansByTypeSubjectId = new Dictionary<string, StaticInitializationPlan>(candidateTypeSubjectIds.Count, StringComparer.Ordinal);
            var requiredExternalRuntimeHelperSubjectIds = new HashSet<string>(candidateTypeSubjectIds.Count, StringComparer.Ordinal);

            foreach (var typeSubjectId in candidateTypeSubjectIds.OrderBy(value => value, StringComparer.Ordinal))
            {
                // Parse assembly name from type subject ID: "AssemblyName/..."
                var slashIdx = typeSubjectId.IndexOf('/');
                if (slashIdx < 0) continue;
                var assemblyName = typeSubjectId.AsSpan(0, slashIdx);
                var fullTypeName = typeSubjectId.AsSpan(slashIdx + 1);

                if (!peReaders.TryGetValue(assemblyName.ToString(), out var reader))
                    continue;

                // Find TypeDefinition for this type
                var typeDefHandle = FindTypeDefinitionByFullName(reader.Md, fullTypeName);
                if (typeDefHandle.IsNil) continue;

                // Find .cctor method
                var cctorHandle = FindMethodByName(reader.Md, typeDefHandle, ".cctor");
                if (cctorHandle.IsNil) continue;

                var cctorMethodDef = reader.Md.GetMethodDefinition(cctorHandle);
                var cctorBody = reader.Pe.GetMethodBody(cctorMethodDef.RelativeVirtualAddress);
                if (cctorBody == null) continue;

                // Skip .cctors with exception regions — complex initialization is
                // handled lazily by the runtime at first type access.
                if (cctorBody.ExceptionRegions.Length > 0) continue;

                var plan = CreateStaticInitPlanFromIl(
                    typeSubjectId, cctorBody.GetILBytes()!,
                    reader.Md, reader.Pe,
                    reachableMethodSubjectIds,
                    requiredExternalRuntimeHelperSubjectIds);

                if (plan is null) continue;

                plansByTypeSubjectId[typeSubjectId] = plan;
            }

            if (plansByTypeSubjectId.Count == 0)
                return StaticInitializationSupportModel.Empty;

            return new StaticInitializationSupportModel(
                plansByTypeSubjectId,
                declaringTypeByStaticFieldSubjectId,
                requiredExternalRuntimeHelperSubjectIds);
        }
        finally
        {
            foreach (var kvp in peReaders)
                kvp.Value.Pe.Dispose();
        }
    }

    /// <summary>
    /// Find a TypeDefinitionHandle by full type name (e.g. "System.Byte" or "Outer+Inner").
    /// </summary>
    private static TypeDefinitionHandle FindTypeDefinitionByFullName(MetadataReader md, ReadOnlySpan<char> fullTypeName)
    {
        foreach (var handle in md.TypeDefinitions)
        {
            var typeDef = md.GetTypeDefinition(handle);
            var name = md.GetString(typeDef.Name);
            var ns = md.GetString(typeDef.Namespace);

            // Check for nested types (format: "OuterType+NestedType")
            var plusIdx = fullTypeName.IndexOf('+');
            if (plusIdx >= 0)
            {
                var outerName = fullTypeName.Slice(0, plusIdx);
                var nestedName = fullTypeName.Slice(plusIdx + 1);

                // Current type must be the outer type (check ns.Name == outerName)
                if (!TypeFullNameMatches(md, typeDef, ns, name, outerName))
                    continue;

                // Find nested type
                foreach (var nestedHandle in typeDef.GetNestedTypes())
                {
                    var nestedDef = md.GetTypeDefinition(nestedHandle);
                    var nestedDefName = md.GetString(nestedDef.Name);
                    if (nestedDefName.AsSpan().Equals(nestedName, StringComparison.Ordinal))
                        return nestedHandle;
                }
                return default;
            }

            if (TypeFullNameMatches(md, typeDef, ns, name, fullTypeName))
                return handle;
        }

        return default;
    }

    private static bool TypeFullNameMatches(MetadataReader md, TypeDefinition typeDef,
        string ns, string name, ReadOnlySpan<char> fullTypeName)
    {
        if (string.IsNullOrEmpty(ns))
            return name.AsSpan().Equals(fullTypeName, StringComparison.Ordinal);
        return fullTypeName.Length > ns.Length + 1 &&
               fullTypeName.StartsWith(ns.AsSpan(), StringComparison.Ordinal) &&
               fullTypeName[ns.Length] == '.' &&
               fullTypeName.Slice(ns.Length + 1).Equals(name.AsSpan(), StringComparison.Ordinal);
    }

    /// <summary>
    /// Find a method by name within a type definition.
    /// </summary>
    private static MethodDefinitionHandle FindMethodByName(MetadataReader md, TypeDefinitionHandle typeHandle, string methodName)
    {
        var typeDef = md.GetTypeDefinition(typeHandle);
        foreach (var methodHandle in typeDef.GetMethods())
        {
            var methodDef = md.GetMethodDefinition(methodHandle);
            var name = md.GetString(methodDef.Name);
            if (string.Equals(name, methodName, StringComparison.Ordinal))
                return methodHandle;
        }
        return default;
    }

    /// <summary>
    /// Create a static initialization plan by parsing raw IL bytes from a .cctor method body.
    /// Looks for the simple pattern: newobj → stsfld (repeated), terminated by ret.
    /// </summary>
    private StaticInitializationPlan? CreateStaticInitPlanFromIl(
        string typeSubjectId,
        byte[] ilBytes,
        MetadataReader md,
        PEReader pe,
        HashSet<string> reachableMethodSubjectIds,
        HashSet<string> requiredExternalRuntimeHelperSubjectIds)
    {
        if (ilBytes.Length == 0) return null;

        // Skip closure types (<>c, <>c__DisplayClass, etc.) — they are compiler-generated
        // and their static constructors reference closure-internal types. Generating
        // chaos_ensure_type_initialized_* for these would produce C3861 errors when
        // the function is referenced from one page but defined in another.
        if (typeSubjectId.Contains("<>c"))
            return null;

        // Skip all leading nops
        int offset = 0;
        while (offset < ilBytes.Length && ilBytes[offset] == 0x00) // nop
            offset++;

        // If only ret (0x2A) remains, empty cctor
        if (offset >= ilBytes.Length || ilBytes[offset] == 0x2A)
            return null;

        var actions = new List<StaticInitializationAction>();

        // Skip method body header (prolog + init locals flags + max stack + code size)
        // This is a simplified skip: read the header format
        // CLI spec: CorILMethod header has flags, max stack, code size, local var sig token
        // But the body returned by GetMethodBody already strips the header — ilBytes is raw IL.
        // Actually, let's check: MethodBodyBlock.GetILBytes() returns just the IL,
        // not the header. So offset starts at 0 for the actual IL.

        while (offset < ilBytes.Length)
        {
            // Skip nops
            while (offset < ilBytes.Length && ilBytes[offset] == 0x00)
                offset++;

            if (offset >= ilBytes.Length) break;

            byte opcode = ilBytes[offset];

            // ret — end of cctor
            if (opcode == 0x2A)
            {
                // Must be the last instruction
                offset++;
                // Skip trailing nops
                while (offset < ilBytes.Length && ilBytes[offset] == 0x00)
                    offset++;
                break;
            }

            // For generic type cctors, skip unsupported opcodes
            if (typeSubjectId.Contains("!!") || typeSubjectId.Contains("`") || typeSubjectId.Contains("<!"))
                return null;

            // newobj (0x73) + 4-byte metadata token
            if (opcode != 0x73 || offset + 5 > ilBytes.Length)
                return null;

            uint ctorToken = ReadUInt32LE(ilBytes, offset + 1);
            string ctorSubjectId = ResolveMethodToken(md, ctorToken);

            if (string.IsNullOrEmpty(ctorSubjectId))
                return null;

            offset += 5;

            // Skip nops between newobj and stsfld
            while (offset < ilBytes.Length && ilBytes[offset] == 0x00)
                offset++;

            // stsfld (0x80) + 4-byte metadata token
            if (offset + 5 > ilBytes.Length || ilBytes[offset] != 0x80)
                return null;

            uint fieldToken = ReadUInt32LE(ilBytes, offset + 1);
            string fieldSubjectId = ResolveFieldToken(md, pe, fieldToken);
            offset += 5;

            if (string.IsNullOrEmpty(fieldSubjectId))
                return null;

            // Check field belongs to this type (validate declaring type)
            var fieldDeclType = GetFieldDeclaringTypeSubjectId(md, pe, fieldToken);
            if (!string.IsNullOrEmpty(fieldDeclType) &&
                !IsFieldDeclaringTypeMatch(fieldDeclType, typeSubjectId))
                return null;

            actions.Add(new StaticInitializationAction(
                fieldSubjectId,
                GetMethodDeclaringTypeFromToken(md, ctorToken),
                ctorSubjectId,
                ElideConstructorCall: false));
        }

        if (actions.Count == 0) return null;

        // Normalize actions: check each constructor for elision or external runtime helper
        var normalizedActions = new List<StaticInitializationAction>(actions.Count);
        var skipPlan = false;

        foreach (var action in actions)
        {
            // Skip malformed SubjectIds (closures, templates, etc.)
            // Do NOT add to normalizedActions — these actions should not generate
            // chaos_ensure_type_initialized_* function definitions.
            if (string.IsNullOrEmpty(action.ConstructorSubjectId) ||
                action.ConstructorSubjectId.Contains("<>c__DisplayClass") ||
                action.ConstructorSubjectId.Contains("<>9__") ||
                action.ConstructorSubjectId.IndexOf("::", StringComparison.Ordinal) <= 0)
            {
                requiredExternalRuntimeHelperSubjectIds.Add(action.ConstructorSubjectId);
                continue;  // Skip action entirely — no plan entry
            }
            if (TryCreateExternalRuntimeHelperDefinition(action.ConstructorSubjectId, out _))
            {
                requiredExternalRuntimeHelperSubjectIds.Add(action.ConstructorSubjectId);
                normalizedActions.Add(action);
                continue;
            }

            if (reachableMethodSubjectIds.Contains(action.ConstructorSubjectId))
            {
                normalizedActions.Add(action);
                continue;
            }

            if (CanElideCtorFromPe(md, pe, action.ConstructorSubjectId))
            {
                normalizedActions.Add(action with { ElideConstructorCall = true });
                continue;
            }

            // Constructor not in reachable set and cannot be elided — skip plan
            skipPlan = true;
            break;
        }

        if (skipPlan || normalizedActions.Count == 0)
            return null;

        return new StaticInitializationPlan(typeSubjectId, normalizedActions);
    }

    /// <summary>
    /// Check if a constructor call can be elided (empty body or just ldarg.0 + call Object::.ctor + ret).
    /// Reads constructor IL directly from PE metadata.
    /// </summary>
    private bool CanElideCtorFromPe(MetadataReader md, PEReader pe, string ctorSubjectId)
    {
        var (asmName, typeName, methodName) = ParseMethodSubjectId(ctorSubjectId);
        if (asmName == null || !_closureAssemblyPathByName.TryGetValue(asmName, out var asmPath))
            return false;

        try
        {
            using var ctorPe = new PEReader(File.OpenRead(asmPath));
            var ctorMd = ctorPe.GetMetadataReader();

            var slashIdx = ctorSubjectId.IndexOf('/');
            var fullTypeName = ctorSubjectId.AsSpan(slashIdx + 1);
            var doubleColon = fullTypeName.IndexOf("::");
            if (doubleColon < 0) return false;

            var typeNameOnly = fullTypeName.Slice(0, doubleColon);
            var typeHandle = FindTypeDefinitionByFullName(ctorMd, typeNameOnly);
            if (typeHandle.IsNil) return false;

            var methodHandle = FindMethodByName(ctorMd, typeHandle, methodName ?? ".ctor");
            if (methodHandle.IsNil) return false;

            var methodDef = ctorMd.GetMethodDefinition(methodHandle);

            // If it's static or has parameters, can't elide
            if ((methodDef.Attributes & MethodAttributes.Static) != 0)
                return false;

            // Read IL body
            var body = ctorPe.GetMethodBody(methodDef.RelativeVirtualAddress);
            if (body == null) return false;

            var il = body.GetILBytes();
            if (il == null || il.Length == 0) return false;

            int offset = 0;
            while (offset < il.Length && il[offset] == 0x00) offset++; // skip nops

            // Just "ret" — empty constructor
            if (offset < il.Length && il[offset] == 0x2A)
                return true;

            // "ldarg.0 (0x02) [or ldarg.s 0 (0x0E 0x00)] + call (0x28) System.Object::.ctor + ret (0x2A)"
            if (offset + 1 >= il.Length) return false;

            // ldarg.0 or ldarg.s 0
            if (il[offset] == 0x02)
                offset += 1;
            else if (il[offset] == 0x0E && il[offset + 1] == 0x00)
                offset += 2;
            else
                return false;

            while (offset < il.Length && il[offset] == 0x00) offset++; // skip nops

            // call (0x28) + 4-byte token
            if (offset + 5 > il.Length || il[offset] != 0x28) return false;
            uint targetToken = ReadUInt32LE(il, offset + 1);
            string targetSubjectId = ResolveMethodToken(ctorMd, targetToken);
            offset += 5;
            if (!string.Equals(targetSubjectId, ObjectCtorMethodSubjectId, StringComparison.Ordinal))
                return false;

            while (offset < il.Length && il[offset] == 0x00) offset++; // skip nops

            // ret
            if (offset >= il.Length || il[offset] != 0x2A) return false;

            return true;
        }
        catch
        {
            return false;
        }
    }

    /// <summary>
    /// Resolve a method metadata token (e.g. 0x06000001) to its subject ID.
    /// </summary>
    private static string ResolveMethodToken(MetadataReader md, uint token)
    {
        try
        {
            var handle = MetadataTokens.Handle((int)token);
            if (handle.Kind != HandleKind.MethodDefinition)
                return string.Empty;

            var methodDef = md.GetMethodDefinition((MethodDefinitionHandle)handle);
            var declTypeHandle = methodDef.GetDeclaringType();
            var declTypeDef = md.GetTypeDefinition(declTypeHandle);
            var typeName = md.GetString(declTypeDef.Name);
            var ns = md.GetString(declTypeDef.Namespace);
            var methodName = md.GetString(methodDef.Name);

            // Build a simple subject ID-compatible string
            string fullTypeName = string.IsNullOrEmpty(ns) ? typeName : $"{ns}.{typeName}";
            return $"{fullTypeName}::{methodName}";
        }
        catch
        {
            return string.Empty;
        }
    }

    /// <summary>
    /// Resolve a field metadata token (e.g. 0x04000001) to its subject ID.
    /// Returns empty string if the field is not static.
    /// </summary>
    private static string ResolveFieldToken(MetadataReader md, PEReader pe, uint token)
    {
        try
        {
            var handle = MetadataTokens.Handle((int)token);
            if (handle.Kind != HandleKind.FieldDefinition)
                return string.Empty;

            var fieldDef = md.GetFieldDefinition((FieldDefinitionHandle)handle);

            // Only handle static fields
            if ((fieldDef.Attributes & FieldAttributes.Static) == 0)
                return string.Empty;

            var declTypeHandle = fieldDef.GetDeclaringType();
            var declTypeDef = md.GetTypeDefinition(declTypeHandle);

            // Build assembly-qualified field subject ID
            // Format: "AssemblyName/FullTypeName.fieldName"
            var assemblyName = GetAssemblyNameFromMetadata(md);
            var typeFullName = GetTypeFullName(md, declTypeDef);
            var fieldName = md.GetString(fieldDef.Name);

            return $"{assemblyName}/{typeFullName}.{fieldName}";
        }
        catch
        {
            return string.Empty;
        }
    }

    /// <summary>
    /// Get the declaring type subject ID for a field from its metadata token.
    /// </summary>
    private static string GetFieldDeclaringTypeSubjectId(MetadataReader md, PEReader pe, uint token)
    {
        try
        {
            var handle = MetadataTokens.Handle((int)token);
            if (handle.Kind != HandleKind.FieldDefinition)
                return string.Empty;

            var fieldDef = md.GetFieldDefinition((FieldDefinitionHandle)handle);
            var declTypeHandle = fieldDef.GetDeclaringType();
            var declTypeDef = md.GetTypeDefinition(declTypeHandle);

            var assemblyName = GetAssemblyNameFromMetadata(md);
            var typeFullName = GetTypeFullName(md, declTypeDef);

            return $"{assemblyName}/{typeFullName}";
        }
        catch
        {
            return string.Empty;
        }
    }

    private static string GetMethodDeclaringTypeFromToken(MetadataReader md, uint token)
    {
        try
        {
            var handle = MetadataTokens.Handle((int)token);
            if (handle.Kind != HandleKind.MethodDefinition)
                return string.Empty;

            var methodDef = md.GetMethodDefinition((MethodDefinitionHandle)handle);
            var declTypeHandle = methodDef.GetDeclaringType();
            var declTypeDef = md.GetTypeDefinition(declTypeHandle);

            var assemblyName = GetAssemblyNameFromMetadata(md);
            var typeFullName = GetTypeFullName(md, declTypeDef);

            return $"{assemblyName}/{typeFullName}";
        }
        catch
        {
            return string.Empty;
        }
    }

    private static string GetAssemblyNameFromMetadata(MetadataReader md)
    {
        var assemblyDef = md.GetAssemblyDefinition();
        return md.GetString(assemblyDef.Name);
    }

    private static string GetTypeFullName(MetadataReader md, TypeDefinition typeDef)
    {
        var name = md.GetString(typeDef.Name);
        var ns = md.GetString(typeDef.Namespace);

        // Nested types
        var declaringHandle = typeDef.GetDeclaringType();
        if (!declaringHandle.IsNil)
        {
            var outerDef = md.GetTypeDefinition(declaringHandle);
            var outerFullName = GetTypeFullName(md, outerDef);
            return $"{outerFullName}+{name}";
        }

        return string.IsNullOrEmpty(ns) ? name : $"{ns}.{name}";
    }

    /// <summary>
    /// Check if the field's declaring type matches or is compatible with the cctor's type.
    /// </summary>
    private static bool IsFieldDeclaringTypeMatch(string fieldDeclType, string cctorType)
    {
        if (string.IsNullOrEmpty(fieldDeclType) || string.IsNullOrEmpty(cctorType))
            return false;

        var normFieldType = NormalizeGenericTypeId(fieldDeclType);
        var normCctorType = NormalizeGenericTypeId(cctorType);

        return string.Equals(normFieldType, normCctorType, StringComparison.Ordinal) ||
               string.Equals(fieldDeclType, cctorType, StringComparison.Ordinal) ||
               (fieldDeclType.StartsWith(cctorType, StringComparison.Ordinal) &&
                fieldDeclType.Length > cctorType.Length &&
                fieldDeclType[cctorType.Length] == '<');
    }

    /// <summary>
    /// Parse a method subject ID into (assemblyName, typeName, methodName).
    /// Format: "AssemblyName/Namespace.TypeName::MethodName:ReturnType(Params)"
    /// </summary>
    private static (string? AssemblyName, string? TypeName, string? MethodName) ParseMethodSubjectId(string subjectId)
    {
        var slashIdx = subjectId.IndexOf('/');
        if (slashIdx < 0) return (null, null, null);

        var assemblyName = subjectId.Substring(0, slashIdx);
        var rest = subjectId.AsSpan(slashIdx + 1);

        var doubleColon = rest.IndexOf("::");
        if (doubleColon < 0) return (assemblyName, null, null);

        var typeName = rest.Slice(0, doubleColon).ToString();

        // Method name is before the colon after ::
        var methodPart = rest.Slice(doubleColon + 2);
        var methodColon = methodPart.IndexOf(':');
        var methodName = methodColon >= 0 ? methodPart.Slice(0, methodColon).ToString() : methodPart.ToString();

        return (assemblyName, typeName, methodName);
    }

    private static uint ReadUInt32LE(byte[] bytes, int offset)
    {
        return (uint)(bytes[offset] | (bytes[offset + 1] << 8) | (bytes[offset + 2] << 16) | (bytes[offset + 3] << 24));
    }

    private static string NormalizeGenericTypeId(string? typeId)
    {
        if (string.IsNullOrEmpty(typeId))
        {
            return string.Empty;
        }

        var result = typeId;
        for (var arity = 9; arity >= 1; arity--)
        {
            var marker = $"`{arity}";
            if (!result.Contains(marker, StringComparison.Ordinal))
            {
                continue;
            }

            var genericArgs = string.Join(
                ",",
                Enumerable.Range(0, arity).Select(i => $"!{i}"));
            result = result.Replace(marker, $"<{genericArgs}>");
        }

        return result;
    }

    private bool TryGetStaticInitializationPlanForType(
        string typeSubjectId,
        out StaticInitializationPlan? plan)
    {
        return _staticInitializationSupport.PlansByTypeSubjectId.TryGetValue(typeSubjectId, out plan);
    }

    private bool TryGetStaticInitializationPlanForField(
        string fieldSubjectId,
        out StaticInitializationPlan? plan)
    {
        plan = null;
        return _staticInitializationSupport.DeclaringTypeByStaticFieldSubjectId.TryGetValue(fieldSubjectId, out var typeSubjectId) &&
               _staticInitializationSupport.PlansByTypeSubjectId.TryGetValue(typeSubjectId, out plan);
    }

    // Note: This is only available via the Loader which we're avoiding.
    // The PE metadata approach checks elision via CanElideCtorFromPe() directly
    // without needing the full managed method model.
}
