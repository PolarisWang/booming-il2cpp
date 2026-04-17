using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.SemanticWorld;

public sealed class SemanticWorldStage
{
    private const string ThreePartStringConcatSubjectId =
        "System.Private.CoreLib/System.String::Concat(System.String,System.String,System.String)";
    private const string PairStringConcatSubjectId =
        "System.Private.CoreLib/System.String::Concat(System.String,System.String)";
    private const string StringJoinStringEnumerableSubjectId =
        "System.Private.CoreLib/System.String::Join(System.String,System.Collections.Generic.IEnumerable<System.String>)";
    private const string GenericStringJoinEnumerableSubjectIdPrefix =
        "System.Private.CoreLib/System.String::Join<";
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

    public SemanticWorldModel Build(LoadedWorldModel loadedWorld)
    {
        var canonicalMethods = loadedWorld.Methods
            .Select(CanonicalizeMethodBody)
            .ToList();
        var canonicalSubjects = BuildCanonicalSubjects(loadedWorld);
        var semanticShapes = BuildSemanticShapes(loadedWorld, canonicalMethods);
        var capabilityBundles = BuildCapabilityBundles(
            loadedWorld.Assemblies.Select(assembly => assembly.Assembly.Name).ToHashSet(StringComparer.Ordinal),
            loadedWorld.Types.ToDictionary(type => type.SubjectId, StringComparer.Ordinal),
            loadedWorld.Fields.ToDictionary(field => field.SubjectId, StringComparer.Ordinal),
            canonicalMethods);

        return new SemanticWorldModel
        {
            InputAssemblyPath = loadedWorld.InputAssemblyPath,
            Assembly = loadedWorld.Assembly,
            Assemblies = loadedWorld.Assemblies.Select(assembly => assembly.Assembly).ToList(),
            EntryPointSubjectId = loadedWorld.EntryPointSubjectId,
            Types = loadedWorld.Types,
            Fields = loadedWorld.Fields,
            Properties = loadedWorld.Properties,
            Methods = canonicalMethods,
            CanonicalSubjects = canonicalSubjects,
            SemanticShapes = semanticShapes,
            CapabilityBundles = capabilityBundles,
        };
    }

    private static CanonicalSubjectsModel BuildCanonicalSubjects(LoadedWorldModel loadedWorld)
    {
        return new CanonicalSubjectsModel
        {
            Subjects =
            [
                .. loadedWorld.Types.Select(type => new CanonicalSubjectModel
                {
                    SubjectKind = "type",
                    SubjectId = type.SubjectId,
                    CanonicalSubjectId = type.SubjectId,
                }),
                .. loadedWorld.Fields.Select(field => new CanonicalSubjectModel
                {
                    SubjectKind = "field",
                    SubjectId = field.SubjectId,
                    CanonicalSubjectId = field.SubjectId,
                }),
                .. loadedWorld.Properties.Select(property => new CanonicalSubjectModel
                {
                    SubjectKind = "property",
                    SubjectId = property.SubjectId,
                    CanonicalSubjectId = property.SubjectId,
                }),
                .. loadedWorld.Methods.Select(method => new CanonicalSubjectModel
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
        LoadedWorldModel loadedWorld,
        IReadOnlyList<ManagedMethodModel> canonicalMethods)
    {
        return new SemanticShapesModel
        {
            Types = loadedWorld.Types.Select(type => new TypeShapeModel
            {
                SubjectId = type.SubjectId,
                Kind = "type-definition",
            }).ToList(),
            Fields = loadedWorld.Fields.Select(field => new FieldShapeModel
            {
                SubjectId = field.SubjectId,
                Kind = "field-definition",
            }).ToList(),
            Properties = loadedWorld.Properties.Select(property => new PropertyShapeModel
            {
                SubjectId = property.SubjectId,
                Kind = "property-definition",
            }).ToList(),
            Methods = canonicalMethods.Select(method =>
            {
                var bodyAvailabilityCode = BodyAvailabilityResolver.Resolve(method);
                return new MethodShapeModel
                {
                    SubjectId = method.SubjectId,
                    MethodRole = ResolveMethodRole(method),
                    BodyAvailability = BodyAvailabilityResolver.ToLegacyLabel(bodyAvailabilityCode),
                    BodyAvailabilityCode = bodyAvailabilityCode,
                };
            }).ToList(),
        };
    }

    private static CapabilityBundlesModel BuildCapabilityBundles(
        IReadOnlySet<string> internalAssemblyNames,
        IReadOnlyDictionary<string, ManagedTypeModel> types,
        IReadOnlyDictionary<string, ManagedFieldModel> fields,
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
                Capabilities = ResolveMethodCapabilities(internalAssemblyNames, types, fields, method, importMethods),
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

    private static IReadOnlyList<string> ResolveMethodCapabilities(
        IReadOnlySet<string> internalAssemblyNames,
        IReadOnlyDictionary<string, ManagedTypeModel> types,
        IReadOnlyDictionary<string, ManagedFieldModel> fields,
        ManagedMethodModel method,
        IReadOnlySet<string> importMethods)
    {
        var capabilities = new HashSet<string>(StringComparer.Ordinal);

        if (IsCompilerGeneratedAsyncStateMachineMethod(method))
        {
            capabilities.Add("requires-async-state-machine");
        }

        if (method.Import is not null)
        {
            capabilities.Add("requires-imported-call");
        }

        if (method.IsUnmanagedCallersOnly)
        {
            capabilities.Add("requires-unmanaged-callers-only-export");
        }

        foreach (var instruction in method.Body.Blocks.SelectMany(block => block.Instructions))
        {
            var reference = instruction.Reference;

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
                case "ldsfld":
                case "stsfld":
                    if (IsThreadStaticField(fields, reference?.SubjectId))
                    {
                        capabilities.Add("requires-thread-static-storage");
                    }

                    break;
                case "callvirt":
                    capabilities.Add("uses-virtual-call-site");
                    if (IsInterfaceDispatchCallee(types, instruction.Callee))
                    {
                        capabilities.Add("uses-interface-call-site");
                    }

                    if (IsDelegateInvokeCallee(instruction.Callee))
                    {
                        capabilities.Add("requires-delegate-invoke");
                    }
                    break;
                case "newarr":
                    capabilities.Add("requires-array-allocation");
                    break;
                case "ldlen":
                    capabilities.Add("requires-array-length-query");
                    break;
                case "stelem.ref":
                    capabilities.Add("requires-reference-array-store");
                    break;
                case "stelem":
                    capabilities.Add("requires-value-array-store");
                    break;
                case "ldelem":
                case "ldelema":
                    capabilities.Add("requires-value-array-load");
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
                if (IsThreadStaticField(fields, reference?.SubjectId))
                {
                    capabilities.Add("requires-thread-static-storage");
                }

                if (IsAsyncStateMachineSurface(null, reference))
                {
                    capabilities.Add("requires-async-state-machine");
                }

                if (IsTaskAwaiterSurface(null, reference))
                {
                    capabilities.Add("requires-task-awaiter");
                }

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

            if (IsAsyncStateMachineSurface(instruction.Callee, reference))
            {
                capabilities.Add("requires-async-state-machine");
            }

            if (IsTaskAwaiterSurface(instruction.Callee, reference))
            {
                capabilities.Add("requires-task-awaiter");
            }

            if (IsUtf8StringMarshalSurface(instruction.Callee, reference))
            {
                capabilities.Add("requires-utf8-string-marshaling");
            }

            if (IsMonitorEnterExitSurface(instruction.Callee, reference))
            {
                capabilities.Add("requires-monitor-enter-exit");
            }

            if (IsStringJoinEnumerableSurface(instruction.Callee))
            {
                capabilities.Add("requires-string-join-enumerable");
            }

            if (reference is not null &&
                internalAssemblyNames.Contains(reference.AssemblyName) &&
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

        if (HasNestedExceptionHandlerShape(method.Body.ExceptionRegions))
        {
            capabilities.Add("requires-nested-exception-handler");
        }

        return capabilities
            .OrderBy(capability => capability, StringComparer.Ordinal)
            .ToList();
    }

    private static bool HasNestedExceptionHandlerShape(
        IReadOnlyList<ManagedExceptionRegionModel> exceptionRegions)
    {
        for (var outerIndex = 0; outerIndex < exceptionRegions.Count; outerIndex++)
        {
            var outerRegion = exceptionRegions[outerIndex];
            var outerTryEnd = outerRegion.TryOffset + outerRegion.TryLength;

            for (var innerIndex = 0; innerIndex < exceptionRegions.Count; innerIndex++)
            {
                if (outerIndex == innerIndex)
                {
                    continue;
                }

                var innerRegion = exceptionRegions[innerIndex];
                var innerTryEnd = innerRegion.TryOffset + innerRegion.TryLength;
                if (outerRegion.TryOffset <= innerRegion.TryOffset &&
                    innerTryEnd <= outerTryEnd &&
                    (outerRegion.TryOffset != innerRegion.TryOffset || outerTryEnd != innerTryEnd))
                {
                    return true;
                }
            }
        }

        return false;
    }

    private static bool IsDelegateInvokeCallee(string? subjectId)
    {
        return !string.IsNullOrWhiteSpace(subjectId) &&
               subjectId.Contains("::Invoke(", StringComparison.Ordinal);
    }

    private static bool IsCompilerGeneratedAsyncStateMachineMethod(ManagedMethodModel method)
    {
        return method.DeclaringTypeDisplayName.Contains("d__", StringComparison.Ordinal) &&
               (string.Equals(method.Name, "MoveNext", StringComparison.Ordinal) ||
                string.Equals(method.Name, "SetStateMachine", StringComparison.Ordinal));
    }

    private static bool IsAsyncStateMachineSurface(
        string? callee,
        ManagedInstructionReference? reference)
    {
        return ContainsAsyncSurfaceMarker(callee) ||
               ContainsAsyncSurfaceMarker(reference?.SubjectId);
    }

    private static bool IsTaskAwaiterSurface(
        string? callee,
        ManagedInstructionReference? reference)
    {
        return ContainsTaskAwaiterMarker(callee) ||
               ContainsTaskAwaiterMarker(reference?.SubjectId);
    }

    private static bool ContainsAsyncSurfaceMarker(string? subjectId)
    {
        return !string.IsNullOrWhiteSpace(subjectId) &&
               (subjectId.Contains("IAsyncStateMachine", StringComparison.Ordinal) ||
                subjectId.Contains("AsyncTaskMethodBuilder", StringComparison.Ordinal));
    }

    private static bool ContainsTaskAwaiterMarker(string? subjectId)
    {
        return !string.IsNullOrWhiteSpace(subjectId) &&
               (subjectId.Contains("TaskAwaiter", StringComparison.Ordinal) ||
                subjectId.Contains("::GetAwaiter(", StringComparison.Ordinal) ||
                subjectId.Contains("::GetResult(", StringComparison.Ordinal));
    }

    private static bool IsStringJoinEnumerableSurface(string? subjectId)
    {
        if (string.IsNullOrWhiteSpace(subjectId))
        {
            return false;
        }

        return string.Equals(subjectId, StringJoinStringEnumerableSubjectId, StringComparison.Ordinal) ||
               (subjectId.StartsWith(GenericStringJoinEnumerableSubjectIdPrefix, StringComparison.Ordinal) &&
                subjectId.Contains("(System.String,System.Collections.Generic.IEnumerable<", StringComparison.Ordinal));
    }

    private static bool IsInterfaceDispatchCallee(
        IReadOnlyDictionary<string, ManagedTypeModel> types,
        string? subjectId)
    {
        if (string.IsNullOrWhiteSpace(subjectId))
        {
            return false;
        }

        var separatorIndex = subjectId.IndexOf("::", StringComparison.Ordinal);
        if (separatorIndex <= 0)
        {
            return false;
        }

        var declaringTypeSubjectId = subjectId[..separatorIndex];
        return types.TryGetValue(declaringTypeSubjectId, out var declaringType) && declaringType.IsInterface;
    }

    private static bool IsThreadStaticField(
        IReadOnlyDictionary<string, ManagedFieldModel> fields,
        string? subjectId)
    {
        return !string.IsNullOrWhiteSpace(subjectId) &&
               fields.TryGetValue(subjectId, out var field) &&
               field.IsThreadStatic;
    }

    private static bool IsMonitorEnterExitSurface(
        string? callee,
        ManagedInstructionReference? reference)
    {
        return ContainsMonitorSurfaceMarker(callee) ||
               ContainsMonitorSurfaceMarker(reference?.SubjectId);
    }

    private static bool IsUtf8StringMarshalSurface(
        string? callee,
        ManagedInstructionReference? reference)
    {
        return ContainsUtf8MarshalSurfaceMarker(callee) ||
               ContainsUtf8MarshalSurfaceMarker(reference?.SubjectId);
    }

    private static bool ContainsMonitorSurfaceMarker(string? subjectId)
    {
        return !string.IsNullOrWhiteSpace(subjectId) &&
               (subjectId.Contains("System.Threading.Monitor::Enter(", StringComparison.Ordinal) ||
                subjectId.Contains("System.Threading.Monitor::Exit(", StringComparison.Ordinal));
    }

    private static bool ContainsUtf8MarshalSurfaceMarker(string? subjectId)
    {
        return !string.IsNullOrWhiteSpace(subjectId) &&
               (subjectId.Contains("System.Runtime.InteropServices.Marshal::StringToCoTaskMemUTF8(", StringComparison.Ordinal) ||
                subjectId.Contains("System.Runtime.InteropServices.Marshal::PtrToStringUTF8(", StringComparison.Ordinal) ||
                subjectId.Contains("System.Runtime.InteropServices.Marshal::FreeCoTaskMem(", StringComparison.Ordinal));
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
