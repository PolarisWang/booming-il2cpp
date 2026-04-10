using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.SemanticWorld;

public sealed class SemanticWorldStage
{
    private const string ThreePartStringConcatSubjectId =
        "System.Private.CoreLib/System.String::Concat(System.String,System.String,System.String)";
    private const string PairStringConcatSubjectId =
        "System.Private.CoreLib/System.String::Concat(System.String,System.String)";
    private const string ConsoleWriteLineStringSubjectId =
        "System.Console/System.Console::WriteLine(System.String)";
    private const string TypeGetTypeFromHandleSubjectId =
        "System.Private.CoreLib/System.Type::GetTypeFromHandle(System.RuntimeTypeHandle)";
    private const string TypeGetFieldSubjectId =
        "System.Private.CoreLib/System.Type::GetField(System.String)";
    private const string TypeGetPropertySubjectId =
        "System.Private.CoreLib/System.Type::GetProperty(System.String)";
    private const string TypeGetMethodSubjectId =
        "System.Private.CoreLib/System.Type::GetMethod(System.String)";
    private const string MethodBaseGetParametersSubjectId =
        "System.Private.CoreLib/System.Reflection.MethodBase::GetParameters()";
    private const string TypeGetGenericTypeDefinitionSubjectId =
        "System.Private.CoreLib/System.Type::GetGenericTypeDefinition()";

    public string Name => "SemanticWorld";

    public SemanticWorldModel Build(LoadedAssemblyModel loadedAssembly)
    {
        var canonicalMethods = loadedAssembly.Methods
            .Select(CanonicalizeMethodBody)
            .ToList();
        var canonicalSubjects = BuildCanonicalSubjects(loadedAssembly);
        var semanticShapes = BuildSemanticShapes(loadedAssembly, canonicalMethods);
        var capabilityBundles = BuildCapabilityBundles(loadedAssembly.Assembly.Name, canonicalMethods);

        return new SemanticWorldModel
        {
            InputAssemblyPath = loadedAssembly.InputAssemblyPath,
            Assembly = loadedAssembly.Assembly,
            EntryPointSubjectId = loadedAssembly.EntryPointSubjectId,
            Types = loadedAssembly.Types,
            Fields = loadedAssembly.Fields,
            Properties = loadedAssembly.Properties,
            Methods = canonicalMethods,
            CanonicalSubjects = canonicalSubjects,
            SemanticShapes = semanticShapes,
            CapabilityBundles = capabilityBundles,
        };
    }

    private static CanonicalSubjectsModel BuildCanonicalSubjects(LoadedAssemblyModel loadedAssembly)
    {
        return new CanonicalSubjectsModel
        {
            Subjects =
            [
                .. loadedAssembly.Types.Select(type => new CanonicalSubjectModel
                {
                    SubjectKind = "type",
                    SubjectId = type.SubjectId,
                    CanonicalSubjectId = type.SubjectId,
                }),
                .. loadedAssembly.Fields.Select(field => new CanonicalSubjectModel
                {
                    SubjectKind = "field",
                    SubjectId = field.SubjectId,
                    CanonicalSubjectId = field.SubjectId,
                }),
                .. loadedAssembly.Properties.Select(property => new CanonicalSubjectModel
                {
                    SubjectKind = "property",
                    SubjectId = property.SubjectId,
                    CanonicalSubjectId = property.SubjectId,
                }),
                .. loadedAssembly.Methods.Select(method => new CanonicalSubjectModel
                {
                    SubjectKind = "method",
                    SubjectId = method.SubjectId,
                    CanonicalSubjectId = method.SubjectId,
                }),
                new CanonicalSubjectModel
                {
                    SubjectKind = "method",
                    SubjectId = ThreePartStringConcatSubjectId,
                    CanonicalSubjectId = PairStringConcatSubjectId,
                },
            ],
        };
    }

    private static SemanticShapesModel BuildSemanticShapes(
        LoadedAssemblyModel loadedAssembly,
        IReadOnlyList<ManagedMethodModel> canonicalMethods)
    {
        return new SemanticShapesModel
        {
            Types = loadedAssembly.Types.Select(type => new TypeShapeModel
            {
                SubjectId = type.SubjectId,
                Kind = "type-definition",
            }).ToList(),
            Fields = loadedAssembly.Fields.Select(field => new FieldShapeModel
            {
                SubjectId = field.SubjectId,
                Kind = "field-definition",
            }).ToList(),
            Properties = loadedAssembly.Properties.Select(property => new PropertyShapeModel
            {
                SubjectId = property.SubjectId,
                Kind = "property-definition",
            }).ToList(),
            Methods = canonicalMethods.Select(method => new MethodShapeModel
            {
                SubjectId = method.SubjectId,
                MethodRole = ResolveMethodRole(method),
                BodyAvailability = ResolveBodyAvailability(method),
            }).ToList(),
        };
    }

    private static CapabilityBundlesModel BuildCapabilityBundles(
        string assemblyName,
        IReadOnlyList<ManagedMethodModel> canonicalMethods)
    {
        var importMethods = canonicalMethods
            .Where(method => method.Import is not null)
            .Select(method => method.SubjectId)
            .ToHashSet(StringComparer.Ordinal);

        var methodBundles = canonicalMethods
            .Select(method => new MethodCapabilityBundleModel
            {
                SubjectId = method.SubjectId,
                Capabilities = ResolveMethodCapabilities(assemblyName, method, importMethods),
            })
            .ToList();

        return new CapabilityBundlesModel
        {
            World = new WorldCapabilityBundleModel
            {
                Capabilities = methodBundles
                    .SelectMany(bundle => bundle.Capabilities)
                    .Distinct(StringComparer.Ordinal)
                    .OrderBy(capability => capability, StringComparer.Ordinal)
                    .ToList(),
            },
            Methods = methodBundles,
        };
    }

    private static ManagedMethodModel CanonicalizeMethodBody(ManagedMethodModel method)
    {
        return method with
        {
            Body = new ManagedMethodBodyModel
            {
                ExceptionRegions = method.Body.ExceptionRegions
                    .Select(region => region with { })
                    .ToList(),
                Blocks = method.Body.Blocks.Select(CanonicalizeBlock).ToList(),
            },
        };
    }

    private static ManagedBlockModel CanonicalizeBlock(ManagedBlockModel block)
    {
        return block with
        {
            Instructions = CanonicalizeInstructions(block.Instructions),
        };
    }

    private static IReadOnlyList<ManagedInstructionModel> CanonicalizeInstructions(
        IReadOnlyList<ManagedInstructionModel> instructions)
    {
        var canonicalized = new List<ManagedInstructionModel>();

        foreach (var instruction in instructions)
        {
            if (instruction.Op == "call" &&
                string.Equals(instruction.Callee, ThreePartStringConcatSubjectId, StringComparison.Ordinal))
            {
                if (canonicalized.Count == 0)
                {
                    throw new NotSupportedException(
                        "unable to canonicalize String.Concat(string,string,string) without prior operands");
                }

                var trailingOperand = canonicalized[^1];
                canonicalized.RemoveAt(canonicalized.Count - 1);
                canonicalized.Add(CreatePairStringConcatInstruction());
                canonicalized.Add(trailingOperand);
                canonicalized.Add(CreatePairStringConcatInstruction());
                continue;
            }

            canonicalized.Add(instruction);
        }

        return canonicalized;
    }

    private static ManagedInstructionModel CreatePairStringConcatInstruction()
    {
        return new ManagedInstructionModel
        {
            Op = "call",
            Callee = PairStringConcatSubjectId,
            ResultType = "System.String",
            Reference = new ManagedInstructionReference
            {
                AssemblyName = "System.Private.CoreLib",
                SubjectKind = "method",
                SubjectId = PairStringConcatSubjectId,
            },
        };
    }

    private static string ResolveMethodRole(ManagedMethodModel method)
    {
        if (method.Import is not null)
        {
            return "imported-method";
        }

        if (string.Equals(method.Name, ".ctor", StringComparison.Ordinal))
        {
            return "constructor";
        }

        if (IsStaticForwarder(method))
        {
            return "static-forwarder";
        }

        if (IsInstanceFieldGetter(method))
        {
            return "instance-field-getter";
        }

        return method.IsStatic
            ? "static-method"
            : "instance-method";
    }

    private static string ResolveBodyAvailability(ManagedMethodModel method)
    {
        return method.Import is null &&
               method.Body.Blocks.Any(block => block.Instructions.Count > 0)
            ? "has-canonical-body"
            : "no-canonical-body";
    }

    private static IReadOnlyList<string> ResolveMethodCapabilities(
        string assemblyName,
        ManagedMethodModel method,
        IReadOnlySet<string> importMethods)
    {
        var capabilities = new HashSet<string>(StringComparer.Ordinal);

        if (method.Import is not null)
        {
            capabilities.Add("requires-imported-call");
        }

        foreach (var instruction in method.Body.Blocks.SelectMany(block => block.Instructions))
        {
            switch (instruction.Op)
            {
                case "ldftn":
                case "ldvirtftn":
                    capabilities.Add("requires-delegate-construction");
                    break;
                case "ldfld":
                case "stfld":
                    capabilities.Add("uses-instance-field-state");
                    break;
                case "callvirt":
                    capabilities.Add("uses-virtual-call-site");
                    if (IsDelegateInvokeCallee(instruction.Callee))
                    {
                        capabilities.Add("requires-delegate-invoke");
                    }
                    break;
                case "newarr":
                    capabilities.Add("requires-array-allocation");
                    break;
                case "stelem.ref":
                    capabilities.Add("requires-reference-array-store");
                    break;
                case "box":
                    capabilities.Add("requires-boxing");
                    break;
                case "throw":
                case "rethrow":
                    capabilities.Add("requires-exception-throw");
                    break;
                case "endfinally":
                    capabilities.Add("requires-finally");
                    break;
            }

            if (string.IsNullOrEmpty(instruction.Callee))
            {
                continue;
            }

            switch (instruction.Callee)
            {
                case PairStringConcatSubjectId:
                    capabilities.Add("requires-string-concat");
                    break;
                case ConsoleWriteLineStringSubjectId:
                    capabilities.Add("requires-console-string-output");
                    break;
                case TypeGetTypeFromHandleSubjectId:
                case TypeGetFieldSubjectId:
                case TypeGetPropertySubjectId:
                case TypeGetMethodSubjectId:
                case MethodBaseGetParametersSubjectId:
                    capabilities.Add("requires-closed-type-member-query");
                    break;
                case TypeGetGenericTypeDefinitionSubjectId:
                    capabilities.Add("requires-generic-type-definition-query");
                    break;
            }

            var reference = instruction.Reference;
            if (reference is not null &&
                string.Equals(reference.AssemblyName, assemblyName, StringComparison.Ordinal) &&
                importMethods.Contains(reference.SubjectId))
            {
                capabilities.Add("requires-imported-call");
            }
        }

        foreach (var region in method.Body.ExceptionRegions)
        {
            capabilities.Add("requires-exception-handler");
            if (string.Equals(region.HandlingKind, "finally", StringComparison.Ordinal))
            {
                capabilities.Add("requires-finally");
            }
        }

        return capabilities
            .OrderBy(capability => capability, StringComparer.Ordinal)
            .ToList();
    }

    private static bool IsDelegateInvokeCallee(string? subjectId)
    {
        return !string.IsNullOrWhiteSpace(subjectId) &&
               subjectId.Contains("::Invoke(", StringComparison.Ordinal);
    }

    private static bool IsStaticForwarder(ManagedMethodModel method)
    {
        if (!method.IsStatic || method.Import is not null || method.Parameters.Count != 1)
        {
            return false;
        }

        var instructions = TryGetSingleBlockInstructions(method);
        return instructions is not null &&
               instructions.Count == 2 &&
               string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "ret", StringComparison.Ordinal) &&
               GetInstructionIntOperandOrNull(instructions[0]) == 0;
    }

    private static bool IsInstanceFieldGetter(ManagedMethodModel method)
    {
        if (method.IsStatic || method.Import is not null)
        {
            return false;
        }

        var instructions = TryGetSingleBlockInstructions(method);
        return instructions is not null &&
               instructions.Count == 3 &&
               string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "ldfld", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "ret", StringComparison.Ordinal) &&
               GetInstructionIntOperandOrNull(instructions[0]) == 0;
    }

    private static IReadOnlyList<ManagedInstructionModel>? TryGetSingleBlockInstructions(ManagedMethodModel method)
    {
        return method.Body.Blocks.Count == 1
            ? method.Body.Blocks[0].Instructions
            : null;
    }

    private static int? GetInstructionIntOperandOrNull(ManagedInstructionModel instruction)
    {
        return instruction.Operand switch
        {
            int value => value,
            long value => checked((int)value),
            _ => null,
        };
    }
}
