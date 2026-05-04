using System.Text;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.CodeGen;

public sealed partial class NativeReferenceProofEmitter
{
    private static NativeReferenceLoweringPlanArtifact BuildLegacyLoweringPlan(
        string assemblyName,
        string entryPointSubjectId,
        IReadOnlyList<TypedIlMethodArtifact> methods,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers)
    {
        var entryPointRegistration = GetRequiredRegistration(methodPointers, entryPointSubjectId);
        var entryPointMethod = GetRequiredMethod(methods, entryPointSubjectId);
        RequireMethodContract(entryPointMethod, "static-method", "has-canonical-body");
        var entryPointInstructions = GetSingleBlockInstructions(entryPointMethod);

        if (TryBuildLegacyPInvokeDirectCallLoweringPlan(
                assemblyName,
                entryPointSubjectId,
                metadataRegistration,
                methodPointers,
                methods,
                entryPointRegistration,
                entryPointMethod,
                entryPointInstructions,
                out var pinvokeDllImportMinimalPlan))
        {
            return pinvokeDllImportMinimalPlan!;
        }

        if (TryBuildLegacyReflectionClosedTypeQueryLoweringPlan(
                assemblyName,
                entryPointSubjectId,
                metadataRegistration,
                entryPointRegistration,
                out var reflectionQueryMinimalPlan))
        {
            return reflectionQueryMinimalPlan!;
        }

        if (MatchesLegacyStaticForwarderCtorGetterEntryPointShape(entryPointInstructions))
        {
            return CreateLegacyStaticForwarderCapturedGetterLoweringPlan(
                assemblyName,
                entryPointSubjectId,
                metadataRegistration,
                methodPointers,
                methods,
                entryPointRegistration,
                entryPointMethod,
                entryPointInstructions);
        }

        return CreateLegacyCapturedStateInstanceMessageLoweringPlan(
            assemblyName,
            entryPointSubjectId,
            metadataRegistration,
            methodPointers,
            methods,
            entryPointRegistration,
            entryPointMethod,
            entryPointInstructions);
    }

    private static NativeReferenceLoweringPlanArtifact CreateLegacyCapturedStateInstanceMessageLoweringPlan(
        string assemblyName,
        string entryPointSubjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        IReadOnlyList<TypedIlMethodArtifact> methods,
        CodeRegistrationEntry entryPointRegistration,
        TypedIlMethodArtifact entryPointMethod,
        IReadOnlyList<TypedIlInstructionArtifact> entryPointInstructions)
    {
        ValidateLegacyCapturedStateInstanceMessageEntryPointShape(entryPointMethod, entryPointInstructions);

        var constructorSubjectId = GetRequiredInstructionCallee(entryPointInstructions[1], entryPointMethod.SubjectId, 1);
        var instanceMethodSubjectId = GetRequiredInstructionCallee(entryPointInstructions[2], entryPointMethod.SubjectId, 2);
        var writeLineStringIcall = GetRequiredInstructionCallee(entryPointInstructions[3], entryPointMethod.SubjectId, 3);

        var constructorRegistration = GetRequiredRegistration(methodPointers, constructorSubjectId);
        var instanceMethodRegistration = GetRequiredRegistration(methodPointers, instanceMethodSubjectId);
        var constructorMethod = GetRequiredMethod(methods, constructorSubjectId);
        var instanceMethod = GetRequiredMethod(methods, instanceMethodSubjectId);

        var constructorInstructions = GetSingleBlockInstructions(constructorMethod);
        var instanceMethodInstructions = GetSingleBlockInstructions(instanceMethod);

        ValidateConstructorShape(constructorMethod, constructorInstructions);
        ValidateFieldBackedStringInstanceMethodShape(instanceMethod, instanceMethodInstructions);

        var constructorTypeSubjectId = GetDeclaringTypeSubjectId(constructorRegistration.SubjectId);
        var instanceMethodTypeSubjectId = GetDeclaringTypeSubjectId(instanceMethodRegistration.SubjectId);
        if (!string.Equals(constructorTypeSubjectId, instanceMethodTypeSubjectId, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects constructor '{constructorRegistration.SubjectId}' and instance method '{instanceMethodRegistration.SubjectId}' to share a declaring type");
        }

        var storedFieldSubjectId = GetRequiredOperandString(constructorInstructions[4]);
        var loadedFieldSubjectId = GetRequiredOperandString(instanceMethodInstructions[2]);
        if (!string.Equals(storedFieldSubjectId, loadedFieldSubjectId, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-reference emitter field access mismatch: constructor writes '{storedFieldSubjectId}', instance method reads '{loadedFieldSubjectId}'");
        }

        var constructorStringLiteral = GetRequiredOperandString(entryPointInstructions[0]);
        var messagePrefixLiteral = GetRequiredOperandString(instanceMethodInstructions[0]);
        var messageSuffixLiteral = GetCapturedStateInstanceMessageSuffixLiteral(instanceMethod, instanceMethodInstructions);
        var concatPairIcall = GetCapturedStateInstanceMessageConcatIcall(instanceMethod, instanceMethodInstructions);

        return new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = "constructorThenInstanceCall",
            AssemblyName = assemblyName,
            EntrySubjectId = entryPointSubjectId,
            IncludeHeader = "codegen_bridge.h",
            NativeEntryFunctionName = "RunNativeReference",
            EntrySymbol = entryPointRegistration.Symbol,
            ConstructorSymbol = constructorRegistration.Symbol,
            InstanceMethodSymbol = instanceMethodRegistration.Symbol,
            ReferenceTypeToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "type", constructorTypeSubjectId)),
            CapturedFieldToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "field", storedFieldSubjectId)),
            EntryMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", entryPointRegistration.SubjectId)),
            ConstructorMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", constructorRegistration.SubjectId)),
            InstanceMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", instanceMethodRegistration.SubjectId)),
            ConsoleWriteLineStringIcall = writeLineStringIcall,
            StringConcatPairIcall = concatPairIcall,
            ConstructorLiteral = ToCppStringLiteral(constructorStringLiteral),
            ConstructorLiteralByteCount = Encoding.UTF8.GetByteCount(constructorStringLiteral),
            MessagePrefixLiteral = ToCppStringLiteral(messagePrefixLiteral),
            MessagePrefixLiteralByteCount = Encoding.UTF8.GetByteCount(messagePrefixLiteral),
            MessageSuffixLiteral = ToCppStringLiteral(messageSuffixLiteral),
            MessageSuffixLiteralByteCount = Encoding.UTF8.GetByteCount(messageSuffixLiteral),
        };
    }

    private static NativeReferenceLoweringPlanArtifact CreateLegacyStaticForwarderCapturedGetterLoweringPlan(
        string assemblyName,
        string entryPointSubjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        IReadOnlyList<TypedIlMethodArtifact> methods,
        CodeRegistrationEntry entryPointRegistration,
        TypedIlMethodArtifact entryPointMethod,
        IReadOnlyList<TypedIlInstructionArtifact> entryPointInstructions)
    {
        ValidateStaticCallCtorGetterEntryPointShape(entryPointMethod, entryPointInstructions);

        var echoMethodSubjectId = GetRequiredInstructionCallee(entryPointInstructions[1], entryPointMethod.SubjectId, 1);
        var constructorSubjectId = GetRequiredInstructionCallee(entryPointInstructions[2], entryPointMethod.SubjectId, 2);
        var getterSubjectId = GetRequiredInstructionCallee(entryPointInstructions[3], entryPointMethod.SubjectId, 3);
        var writeLineStringIcall = GetRequiredInstructionCallee(entryPointInstructions[4], entryPointMethod.SubjectId, 4);

        var echoMethodRegistration = GetRequiredRegistration(methodPointers, echoMethodSubjectId);
        var constructorRegistration = GetRequiredRegistration(methodPointers, constructorSubjectId);
        var getterRegistration = GetRequiredRegistration(methodPointers, getterSubjectId);
        var echoMethod = GetRequiredMethod(methods, echoMethodSubjectId);
        var constructorMethod = GetRequiredMethod(methods, constructorSubjectId);
        var getterMethod = GetRequiredMethod(methods, getterSubjectId);

        var echoInstructions = GetSingleBlockInstructions(echoMethod);
        var constructorInstructions = GetSingleBlockInstructions(constructorMethod);
        var getterInstructions = GetSingleBlockInstructions(getterMethod);

        ValidateSingleArgumentForwarderShape(echoMethod, echoInstructions);
        ValidateConstructorShape(constructorMethod, constructorInstructions);
        ValidateFieldGetterShape(getterMethod, getterInstructions);

        var constructorTypeSubjectId = GetDeclaringTypeSubjectId(constructorRegistration.SubjectId);
        var getterTypeSubjectId = GetDeclaringTypeSubjectId(getterRegistration.SubjectId);
        if (!string.Equals(constructorTypeSubjectId, getterTypeSubjectId, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects constructor '{constructorRegistration.SubjectId}' and getter '{getterRegistration.SubjectId}' to share a declaring type");
        }

        var storedFieldSubjectId = GetRequiredOperandString(constructorInstructions[4]);
        var loadedFieldSubjectId = GetRequiredOperandString(getterInstructions[1]);
        if (!string.Equals(storedFieldSubjectId, loadedFieldSubjectId, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-reference emitter field access mismatch: constructor writes '{storedFieldSubjectId}', getter reads '{loadedFieldSubjectId}'");
        }

        var echoLiteral = GetRequiredOperandString(entryPointInstructions[0]);

        return new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = "staticCallCtorGetter",
            AssemblyName = assemblyName,
            EntrySubjectId = entryPointSubjectId,
            IncludeHeader = "codegen_bridge.h",
            NativeEntryFunctionName = "RunNativeReference",
            EntrySymbol = entryPointRegistration.Symbol,
            EchoMethodSymbol = echoMethodRegistration.Symbol,
            ConstructorSymbol = constructorRegistration.Symbol,
            GetterSymbol = getterRegistration.Symbol,
            ReferenceTypeToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "type", constructorTypeSubjectId)),
            CapturedFieldToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "field", storedFieldSubjectId)),
            EntryMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", entryPointRegistration.SubjectId)),
            EchoMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", echoMethodRegistration.SubjectId)),
            ConstructorMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", constructorRegistration.SubjectId)),
            GetterMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", getterRegistration.SubjectId)),
            ConsoleWriteLineStringIcall = writeLineStringIcall,
            EchoLiteral = ToCppStringLiteral(echoLiteral),
            EchoLiteralByteCount = Encoding.UTF8.GetByteCount(echoLiteral),
        };
    }

    private static bool TryBuildLegacyPInvokeDirectCallLoweringPlan(
        string assemblyName,
        string entryPointSubjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        IReadOnlyList<TypedIlMethodArtifact> methods,
        CodeRegistrationEntry entryPointRegistration,
        TypedIlMethodArtifact entryPointMethod,
        IReadOnlyList<TypedIlInstructionArtifact> entryPointInstructions,
        out NativeReferenceLoweringPlanArtifact? loweringPlan)
    {
        loweringPlan = null;

        if (!IsPInvokeDllImportMinimalEntryPointShape(entryPointInstructions))
        {
            return false;
        }

        var importMethodSubjectId = GetRequiredInstructionCallee(entryPointInstructions[3], entryPointMethod.SubjectId, 3);
        var importMethodRegistration = GetRequiredRegistration(methodPointers, importMethodSubjectId);
        var importMethod = GetRequiredMethod(methods, importMethodSubjectId);
        if (!HasCapability(entryPointMethod, "requires-imported-call") ||
            !HasCapability(importMethod, "requires-imported-call") ||
            !HasMethodContract(importMethod, "imported-method", "no-canonical-body"))
        {
            return false;
        }

        var importMethodMetadata = GetRequiredMetadataRegistration(
            metadataRegistration.Registrations,
            "method",
            importMethodSubjectId,
            "pinvoke import method");

        if (importMethodMetadata.IsImported != true)
        {
            return false;
        }

        if (string.IsNullOrWhiteSpace(importMethodMetadata.ImportModuleName) ||
            string.IsNullOrWhiteSpace(importMethodMetadata.ImportEntryPointName))
        {
            return false;
        }

        loweringPlan = new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = NativeReferenceProofCatalog.Stringify(NativeReferencePlanKind.InteropPInvokeDirectCallMinimal),
            AssemblyName = assemblyName,
            EntrySubjectId = entryPointSubjectId,
            IncludeHeader = "codegen_bridge.h",
            NativeEntryFunctionName = "RunNativeReference",
            EntrySymbol = entryPointRegistration.Symbol,
            ReferenceTypeToken = "0u",
            CapturedFieldToken = "0u",
            EntryMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", entryPointRegistration.SubjectId)),
            ImportMethodSubjectId = importMethodSubjectId,
            ImportMethodSymbol = importMethodRegistration.Symbol,
            ImportModuleName = importMethodMetadata.ImportModuleName,
            ImportEntryPointName = importMethodMetadata.ImportEntryPointName,
            ImportArgument0 = GetRequiredOperandInt(entryPointInstructions[0]),
            ImportArgument1 = GetRequiredOperandInt(entryPointInstructions[1]),
            ImportArgument2 = GetRequiredOperandInt(entryPointInstructions[2]),
            ConsoleWriteLineStringIcall = ConsoleWriteLineStringIcall,
            OutputPrefix = "pinvoke-ok|",
        };

        return true;
    }

    private static bool TryBuildLegacyReflectionClosedTypeQueryLoweringPlan(
        string assemblyName,
        string entryPointSubjectId,
        MetadataRegistrationArtifact metadataRegistration,
        CodeRegistrationEntry entryPointRegistration,
        out NativeReferenceLoweringPlanArtifact? loweringPlan)
    {
        loweringPlan = null;

        var registrations = metadataRegistration.Registrations;
        var closedTypeRegistration = GetMetadataRegistrationsForKind(registrations, "type").FirstOrDefault(registration =>
            !string.IsNullOrWhiteSpace(registration.DefinitionSubjectId) &&
            !string.Equals(registration.SubjectId, registration.DefinitionSubjectId, StringComparison.Ordinal));
        if (closedTypeRegistration is null)
        {
            return false;
        }

        var propertyRegistration = GetMetadataRegistrationsForKind(registrations, "property").FirstOrDefault(registration =>
            string.Equals(registration.DeclaringTypeSubjectId, closedTypeRegistration.SubjectId, StringComparison.Ordinal) &&
            !IsCompilerGeneratedMemberName(registration.Name));
        var queryMethodRegistration = GetMetadataRegistrationsForKind(registrations, "method").FirstOrDefault(registration =>
            string.Equals(registration.DeclaringTypeSubjectId, closedTypeRegistration.SubjectId, StringComparison.Ordinal) &&
            (registration.ParameterCount ?? 0) > 0 &&
            !IsSpecialMethodName(registration.Name));
        if (propertyRegistration is null || queryMethodRegistration is null)
        {
            return false;
        }

        var genericTypeDefinitionRegistration = GetRequiredMetadataRegistration(
            registrations,
            "type",
            closedTypeRegistration.DefinitionSubjectId!,
            "reflection query generic type definition");
        var fieldRegistration = GetRequiredMetadataRegistration(
            registrations,
            "field",
            registration =>
                string.Equals(registration.DeclaringTypeSubjectId, closedTypeRegistration.SubjectId, StringComparison.Ordinal) &&
                !IsCompilerGeneratedMemberName(registration.Name),
            $"reflection query field on '{closedTypeRegistration.SubjectId}'");
        var parameterRegistration = GetRequiredMetadataRegistration(
            registrations,
            "parameter",
            registration =>
                string.Equals(registration.DeclaringMethodSubjectId, queryMethodRegistration.SubjectId, StringComparison.Ordinal) &&
                registration.ParameterIndex == 0,
            $"reflection query first parameter for '{queryMethodRegistration.SubjectId}'");

        var closedTypeDisplayName = GetRequiredRegistrationDisplayName(closedTypeRegistration);
        var genericTypeDefinitionName = GetRequiredRegistrationName(genericTypeDefinitionRegistration);
        var fieldQueryName = GetRequiredRegistrationName(fieldRegistration);
        var propertyQueryName = GetRequiredRegistrationName(propertyRegistration);
        var methodQueryName = GetRequiredRegistrationName(queryMethodRegistration);
        var parameterName = GetRequiredRegistrationName(parameterRegistration);
        var methodParameterCount = queryMethodRegistration.ParameterCount
            ?? throw new InvalidOperationException(
                $"reflection query method '{queryMethodRegistration.SubjectId}' is missing parameterCount metadata");
        var parameterIndex = parameterRegistration.ParameterIndex
            ?? throw new InvalidOperationException(
                $"reflection query parameter '{parameterRegistration.SubjectId}' is missing parameterIndex metadata");
        var expectedOutput =
            $"reflection-ok|{closedTypeDisplayName}|{fieldQueryName}|{propertyQueryName}|{methodQueryName}|{parameterName}|{genericTypeDefinitionName}";

        loweringPlan = new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = "reflectionQueryMinimal",
            AssemblyName = assemblyName,
            EntrySubjectId = entryPointSubjectId,
            IncludeHeader = "reflection_query_model.h",
            NativeEntryFunctionName = "RunNativeReference",
            EntrySymbol = entryPointRegistration.Symbol,
            ReferenceTypeToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "type", closedTypeRegistration.SubjectId)),
            CapturedFieldToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "field", fieldRegistration.SubjectId)),
            EntryMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", entryPointRegistration.SubjectId)),
            InstanceMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", queryMethodRegistration.SubjectId)),
            ConsoleWriteLineStringIcall = ConsoleWriteLineStringIcall,
            ClosedTypeSubjectId = closedTypeRegistration.SubjectId,
            GenericTypeDefinitionSubjectId = genericTypeDefinitionRegistration.SubjectId,
            FieldSubjectId = fieldRegistration.SubjectId,
            PropertySubjectId = propertyRegistration.SubjectId,
            MethodSubjectId = queryMethodRegistration.SubjectId,
            ParameterSubjectId = parameterRegistration.SubjectId,
            ClosedTypeToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "type", closedTypeRegistration.SubjectId)),
            GenericTypeDefinitionToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "type", genericTypeDefinitionRegistration.SubjectId)),
            ClosedTypeNamespaceName = closedTypeRegistration.NamespaceName ?? string.Empty,
            ClosedTypeName = GetRequiredRegistrationName(closedTypeRegistration),
            ClosedTypeDisplayName = closedTypeDisplayName,
            GenericTypeDefinitionName = genericTypeDefinitionName,
            FieldQueryName = fieldQueryName,
            PropertyQueryName = propertyQueryName,
            MethodQueryName = methodQueryName,
            MethodParameterCount = methodParameterCount,
            ParameterIndex = parameterIndex,
            ExpectedOutput = expectedOutput,
            ExpectedOutputByteCount = Encoding.UTF8.GetByteCount(expectedOutput),
        };

        return true;
    }
}
