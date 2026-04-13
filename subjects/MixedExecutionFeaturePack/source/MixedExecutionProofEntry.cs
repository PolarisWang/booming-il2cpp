using System.Reflection;
using System.Text.Json;
using Chaos.IL2CPP.HotUpdate;
using Chaos.IL2CPP.Interpreter;
using Chaos.IL2CPP.Loader;
using Chaos.TestFramework;

namespace MixedExecutionFeaturePack;

internal static class MixedExecutionProofEntry
{
    private const string CurrentAotVersion = "1.0.0";
    private const string AotBridgeSubjectId = "InterpreterArithmeticProof.AotBridge/AotBridgeExports::Add(System.Int32,System.Int32)";
    private const string EngineBridgeId = "mixed-engine-add-five";
    private const string EngineBridgeSubjectId = "MixedExecutionProof/Engine::AddFive(System.Int32)";

    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "mixed-execution-proof",
        Requires = ChaosRuntimeFeature.HotUpdate,
        Evidence = ChaosEvidenceKind.Stdout | ChaosEvidenceKind.Metadata,
        Priority = 1)]
    public static int Run()
    {
        var assemblyPath = InterpreterArithmeticSupport.ResolveAssemblyPath();
        var workspace = Path.Combine(Path.GetTempPath(), $"mixed-execution-proof-{Guid.NewGuid():N}");
        Directory.CreateDirectory(workspace);

        try
        {
            var loweredMethods = BuildLoweredMethods(assemblyPath);
            var runtimeManager = new RuntimeManager();
            var addTwentyOneMethod = GetRequiredMethod(loweredMethods, "AddTwentyOne");
            var subjectId = addTwentyOneMethod.SubjectId;
            var interpreterInvoker = new ManagedInterpreterExecutor(
                methodResolver: subject => loweredMethods[subject]).CreateInt32UnaryInvoker(addTwentyOneMethod);
            var aotBridgeMethod = GetRequiredMethod(loweredMethods, "CallAotBridgeAdd");
            var localCallMethod = GetRequiredMethod(loweredMethods, "CallLocalAdd");
            var instanceCallMethod = GetRequiredMethod(loweredMethods, "CallInstanceAddOne");
            var realCatchMethod = GetRequiredMethod(loweredMethods, "DivideOrCatch");
            var realRethrowMethod = GetRequiredMethod(loweredMethods, "DivideOrRethrow");
            var realLeaveFinallyMethod = GetRequiredMethod(loweredMethods, "AddWithFinally");
            Console.WriteLine($"mixed-aot-to-interpreter-before-load={runtimeManager.DispatchInt32Unary(subjectId, 21, static value => value + 1)}");

            var packageRoot = CreatePackageRoot(workspace, assemblyPath);
            var package = PackageReader.ReadFromDirectory(packageRoot);
            runtimeManager.LoadPackage(
                package,
                CurrentAotVersion,
                new Dictionary<string, int>(StringComparer.Ordinal),
                new Dictionary<string, Func<int, int>>(StringComparer.Ordinal)
                {
                    [subjectId] = interpreterInvoker,
                });

            Console.WriteLine($"mixed-aot-to-interpreter={runtimeManager.DispatchInt32Unary(subjectId, 21, static value => value + 1)}");

            var dispatcher = new BridgeDispatcher();
            dispatcher.ApplyPlan(new BridgePlan
            {
                AotToHotUpdate = [],
                HotUpdateToAot =
                [
                    new HotUpdateToAotBridgeEntry
                    {
                        BridgeId = AotBridgeSubjectId,
                        AotSubjectId = AotBridgeSubjectId,
                    },
                ],
                HotUpdateToEngine =
                [
                    new HotUpdateToEngineBridgeEntry
                    {
                        BridgeId = EngineBridgeId,
                        EngineSubjectId = EngineBridgeSubjectId,
                    },
                ],
                DelegateWrappers = [],
            });
            dispatcher.RegisterAotInt32BinaryTarget(AotBridgeSubjectId, static (left, right) => left + right);
            dispatcher.RegisterEngineInt32UnaryTarget(EngineBridgeSubjectId, static value => value + 5);

            var bridgeExecutor = new ManagedInterpreterExecutor((bridgeId, bridgeArguments) =>
            {
                if (string.Equals(bridgeId, AotBridgeSubjectId, StringComparison.Ordinal))
                {
                    if (bridgeArguments.Count != 2)
                    {
                        throw new InvalidOperationException($"bridge '{bridgeId}' expects 2 arguments.");
                    }

                    return dispatcher.InvokeHotUpdateToAotInt32(
                        bridgeId,
                        Convert.ToInt32(bridgeArguments[0]),
                        Convert.ToInt32(bridgeArguments[1]));
                }

                if (string.Equals(bridgeId, EngineBridgeId, StringComparison.Ordinal))
                {
                    if (bridgeArguments.Count != 1)
                    {
                        throw new InvalidOperationException($"bridge '{bridgeId}' expects 1 argument.");
                    }

                    return dispatcher.InvokeHotUpdateToEngineInt32(bridgeId, Convert.ToInt32(bridgeArguments[0]));
                }

                if (string.Equals(bridgeId, "System.Private.CoreLib/System.String::get_Length()", StringComparison.Ordinal))
                {
                    if (bridgeArguments.Count != 1 || bridgeArguments[0] is not string instance)
                    {
                        var receiver = bridgeArguments.Count == 0 ? null : bridgeArguments[0];
                        throw new InvalidOperationException(
                            $"bridge '{bridgeId}' expects 1 string receiver but received count={bridgeArguments.Count}, type={receiver?.GetType().FullName ?? "<null>"}, value={receiver ?? "<null>"}.");
                    }

                    return instance.Length;
                }

                throw new InvalidOperationException($"unsupported bridge '{bridgeId}'.");
            },
            methodResolver: subjectKey => loweredMethods[subjectKey]);

            Console.WriteLine($"mixed-interpreter-to-aot={bridgeExecutor.ExecuteInt32(aotBridgeMethod, new int[] { 10, 20 })}");
            Console.WriteLine($"mixed-interpreter-to-aot-ops={GetOpSequence(aotBridgeMethod)}");
            Console.WriteLine($"mixed-interpreter-to-aot-target={GetCallTarget(aotBridgeMethod, IROpCode.CallBridge)}");
            Console.WriteLine($"mixed-interpreter-local-call={bridgeExecutor.ExecuteInt32(localCallMethod, new int[] { 21, 21 })}");
            Console.WriteLine($"mixed-interpreter-local-call-ops={GetOpSequence(localCallMethod)}");
            var instanceReceiver = CreateInstanceArithmeticReceiver(assemblyPath);
            Console.WriteLine($"mixed-interpreter-instance-call={bridgeExecutor.ExecuteInt32(instanceCallMethod, [instanceReceiver, 41])}");
            Console.WriteLine($"mixed-interpreter-instance-call-ops={GetOpSequence(instanceCallMethod)}");
            Console.WriteLine($"mixed-interpreter-instance-call-target={GetCallTarget(instanceCallMethod, IROpCode.CallVirt)}");
            var stringBridgeMethod = CreateStringLengthBridgeMethod();
            Console.WriteLine($"mixed-interpreter-string-bridge={bridgeExecutor.ExecuteInt32(stringBridgeMethod, Array.Empty<int>())}");
            Console.WriteLine($"mixed-interpreter-string-bridge-ops={GetOpSequence(stringBridgeMethod)}");
            Console.WriteLine($"mixed-interpreter-real-catch={bridgeExecutor.ExecuteInt32(realCatchMethod, new int[] { 0 })}");
            Console.WriteLine($"mixed-interpreter-real-catch-region-kind={GetSingleRegionKind(realCatchMethod)}");
            Console.WriteLine($"mixed-interpreter-real-rethrow-caught={RunRealInterpreterRethrowProof(bridgeExecutor, realRethrowMethod)}");
            Console.WriteLine($"mixed-interpreter-real-rethrow-region-kind={GetSingleRegionKind(realRethrowMethod)}");
            Console.WriteLine($"mixed-interpreter-real-leave-finally={bridgeExecutor.ExecuteInt32(realLeaveFinallyMethod, Array.Empty<int>())}");
            Console.WriteLine($"mixed-interpreter-real-leave-finally-region-kind={GetSingleRegionKind(realLeaveFinallyMethod)}");
            Console.WriteLine($"mixed-interpreter-real-leave-finally-opcodes={GetOpSequence(realLeaveFinallyMethod, IROpCode.Leave, IROpCode.EndFinally)}");
            Console.WriteLine($"mixed-interpreter-to-engine={bridgeExecutor.ExecuteInt32(CreateCallEngineBridgeMethod(), Array.Empty<int>())}");
            Console.WriteLine($"mixed-interpreter-throw-caught={RunInterpreterThrowCatchProof()}");
            Console.WriteLine($"mixed-interpreter-leave-finally={RunInterpreterLeaveFinallyProof()}");
            Console.WriteLine($"mixed-interpreter-rethrow-caught={RunInterpreterRethrowCatchProof()}");

            runtimeManager.UnloadPackage();
            Console.WriteLine($"mixed-aot-to-interpreter-after-unload={runtimeManager.DispatchInt32Unary(subjectId, 21, static value => value + 1)}");
            return 0;
        }
        finally
        {
            if (Directory.Exists(workspace))
            {
                Directory.Delete(workspace, recursive: true);
            }
        }
    }

    private static IReadOnlyDictionary<string, IRMethod> BuildLoweredMethods(string assemblyPath)
    {
        var loader = new LoaderStage();
        var loadedAssembly = loader.Load(new Chaos.IL2CPP.Contracts.ManagedClosureRequest(
            assemblyPath,
            Path.Combine(Path.GetTempPath(), "chaos-il2cpp", "mixed-execution-proof")));
        var lowering = new ILToIRLowering();
        var selectedNames = new HashSet<string>(StringComparer.Ordinal)
        {
            "Add",
            "AddOne",
            "AddTwentyOne",
            "CallAotBridgeAdd",
            "CallLocalAdd",
            "CallInstanceAddOne",
            "DivideOrCatch",
            "DivideOrRethrow",
            "AddWithFinally",
        };
        return loadedAssembly.Methods
            .Where(method => selectedNames.Contains(method.Name))
            .ToDictionary(method => method.SubjectId, method => lowering.Lower(method), StringComparer.Ordinal);
    }

    private static string CreatePackageRoot(string workspaceRoot, string assemblyPath)
    {
        var packageRoot = Path.Combine(workspaceRoot, "package");
        Directory.CreateDirectory(packageRoot);

        var assemblyBytes = File.ReadAllBytes(assemblyPath);
        var assemblyName = "HotPatch.dll";
        File.WriteAllBytes(Path.Combine(packageRoot, assemblyName), assemblyBytes);

        var metadataName = "metadata-supplement.bin";
        File.WriteAllBytes(Path.Combine(packageRoot, metadataName), [0x07, 0x08, 0x09]);

        var manifest = new HotUpdatePackage
        {
            PackageId = "com.example.mixed-execution-proof",
            TargetAotVersion = CurrentAotVersion,
            Assemblies =
            [
                new HotUpdateAssemblyEntry
                {
                    Name = assemblyName,
                    Hash = PackageReader.ComputeFileHash(assemblyBytes),
                    Size = assemblyBytes.Length,
                    EntryPoint = "InterpreterArithmeticProof/Program::Main()",
                },
            ],
            SupplementalMetadata = metadataName,
            Signature = "signed-proof",
        };

        File.WriteAllText(
            Path.Combine(packageRoot, PackageReader.ManifestFileName),
            JsonSerializer.Serialize(manifest, new JsonSerializerOptions { WriteIndented = true }));
        return packageRoot;
    }

    private static string RunInterpreterThrowCatchProof()
    {
        try
        {
            _ = new ManagedInterpreterExecutor().ExecuteInt32(CreateThrowMethod(), Array.Empty<int>());
            return "missing";
        }
        catch (InvalidOperationException exception) when (string.Equals(exception.Message, "mixed-proof-throw", StringComparison.Ordinal))
        {
            return "ok";
        }
    }

    private static string RunInterpreterLeaveFinallyProof()
    {
        var result = new ManagedInterpreterExecutor().ExecuteInt32(CreateLeaveFinallyMethod(), Array.Empty<int>());
        return result == 42 ? "ok" : $"unexpected:{result}";
    }

    private static string RunInterpreterRethrowCatchProof()
    {
        try
        {
            _ = new ManagedInterpreterExecutor().ExecuteInt32(CreateRethrowMethod(), Array.Empty<int>());
            return "missing";
        }
        catch (InvalidOperationException exception) when (string.Equals(exception.Message, "mixed-proof-rethrow", StringComparison.Ordinal))
        {
            return "ok";
        }
    }

    private static string RunRealInterpreterRethrowProof(ManagedInterpreterExecutor executor, IRMethod method)
    {
        try
        {
            _ = executor.ExecuteInt32(method, new int[] { 0 });
            return "missing";
        }
        catch (DivideByZeroException)
        {
            return "ok";
        }
    }

    private static IRMethod CreateThrowMethod()
    {
        return new IRMethod
        {
            MethodId = "mixed-throw",
            SubjectId = "MixedExecutionProof/Interpreter::Throw()",
            Blocks =
            [
                new IRBasicBlock
                {
                    BlockId = 0,
                    Instructions =
                    [
                        new IRInstruction
                        {
                            OpCode = IROpCode.Throw,
                            Operands =
                            [
                                new IROperand
                                {
                                    Kind = IROperandKind.StringLiteral,
                                    TypeTag = IRTypeTag.String,
                                    StringValue = "mixed-proof-throw",
                                },
                            ],
                        },
                    ],
                },
            ],
        };
    }

    private static IRMethod CreateLeaveFinallyMethod()
    {
        return new IRMethod
        {
            MethodId = "mixed-leave-finally",
            SubjectId = "MixedExecutionProof/Interpreter::LeaveFinally()",
            Blocks =
            [
                new IRBasicBlock
                {
                    BlockId = 0,
                    Instructions =
                    [
                        new IRInstruction
                        {
                            OpCode = IROpCode.LdcI4,
                            Operands =
                            [
                                new IROperand
                                {
                                    Kind = IROperandKind.Int32Literal,
                                    TypeTag = IRTypeTag.Int32,
                                    Int32Value = 40,
                                },
                            ],
                            Result = new IROperand
                            {
                                Kind = IROperandKind.Local,
                                TypeTag = IRTypeTag.Int32,
                                Symbol = "stack0",
                            },
                        },
                        new IRInstruction
                        {
                            OpCode = IROpCode.StLoc,
                            Operands =
                            [
                                new IROperand
                                {
                                    Kind = IROperandKind.Local,
                                    TypeTag = IRTypeTag.Int32,
                                    Symbol = "stack0",
                                },
                                new IROperand
                                {
                                    Kind = IROperandKind.Local,
                                    TypeTag = IRTypeTag.Int32,
                                    Symbol = "local0",
                                },
                            ],
                        },
                        new IRInstruction
                        {
                            OpCode = IROpCode.Leave,
                            Operands =
                            [
                                new IROperand
                                {
                                    Kind = IROperandKind.BasicBlock,
                                    TypeTag = IRTypeTag.Void,
                                    BasicBlockId = 2,
                                },
                                new IROperand
                                {
                                    Kind = IROperandKind.BasicBlock,
                                    TypeTag = IRTypeTag.Void,
                                    BasicBlockId = 1,
                                },
                            ],
                        },
                    ],
                },
                new IRBasicBlock
                {
                    BlockId = 1,
                    Instructions =
                    [
                        new IRInstruction
                        {
                            OpCode = IROpCode.LdLoc,
                            Operands =
                            [
                                new IROperand
                                {
                                    Kind = IROperandKind.Local,
                                    TypeTag = IRTypeTag.Int32,
                                    Symbol = "local0",
                                },
                            ],
                            Result = new IROperand
                            {
                                Kind = IROperandKind.Local,
                                TypeTag = IRTypeTag.Int32,
                                Symbol = "stack1",
                            },
                        },
                        new IRInstruction
                        {
                            OpCode = IROpCode.LdcI4,
                            Operands =
                            [
                                new IROperand
                                {
                                    Kind = IROperandKind.Int32Literal,
                                    TypeTag = IRTypeTag.Int32,
                                    Int32Value = 2,
                                },
                            ],
                            Result = new IROperand
                            {
                                Kind = IROperandKind.Local,
                                TypeTag = IRTypeTag.Int32,
                                Symbol = "stack2",
                            },
                        },
                        new IRInstruction
                        {
                            OpCode = IROpCode.Add,
                            Operands =
                            [
                                new IROperand
                                {
                                    Kind = IROperandKind.Local,
                                    TypeTag = IRTypeTag.Int32,
                                    Symbol = "stack1",
                                },
                                new IROperand
                                {
                                    Kind = IROperandKind.Local,
                                    TypeTag = IRTypeTag.Int32,
                                    Symbol = "stack2",
                                },
                            ],
                            Result = new IROperand
                            {
                                Kind = IROperandKind.Local,
                                TypeTag = IRTypeTag.Int32,
                                Symbol = "stack3",
                            },
                        },
                        new IRInstruction
                        {
                            OpCode = IROpCode.StLoc,
                            Operands =
                            [
                                new IROperand
                                {
                                    Kind = IROperandKind.Local,
                                    TypeTag = IRTypeTag.Int32,
                                    Symbol = "stack3",
                                },
                                new IROperand
                                {
                                    Kind = IROperandKind.Local,
                                    TypeTag = IRTypeTag.Int32,
                                    Symbol = "local0",
                                },
                            ],
                        },
                        new IRInstruction
                        {
                            OpCode = IROpCode.EndFinally,
                        },
                    ],
                },
                new IRBasicBlock
                {
                    BlockId = 2,
                    Instructions =
                    [
                        new IRInstruction
                        {
                            OpCode = IROpCode.LdLoc,
                            Operands =
                            [
                                new IROperand
                                {
                                    Kind = IROperandKind.Local,
                                    TypeTag = IRTypeTag.Int32,
                                    Symbol = "local0",
                                },
                            ],
                            Result = new IROperand
                            {
                                Kind = IROperandKind.Local,
                                TypeTag = IRTypeTag.Int32,
                                Symbol = "stack4",
                            },
                        },
                        new IRInstruction
                        {
                            OpCode = IROpCode.Ret,
                            Operands =
                            [
                                new IROperand
                                {
                                    Kind = IROperandKind.Local,
                                    TypeTag = IRTypeTag.Int32,
                                    Symbol = "stack4",
                                },
                            ],
                        },
                    ],
                },
            ],
        };
    }

    private static object CreateInstanceArithmeticReceiver(string assemblyPath)
    {
        var assembly = Assembly.LoadFrom(assemblyPath);
        var receiverType = assembly.GetType("InterpreterArithmeticProof.InstanceArithmetic", throwOnError: true, ignoreCase: false)
            ?? throw new InvalidOperationException("failed to load InstanceArithmetic receiver type.");
        return Activator.CreateInstance(receiverType)
            ?? throw new InvalidOperationException("failed to create InstanceArithmetic receiver instance.");
    }

    private static IRMethod CreateStringLengthBridgeMethod()
    {
        return new IRMethod
        {
            MethodId = "mixed-string-bridge",
            SubjectId = "MixedExecutionProof/Interpreter::StringBridge()",
            Blocks =
            [
                new IRBasicBlock
                {
                    BlockId = 0,
                    Instructions =
                    [
                        new IRInstruction
                        {
                            OpCode = IROpCode.LdStr,
                            Operands =
                            [
                                new IROperand
                                {
                                    Kind = IROperandKind.StringLiteral,
                                    TypeTag = IRTypeTag.String,
                                    StringValue = "chaos",
                                },
                            ],
                            Result = new IROperand
                            {
                                Kind = IROperandKind.Local,
                                TypeTag = IRTypeTag.String,
                                Symbol = "stack0",
                            },
                        },
                        new IRInstruction
                        {
                            OpCode = IROpCode.CallBridge,
                            Operands =
                            [
                                new IROperand
                                {
                                    Kind = IROperandKind.Method,
                                    TypeTag = IRTypeTag.Int32,
                                    Symbol = "System.Private.CoreLib/System.String::get_Length()",
                                },
                                new IROperand
                                {
                                    Kind = IROperandKind.Local,
                                    TypeTag = IRTypeTag.String,
                                    Symbol = "stack0",
                                },
                            ],
                            Result = new IROperand
                            {
                                Kind = IROperandKind.Local,
                                TypeTag = IRTypeTag.Int32,
                                Symbol = "stack1",
                            },
                        },
                        new IRInstruction
                        {
                            OpCode = IROpCode.Ret,
                            Operands =
                            [
                                new IROperand
                                {
                                    Kind = IROperandKind.Local,
                                    TypeTag = IRTypeTag.Int32,
                                    Symbol = "stack1",
                                },
                            ],
                        },
                    ],
                },
            ],
        };
    }

    private static IRMethod CreateRethrowMethod()
    {
        return new IRMethod
        {
            MethodId = "mixed-rethrow",
            SubjectId = "MixedExecutionProof/Interpreter::Rethrow()",
            Blocks =
            [
                new IRBasicBlock
                {
                    BlockId = 0,
                    Instructions =
                    [
                        new IRInstruction
                        {
                            OpCode = IROpCode.Throw,
                            Operands =
                            [
                                new IROperand
                                {
                                    Kind = IROperandKind.StringLiteral,
                                    TypeTag = IRTypeTag.String,
                                    StringValue = "mixed-proof-rethrow",
                                },
                            ],
                        },
                    ],
                },
                new IRBasicBlock
                {
                    BlockId = 1,
                    Instructions =
                    [
                        new IRInstruction
                        {
                            OpCode = IROpCode.Rethrow,
                        },
                    ],
                },
            ],
            ExceptionRegions =
            [
                new IRExceptionRegion
                {
                    Kind = IRExceptionRegionKind.Catch,
                    TryBlockIds = [0],
                    HandlerBlockId = 1,
                },
            ],
        };
    }

    private static string GetOpSequence(IRMethod method)
    {
        return string.Join(",", method.Blocks.SelectMany(static block => block.Instructions).Select(static instruction => instruction.OpCode switch
        {
            IROpCode.LdArg => "ldarg",
            IROpCode.Call => "call",
            IROpCode.CallBridge => "callbridge",
            IROpCode.Ret => "ret",
            _ => instruction.OpCode.ToString().ToLowerInvariant(),
        }));
    }

    private static string GetOpSequence(IRMethod method, params IROpCode[] opCodes)
    {
        var selected = new HashSet<IROpCode>(opCodes);
        return string.Join(",", method.Blocks
            .SelectMany(static block => block.Instructions)
            .Where(instruction => selected.Contains(instruction.OpCode))
            .Select(static instruction => instruction.OpCode.ToString().ToLowerInvariant()));
    }

    private static string GetCallTarget(IRMethod method, IROpCode opCode)
    {
        return method.Blocks
            .SelectMany(static block => block.Instructions)
            .Single(instruction => instruction.OpCode == opCode)
            .Operands[0]
            .Symbol
            ?? string.Empty;
    }

    private static string GetSingleRegionKind(IRMethod method)
    {
        return method.ExceptionRegions.Single().Kind.ToString().ToLowerInvariant();
    }

    private static IRMethod GetRequiredMethod(IReadOnlyDictionary<string, IRMethod> methods, string methodName)
    {
        return methods.Values.Single(method =>
            string.Equals(method.SubjectId.Split("::", StringSplitOptions.None)[1].Split('(')[0], methodName, StringComparison.Ordinal));
    }

    private static IRMethod CreateCallEngineBridgeMethod()
    {
        return new IRMethod
        {
            MethodId = "mixed-call-engine-bridge",
            SubjectId = "MixedExecutionProof/Interpreter::CallEngineBridge()",
            Blocks =
            [
                new IRBasicBlock
                {
                    BlockId = 0,
                    Instructions =
                    [
                        new IRInstruction
                        {
                            OpCode = IROpCode.LdcI4,
                            Operands =
                            [
                                new IROperand
                                {
                                    Kind = IROperandKind.Int32Literal,
                                    TypeTag = IRTypeTag.Int32,
                                    Int32Value = 2,
                                },
                            ],
                            Result = new IROperand
                            {
                                Kind = IROperandKind.Local,
                                TypeTag = IRTypeTag.Int32,
                                Symbol = "stack0",
                            },
                        },
                        new IRInstruction
                        {
                            OpCode = IROpCode.CallBridge,
                            Operands =
                            [
                                new IROperand
                                {
                                    Kind = IROperandKind.Method,
                                    TypeTag = IRTypeTag.Int32,
                                    Symbol = EngineBridgeId,
                                },
                                new IROperand
                                {
                                    Kind = IROperandKind.Local,
                                    TypeTag = IRTypeTag.Int32,
                                    Symbol = "stack0",
                                },
                            ],
                            Result = new IROperand
                            {
                                Kind = IROperandKind.Local,
                                TypeTag = IRTypeTag.Int32,
                                Symbol = "stack1",
                            },
                        },
                        new IRInstruction
                        {
                            OpCode = IROpCode.Ret,
                            Operands =
                            [
                                new IROperand
                                {
                                    Kind = IROperandKind.Local,
                                    TypeTag = IRTypeTag.Int32,
                                    Symbol = "stack1",
                                },
                            ],
                        },
                    ],
                },
            ],
        };
    }
}
