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
            var beforeLoad = runtimeManager.DispatchInt32Unary(subjectId, 21, static value => value + 1);
            Assert.Equal(22, beforeLoad);

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

            var afterLoad = runtimeManager.DispatchInt32Unary(subjectId, 21, static value => value + 1);
            Assert.Equal(42, afterLoad);

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

            var interpreterToAot = bridgeExecutor.ExecuteInt32(aotBridgeMethod, new int[] { 10, 20 });
            var interpreterToAotOps = GetOpSequence(aotBridgeMethod);
            var interpreterToAotTarget = GetCallTarget(aotBridgeMethod, IROpCode.CallBridge);
            var localCall = bridgeExecutor.ExecuteInt32(localCallMethod, new int[] { 21, 21 });
            var localCallOps = GetOpSequence(localCallMethod);
            Assert.Equal(30, interpreterToAot);
            Assert.Equal("ldarg,ldarg,callbridge,ret", interpreterToAotOps);
            Assert.Equal(AotBridgeSubjectId, interpreterToAotTarget);
            Assert.Equal(42, localCall);
            Assert.Equal("ldarg,ldarg,call,ret", localCallOps);
            var instanceReceiver = CreateInstanceArithmeticReceiver(assemblyPath);
            var instanceCall = bridgeExecutor.ExecuteInt32(instanceCallMethod, [instanceReceiver, 41]);
            var instanceCallOps = GetOpSequence(instanceCallMethod);
            var instanceCallTarget = GetCallTarget(instanceCallMethod, IROpCode.CallVirt);
            Assert.Equal(42, instanceCall);
            Assert.Equal("ldarg,ldarg,callvirt,ret", instanceCallOps);
            Assert.Equal("InterpreterArithmeticProof/InstanceArithmetic::AddOne(System.Int32)", instanceCallTarget);
            var stringBridgeMethod = CreateStringLengthBridgeMethod();
            var stringBridge = bridgeExecutor.ExecuteInt32(stringBridgeMethod, Array.Empty<int>());
            var stringBridgeOps = GetOpSequence(stringBridgeMethod);
            var realCatch = bridgeExecutor.ExecuteInt32(realCatchMethod, new int[] { 0 });
            var realCatchRegionKind = GetSingleRegionKind(realCatchMethod);
            var realRethrowCaught = RunRealInterpreterRethrowProof(bridgeExecutor, realRethrowMethod);
            var realRethrowRegionKind = GetSingleRegionKind(realRethrowMethod);
            var realLeaveFinally = bridgeExecutor.ExecuteInt32(realLeaveFinallyMethod, Array.Empty<int>());
            var realLeaveFinallyRegionKind = GetSingleRegionKind(realLeaveFinallyMethod);
            var realLeaveFinallyOpcodes = GetOpSequence(realLeaveFinallyMethod, IROpCode.Leave, IROpCode.EndFinally);
            var toEngine = bridgeExecutor.ExecuteInt32(CreateCallEngineBridgeMethod(), Array.Empty<int>());
            var throwCaught = RunInterpreterThrowCatchProof();
            var leaveFinally = RunInterpreterLeaveFinallyProof();
            var rethrowCaught = RunInterpreterRethrowCatchProof();
            Assert.Equal(5, stringBridge);
            Assert.Equal("ldstr,callbridge,ret", stringBridgeOps);
            Assert.Equal(42, realCatch);
            Assert.Equal("catch", realCatchRegionKind);
            Assert.Equal("ok", realRethrowCaught);
            Assert.Equal("catch", realRethrowRegionKind);
            Assert.Equal(42, realLeaveFinally);
            Assert.Equal("finally", realLeaveFinallyRegionKind);
            Assert.Equal("leave,endfinally", realLeaveFinallyOpcodes);
            Assert.Equal(7, toEngine);
            Assert.Equal("ok", throwCaught);
            Assert.Equal("ok", leaveFinally);
            Assert.Equal("ok", rethrowCaught);

            runtimeManager.UnloadPackage();
            var afterUnload = runtimeManager.DispatchInt32Unary(subjectId, 21, static value => value + 1);
            Assert.Equal(22, afterUnload);
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
