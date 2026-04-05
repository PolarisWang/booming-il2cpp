using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.CodeGen;

public sealed class CodeGenStage
{
    public string Name => "CodeGen";

    public ManagedClosureResult Generate(
        ManagedClosureRequest request,
        LinkedWorldModel linkedWorld,
        MetadataWriterOutput metadataWriterOutput)
    {
        var typedIl = new TypedIlIrArtifact
        {
            Methods = linkedWorld.Methods.Select(ToTypedIlMethodArtifact).ToList(),
        };

        var codeRegistration = new CodeRegistrationArtifact
        {
            Modules =
            [
                new CodeRegistrationModule
                {
                    ModuleName = $"{linkedWorld.Assembly.Name}.dll",
                    Registrations = linkedWorld.Methods.Select((method, index) => new CodeRegistrationEntry
                    {
                        RegistrationKind = "methodPointer",
                        Slot = index,
                        Symbol = ManagedNaming.CreateMethodSymbol(method),
                        SubjectId = method.SubjectId,
                    }).ToList(),
                },
            ],
        };

        var closureManifest = new ManagedClosureManifestArtifact
        {
            AssemblyName = linkedWorld.Assembly.Name,
            EntrySubjectId = linkedWorld.EntryPointSubjectId,
            InputAssemblyPath = ManagedNaming.NormalizePathForManifest(request.InputAssemblyPath, Environment.CurrentDirectory),
            InputModuleVersionId = linkedWorld.Assembly.ModuleVersionId.ToString(),
            Artifacts =
            [
                new ManagedClosureArtifactRef { Kind = "typedIlIr", Path = ManagedClosureArtifactNames.TypedIlIr },
                new ManagedClosureArtifactRef { Kind = "aotManifest", Path = ManagedClosureArtifactNames.AotManifest },
                new ManagedClosureArtifactRef { Kind = "metadataRegistration", Path = ManagedClosureArtifactNames.MetadataRegistration },
                new ManagedClosureArtifactRef { Kind = "codeRegistration", Path = ManagedClosureArtifactNames.CodeRegistration },
            ],
        };

        return new ManagedClosureResult
        {
            OutputRootPath = request.OutputRootPath,
            TypedIlIr = typedIl,
            AotManifest = metadataWriterOutput.AotManifest,
            MetadataRegistration = metadataWriterOutput.MetadataRegistration,
            CodeRegistration = codeRegistration,
            ClosureManifest = closureManifest,
        };
    }

    private static TypedIlMethodArtifact ToTypedIlMethodArtifact(ManagedMethodModel method)
    {
        return new TypedIlMethodArtifact
        {
            MethodId = ManagedNaming.CreateMethodId(method),
            SubjectId = method.SubjectId,
            Signature = method.Signature,
            Parameters = method.Parameters.Select(parameter => new TypedIlParameterArtifact
            {
                Name = parameter.Name,
                Type = parameter.Type,
            }).ToList(),
            Blocks = method.Body.Blocks.Select(block => new TypedIlBlockArtifact
            {
                BlockId = block.BlockId,
                Instructions = NormalizeInstructions(block.Instructions).Select(ToTypedIlInstructionArtifact).ToList(),
            }).ToList(),
        };
    }

    private static IReadOnlyList<ManagedInstructionModel> NormalizeInstructions(IReadOnlyList<ManagedInstructionModel> instructions)
    {
        var normalized = new List<ManagedInstructionModel>();

        foreach (var instruction in instructions)
        {
            if (instruction.Op == "call" &&
                string.Equals(
                    instruction.Callee,
                    "System.Private.CoreLib/System.String::Concat(System.String,System.String,System.String)",
                    StringComparison.Ordinal))
            {
                if (normalized.Count == 0)
                {
                    throw new NotSupportedException("unable to canonicalize String.Concat(string,string,string) without prior operands");
                }

                var trailingOperand = normalized[^1];
                normalized.RemoveAt(normalized.Count - 1);

                normalized.Add(new ManagedInstructionModel
                {
                    Op = "call",
                    Callee = "System.Private.CoreLib/System.String::Concat(System.String,System.String)",
                    ResultType = "System.String",
                });
                normalized.Add(trailingOperand);
                normalized.Add(new ManagedInstructionModel
                {
                    Op = "call",
                    Callee = "System.Private.CoreLib/System.String::Concat(System.String,System.String)",
                    ResultType = "System.String",
                });
                continue;
            }

            normalized.Add(instruction);
        }

        return normalized;
    }

    private static TypedIlInstructionArtifact ToTypedIlInstructionArtifact(ManagedInstructionModel instruction)
    {
        return new TypedIlInstructionArtifact
        {
            Op = instruction.Op,
            Operand = instruction.Operand,
            ResultType = instruction.ResultType,
            Callee = instruction.Callee,
        };
    }
}
