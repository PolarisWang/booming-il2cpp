using Chaos.IL2CPP.Contracts;
using Chaos.IL2CPP.Interpreter;
using Chaos.IL2CPP.Loader;
using Chaos.TestFramework;

namespace MixedExecutionFeaturePack;

internal static class InterpreterLoweringProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "interpreter-lowering-proof",
        Evidence = ChaosEvidenceKind.Stdout,
        Priority = 2)]
    public static int Run()
    {
        var assemblyPath = InterpreterArithmeticSupport.ResolveAssemblyPath();

        var loader = new LoaderStage();
        var assembly = loader.Load(new ManagedClosureRequest(
            assemblyPath,
            Path.Combine(Path.GetTempPath(), "chaos-il2cpp", "interpreter-lowering-proof")));
        var method = assembly.Methods.Single(static method =>
            string.Equals(method.DeclaringTypeDisplayName, "NativeExports", StringComparison.Ordinal) &&
            string.Equals(method.Name, "Add", StringComparison.Ordinal) &&
            method.Parameters.Count == 2 &&
            method.Parameters.All(static parameter => string.Equals(parameter.Type, "System.Int32", StringComparison.Ordinal)));

        var irMethod = new ILToIRLowering().Lower(method);
        var instructions = irMethod.Blocks.SelectMany(static block => block.Instructions).ToList();
        var addInstruction = instructions.Single(static instruction => instruction.OpCode == IROpCode.Add);
        var opSequence = string.Join(",", instructions.Select(static instruction => GetDisplayOp(instruction.OpCode)));
        var callMethod = assembly.Methods.Single(static method =>
            string.Equals(method.DeclaringTypeDisplayName, "NativeExports", StringComparison.Ordinal) &&
            string.Equals(method.Name, "CallAotBridgeAdd", StringComparison.Ordinal) &&
            method.Parameters.Count == 2 &&
            method.Parameters.All(static parameter => string.Equals(parameter.Type, "System.Int32", StringComparison.Ordinal)));
        var callIrMethod = new ILToIRLowering().Lower(callMethod);
        var callInstructions = callIrMethod.Blocks.SelectMany(static block => block.Instructions).ToList();
        var callInstruction = callInstructions.Single(static instruction => instruction.OpCode == IROpCode.CallBridge);
        var callOpSequence = string.Join(",", callInstructions.Select(static instruction => GetDisplayOp(instruction.OpCode)));
        var localCallMethod = assembly.Methods.Single(static method =>
            string.Equals(method.DeclaringTypeDisplayName, "NativeExports", StringComparison.Ordinal) &&
            string.Equals(method.Name, "CallLocalAdd", StringComparison.Ordinal) &&
            method.Parameters.Count == 2 &&
            method.Parameters.All(static parameter => string.Equals(parameter.Type, "System.Int32", StringComparison.Ordinal)));
        var localCallIrMethod = new ILToIRLowering().Lower(localCallMethod);
        var localCallInstructions = localCallIrMethod.Blocks.SelectMany(static block => block.Instructions).ToList();
        var localCallInstruction = localCallInstructions.Single(static instruction => instruction.OpCode == IROpCode.Call);
        var localCallOpSequence = string.Join(",", localCallInstructions.Select(static instruction => GetDisplayOp(instruction.OpCode)));
        var callVirtMethod = assembly.Methods.Single(static method =>
            string.Equals(method.DeclaringTypeDisplayName, "NativeExports", StringComparison.Ordinal) &&
            string.Equals(method.Name, "CallStringLength", StringComparison.Ordinal) &&
            method.Parameters.Count == 0);
        var callVirtIrMethod = new ILToIRLowering().Lower(callVirtMethod);
        var callVirtInstructions = callVirtIrMethod.Blocks.SelectMany(static block => block.Instructions).ToList();
        var callVirtInstruction = callVirtInstructions.Single(static instruction => instruction.OpCode == IROpCode.CallBridge);
        var callVirtOpSequence = string.Join(",", callVirtInstructions.Select(static instruction => GetDisplayOp(instruction.OpCode)));
        var instanceCallMethod = assembly.Methods.Single(static method =>
            string.Equals(method.DeclaringTypeDisplayName, "NativeExports", StringComparison.Ordinal) &&
            string.Equals(method.Name, "CallInstanceAddOne", StringComparison.Ordinal) &&
            method.Parameters.Count == 2);
        var instanceCallIrMethod = new ILToIRLowering().Lower(instanceCallMethod);
        var instanceCallInstructions = instanceCallIrMethod.Blocks.SelectMany(static block => block.Instructions).ToList();
        var instanceCallInstruction = instanceCallInstructions.Single(static instruction => instruction.OpCode == IROpCode.CallVirt);
        var instanceCallOpSequence = string.Join(",", instanceCallInstructions.Select(static instruction => GetDisplayOp(instruction.OpCode)));

        Console.WriteLine($"method-subject={method.SubjectId}");
        Console.WriteLine($"blocks={irMethod.Blocks.Count}");
        Console.WriteLine($"ops={opSequence}");
        Console.WriteLine($"add-result={addInstruction.Result?.TypeTag}");
        Console.WriteLine($"call-method-subject={callMethod.SubjectId}");
        Console.WriteLine($"call-ops={callOpSequence}");
        Console.WriteLine($"call-target={callInstruction.Operands[0].Symbol}");
        Console.WriteLine($"local-call-method-subject={localCallMethod.SubjectId}");
        Console.WriteLine($"local-call-ops={localCallOpSequence}");
        Console.WriteLine($"local-call-target={localCallInstruction.Operands[0].Symbol}");
        Console.WriteLine($"callvirt-method-subject={callVirtMethod.SubjectId}");
        Console.WriteLine($"callvirt-ops={callVirtOpSequence}");
        Console.WriteLine($"callvirt-target={callVirtInstruction.Operands[0].Symbol}");
        Console.WriteLine($"instance-call-method-subject={instanceCallMethod.SubjectId}");
        Console.WriteLine($"instance-call-ops={instanceCallOpSequence}");
        Console.WriteLine($"instance-call-target={instanceCallInstruction.Operands[0].Symbol}");
        Console.WriteLine("lowering-proof=ok");
        return 0;
    }

    private static string GetDisplayOp(IROpCode opCode)
    {
        return opCode switch
        {
            IROpCode.LdArg => "ldarg",
            IROpCode.LdLoc => "ldloc",
            IROpCode.StLoc => "stloc",
            IROpCode.LdcI4 => "ldc.i4",
            IROpCode.LdStr => "ldstr",
            IROpCode.LdNull => "ldnull",
            IROpCode.Add => "add",
            IROpCode.Sub => "sub",
            IROpCode.Mul => "mul",
            IROpCode.Div => "div",
            IROpCode.Rem => "rem",
            IROpCode.Ceq => "ceq",
            IROpCode.Clt => "clt",
            IROpCode.Cgt => "cgt",
            IROpCode.Call => "call",
            IROpCode.CallVirt => "callvirt",
            IROpCode.CallBridge => "callbridge",
            IROpCode.Br => "br",
            IROpCode.BrTrue => "brtrue",
            IROpCode.BrFalse => "brfalse",
            IROpCode.Ret => "ret",
            _ => opCode.ToString().ToLowerInvariant(),
        };
    }
}
