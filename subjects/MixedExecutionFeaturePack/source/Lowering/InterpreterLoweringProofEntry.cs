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
        CapabilityFamily = ChaosCapabilityFamily.MixedExecution,
        Capability = ChaosCapabilityItem.InterpreterLowering,
        Archetype = ChaosSolutionArchetype.MixedBridgeSolution,
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
        Assert.Equal("InterpreterArithmeticProof/NativeExports::Add(System.Int32,System.Int32)", method.SubjectId);
        Assert.Equal(1, irMethod.Blocks.Count);
        Assert.Equal("ldarg,ldarg,add,ret", opSequence);
        Assert.NotNull(addInstruction.Result);
        Assert.Equal(IRTypeTag.Int32, addInstruction.Result!.TypeTag);
        Assert.Equal("InterpreterArithmeticProof/NativeExports::CallAotBridgeAdd(System.Int32,System.Int32)", callMethod.SubjectId);
        Assert.Equal("ldarg,ldarg,callbridge,ret", callOpSequence);
        Assert.Equal("InterpreterArithmeticProof.AotBridge/AotBridgeExports::Add(System.Int32,System.Int32)", callInstruction.Operands[0].Symbol);
        Assert.Equal("InterpreterArithmeticProof/NativeExports::CallLocalAdd(System.Int32,System.Int32)", localCallMethod.SubjectId);
        Assert.Equal("ldarg,ldarg,call,ret", localCallOpSequence);
        Assert.Equal("InterpreterArithmeticProof/NativeExports::Add(System.Int32,System.Int32)", localCallInstruction.Operands[0].Symbol);
        Assert.Equal("InterpreterArithmeticProof/NativeExports::CallStringLength()", callVirtMethod.SubjectId);
        Assert.Equal("ldstr,callbridge,ret", callVirtOpSequence);
        Assert.Equal("System.Private.CoreLib/System.String::get_Length()", callVirtInstruction.Operands[0].Symbol);
        Assert.Equal("InterpreterArithmeticProof/NativeExports::CallInstanceAddOne(InstanceArithmetic,System.Int32)", instanceCallMethod.SubjectId);
        Assert.Equal("ldarg,ldarg,callvirt,ret", instanceCallOpSequence);
        Assert.Equal("InterpreterArithmeticProof/InstanceArithmetic::AddOne(System.Int32)", instanceCallInstruction.Operands[0].Symbol);

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
