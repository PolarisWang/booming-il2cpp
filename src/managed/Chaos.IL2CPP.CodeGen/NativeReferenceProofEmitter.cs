using System.Text;
using System.Text.Json;
using Chaos.IL2CPP.Contracts;
using Scriban;
using Scriban.Runtime;

namespace Chaos.IL2CPP.CodeGen;

public sealed class NativeReferenceProofEmitter
{
    private const string GeneratedTranslationUnitTemplateRelativePath = "Templates/NativeReferenceProof.cpp.scriban";
    private const string ObjectCtorSubjectId = "System.Private.CoreLib/System.Object::.ctor()";
    private const string ConsoleWriteLineStringIcall = "System.Console/System.Console::WriteLine(System.String)";
    private const string StringConcatPairIcall = "System.Private.CoreLib/System.String::Concat(System.String,System.String)";
    private static readonly JsonSerializerOptions JsonOptions = new()
    {
        PropertyNameCaseInsensitive = true,
    };
    private static readonly Lazy<Template> GeneratedTranslationUnitTemplate = new(LoadGeneratedTranslationUnitTemplate);

    public NativeReferenceProofResult Generate(NativeReferenceProofRequest request)
    {
        var managedClosureRoot = Path.GetFullPath(request.ManagedClosureRootPath);
        var typedIlPath = Path.Combine(managedClosureRoot, ManagedClosureArtifactNames.TypedIlIr);
        var metadataRegistrationPath = Path.Combine(managedClosureRoot, ManagedClosureArtifactNames.MetadataRegistration);
        var closureManifestPath = Path.Combine(managedClosureRoot, ManagedClosureArtifactNames.ClosureManifest);
        var codeRegistrationPath = Path.Combine(managedClosureRoot, ManagedClosureArtifactNames.CodeRegistration);

        var typedIl = LoadRequiredJson<TypedIlIrArtifact>(typedIlPath);
        var metadataRegistration = LoadRequiredJson<MetadataRegistrationArtifact>(metadataRegistrationPath);
        var closureManifest = LoadRequiredJson<ManagedClosureManifestArtifact>(closureManifestPath);
        var codeRegistration = LoadRequiredJson<CodeRegistrationArtifact>(codeRegistrationPath);

        var methodPointers = codeRegistration.Modules
            .SelectMany(module => module.Registrations)
            .Where(registration => string.Equals(registration.RegistrationKind, "methodPointer", StringComparison.Ordinal))
            .ToList();

        var entryPointRegistration = GetRequiredRegistration(methodPointers, closureManifest.EntrySubjectId);
        var entryPointMethod = GetRequiredMethod(typedIl.Methods, closureManifest.EntrySubjectId);
        var entryPointInstructions = GetSingleBlockInstructions(entryPointMethod);
        ValidateEntryPointShape(entryPointMethod, entryPointInstructions);

        var constructorSubjectId = GetRequiredInstructionCallee(entryPointInstructions[1], entryPointMethod.SubjectId, 1);
        var instanceMethodSubjectId = GetRequiredInstructionCallee(entryPointInstructions[2], entryPointMethod.SubjectId, 2);
        var writeLineStringIcall = GetRequiredInstructionCallee(entryPointInstructions[3], entryPointMethod.SubjectId, 3);

        var constructorRegistration = GetRequiredRegistration(methodPointers, constructorSubjectId);
        var instanceMethodRegistration = GetRequiredRegistration(methodPointers, instanceMethodSubjectId);
        var constructorMethod = GetRequiredMethod(typedIl.Methods, constructorSubjectId);
        var instanceMethod = GetRequiredMethod(typedIl.Methods, instanceMethodSubjectId);

        var translationUnit = BuildGeneratedTranslationUnit(
            closureManifest.AssemblyName,
            metadataRegistration,
            entryPointRegistration,
            constructorRegistration,
            instanceMethodRegistration,
            entryPointMethod,
            constructorMethod,
            instanceMethod,
            writeLineStringIcall);
        var generatedSource = new NativeReferenceGeneratedSource
        {
            RelativePath = NativeReferenceArtifactNames.GeneratedTranslationUnit,
            Contents = translationUnit,
        };

        var manifest = new NativeReferenceProofManifestArtifact
        {
            AssemblyName = closureManifest.AssemblyName,
            EntrySubjectId = closureManifest.EntrySubjectId,
            ManagedClosureRootPath = ManagedNaming.NormalizePathForManifest(managedClosureRoot, Environment.CurrentDirectory),
            GeneratedArtifacts =
            [
                new NativeReferenceGeneratedArtifactRef
                {
                    Kind = "generatedTranslationUnit",
                    Path = generatedSource.RelativePath,
                },
            ],
        };

        return new NativeReferenceProofResult
        {
            OutputRootPath = request.OutputRootPath,
            ProofManifest = manifest,
            GeneratedSources = [generatedSource],
        };
    }

    private static string BuildGeneratedTranslationUnit(
        string assemblyName,
        MetadataRegistrationArtifact metadataRegistration,
        CodeRegistrationEntry entryPointRegistration,
        CodeRegistrationEntry constructorRegistration,
        CodeRegistrationEntry instanceMethodRegistration,
        TypedIlMethodArtifact entryPointMethod,
        TypedIlMethodArtifact constructorMethod,
        TypedIlMethodArtifact instanceMethod,
        string writeLineStringIcall)
    {
        var templateModel = CreateGeneratedTranslationUnitModel(
            assemblyName,
            metadataRegistration,
            entryPointRegistration,
            constructorRegistration,
            instanceMethodRegistration,
            entryPointMethod,
            constructorMethod,
            instanceMethod,
            writeLineStringIcall);
        var context = new TemplateContext();
        context.PushGlobal(templateModel);

        try
        {
            return GeneratedTranslationUnitTemplate.Value.Render(context);
        }
        finally
        {
            context.PopGlobal();
        }
    }

    private static ScriptObject CreateGeneratedTranslationUnitModel(
        string assemblyName,
        MetadataRegistrationArtifact metadataRegistration,
        CodeRegistrationEntry entryPointRegistration,
        CodeRegistrationEntry constructorRegistration,
        CodeRegistrationEntry instanceMethodRegistration,
        TypedIlMethodArtifact entryPointMethod,
        TypedIlMethodArtifact constructorMethod,
        TypedIlMethodArtifact instanceMethod,
        string writeLineStringIcall)
    {
        var entryPointInstructions = GetSingleBlockInstructions(entryPointMethod);
        var constructorInstructions = GetSingleBlockInstructions(constructorMethod);
        var instanceMethodInstructions = GetSingleBlockInstructions(instanceMethod);

        ValidateEntryPointShape(entryPointMethod, entryPointInstructions);
        ValidateConstructorShape(constructorMethod, constructorInstructions);
        ValidateInstanceMethodShape(instanceMethod, instanceMethodInstructions);

        var constructorSubjectId = constructorRegistration.SubjectId;
        var instanceMethodSubjectId = instanceMethodRegistration.SubjectId;
        var constructorTypeSubjectId = GetDeclaringTypeSubjectId(constructorSubjectId);
        var instanceMethodTypeSubjectId = GetDeclaringTypeSubjectId(instanceMethodSubjectId);
        if (!string.Equals(constructorTypeSubjectId, instanceMethodTypeSubjectId, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"first-proof emitter expects constructor '{constructorSubjectId}' and instance method '{instanceMethodSubjectId}' to share a declaring type");
        }

        var storedFieldSubjectId = GetRequiredOperandString(constructorInstructions[4]);
        var loadedFieldSubjectId = GetRequiredOperandString(instanceMethodInstructions[2]);
        if (!string.Equals(storedFieldSubjectId, loadedFieldSubjectId, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"first-proof field access mismatch: constructor writes '{storedFieldSubjectId}', instance method reads '{loadedFieldSubjectId}'");
        }

        var constructorStringLiteral = GetRequiredOperandString(entryPointInstructions[0]);
        var messagePrefixLiteral = GetRequiredOperandString(instanceMethodInstructions[0]);
        var messageSuffixLiteral = GetRequiredOperandString(instanceMethodInstructions[4]);
        var concatPairIcall = GetRequiredInstructionCallee(instanceMethodInstructions[3], instanceMethod.SubjectId, 3);

        return new ScriptObject
        {
            ["include_header"] = "codegen_bridge.h",
            ["proof_function_name"] = "RunNativeReferenceProof",
            ["assembly_name"] = assemblyName,
            ["entry_symbol"] = entryPointRegistration.Symbol,
            ["constructor_symbol"] = constructorRegistration.Symbol,
            ["instance_method_symbol"] = instanceMethodRegistration.Symbol,
            ["reference_type_token"] = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "type", constructorTypeSubjectId)),
            ["captured_field_token"] = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "field", storedFieldSubjectId)),
            ["entry_method_token"] = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", entryPointRegistration.SubjectId)),
            ["constructor_method_token"] = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", constructorRegistration.SubjectId)),
            ["instance_method_token"] = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", instanceMethodRegistration.SubjectId)),
            ["console_write_line_string_icall"] = writeLineStringIcall,
            ["string_concat_pair_icall"] = concatPairIcall,
            ["constructor_literal"] = ToCppStringLiteral(constructorStringLiteral),
            ["constructor_literal_byte_count"] = Encoding.UTF8.GetByteCount(constructorStringLiteral),
            ["message_prefix_literal"] = ToCppStringLiteral(messagePrefixLiteral),
            ["message_prefix_literal_byte_count"] = Encoding.UTF8.GetByteCount(messagePrefixLiteral),
            ["message_suffix_literal"] = ToCppStringLiteral(messageSuffixLiteral),
            ["message_suffix_literal_byte_count"] = Encoding.UTF8.GetByteCount(messageSuffixLiteral),
        };
    }

    private static Template LoadGeneratedTranslationUnitTemplate()
    {
        var templatePath = Path.Combine(AppContext.BaseDirectory, "Templates", "NativeReferenceProof.cpp.scriban");
        if (!File.Exists(templatePath))
        {
            throw new FileNotFoundException("required Scriban template is missing", templatePath);
        }

        var template = Template.Parse(File.ReadAllText(templatePath), templatePath);
        if (!template.HasErrors)
        {
            return template;
        }

        throw new InvalidOperationException(
            $"failed to parse Scriban template '{GeneratedTranslationUnitTemplateRelativePath}':{Environment.NewLine}{string.Join(Environment.NewLine, template.Messages)}");
    }

    private static IReadOnlyList<TypedIlInstructionArtifact> GetSingleBlockInstructions(TypedIlMethodArtifact method)
    {
        if (method.Blocks.Count != 1)
        {
            throw new InvalidOperationException(
                $"first-proof emitter expects a single block in '{method.SubjectId}', but found {method.Blocks.Count}");
        }

        return method.Blocks[0].Instructions;
    }

    private static void ValidateEntryPointShape(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        RequireInstructionCount(method, instructions, 6);
        RequireInstructionOp(instructions[0], "ldstr", method.SubjectId, 0);
        RequireInstructionOp(instructions[1], "newobj", method.SubjectId, 1);
        RequireInstructionOp(instructions[2], "callvirt", method.SubjectId, 2);
        RequireInstructionOp(instructions[3], "call", method.SubjectId, 3);
        RequireInstructionOp(instructions[4], "ldc.i4", method.SubjectId, 4);
        RequireInstructionOp(instructions[5], "ret", method.SubjectId, 5);
        RequireInstructionCallee(instructions[3], ConsoleWriteLineStringIcall, method.SubjectId, 3);

        if (GetRequiredOperandInt(instructions[4]) != 0)
        {
            throw new InvalidOperationException(
                $"first-proof emitter expects '{method.SubjectId}' to return ldc.i4 0 before ret");
        }
    }

    private static void ValidateConstructorShape(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        RequireInstructionCount(method, instructions, 6);
        RequireInstructionOp(instructions[0], "ldarg", method.SubjectId, 0);
        RequireInstructionOp(instructions[1], "call", method.SubjectId, 1);
        RequireInstructionOp(instructions[2], "ldarg", method.SubjectId, 2);
        RequireInstructionOp(instructions[3], "ldarg", method.SubjectId, 3);
        RequireInstructionOp(instructions[4], "stfld", method.SubjectId, 4);
        RequireInstructionOp(instructions[5], "ret", method.SubjectId, 5);

        if (GetRequiredOperandInt(instructions[0]) != 0 || GetRequiredOperandInt(instructions[2]) != 0 || GetRequiredOperandInt(instructions[3]) != 1)
        {
            throw new InvalidOperationException($"first-proof emitter expects '{method.SubjectId}' to use ldarg 0/0/1 pattern");
        }

        RequireInstructionCallee(instructions[1], ObjectCtorSubjectId, method.SubjectId, 1);
    }

    private static void ValidateInstanceMethodShape(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        RequireInstructionCount(method, instructions, 7);
        RequireInstructionOp(instructions[0], "ldstr", method.SubjectId, 0);
        RequireInstructionOp(instructions[1], "ldarg", method.SubjectId, 1);
        RequireInstructionOp(instructions[2], "ldfld", method.SubjectId, 2);
        RequireInstructionOp(instructions[3], "call", method.SubjectId, 3);
        RequireInstructionOp(instructions[4], "ldstr", method.SubjectId, 4);
        RequireInstructionOp(instructions[5], "call", method.SubjectId, 5);
        RequireInstructionOp(instructions[6], "ret", method.SubjectId, 6);

        if (GetRequiredOperandInt(instructions[1]) != 0)
        {
            throw new InvalidOperationException($"first-proof emitter expects '{method.SubjectId}' to start with ldarg 0");
        }

        RequireInstructionCallee(instructions[3], StringConcatPairIcall, method.SubjectId, 3);
        RequireInstructionCallee(instructions[5], StringConcatPairIcall, method.SubjectId, 5);
    }

    private static void RequireInstructionCount(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions,
        int expectedCount)
    {
        if (instructions.Count != expectedCount)
        {
            throw new InvalidOperationException(
                $"first-proof emitter expects '{method.SubjectId}' to have {expectedCount} instructions, but found {instructions.Count}");
        }
    }

    private static void RequireInstructionOp(
        TypedIlInstructionArtifact instruction,
        string expectedOp,
        string subjectId,
        int instructionIndex)
    {
        if (!string.Equals(instruction.Op, expectedOp, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"first-proof emitter expects instruction {instructionIndex} in '{subjectId}' to be '{expectedOp}', but found '{instruction.Op}'");
        }
    }

    private static void RequireInstructionCallee(
        TypedIlInstructionArtifact instruction,
        string expectedCallee,
        string subjectId,
        int instructionIndex)
    {
        if (!string.Equals(instruction.Callee, expectedCallee, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"first-proof emitter expects instruction {instructionIndex} in '{subjectId}' to call '{expectedCallee}', but found '{instruction.Callee ?? "<null>"}'");
        }
    }

    private static string GetRequiredInstructionCallee(
        TypedIlInstructionArtifact instruction,
        string subjectId,
        int instructionIndex)
    {
        if (string.IsNullOrWhiteSpace(instruction.Callee))
        {
            throw new InvalidOperationException(
                $"first-proof emitter expects instruction {instructionIndex} in '{subjectId}' to carry a callee");
        }

        return instruction.Callee;
    }

    private static string GetRequiredOperandString(TypedIlInstructionArtifact instruction)
    {
        return instruction.Operand switch
        {
            string value => value,
            JsonElement element when element.ValueKind == JsonValueKind.String => element.GetString()
                ?? throw new InvalidOperationException($"expected string operand for '{instruction.Op}'"),
            _ => throw new InvalidOperationException($"expected string operand for '{instruction.Op}'"),
        };
    }

    private static int GetRequiredOperandInt(TypedIlInstructionArtifact instruction)
    {
        return instruction.Operand switch
        {
            int value => value,
            long value => checked((int)value),
            JsonElement element when element.ValueKind == JsonValueKind.Number => element.GetInt32(),
            _ => throw new InvalidOperationException($"expected integer operand for '{instruction.Op}'"),
        };
    }

    private static string GetDeclaringTypeSubjectId(string subjectId)
    {
        var separatorIndex = subjectId.IndexOf("::", StringComparison.Ordinal);
        if (separatorIndex <= 0)
        {
            throw new InvalidOperationException($"failed to extract declaring type from subject id '{subjectId}'");
        }

        return subjectId[..separatorIndex];
    }

    private static uint GetRequiredMetadataToken(
        MetadataRegistrationArtifact metadataRegistration,
        string registrationKind,
        string subjectId)
    {
        uint rowIndex = 0u;

        foreach (var registration in metadataRegistration.Registrations)
        {
            if (!string.Equals(registration.RegistrationKind, registrationKind, StringComparison.Ordinal))
            {
                continue;
            }

            rowIndex++;
            if (string.Equals(registration.SubjectId, subjectId, StringComparison.Ordinal))
            {
                return GetMetadataTokenPrefix(registrationKind) | rowIndex;
            }
        }

        throw new InvalidOperationException(
            $"missing required '{registrationKind}' metadata registration for '{subjectId}'");
    }

    private static uint GetMetadataTokenPrefix(string registrationKind)
    {
        return registrationKind switch
        {
            "type" => 0x02000000u,
            "field" => 0x04000000u,
            "method" => 0x06000000u,
            _ => throw new InvalidOperationException(
                $"first-proof emitter does not know how to derive metadata token for registration kind '{registrationKind}'"),
        };
    }

    private static string FormatCppTokenLiteral(uint token)
    {
        return $"0x{token:X8}u";
    }

    private static string ToCppStringLiteral(string value)
    {
        var builder = new StringBuilder();
        builder.Append('"');

        foreach (var current in value)
        {
            builder.Append(current switch
            {
                '\\' => "\\\\",
                '"' => "\\\"",
                '\r' => "\\r",
                '\n' => "\\n",
                '\t' => "\\t",
                _ => current.ToString(),
            });
        }

        builder.Append('"');
        return builder.ToString();
    }

    private static TypedIlMethodArtifact GetRequiredMethod(
        IReadOnlyList<TypedIlMethodArtifact> methods,
        string subjectId)
    {
        var method = methods.FirstOrDefault(item => string.Equals(item.SubjectId, subjectId, StringComparison.Ordinal));
        if (method is null)
        {
            throw new InvalidOperationException($"missing required typed-il method for '{subjectId}'");
        }

        return method;
    }

    private static CodeRegistrationEntry GetRequiredRegistration(
        IReadOnlyList<CodeRegistrationEntry> registrations,
        string subjectId)
    {
        var registration = registrations.FirstOrDefault(item => string.Equals(item.SubjectId, subjectId, StringComparison.Ordinal));
        if (registration is null)
        {
            throw new InvalidOperationException($"missing required method pointer registration for '{subjectId}'");
        }

        return registration;
    }

    private static T LoadRequiredJson<T>(string path)
    {
        if (!File.Exists(path))
        {
            throw new FileNotFoundException("required Stage 4 input artifact is missing", path);
        }

        var value = JsonSerializer.Deserialize<T>(File.ReadAllText(path), JsonOptions);
        if (value is null)
        {
            throw new InvalidOperationException($"failed to deserialize required JSON artifact: {path}");
        }

        return value;
    }
}
