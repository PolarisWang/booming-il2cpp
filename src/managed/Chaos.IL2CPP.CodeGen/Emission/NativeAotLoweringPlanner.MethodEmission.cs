using System;
using System.Collections.Generic;
using System.Collections.Immutable;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Reflection.Metadata;
using System.Reflection.Metadata.Ecma335;
using System.Reflection.PortableExecutable;
using System.Runtime.CompilerServices;
using System.Text;
using System.Text.Json;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.CodeGen;

public sealed partial class NativeAotLoweringPlanner
{
	private static string FormatMethodDeclaration(AotCoreIrMethodArtifact method)
	{
		return $"extern \"C\" {MapAbiSlotReturnType(method.ReturnAbi)} {method.NativeSymbol}({FormatAbiSlotParameterSignature(GetMethodAbiParameterSlots(method))});";
	}

	private void EmitManagedMethod(StringBuilder builder, AotCoreIrMethodArtifact method)
	{
		ValidateMethod(method);
		IReadOnlyList<AotCoreIrInstructionArtifact> instructions = method.Instructions;
		ValidateInstructions(method, instructions);
		IReadOnlyList<AotCoreIrAbiSlotArtifact> methodAbiParameterSlots = GetMethodAbiParameterSlots(method);
		int value = Math.Max(instructions.Count, 1);
		IReadOnlyDictionary<int, int?> nextOffsetsByIlOffset = CreateNextOffsets(instructions);
		HashSet<int> offsets = instructions.Select(GetRequiredIlOffset).ToHashSet();
		StringBuilder stringBuilder = builder;
		StringBuilder stringBuilder2 = stringBuilder;
		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(19, 1, stringBuilder);
		handler.AppendLiteral("// Managed method: ");
		handler.AppendFormatted(method.SubjectId);
		stringBuilder2.AppendLine(ref handler);
		stringBuilder = builder;
		StringBuilder stringBuilder3 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(14, 3, stringBuilder);
		handler.AppendLiteral("extern \"C\" ");
		handler.AppendFormatted(MapAbiSlotReturnType(method.ReturnAbi));
		handler.AppendLiteral(" ");
		handler.AppendFormatted(method.NativeSymbol);
		handler.AppendLiteral("(");
		handler.AppendFormatted(FormatAbiSlotParameterSignature(methodAbiParameterSlots));
		handler.AppendLiteral(")");
		stringBuilder3.AppendLine(ref handler);
		builder.AppendLine("{");
		stringBuilder = builder;
		StringBuilder stringBuilder4 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(45, 1, stringBuilder);
		handler.AppendLiteral("    std::array<std::intptr_t, ");
		handler.AppendFormatted(Math.Max(methodAbiParameterSlots.Count, 1));
		handler.AppendLiteral("> chaos_args{};");
		stringBuilder4.AppendLine(ref handler);
		stringBuilder = builder;
		StringBuilder stringBuilder5 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(47, 1, stringBuilder);
		handler.AppendLiteral("    std::array<std::intptr_t, ");
		handler.AppendFormatted(Math.Max(method.LocalCount, 1));
		handler.AppendLiteral("> chaos_locals{};");
		stringBuilder5.AppendLine(ref handler);
		stringBuilder = builder;
		StringBuilder stringBuilder6 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(51, 1, stringBuilder);
		handler.AppendLiteral("    std::array<std::intptr_t, ");
		handler.AppendFormatted(value);
		handler.AppendLiteral("> chaos_eval_stack{};");
		stringBuilder6.AppendLine(ref handler);
		builder.AppendLine("    std::size_t chaos_stack_top = 0;");
		EmitAbiArgumentInitialization(builder, methodAbiParameterSlots);
		builder.AppendLine();
		if (TryCreateCatchOnlyExceptionMethodShape(method, out CatchOnlyExceptionMethodShape catchOnlyShape))
		{
			EmitCatchOnlyExceptionMethodBody(builder, method, catchOnlyShape, nextOffsetsByIlOffset, offsets);
			builder.AppendLine("}");
			return;
		}
		if (TryCreateFilterOnlyExceptionMethodShape(method, out FilterOnlyExceptionMethodShape filterOnlyShape))
		{
			EmitFilterOnlyExceptionMethodBody(builder, method, filterOnlyShape, nextOffsetsByIlOffset, offsets);
			builder.AppendLine("}");
			return;
		}
		if (TryCreateFinallyOnlyExceptionMethodShape(method, out FinallyOnlyExceptionMethodShape finallyOnlyShape))
		{
			EmitFinallyOnlyExceptionMethodBody(builder, method, finallyOnlyShape, nextOffsetsByIlOffset, offsets);
			builder.AppendLine("}");
			return;
		}
		if (TryCreateCatchAndFinallyExceptionMethodShape(method, out CatchAndFinallyExceptionMethodShape catchAndFinallyShape))
		{
			EmitCatchAndFinallyExceptionMethodBody(builder, method, catchAndFinallyShape, nextOffsetsByIlOffset, offsets);
			builder.AppendLine("}");
			return;
		}
		if (TryCreateFilterAndFinallyExceptionMethodShape(method, out FilterAndFinallyExceptionMethodShape filterAndFinallyShape))
		{
			EmitFilterAndFinallyExceptionMethodBody(builder, method, filterAndFinallyShape, nextOffsetsByIlOffset, offsets);
			builder.AppendLine("}");
			return;
		}
		int requiredIlOffset = GetRequiredIlOffset(instructions[0]);
		stringBuilder = builder;
		StringBuilder stringBuilder7 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(19, 1, stringBuilder);
		handler.AppendLiteral("    goto chaos_ip_");
		handler.AppendFormatted(requiredIlOffset);
		handler.AppendLiteral(";");
		stringBuilder7.AppendLine(ref handler);
		builder.AppendLine();
		EmitInstructionRange(builder, method, instructions, nextOffsetsByIlOffset, offsets);
		builder.AppendLine("}");
	}

	private static void ValidateEntryMethod(AotCoreIrMethodArtifact entryMethod)
	{
		ValidateMethod(entryMethod);
		if (entryMethod.ParameterCount != 0)
		{
			throw new NotSupportedException("native-aot entry '" + entryMethod.SubjectId + "' must not take parameters");
		}
		if (!string.Equals(entryMethod.ReturnType, "System.Int32", StringComparison.Ordinal))
		{
			throw new NotSupportedException("native-aot entry '" + entryMethod.SubjectId + "' must return System.Int32");
		}
	}

	private static void ValidateMethod(AotCoreIrMethodArtifact method)
	{
		if (string.IsNullOrWhiteSpace(method.NativeSymbol))
		{
			throw new InvalidOperationException("native-aot method '" + method.SubjectId + "' is missing native symbol metadata");
		}
		MapAbiSlotReturnType(method.ReturnAbi);
		if (method.ExceptionRegionCount != 0 && !TryCreateCatchOnlyExceptionMethodShape(method, out CatchOnlyExceptionMethodShape _) && !TryCreateFilterOnlyExceptionMethodShape(method, out FilterOnlyExceptionMethodShape _) && !TryCreateFinallyOnlyExceptionMethodShape(method, out FinallyOnlyExceptionMethodShape _) && !TryCreateCatchAndFinallyExceptionMethodShape(method, out CatchAndFinallyExceptionMethodShape _) && !TryCreateFilterAndFinallyExceptionMethodShape(method, out FilterAndFinallyExceptionMethodShape _))
		{
			throw new NotSupportedException("native-aot method '" + method.SubjectId + "' does not support current exception region shape");
		}
	}

	private static void ValidateInstructions(AotCoreIrMethodArtifact entryMethod, IReadOnlyList<AotCoreIrInstructionArtifact> instructions)
	{
		if (instructions.Count == 0)
		{
			throw new InvalidOperationException("native-aot entry '" + entryMethod.SubjectId + "' does not contain instructions");
		}
		HashSet<int> hashSet = new HashSet<int>();
		foreach (AotCoreIrInstructionArtifact instruction in instructions)
		{
			int requiredIlOffset = GetRequiredIlOffset(instruction);
			if (!hashSet.Add(requiredIlOffset))
			{
				throw new InvalidOperationException($"native-aot entry '{entryMethod.SubjectId}' contains duplicate IL offset {requiredIlOffset}");
			}
		}
	}

	private void EmitInstruction(StringBuilder builder, AotCoreIrMethodArtifact method, AotCoreIrInstructionArtifact instruction, int? nextOffset, IReadOnlySet<int> offsets)
	{
		AotCoreIrReferenceArtifact targetReference = instruction.TargetReference;
		if ((object)targetReference?.GenericContext != null)
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder2 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(4, 1, stringBuilder);
			handler.AppendLiteral("    ");
			handler.AppendFormatted(FormatGenericContextComment(targetReference.GenericContext));
			stringBuilder2.AppendLine(ref handler);
		}
		switch (instruction.Op)
		{
		case "ldc.i4":
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder11 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(71, 1, stringBuilder);
			handler.AppendLiteral("    chaos_eval_stack[chaos_stack_top++] = static_cast<std::intptr_t>(");
			handler.AppendFormatted(FormatInt32Literal(GetRequiredIntOperand(instruction)));
			handler.AppendLiteral(");");
			stringBuilder11.AppendLine(ref handler);
			AppendGotoNext(builder, nextOffset, instruction.Op);
			break;
		}
		case "ldc.i8":
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder10 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(62, 1, stringBuilder);
			handler.AppendLiteral("    chaos_eval_stack[chaos_stack_top++] = chaos_store_int64(");
			handler.AppendFormatted(FormatInt64Literal(GetRequiredInt64Operand(instruction)));
			handler.AppendLiteral(");");
			stringBuilder10.AppendLine(ref handler);
			AppendGotoNext(builder, nextOffset, instruction.Op);
			break;
		}
		case "ldc.r4":
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder9 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(64, 1, stringBuilder);
			handler.AppendLiteral("    chaos_eval_stack[chaos_stack_top++] = chaos_store_float32(");
			handler.AppendFormatted(FormatFloat32Literal(GetRequiredSingleOperand(instruction)));
			handler.AppendLiteral(");");
			stringBuilder9.AppendLine(ref handler);
			AppendGotoNext(builder, nextOffset, instruction.Op);
			break;
		}
		case "ldc.r8":
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder8 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(64, 1, stringBuilder);
			handler.AppendLiteral("    chaos_eval_stack[chaos_stack_top++] = chaos_store_float64(");
			handler.AppendFormatted(FormatFloat64Literal(GetRequiredDoubleOperand(instruction)));
			handler.AppendLiteral(");");
			stringBuilder8.AppendLine(ref handler);
			AppendGotoNext(builder, nextOffset, instruction.Op);
			break;
		}
		case "ldarg":
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder7 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(55, 1, stringBuilder);
			handler.AppendLiteral("    chaos_eval_stack[chaos_stack_top++] = chaos_args[");
			handler.AppendFormatted(GetRequiredIntOperand(instruction));
			handler.AppendLiteral("];");
			stringBuilder7.AppendLine(ref handler);
			AppendGotoNext(builder, nextOffset, instruction.Op);
			break;
		}
		case "ldstr":
			EmitLoadStringLiteral(builder, instruction, nextOffset, instruction.Op);
			break;
		case "ldtoken":
			EmitLoadTypeToken(builder, instruction, nextOffset, instruction.Op);
			break;
		case "ldarga":
			EmitLoadArgumentAddress(builder, method, instruction, nextOffset, instruction.Op);
			break;
		case "ldnull":
			builder.AppendLine("    chaos_eval_stack[chaos_stack_top++] = static_cast<std::intptr_t>(0);");
			AppendGotoNext(builder, nextOffset, instruction.Op);
			break;
		case "dup":
			builder.AppendLine("    chaos_eval_stack[chaos_stack_top] = chaos_eval_stack[chaos_stack_top - 1];");
			builder.AppendLine("    chaos_stack_top++;");
			AppendGotoNext(builder, nextOffset, instruction.Op);
			break;
		case "conv.i4":
			builder.AppendLine("    chaos_eval_stack[chaos_stack_top - 1] = static_cast<std::intptr_t>(static_cast<std::int32_t>(chaos_eval_stack[chaos_stack_top - 1]));");
			AppendGotoNext(builder, nextOffset, instruction.Op);
			break;
		case "conv.i1":
			EmitStackTopConversion(builder, "std::int8_t", nextOffset, instruction.Op);
			break;
		case "conv.i2":
			EmitStackTopConversion(builder, "std::int16_t", nextOffset, instruction.Op);
			break;
		case "conv.i8":
			EmitStackTopConversion(builder, "std::int64_t", nextOffset, instruction.Op);
			break;
		case "conv.r4":
			EmitStackTopFloatingPointConversion(builder, "float", "chaos_store_float32", nextOffset, instruction.Op);
			break;
		case "conv.r8":
			EmitStackTopFloatingPointConversion(builder, "double", "chaos_store_float64", nextOffset, instruction.Op);
			break;
		case "conv.u":
			EmitStackTopConversion(builder, "std::uintptr_t", nextOffset, instruction.Op);
			break;
		case "localloc":
			EmitLocalAlloc(builder, nextOffset, instruction.Op);
			break;
		case "conv.u1":
			EmitStackTopConversion(builder, "std::uint8_t", nextOffset, instruction.Op);
			break;
		case "conv.u2":
			EmitStackTopConversion(builder, "std::uint16_t", nextOffset, instruction.Op);
			break;
		case "conv.ovf.i1":
			EmitCheckedStackTopConversion(builder, "std::int8_t", "std::numeric_limits<std::int8_t>::min()", "std::numeric_limits<std::int8_t>::max()", nextOffset, instruction.Op);
			break;
		case "conv.ovf.u1":
			EmitCheckedStackTopConversion(builder, "std::uint8_t", "0", "std::numeric_limits<std::uint8_t>::max()", nextOffset, instruction.Op);
			break;
		case "cgt.un":
			builder.AppendLine("    {");
			builder.AppendLine("        const auto chaos_right = static_cast<std::uintptr_t>(chaos_eval_stack[--chaos_stack_top]);");
			builder.AppendLine("        const auto chaos_left = static_cast<std::uintptr_t>(chaos_eval_stack[--chaos_stack_top]);");
			builder.AppendLine("        chaos_eval_stack[chaos_stack_top++] = static_cast<std::intptr_t>(chaos_left > chaos_right ? 1 : 0);");
			builder.AppendLine("    }");
			AppendGotoNext(builder, nextOffset, instruction.Op);
			break;
		case "ceq":
			EmitComparisonResult(builder, instruction, "std::intptr_t", "==", nextOffset, instruction.Op);
			break;
		case "cgt":
			EmitComparisonResult(builder, instruction, "std::int32_t", ">", nextOffset, instruction.Op);
			break;
		case "clt":
			EmitComparisonResult(builder, instruction, "std::int32_t", "<", nextOffset, instruction.Op);
			break;
		case "ldloc":
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder6 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(57, 1, stringBuilder);
			handler.AppendLiteral("    chaos_eval_stack[chaos_stack_top++] = chaos_locals[");
			handler.AppendFormatted(GetRequiredIntOperand(instruction));
			handler.AppendLiteral("];");
			stringBuilder6.AppendLine(ref handler);
			AppendGotoNext(builder, nextOffset, instruction.Op);
			break;
		}
		case "pop":
			builder.AppendLine("    chaos_stack_top--;");
			AppendGotoNext(builder, nextOffset, instruction.Op);
			break;
		case "ldloca":
			EmitLoadLocalAddress(builder, instruction, nextOffset, instruction.Op);
			break;
		case "stloc":
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder5 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(57, 1, stringBuilder);
			handler.AppendLiteral("    chaos_locals[");
			handler.AppendFormatted(GetRequiredIntOperand(instruction));
			handler.AppendLiteral("] = chaos_eval_stack[--chaos_stack_top];");
			stringBuilder5.AppendLine(ref handler);
			AppendGotoNext(builder, nextOffset, instruction.Op);
			break;
		}
		case "add":
			EmitBinaryArithmetic(builder, "chaos_wrap_add", nextOffset, instruction.Op);
			break;
		case "add.ovf":
			EmitOverflowingBinaryArithmetic(builder, instruction, "+", nextOffset, instruction.Op);
			break;
		case "sub":
			EmitBinaryArithmetic(builder, "chaos_wrap_sub", nextOffset, instruction.Op);
			break;
		case "sub.ovf":
			EmitOverflowingBinaryArithmetic(builder, instruction, "-", nextOffset, instruction.Op);
			break;
		case "mul":
			EmitBinaryArithmetic(builder, "chaos_wrap_mul", nextOffset, instruction.Op);
			break;
		case "mul.ovf":
			EmitOverflowingBinaryArithmetic(builder, instruction, "*", nextOffset, instruction.Op);
			break;
		case "div":
			EmitBinaryArithmetic(builder, "chaos_div", nextOffset, instruction.Op);
			break;
		case "rem":
			EmitBinaryArithmetic(builder, "chaos_rem", nextOffset, instruction.Op);
			break;
		case "shl":
			EmitShift(builder, instruction, "chaos_shift_left_int32", nextOffset, instruction.Op);
			break;
		case "shr":
			EmitShift(builder, instruction, "chaos_shift_right_int32", nextOffset, instruction.Op);
			break;
		case "shr.un":
			EmitShift(builder, instruction, "chaos_shift_right_un_int32", nextOffset, instruction.Op);
			break;
		case "not":
			EmitBitwiseNot(builder, instruction, nextOffset, instruction.Op);
			break;
		case "and":
			EmitBinaryBitwise(builder, "&", nextOffset, instruction.Op);
			break;
		case "or":
			EmitBinaryBitwise(builder, "|", nextOffset, instruction.Op);
			break;
		case "xor":
			EmitBinaryBitwise(builder, "^", nextOffset, instruction.Op);
			break;
		case "call":
			EmitDirectCall(builder, instruction, nextOffset, instruction.Op);
			break;
		case "callvirt":
			EmitCallVirt(builder, instruction, nextOffset, instruction.Op);
			break;
		case "ldftn":
			EmitLoadFunctionPointer(builder, instruction, nextOffset, instruction.Op);
			break;
		case "calli":
			EmitIndirectCall(builder, instruction, nextOffset, instruction.Op);
			break;
		case "newobj":
			EmitNewObject(builder, instruction, nextOffset, instruction.Op);
			break;
		case "throw":
			builder.AppendLine("    throw chaos_managed_exception{chaos_eval_stack[--chaos_stack_top]};");
			break;
		case "newarr":
			EmitNewArray(builder, instruction, nextOffset, instruction.Op);
			break;
		case "ldlen":
			EmitArrayLength(builder, nextOffset, instruction.Op);
			break;
		case "ldelema":
			EmitArrayElementAddress(builder, instruction, nextOffset, instruction.Op);
			break;
		case "castclass":
			EmitCastClass(builder, instruction, nextOffset, instruction.Op);
			break;
		case "isinst":
			EmitIsInst(builder, instruction, nextOffset, instruction.Op);
			break;
		case "ldfld":
		case "ldsfld":
			EmitFieldLoad(builder, instruction, nextOffset, instruction.Op);
			break;
		case "ldflda":
		case "ldsflda":
			EmitFieldAddress(builder, instruction, nextOffset, instruction.Op);
			break;
		case "stfld":
		case "stsfld":
			EmitFieldStore(builder, instruction, nextOffset, instruction.Op);
			break;
		case "ldelem":
		case "ldelem.ref":
			EmitArrayLoad(builder, nextOffset, instruction.Op);
			break;
		case "stelem":
		case "stelem.ref":
			EmitArrayStore(builder, nextOffset, instruction.Op);
			break;
		case "ldind.i4":
			EmitLoadIndirectInt32(builder, nextOffset, instruction.Op);
			break;
		case "stind.i4":
			EmitStoreIndirectInt32(builder, nextOffset, instruction.Op);
			break;
		case "cpblk":
			EmitCopyBlock(builder, nextOffset, instruction.Op);
			break;
		case "box":
			EmitBox(builder, instruction, nextOffset, instruction.Op);
			break;
		case "unbox":
			EmitUnbox(builder, instruction, nextOffset, instruction.Op);
			break;
		case "unbox.any":
			EmitUnboxAny(builder, instruction, nextOffset, instruction.Op);
			break;
		case "initobj":
			EmitInitObj(builder, instruction, nextOffset, instruction.Op);
			break;
		case "ldobj":
			EmitLoadObjectValue(builder, instruction, nextOffset, instruction.Op);
			break;
		case "stobj":
			EmitStoreObjectValue(builder, instruction, nextOffset, instruction.Op);
			break;
		case "br":
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder4 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(19, 1, stringBuilder);
			handler.AppendLiteral("    goto chaos_ip_");
			handler.AppendFormatted(GetRequiredBranchTarget(instruction, offsets));
			handler.AppendLiteral(";");
			stringBuilder4.AppendLine(ref handler);
			break;
		}
		case "brtrue":
			EmitTruthBranch(builder, shouldBranchWhenNonZero: true, instruction, nextOffset, offsets);
			break;
		case "brfalse":
			EmitTruthBranch(builder, shouldBranchWhenNonZero: false, instruction, nextOffset, offsets);
			break;
		case "leave":
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder3 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(19, 1, stringBuilder);
			handler.AppendLiteral("    goto chaos_ip_");
			handler.AppendFormatted(GetRequiredBranchTarget(instruction, offsets));
			handler.AppendLiteral(";");
			stringBuilder3.AppendLine(ref handler);
			break;
		}
		case "beq":
			EmitComparisonBranch(builder, "==", instruction, nextOffset, offsets);
			break;
		case "blt":
			EmitComparisonBranch(builder, "<", instruction, nextOffset, offsets);
			break;
		case "bgt":
			EmitComparisonBranch(builder, ">", instruction, nextOffset, offsets);
			break;
		case "ble":
			EmitComparisonBranch(builder, "<=", instruction, nextOffset, offsets);
			break;
		case "bge":
			EmitComparisonBranch(builder, ">=", instruction, nextOffset, offsets);
			break;
		case "bge.un":
			EmitUnsignedComparisonBranch(builder, ">=", instruction, nextOffset, offsets);
			break;
		case "bne.un":
			EmitComparisonBranch(builder, "!=", instruction, nextOffset, offsets);
			break;
		case "switch":
			EmitSwitch(builder, instruction, nextOffset, offsets);
			break;
		case "ret":
			EmitMethodReturn(builder, method.ReturnAbi);
			break;
		default:
			throw new NotSupportedException("native-aot lowering does not support opcode '" + instruction.Op + "'");
		}
	}

	private void EmitDirectCall(StringBuilder builder, AotCoreIrInstructionArtifact instruction, int? nextOffset, string op)
	{
		InvocationTarget invocationTarget = ResolveDirectInvocationTarget(instruction);
		EmitResolvedInvocation(builder, invocationTarget.TargetSymbol, invocationTarget.ParameterAbis, invocationTarget.ReturnAbi, invocationTarget.RawArgumentIndices, nextOffset, op, enforceInstanceNullCheck: false);
	}

	private void EmitCallVirt(StringBuilder builder, AotCoreIrInstructionArtifact instruction, int? nextOffset, string op)
	{
		if (IsDelegateInvokeInstruction(instruction))
		{
			EmitDelegateInvoke(builder, instruction, nextOffset, op);
			return;
		}
		switch (instruction.DispatchKindCode.GetValueOrDefault())
		{
		case HybridDispatchKind.None:
		case HybridDispatchKind.Direct:
		case HybridDispatchKind.ExternalRuntime:
		{
			InvocationTarget invocationTarget = ResolveDirectInvocationTarget(instruction);
			EmitResolvedInvocation(builder, invocationTarget.TargetSymbol, invocationTarget.ParameterAbis, invocationTarget.ReturnAbi, invocationTarget.RawArgumentIndices, nextOffset, op, enforceInstanceNullCheck: true);
			break;
		}
		case HybridDispatchKind.Virtual:
			EmitVirtualDispatchCall(builder, instruction, nextOffset, op);
			break;
		default:
			throw new NotSupportedException($"native-aot lowering does not support callvirt dispatch kind '{instruction.DispatchKindCode}'.");
		}
	}

	private static void EmitResolvedInvocation(StringBuilder builder, string targetSymbol, IReadOnlyList<AotCoreIrAbiSlotArtifact> parameterAbis, AotCoreIrAbiSlotArtifact returnAbi, IReadOnlySet<int> rawArgumentIndices, int? nextOffset, string op, bool enforceInstanceNullCheck)
	{
		string a = MapAbiSlotReturnType(returnAbi);
		builder.AppendLine("    {");
		for (int num = parameterAbis.Count - 1; num >= 0; num--)
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder2 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(72, 1, stringBuilder);
			handler.AppendLiteral("        const auto chaos_raw_arg_");
			handler.AppendFormatted(num);
			handler.AppendLiteral(" = chaos_eval_stack[--chaos_stack_top];");
			stringBuilder2.AppendLine(ref handler);
			builder.AppendLine(rawArgumentIndices.Contains(num) ? $"        const auto chaos_arg_{num} = chaos_raw_arg_{num};" : $"        const auto chaos_arg_{num} = {FormatInboundAbiArgumentExpression(parameterAbis[num], $"chaos_raw_arg_{num}")};");
		}
		if (enforceInstanceNullCheck && parameterAbis.Count > 0)
		{
			builder.AppendLine("        if (chaos_arg_0 == static_cast<std::intptr_t>(0))");
			builder.AppendLine("        {");
			builder.AppendLine("            std::abort();");
			builder.AppendLine("        }");
		}
		string value = targetSymbol + "(" + FormatAbiInvocationArgumentList(parameterAbis) + ")";
		if (string.Equals(a, "void", StringComparison.Ordinal))
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder3 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(9, 1, stringBuilder);
			handler.AppendLiteral("        ");
			handler.AppendFormatted(value);
			handler.AppendLiteral(";");
			stringBuilder3.AppendLine(ref handler);
		}
		else
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder4 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(35, 1, stringBuilder);
			handler.AppendLiteral("        const auto chaos_result = ");
			handler.AppendFormatted(value);
			handler.AppendLiteral(";");
			stringBuilder4.AppendLine(ref handler);
			EmitAbiReturnPush(builder, returnAbi, "chaos_result", "        ");
		}
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}

	private void EmitDelegateInvoke(StringBuilder builder, AotCoreIrInstructionArtifact instruction, int? nextOffset, string op)
	{
		string methodDeclaringTypeSubjectId = GetMethodDeclaringTypeSubjectId(instruction.Callee);
		IReadOnlyList<AotCoreIrAbiSlotArtifact> readOnlyList = ResolveDelegateInvokeParameterAbis(instruction);
		AotCoreIrAbiSlotArtifact aotCoreIrAbiSlotArtifact = ResolveDelegateInvokeReturnAbi(instruction);
		string text = MapAbiSlotReturnType(aotCoreIrAbiSlotArtifact);
		string value = (string.IsNullOrWhiteSpace(FormatAbiSlotParameterSignature(readOnlyList)) ? (text + "(*)()") : (text + "(*)(" + FormatAbiSlotParameterSignature(readOnlyList) + ")"));
		string value2 = (string.IsNullOrWhiteSpace(FormatAbiSlotParameterSignature(readOnlyList)) ? (text + "(*)(std::intptr_t chaos_delegate_target)") : (text + "(*)(std::intptr_t chaos_delegate_target, " + FormatAbiSlotParameterSignature(readOnlyList) + ")"));
		builder.AppendLine("    {");
		StringBuilder stringBuilder;
		StringBuilder.AppendInterpolatedStringHandler handler;
		for (int num = readOnlyList.Count - 1; num >= 0; num--)
		{
			stringBuilder = builder;
			StringBuilder stringBuilder2 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(72, 1, stringBuilder);
			handler.AppendLiteral("        const auto chaos_raw_arg_");
			handler.AppendFormatted(num);
			handler.AppendLiteral(" = chaos_eval_stack[--chaos_stack_top];");
			stringBuilder2.AppendLine(ref handler);
			stringBuilder = builder;
			StringBuilder stringBuilder3 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(33, 2, stringBuilder);
			handler.AppendLiteral("        const auto chaos_arg_");
			handler.AppendFormatted(num);
			handler.AppendLiteral(" = ");
			handler.AppendFormatted(FormatInboundAbiArgumentExpression(readOnlyList[num], $"chaos_raw_arg_{num}"));
			handler.AppendLiteral(";");
			stringBuilder3.AppendLine(ref handler);
		}
		builder.AppendLine("        const auto chaos_delegate_value = chaos_eval_stack[--chaos_stack_top];");
		builder.AppendLine("        if (chaos_delegate_value == static_cast<std::intptr_t>(0))");
		builder.AppendLine("        {");
		builder.AppendLine("            std::abort();");
		builder.AppendLine("        }");
		stringBuilder = builder;
		StringBuilder stringBuilder4 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(73, 1, stringBuilder);
		handler.AppendLiteral("        auto* chaos_delegate = reinterpret_cast<");
		handler.AppendFormatted(GetNativeTypeSymbol(methodDeclaringTypeSubjectId));
		handler.AppendLiteral("*>(chaos_delegate_value);");
		stringBuilder4.AppendLine(ref handler);
		builder.AppendLine("        if (chaos_delegate->chaos_delegate_invocation_count > static_cast<std::intptr_t>(0))");
		builder.AppendLine("        {");
		builder.AppendLine("            const auto* chaos_invocation_list = reinterpret_cast<const std::vector<std::intptr_t>*>(chaos_delegate->chaos_delegate_invocation_list);");
		builder.AppendLine("            if (chaos_invocation_list == nullptr ||");
		builder.AppendLine("                static_cast<std::intptr_t>(chaos_invocation_list->size()) != chaos_delegate->chaos_delegate_invocation_count)");
		builder.AppendLine("            {");
		builder.AppendLine("                std::abort();");
		builder.AppendLine("            }");
		builder.AppendLine();
		if (!string.Equals(text, "void", StringComparison.Ordinal))
		{
			stringBuilder = builder;
			StringBuilder stringBuilder5 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(28, 1, stringBuilder);
			handler.AppendLiteral("            ");
			handler.AppendFormatted(text);
			handler.AppendLiteral(" chaos_result{};");
			stringBuilder5.AppendLine(ref handler);
		}
		builder.AppendLine("            for (std::size_t chaos_delegate_index = 0; chaos_delegate_index < chaos_invocation_list->size(); ++chaos_delegate_index)");
		builder.AppendLine("            {");
		builder.AppendLine("                const auto chaos_invocation_delegate_value = (*chaos_invocation_list)[chaos_delegate_index];");
		builder.AppendLine("                if (chaos_invocation_delegate_value == static_cast<std::intptr_t>(0))");
		builder.AppendLine("                {");
		builder.AppendLine("                    std::abort();");
		builder.AppendLine("                }");
		stringBuilder = builder;
		StringBuilder stringBuilder6 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(103, 1, stringBuilder);
		handler.AppendLiteral("                auto* chaos_invocation_delegate = reinterpret_cast<");
		handler.AppendFormatted(GetNativeTypeSymbol(methodDeclaringTypeSubjectId));
		handler.AppendLiteral("*>(chaos_invocation_delegate_value);");
		stringBuilder6.AppendLine(ref handler);
		builder.AppendLine("                if (chaos_invocation_delegate->chaos_delegate_method_ptr == static_cast<std::intptr_t>(0))");
		builder.AppendLine("                {");
		builder.AppendLine("                    std::abort();");
		builder.AppendLine("                }");
		builder.AppendLine("                if (chaos_invocation_delegate->chaos_delegate_target == static_cast<std::intptr_t>(0))");
		builder.AppendLine("                {");
		stringBuilder = builder;
		StringBuilder stringBuilder7 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(126, 1, stringBuilder);
		handler.AppendLiteral("                    const auto chaos_open_function = reinterpret_cast<");
		handler.AppendFormatted(value);
		handler.AppendLiteral(">(chaos_invocation_delegate->chaos_delegate_method_ptr);");
		stringBuilder7.AppendLine(ref handler);
		string value3 = "chaos_open_function(" + FormatAbiInvocationArgumentList(readOnlyList) + ")";
		if (string.Equals(text, "void", StringComparison.Ordinal))
		{
			stringBuilder = builder;
			StringBuilder stringBuilder8 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(21, 1, stringBuilder);
			handler.AppendLiteral("                    ");
			handler.AppendFormatted(value3);
			handler.AppendLiteral(";");
			stringBuilder8.AppendLine(ref handler);
		}
		else
		{
			stringBuilder = builder;
			StringBuilder stringBuilder9 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(36, 1, stringBuilder);
			handler.AppendLiteral("                    chaos_result = ");
			handler.AppendFormatted(value3);
			handler.AppendLiteral(";");
			stringBuilder9.AppendLine(ref handler);
		}
		builder.AppendLine("                }");
		builder.AppendLine("                else");
		builder.AppendLine("                {");
		stringBuilder = builder;
		StringBuilder stringBuilder10 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(128, 1, stringBuilder);
		handler.AppendLiteral("                    const auto chaos_closed_function = reinterpret_cast<");
		handler.AppendFormatted(value2);
		handler.AppendLiteral(">(chaos_invocation_delegate->chaos_delegate_method_ptr);");
		stringBuilder10.AppendLine(ref handler);
		string value4 = "chaos_closed_function(chaos_invocation_delegate->chaos_delegate_target" + ((readOnlyList.Count == 0) ? string.Empty : (", " + FormatAbiInvocationArgumentList(readOnlyList))) + ")";
		if (string.Equals(text, "void", StringComparison.Ordinal))
		{
			stringBuilder = builder;
			StringBuilder stringBuilder11 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(21, 1, stringBuilder);
			handler.AppendLiteral("                    ");
			handler.AppendFormatted(value4);
			handler.AppendLiteral(";");
			stringBuilder11.AppendLine(ref handler);
		}
		else
		{
			stringBuilder = builder;
			StringBuilder stringBuilder12 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(36, 1, stringBuilder);
			handler.AppendLiteral("                    chaos_result = ");
			handler.AppendFormatted(value4);
			handler.AppendLiteral(";");
			stringBuilder12.AppendLine(ref handler);
		}
		builder.AppendLine("                }");
		builder.AppendLine("            }");
		if (!string.Equals(text, "void", StringComparison.Ordinal))
		{
			EmitAbiReturnPush(builder, aotCoreIrAbiSlotArtifact, "chaos_result", "            ");
		}
		builder.AppendLine("        }");
		builder.AppendLine("        else");
		builder.AppendLine("        {");
		builder.AppendLine("            if (chaos_delegate->chaos_delegate_method_ptr == static_cast<std::intptr_t>(0))");
		builder.AppendLine("            {");
		builder.AppendLine("                std::abort();");
		builder.AppendLine("            }");
		builder.AppendLine("            if (chaos_delegate->chaos_delegate_target == static_cast<std::intptr_t>(0))");
		builder.AppendLine("            {");
		stringBuilder = builder;
		StringBuilder stringBuilder13 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(111, 1, stringBuilder);
		handler.AppendLiteral("                const auto chaos_open_function = reinterpret_cast<");
		handler.AppendFormatted(value);
		handler.AppendLiteral(">(chaos_delegate->chaos_delegate_method_ptr);");
		stringBuilder13.AppendLine(ref handler);
		if (string.Equals(text, "void", StringComparison.Ordinal))
		{
			stringBuilder = builder;
			StringBuilder stringBuilder14 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(17, 1, stringBuilder);
			handler.AppendLiteral("                ");
			handler.AppendFormatted(value3);
			handler.AppendLiteral(";");
			stringBuilder14.AppendLine(ref handler);
		}
		else
		{
			stringBuilder = builder;
			StringBuilder stringBuilder15 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(43, 1, stringBuilder);
			handler.AppendLiteral("                const auto chaos_result = ");
			handler.AppendFormatted(value3);
			handler.AppendLiteral(";");
			stringBuilder15.AppendLine(ref handler);
			EmitAbiReturnPush(builder, aotCoreIrAbiSlotArtifact, "chaos_result", "                ");
		}
		builder.AppendLine("            }");
		builder.AppendLine("            else");
		builder.AppendLine("            {");
		stringBuilder = builder;
		StringBuilder stringBuilder16 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(113, 1, stringBuilder);
		handler.AppendLiteral("                const auto chaos_closed_function = reinterpret_cast<");
		handler.AppendFormatted(value2);
		handler.AppendLiteral(">(chaos_delegate->chaos_delegate_method_ptr);");
		stringBuilder16.AppendLine(ref handler);
		string value5 = "chaos_closed_function(chaos_delegate->chaos_delegate_target" + ((readOnlyList.Count == 0) ? string.Empty : (", " + FormatAbiInvocationArgumentList(readOnlyList))) + ")";
		if (string.Equals(text, "void", StringComparison.Ordinal))
		{
			stringBuilder = builder;
			StringBuilder stringBuilder17 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(17, 1, stringBuilder);
			handler.AppendLiteral("                ");
			handler.AppendFormatted(value5);
			handler.AppendLiteral(";");
			stringBuilder17.AppendLine(ref handler);
		}
		else
		{
			stringBuilder = builder;
			StringBuilder stringBuilder18 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(43, 1, stringBuilder);
			handler.AppendLiteral("                const auto chaos_result = ");
			handler.AppendFormatted(value5);
			handler.AppendLiteral(";");
			stringBuilder18.AppendLine(ref handler);
			EmitAbiReturnPush(builder, aotCoreIrAbiSlotArtifact, "chaos_result", "                ");
		}
		builder.AppendLine("            }");
		builder.AppendLine("        }");
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}

	private void EmitVirtualDispatchCall(StringBuilder builder, AotCoreIrInstructionArtifact instruction, int? nextOffset, string op)
	{
		AotCoreIrMethodArtifact aotCoreIrMethodArtifact = ResolveRequiredDispatchSlotMethod(instruction);
		IReadOnlyList<AotCoreIrMethodArtifact> readOnlyList = ResolveVirtualDispatchTargets(instruction);
		if (readOnlyList.Count == 0)
		{
			throw new NotSupportedException("native-aot lowering could not resolve virtual dispatch targets for '" + (instruction.Callee ?? aotCoreIrMethodArtifact.SubjectId) + "'.");
		}
		IReadOnlyList<AotCoreIrAbiSlotArtifact> methodAbiParameterSlots = GetMethodAbiParameterSlots(aotCoreIrMethodArtifact);
		string text = MapAbiSlotReturnType(aotCoreIrMethodArtifact.ReturnAbi);
		string value = $"chaos_callvirt_done_{instruction.IlOffset}";
		builder.AppendLine("    {");
		StringBuilder stringBuilder;
		StringBuilder.AppendInterpolatedStringHandler handler;
		for (int num = methodAbiParameterSlots.Count - 1; num >= 0; num--)
		{
			stringBuilder = builder;
			StringBuilder stringBuilder2 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(72, 1, stringBuilder);
			handler.AppendLiteral("        const auto chaos_raw_arg_");
			handler.AppendFormatted(num);
			handler.AppendLiteral(" = chaos_eval_stack[--chaos_stack_top];");
			stringBuilder2.AppendLine(ref handler);
			stringBuilder = builder;
			StringBuilder stringBuilder3 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(33, 2, stringBuilder);
			handler.AppendLiteral("        const auto chaos_arg_");
			handler.AppendFormatted(num);
			handler.AppendLiteral(" = ");
			handler.AppendFormatted(FormatInboundAbiArgumentExpression(methodAbiParameterSlots[num], $"chaos_raw_arg_{num}"));
			handler.AppendLiteral(";");
			stringBuilder3.AppendLine(ref handler);
		}
		builder.AppendLine("        const auto chaos_instance = chaos_arg_0;");
		builder.AppendLine("        if (chaos_instance == static_cast<std::intptr_t>(0))");
		builder.AppendLine("        {");
		builder.AppendLine("            std::abort();");
		builder.AppendLine("        }");
		builder.AppendLine("        auto* chaos_header = reinterpret_cast<chaos_object_header*>(chaos_instance);");
		if (!string.Equals(text, "void", StringComparison.Ordinal))
		{
			stringBuilder = builder;
			StringBuilder stringBuilder4 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(33, 1, stringBuilder);
			handler.AppendLiteral("        ");
			handler.AppendFormatted(text);
			handler.AppendLiteral(" chaos_callvirt_result{};");
			stringBuilder4.AppendLine(ref handler);
		}
		builder.AppendLine("        switch (chaos_header->type_id)");
		builder.AppendLine("        {");
		foreach (AotCoreIrMethodArtifact item in readOnlyList)
		{
			string virtualDispatchTargetTypeIdSymbol = GetVirtualDispatchTargetTypeIdSymbol(item.Identity.DeclaringTypeSubjectId);
			string virtualDispatchInstanceExpression = GetVirtualDispatchInstanceExpression(item.Identity.DeclaringTypeSubjectId, "chaos_instance");
			stringBuilder = builder;
			StringBuilder stringBuilder5 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(18, 1, stringBuilder);
			handler.AppendLiteral("            case ");
			handler.AppendFormatted(virtualDispatchTargetTypeIdSymbol);
			handler.AppendLiteral(":");
			stringBuilder5.AppendLine(ref handler);
			if (string.Equals(text, "void", StringComparison.Ordinal))
			{
				stringBuilder = builder;
				StringBuilder stringBuilder6 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(19, 2, stringBuilder);
				handler.AppendLiteral("                ");
				handler.AppendFormatted(item.NativeSymbol);
				handler.AppendLiteral("(");
				handler.AppendFormatted(FormatAbiInvocationArgumentList(methodAbiParameterSlots, virtualDispatchInstanceExpression));
				handler.AppendLiteral(");");
				stringBuilder6.AppendLine(ref handler);
			}
			else
			{
				stringBuilder = builder;
				StringBuilder stringBuilder7 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(43, 2, stringBuilder);
				handler.AppendLiteral("                chaos_callvirt_result = ");
				handler.AppendFormatted(item.NativeSymbol);
				handler.AppendLiteral("(");
				handler.AppendFormatted(FormatAbiInvocationArgumentList(methodAbiParameterSlots, virtualDispatchInstanceExpression));
				handler.AppendLiteral(");");
				stringBuilder7.AppendLine(ref handler);
			}
			stringBuilder = builder;
			StringBuilder stringBuilder8 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(22, 1, stringBuilder);
			handler.AppendLiteral("                goto ");
			handler.AppendFormatted(value);
			handler.AppendLiteral(";");
			stringBuilder8.AppendLine(ref handler);
		}
		builder.AppendLine("            default:");
		builder.AppendLine("                break;");
		builder.AppendLine("        }");
		builder.AppendLine();
		builder.AppendLine("        auto chaos_current_type_id = chaos_get_base_type_id(chaos_header->type_id);");
		builder.AppendLine("        while (chaos_current_type_id != static_cast<std::intptr_t>(0))");
		builder.AppendLine("        {");
		builder.AppendLine("            switch (chaos_current_type_id)");
		builder.AppendLine("            {");
		foreach (AotCoreIrMethodArtifact item2 in readOnlyList)
		{
			string virtualDispatchTargetTypeIdSymbol2 = GetVirtualDispatchTargetTypeIdSymbol(item2.Identity.DeclaringTypeSubjectId);
			string virtualDispatchInstanceExpression2 = GetVirtualDispatchInstanceExpression(item2.Identity.DeclaringTypeSubjectId, "chaos_instance");
			stringBuilder = builder;
			StringBuilder stringBuilder9 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(22, 1, stringBuilder);
			handler.AppendLiteral("                case ");
			handler.AppendFormatted(virtualDispatchTargetTypeIdSymbol2);
			handler.AppendLiteral(":");
			stringBuilder9.AppendLine(ref handler);
			if (string.Equals(text, "void", StringComparison.Ordinal))
			{
				stringBuilder = builder;
				StringBuilder stringBuilder10 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(23, 2, stringBuilder);
				handler.AppendLiteral("                    ");
				handler.AppendFormatted(item2.NativeSymbol);
				handler.AppendLiteral("(");
				handler.AppendFormatted(FormatAbiInvocationArgumentList(methodAbiParameterSlots, virtualDispatchInstanceExpression2));
				handler.AppendLiteral(");");
				stringBuilder10.AppendLine(ref handler);
			}
			else
			{
				stringBuilder = builder;
				StringBuilder stringBuilder11 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(47, 2, stringBuilder);
				handler.AppendLiteral("                    chaos_callvirt_result = ");
				handler.AppendFormatted(item2.NativeSymbol);
				handler.AppendLiteral("(");
				handler.AppendFormatted(FormatAbiInvocationArgumentList(methodAbiParameterSlots, virtualDispatchInstanceExpression2));
				handler.AppendLiteral(");");
				stringBuilder11.AppendLine(ref handler);
			}
			stringBuilder = builder;
			StringBuilder stringBuilder12 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(26, 1, stringBuilder);
			handler.AppendLiteral("                    goto ");
			handler.AppendFormatted(value);
			handler.AppendLiteral(";");
			stringBuilder12.AppendLine(ref handler);
		}
		builder.AppendLine("                default:");
		builder.AppendLine("                    break;");
		builder.AppendLine("            }");
		builder.AppendLine();
		builder.AppendLine("            chaos_current_type_id = chaos_get_base_type_id(chaos_current_type_id);");
		builder.AppendLine("        }");
		builder.AppendLine();
		builder.AppendLine("        std::abort();");
		stringBuilder = builder;
		StringBuilder stringBuilder13 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(1, 1, stringBuilder);
		handler.AppendFormatted(value);
		handler.AppendLiteral(":");
		stringBuilder13.AppendLine(ref handler);
		if (!string.Equals(text, "void", StringComparison.Ordinal))
		{
			EmitAbiReturnPush(builder, aotCoreIrMethodArtifact.ReturnAbi, "chaos_callvirt_result", "        ");
		}
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}

	private void EmitLoadFunctionPointer(StringBuilder builder, AotCoreIrInstructionArtifact instruction, int? nextOffset, string op)
	{
		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(77, 1, builder);
		handler.AppendLiteral("    chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<std::intptr_t>(&");
		handler.AppendFormatted(GetRequiredFunctionPointerTargetSymbol(instruction));
		handler.AppendLiteral(");");
		builder.AppendLine(ref handler);
		AppendGotoNext(builder, nextOffset, op);
	}

	private static void EmitIndirectCall(StringBuilder builder, AotCoreIrInstructionArtifact instruction, int? nextOffset, string op)
	{
		IReadOnlyList<AotCoreIrAbiSlotArtifact> readOnlyList = CreateCallSiteParameterAbis(instruction);
		AotCoreIrAbiSlotArtifact aotCoreIrAbiSlotArtifact = CreateCallSiteReturnAbi(instruction);
		string text = MapAbiSlotReturnType(aotCoreIrAbiSlotArtifact);
		string value = (string.IsNullOrWhiteSpace(FormatAbiSlotParameterSignature(readOnlyList)) ? (text + "(*)()") : (text + "(*)(" + FormatAbiSlotParameterSignature(readOnlyList) + ")"));
		builder.AppendLine("    {");
		builder.AppendLine("        const auto chaos_target = chaos_eval_stack[--chaos_stack_top];");
		builder.AppendLine("        if (chaos_target == static_cast<std::intptr_t>(0))");
		builder.AppendLine("        {");
		builder.AppendLine("            std::abort();");
		builder.AppendLine("        }");
		StringBuilder stringBuilder;
		StringBuilder.AppendInterpolatedStringHandler handler;
		for (int num = readOnlyList.Count - 1; num >= 0; num--)
		{
			stringBuilder = builder;
			StringBuilder stringBuilder2 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(72, 1, stringBuilder);
			handler.AppendLiteral("        const auto chaos_raw_arg_");
			handler.AppendFormatted(num);
			handler.AppendLiteral(" = chaos_eval_stack[--chaos_stack_top];");
			stringBuilder2.AppendLine(ref handler);
			stringBuilder = builder;
			StringBuilder stringBuilder3 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(33, 2, stringBuilder);
			handler.AppendLiteral("        const auto chaos_arg_");
			handler.AppendFormatted(num);
			handler.AppendLiteral(" = ");
			handler.AppendFormatted(FormatInboundAbiArgumentExpression(readOnlyList[num], $"chaos_raw_arg_{num}"));
			handler.AppendLiteral(";");
			stringBuilder3.AppendLine(ref handler);
		}
		stringBuilder = builder;
		StringBuilder stringBuilder4 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(69, 1, stringBuilder);
		handler.AppendLiteral("        const auto chaos_function = reinterpret_cast<");
		handler.AppendFormatted(value);
		handler.AppendLiteral(">(chaos_target);");
		stringBuilder4.AppendLine(ref handler);
		string value2 = "chaos_function(" + FormatAbiInvocationArgumentList(readOnlyList) + ")";
		if (string.Equals(text, "void", StringComparison.Ordinal))
		{
			stringBuilder = builder;
			StringBuilder stringBuilder5 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(9, 1, stringBuilder);
			handler.AppendLiteral("        ");
			handler.AppendFormatted(value2);
			handler.AppendLiteral(";");
			stringBuilder5.AppendLine(ref handler);
		}
		else
		{
			stringBuilder = builder;
			StringBuilder stringBuilder6 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(35, 1, stringBuilder);
			handler.AppendLiteral("        const auto chaos_result = ");
			handler.AppendFormatted(value2);
			handler.AppendLiteral(";");
			stringBuilder6.AppendLine(ref handler);
			EmitAbiReturnPush(builder, aotCoreIrAbiSlotArtifact, "chaos_result", "        ");
		}
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}

	private static void EmitBinaryArithmetic(StringBuilder builder, string helperName, int? nextOffset, string op)
	{
		builder.AppendLine("    {");
		builder.AppendLine("        const auto chaos_right = static_cast<std::int32_t>(chaos_eval_stack[--chaos_stack_top]);");
		builder.AppendLine("        const auto chaos_left = static_cast<std::int32_t>(chaos_eval_stack[--chaos_stack_top]);");
		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(100, 1, builder);
		handler.AppendLiteral("        chaos_eval_stack[chaos_stack_top++] = static_cast<std::intptr_t>(");
		handler.AppendFormatted(helperName);
		handler.AppendLiteral("(chaos_left, chaos_right));");
		builder.AppendLine(ref handler);
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}

	private static void EmitBinaryBitwise(StringBuilder builder, string operation, int? nextOffset, string op)
	{
		builder.AppendLine("    {");
		builder.AppendLine("        const auto chaos_right = chaos_load_uint64(chaos_eval_stack[--chaos_stack_top]);");
		builder.AppendLine("        const auto chaos_left = chaos_load_uint64(chaos_eval_stack[--chaos_stack_top]);");
		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(90, 1, builder);
		handler.AppendLiteral("        chaos_eval_stack[chaos_stack_top++] = chaos_store_uint64(chaos_left ");
		handler.AppendFormatted(operation);
		handler.AppendLiteral(" chaos_right);");
		builder.AppendLine(ref handler);
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}

	private static void EmitShift(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string helperName, int? nextOffset, string op)
	{
		RequireInt32IntegralResultType(instruction);
		builder.AppendLine("    {");
		builder.AppendLine("        const auto chaos_shift = static_cast<std::int32_t>(chaos_eval_stack[--chaos_stack_top]);");
		builder.AppendLine("        const auto chaos_value = static_cast<std::int32_t>(chaos_eval_stack[--chaos_stack_top]);");
		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(101, 1, builder);
		handler.AppendLiteral("        chaos_eval_stack[chaos_stack_top++] = static_cast<std::intptr_t>(");
		handler.AppendFormatted(helperName);
		handler.AppendLiteral("(chaos_value, chaos_shift));");
		builder.AppendLine(ref handler);
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}

	private static void EmitBitwiseNot(StringBuilder builder, AotCoreIrInstructionArtifact instruction, int? nextOffset, string op)
	{
		RequireInt32IntegralResultType(instruction);
		builder.AppendLine("    {");
		builder.AppendLine("        const auto chaos_value = static_cast<std::int32_t>(chaos_eval_stack[chaos_stack_top - 1]);");
		builder.AppendLine("        chaos_eval_stack[chaos_stack_top - 1] = static_cast<std::intptr_t>(~chaos_value);");
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}

	private static void EmitStackTopConversion(StringBuilder builder, string castType, int? nextOffset, string op)
	{
		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(125, 1, builder);
		handler.AppendLiteral("    chaos_eval_stack[chaos_stack_top - 1] = static_cast<std::intptr_t>(static_cast<");
		handler.AppendFormatted(castType);
		handler.AppendLiteral(">(chaos_eval_stack[chaos_stack_top - 1]));");
		builder.AppendLine(ref handler);
		AppendGotoNext(builder, nextOffset, op);
	}

	private static void EmitStackTopFloatingPointConversion(StringBuilder builder, string castType, string storeHelperName, int? nextOffset, string op)
	{
		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(99, 2, builder);
		handler.AppendLiteral("    chaos_eval_stack[chaos_stack_top - 1] = ");
		handler.AppendFormatted(storeHelperName);
		handler.AppendLiteral("(static_cast<");
		handler.AppendFormatted(castType);
		handler.AppendLiteral(">(chaos_eval_stack[chaos_stack_top - 1]));");
		builder.AppendLine(ref handler);
		AppendGotoNext(builder, nextOffset, op);
	}

	private void EmitCheckedStackTopConversion(StringBuilder builder, string castType, string minValueExpression, string maxValueExpression, int? nextOffset, string op)
	{
		builder.AppendLine("    {");
		builder.AppendLine("        const auto chaos_value = chaos_eval_stack[chaos_stack_top - 1];");
		StringBuilder stringBuilder = builder;
		StringBuilder stringBuilder2 = stringBuilder;
		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(57, 1, stringBuilder);
		handler.AppendLiteral("        if (chaos_value < static_cast<std::intptr_t>(");
		handler.AppendFormatted(minValueExpression);
		handler.AppendLiteral(") ||");
		stringBuilder2.AppendLine(ref handler);
		stringBuilder = builder;
		StringBuilder stringBuilder3 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(55, 1, stringBuilder);
		handler.AppendLiteral("            chaos_value > static_cast<std::intptr_t>(");
		handler.AppendFormatted(maxValueExpression);
		handler.AppendLiteral("))");
		stringBuilder3.AppendLine(ref handler);
		builder.AppendLine("        {");
		EmitThrowOverflowException(builder, "            ");
		builder.AppendLine("        }");
		stringBuilder = builder;
		StringBuilder stringBuilder4 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(103, 1, stringBuilder);
		handler.AppendLiteral("        chaos_eval_stack[chaos_stack_top - 1] = static_cast<std::intptr_t>(static_cast<");
		handler.AppendFormatted(castType);
		handler.AppendLiteral(">(chaos_value));");
		stringBuilder4.AppendLine(ref handler);
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}

	private void EmitOverflowingBinaryArithmetic(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string operation, int? nextOffset, string op)
	{
		RequireInt32IntegralResultType(instruction);
		builder.AppendLine("    {");
		builder.AppendLine("        const auto chaos_right = static_cast<std::int32_t>(chaos_eval_stack[--chaos_stack_top]);");
		builder.AppendLine("        const auto chaos_left = static_cast<std::int32_t>(chaos_eval_stack[--chaos_stack_top]);");
		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(110, 1, builder);
		handler.AppendLiteral("        const auto chaos_wide = static_cast<std::int64_t>(chaos_left) ");
		handler.AppendFormatted(operation);
		handler.AppendLiteral(" static_cast<std::int64_t>(chaos_right);");
		builder.AppendLine(ref handler);
		builder.AppendLine("        if (chaos_wide < static_cast<std::int64_t>(std::numeric_limits<std::int32_t>::min()) ||");
		builder.AppendLine("            chaos_wide > static_cast<std::int64_t>(std::numeric_limits<std::int32_t>::max()))");
		builder.AppendLine("        {");
		EmitThrowOverflowException(builder, "            ");
		builder.AppendLine("        }");
		builder.AppendLine("        chaos_eval_stack[chaos_stack_top++] = static_cast<std::intptr_t>(static_cast<std::int32_t>(chaos_wide));");
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}

	private void EmitThrowOverflowException(StringBuilder builder, string indentation)
	{
		StringBuilder stringBuilder = builder;
		StringBuilder stringBuilder2 = stringBuilder;
		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(31, 2, stringBuilder);
		handler.AppendFormatted(indentation);
		handler.AppendLiteral("auto* chaos_exception = new ");
		handler.AppendFormatted(GetNativeTypeSymbol("System.Private.CoreLib/System.OverflowException"));
		handler.AppendLiteral("{};");
		stringBuilder2.AppendLine(ref handler);
		stringBuilder = builder;
		StringBuilder stringBuilder3 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(35, 2, stringBuilder);
		handler.AppendFormatted(indentation);
		handler.AppendLiteral("chaos_exception->header.type_id = ");
		handler.AppendFormatted(GetNativeTypeIdSymbol("System.Private.CoreLib/System.OverflowException"));
		handler.AppendLiteral(";");
		stringBuilder3.AppendLine(ref handler);
		stringBuilder = builder;
		StringBuilder stringBuilder4 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(80, 1, stringBuilder);
		handler.AppendFormatted(indentation);
		handler.AppendLiteral("throw chaos_managed_exception{reinterpret_cast<std::intptr_t>(chaos_exception)};");
		stringBuilder4.AppendLine(ref handler);
	}

	private static void EmitLoadLocalAddress(StringBuilder builder, AotCoreIrInstructionArtifact instruction, int? nextOffset, string op)
	{
		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(130, 1, builder);
		handler.AppendLiteral("    chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<std::intptr_t>(&chaos_locals[");
		handler.AppendFormatted(GetRequiredIntOperand(instruction));
		handler.AppendLiteral("]) | chaos_managed_pointer_local_slot_tag;");
		builder.AppendLine(ref handler);
		AppendGotoNext(builder, nextOffset, op);
	}

	private static void EmitLoadArgumentAddress(StringBuilder builder, AotCoreIrMethodArtifact method, AotCoreIrInstructionArtifact instruction, int? nextOffset, string op)
	{
		int requiredIntOperand = GetRequiredIntOperand(instruction);
		AotCoreIrAbiSlotArtifact requiredMethodAbiParameterSlot = GetRequiredMethodAbiParameterSlot(method, requiredIntOperand);
		switch (requiredMethodAbiParameterSlot.CarrierKindCode)
		{
		case AotCoreIrAbiCarrierKind.Int32:
		case AotCoreIrAbiCarrierKind.NativeInt:
		case AotCoreIrAbiCarrierKind.Int8:
		case AotCoreIrAbiCarrierKind.UInt8:
		case AotCoreIrAbiCarrierKind.Int16:
		case AotCoreIrAbiCarrierKind.UInt16:
		case AotCoreIrAbiCarrierKind.Float32:
		case AotCoreIrAbiCarrierKind.Float64:
		case AotCoreIrAbiCarrierKind.Int64:
		case AotCoreIrAbiCarrierKind.UInt64:
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder3 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(128, 1, stringBuilder);
			handler.AppendLiteral("    chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<std::intptr_t>(&chaos_args[");
			handler.AppendFormatted(requiredIntOperand);
			handler.AppendLiteral("]) | chaos_managed_pointer_local_slot_tag;");
			stringBuilder3.AppendLine(ref handler);
			break;
		}
		case AotCoreIrAbiCarrierKind.ValueTypeByValue:
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder2 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(55, 1, stringBuilder);
			handler.AppendLiteral("    chaos_eval_stack[chaos_stack_top++] = chaos_args[");
			handler.AppendFormatted(requiredIntOperand);
			handler.AppendLiteral("];");
			stringBuilder2.AppendLine(ref handler);
			break;
		}
		default:
			throw new NotSupportedException($"native-aot lowering does not support ldarga for ABI carrier '{requiredMethodAbiParameterSlot.CarrierKindCode}'.");
		}
		AppendGotoNext(builder, nextOffset, op);
	}

	private static void EmitLoadStringLiteral(StringBuilder builder, AotCoreIrInstructionArtifact instruction, int? nextOffset, string op)
	{
		string requiredStringOperand = GetRequiredStringOperand(instruction);
		builder.AppendLine("    {");
		StringBuilder stringBuilder = builder;
		StringBuilder stringBuilder2 = stringBuilder;
		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(36, 1, stringBuilder);
		handler.AppendLiteral("        auto* chaos_string = new ");
		handler.AppendFormatted(GetNativeTypeSymbol("System.Private.CoreLib/System.String"));
		handler.AppendLiteral("{};");
		stringBuilder2.AppendLine(ref handler);
		stringBuilder = builder;
		StringBuilder stringBuilder3 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(40, 1, stringBuilder);
		handler.AppendLiteral("        chaos_string->header.type_id = ");
		handler.AppendFormatted(GetNativeTypeIdSymbol("System.Private.CoreLib/System.String"));
		handler.AppendLiteral(";");
		stringBuilder3.AppendLine(ref handler);
		stringBuilder = builder;
		StringBuilder stringBuilder4 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(60, 1, stringBuilder);
		handler.AppendLiteral("        chaos_string->length = static_cast<std::intptr_t>(");
		handler.AppendFormatted(Encoding.UTF8.GetByteCount(requiredStringOperand));
		handler.AppendLiteral(");");
		stringBuilder4.AppendLine(ref handler);
		stringBuilder = builder;
		StringBuilder stringBuilder5 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(35, 1, stringBuilder);
		handler.AppendLiteral("        chaos_string->utf8_data = ");
		handler.AppendFormatted(ToCppStringLiteral(requiredStringOperand));
		handler.AppendLiteral(";");
		stringBuilder5.AppendLine(ref handler);
		builder.AppendLine("        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<std::intptr_t>(chaos_string);");
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}

	private static void EmitLoadTypeToken(StringBuilder builder, AotCoreIrInstructionArtifact instruction, int? nextOffset, string op)
	{
		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(43, 1, builder);
		handler.AppendLiteral("    chaos_eval_stack[chaos_stack_top++] = ");
		handler.AppendFormatted(GetRequiredTypeHandleLiteral(instruction));
		handler.AppendLiteral(";");
		builder.AppendLine(ref handler);
		AppendGotoNext(builder, nextOffset, op);
	}

	private void EmitNewObject(StringBuilder builder, AotCoreIrInstructionArtifact instruction, int? nextOffset, string op)
	{
		AotCoreIrReferenceArtifact requiredTargetReference = GetRequiredTargetReference(instruction);
		if (requiredTargetReference.Kind != AotCoreIrReferenceKind.Type)
		{
			throw new NotSupportedException($"native-aot newobj requires type target reference, got '{requiredTargetReference.Kind}'.");
		}
		if (IsDelegateTypeSubjectId(requiredTargetReference.SubjectId, _referenceTypeBaseSubjectIds))
		{
			builder.AppendLine("    {");
			builder.AppendLine("        const auto chaos_method_ptr = chaos_eval_stack[--chaos_stack_top];");
			builder.AppendLine("        const auto chaos_target = chaos_eval_stack[--chaos_stack_top];");
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder2 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(36, 1, stringBuilder);
			handler.AppendLiteral("        auto* chaos_object = new ");
			handler.AppendFormatted(GetNativeTypeSymbol(requiredTargetReference.SubjectId));
			handler.AppendLiteral("{};");
			stringBuilder2.AppendLine(ref handler);
			stringBuilder = builder;
			StringBuilder stringBuilder3 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(40, 1, stringBuilder);
			handler.AppendLiteral("        chaos_object->header.type_id = ");
			handler.AppendFormatted(GetNativeTypeIdSymbol(requiredTargetReference.SubjectId));
			handler.AppendLiteral(";");
			stringBuilder3.AppendLine(ref handler);
			builder.AppendLine("        chaos_object->chaos_delegate_target = chaos_target;");
			builder.AppendLine("        chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;");
			builder.AppendLine("        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<std::intptr_t>(chaos_object);");
			builder.AppendLine("    }");
			AppendGotoNext(builder, nextOffset, op);
			return;
		}
		if (requiredTargetReference.TypeShape == AotCoreIrTypeShapeKind.ValueType)
		{
			InvocationTarget invocationTarget = TryResolveDirectInvocationTarget(instruction.Callee) ?? throw new NotSupportedException("native-aot lowering requires constructor target for value-type newobj '" + (instruction.Callee ?? "<null>") + "'.");
			if (invocationTarget.ParameterAbis.Count == 0)
			{
				throw new NotSupportedException("native-aot lowering requires instance constructor ABI for '" + (instruction.Callee ?? "<null>") + "'.");
			}
			if (invocationTarget.ReturnAbi.CarrierKindCode != AotCoreIrAbiCarrierKind.Void)
			{
				throw new NotSupportedException("native-aot lowering requires void constructor return ABI for '" + (instruction.Callee ?? "<null>") + "'.");
			}
			builder.AppendLine("    {");
			StringBuilder stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler;
			for (int num = invocationTarget.ParameterAbis.Count - 1; num >= 1; num--)
			{
				stringBuilder = builder;
				StringBuilder stringBuilder4 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(72, 1, stringBuilder);
				handler.AppendLiteral("        const auto chaos_raw_arg_");
				handler.AppendFormatted(num);
				handler.AppendLiteral(" = chaos_eval_stack[--chaos_stack_top];");
				stringBuilder4.AppendLine(ref handler);
				builder.AppendLine(invocationTarget.RawArgumentIndices.Contains(num) ? $"        const auto chaos_arg_{num} = chaos_raw_arg_{num};" : $"        const auto chaos_arg_{num} = {FormatInboundAbiArgumentExpression(invocationTarget.ParameterAbis[num], $"chaos_raw_arg_{num}")};");
			}
			builder.AppendLine("        std::intptr_t chaos_value = static_cast<std::intptr_t>(0);");
			builder.AppendLine("        const auto chaos_arg_0 = reinterpret_cast<std::intptr_t>(&chaos_value) | chaos_managed_pointer_local_slot_tag;");
			stringBuilder = builder;
			StringBuilder stringBuilder5 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(11, 2, stringBuilder);
			handler.AppendLiteral("        ");
			handler.AppendFormatted(invocationTarget.TargetSymbol);
			handler.AppendLiteral("(");
			handler.AppendFormatted(FormatAbiInvocationArgumentList(invocationTarget.ParameterAbis));
			handler.AppendLiteral(");");
			stringBuilder5.AppendLine(ref handler);
			builder.AppendLine("        chaos_eval_stack[chaos_stack_top++] = chaos_value;");
			builder.AppendLine("    }");
			AppendGotoNext(builder, nextOffset, op);
			return;
		}
		InvocationTarget? invocationTarget2 = TryResolveDirectInvocationTarget(instruction.Callee);
		if (invocationTarget2.HasValue)
		{
			InvocationTarget valueOrDefault = invocationTarget2.GetValueOrDefault();
			if (valueOrDefault.ParameterAbis.Count == 0)
			{
				throw new NotSupportedException("native-aot lowering requires instance constructor ABI for '" + (instruction.Callee ?? "<null>") + "'.");
			}
			if (valueOrDefault.ReturnAbi.CarrierKindCode != AotCoreIrAbiCarrierKind.Void)
			{
				throw new NotSupportedException("native-aot lowering requires void constructor return ABI for '" + (instruction.Callee ?? "<null>") + "'.");
			}
			builder.AppendLine("    {");
			StringBuilder stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler;
			for (int num2 = valueOrDefault.ParameterAbis.Count - 1; num2 >= 1; num2--)
			{
				stringBuilder = builder;
				StringBuilder stringBuilder6 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(72, 1, stringBuilder);
				handler.AppendLiteral("        const auto chaos_raw_arg_");
				handler.AppendFormatted(num2);
				handler.AppendLiteral(" = chaos_eval_stack[--chaos_stack_top];");
				stringBuilder6.AppendLine(ref handler);
				builder.AppendLine(valueOrDefault.RawArgumentIndices.Contains(num2) ? $"        const auto chaos_arg_{num2} = chaos_raw_arg_{num2};" : $"        const auto chaos_arg_{num2} = {FormatInboundAbiArgumentExpression(valueOrDefault.ParameterAbis[num2], $"chaos_raw_arg_{num2}")};");
			}
			stringBuilder = builder;
			StringBuilder stringBuilder7 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(36, 1, stringBuilder);
			handler.AppendLiteral("        auto* chaos_object = new ");
			handler.AppendFormatted(GetNativeTypeSymbol(requiredTargetReference.SubjectId));
			handler.AppendLiteral("{};");
			stringBuilder7.AppendLine(ref handler);
			stringBuilder = builder;
			StringBuilder stringBuilder8 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(40, 1, stringBuilder);
			handler.AppendLiteral("        chaos_object->header.type_id = ");
			handler.AppendFormatted(GetNativeTypeIdSymbol(requiredTargetReference.SubjectId));
			handler.AppendLiteral(";");
			stringBuilder8.AppendLine(ref handler);
			builder.AppendLine("        const auto chaos_arg_0 = reinterpret_cast<std::intptr_t>(chaos_object);");
			stringBuilder = builder;
			StringBuilder stringBuilder9 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(11, 2, stringBuilder);
			handler.AppendLiteral("        ");
			handler.AppendFormatted(valueOrDefault.TargetSymbol);
			handler.AppendLiteral("(");
			handler.AppendFormatted(FormatAbiInvocationArgumentList(valueOrDefault.ParameterAbis));
			handler.AppendLiteral(");");
			stringBuilder9.AppendLine(ref handler);
			builder.AppendLine("        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<std::intptr_t>(chaos_object);");
			builder.AppendLine("    }");
			AppendGotoNext(builder, nextOffset, op);
		}
		else
		{
			builder.AppendLine("    {");
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder10 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(36, 1, stringBuilder);
			handler.AppendLiteral("        auto* chaos_object = new ");
			handler.AppendFormatted(GetNativeTypeSymbol(requiredTargetReference.SubjectId));
			handler.AppendLiteral("{};");
			stringBuilder10.AppendLine(ref handler);
			stringBuilder = builder;
			StringBuilder stringBuilder11 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(40, 1, stringBuilder);
			handler.AppendLiteral("        chaos_object->header.type_id = ");
			handler.AppendFormatted(GetNativeTypeIdSymbol(requiredTargetReference.SubjectId));
			handler.AppendLiteral(";");
			stringBuilder11.AppendLine(ref handler);
			builder.AppendLine("        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<std::intptr_t>(chaos_object);");
			builder.AppendLine("    }");
			AppendGotoNext(builder, nextOffset, op);
		}
	}

	private static void EmitNewArray(StringBuilder builder, AotCoreIrInstructionArtifact instruction, int? nextOffset, string op)
	{
		AotCoreIrReferenceArtifact requiredTargetReference = GetRequiredTargetReference(instruction);
		if (requiredTargetReference.Kind != AotCoreIrReferenceKind.Type)
		{
			throw new NotSupportedException($"native-aot newarr requires type target reference, got '{requiredTargetReference.Kind}'.");
		}
		string subjectId = (HasArrayElementReference(requiredTargetReference) ? requiredTargetReference.ArrayElementSubjectId : requiredTargetReference.SubjectId);
		AotCoreIrTypeShapeKind typeShape = (HasArrayElementReference(requiredTargetReference) ? requiredTargetReference.ArrayElementTypeShape : requiredTargetReference.TypeShape);
		builder.AppendLine("    {");
		builder.AppendLine("        const auto chaos_length = static_cast<std::int32_t>(chaos_eval_stack[--chaos_stack_top]);");
		builder.AppendLine("        if (chaos_length < 0)");
		builder.AppendLine("        {");
		builder.AppendLine("            std::abort();");
		builder.AppendLine("        }");
		builder.AppendLine("        auto* chaos_array = new chaos_managed_array{};");
		builder.AppendLine("        chaos_array->header.type_id = chaos_type_id_managed_array;");
		StringBuilder stringBuilder = builder;
		StringBuilder stringBuilder2 = stringBuilder;
		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(43, 1, stringBuilder);
		handler.AppendLiteral("        chaos_array->element_type_shape = ");
		handler.AppendFormatted(GetNativeTypeShapeValue(typeShape));
		handler.AppendLiteral(";");
		stringBuilder2.AppendLine(ref handler);
		stringBuilder = builder;
		StringBuilder stringBuilder3 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(40, 1, stringBuilder);
		handler.AppendLiteral("        chaos_array->element_type_id = ");
		handler.AppendFormatted(GetRuntimeTypeIdExpression(subjectId, typeShape));
		handler.AppendLiteral(";");
		stringBuilder3.AppendLine(ref handler);
		builder.AppendLine("        chaos_array->length = static_cast<std::intptr_t>(chaos_length);");
		builder.AppendLine("        chaos_array->elements = chaos_length == 0 ? nullptr : new std::intptr_t[static_cast<std::size_t>(chaos_length)]{};");
		builder.AppendLine("        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<std::intptr_t>(chaos_array);");
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}

	private static void EmitArrayLength(StringBuilder builder, int? nextOffset, string op)
	{
		builder.AppendLine("    {");
		builder.AppendLine("        auto* chaos_array = reinterpret_cast<chaos_managed_array*>(chaos_eval_stack[--chaos_stack_top]);");
		builder.AppendLine("        if (chaos_array == nullptr)");
		builder.AppendLine("        {");
		builder.AppendLine("            std::abort();");
		builder.AppendLine("        }");
		builder.AppendLine("        chaos_eval_stack[chaos_stack_top++] = chaos_array->length;");
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}

	private static void EmitArrayElementAddress(StringBuilder builder, AotCoreIrInstructionArtifact instruction, int? nextOffset, string op)
	{
		AotCoreIrReferenceArtifact requiredTargetReference = GetRequiredTargetReference(instruction);
		if (requiredTargetReference.Kind != AotCoreIrReferenceKind.Type)
		{
			throw new NotSupportedException($"native-aot ldelema requires type target reference, got '{requiredTargetReference.Kind}'.");
		}
		builder.AppendLine("    {");
		builder.AppendLine("        const auto chaos_index = static_cast<std::int32_t>(chaos_eval_stack[--chaos_stack_top]);");
		builder.AppendLine("        auto* chaos_array = reinterpret_cast<chaos_managed_array*>(chaos_eval_stack[--chaos_stack_top]);");
		builder.AppendLine("        if (chaos_array == nullptr)");
		builder.AppendLine("        {");
		builder.AppendLine("            std::abort();");
		builder.AppendLine("        }");
		builder.AppendLine("        if (chaos_index < 0 || static_cast<std::intptr_t>(chaos_index) >= chaos_array->length)");
		builder.AppendLine("        {");
		builder.AppendLine("            std::abort();");
		builder.AppendLine("        }");
		builder.AppendLine("        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<std::intptr_t>(&chaos_array->elements[static_cast<std::size_t>(chaos_index)]);");
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}

	private static void EmitCastClass(StringBuilder builder, AotCoreIrInstructionArtifact instruction, int? nextOffset, string op)
	{
		AotCoreIrReferenceArtifact requiredTargetReference = GetRequiredTargetReference(instruction);
		if (requiredTargetReference.Kind != AotCoreIrReferenceKind.Type)
		{
			throw new NotSupportedException($"native-aot castclass requires type target reference, got '{requiredTargetReference.Kind}'.");
		}
		builder.AppendLine("    {");
		builder.AppendLine("        const auto chaos_value = chaos_eval_stack[chaos_stack_top - 1];");
		builder.AppendLine("        if (chaos_value != static_cast<std::intptr_t>(0))");
		builder.AppendLine("        {");
		builder.AppendLine("            auto* chaos_header = reinterpret_cast<chaos_object_header*>(chaos_value);");
		if (HasArrayElementReference(requiredTargetReference))
		{
			builder.AppendLine("            if (chaos_header->type_id != chaos_type_id_managed_array)");
			builder.AppendLine("            {");
			builder.AppendLine("                std::abort();");
			builder.AppendLine("            }");
			builder.AppendLine("            auto* chaos_array = reinterpret_cast<chaos_managed_array*>(chaos_value);");
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder2 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(115, 2, stringBuilder);
			handler.AppendLiteral("            if (!chaos_is_array_type_compatible(chaos_array->element_type_shape, chaos_array->element_type_id, ");
			handler.AppendFormatted(GetNativeTypeShapeValue(requiredTargetReference.ArrayElementTypeShape));
			handler.AppendLiteral(", ");
			handler.AppendFormatted(GetRuntimeTypeIdExpression(requiredTargetReference.ArrayElementSubjectId, requiredTargetReference.ArrayElementTypeShape));
			handler.AppendLiteral("))");
			stringBuilder2.AppendLine(ref handler);
		}
		else if (requiredTargetReference.TypeShape == AotCoreIrTypeShapeKind.InterfaceType)
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder3 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(78, 1, stringBuilder);
			handler.AppendLiteral("            if (!chaos_does_type_implement_interface(chaos_header->type_id, ");
			handler.AppendFormatted(GetNativeTypeIdSymbol(requiredTargetReference.SubjectId));
			handler.AppendLiteral("))");
			stringBuilder3.AppendLine(ref handler);
		}
		else if (requiredTargetReference.TypeShape == AotCoreIrTypeShapeKind.ReferenceType)
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder4 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(67, 1, stringBuilder);
			handler.AppendLiteral("            if (!chaos_is_type_compatible(chaos_header->type_id, ");
			handler.AppendFormatted(GetNativeTypeIdSymbol(requiredTargetReference.SubjectId));
			handler.AppendLiteral("))");
			stringBuilder4.AppendLine(ref handler);
		}
		else
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder5 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(42, 1, stringBuilder);
			handler.AppendLiteral("            if (chaos_header->type_id != ");
			handler.AppendFormatted(GetNativeTypeIdSymbol(requiredTargetReference.SubjectId));
			handler.AppendLiteral(")");
			stringBuilder5.AppendLine(ref handler);
		}
		builder.AppendLine("            {");
		builder.AppendLine("                std::abort();");
		builder.AppendLine("            }");
		builder.AppendLine("        }");
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}

	private static void EmitIsInst(StringBuilder builder, AotCoreIrInstructionArtifact instruction, int? nextOffset, string op)
	{
		AotCoreIrReferenceArtifact requiredTargetReference = GetRequiredTargetReference(instruction);
		if (requiredTargetReference.Kind != AotCoreIrReferenceKind.Type)
		{
			throw new NotSupportedException($"native-aot isinst requires type target reference, got '{requiredTargetReference.Kind}'.");
		}
		builder.AppendLine("    {");
		builder.AppendLine("        const auto chaos_value = chaos_eval_stack[--chaos_stack_top];");
		builder.AppendLine("        auto chaos_matches = false;");
		builder.AppendLine("        if (chaos_value != static_cast<std::intptr_t>(0))");
		builder.AppendLine("        {");
		builder.AppendLine("            auto* chaos_header = reinterpret_cast<chaos_object_header*>(chaos_value);");
		if (HasArrayElementReference(requiredTargetReference))
		{
			builder.AppendLine("            if (chaos_header->type_id == chaos_type_id_managed_array)");
			builder.AppendLine("            {");
			builder.AppendLine("                auto* chaos_array = reinterpret_cast<chaos_managed_array*>(chaos_value);");
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder2 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(130, 2, stringBuilder);
			handler.AppendLiteral("                chaos_matches = chaos_is_array_type_compatible(chaos_array->element_type_shape, chaos_array->element_type_id, ");
			handler.AppendFormatted(GetNativeTypeShapeValue(requiredTargetReference.ArrayElementTypeShape));
			handler.AppendLiteral(", ");
			handler.AppendFormatted(GetRuntimeTypeIdExpression(requiredTargetReference.ArrayElementSubjectId, requiredTargetReference.ArrayElementTypeShape));
			handler.AppendLiteral(");");
			stringBuilder2.AppendLine(ref handler);
			builder.AppendLine("            }");
		}
		else if (requiredTargetReference.TypeShape == AotCoreIrTypeShapeKind.InterfaceType)
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder3 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(89, 1, stringBuilder);
			handler.AppendLiteral("            chaos_matches = chaos_does_type_implement_interface(chaos_header->type_id, ");
			handler.AppendFormatted(GetNativeTypeIdSymbol(requiredTargetReference.SubjectId));
			handler.AppendLiteral(");");
			stringBuilder3.AppendLine(ref handler);
		}
		else if (requiredTargetReference.TypeShape == AotCoreIrTypeShapeKind.ReferenceType)
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder4 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(78, 1, stringBuilder);
			handler.AppendLiteral("            chaos_matches = chaos_is_type_compatible(chaos_header->type_id, ");
			handler.AppendFormatted(GetNativeTypeIdSymbol(requiredTargetReference.SubjectId));
			handler.AppendLiteral(");");
			stringBuilder4.AppendLine(ref handler);
		}
		else
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder5 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(54, 1, stringBuilder);
			handler.AppendLiteral("            chaos_matches = chaos_header->type_id == ");
			handler.AppendFormatted(GetNativeTypeIdSymbol(requiredTargetReference.SubjectId));
			handler.AppendLiteral(";");
			stringBuilder5.AppendLine(ref handler);
		}
		builder.AppendLine("        }");
		builder.AppendLine("        chaos_eval_stack[chaos_stack_top++] = chaos_matches ? chaos_value : static_cast<std::intptr_t>(0);");
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}

	private static void EmitFieldLoad(StringBuilder builder, AotCoreIrInstructionArtifact instruction, int? nextOffset, string op)
	{
		AotCoreIrReferenceArtifact requiredTargetReference = GetRequiredTargetReference(instruction);
		if (requiredTargetReference.Kind != AotCoreIrReferenceKind.Field)
		{
			throw new NotSupportedException($"native-aot field load requires field target reference, got '{requiredTargetReference.Kind}'.");
		}
		if (instruction.RuntimeServiceKind == AotCoreIrRuntimeServiceKind.LoadStaticField)
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder2 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(43, 1, stringBuilder);
			handler.AppendLiteral("    chaos_eval_stack[chaos_stack_top++] = ");
			handler.AppendFormatted(GetNativeStaticFieldSymbol(requiredTargetReference.SubjectId));
			handler.AppendLiteral(";");
			stringBuilder2.AppendLine(ref handler);
			AppendGotoNext(builder, nextOffset, op);
			return;
		}
		builder.AppendLine("    {");
		if (requiredTargetReference.DeclaringTypeShape == AotCoreIrTypeShapeKind.ValueType)
		{
			string requiredDeclaringTypeSubjectId = GetRequiredDeclaringTypeSubjectId(requiredTargetReference);
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder3 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(103, 1, stringBuilder);
			handler.AppendLiteral("        auto* chaos_value = chaos_resolve_managed_value_pointer<");
			handler.AppendFormatted(GetNativeValueTypeSymbol(requiredDeclaringTypeSubjectId));
			handler.AppendLiteral(">(chaos_eval_stack[--chaos_stack_top]);");
			stringBuilder3.AppendLine(ref handler);
			stringBuilder = builder;
			StringBuilder stringBuilder4 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(60, 1, stringBuilder);
			handler.AppendLiteral("        chaos_eval_stack[chaos_stack_top++] = chaos_value->");
			handler.AppendFormatted(GetNativeFieldMemberName(requiredTargetReference.SubjectId));
			handler.AppendLiteral(";");
			stringBuilder4.AppendLine(ref handler);
		}
		else
		{
			string requiredDeclaringTypeSubjectId2 = GetRequiredDeclaringTypeSubjectId(requiredTargetReference);
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder5 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(86, 1, stringBuilder);
			handler.AppendLiteral("        auto* chaos_object = reinterpret_cast<");
			handler.AppendFormatted(GetNativeTypeSymbol(requiredDeclaringTypeSubjectId2));
			handler.AppendLiteral("*>(chaos_eval_stack[--chaos_stack_top]);");
			stringBuilder5.AppendLine(ref handler);
			stringBuilder = builder;
			StringBuilder stringBuilder6 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(61, 1, stringBuilder);
			handler.AppendLiteral("        chaos_eval_stack[chaos_stack_top++] = chaos_object->");
			handler.AppendFormatted(GetNativeFieldMemberName(requiredTargetReference.SubjectId));
			handler.AppendLiteral(";");
			stringBuilder6.AppendLine(ref handler);
		}
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}

	private static void EmitFieldAddress(StringBuilder builder, AotCoreIrInstructionArtifact instruction, int? nextOffset, string op)
	{
		AotCoreIrReferenceArtifact requiredTargetReference = GetRequiredTargetReference(instruction);
		if (requiredTargetReference.Kind != AotCoreIrReferenceKind.Field)
		{
			throw new NotSupportedException($"native-aot field address requires field target reference, got '{requiredTargetReference.Kind}'.");
		}
		if (instruction.RuntimeServiceKind == AotCoreIrRuntimeServiceKind.LoadStaticField)
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder2 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(77, 1, stringBuilder);
			handler.AppendLiteral("    chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<std::intptr_t>(&");
			handler.AppendFormatted(GetNativeStaticFieldSymbol(requiredTargetReference.SubjectId));
			handler.AppendLiteral(");");
			stringBuilder2.AppendLine(ref handler);
			AppendGotoNext(builder, nextOffset, op);
			return;
		}
		builder.AppendLine("    {");
		string requiredDeclaringTypeSubjectId = GetRequiredDeclaringTypeSubjectId(requiredTargetReference);
		if (requiredTargetReference.DeclaringTypeShape == AotCoreIrTypeShapeKind.ValueType)
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder3 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(103, 1, stringBuilder);
			handler.AppendLiteral("        auto* chaos_value = chaos_resolve_managed_value_pointer<");
			handler.AppendFormatted(GetNativeValueTypeSymbol(requiredDeclaringTypeSubjectId));
			handler.AppendLiteral(">(chaos_eval_stack[--chaos_stack_top]);");
			stringBuilder3.AppendLine(ref handler);
			stringBuilder = builder;
			StringBuilder stringBuilder4 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(94, 1, stringBuilder);
			handler.AppendLiteral("        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<std::intptr_t>(&chaos_value->");
			handler.AppendFormatted(GetNativeFieldMemberName(requiredTargetReference.SubjectId));
			handler.AppendLiteral(");");
			stringBuilder4.AppendLine(ref handler);
		}
		else
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder5 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(86, 1, stringBuilder);
			handler.AppendLiteral("        auto* chaos_object = reinterpret_cast<");
			handler.AppendFormatted(GetNativeTypeSymbol(requiredDeclaringTypeSubjectId));
			handler.AppendLiteral("*>(chaos_eval_stack[--chaos_stack_top]);");
			stringBuilder5.AppendLine(ref handler);
			stringBuilder = builder;
			StringBuilder stringBuilder6 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(95, 1, stringBuilder);
			handler.AppendLiteral("        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<std::intptr_t>(&chaos_object->");
			handler.AppendFormatted(GetNativeFieldMemberName(requiredTargetReference.SubjectId));
			handler.AppendLiteral(");");
			stringBuilder6.AppendLine(ref handler);
		}
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}

	private static void EmitFieldStore(StringBuilder builder, AotCoreIrInstructionArtifact instruction, int? nextOffset, string op)
	{
		AotCoreIrReferenceArtifact requiredTargetReference = GetRequiredTargetReference(instruction);
		if (requiredTargetReference.Kind != AotCoreIrReferenceKind.Field)
		{
			throw new NotSupportedException($"native-aot field store requires field target reference, got '{requiredTargetReference.Kind}'.");
		}
		builder.AppendLine("    {");
		builder.AppendLine("        const auto chaos_value = chaos_eval_stack[--chaos_stack_top];");
		if (instruction.RuntimeServiceKind == AotCoreIrRuntimeServiceKind.StoreStaticField)
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder2 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(23, 1, stringBuilder);
			handler.AppendLiteral("        ");
			handler.AppendFormatted(GetNativeStaticFieldSymbol(requiredTargetReference.SubjectId));
			handler.AppendLiteral(" = chaos_value;");
			stringBuilder2.AppendLine(ref handler);
		}
		else
		{
			string requiredDeclaringTypeSubjectId = GetRequiredDeclaringTypeSubjectId(requiredTargetReference);
			if (requiredTargetReference.DeclaringTypeShape == AotCoreIrTypeShapeKind.ValueType)
			{
				StringBuilder stringBuilder = builder;
				StringBuilder stringBuilder3 = stringBuilder;
				StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(109, 1, stringBuilder);
				handler.AppendLiteral("        auto* chaos_value_owner = chaos_resolve_managed_value_pointer<");
				handler.AppendFormatted(GetNativeValueTypeSymbol(requiredDeclaringTypeSubjectId));
				handler.AppendLiteral(">(chaos_eval_stack[--chaos_stack_top]);");
				stringBuilder3.AppendLine(ref handler);
				stringBuilder = builder;
				StringBuilder stringBuilder4 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(42, 1, stringBuilder);
				handler.AppendLiteral("        chaos_value_owner->");
				handler.AppendFormatted(GetNativeFieldMemberName(requiredTargetReference.SubjectId));
				handler.AppendLiteral(" = chaos_value;");
				stringBuilder4.AppendLine(ref handler);
			}
			else
			{
				StringBuilder stringBuilder = builder;
				StringBuilder stringBuilder5 = stringBuilder;
				StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(86, 1, stringBuilder);
				handler.AppendLiteral("        auto* chaos_object = reinterpret_cast<");
				handler.AppendFormatted(GetNativeTypeSymbol(requiredDeclaringTypeSubjectId));
				handler.AppendLiteral("*>(chaos_eval_stack[--chaos_stack_top]);");
				stringBuilder5.AppendLine(ref handler);
				stringBuilder = builder;
				StringBuilder stringBuilder6 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(37, 1, stringBuilder);
				handler.AppendLiteral("        chaos_object->");
				handler.AppendFormatted(GetNativeFieldMemberName(requiredTargetReference.SubjectId));
				handler.AppendLiteral(" = chaos_value;");
				stringBuilder6.AppendLine(ref handler);
			}
		}
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}

	private static void EmitArrayLoad(StringBuilder builder, int? nextOffset, string op)
	{
		builder.AppendLine("    {");
		builder.AppendLine("        const auto chaos_index = static_cast<std::int32_t>(chaos_eval_stack[--chaos_stack_top]);");
		builder.AppendLine("        auto* chaos_array = reinterpret_cast<chaos_managed_array*>(chaos_eval_stack[--chaos_stack_top]);");
		builder.AppendLine("        if (chaos_array == nullptr)");
		builder.AppendLine("        {");
		builder.AppendLine("            std::abort();");
		builder.AppendLine("        }");
		builder.AppendLine("        if (chaos_index < 0 || static_cast<std::intptr_t>(chaos_index) >= chaos_array->length)");
		builder.AppendLine("        {");
		builder.AppendLine("            std::abort();");
		builder.AppendLine("        }");
		builder.AppendLine("        chaos_eval_stack[chaos_stack_top++] = chaos_array->elements[static_cast<std::size_t>(chaos_index)];");
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}

	private static void EmitArrayStore(StringBuilder builder, int? nextOffset, string op)
	{
		builder.AppendLine("    {");
		builder.AppendLine("        const auto chaos_value = chaos_eval_stack[--chaos_stack_top];");
		builder.AppendLine("        const auto chaos_index = static_cast<std::int32_t>(chaos_eval_stack[--chaos_stack_top]);");
		builder.AppendLine("        auto* chaos_array = reinterpret_cast<chaos_managed_array*>(chaos_eval_stack[--chaos_stack_top]);");
		builder.AppendLine("        if (chaos_array == nullptr)");
		builder.AppendLine("        {");
		builder.AppendLine("            std::abort();");
		builder.AppendLine("        }");
		builder.AppendLine("        if (chaos_index < 0 || static_cast<std::intptr_t>(chaos_index) >= chaos_array->length)");
		builder.AppendLine("        {");
		builder.AppendLine("            std::abort();");
		builder.AppendLine("        }");
		if (string.Equals(op, "stelem.ref", StringComparison.Ordinal))
		{
			builder.AppendLine("        if (!chaos_is_array_store_compatible(chaos_array, chaos_value))");
			builder.AppendLine("        {");
			builder.AppendLine("            std::abort();");
			builder.AppendLine("        }");
		}
		builder.AppendLine("        chaos_array->elements[static_cast<std::size_t>(chaos_index)] = chaos_value;");
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}

	private static void EmitLoadIndirectInt32(StringBuilder builder, int? nextOffset, string op)
	{
		builder.AppendLine("    {");
		builder.AppendLine("        const auto chaos_address = chaos_eval_stack[--chaos_stack_top];");
		builder.AppendLine("        if (chaos_address == static_cast<std::intptr_t>(0))");
		builder.AppendLine("        {");
		builder.AppendLine("            std::abort();");
		builder.AppendLine("        }");
		builder.AppendLine("        std::int32_t chaos_value = 0;");
		builder.AppendLine("        if ((chaos_address & chaos_raw_int32_pointer_tag) != 0)");
		builder.AppendLine("        {");
		builder.AppendLine("            const auto* chaos_value_ptr = reinterpret_cast<const std::int32_t*>(static_cast<std::uintptr_t>(chaos_address & ~chaos_raw_int32_pointer_tag));");
		builder.AppendLine("            chaos_value = *chaos_value_ptr;");
		builder.AppendLine("        }");
		builder.AppendLine("        else");
		builder.AppendLine("        {");
		builder.AppendLine("            const auto* chaos_value_ptr = chaos_resolve_native_int_slot(chaos_address);");
		builder.AppendLine("            chaos_value = static_cast<std::int32_t>(*chaos_value_ptr);");
		builder.AppendLine("        }");
		builder.AppendLine("        chaos_eval_stack[chaos_stack_top++] = static_cast<std::intptr_t>(chaos_value);");
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}

	private static void EmitStoreIndirectInt32(StringBuilder builder, int? nextOffset, string op)
	{
		builder.AppendLine("    {");
		builder.AppendLine("        const auto chaos_value = static_cast<std::int32_t>(chaos_eval_stack[--chaos_stack_top]);");
		builder.AppendLine("        const auto chaos_address = chaos_eval_stack[--chaos_stack_top];");
		builder.AppendLine("        if (chaos_address == static_cast<std::intptr_t>(0))");
		builder.AppendLine("        {");
		builder.AppendLine("            std::abort();");
		builder.AppendLine("        }");
		builder.AppendLine("        if ((chaos_address & chaos_raw_int32_pointer_tag) != 0)");
		builder.AppendLine("        {");
		builder.AppendLine("            auto* chaos_value_ptr = reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(chaos_address & ~chaos_raw_int32_pointer_tag));");
		builder.AppendLine("            *chaos_value_ptr = chaos_value;");
		builder.AppendLine("        }");
		builder.AppendLine("        else");
		builder.AppendLine("        {");
		builder.AppendLine("            auto* chaos_value_ptr = chaos_resolve_native_int_slot(chaos_address);");
		builder.AppendLine("            *chaos_value_ptr = static_cast<std::intptr_t>(chaos_value);");
		builder.AppendLine("        }");
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}

	private static void EmitLocalAlloc(StringBuilder builder, int? nextOffset, string op)
	{
		builder.AppendLine("    // localloc");
		builder.AppendLine("    {");
		builder.AppendLine("        const auto chaos_size = static_cast<std::intptr_t>(chaos_eval_stack[--chaos_stack_top]);");
		builder.AppendLine("        if (chaos_size < 0)");
		builder.AppendLine("        {");
		builder.AppendLine("            std::abort();");
		builder.AppendLine("        }");
		builder.AppendLine("        const auto chaos_byte_count = static_cast<std::size_t>(chaos_size);");
		builder.AppendLine("        void* chaos_block = std::malloc(chaos_byte_count == static_cast<std::size_t>(0) ? static_cast<std::size_t>(1) : chaos_byte_count);");
		builder.AppendLine("        if (chaos_block == nullptr)");
		builder.AppendLine("        {");
		builder.AppendLine("            std::abort();");
		builder.AppendLine("        }");
		builder.AppendLine("        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<std::intptr_t>(chaos_block);");
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}

	private static void EmitCopyBlock(StringBuilder builder, int? nextOffset, string op)
	{
		builder.AppendLine("    // cpblk");
		builder.AppendLine("    {");
		builder.AppendLine("        const auto chaos_size = static_cast<std::size_t>(static_cast<std::uintptr_t>(chaos_eval_stack[--chaos_stack_top]));");
		builder.AppendLine("        const auto chaos_source = chaos_eval_stack[--chaos_stack_top];");
		builder.AppendLine("        const auto chaos_destination = chaos_eval_stack[--chaos_stack_top];");
		builder.AppendLine("        if (chaos_size != static_cast<std::size_t>(0))");
		builder.AppendLine("        {");
		builder.AppendLine("            const auto chaos_resolve_cpblk_address = [](std::intptr_t chaos_address) -> void*");
		builder.AppendLine("            {");
		builder.AppendLine("                if ((chaos_address & chaos_raw_int32_pointer_tag) != 0)");
		builder.AppendLine("                {");
		builder.AppendLine("                    return reinterpret_cast<void*>(static_cast<std::uintptr_t>(chaos_address & ~chaos_raw_int32_pointer_tag));");
		builder.AppendLine("                }");
		builder.AppendLine("                if ((chaos_address & chaos_managed_pointer_local_slot_tag) != 0)");
		builder.AppendLine("                {");
		builder.AppendLine("                    return reinterpret_cast<void*>(static_cast<std::uintptr_t>(chaos_address & ~chaos_managed_pointer_local_slot_tag));");
		builder.AppendLine("                }");
		builder.AppendLine("                return reinterpret_cast<void*>(chaos_address);");
		builder.AppendLine("            };");
		builder.AppendLine("            if (chaos_source == static_cast<std::intptr_t>(0) || chaos_destination == static_cast<std::intptr_t>(0))");
		builder.AppendLine("            {");
		builder.AppendLine("                std::abort();");
		builder.AppendLine("            }");
		builder.AppendLine("            std::memcpy(chaos_resolve_cpblk_address(chaos_destination), chaos_resolve_cpblk_address(chaos_source), chaos_size);");
		builder.AppendLine("        }");
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}

	private static void EmitBox(StringBuilder builder, AotCoreIrInstructionArtifact instruction, int? nextOffset, string op)
	{
		AotCoreIrReferenceArtifact requiredTargetReference = GetRequiredTargetReference(instruction);
		if (requiredTargetReference.Kind != AotCoreIrReferenceKind.Type)
		{
			throw new NotSupportedException($"native-aot box requires type target reference, got '{requiredTargetReference.Kind}'.");
		}
		builder.AppendLine("    {");
		builder.AppendLine("        const auto chaos_value = chaos_eval_stack[--chaos_stack_top];");
		StringBuilder stringBuilder = builder;
		StringBuilder stringBuilder2 = stringBuilder;
		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(35, 1, stringBuilder);
		handler.AppendLiteral("        auto* chaos_boxed = new ");
		handler.AppendFormatted(GetNativeBoxTypeSymbol(requiredTargetReference.SubjectId));
		handler.AppendLiteral("{};");
		stringBuilder2.AppendLine(ref handler);
		stringBuilder = builder;
		StringBuilder stringBuilder3 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(39, 1, stringBuilder);
		handler.AppendLiteral("        chaos_boxed->header.type_id = ");
		handler.AppendFormatted(GetNativeBoxTypeIdSymbol(requiredTargetReference.SubjectId));
		handler.AppendLiteral(";");
		stringBuilder3.AppendLine(ref handler);
		if (RequiresStructuredValueTypePayload(requiredTargetReference))
		{
			stringBuilder = builder;
			StringBuilder stringBuilder4 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(81, 1, stringBuilder);
			handler.AppendLiteral("        auto* chaos_payload = chaos_resolve_managed_value_pointer<");
			handler.AppendFormatted(GetNativeValueTypeSymbol(requiredTargetReference.SubjectId));
			handler.AppendLiteral(">(chaos_value);");
			stringBuilder4.AppendLine(ref handler);
			builder.AppendLine("        chaos_boxed->value = *chaos_payload;");
		}
		else
		{
			builder.AppendLine("        chaos_boxed->value = chaos_value;");
		}
		builder.AppendLine("        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<std::intptr_t>(chaos_boxed);");
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}

	private static void EmitUnbox(StringBuilder builder, AotCoreIrInstructionArtifact instruction, int? nextOffset, string op)
	{
		AotCoreIrReferenceArtifact requiredTargetReference = GetRequiredTargetReference(instruction);
		if (requiredTargetReference.Kind != AotCoreIrReferenceKind.Type)
		{
			throw new NotSupportedException($"native-aot unbox requires type target reference, got '{requiredTargetReference.Kind}'.");
		}
		builder.AppendLine("    {");
		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(85, 1, builder);
		handler.AppendLiteral("        auto* chaos_boxed = reinterpret_cast<");
		handler.AppendFormatted(GetNativeBoxTypeSymbol(requiredTargetReference.SubjectId));
		handler.AppendLiteral("*>(chaos_eval_stack[--chaos_stack_top]);");
		builder.AppendLine(ref handler);
		builder.AppendLine("        if (chaos_boxed == nullptr)");
		builder.AppendLine("        {");
		builder.AppendLine("            std::abort();");
		builder.AppendLine("        }");
		builder.AppendLine("        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<std::intptr_t>(&chaos_boxed->value);");
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}

	private static void EmitUnboxAny(StringBuilder builder, AotCoreIrInstructionArtifact instruction, int? nextOffset, string op)
	{
		AotCoreIrReferenceArtifact requiredTargetReference = GetRequiredTargetReference(instruction);
		if (requiredTargetReference.Kind != AotCoreIrReferenceKind.Type)
		{
			throw new NotSupportedException($"native-aot unbox.any requires type target reference, got '{requiredTargetReference.Kind}'.");
		}
		builder.AppendLine("    {");
		StringBuilder stringBuilder = builder;
		StringBuilder stringBuilder2 = stringBuilder;
		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(85, 1, stringBuilder);
		handler.AppendLiteral("        auto* chaos_boxed = reinterpret_cast<");
		handler.AppendFormatted(GetNativeBoxTypeSymbol(requiredTargetReference.SubjectId));
		handler.AppendLiteral("*>(chaos_eval_stack[--chaos_stack_top]);");
		stringBuilder2.AppendLine(ref handler);
		builder.AppendLine("        if (chaos_boxed == nullptr)");
		builder.AppendLine("        {");
		builder.AppendLine("            std::abort();");
		builder.AppendLine("        }");
		if (RequiresStructuredValueTypePayload(requiredTargetReference))
		{
			stringBuilder = builder;
			StringBuilder stringBuilder3 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(35, 1, stringBuilder);
			handler.AppendLiteral("        auto* chaos_value = new ");
			handler.AppendFormatted(GetNativeValueTypeSymbol(requiredTargetReference.SubjectId));
			handler.AppendLiteral("{};");
			stringBuilder3.AppendLine(ref handler);
			builder.AppendLine("        *chaos_value = chaos_boxed->value;");
			builder.AppendLine("        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<std::intptr_t>(chaos_value);");
		}
		else
		{
			builder.AppendLine("        chaos_eval_stack[chaos_stack_top++] = chaos_boxed->value;");
		}
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}

	private static void EmitLoadObjectValue(StringBuilder builder, AotCoreIrInstructionArtifact instruction, int? nextOffset, string op)
	{
		AotCoreIrReferenceArtifact requiredTargetReference = GetRequiredTargetReference(instruction);
		if (requiredTargetReference.Kind != AotCoreIrReferenceKind.Type)
		{
			throw new NotSupportedException($"native-aot ldobj requires type target reference, got '{requiredTargetReference.Kind}'.");
		}
		builder.AppendLine("    {");
		if (RequiresStructuredValueTypePayload(requiredTargetReference))
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder2 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(104, 1, stringBuilder);
			handler.AppendLiteral("        auto* chaos_source = chaos_resolve_managed_value_pointer<");
			handler.AppendFormatted(GetNativeValueTypeSymbol(requiredTargetReference.SubjectId));
			handler.AppendLiteral(">(chaos_eval_stack[--chaos_stack_top]);");
			stringBuilder2.AppendLine(ref handler);
			stringBuilder = builder;
			StringBuilder stringBuilder3 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(35, 1, stringBuilder);
			handler.AppendLiteral("        auto* chaos_value = new ");
			handler.AppendFormatted(GetNativeValueTypeSymbol(requiredTargetReference.SubjectId));
			handler.AppendLiteral("{};");
			stringBuilder3.AppendLine(ref handler);
			builder.AppendLine("        *chaos_value = *chaos_source;");
			builder.AppendLine("        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<std::intptr_t>(chaos_value);");
		}
		else
		{
			builder.AppendLine("        auto* chaos_source = chaos_resolve_managed_value_pointer<std::intptr_t>(chaos_eval_stack[--chaos_stack_top]);");
			builder.AppendLine("        chaos_eval_stack[chaos_stack_top++] = *chaos_source;");
		}
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}

	private static void EmitStoreObjectValue(StringBuilder builder, AotCoreIrInstructionArtifact instruction, int? nextOffset, string op)
	{
		AotCoreIrReferenceArtifact requiredTargetReference = GetRequiredTargetReference(instruction);
		if (requiredTargetReference.Kind != AotCoreIrReferenceKind.Type)
		{
			throw new NotSupportedException($"native-aot stobj requires type target reference, got '{requiredTargetReference.Kind}'.");
		}
		builder.AppendLine("    {");
		if (RequiresStructuredValueTypePayload(requiredTargetReference))
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder2 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(86, 1, stringBuilder);
			handler.AppendLiteral("        auto* chaos_source = reinterpret_cast<");
			handler.AppendFormatted(GetNativeValueTypeSymbol(requiredTargetReference.SubjectId));
			handler.AppendLiteral("*>(chaos_eval_stack[--chaos_stack_top]);");
			stringBuilder2.AppendLine(ref handler);
			stringBuilder = builder;
			StringBuilder stringBuilder3 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(109, 1, stringBuilder);
			handler.AppendLiteral("        auto* chaos_destination = chaos_resolve_managed_value_pointer<");
			handler.AppendFormatted(GetNativeValueTypeSymbol(requiredTargetReference.SubjectId));
			handler.AppendLiteral(">(chaos_eval_stack[--chaos_stack_top]);");
			stringBuilder3.AppendLine(ref handler);
			builder.AppendLine("        if (chaos_source == nullptr)");
			builder.AppendLine("        {");
			builder.AppendLine("            std::abort();");
			builder.AppendLine("        }");
			builder.AppendLine("        *chaos_destination = *chaos_source;");
		}
		else
		{
			builder.AppendLine("        const auto chaos_value = chaos_eval_stack[--chaos_stack_top];");
			builder.AppendLine("        auto* chaos_destination = chaos_resolve_managed_value_pointer<std::intptr_t>(chaos_eval_stack[--chaos_stack_top]);");
			builder.AppendLine("        *chaos_destination = chaos_value;");
		}
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}

	private static void EmitInitObj(StringBuilder builder, AotCoreIrInstructionArtifact instruction, int? nextOffset, string op)
	{
		AotCoreIrReferenceArtifact requiredTargetReference = GetRequiredTargetReference(instruction);
		if (requiredTargetReference.Kind != AotCoreIrReferenceKind.Type)
		{
			throw new NotSupportedException($"native-aot initobj requires type target reference, got '{requiredTargetReference.Kind}'.");
		}
		builder.AppendLine("    {");
		if (RequiresStructuredValueTypePayload(requiredTargetReference))
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder2 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(103, 1, stringBuilder);
			handler.AppendLiteral("        auto* chaos_value = chaos_resolve_managed_value_pointer<");
			handler.AppendFormatted(GetNativeValueTypeSymbol(requiredTargetReference.SubjectId));
			handler.AppendLiteral(">(chaos_eval_stack[--chaos_stack_top]);");
			stringBuilder2.AppendLine(ref handler);
			stringBuilder = builder;
			StringBuilder stringBuilder3 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(26, 1, stringBuilder);
			handler.AppendLiteral("        *chaos_value = ");
			handler.AppendFormatted(GetNativeValueTypeSymbol(requiredTargetReference.SubjectId));
			handler.AppendLiteral("{};");
			stringBuilder3.AppendLine(ref handler);
		}
		else
		{
			builder.AppendLine("        const auto chaos_address = chaos_eval_stack[--chaos_stack_top];");
			builder.AppendLine("        if ((chaos_address & chaos_managed_pointer_local_slot_tag) != 0)");
			builder.AppendLine("        {");
			builder.AppendLine("            auto* chaos_slot = reinterpret_cast<std::intptr_t*>(static_cast<std::uintptr_t>(chaos_address & ~chaos_managed_pointer_local_slot_tag));");
			builder.AppendLine("            *chaos_slot = static_cast<std::intptr_t>(0);");
			builder.AppendLine("        }");
			builder.AppendLine("        else");
			builder.AppendLine("        {");
			builder.AppendLine("            *reinterpret_cast<std::intptr_t*>(chaos_address) = static_cast<std::intptr_t>(0);");
			builder.AppendLine("        }");
		}
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}

	private static void EmitComparisonBranch(StringBuilder builder, string comparisonOperator, AotCoreIrInstructionArtifact instruction, int? nextOffset, IReadOnlySet<int> offsets)
	{
		if (!nextOffset.HasValue)
		{
			throw new InvalidOperationException("branch opcode '" + instruction.Op + "' cannot terminate the method");
		}
		int requiredBranchTarget = GetRequiredBranchTarget(instruction, offsets);
		bool flag = ((comparisonOperator == "==" || comparisonOperator == "!=") ? true : false);
		string value = (flag ? "std::intptr_t" : "std::int32_t");
		builder.AppendLine("    {");
		StringBuilder stringBuilder = builder;
		StringBuilder stringBuilder2 = stringBuilder;
		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(84, 1, stringBuilder);
		handler.AppendLiteral("        const auto chaos_right = static_cast<");
		handler.AppendFormatted(value);
		handler.AppendLiteral(">(chaos_eval_stack[--chaos_stack_top]);");
		stringBuilder2.AppendLine(ref handler);
		stringBuilder = builder;
		StringBuilder stringBuilder3 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(83, 1, stringBuilder);
		handler.AppendLiteral("        const auto chaos_left = static_cast<");
		handler.AppendFormatted(value);
		handler.AppendLiteral(">(chaos_eval_stack[--chaos_stack_top]);");
		stringBuilder3.AppendLine(ref handler);
		stringBuilder = builder;
		StringBuilder stringBuilder4 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(36, 1, stringBuilder);
		handler.AppendLiteral("        if (chaos_left ");
		handler.AppendFormatted(comparisonOperator);
		handler.AppendLiteral(" chaos_right)");
		stringBuilder4.AppendLine(ref handler);
		builder.AppendLine("        {");
		stringBuilder = builder;
		StringBuilder stringBuilder5 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(27, 1, stringBuilder);
		handler.AppendLiteral("            goto chaos_ip_");
		handler.AppendFormatted(requiredBranchTarget);
		handler.AppendLiteral(";");
		stringBuilder5.AppendLine(ref handler);
		builder.AppendLine("        }");
		builder.AppendLine("    }");
		stringBuilder = builder;
		StringBuilder stringBuilder6 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(19, 1, stringBuilder);
		handler.AppendLiteral("    goto chaos_ip_");
		handler.AppendFormatted(nextOffset.Value);
		handler.AppendLiteral(";");
		stringBuilder6.AppendLine(ref handler);
	}

	private static void EmitUnsignedComparisonBranch(StringBuilder builder, string comparisonOperator, AotCoreIrInstructionArtifact instruction, int? nextOffset, IReadOnlySet<int> offsets)
	{
		if (!nextOffset.HasValue)
		{
			throw new InvalidOperationException("branch opcode '" + instruction.Op + "' cannot terminate the method");
		}
		int requiredBranchTarget = GetRequiredBranchTarget(instruction, offsets);
		builder.AppendLine("    {");
		builder.AppendLine("        const auto chaos_right = static_cast<std::uint32_t>(static_cast<std::int32_t>(chaos_eval_stack[--chaos_stack_top]));");
		builder.AppendLine("        const auto chaos_left = static_cast<std::uint32_t>(static_cast<std::int32_t>(chaos_eval_stack[--chaos_stack_top]));");
		StringBuilder stringBuilder = builder;
		StringBuilder stringBuilder2 = stringBuilder;
		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(36, 1, stringBuilder);
		handler.AppendLiteral("        if (chaos_left ");
		handler.AppendFormatted(comparisonOperator);
		handler.AppendLiteral(" chaos_right)");
		stringBuilder2.AppendLine(ref handler);
		builder.AppendLine("        {");
		stringBuilder = builder;
		StringBuilder stringBuilder3 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(27, 1, stringBuilder);
		handler.AppendLiteral("            goto chaos_ip_");
		handler.AppendFormatted(requiredBranchTarget);
		handler.AppendLiteral(";");
		stringBuilder3.AppendLine(ref handler);
		builder.AppendLine("        }");
		builder.AppendLine("    }");
		stringBuilder = builder;
		StringBuilder stringBuilder4 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(19, 1, stringBuilder);
		handler.AppendLiteral("    goto chaos_ip_");
		handler.AppendFormatted(nextOffset.Value);
		handler.AppendLiteral(";");
		stringBuilder4.AppendLine(ref handler);
	}

	private static void EmitTruthBranch(StringBuilder builder, bool shouldBranchWhenNonZero, AotCoreIrInstructionArtifact instruction, int? nextOffset, IReadOnlySet<int> offsets)
	{
		if (!nextOffset.HasValue)
		{
			throw new InvalidOperationException("branch opcode '" + instruction.Op + "' cannot terminate the method");
		}
		int requiredBranchTarget = GetRequiredBranchTarget(instruction, offsets);
		builder.AppendLine("    {");
		builder.AppendLine("        const auto chaos_condition = chaos_eval_stack[--chaos_stack_top];");
		builder.AppendLine(shouldBranchWhenNonZero ? "        if (chaos_condition != static_cast<std::intptr_t>(0))" : "        if (chaos_condition == static_cast<std::intptr_t>(0))");
		builder.AppendLine("        {");
		StringBuilder stringBuilder = builder;
		StringBuilder stringBuilder2 = stringBuilder;
		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(27, 1, stringBuilder);
		handler.AppendLiteral("            goto chaos_ip_");
		handler.AppendFormatted(requiredBranchTarget);
		handler.AppendLiteral(";");
		stringBuilder2.AppendLine(ref handler);
		builder.AppendLine("        }");
		builder.AppendLine("    }");
		stringBuilder = builder;
		StringBuilder stringBuilder3 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(19, 1, stringBuilder);
		handler.AppendLiteral("    goto chaos_ip_");
		handler.AppendFormatted(nextOffset.Value);
		handler.AppendLiteral(";");
		stringBuilder3.AppendLine(ref handler);
	}

	private static void EmitSwitch(StringBuilder builder, AotCoreIrInstructionArtifact instruction, int? nextOffset, IReadOnlySet<int> offsets)
	{
		if (!nextOffset.HasValue)
		{
			throw new InvalidOperationException("branch opcode '" + instruction.Op + "' cannot terminate the method");
		}
		IReadOnlyList<int> requiredSwitchTargets = GetRequiredSwitchTargets(instruction, offsets);
		builder.AppendLine("    {");
		builder.AppendLine("        const auto chaos_switch_value = static_cast<std::int32_t>(chaos_eval_stack[--chaos_stack_top]);");
		StringBuilder stringBuilder;
		StringBuilder.AppendInterpolatedStringHandler handler;
		if (requiredSwitchTargets.Count == 0)
		{
			stringBuilder = builder;
			StringBuilder stringBuilder2 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(23, 1, stringBuilder);
			handler.AppendLiteral("        goto chaos_ip_");
			handler.AppendFormatted(nextOffset.Value);
			handler.AppendLiteral(";");
			stringBuilder2.AppendLine(ref handler);
			builder.AppendLine("    }");
			return;
		}
		builder.AppendLine("        switch (chaos_switch_value)");
		builder.AppendLine("        {");
		for (int i = 0; i < requiredSwitchTargets.Count; i++)
		{
			stringBuilder = builder;
			StringBuilder stringBuilder3 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(18, 1, stringBuilder);
			handler.AppendLiteral("            case ");
			handler.AppendFormatted(i);
			handler.AppendLiteral(":");
			stringBuilder3.AppendLine(ref handler);
			stringBuilder = builder;
			StringBuilder stringBuilder4 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(31, 1, stringBuilder);
			handler.AppendLiteral("                goto chaos_ip_");
			handler.AppendFormatted(requiredSwitchTargets[i]);
			handler.AppendLiteral(";");
			stringBuilder4.AppendLine(ref handler);
		}
		builder.AppendLine("            default:");
		stringBuilder = builder;
		StringBuilder stringBuilder5 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(31, 1, stringBuilder);
		handler.AppendLiteral("                goto chaos_ip_");
		handler.AppendFormatted(nextOffset.Value);
		handler.AppendLiteral(";");
		stringBuilder5.AppendLine(ref handler);
		builder.AppendLine("        }");
		builder.AppendLine("    }");
	}

	private static void EmitComparisonResult(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string operandType, string comparisonOperator, int? nextOffset, string op)
	{
		RequireInt32IntegralResultType(instruction);
		builder.AppendLine("    {");
		StringBuilder stringBuilder = builder;
		StringBuilder stringBuilder2 = stringBuilder;
		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(84, 1, stringBuilder);
		handler.AppendLiteral("        const auto chaos_right = static_cast<");
		handler.AppendFormatted(operandType);
		handler.AppendLiteral(">(chaos_eval_stack[--chaos_stack_top]);");
		stringBuilder2.AppendLine(ref handler);
		stringBuilder = builder;
		StringBuilder stringBuilder3 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(83, 1, stringBuilder);
		handler.AppendLiteral("        const auto chaos_left = static_cast<");
		handler.AppendFormatted(operandType);
		handler.AppendLiteral(">(chaos_eval_stack[--chaos_stack_top]);");
		stringBuilder3.AppendLine(ref handler);
		stringBuilder = builder;
		StringBuilder stringBuilder4 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(106, 1, stringBuilder);
		handler.AppendLiteral("        chaos_eval_stack[chaos_stack_top++] = static_cast<std::intptr_t>(chaos_left ");
		handler.AppendFormatted(comparisonOperator);
		handler.AppendLiteral(" chaos_right ? 1 : 0);");
		stringBuilder4.AppendLine(ref handler);
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}

	private static void AppendGotoNext(StringBuilder builder, int? nextOffset, string op)
	{
		if (!nextOffset.HasValue)
		{
			throw new InvalidOperationException("opcode '" + op + "' cannot be the final instruction in the method");
		}
		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(19, 1, builder);
		handler.AppendLiteral("    goto chaos_ip_");
		handler.AppendFormatted(nextOffset.Value);
		handler.AppendLiteral(";");
		builder.AppendLine(ref handler);
	}

	private void EmitInstructionRange(StringBuilder builder, AotCoreIrMethodArtifact method, IReadOnlyList<AotCoreIrInstructionArtifact> instructions, IReadOnlyDictionary<int, int?> nextOffsetsByIlOffset, IReadOnlySet<int> offsets)
	{
		foreach (AotCoreIrInstructionArtifact instruction in instructions)
		{
			int requiredIlOffset = GetRequiredIlOffset(instruction);
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(10, 1, builder);
			handler.AppendLiteral("chaos_ip_");
			handler.AppendFormatted(requiredIlOffset);
			handler.AppendLiteral(":");
			builder.AppendLine(ref handler);
			EmitInstruction(builder, method, instruction, nextOffsetsByIlOffset[requiredIlOffset], offsets);
			builder.AppendLine();
		}
	}

}