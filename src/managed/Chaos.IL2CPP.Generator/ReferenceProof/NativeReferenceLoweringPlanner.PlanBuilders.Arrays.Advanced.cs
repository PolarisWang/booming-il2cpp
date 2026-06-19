using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.Json;
using Chaos.IL2CPP.Contracts;
using Scriban;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeReferenceLoweringPlanner
{



    private static bool TryCreateNestedExceptionLoweringPlan(
        string assemblyName,
        string entryPointSubjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        CodeRegistrationEntry entryPointRegistration,
        out NativeReferenceLoweringPlanArtifact? loweringPlan)
    {
        loweringPlan = null;

        if (!entryPointSubjectId.Contains("NestedExceptionProofEntry::Run", StringComparison.Ordinal))
        {
            return false;
        }

        var registrations = metadataRegistration.Registrations;
        var harnessTypeRegistration = GetRequiredMetadataRegistration(
            registrations,
            "type",
            registration => string.Equals(registration.Name, "NestedExceptionHarness", StringComparison.Ordinal),
            "nested exception harness type");
        var constructorMetadata = GetRequiredMetadataRegistration(
            registrations,
            "method",
            registration =>
                string.Equals(registration.DeclaringTypeSubjectId, harnessTypeRegistration.SubjectId, StringComparison.Ordinal) &&
                string.Equals(registration.Name, ".ctor", StringComparison.Ordinal) &&
                registration.ParameterCount == 0,
            "nested exception constructor");
        var captureMetadata = GetRequiredMetadataRegistration(
            registrations,
            "method",
            registration =>
                string.Equals(registration.DeclaringTypeSubjectId, harnessTypeRegistration.SubjectId, StringComparison.Ordinal) &&
                string.Equals(registration.Name, "Capture", StringComparison.Ordinal) &&
                registration.ParameterCount == 0,
            "nested exception capture method");
        var throwMetadata = GetRequiredMetadataRegistration(
            registrations,
            "method",
            registration =>
                string.Equals(registration.DeclaringTypeSubjectId, harnessTypeRegistration.SubjectId, StringComparison.Ordinal) &&
                string.Equals(registration.Name, "ThrowInner", StringComparison.Ordinal) &&
                registration.ParameterCount == 0,
            "nested exception throw method");

        var constructorRegistration = GetRequiredRegistration(methodPointers, constructorMetadata.SubjectId);
        var captureRegistration = GetRequiredRegistration(methodPointers, captureMetadata.SubjectId);
        var throwRegistration = GetRequiredRegistration(methodPointers, throwMetadata.SubjectId);

        const string expectedOutput = "Nested EH native proof: inner caught.";
        const string outerFinallyLiteral = "Nested EH outer finally.";

        loweringPlan = new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = "nestedExceptionThrowCatchFinallyMinimal",
            AssemblyName = assemblyName,
            EntrySubjectId = entryPointSubjectId,
            IncludeHeader = "codegen_bridge.h",
            NativeEntryFunctionName = "RunNativeReference",
            EntrySymbol = entryPointRegistration.Symbol,
            ConstructorSymbol = constructorRegistration.Symbol,
            InstanceMethodSymbol = captureRegistration.Symbol,
            ThrowMethodSymbol = throwRegistration.Symbol,
            ReferenceTypeToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "type", harnessTypeRegistration.SubjectId)),
            CapturedFieldToken = "0u",
            EntryMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", entryPointRegistration.SubjectId)),
            ConstructorMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", constructorMetadata.SubjectId)),
            InstanceMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", captureMetadata.SubjectId)),
            ThrowMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", throwMetadata.SubjectId)),
            ConsoleWriteLineStringIcall = ConsoleWriteLineStringIcall,
            FinallyLiteral = ToCppStringLiteral(outerFinallyLiteral),
            FinallyLiteralByteCount = Encoding.UTF8.GetByteCount(outerFinallyLiteral),
            ExpectedOutput = expectedOutput,
            ExpectedOutputByteCount = Encoding.UTF8.GetByteCount(expectedOutput),
        };

        return true;
    }





    private static bool TryCreateReflectionInteropClosureLoweringPlan(
        string assemblyName,
        string entryPointSubjectId,
        MetadataRegistrationArtifact metadataRegistration,
        CodeRegistrationEntry entryPointRegistration,
        out NativeReferenceLoweringPlanArtifact? loweringPlan)
    {
        loweringPlan = null;

        if (!entryPointSubjectId.Contains("ReflectionInteropClosureEntry::Run", StringComparison.Ordinal))
        {
            return false;
        }

        var registrations = metadataRegistration.Registrations;
        var closedTypeRegistration = GetRequiredMetadataRegistration(
            registrations,
            "type",
            registration =>
                !string.IsNullOrWhiteSpace(registration.DefinitionSubjectId) &&
                !string.Equals(registration.SubjectId, registration.DefinitionSubjectId, StringComparison.Ordinal) &&
                registration.SubjectId.Contains("ReflectionClosureBox", StringComparison.Ordinal) &&
                registration.SubjectId.Contains("System.String", StringComparison.Ordinal),
            "reflection interop closed type");
        var genericTypeDefinitionRegistration = GetRequiredMetadataRegistration(
            registrations,
            "type",
            closedTypeRegistration.DefinitionSubjectId!,
            "reflection interop generic type definition");
        var fieldRegistration = GetRequiredMetadataRegistration(
            registrations,
            "field",
            registration =>
                string.Equals(registration.DeclaringTypeSubjectId, closedTypeRegistration.SubjectId, StringComparison.Ordinal) &&
                string.Equals(registration.Name, "Value", StringComparison.Ordinal),
            "reflection interop field");
        var methodRegistration = GetRequiredMetadataRegistration(
            registrations,
            "method",
            registration =>
                string.Equals(registration.DeclaringTypeSubjectId, closedTypeRegistration.SubjectId, StringComparison.Ordinal) &&
                string.Equals(registration.Name, "Echo", StringComparison.Ordinal) &&
                registration.ParameterCount == 1,
            "reflection interop method");
        var parameterRegistration = GetRequiredMetadataRegistration(
            registrations,
            "parameter",
            registration =>
                string.Equals(registration.DeclaringMethodSubjectId, methodRegistration.SubjectId, StringComparison.Ordinal) &&
                registration.ParameterIndex == 0,
            "reflection interop parameter");
        var importMethodRegistration = GetRequiredMetadataRegistration(
            registrations,
            "method",
            registration =>
                registration.IsImported == true &&
                string.Equals(registration.Name, "GetTickCount64", StringComparison.Ordinal),
            "reflection interop imported method");

        const string expectedOutput = "closure-ok|ReflectionClosureBox<String>|Value|Echo|GetTickCount64";

        loweringPlan = new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = "reflectionInteropClosureMinimal",
            AssemblyName = assemblyName,
            EntrySubjectId = entryPointSubjectId,
            IncludeHeader = "reflection_query_model.h",
            NativeEntryFunctionName = "RunNativeReference",
            EntrySymbol = entryPointRegistration.Symbol,
            ReferenceTypeToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "type", closedTypeRegistration.SubjectId)),
            CapturedFieldToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "field", fieldRegistration.SubjectId)),
            EntryMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", entryPointRegistration.SubjectId)),
            InstanceMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", methodRegistration.SubjectId)),
            ConsoleWriteLineStringIcall = ConsoleWriteLineStringIcall,
            ImportMethodSubjectId = importMethodRegistration.SubjectId,
            ImportModuleName = importMethodRegistration.ImportModuleName,
            ImportEntryPointName = importMethodRegistration.ImportEntryPointName,
            ClosedTypeSubjectId = closedTypeRegistration.SubjectId,
            GenericTypeDefinitionSubjectId = genericTypeDefinitionRegistration.SubjectId,
            FieldSubjectId = fieldRegistration.SubjectId,
            MethodSubjectId = methodRegistration.SubjectId,
            ParameterSubjectId = parameterRegistration.SubjectId,
            ParameterAttributes = parameterRegistration.ParameterAttributes,
            ClosedTypeToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "type", closedTypeRegistration.SubjectId)),
            GenericTypeDefinitionToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "type", genericTypeDefinitionRegistration.SubjectId)),
            ClosedTypeNamespaceName = closedTypeRegistration.NamespaceName ?? string.Empty,
            ClosedTypeName = GetRequiredRegistrationName(closedTypeRegistration),
            ClosedTypeDisplayName = "ReflectionClosureBox<String>",
            GenericTypeDefinitionName = "ReflectionClosureBox<T>",
            FieldQueryName = "Value",
            MethodQueryName = "Echo",
            MethodParameterCount = 1,
            ParameterIndex = 0,
            ExpectedOutput = expectedOutput,
            ExpectedOutputByteCount = Encoding.UTF8.GetByteCount(expectedOutput),
        };

        return true;
    }





    private static bool TryCreatePInvokeDllImportMinimalLoweringPlan(
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
        RequireCapability(entryPointMethod, "requires-imported-call");
        RequireCapability(importMethod, "requires-imported-call");
        RequireMethodContract(importMethod, "imported-method", "no-canonical-body");
        var importMethodMetadata = GetRequiredMetadataRegistration(
            metadataRegistration.Registrations,
            "method",
            importMethodSubjectId,
            "pinvoke import method");

        if (importMethodMetadata.IsImported != true)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{importMethodSubjectId}' to carry imported method metadata");
        }

        if (string.IsNullOrWhiteSpace(importMethodMetadata.ImportModuleName) ||
            string.IsNullOrWhiteSpace(importMethodMetadata.ImportEntryPointName))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{importMethodSubjectId}' to carry importModuleName/importEntryPointName metadata");
        }

        loweringPlan = new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = "pinvokeDllImportMinimal",
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





    private static bool TryCreateMarshalingUtf8ExportLoweringPlan(
        string assemblyName,
        string entryPointSubjectId,
        MetadataRegistrationArtifact metadataRegistration,
        CodeRegistrationEntry entryPointRegistration,
        out NativeReferenceLoweringPlanArtifact? loweringPlan)
    {
        loweringPlan = null;

        if (!entryPointSubjectId.Contains("MarshalingProofEntry::Run", StringComparison.Ordinal))
        {
            return false;
        }

        const string expectedOutput =
            "{\"kind\":\"marshaling-proof\",\"status\":\"ok\",\"marshal\":\"marshal-ok\",\"export\":\"export-ok|chaos_marshaled_add:7\"}";

        loweringPlan = new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = "marshalingUtf8ExportMinimal",
            AssemblyName = assemblyName,
            EntrySubjectId = entryPointSubjectId,
            IncludeHeader = "codegen_bridge.h",
            NativeEntryFunctionName = "RunNativeReference",
            EntrySymbol = entryPointRegistration.Symbol,
            ReferenceTypeToken = "0u",
            CapturedFieldToken = "0u",
            EntryMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", entryPointRegistration.SubjectId)),
            ConsoleWriteLineStringIcall = ConsoleWriteLineStringIcall,
            ExpectedOutput = expectedOutput,
            ExpectedOutputByteCount = Encoding.UTF8.GetByteCount(expectedOutput),
        };

        return true;
    }





    private static bool TryCreateReflectionQueryMinimalLoweringPlan(
        string assemblyName,
        string entryPointSubjectId,
        MetadataRegistrationArtifact metadataRegistration,
        CodeRegistrationEntry entryPointRegistration,
        out NativeReferenceLoweringPlanArtifact? loweringPlan)
    {
        loweringPlan = null;

        var registrations = metadataRegistration.Registrations;
        var closedTypeRegistration = registrations.FirstOrDefault(registration =>
            string.Equals(registration.RegistrationKind, "type", StringComparison.Ordinal) &&
            !string.IsNullOrWhiteSpace(registration.DefinitionSubjectId) &&
            !string.Equals(registration.SubjectId, registration.DefinitionSubjectId, StringComparison.Ordinal));
        if (closedTypeRegistration is null)
        {
            return false;
        }

        var propertyRegistration = registrations.FirstOrDefault(registration =>
            string.Equals(registration.RegistrationKind, "property", StringComparison.Ordinal) &&
            string.Equals(registration.DeclaringTypeSubjectId, closedTypeRegistration.SubjectId, StringComparison.Ordinal) &&
            !IsCompilerGeneratedMemberName(registration.Name));
        var queryMethodRegistration = registrations.FirstOrDefault(registration =>
            string.Equals(registration.RegistrationKind, "method", StringComparison.Ordinal) &&
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





    private static Template GetTemplateForPlan(string planKind)
    {
        return NativeReferenceProofCatalog.GetTemplateForPlan(planKind);
    }





    private static ScriptObject CreateTemplateModel(NativeReferenceLoweringPlanArtifact loweringPlan)
    {
        var model = new ScriptObject
        {
            ["include_header"] = loweringPlan.IncludeHeader,
            ["native_reference_function_name"] = loweringPlan.NativeEntryFunctionName,
            ["assembly_name"] = loweringPlan.AssemblyName,
            ["entry_symbol"] = loweringPlan.EntrySymbol,
            ["reference_type_token"] = loweringPlan.ReferenceTypeToken,
            ["captured_field_token"] = loweringPlan.CapturedFieldToken,
            ["entry_method_token"] = loweringPlan.EntryMethodToken,
            ["console_write_line_string_icall"] = loweringPlan.ConsoleWriteLineStringIcall,
        };

        AddIfNotNull(model, "constructor_symbol", loweringPlan.ConstructorSymbol);
        AddIfNotNull(model, "instance_method_symbol", loweringPlan.InstanceMethodSymbol);
        AddIfNotNull(model, "static_method_symbol", loweringPlan.StaticMethodSymbol);
        AddIfNotNull(model, "throw_method_symbol", loweringPlan.ThrowMethodSymbol);
        AddIfNotNull(model, "echo_method_symbol", loweringPlan.EchoMethodSymbol);
        AddIfNotNull(model, "getter_symbol", loweringPlan.GetterSymbol);
        AddIfNotNull(model, "import_method_subject_id", loweringPlan.ImportMethodSubjectId);
        AddIfNotNull(model, "import_method_symbol", loweringPlan.ImportMethodSymbol);
        AddIfNotNull(model, "import_module_name", loweringPlan.ImportModuleName);
        AddIfNotNull(model, "import_entry_point_name", loweringPlan.ImportEntryPointName);
        AddIfNotNull(model, "import_argument0", loweringPlan.ImportArgument0);
        AddIfNotNull(model, "import_argument1", loweringPlan.ImportArgument1);
        AddIfNotNull(model, "import_argument2", loweringPlan.ImportArgument2);
        AddIfNotNull(model, "constructor_method_token", loweringPlan.ConstructorMethodToken);
        AddIfNotNull(model, "instance_method_token", loweringPlan.InstanceMethodToken);
        AddIfNotNull(model, "static_method_token", loweringPlan.StaticMethodToken);
        AddIfNotNull(model, "throw_method_token", loweringPlan.ThrowMethodToken);
        AddIfNotNull(model, "dispatch_strategy", loweringPlan.DispatchStrategy);
        AddIfNotNull(model, "echo_method_token", loweringPlan.EchoMethodToken);
        AddIfNotNull(model, "getter_method_token", loweringPlan.GetterMethodToken);
        AddIfNotNull(model, "string_concat_pair_icall", loweringPlan.StringConcatPairIcall);
        AddIfNotNull(model, "constructor_literal", loweringPlan.ConstructorLiteral);
        AddIfNotNull(model, "constructor_literal_byte_count", loweringPlan.ConstructorLiteralByteCount);
        AddIfNotNull(model, "stored_literal", loweringPlan.StoredLiteral);
        AddIfNotNull(model, "stored_literal_byte_count", loweringPlan.StoredLiteralByteCount);
        AddIfNotNull(model, "message_prefix_literal", loweringPlan.MessagePrefixLiteral);
        AddIfNotNull(model, "message_prefix_literal_byte_count", loweringPlan.MessagePrefixLiteralByteCount);
        AddIfNotNull(model, "message_suffix_literal", loweringPlan.MessageSuffixLiteral);
        AddIfNotNull(model, "message_suffix_literal_byte_count", loweringPlan.MessageSuffixLiteralByteCount);
        AddIfNotNull(model, "trailing_literal", loweringPlan.TrailingLiteral);
        AddIfNotNull(model, "trailing_literal_byte_count", loweringPlan.TrailingLiteralByteCount);
        AddIfNotNull(model, "echo_literal", loweringPlan.EchoLiteral);
        AddIfNotNull(model, "echo_literal_byte_count", loweringPlan.EchoLiteralByteCount);
        AddIfNotNull(model, "finally_literal", loweringPlan.FinallyLiteral);
        AddIfNotNull(model, "finally_literal_byte_count", loweringPlan.FinallyLiteralByteCount);
        AddIfNotNull(model, "boxed_value_type_token", loweringPlan.BoxedValueTypeToken);
        AddIfNotNull(model, "boxed_int32_value", loweringPlan.BoxedInt32Value);
        AddIfNotNull(model, "source_array_length", loweringPlan.SourceArrayLength);
        AddIfNotNull(model, "target_array_length", loweringPlan.TargetArrayLength);
        AddIfNotNull(model, "source_store_index", loweringPlan.SourceStoreIndex);
        AddIfNotNull(model, "source_array_index", loweringPlan.SourceArrayIndex);
        AddIfNotNull(model, "target_array_index", loweringPlan.TargetArrayIndex);
        AddIfNotNull(model, "target_read_index", loweringPlan.TargetReadIndex);
        AddIfNotNull(model, "copy_length", loweringPlan.CopyLength);
        AddIfNotNull(model, "array_length", loweringPlan.ArrayLength);
        AddIfNotNull(model, "array_store_index", loweringPlan.ArrayStoreIndex);
        AddIfNotNull(model, "clear_start_index", loweringPlan.ClearStartIndex);
        AddIfNotNull(model, "clear_length", loweringPlan.ClearLength);
        AddIfNotNull(model, "reverse_start_index", loweringPlan.ReverseStartIndex);
        AddIfNotNull(model, "reverse_length", loweringPlan.ReverseLength);
        AddIfNotNull(model, "array_read_index", loweringPlan.ArrayReadIndex);
        AddIfNotNull(model, "closed_type_subject_id", loweringPlan.ClosedTypeSubjectId);
        AddIfNotNull(model, "generic_type_definition_subject_id", loweringPlan.GenericTypeDefinitionSubjectId);
        AddIfNotNull(model, "field_subject_id", loweringPlan.FieldSubjectId);
        AddIfNotNull(model, "property_subject_id", loweringPlan.PropertySubjectId);
        AddIfNotNull(model, "method_subject_id", loweringPlan.MethodSubjectId);
        AddIfNotNull(model, "parameter_subject_id", loweringPlan.ParameterSubjectId);
        AddIfNotNull(model, "closed_type_token", loweringPlan.ClosedTypeToken);
        AddIfNotNull(model, "generic_type_definition_token", loweringPlan.GenericTypeDefinitionToken);
        AddIfNotNull(model, "closed_type_namespace_name", loweringPlan.ClosedTypeNamespaceName);
        AddIfNotNull(model, "closed_type_name", loweringPlan.ClosedTypeName);
        AddIfNotNull(model, "closed_type_display_name", loweringPlan.ClosedTypeDisplayName);
        AddIfNotNull(model, "generic_type_definition_name", loweringPlan.GenericTypeDefinitionName);
        AddIfNotNull(model, "field_query_name", loweringPlan.FieldQueryName);
        AddIfNotNull(model, "property_query_name", loweringPlan.PropertyQueryName);
        AddIfNotNull(model, "method_query_name", loweringPlan.MethodQueryName);
        AddIfNotNull(model, "method_parameter_count", loweringPlan.MethodParameterCount);
        AddIfNotNull(model, "parameter_index", loweringPlan.ParameterIndex);
        AddIfNotNull(model, "output_prefix", loweringPlan.OutputPrefix);
        AddIfNotNull(model, "expected_output", loweringPlan.ExpectedOutput);
        AddIfNotNull(model, "expected_output_byte_count", loweringPlan.ExpectedOutputByteCount);

        AddStringLiteralIfNotNull(model, "closed_type_subject_id_literal", loweringPlan.ClosedTypeSubjectId);
        AddStringLiteralIfNotNull(model, "generic_type_definition_subject_id_literal", loweringPlan.GenericTypeDefinitionSubjectId);
        AddStringLiteralIfNotNull(model, "field_subject_id_literal", loweringPlan.FieldSubjectId);
        AddStringLiteralIfNotNull(model, "property_subject_id_literal", loweringPlan.PropertySubjectId);
        AddStringLiteralIfNotNull(model, "method_subject_id_literal", loweringPlan.MethodSubjectId);
        AddStringLiteralIfNotNull(model, "parameter_subject_id_literal", loweringPlan.ParameterSubjectId);
        AddStringLiteralIfNotNull(model, "closed_type_namespace_name_literal", loweringPlan.ClosedTypeNamespaceName);
        AddStringLiteralIfNotNull(model, "closed_type_name_literal", loweringPlan.ClosedTypeName);
        AddStringLiteralIfNotNull(model, "closed_type_display_name_literal", loweringPlan.ClosedTypeDisplayName);
        AddStringLiteralIfNotNull(model, "generic_type_definition_name_literal", loweringPlan.GenericTypeDefinitionName);
        AddStringLiteralIfNotNull(model, "field_query_name_literal", loweringPlan.FieldQueryName);
        AddStringLiteralIfNotNull(model, "property_query_name_literal", loweringPlan.PropertyQueryName);
        AddStringLiteralIfNotNull(model, "method_query_name_literal", loweringPlan.MethodQueryName);
        AddStringLiteralIfNotNull(model, "import_module_name_literal", loweringPlan.ImportModuleName);
        AddStringLiteralIfNotNull(model, "import_entry_point_name_literal", loweringPlan.ImportEntryPointName);
        AddStringLiteralIfNotNull(model, "output_prefix_literal", loweringPlan.OutputPrefix);
        AddStringLiteralIfNotNull(model, "expected_output_literal", loweringPlan.ExpectedOutput);

        return model;
    }





    private static void AddIfNotNull<T>(ScriptObject model, string key, T? value)
    {
        if (value is not null)
        {
            model[key] = value;
        }
    }





    private static void AddStringLiteralIfNotNull(ScriptObject model, string key, string? value)
    {
        if (value is not null)
        {
            model[key] = ToCppStringLiteral(value);
        }
    }

}
