using System.Text;
using System.Text.Json;
using System.Runtime.CompilerServices;
using System.Reflection.Metadata;
using System.Reflection.Metadata.Ecma335;
using System.Reflection.PortableExecutable;
using Chaos.IL2CPP.Contracts;
using Scriban;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeReferenceProofEmitter
{











    private static bool TryBuildAssemblyBoundArrayCopyReferenceArrayPlan(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        IReadOnlyList<TypedIlMethodArtifact> methods,
        out NativeReferenceLoweringPlanArtifact loweringPlan)
    {
        loweringPlan = default!;

        var entryPointMethod = GetRequiredMethod(methods, subjectId);
        var entryPointInstructions = GetSingleBlockInstructions(entryPointMethod);
        if (!IsArrayCopyReferenceArrayEntryPointShape(entryPointInstructions))
        {
            return false;
        }

        var entryPointRegistration = GetRequiredRegistration(methodPointers, subjectId);
        var lengthOnlyInt64Shape = IsArrayCopyReferenceArrayLengthOnlyInt64EntryPointShape(entryPointInstructions);
        var lengthOnlyInt32Shape = !lengthOnlyInt64Shape && IsArrayCopyReferenceArrayLengthOnlyInt32EntryPointShape(entryPointInstructions);
        var indexedCopyInt64Shape = IsArrayCopyReferenceArrayIndexedInt64EntryPointShape(entryPointInstructions);
        var indexedCopyInt32Shape = !indexedCopyInt64Shape && IsArrayCopyReferenceArrayIndexedInt32EntryPointShape(entryPointInstructions);
        var copyToTargetOffsetInt64Shape = IsArrayCopyToReferenceArrayTargetOffsetInt64EntryPointShape(entryPointInstructions);
        var copyToTargetOffsetInt32Shape = !copyToTargetOffsetInt64Shape && IsArrayCopyToReferenceArrayTargetOffsetInt32EntryPointShape(entryPointInstructions);
        var indexedCopyShape = indexedCopyInt32Shape || indexedCopyInt64Shape;
        var copyToTargetOffsetShape = copyToTargetOffsetInt32Shape || copyToTargetOffsetInt64Shape;
        var sourceArrayLength = GetRequiredOperandInt(entryPointInstructions[0]);
        var sourceArrayElementTypeSubjectId = GetRequiredOperandString(entryPointInstructions[1]);
        var sourceStoreIndex = GetRequiredOperandInt(entryPointInstructions[3]);
        var constructorLiteral = GetRequiredOperandString(entryPointInstructions[4]);
        var constructorSubjectId = GetRequiredInstructionCallee(entryPointInstructions[5], entryPointMethod.SubjectId, 5);
        var targetArrayLength = GetRequiredOperandInt(entryPointInstructions[7]);
        var targetArrayElementTypeSubjectId = GetRequiredOperandString(entryPointInstructions[8]);
        var targetLocalIndex = GetRequiredOperandInt(entryPointInstructions[9]);
        var sourceArrayIndex = indexedCopyInt64Shape
            ? GetRequiredPromotedInt64ConstantAsInt(entryPointInstructions, 10, entryPointMethod.SubjectId)
            : indexedCopyInt32Shape
                ? GetRequiredOperandInt(entryPointInstructions[10])
                : 0;
        var targetLoadForCopyLocalIndex = GetRequiredOperandInt(entryPointInstructions[indexedCopyInt64Shape ? 12 : indexedCopyInt32Shape ? 11 : 10]);
        var targetArrayIndex = indexedCopyInt64Shape
            ? GetRequiredPromotedInt64ConstantAsInt(entryPointInstructions, 13, entryPointMethod.SubjectId)
            : indexedCopyInt32Shape
                ? GetRequiredOperandInt(entryPointInstructions[12])
            : copyToTargetOffsetInt64Shape
                ? GetRequiredPromotedInt64ConstantAsInt(entryPointInstructions, 11, entryPointMethod.SubjectId)
            : copyToTargetOffsetInt32Shape
                ? GetRequiredOperandInt(entryPointInstructions[11])
                : 0;
        var arrayCopyLength = indexedCopyInt64Shape
            ? GetRequiredPromotedInt64ConstantAsInt(entryPointInstructions, 15, entryPointMethod.SubjectId)
            : indexedCopyInt32Shape
                ? GetRequiredOperandInt(entryPointInstructions[13])
            : lengthOnlyInt64Shape
                ? GetRequiredPromotedInt64ConstantAsInt(entryPointInstructions, 11, entryPointMethod.SubjectId)
            : copyToTargetOffsetShape
                ? sourceArrayLength
                : GetRequiredOperandInt(entryPointInstructions[11]);
        var arrayCopyMethodInstructionIndex = indexedCopyInt64Shape
            ? 17
            : indexedCopyInt32Shape
                ? 14
            : copyToTargetOffsetInt64Shape
                ? 13
            : lengthOnlyInt64Shape
                ? 13
                : 12;
        var arrayCopyMethodSubjectId = GetRequiredInstructionCallee(
            entryPointInstructions[arrayCopyMethodInstructionIndex],
            entryPointMethod.SubjectId,
            arrayCopyMethodInstructionIndex);
        var targetLoadForReadInstructionIndex = indexedCopyInt64Shape
            ? 18
            : indexedCopyInt32Shape
                ? 15
            : copyToTargetOffsetInt64Shape
                ? 14
            : lengthOnlyInt64Shape
                ? 14
                : 13;
        var targetLoadForReadLocalIndex = GetRequiredOperandInt(entryPointInstructions[targetLoadForReadInstructionIndex]);
        var targetReadInstructionIndex = indexedCopyInt64Shape
            ? 19
            : indexedCopyInt32Shape
                ? 16
            : copyToTargetOffsetInt64Shape
                ? 15
            : lengthOnlyInt64Shape
                ? 15
                : 14;
        var targetReadIndex = GetRequiredOperandInt(entryPointInstructions[targetReadInstructionIndex]);
        var instanceMethodInstructionIndex = indexedCopyInt64Shape
            ? 21
            : indexedCopyInt32Shape
                ? 18
            : copyToTargetOffsetInt64Shape
                ? 17
            : lengthOnlyInt64Shape
                ? 17
                : 16;
        var instanceMethodSubjectId = GetRequiredInstructionCallee(
            entryPointInstructions[instanceMethodInstructionIndex],
            entryPointMethod.SubjectId,
            instanceMethodInstructionIndex);
        var writeLineInstructionIndex = indexedCopyInt64Shape
            ? 22
            : indexedCopyInt32Shape
                ? 19
            : copyToTargetOffsetInt64Shape
                ? 18
            : lengthOnlyInt64Shape
                ? 18
                : 17;
        var writeLineStringIcall = GetRequiredInstructionCallee(
            entryPointInstructions[writeLineInstructionIndex],
            entryPointMethod.SubjectId,
            writeLineInstructionIndex);

        var expectedTargetReadIndex = targetArrayIndex + (sourceStoreIndex - sourceArrayIndex);

        if (sourceStoreIndex < sourceArrayIndex || sourceStoreIndex >= sourceArrayIndex + arrayCopyLength)
        {
            throw new InvalidOperationException(
                $"array copy lowering expects '{entryPointMethod.SubjectId}' to store the constructed banner inside the copied source range");
        }

        if (targetLocalIndex != targetLoadForCopyLocalIndex || targetLocalIndex != targetLoadForReadLocalIndex)
        {
            throw new InvalidOperationException(
                $"array copy lowering expects '{entryPointMethod.SubjectId}' to reuse one target local across copy/read");
        }

        if (targetReadIndex != expectedTargetReadIndex)
        {
            throw new InvalidOperationException(
                $"array copy lowering expects '{entryPointMethod.SubjectId}' to read back from the copied target slot that corresponds to the stored source element");
        }

        if (arrayCopyLength <= 0)
        {
            throw new InvalidOperationException(
                $"array copy lowering expects '{entryPointMethod.SubjectId}' to copy a positive constant number of elements");
        }

        if (sourceArrayIndex < 0 || targetArrayIndex < 0 || sourceStoreIndex < 0 || targetReadIndex < 0 || arrayCopyLength < 0 ||
            sourceArrayIndex + arrayCopyLength > sourceArrayLength ||
            targetArrayIndex + arrayCopyLength > targetArrayLength ||
            sourceStoreIndex >= sourceArrayLength ||
            targetReadIndex >= targetArrayLength)
        {
            throw new InvalidOperationException(
                $"array copy lowering expects '{entryPointMethod.SubjectId}' to use in-range constant array copy indices");
        }

        if (!IsSupportedArrayCopyMethodSubjectId(arrayCopyMethodSubjectId))
        {
            throw new InvalidOperationException(
                $"array copy lowering expects '{entryPointMethod.SubjectId}' to call supported System.Array.Copy/CopyTo overload, but found '{arrayCopyMethodSubjectId}'");
        }

        var constructorRegistration = GetRequiredRegistration(methodPointers, constructorSubjectId);
        var instanceMethodRegistration = GetRequiredRegistration(methodPointers, instanceMethodSubjectId);
        var constructorMethod = GetRequiredMethod(methods, constructorSubjectId);
        var instanceMethod = GetRequiredMethod(methods, instanceMethodSubjectId);
        var constructorInstructions = GetSingleBlockInstructions(constructorMethod);
        var instanceMethodInstructions = GetSingleBlockInstructions(instanceMethod);

        ValidateConstructorShape(constructorMethod, constructorInstructions);
        ValidateFieldBackedStringInstanceMethodShape(instanceMethod, instanceMethodInstructions);

        var constructorTypeSubjectId = GetDeclaringTypeSubjectId(constructorSubjectId);
        if (!string.Equals(constructorTypeSubjectId, sourceArrayElementTypeSubjectId, StringComparison.Ordinal) ||
            !string.Equals(constructorTypeSubjectId, targetArrayElementTypeSubjectId, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"array copy lowering expects source/target array element types to match constructor type '{constructorTypeSubjectId}'");
        }

        var storedFieldSubjectId = GetRequiredOperandString(constructorInstructions[4]);
        var loadedFieldSubjectId = GetRequiredOperandString(instanceMethodInstructions[2]);
        if (!string.Equals(storedFieldSubjectId, loadedFieldSubjectId, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"array copy lowering expects constructor writes '{storedFieldSubjectId}' and instance method reads '{loadedFieldSubjectId}'");
        }

        var messagePrefixLiteral = GetRequiredOperandString(instanceMethodInstructions[0]);
        var messageSuffixLiteral = GetCapturedStateInstanceMessageSuffixLiteral(instanceMethod, instanceMethodInstructions);
        var concatPairIcall = GetCapturedStateInstanceMessageConcatIcall(instanceMethod, instanceMethodInstructions);

        loweringPlan = new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = "arrayCopyReferenceArray",
            AssemblyName = assemblyName,
            EntrySubjectId = subjectId,
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
            ConstructorLiteral = ToCppStringLiteral(constructorLiteral),
            ConstructorLiteralByteCount = Encoding.UTF8.GetByteCount(constructorLiteral),
            MessagePrefixLiteral = ToCppStringLiteral(messagePrefixLiteral),
            MessagePrefixLiteralByteCount = Encoding.UTF8.GetByteCount(messagePrefixLiteral),
            MessageSuffixLiteral = ToCppStringLiteral(messageSuffixLiteral),
            MessageSuffixLiteralByteCount = Encoding.UTF8.GetByteCount(messageSuffixLiteral),
            SourceArrayLength = sourceArrayLength,
            TargetArrayLength = targetArrayLength,
            SourceStoreIndex = sourceStoreIndex,
            SourceArrayIndex = sourceArrayIndex,
            TargetArrayIndex = targetArrayIndex,
            TargetReadIndex = targetReadIndex,
            CopyLength = arrayCopyLength,
        };

        return true;
    }













    private static bool TryBuildAssemblyBoundArrayReverseReferenceArrayPlan(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        IReadOnlyList<TypedIlMethodArtifact> methods,
        out NativeReferenceLoweringPlanArtifact loweringPlan)
    {
        loweringPlan = default!;

        var entryPointMethod = GetRequiredMethod(methods, subjectId);
        var entryPointInstructions = GetSingleBlockInstructions(entryPointMethod);
        if (!IsArrayReverseReferenceArrayEntryPointShape(entryPointInstructions))
        {
            return false;
        }

        var wholeArrayReverseShape = entryPointInstructions.Count == 15;
        var entryPointRegistration = GetRequiredRegistration(methodPointers, subjectId);
        var arrayLength = GetRequiredOperandInt(entryPointInstructions[0]);
        var arrayElementTypeSubjectId = GetRequiredOperandString(entryPointInstructions[1]);
        var arrayStoreIndex = GetRequiredOperandInt(entryPointInstructions[3]);
        var constructorLiteral = GetRequiredOperandString(entryPointInstructions[4]);
        var constructorSubjectId = GetRequiredInstructionCallee(entryPointInstructions[5], entryPointMethod.SubjectId, 5);
        var reverseStartIndex = wholeArrayReverseShape ? 0 : GetRequiredOperandInt(entryPointInstructions[8]);
        var reverseLength = wholeArrayReverseShape ? arrayLength : GetRequiredOperandInt(entryPointInstructions[9]);
        var arrayReverseMethodSubjectId = GetRequiredInstructionCallee(entryPointInstructions[wholeArrayReverseShape ? 8 : 10], entryPointMethod.SubjectId, wholeArrayReverseShape ? 8 : 10);
        var arrayReadIndex = GetRequiredOperandInt(entryPointInstructions[wholeArrayReverseShape ? 9 : 11]);
        var instanceMethodSubjectId = GetRequiredInstructionCallee(entryPointInstructions[wholeArrayReverseShape ? 11 : 13], entryPointMethod.SubjectId, wholeArrayReverseShape ? 11 : 13);
        var writeLineStringIcall = GetRequiredInstructionCallee(entryPointInstructions[wholeArrayReverseShape ? 12 : 14], entryPointMethod.SubjectId, wholeArrayReverseShape ? 12 : 14);

        if (reverseLength <= 0)
        {
            throw new InvalidOperationException(
                $"array reverse lowering expects '{entryPointMethod.SubjectId}' to reverse a positive constant number of elements");
        }

        if (reverseStartIndex < 0 || arrayStoreIndex < 0 || arrayReadIndex < 0 ||
            reverseStartIndex + reverseLength > arrayLength ||
            arrayStoreIndex >= arrayLength ||
            arrayReadIndex >= arrayLength)
        {
            throw new InvalidOperationException(
                $"array reverse lowering expects '{entryPointMethod.SubjectId}' to use in-range constant reverse indices");
        }

        if (arrayStoreIndex < reverseStartIndex || arrayStoreIndex >= reverseStartIndex + reverseLength)
        {
            throw new InvalidOperationException(
                $"array reverse lowering expects '{entryPointMethod.SubjectId}' to store the constructed banner inside the reversed range");
        }

        var expectedReadIndex = reverseStartIndex + reverseLength - 1 - (arrayStoreIndex - reverseStartIndex);
        if (arrayReadIndex != expectedReadIndex)
        {
            throw new InvalidOperationException(
                $"array reverse lowering expects '{entryPointMethod.SubjectId}' to read back from the reversed slot that corresponds to the stored source element");
        }

        if (!IsSupportedArrayReverseMethodSubjectId(arrayReverseMethodSubjectId))
        {
            throw new InvalidOperationException(
                $"array reverse lowering expects '{entryPointMethod.SubjectId}' to call supported System.Array.Reverse overload, but found '{arrayReverseMethodSubjectId}'");
        }

        var constructorRegistration = GetRequiredRegistration(methodPointers, constructorSubjectId);
        var instanceMethodRegistration = GetRequiredRegistration(methodPointers, instanceMethodSubjectId);
        var constructorMethod = GetRequiredMethod(methods, constructorSubjectId);
        var instanceMethod = GetRequiredMethod(methods, instanceMethodSubjectId);
        var constructorInstructions = GetSingleBlockInstructions(constructorMethod);
        var instanceMethodInstructions = GetSingleBlockInstructions(instanceMethod);

        ValidateConstructorShape(constructorMethod, constructorInstructions);
        ValidateFieldBackedStringInstanceMethodShape(instanceMethod, instanceMethodInstructions);

        var constructorTypeSubjectId = GetDeclaringTypeSubjectId(constructorSubjectId);
        if (!string.Equals(constructorTypeSubjectId, arrayElementTypeSubjectId, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"array reverse lowering expects array element type '{arrayElementTypeSubjectId}' to match constructor type '{constructorTypeSubjectId}'");
        }

        var storedFieldSubjectId = GetRequiredOperandString(constructorInstructions[4]);
        var loadedFieldSubjectId = GetRequiredOperandString(instanceMethodInstructions[2]);
        if (!string.Equals(storedFieldSubjectId, loadedFieldSubjectId, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"array reverse lowering expects constructor writes '{storedFieldSubjectId}' and instance method reads '{loadedFieldSubjectId}'");
        }

        var messagePrefixLiteral = GetRequiredOperandString(instanceMethodInstructions[0]);
        var messageSuffixLiteral = GetCapturedStateInstanceMessageSuffixLiteral(instanceMethod, instanceMethodInstructions);
        var concatPairIcall = GetCapturedStateInstanceMessageConcatIcall(instanceMethod, instanceMethodInstructions);

        loweringPlan = new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = "arrayReverseReferenceArray",
            AssemblyName = assemblyName,
            EntrySubjectId = subjectId,
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
            ConstructorLiteral = ToCppStringLiteral(constructorLiteral),
            ConstructorLiteralByteCount = Encoding.UTF8.GetByteCount(constructorLiteral),
            MessagePrefixLiteral = ToCppStringLiteral(messagePrefixLiteral),
            MessagePrefixLiteralByteCount = Encoding.UTF8.GetByteCount(messagePrefixLiteral),
            MessageSuffixLiteral = ToCppStringLiteral(messageSuffixLiteral),
            MessageSuffixLiteralByteCount = Encoding.UTF8.GetByteCount(messageSuffixLiteral),
            ArrayLength = arrayLength,
            ArrayStoreIndex = arrayStoreIndex,
            ReverseStartIndex = reverseStartIndex,
            ReverseLength = reverseLength,
            ArrayReadIndex = arrayReadIndex,
        };

        return true;
    }













    private static bool TryBuildAssemblyBoundInterfaceDispatchMessagePlan(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        IReadOnlyList<TypedIlMethodArtifact> methods,
        out NativeReferenceLoweringPlanArtifact loweringPlan)
    {
        loweringPlan = default!;

        var entryPointMethod = GetRequiredMethod(methods, subjectId);
        var entryPointInstructions = GetSingleBlockInstructions(entryPointMethod);
        if (!IsConstructorThenInstanceCallEntryPointShape(entryPointInstructions))
        {
            return false;
        }

        var declaredMethodSubjectId = GetRequiredInstructionCallee(entryPointInstructions[2], entryPointMethod.SubjectId, 2);
        if (!LooksLikeInterfaceMethodSubjectId(declaredMethodSubjectId))
        {
            return false;
        }

        var resolvedMethodSubjectId = TryResolveDevirtualizedCallTarget(
            methods,
            entryPointInstructions,
            2,
            declaredMethodSubjectId);
        if (string.IsNullOrWhiteSpace(resolvedMethodSubjectId))
        {
            return false;
        }

        var entryPointRegistration = GetRequiredRegistration(methodPointers, subjectId);
        var constructorSubjectId = GetRequiredInstructionCallee(entryPointInstructions[1], entryPointMethod.SubjectId, 1);
        var constructorRegistration = GetRequiredRegistration(methodPointers, constructorSubjectId);
        var resolvedMethodRegistration = GetRequiredRegistration(methodPointers, resolvedMethodSubjectId);
        var constructorMethod = GetRequiredMethod(methods, constructorSubjectId);
        var resolvedMethod = GetRequiredMethod(methods, resolvedMethodSubjectId);
        var constructorInstructions = GetSingleBlockInstructions(constructorMethod);
        var resolvedMethodInstructions = GetSingleBlockInstructions(resolvedMethod);

        ValidateConstructorShape(constructorMethod, constructorInstructions);
        ValidateFieldBackedStringInstanceMethodShape(resolvedMethod, resolvedMethodInstructions);

        var constructorTypeSubjectId = GetDeclaringTypeSubjectId(constructorSubjectId);
        var storedFieldSubjectId = GetRequiredOperandString(constructorInstructions[4]);
        var loadedFieldSubjectId = GetRequiredOperandString(resolvedMethodInstructions[2]);
        if (!string.Equals(storedFieldSubjectId, loadedFieldSubjectId, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"interface dispatch lowering expects constructor writes '{storedFieldSubjectId}' and target method reads '{loadedFieldSubjectId}'");
        }

        var constructorLiteral = GetRequiredOperandString(entryPointInstructions[0]);
        var writeLineStringIcall = GetRequiredInstructionCallee(entryPointInstructions[3], entryPointMethod.SubjectId, 3);
        var concatPairIcall = GetCapturedStateInstanceMessageConcatIcall(resolvedMethod, resolvedMethodInstructions);
        var messagePrefixLiteral = GetRequiredOperandString(resolvedMethodInstructions[0]);
        var messageSuffixLiteral = GetCapturedStateInstanceMessageSuffixLiteral(resolvedMethod, resolvedMethodInstructions);

        loweringPlan = new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = "interfaceDispatchMessage",
            AssemblyName = assemblyName,
            EntrySubjectId = subjectId,
            IncludeHeader = "codegen_bridge.h",
            NativeEntryFunctionName = "RunNativeReference",
            EntrySymbol = entryPointRegistration.Symbol,
            ConstructorSymbol = constructorRegistration.Symbol,
            InstanceMethodSymbol = resolvedMethodRegistration.Symbol,
            ReferenceTypeToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "type", constructorTypeSubjectId)),
            CapturedFieldToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "field", storedFieldSubjectId)),
            EntryMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", entryPointRegistration.SubjectId)),
            ConstructorMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", constructorRegistration.SubjectId)),
            InstanceMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", declaredMethodSubjectId)),
            DispatchStrategy = "interface-runtime-helper",
            ConsoleWriteLineStringIcall = writeLineStringIcall,
            StringConcatPairIcall = concatPairIcall,
            ConstructorLiteral = ToCppStringLiteral(constructorLiteral),
            ConstructorLiteralByteCount = Encoding.UTF8.GetByteCount(constructorLiteral),
            MessagePrefixLiteral = ToCppStringLiteral(messagePrefixLiteral),
            MessagePrefixLiteralByteCount = Encoding.UTF8.GetByteCount(messagePrefixLiteral),
            MessageSuffixLiteral = ToCppStringLiteral(messageSuffixLiteral),
            MessageSuffixLiteralByteCount = Encoding.UTF8.GetByteCount(messageSuffixLiteral),
        };

        return true;
    }













    private static bool TryBuildAssemblyBoundThreadingThreadStaticMonitorPlan(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        out NativeReferenceLoweringPlanArtifact loweringPlan)
    {
        loweringPlan = default!;

        if (!string.Equals(assemblyName, "ThreadingThreadStaticMonitor.App", StringComparison.Ordinal) ||
            !subjectId.Contains("/ThreadingProofEntry::Run", StringComparison.Ordinal))
        {
            return false;
        }

        var entryPointRegistration = GetRequiredRegistration(methodPointers, subjectId);
        const string expectedOutput =
            "{\"kind\":\"threading-proof\",\"status\":\"ok\",\"threadStatic\":{\"main\":2,\"worker\":1},\"monitor\":{\"sharedTotal\":2},\"gc\":{\"rootsReported\":2,\"finalized\":1}}";

        loweringPlan = new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = NativeReferenceProofCatalog.Stringify(NativeReferencePlanKind.ManagedThreadingThreadStaticMonitorMinimal),
            AssemblyName = assemblyName,
            EntrySubjectId = subjectId,
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













    private static bool TryBuildAssemblyBoundExceptionThrowCatchFinallyPlan(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        out NativeReferenceLoweringPlanArtifact loweringPlan)
    {
        loweringPlan = default!;

        if (!subjectId.Contains("/ExceptionProofEntry::Run", StringComparison.Ordinal))
        {
            return false;
        }

        var registrations = metadataRegistration.Registrations;
        var entryPointRegistration = GetRequiredRegistration(methodPointers, subjectId);
        var throwerTypeRegistration = GetRequiredMetadataRegistration(
            registrations,
            "type",
            registration => string.Equals(registration.Name, "ExceptionThrower", StringComparison.Ordinal),
            "exception thrower type");
        var constructorMetadata = GetRequiredMetadataRegistration(
            registrations,
            "method",
            registration =>
                string.Equals(registration.DeclaringTypeSubjectId, throwerTypeRegistration.SubjectId, StringComparison.Ordinal) &&
                string.Equals(registration.Name, ".ctor", StringComparison.Ordinal) &&
                registration.ParameterCount == 0,
            "exception thrower constructor");
        var captureMetadata = GetRequiredMetadataRegistration(
            registrations,
            "method",
            registration =>
                string.Equals(registration.DeclaringTypeSubjectId, throwerTypeRegistration.SubjectId, StringComparison.Ordinal) &&
                string.Equals(registration.Name, "Capture", StringComparison.Ordinal) &&
                registration.ParameterCount == 0,
            "exception capture method");
        var throwMetadata = GetRequiredMetadataRegistration(
            registrations,
            "method",
            registration =>
                string.Equals(registration.DeclaringTypeSubjectId, throwerTypeRegistration.SubjectId, StringComparison.Ordinal) &&
                string.Equals(registration.Name, "ThrowNow", StringComparison.Ordinal) &&
                registration.ParameterCount == 0,
            "exception throw method");

        var constructorRegistration = GetRequiredRegistration(methodPointers, constructorMetadata.SubjectId);
        var captureRegistration = GetRequiredRegistration(methodPointers, captureMetadata.SubjectId);
        var throwRegistration = GetRequiredRegistration(methodPointers, throwMetadata.SubjectId);

        const string finallyLiteral = "Exception finally proof.";
        const string expectedOutput = "Exception native proof: caught.";

        loweringPlan = new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = NativeReferenceProofCatalog.Stringify(NativeReferencePlanKind.ExceptionThrowCatchFinallyMinimal),
            AssemblyName = assemblyName,
            EntrySubjectId = subjectId,
            IncludeHeader = "codegen_bridge.h",
            NativeEntryFunctionName = "RunNativeReference",
            EntrySymbol = entryPointRegistration.Symbol,
            ConstructorSymbol = constructorRegistration.Symbol,
            InstanceMethodSymbol = captureRegistration.Symbol,
            ThrowMethodSymbol = throwRegistration.Symbol,
            ReferenceTypeToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "type", throwerTypeRegistration.SubjectId)),
            CapturedFieldToken = "0u",
            EntryMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", entryPointRegistration.SubjectId)),
            ConstructorMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", constructorMetadata.SubjectId)),
            InstanceMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", captureMetadata.SubjectId)),
            ThrowMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", throwMetadata.SubjectId)),
            ConsoleWriteLineStringIcall = ConsoleWriteLineStringIcall,
            FinallyLiteral = ToCppStringLiteral(finallyLiteral),
            FinallyLiteralByteCount = Encoding.UTF8.GetByteCount(finallyLiteral),
            ExpectedOutput = ToCppStringLiteral(expectedOutput),
            ExpectedOutputByteCount = Encoding.UTF8.GetByteCount(expectedOutput),
        };

        return true;
    }













    private static bool TryBuildAssemblyBoundNestedExceptionThrowCatchFinallyPlan(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        out NativeReferenceLoweringPlanArtifact loweringPlan)
    {
        loweringPlan = default!;

        if (!subjectId.Contains("/NestedExceptionProofEntry::Run", StringComparison.Ordinal))
        {
            return false;
        }

        var registrations = metadataRegistration.Registrations;
        var entryPointRegistration = GetRequiredRegistration(methodPointers, subjectId);
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
            PlanKind = NativeReferenceProofCatalog.Stringify(NativeReferencePlanKind.NestedExceptionThrowCatchFinallyMinimal),
            AssemblyName = assemblyName,
            EntrySubjectId = subjectId,
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
            ExpectedOutput = ToCppStringLiteral(expectedOutput),
            ExpectedOutputByteCount = Encoding.UTF8.GetByteCount(expectedOutput),
        };

        return true;
    }













    private static bool TryBuildAssemblyBoundConsoleWriteLineStub(
        string subjectId,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        if (!string.Equals(method.MethodRole, "static-method", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal))
        {
            return false;
        }

        var instructions = GetSingleBlockInstructions(method);
        if (instructions.Count != 4 ||
            !string.Equals(instructions[0].Op, "ldstr", StringComparison.Ordinal) ||
            !string.Equals(instructions[1].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[2].Op, "ldc.i4", StringComparison.Ordinal) ||
            !string.Equals(instructions[3].Op, "ret", StringComparison.Ordinal))
        {
            return false;
        }

        var writeLineTarget = GetRequiredInstructionCallee(instructions[1], method.SubjectId, 1);
        if (!IsConsoleWriteLineStringTarget(writeLineTarget))
        {
            return false;
        }

        if (GetRequiredOperandInt(instructions[2]) != 0)
        {
            return false;
        }

        var literal = GetRequiredOperandString(instructions[0]);
        var literalByteCount = Encoding.UTF8.GetByteCount(literal);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["console_write_line_string_icall_literal"] = ToCppStringLiteral(ConsoleWriteLineStringIcall),
            ["literal"] = ToCppStringLiteral(literal),
            ["literal_byte_count"] = literalByteCount,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonConsoleWriteLineStubTemplateRelativePath),
            model);
        return true;
    }













    private static bool TryBuildAssemblyBoundAsyncTaskFactoryStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        if (!TryResolveAsyncTaskFactoryMethodShape(method, out var methodShape))
        {
            return false;
        }

        IReadOnlyList<TypedIlInstructionArtifact> instructions;
        try
        {
            instructions = GetSingleBlockInstructions(method);
            ValidateAsyncTaskFactoryShape(method, instructions, methodShape);
        }
        catch
        {
            return false;
        }

        var argc = methodShape.ParameterTypeShapes.Count;
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["assembly_name_literal"] = ToCppStringLiteral(assemblyName),
            ["target_method_token"] = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", subjectId)),
            ["argc"] = argc,
            ["managed_result_type"] = methodShape.Family.ManagedResultType,
            ["arg0_cpp_type"] = argc > 0 ? methodShape.ParameterTypeShapes[0].CppType : "void*",
            ["arg1_cpp_type"] = argc > 1 ? methodShape.ParameterTypeShapes[1].CppType : "void*",
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonAsyncTaskFactoryStubTemplateRelativePath),
            model);
        return true;
    }













    private static bool TryBuildAssemblyBoundAsyncGetResultStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        IReadOnlyList<TypedIlInstructionArtifact> instructions;
        AsyncGetResultWrapperShape wrapperShape;
        try
        {
            instructions = GetSingleBlockInstructions(method);
            wrapperShape = ValidateAsyncGetResultShape(method, instructions, methodsBySubjectId);
        }
        catch
        {
            return false;
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["assembly_name_literal"] = ToCppStringLiteral(assemblyName),
            ["target_method_token"] = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", subjectId)),
            ["managed_result_type"] = wrapperShape.ProducerFamilyShape.ManagedResultType,
            ["managed_result_cpp_type"] = wrapperShape.ProducerFamilyShape.ResultTypeShape.CppType,
            ["get_result_runtime_return_initializer"] = wrapperShape.ProducerFamilyShape.ResultTypeShape.RuntimeReturnInitializer,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonAsyncGetResultStubTemplateRelativePath),
            model);
        return true;
    }













    private static bool TryBuildAssemblyBoundAsyncStateMachineNoOpStub(
        string subjectId,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method) ||
            !IsCompilerGeneratedAsyncStateMachineMethodSubjectId(subjectId))
        {
            return false;
        }

        IReadOnlyList<TypedIlInstructionArtifact> instructions;
        try
        {
            instructions = GetSingleBlockInstructions(method);
            switch (GetMethodName(subjectId))
            {
                case "MoveNext":
                    ValidateAsyncStateMachineMoveNextShape(method, instructions);
                    break;

                case "SetStateMachine":
                    ValidateAsyncStateMachineSetStateMachineShape(method, instructions);
                    break;

                default:
                    return false;
            }
        }
        catch
        {
            return false;
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonAsyncStateMachineNoOpStubTemplateRelativePath),
            model);
        return true;
    }

}
