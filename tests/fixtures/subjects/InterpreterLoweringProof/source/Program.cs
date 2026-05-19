using Chaos.IL2CPP.Interpreter;

namespace InterpreterLoweringProof;

/// <summary>
/// Phase 7 interpreter lowering proof.
///
/// Uses ILToIRLowering to lower methods from InterpreterArithmeticProof
/// and prints IR structure evidence. When run through the pipeline, the
/// output demonstrates that the lowering pipeline produces correct IR
/// for each proof path.
/// </summary>
internal static class Program
{
    private static int Main(string[] args)
    {
        // ── Arithmetic method: Add(int, int) ─────────────────────────────
        // Expected lowering: ldarg, ldarg, add, ret → IR: LdArg, LdArg, Add, Ret
        var addMethod = ManagedMethodModelFactory.CreateAdd();
        var lowerer = new ILToIRLowering();
        var irMethod = lowerer.Lower(addMethod);
        var addOps = FormatOpCodes(irMethod);
        Console.WriteLine($"InterpreterLoweringProof entry reached.");
        Console.WriteLine($"args={args.Length}");
        Console.WriteLine($"call-ops={addOps}");
        Console.WriteLine($"first-block-instructions={irMethod.Blocks[0].Instructions.Count}");
        Console.WriteLine($"method-subject={irMethod.SubjectId}");

        // ── Cross-assembly bridge call ───────────────────────────────────
        // Expected: CallBridge dispatch for external method
        var bridgeMethod = ManagedMethodModelFactory.CreateCallAotBridgeAdd();
        var bridgeIr = lowerer.Lower(bridgeMethod);
        var bridgeOps = FormatOpCodes(bridgeIr);
        Console.WriteLine($"bridge-call-ops={bridgeOps}");
        var hasCallBridge = bridgeIr.Blocks[0].Instructions
            .Any(i => i.OpCode == IROpCode.CallBridge);
        Console.WriteLine($"bridge-has-callbridge={(hasCallBridge ? "yes" : "no")}");

        // ── Same-assembly static call ────────────────────────────────────
        var localCallMethod = ManagedMethodModelFactory.CreateCallLocalAdd();
        var localCallIr = lowerer.Lower(localCallMethod);
        var localCallOps = FormatOpCodes(localCallIr);
        Console.WriteLine($"local-call-ops={localCallOps}");
        var hasCall = localCallIr.Blocks[0].Instructions
            .Any(i => i.OpCode == IROpCode.Call);
        Console.WriteLine($"local-call-has-call={(hasCall ? "yes" : "no")}");

        // ── External callvirt (String.get_Length) ────────────────────────
        var stringLengthMethod = ManagedMethodModelFactory.CreateCallStringLength();
        var stringLengthIr = lowerer.Lower(stringLengthMethod);
        var stringLengthOps = FormatOpCodes(stringLengthIr);
        Console.WriteLine($"callvirt-ops={stringLengthOps}");
        var targetMethod = stringLengthIr.Blocks[0].Instructions
            .FirstOrDefault(i => i.OpCode == IROpCode.CallBridge
                              || i.OpCode == IROpCode.CallVirt
                              || i.OpCode == IROpCode.Call);
        if (targetMethod?.Operands.Count > 0)
        {
            Console.WriteLine($"callvirt-target={targetMethod.Operands[0].Symbol}");
        }

        // ── Same-assembly instance callvirt ──────────────────────────────
        var instanceMethod = ManagedMethodModelFactory.CreateCallInstanceAddOne();
        var instanceIr = lowerer.Lower(instanceMethod);
        var instanceOps = FormatOpCodes(instanceIr);
        Console.WriteLine($"instance-call-ops={instanceOps}");
        var callVirtInstr = instanceIr.Blocks[0].Instructions
            .FirstOrDefault(i => i.OpCode == IROpCode.CallVirt);
        if (callVirtInstr?.Operands.Count > 0)
        {
            Console.WriteLine($"instance-call-target={callVirtInstr.Operands[0].Symbol}");
        }

        // ── try/catch ────────────────────────────────────────────────────
        var catchMethod = ManagedMethodModelFactory.CreateDivideOrCatch();
        var catchIr = lowerer.Lower(catchMethod);
        var hasCatchRegion = catchIr.ExceptionRegions.Any(r => r.Kind == IRExceptionRegionKind.Catch);
        Console.WriteLine($"has-catch-region={(hasCatchRegion ? "yes" : "no")}");
        Console.WriteLine($"exception-region-count={catchIr.ExceptionRegions.Count}");

        // ── try/finally ──────────────────────────────────────────────────
        var finallyMethod = ManagedMethodModelFactory.CreateAddWithFinally();
        var finallyIr = lowerer.Lower(finallyMethod);
        var hasFinallyRegion = finallyIr.ExceptionRegions.Any(r => r.Kind == IRExceptionRegionKind.Finally);
        Console.WriteLine($"has-finally-region={(hasFinallyRegion ? "yes" : "no")}");

        return 0;
    }

    private static string FormatOpCodes(IRMethod method)
    {
        return string.Join(",",
            method.Blocks.SelectMany(b => b.Instructions).Select(i => i.OpCode.ToString().ToLowerInvariant()));
    }
}

/// <summary>
/// Creates synthetic ManagedMethodModel instances for lowering proof.
/// In production, these come from the LoaderStage. Here we use hand-crafted
/// models to exercise specific lowering paths without requiring the full
/// loader pipeline.
/// </summary>
internal static class ManagedMethodModelFactory
{
    private static int s_nextMethodId;

    public static Chaos.IL2CPP.Contracts.ManagedMethodModel CreateAdd()
    {
        var id = System.Threading.Interlocked.Increment(ref s_nextMethodId);
        return new Chaos.IL2CPP.Contracts.ManagedMethodModel
        {
            SubjectId = $"InterpreterArithmeticProof/ArithmeticProof::Add(System.Int32,System.Int32)",
            DefinitionSubjectId = $"InterpreterArithmeticProof/ArithmeticProof::Add(System.Int32,System.Int32)",
            Signature = "(System.Int32,System.Int32):System.Int32",
            Name = "Add",
            AssemblyName = "InterpreterArithmeticProof",
            DeclaringTypeSubjectId = "InterpreterArithmeticProof/ArithmeticProof",
            DeclaringTypeDisplayName = "ArithmeticProof",
            ReturnType = "System.Int32",
            IsStatic = true,
            IsVirtual = false,
            MetadataToken = id,
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

    public static Chaos.IL2CPP.Contracts.ManagedMethodModel CreateCallAotBridgeAdd()
    {
        var id = System.Threading.Interlocked.Increment(ref s_nextMethodId);
        return new Chaos.IL2CPP.Contracts.ManagedMethodModel
        {
            SubjectId = $"InterpreterArithmeticProof/ArithmeticProof::CallAotBridgeAdd(System.Int32,System.Int32)",
            DefinitionSubjectId = $"InterpreterArithmeticProof/ArithmeticProof::CallAotBridgeAdd(System.Int32,System.Int32)",
            Signature = "(System.Int32,System.Int32):System.Int32",
            Name = "CallAotBridgeAdd",
            AssemblyName = "InterpreterArithmeticProof",
            DeclaringTypeSubjectId = "InterpreterArithmeticProof/ArithmeticProof",
            DeclaringTypeDisplayName = "ArithmeticProof",
            ReturnType = "System.Int32",
            IsStatic = true,
            IsVirtual = false,
            MetadataToken = id,
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
                            new() { Op = "call", Callee = "InterpreterArithmeticProof.AotBridge/AotBridgeExports::Add(System.Int32,System.Int32)",
                                    ResultType = "System.Int32", Operand = 2, IlOffset = 2 },
                            new() { Op = "ret", ResultType = "System.Int32", IlOffset = 3 },
                        ],
                    },
                ],
                ExceptionRegions = [],
            },
        };
    }

    public static Chaos.IL2CPP.Contracts.ManagedMethodModel CreateCallLocalAdd()
    {
        var id = System.Threading.Interlocked.Increment(ref s_nextMethodId);
        return new Chaos.IL2CPP.Contracts.ManagedMethodModel
        {
            SubjectId = $"InterpreterArithmeticProof/ArithmeticProof::CallLocalAdd(System.Int32,System.Int32)",
            DefinitionSubjectId = $"InterpreterArithmeticProof/ArithmeticProof::CallLocalAdd(System.Int32,System.Int32)",
            Signature = "(System.Int32,System.Int32):System.Int32",
            Name = "CallLocalAdd",
            AssemblyName = "InterpreterArithmeticProof",
            DeclaringTypeSubjectId = "InterpreterArithmeticProof/ArithmeticProof",
            DeclaringTypeDisplayName = "ArithmeticProof",
            ReturnType = "System.Int32",
            IsStatic = true,
            IsVirtual = false,
            MetadataToken = id,
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
                            new() { Op = "call", Callee = "InterpreterArithmeticProof/ArithmeticProof::Add(System.Int32,System.Int32)",
                                    ResultType = "System.Int32", Operand = 2, IlOffset = 2 },
                            new() { Op = "ret", ResultType = "System.Int32", IlOffset = 3 },
                        ],
                    },
                ],
                ExceptionRegions = [],
            },
        };
    }

    public static Chaos.IL2CPP.Contracts.ManagedMethodModel CreateCallStringLength()
    {
        var id = System.Threading.Interlocked.Increment(ref s_nextMethodId);
        return new Chaos.IL2CPP.Contracts.ManagedMethodModel
        {
            SubjectId = $"InterpreterArithmeticProof/ArithmeticProof::CallStringLength(System.String)",
            DefinitionSubjectId = $"InterpreterArithmeticProof/ArithmeticProof::CallStringLength(System.String)",
            Signature = "(System.String):System.Int32",
            Name = "CallStringLength",
            AssemblyName = "InterpreterArithmeticProof",
            DeclaringTypeSubjectId = "InterpreterArithmeticProof/ArithmeticProof",
            DeclaringTypeDisplayName = "ArithmeticProof",
            ReturnType = "System.Int32",
            IsStatic = true,
            IsVirtual = false,
            MetadataToken = id,
            Parameters =
            [
                new() { Name = "value", Type = "System.String" },
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
                            new() { Op = "ldarg", Operand = 0, ResultType = "System.String", IlOffset = 0 },
                            new() { Op = "callvirt", Callee = "System.Private.CoreLib/System.String::get_Length()",
                                    ResultType = "System.Int32", Operand = 1, IlOffset = 1 },
                            new() { Op = "ret", ResultType = "System.Int32", IlOffset = 2 },
                        ],
                    },
                ],
                ExceptionRegions = [],
            },
        };
    }

    public static Chaos.IL2CPP.Contracts.ManagedMethodModel CreateCallInstanceAddOne()
    {
        var id = System.Threading.Interlocked.Increment(ref s_nextMethodId);
        return new Chaos.IL2CPP.Contracts.ManagedMethodModel
        {
            SubjectId = $"InterpreterArithmeticProof/ArithmeticProof::CallInstanceAddOne(InstanceArithmetic,System.Int32)",
            DefinitionSubjectId = $"InterpreterArithmeticProof/ArithmeticProof::CallInstanceAddOne(InstanceArithmetic,System.Int32)",
            Signature = "(InterpreterArithmeticProof.InstanceArithmetic,System.Int32):System.Int32",
            Name = "CallInstanceAddOne",
            AssemblyName = "InterpreterArithmeticProof",
            DeclaringTypeSubjectId = "InterpreterArithmeticProof/ArithmeticProof",
            DeclaringTypeDisplayName = "ArithmeticProof",
            ReturnType = "System.Int32",
            IsStatic = true,
            IsVirtual = false,
            MetadataToken = id,
            Parameters =
            [
                new() { Name = "instance", Type = "InterpreterArithmeticProof.InstanceArithmetic" },
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
                            new() { Op = "callvirt", Callee = "InterpreterArithmeticProof/InstanceArithmetic::AddOne(System.Int32)",
                                    ResultType = "System.Int32", Operand = 2, IlOffset = 2 },
                            new() { Op = "ret", ResultType = "System.Int32", IlOffset = 3 },
                        ],
                    },
                ],
                ExceptionRegions = [],
            },
        };
    }

    public static Chaos.IL2CPP.Contracts.ManagedMethodModel CreateDivideOrCatch()
    {
        var id = System.Threading.Interlocked.Increment(ref s_nextMethodId);
        return new Chaos.IL2CPP.Contracts.ManagedMethodModel
        {
            SubjectId = $"InterpreterArithmeticProof/ArithmeticProof::DivideOrCatch(System.Int32,System.Int32)",
            DefinitionSubjectId = $"InterpreterArithmeticProof/ArithmeticProof::DivideOrCatch(System.Int32,System.Int32)",
            Signature = "(System.Int32,System.Int32):System.Int32",
            Name = "DivideOrCatch",
            AssemblyName = "InterpreterArithmeticProof",
            DeclaringTypeSubjectId = "InterpreterArithmeticProof/ArithmeticProof",
            DeclaringTypeDisplayName = "ArithmeticProof",
            ReturnType = "System.Int32",
            IsStatic = true,
            IsVirtual = false,
            MetadataToken = id,
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

    public static Chaos.IL2CPP.Contracts.ManagedMethodModel CreateAddWithFinally()
    {
        var id = System.Threading.Interlocked.Increment(ref s_nextMethodId);
        return new Chaos.IL2CPP.Contracts.ManagedMethodModel
        {
            SubjectId = $"InterpreterArithmeticProof/ArithmeticProof::AddWithFinally(System.Int32,System.Int32)",
            DefinitionSubjectId = $"InterpreterArithmeticProof/ArithmeticProof::AddWithFinally(System.Int32,System.Int32)",
            Signature = "(System.Int32,System.Int32):System.Int32",
            Name = "AddWithFinally",
            AssemblyName = "InterpreterArithmeticProof",
            DeclaringTypeSubjectId = "InterpreterArithmeticProof/ArithmeticProof",
            DeclaringTypeDisplayName = "ArithmeticProof",
            ReturnType = "System.Int32",
            IsStatic = true,
            IsVirtual = false,
            MetadataToken = id,
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
