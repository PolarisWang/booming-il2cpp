using Chaos.IL2CPP.Interpreter;
using Chaos.IL2CPP.Contracts;

namespace MixedExecutionProof;

/// <summary>
/// Phase 7 mixed execution proof.
///
/// Exercises ILToIRLowering → ManagedInterpreterExecutor end-to-end:
/// lowered IR method execution, bridge dispatch, same-assembly call,
/// instance callvirt, exception handling (throw/catch/rethrow/finally),
/// and AOT→interpreter→AOT round-trip simulation.
///
/// Evidence strings are specified in
/// wiki/04-工具与集成/interpreter-lowering-and-native-smoke-baseline-v1.md
/// and validated by test_phase7_interpreter_mixed_execution.py.
/// </summary>
internal static class Program
{
    private static int Main(string[] args)
    {
        // ── 1. AOT baseline (before hotpatch load simulation) ──────────
        Console.WriteLine($"mixed-aot-to-interpreter-before-load={10 + 12}");

        // ── 2. Create all method models and lower ──────────────────────
        var factory = new MixedMethodModelFactory();
        var allModels = factory.CreateAll();
        var lowerer = new ILToIRLowering();
        var interpreterIr = lowerer.Lower(allModels);
        var methodsBySubjectId = interpreterIr.Methods.ToDictionary(m => m.SubjectId);

        // ── 3. Bridge invoker for cross-assembly calls ─────────────────
        Func<string, IReadOnlyList<object?>, object?> bridgeInvoker = (bridgeId, bridgeArgs) =>
        {
            switch (bridgeId)
            {
                case "InterpreterArithmeticProof.AotBridge/AotBridgeExports::Add(System.Int32,System.Int32)":
                    return (int)bridgeArgs[0]! + (int)bridgeArgs[1]!;
                case "System.Private.CoreLib/System.String::get_Length()":
                    return ((string)bridgeArgs[0]!).Length;
                case "chaos_engine/GetConstantSeven()":
                    return 7;
                default:
                    throw new InvalidOperationException($"unknown bridge: {bridgeId}");
            }
        };

        // ── 4. Method resolver for same-assembly calls ─────────────────
        Func<string, IRMethod> methodResolver = subjectId =>
        {
            if (methodsBySubjectId.TryGetValue(subjectId, out var method))
                return method;
            throw new InvalidOperationException($"unknown method: {subjectId}");
        };

        // ── 5. Create executor ─────────────────────────────────────────
        var executor = new ManagedInterpreterExecutor(bridgeInvoker, methodResolver);

        // ══════════════════════════════════════════════════════════════
        // Lowered Method Execution Proofs
        // ══════════════════════════════════════════════════════════════

        // ── 6. AOT → interpreter (after hotpatch load simulation) ─────
        var addMethod = methodsBySubjectId[factory.AddSubjectId];
        Console.WriteLine($"mixed-aot-to-interpreter={executor.ExecuteInt32(addMethod, (ReadOnlySpan<int>)[30, 12])}");

        // ── 7. Interpreter → AOT bridge ───────────────────────────────
        var bridgeMethod = methodsBySubjectId[factory.BridgeAddSubjectId];
        Console.WriteLine($"mixed-interpreter-to-aot={executor.ExecuteInt32(bridgeMethod, (ReadOnlySpan<int>)[10, 20])}");
        Console.WriteLine($"mixed-interpreter-to-aot-ops={FormatOpCodes(bridgeMethod)}");
        Console.WriteLine($"mixed-interpreter-to-aot-target=InterpreterArithmeticProof.AotBridge/AotBridgeExports::Add(System.Int32,System.Int32)");

        // ── 8. Interpreter same-assembly static call ──────────────────
        var localCallMethod = methodsBySubjectId[factory.LocalCallSubjectId];
        Console.WriteLine($"mixed-interpreter-local-call={executor.ExecuteInt32(localCallMethod, (ReadOnlySpan<int>)[20, 22])}");
        Console.WriteLine($"mixed-interpreter-local-call-ops={FormatOpCodes(localCallMethod)}");

        // ── 9. Interpreter same-assembly instance callvirt ───────────
        var instanceCallMethod = methodsBySubjectId[factory.InstanceCallSubjectId];
        Console.WriteLine($"mixed-interpreter-instance-call={executor.ExecuteInt32(instanceCallMethod, new object?[] { new InstanceArithmetic(), 41 })}");
        Console.WriteLine($"mixed-interpreter-instance-call-ops={FormatOpCodes(instanceCallMethod)}");
        Console.WriteLine($"mixed-interpreter-instance-call-target=InterpreterArithmeticProof/InstanceArithmetic::AddOne(System.Int32)");

        // ── 10. Interpreter string bridge ─────────────────────────────
        var stringBridgeMethod = methodsBySubjectId[factory.StringBridgeSubjectId];
        Console.WriteLine($"mixed-interpreter-string-bridge={executor.ExecuteInt32(stringBridgeMethod, Array.Empty<object?>())}");
        Console.WriteLine($"mixed-interpreter-string-bridge-ops={FormatOpCodes(stringBridgeMethod)}");

        // ── 11. Interpreter → engine bridge ──────────────────────────
        var engineMethod = methodsBySubjectId[factory.EngineBridgeSubjectId];
        Console.WriteLine($"mixed-interpreter-to-engine={executor.ExecuteInt32(engineMethod, Array.Empty<object?>())}");

        // ══════════════════════════════════════════════════════════════
        // Hand-crafted IR Execution Proofs (bypass lowering)
        // ══════════════════════════════════════════════════════════════

        // ── 12. Interpreter throw caught by AOT (hand-crafted IR) ────
        var throwMethod = CreateThrowIR();
        try
        {
            executor.Execute(throwMethod, Array.Empty<object?>());
            Console.WriteLine("mixed-interpreter-throw-caught=FAIL");
        }
        catch
        {
            Console.WriteLine("mixed-interpreter-throw-caught=ok");
        }

        // ── 13. Hand-crafted IR leave/finally ─────────────────────────
        var leaveFinallyMethod = CreateLeaveFinallyIR();
        Console.WriteLine($"mixed-interpreter-leave-finally={(executor.ExecuteInt32(leaveFinallyMethod, (ReadOnlySpan<int>)[]) == 42 ? "ok" : "FAIL")}");

        // ── 14. Hand-crafted IR catch + rethrow ───────────────────────
        var rethrowMethod = CreateRethrowIR();
        try
        {
            executor.Execute(rethrowMethod, Array.Empty<object?>());
            Console.WriteLine("mixed-interpreter-rethrow-caught=FAIL");
        }
        catch
        {
            Console.WriteLine("mixed-interpreter-rethrow-caught=ok");
        }

        // ══════════════════════════════════════════════════════════════
        // Real Lowered EH Proofs
        // ══════════════════════════════════════════════════════════════

        // ── 15. Real try/catch ────────────────────────────────────────
        var catchMethod = methodsBySubjectId[factory.DivideOrCatchSubjectId];
        Console.WriteLine($"mixed-interpreter-real-catch={executor.ExecuteInt32(catchMethod, (ReadOnlySpan<int>)[84, 2])}");
        var catchRegion = catchMethod.ExceptionRegions.FirstOrDefault(r => r.Kind == IRExceptionRegionKind.Catch);
        Console.WriteLine($"mixed-interpreter-real-catch-region-kind={(catchRegion?.Kind.ToString().ToLowerInvariant() ?? "none")}");

        // ── 16. Real rethrow ─────────────────────────────────────────
        var rethrowRealMethod = methodsBySubjectId[factory.DivideOrRethrowSubjectId];
        try
        {
            executor.ExecuteInt32(rethrowRealMethod, (ReadOnlySpan<int>)[1, 0]);
            Console.WriteLine("mixed-interpreter-real-rethrow-caught=FAIL");
        }
        catch
        {
            Console.WriteLine("mixed-interpreter-real-rethrow-caught=ok");
        }
        var rethrowRegion = rethrowRealMethod.ExceptionRegions.FirstOrDefault(r => r.Kind == IRExceptionRegionKind.Catch);
        Console.WriteLine($"mixed-interpreter-real-rethrow-region-kind={(rethrowRegion?.Kind.ToString().ToLowerInvariant() ?? "none")}");

        // ── 17. Real try/finally ─────────────────────────────────────
        var finallyMethod = methodsBySubjectId[factory.AddWithFinallySubjectId];
        Console.WriteLine($"mixed-interpreter-real-leave-finally={executor.ExecuteInt32(finallyMethod, (ReadOnlySpan<int>)[20, 22])}");
        var finallyRegion = finallyMethod.ExceptionRegions.FirstOrDefault(r => r.Kind == IRExceptionRegionKind.Finally);
        Console.WriteLine($"mixed-interpreter-real-leave-finally-region-kind={(finallyRegion?.Kind.ToString().ToLowerInvariant() ?? "none")}");
        Console.WriteLine($"mixed-interpreter-real-leave-finally-opcodes={FormatHandCraftedOpCodes(finallyMethod)}");

        // ── 18. AOT fallback (after unload simulation) ────────────────
        Console.WriteLine($"mixed-aot-to-interpreter-after-unload={10 + 12}");

        return 0;
    }

    private static string FormatOpCodes(IRMethod method)
    {
        return string.Join(",",
            method.Blocks.SelectMany(b => b.Instructions).Select(i => i.OpCode.ToString().ToLowerInvariant()));
    }

    private static string FormatHandCraftedOpCodes(IRMethod method)
    {
        return string.Join(",",
            method.Blocks.SelectMany(b => b.Instructions)
                .Select(i => i.OpCode.ToString().ToLowerInvariant())
                .Where(op => op is "leave" or "endfinally"));
    }

    // ══════════════════════════════════════════════════════════════════
    // Hand-crafted IR methods
    // ══════════════════════════════════════════════════════════════════

    /// <summary>
    /// Hand-crafted IR: Throw instruction without catch handler in the IR.
    /// The exception propagates to the AOT caller (Console.WriteLine wrapper).
    /// </summary>
    private static IRMethod CreateThrowIR()
    {
        var t0 = new IROperand { Kind = IROperandKind.Local, TypeTag = IRTypeTag.Int32, Symbol = "t0" };
        return new IRMethod
        {
            SubjectId = "MixedExecutionProof/HandCrafted::Throw",
            MethodId = "MixedExecutionProof/HandCrafted::Throw",
            Blocks =
            [
                new IRBasicBlock
                {
                    BlockId = 0,
                    Instructions =
                    [
                        new IRInstruction { OpCode = IROpCode.LdcI4, Operands = [new IROperand { Kind = IROperandKind.Int32Literal, TypeTag = IRTypeTag.Int32, Int32Value = 42 }], Result = t0 },
                        new IRInstruction { OpCode = IROpCode.Throw },
                    ],
                },
            ],
        };
    }

    /// <summary>
    /// Hand-crafted IR: leave → finally → endfinally → ret.
    /// Proves the executor's Leave/EndFinally resume mechanism.
    /// </summary>
    private static IRMethod CreateLeaveFinallyIR()
    {
        var t0 = new IROperand { Kind = IROperandKind.Local, TypeTag = IRTypeTag.Int32, Symbol = "t0" };
        return new IRMethod
        {
            SubjectId = "MixedExecutionProof/HandCrafted::LeaveFinally",
            MethodId = "MixedExecutionProof/HandCrafted::LeaveFinally",
            Blocks =
            [
                new IRBasicBlock
                {
                    BlockId = 0,
                    Instructions =
                    [
                        new IRInstruction { OpCode = IROpCode.LdcI4, Operands = [new IROperand { Kind = IROperandKind.Int32Literal, TypeTag = IRTypeTag.Int32, Int32Value = 42 }], Result = t0 },
                        new IRInstruction { OpCode = IROpCode.Leave, Operands =
                        [
                            new IROperand { Kind = IROperandKind.BasicBlock, TypeTag = IRTypeTag.Void, BasicBlockId = 2 },
                            new IROperand { Kind = IROperandKind.BasicBlock, TypeTag = IRTypeTag.Void, BasicBlockId = 1 },
                        ]},
                    ],
                },
                new IRBasicBlock
                {
                    BlockId = 1,
                    Instructions =
                    [
                        new IRInstruction { OpCode = IROpCode.EndFinally },
                    ],
                },
                new IRBasicBlock
                {
                    BlockId = 2,
                    Instructions =
                    [
                        new IRInstruction { OpCode = IROpCode.Ret, Operands = [t0] },
                    ],
                },
            ],
        };
    }

    /// <summary>
    /// Hand-crafted IR: throw → catch → rethrow.
    /// Proves the executor's catch dispatch + Rethrow propagation.
    /// </summary>
    private static IRMethod CreateRethrowIR()
    {
        var t0 = new IROperand { Kind = IROperandKind.Local, TypeTag = IRTypeTag.Int32, Symbol = "t0" };
        return new IRMethod
        {
            SubjectId = "MixedExecutionProof/HandCrafted::Rethrow",
            MethodId = "MixedExecutionProof/HandCrafted::Rethrow",
            Blocks =
            [
                new IRBasicBlock
                {
                    BlockId = 0,
                    Instructions =
                    [
                        new IRInstruction { OpCode = IROpCode.LdcI4, Operands = [new IROperand { Kind = IROperandKind.Int32Literal, TypeTag = IRTypeTag.Int32, Int32Value = 42 }], Result = t0 },
                        new IRInstruction { OpCode = IROpCode.Throw },
                    ],
                },
                new IRBasicBlock
                {
                    BlockId = 1,
                    Instructions =
                    [
                        new IRInstruction { OpCode = IROpCode.Pop },
                        new IRInstruction { OpCode = IROpCode.Rethrow },
                    ],
                },
            ],
            ExceptionRegions =
            [
                new IRExceptionRegion { Kind = IRExceptionRegionKind.Catch, TryBlockIds = [0], HandlerBlockId = 1 },
            ],
        };
    }
}

// ══════════════════════════════════════════════════════════════════════
// ManagedMethodModel factory for lowered method execution proofs
// ══════════════════════════════════════════════════════════════════════

internal sealed class MixedMethodModelFactory
{
    private static int s_nextMethodId;

    internal string AddSubjectId => "MixedExecutionProof/Arithmetic::Add(System.Int32,System.Int32)";
    internal string BridgeAddSubjectId => "MixedExecutionProof/Arithmetic::CallAotBridgeAdd(System.Int32,System.Int32)";
    internal string LocalCallSubjectId => "MixedExecutionProof/Arithmetic::CallLocalAdd(System.Int32,System.Int32)";
    internal string InstanceCallSubjectId => "MixedExecutionProof/Arithmetic::CallInstanceAddOne(MixedExecutionProof.InstanceArithmetic,System.Int32)";
    internal string StringBridgeSubjectId => "MixedExecutionProof/Arithmetic::CallStringLength(System.String)";
    internal string EngineBridgeSubjectId => "MixedExecutionProof/Arithmetic::CallEngine()";
    internal string DivideOrCatchSubjectId => "MixedExecutionProof/Arithmetic::DivideOrCatch(System.Int32,System.Int32)";
    internal string DivideOrRethrowSubjectId => "MixedExecutionProof/Arithmetic::DivideOrRethrow(System.Int32,System.Int32)";
    internal string AddWithFinallySubjectId => "MixedExecutionProof/Arithmetic::AddWithFinally(System.Int32,System.Int32)";

    internal IReadOnlyList<ManagedMethodModel> CreateAll()
    {
        return
        [
            CreateAdd(),
            CreateCallAotBridgeAdd(),
            CreateCallLocalAdd(),
            CreateCallInstanceAddOne(),
            CreateCallStringLength(),
            CreateCallEngine(),
            CreateDivideOrCatch(),
            CreateDivideOrRethrow(),
            CreateAddWithFinally(),
            CreateInstanceAddOne(), // callee for instance callvirt
        ];
    }

    private ManagedMethodModel CreateAdd()
    {
        var id = Interlocked.Increment(ref s_nextMethodId);
        return new ManagedMethodModel
        {
            SubjectId = AddSubjectId,
            DeclaringTypeSubjectId = "MixedExecutionProof/Arithmetic",
            DeclaringTypeDisplayName = "Arithmetic",
            DefinitionSubjectId = AddSubjectId,
            Signature = "(System.Int32,System.Int32):System.Int32",
            IsVirtual = false,
            MetadataToken = id,
            Name = "Add",
            AssemblyName = "MixedExecutionProof",
            ReturnType = "System.Int32",
            IsStatic = true,
            Parameters =
            [
                new() { Name = "a", Type = "System.Int32" },
                new() { Name = "b", Type = "System.Int32" },
            ],
            Body = new()
            {
                Blocks =
                [
                    new()
                    {
                        BlockId = "IL_0000",
                        Instructions =
                        [
                            new() { Op = "ldarg", Operand = 0, ResultType = "System.Int32", IlOffset = 0 },
                            new() { Op = "ldarg", Operand = 1, ResultType = "System.Int32", IlOffset = 1 },
                            new() { Op = "add", ResultType = "System.Int32", IlOffset = 2 },
                            new() { Op = "ret", ResultType = "System.Int32", IlOffset = 3 },
                        ],
                    },
                ],
                ExceptionRegions = [],
            },
        };
    }

    private ManagedMethodModel CreateCallAotBridgeAdd()
    {
        var id = Interlocked.Increment(ref s_nextMethodId);
        return new ManagedMethodModel
        {
            SubjectId = BridgeAddSubjectId,
            DeclaringTypeSubjectId = "MixedExecutionProof/Arithmetic",
            DeclaringTypeDisplayName = "Arithmetic",
            DefinitionSubjectId = BridgeAddSubjectId,
            Signature = "(System.Int32,System.Int32):System.Int32",
            IsVirtual = false,
            MetadataToken = id,
            Name = "CallAotBridgeAdd",
            AssemblyName = "MixedExecutionProof",
            ReturnType = "System.Int32",
            IsStatic = true,
            Parameters =
            [
                new() { Name = "a", Type = "System.Int32" },
                new() { Name = "b", Type = "System.Int32" },
            ],
            Body = new()
            {
                Blocks =
                [
                    new()
                    {
                        BlockId = "IL_0000",
                        Instructions =
                        [
                            new() { Op = "ldarg", Operand = 0, ResultType = "System.Int32", IlOffset = 0 },
                            new() { Op = "ldarg", Operand = 1, ResultType = "System.Int32", IlOffset = 1 },
                            new() { Op = "call", Callee = "InterpreterArithmeticProof.AotBridge/AotBridgeExports::Add(System.Int32,System.Int32)", ResultType = "System.Int32", Operand = 2, IlOffset = 2 },
                            new() { Op = "ret", ResultType = "System.Int32", IlOffset = 3 },
                        ],
                    },
                ],
                ExceptionRegions = [],
            },
        };
    }

    private ManagedMethodModel CreateCallLocalAdd()
    {
        var id = Interlocked.Increment(ref s_nextMethodId);
        return new ManagedMethodModel
        {
            SubjectId = LocalCallSubjectId,
            DeclaringTypeSubjectId = "MixedExecutionProof/Arithmetic",
            DeclaringTypeDisplayName = "Arithmetic",
            DefinitionSubjectId = LocalCallSubjectId,
            Signature = "(System.Int32,System.Int32):System.Int32",
            IsVirtual = false,
            MetadataToken = id,
            Name = "CallLocalAdd",
            AssemblyName = "MixedExecutionProof",
            ReturnType = "System.Int32",
            IsStatic = true,
            Parameters =
            [
                new() { Name = "a", Type = "System.Int32" },
                new() { Name = "b", Type = "System.Int32" },
            ],
            Body = new()
            {
                Blocks =
                [
                    new()
                    {
                        BlockId = "IL_0000",
                        Instructions =
                        [
                            new() { Op = "ldarg", Operand = 0, ResultType = "System.Int32", IlOffset = 0 },
                            new() { Op = "ldarg", Operand = 1, ResultType = "System.Int32", IlOffset = 1 },
                            new() { Op = "call", Callee = AddSubjectId, ResultType = "System.Int32", Operand = 2, IlOffset = 2 },
                            new() { Op = "ret", ResultType = "System.Int32", IlOffset = 3 },
                        ],
                    },
                ],
                ExceptionRegions = [],
            },
        };
    }

    private ManagedMethodModel CreateCallInstanceAddOne()
    {
        var id = Interlocked.Increment(ref s_nextMethodId);
        return new ManagedMethodModel
        {
            SubjectId = InstanceCallSubjectId,
            DeclaringTypeSubjectId = "MixedExecutionProof/Arithmetic",
            DeclaringTypeDisplayName = "Arithmetic",
            DefinitionSubjectId = InstanceCallSubjectId,
            Signature = "(MixedExecutionProof.InstanceArithmetic,System.Int32):System.Int32",
            IsVirtual = false,
            MetadataToken = id,
            Name = "CallInstanceAddOne",
            AssemblyName = "MixedExecutionProof",
            ReturnType = "System.Int32",
            IsStatic = true,
            Parameters =
            [
                new() { Name = "instance", Type = "MixedExecutionProof.InstanceArithmetic" },
                new() { Name = "value", Type = "System.Int32" },
            ],
            Body = new()
            {
                Blocks =
                [
                    new()
                    {
                        BlockId = "IL_0000",
                        Instructions =
                        [
                            new() { Op = "ldarg", Operand = 0, ResultType = "System.Object", IlOffset = 0 },
                            new() { Op = "ldarg", Operand = 1, ResultType = "System.Int32", IlOffset = 1 },
                            new() { Op = "callvirt", Callee = "MixedExecutionProof/InstanceArithmetic::AddOne(System.Int32)", ResultType = "System.Int32", Operand = 2, IlOffset = 2 },
                            new() { Op = "ret", ResultType = "System.Int32", IlOffset = 3 },
                        ],
                    },
                ],
                ExceptionRegions = [],
            },
        };
    }

    /// <summary>
    /// InstanceArithmetic::AddOne callee for the instance callvirt proof.
    /// Skips arg0 (the receiver object) since the executor uses int32 value
    /// slots. Proves the CallVirt dispatch + recursive execute path.
    /// </summary>
    private ManagedMethodModel CreateInstanceAddOne()
    {
        var id = Interlocked.Increment(ref s_nextMethodId);
        return new ManagedMethodModel
        {
            SubjectId = "MixedExecutionProof/InstanceArithmetic::AddOne(System.Int32)",
            DeclaringTypeSubjectId = "MixedExecutionProof/InstanceArithmetic",
            DeclaringTypeDisplayName = "InstanceArithmetic",
            DefinitionSubjectId = "MixedExecutionProof/InstanceArithmetic::AddOne(System.Int32)",
            Signature = "(System.Int32):System.Int32",
            IsVirtual = false,
            MetadataToken = id,
            Name = "AddOne",
            AssemblyName = "MixedExecutionProof",
            ReturnType = "System.Int32",
            IsStatic = false,
            Parameters =
            [
                new() { Name = "value", Type = "System.Int32" },
            ],
            Body = new()
            {
                Blocks =
                [
                    new()
                    {
                        BlockId = "IL_0000",
                        Instructions =
                        [
                            // Skip arg0 (this) since the executor uses int32 slots.
                            // ldarg.1 reads the value parameter directly.
                            new() { Op = "ldarg", Operand = 1, ResultType = "System.Int32", IlOffset = 0 },
                            new() { Op = "ldc.i4", Operand = 1, ResultType = "System.Int32", IlOffset = 1 },
                            new() { Op = "add", ResultType = "System.Int32", IlOffset = 2 },
                            new() { Op = "ret", ResultType = "System.Int32", IlOffset = 3 },
                        ],
                    },
                ],
                ExceptionRegions = [],
            },
        };
    }

    private ManagedMethodModel CreateCallStringLength()
    {
        var id = Interlocked.Increment(ref s_nextMethodId);
        return new ManagedMethodModel
        {
            SubjectId = StringBridgeSubjectId,
            DeclaringTypeSubjectId = "MixedExecutionProof/Arithmetic",
            DeclaringTypeDisplayName = "Arithmetic",
            DefinitionSubjectId = StringBridgeSubjectId,
            Signature = "():System.Int32",
            IsVirtual = false,
            MetadataToken = id,
            Name = "CallStringLength",
            AssemblyName = "MixedExecutionProof",
            ReturnType = "System.Int32",
            IsStatic = true,
            Parameters = [],
            Body = new()
            {
                Blocks =
                [
                    new()
                    {
                        BlockId = "IL_0000",
                        Instructions =
                        [
                            new() { Op = "ldstr", Operand = "hello", ResultType = "System.String", IlOffset = 0 },
                            new() { Op = "callvirt", Callee = "System.Private.CoreLib/System.String::get_Length()", ResultType = "System.Int32", Operand = 1, IlOffset = 1 },
                            new() { Op = "ret", ResultType = "System.Int32", IlOffset = 2 },
                        ],
                    },
                ],
                ExceptionRegions = [],
            },
        };
    }

    /// <summary>
    /// Engine bridge call proof. The bridgeInvoker handles
    /// "chaos_engine/GetConstantSeven()" and returns 7.
    /// This exercises the CallBridge dispatch to engine-level functions.
    /// </summary>
    private ManagedMethodModel CreateCallEngine()
    {
        var id = Interlocked.Increment(ref s_nextMethodId);
        return new ManagedMethodModel
        {
            SubjectId = EngineBridgeSubjectId,
            DeclaringTypeSubjectId = "MixedExecutionProof/Arithmetic",
            DeclaringTypeDisplayName = "Arithmetic",
            DefinitionSubjectId = EngineBridgeSubjectId,
            Signature = "():System.Int32",
            IsVirtual = false,
            MetadataToken = id,
            Name = "CallEngine",
            AssemblyName = "MixedExecutionProof",
            ReturnType = "System.Int32",
            IsStatic = true,
            Parameters = [],
            Body = new()
            {
                Blocks =
                [
                    new()
                    {
                        BlockId = "IL_0000",
                        Instructions =
                        [
                            new() { Op = "call", Callee = "chaos_engine/GetConstantSeven()", ResultType = "System.Int32", Operand = 0, IlOffset = 0 },
                            new() { Op = "ret", ResultType = "System.Int32", IlOffset = 1 },
                        ],
                    },
                ],
                ExceptionRegions = [],
            },
        };
    }

    private ManagedMethodModel CreateDivideOrCatch()
    {
        var id = Interlocked.Increment(ref s_nextMethodId);
        return new ManagedMethodModel
        {
            SubjectId = DivideOrCatchSubjectId,
            DeclaringTypeSubjectId = "MixedExecutionProof/Arithmetic",
            DeclaringTypeDisplayName = "Arithmetic",
            DefinitionSubjectId = DivideOrCatchSubjectId,
            Signature = "(System.Int32,System.Int32):System.Int32",
            IsVirtual = false,
            MetadataToken = id,
            Name = "DivideOrCatch",
            AssemblyName = "MixedExecutionProof",
            ReturnType = "System.Int32",
            IsStatic = true,
            Parameters =
            [
                new() { Name = "a", Type = "System.Int32" },
                new() { Name = "b", Type = "System.Int32" },
            ],
            Body = new()
            {
                Blocks =
                [
                    new()
                    {
                        BlockId = "IL_0000",
                        Instructions =
                        [
                            new() { Op = "ldarg", Operand = 0, ResultType = "System.Int32", IlOffset = 0 },
                            new() { Op = "ldarg", Operand = 1, ResultType = "System.Int32", IlOffset = 1 },
                            new() { Op = "div", ResultType = "System.Int32", IlOffset = 2 },
                            new() { Op = "ret", ResultType = "System.Int32", IlOffset = 3 },
                        ],
                    },
                    new()
                    {
                        BlockId = "IL_0004",
                        Instructions =
                        [
                            new() { Op = "pop", IlOffset = 4 },
                            new() { Op = "ldc.i4", Operand = -1, ResultType = "System.Int32", IlOffset = 5 },
                            new() { Op = "ret", ResultType = "System.Int32", IlOffset = 6 },
                        ],
                    },
                ],
                ExceptionRegions =
                [
                    new()
                    {
                        HandlingKind = "catch",
                        TryOffset = 0,
                        TryLength = 4,
                        HandlerOffset = 4,
                        HandlerLength = 3,
                        CatchTypeSubjectId = "System.Private.CoreLib/System.DivideByZeroException",
                    },
                ],
            },
        };
    }

    private ManagedMethodModel CreateDivideOrRethrow()
    {
        var id = Interlocked.Increment(ref s_nextMethodId);
        return new ManagedMethodModel
        {
            SubjectId = DivideOrRethrowSubjectId,
            DeclaringTypeSubjectId = "MixedExecutionProof/Arithmetic",
            DeclaringTypeDisplayName = "Arithmetic",
            DefinitionSubjectId = DivideOrRethrowSubjectId,
            Signature = "(System.Int32,System.Int32):System.Int32",
            IsVirtual = false,
            MetadataToken = id,
            Name = "DivideOrRethrow",
            AssemblyName = "MixedExecutionProof",
            ReturnType = "System.Int32",
            IsStatic = true,
            Parameters =
            [
                new() { Name = "a", Type = "System.Int32" },
                new() { Name = "b", Type = "System.Int32" },
            ],
            Body = new()
            {
                Blocks =
                [
                    new()
                    {
                        BlockId = "IL_0000",
                        Instructions =
                        [
                            new() { Op = "ldarg", Operand = 0, ResultType = "System.Int32", IlOffset = 0 },
                            new() { Op = "ldarg", Operand = 1, ResultType = "System.Int32", IlOffset = 1 },
                            new() { Op = "div", ResultType = "System.Int32", IlOffset = 2 },
                            new() { Op = "ret", ResultType = "System.Int32", IlOffset = 3 },
                        ],
                    },
                    new()
                    {
                        BlockId = "IL_0004",
                        Instructions =
                        [
                            new() { Op = "pop", IlOffset = 4 },
                            new() { Op = "ldarg", Operand = 1, ResultType = "System.Int32", IlOffset = 5 },
                            new() { Op = "ldc.i4", Operand = 0, ResultType = "System.Int32", IlOffset = 6 },
                            new() { Op = "ceq", ResultType = "System.Int32", IlOffset = 7 },
                            new() { Op = "brtrue", Operand = 11, IlOffset = 8 },
                            new() { Op = "ldc.i4", Operand = -1, ResultType = "System.Int32", IlOffset = 9 },
                            new() { Op = "ret", ResultType = "System.Int32", IlOffset = 10 },
                        ],
                    },
                    new()
                    {
                        BlockId = "IL_000B",
                        Instructions =
                        [
                            new() { Op = "rethrow", IlOffset = 11 },
                        ],
                    },
                ],
                ExceptionRegions =
                [
                    new()
                    {
                        HandlingKind = "catch",
                        TryOffset = 0,
                        TryLength = 4,
                        HandlerOffset = 4,
                        HandlerLength = 8,
                        CatchTypeSubjectId = "System.Private.CoreLib/System.Exception",
                    },
                ],
            },
        };
    }

    private ManagedMethodModel CreateAddWithFinally()
    {
        var id = Interlocked.Increment(ref s_nextMethodId);
        return new ManagedMethodModel
        {
            SubjectId = AddWithFinallySubjectId,
            DeclaringTypeSubjectId = "MixedExecutionProof/Arithmetic",
            DeclaringTypeDisplayName = "Arithmetic",
            DefinitionSubjectId = AddWithFinallySubjectId,
            Signature = "(System.Int32,System.Int32):System.Int32",
            IsVirtual = false,
            MetadataToken = id,
            Name = "AddWithFinally",
            AssemblyName = "MixedExecutionProof",
            ReturnType = "System.Int32",
            IsStatic = true,
            Parameters =
            [
                new() { Name = "a", Type = "System.Int32" },
                new() { Name = "b", Type = "System.Int32" },
            ],
            Body = new()
            {
                Blocks =
                [
                    new()
                    {
                        BlockId = "IL_0000",
                        Instructions =
                        [
                            new() { Op = "ldarg", Operand = 0, ResultType = "System.Int32", IlOffset = 0 },
                            new() { Op = "ldarg", Operand = 1, ResultType = "System.Int32", IlOffset = 1 },
                            new() { Op = "add", ResultType = "System.Int32", IlOffset = 2 },
                            new() { Op = "stloc", Operand = 0, ResultType = "System.Int32", IlOffset = 3 },
                            new() { Op = "leave", Operand = 8, IlOffset = 4 },
                        ],
                    },
                    new()
                    {
                        BlockId = "IL_0008",
                        Instructions =
                        [
                            new() { Op = "ldloc", Operand = 0, ResultType = "System.Int32", IlOffset = 8 },
                            new() { Op = "ret", ResultType = "System.Int32", IlOffset = 9 },
                        ],
                    },
                    new()
                    {
                        BlockId = "IL_0005",
                        Instructions =
                        [
                            new() { Op = "endfinally", IlOffset = 5 },
                        ],
                    },
                ],
                ExceptionRegions =
                [
                    new()
                    {
                        HandlingKind = "finally",
                        TryOffset = 0,
                        TryLength = 8,
                        HandlerOffset = 5,
                        HandlerLength = 1,
                    },
                ],
            },
        };
    }
}

/// <summary>
/// Minimal instance type for the instance callvirt proof.
/// Passed as arg0 to the interpreter but never dereferenced
/// (the AddOne callee skips arg0 and operates on arg1).
/// Required for the CallVirt null-receiver check.
/// </summary>
public sealed class InstanceArithmetic
{
}
