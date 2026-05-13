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
	private int _linearScratchCounter;

	// Parallel type tracking for structured IR slot emission.
	// Mirrors the eval stack: each entry tracks whether the slot
	// contains Float32/Float64 (stored via ChaosStoreFloat32/64) or
	// NativeInt (plain integer-as-pointer).  Consumers (conv.i4,
	// ceq, etc.) use this to emit chaos_load_float32/ChaosLoadFloat64
	// before operating on the value.
	private readonly Stack<string> _structuredSlotTypes = new();

	private string PeekSlotType()
		=> _activeStructuredSlotContext is not null && _structuredSlotTypes.Count > 0
			? _structuredSlotTypes.Peek()
			: "NativeInt";

	private void PushSlotType(string type)
	{
		if (_activeStructuredSlotContext is not null)
			_structuredSlotTypes.Push(type);
	}

	private string ConsumeSlotType()
	{
		if (_activeStructuredSlotContext is not null && _structuredSlotTypes.Count > 0)
			return _structuredSlotTypes.Pop();
		return "NativeInt";
	}

	private void UpdateSlotType(string type)
	{
		if (_activeStructuredSlotContext is not null && _structuredSlotTypes.Count > 0)
		{
			_structuredSlotTypes.Pop();
			_structuredSlotTypes.Push(type);
		}
	}

	private string AllocateLinearScratchName(string prefix)
		=> "chaos_" + prefix + "_" + (_linearScratchCounter++).ToString(CultureInfo.InvariantCulture);


	private void EmitInstructionSequence(StringBuilder builder, IReadOnlyList<AotCoreIrInstructionArtifact> instructions, string indentation)
	{
		foreach (AotCoreIrInstructionArtifact instruction in instructions)
		{
			EmitInstruction(builder, instruction, indentation);
		}
	}

	private void EmitStructuredFinallyHandlerSequence(StringBuilder builder, FinallyHandlerShape handlerShape, string indentation)
	{
		if (!TryCreateFinallyHandlerEmissionPlan(handlerShape, out FinallyHandlerEmissionPlan? emissionPlan) || emissionPlan is null)
		{
			throw new NotSupportedException($"native-aot finally handler at IL offset {handlerShape.ExceptionRegion.HandlerOffset} does not support current structured emission shape");
		}
		if (emissionPlan.Guard is null)
		{
			EmitInstructionSequence(builder, emissionPlan.BodyInstructions, indentation);
			return;
		}
		EmitInstructionSequence(builder, emissionPlan.Guard.ConditionInstructions, indentation);
		StringBuilder stringBuilder = builder;
		StringBuilder stringBuilder2 = stringBuilder;
		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(1, 1, stringBuilder);
		handler.AppendFormatted(indentation);
		handler.AppendLiteral("{");
		stringBuilder2.AppendLine(ref handler);
		stringBuilder = builder;
		StringBuilder stringBuilder3 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(77, 1, stringBuilder);
		handler.AppendFormatted(indentation);
		handler.AppendLiteral("    const auto chaos_finally_condition = chaos_eval_stack[--chaos_stack_top];");
		stringBuilder3.AppendLine(ref handler);
		builder.AppendLine(emissionPlan.Guard.BranchWhenNonZeroToEnd ? (indentation + "    if (chaos_finally_condition == static_cast<CHAOS_IL2CPP_INTPTR>(0))") : (indentation + "    if (chaos_finally_condition != static_cast<CHAOS_IL2CPP_INTPTR>(0))"));
		stringBuilder = builder;
		StringBuilder stringBuilder4 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(5, 1, stringBuilder);
		handler.AppendFormatted(indentation);
		handler.AppendLiteral("    {");
		stringBuilder4.AppendLine(ref handler);
		EmitInstructionSequence(builder, emissionPlan.BodyInstructions, indentation + "        ");
		stringBuilder = builder;
		StringBuilder stringBuilder5 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(5, 1, stringBuilder);
		handler.AppendFormatted(indentation);
		handler.AppendLiteral("    }");
		stringBuilder5.AppendLine(ref handler);
		stringBuilder = builder;
		StringBuilder stringBuilder6 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(1, 1, stringBuilder);
		handler.AppendFormatted(indentation);
		handler.AppendLiteral("}");
		stringBuilder6.AppendLine(ref handler);
	}

	private void EmitInstruction(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string indentation)
	{
		switch (instruction.Op)
		{
		case "ldc.i4":
		{
			EmitEvalStackPush(builder, indentation, $"static_cast<CHAOS_IL2CPP_INTPTR>({FormatInt32Literal(GetRequiredIntOperand(instruction))})");
			break;
		}
		case "ldc.i8":
		{
			EmitEvalStackPush(builder, indentation, $"ChaosStoreInt64({FormatInt64Literal(GetRequiredInt64Operand(instruction))})");
			break;
		}
		case "ldc.r8":
		{
			EmitEvalStackPush(builder, indentation, $"ChaosStoreFloat64({FormatFloat64Literal(GetRequiredDoubleOperand(instruction))})");
			PushSlotType("Float64");
			break;
		}
		case "ldc.r4":
		{
			EmitEvalStackPush(builder, indentation, $"ChaosStoreFloat32({FormatFloat32Literal(GetRequiredSingleOperand(instruction))})");
			PushSlotType("Float32");
			break;
		}
		case "ldarg":
		{
			EmitEvalStackPush(builder, indentation, $"chaos_args[{GetRequiredIntOperand(instruction)}]");
			break;
		}
		case "ldstr":
			EmitLinearLoadStringLiteral(builder, instruction, indentation);
			break;
		case "ldtoken":
			EmitLinearLoadTypeToken(builder, instruction, indentation);
			break;
		case "ldloc":
		{
			EmitEvalStackPush(builder, indentation, $"chaos_locals[{GetRequiredIntOperand(instruction)}]");
			break;
		}
		case "ldsfld":
		case "ldfld":
			EmitLinearFieldLoad(builder, instruction, indentation);
			break;
		case "call":
			EmitLinearCall(builder, instruction, indentation);
			break;
		case "callvirt":
			EmitLinearCallVirt(builder, instruction, indentation);
			break;
		case "newobj":
			EmitLinearNewObject(builder, instruction, indentation);
			break;
		case "ldftn":
		{
			var targetSymbol = GetRequiredFunctionPointerTargetSymbol(instruction);
			EmitEvalStackPush(builder, indentation, $"reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&{targetSymbol})");
			break;
		}
		case "stloc":
		{
			builder.AppendLine($"{indentation}chaos_locals[{GetRequiredIntOperand(instruction)}] = {ConsumeEvalStackValueExpression()};");
			break;
		}
		case "starg":
		{
			builder.AppendLine($"{indentation}chaos_args[{GetRequiredIntOperand(instruction)}] = {ConsumeEvalStackValueExpression()};");
			break;
		}
		case "pop":
		{
			EmitEvalStackDiscard(builder, indentation);
			break;
		}
		case "ldnull":
		{
			EmitEvalStackPush(builder, indentation, "static_cast<CHAOS_IL2CPP_INTPTR>(0)");
			break;
		}
		case "cgt.un":
		{
			builder.AppendLine($"{indentation}{{");
			builder.AppendLine($"{indentation}    const auto chaos_right = static_cast<CHAOS_IL2CPP_UINTPTR>({ConsumeEvalStackValueExpression()});");
			builder.AppendLine($"{indentation}    const auto chaos_left = static_cast<CHAOS_IL2CPP_UINTPTR>({ConsumeEvalStackValueExpression()});");
			EmitEvalStackPush(builder, indentation + "    ", "static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left > chaos_right ? 1 : 0)");
			builder.AppendLine($"{indentation}}}");
			break;
		}
		case "ceq":
		{
			string _rType = PeekSlotType();
			string _rExpr = ConsumeEvalStackValueExpression();
			string _lType = PeekSlotType();
			string _lExpr = ConsumeEvalStackValueExpression();
			string _rLoad = _rType switch
			{
				"Float32" => $"chaos_load_float32({_rExpr})",
				"Float64" => $"ChaosLoadFloat64({_rExpr})",
				_ => $"static_cast<CHAOS_IL2CPP_INTPTR>({_rExpr})",
			};
			string _lLoad = _lType switch
			{
				"Float32" => $"chaos_load_float32({_lExpr})",
				"Float64" => $"ChaosLoadFloat64({_lExpr})",
				_ => $"static_cast<CHAOS_IL2CPP_INTPTR>({_lExpr})",
			};
			builder.AppendLine($"{indentation}{{");
			builder.AppendLine($"{indentation}    const auto chaos_right = {_rLoad};");
			builder.AppendLine($"{indentation}    const auto chaos_left = {_lLoad};");
			EmitEvalStackPush(builder, indentation + "    ", $"static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0)");
			builder.AppendLine($"{indentation}}}");
			PushSlotType("NativeInt");
			break;
		}
		case "cgt":
		{
			string _rType = PeekSlotType();
			string _rExpr = ConsumeEvalStackValueExpression();
			string _lType = PeekSlotType();
			string _lExpr = ConsumeEvalStackValueExpression();
			string _rLoad = _rType switch
			{
				"Float32" => $"chaos_load_float32({_rExpr})",
				"Float64" => $"ChaosLoadFloat64({_rExpr})",
				_ => $"static_cast<CHAOS_IL2CPP_INT32>({_rExpr})",
			};
			string _lLoad = _lType switch
			{
				"Float32" => $"chaos_load_float32({_lExpr})",
				"Float64" => $"ChaosLoadFloat64({_lExpr})",
				_ => $"static_cast<CHAOS_IL2CPP_INT32>({_lExpr})",
			};
			builder.AppendLine($"{indentation}{{");
			builder.AppendLine($"{indentation}    const auto chaos_right = {_rLoad};");
			builder.AppendLine($"{indentation}    const auto chaos_left = {_lLoad};");
			EmitEvalStackPush(builder, indentation + "    ", $"static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left > chaos_right ? 1 : 0)");
			builder.AppendLine($"{indentation}}}");
			PushSlotType("NativeInt");
			break;
		}
		case "clt":
		case "clt.un":
		{
			string _rType = PeekSlotType();
			string _rExpr = ConsumeEvalStackValueExpression();
			string _lType = PeekSlotType();
			string _lExpr = ConsumeEvalStackValueExpression();
			string _rLoad = _rType switch
			{
				"Float32" => $"chaos_load_float32({_rExpr})",
				"Float64" => $"ChaosLoadFloat64({_rExpr})",
				_ => $"static_cast<CHAOS_IL2CPP_INT32>({_rExpr})",
			};
			string _lLoad = _lType switch
			{
				"Float32" => $"chaos_load_float32({_lExpr})",
				"Float64" => $"ChaosLoadFloat64({_lExpr})",
				_ => $"static_cast<CHAOS_IL2CPP_INT32>({_lExpr})",
			};
			builder.AppendLine($"{indentation}{{");
			builder.AppendLine($"{indentation}    const auto chaos_right = {_rLoad};");
			builder.AppendLine($"{indentation}    const auto chaos_left = {_lLoad};");
			EmitEvalStackPush(builder, indentation + "    ", $"static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left < chaos_right ? 1 : 0)");
			builder.AppendLine($"{indentation}}}");
			PushSlotType("NativeInt");
			break;
		}
		case "add":
			EmitLinearBinaryArithmetic(builder, indentation, "ChaosWrapAdd");
			break;
		case "sub":
			EmitLinearBinaryArithmetic(builder, indentation, "ChaosWrapSub");
			break;
		case "add.ovf":
		case "add.ovf.un":
			EmitLinearBinaryArithmetic(builder, indentation, "ChaosWrapAdd");
			break;
		case "sub.ovf":
			EmitLinearBinaryArithmetic(builder, indentation, "ChaosWrapSub");
			break;
		case "sub.ovf.un":
			EmitLinearBinaryArithmetic(builder, indentation, "ChaosWrapSub");
			break;
		case "mul.ovf":
		case "mul.ovf.un":
			EmitLinearBinaryArithmetic(builder, indentation, "ChaosWrapMul");
			break;
		case "mul":
			EmitLinearBinaryArithmetic(builder, indentation, "ChaosWrapMul");
			break;
		case "div":
		case "div.un":
			EmitLinearBinaryArithmetic(builder, indentation, "ChaosDiv");
			break;
		case "rem":
		case "rem.un":
			EmitLinearBinaryArithmetic(builder, indentation, "ChaosRem");
			break;
		case "shl":
			EmitLinearShift(builder, instruction, indentation, "chaos_shift_left_int32");
			break;
		case "shr":
			EmitLinearShift(builder, instruction, indentation, "chaos_shift_right_int32");
			break;
		case "shr.un":
			EmitLinearShift(builder, instruction, indentation, "chaos_shift_right_un_int32");
			break;
		case "not":
			EmitLinearBitwiseNot(builder, instruction, indentation);
			break;
		case "and":
			EmitLinearBinaryBitwise(builder, indentation, "&");
			break;
		case "or":
			EmitLinearBinaryBitwise(builder, indentation, "|");
			break;
		case "xor":
			EmitLinearBinaryBitwise(builder, indentation, "^");
			break;

		case "ldlen":
		{
			EmitEvalStackPush(builder, indentation, $"static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>({ConsumeEvalStackValueExpression()}))");
			break;
		}
		case "dup":
		{
			string source = AccessEvalStackTopExpression();
			string destination = AllocateEvalStackTargetExpression();
			builder.AppendLine($"{indentation}{destination} = {source};");
			break;
		}
		case "neg":
		{
			builder.AppendLine($"{indentation}{AccessEvalStackTopExpression()} = static_cast<CHAOS_IL2CPP_INTPTR>(-static_cast<CHAOS_IL2CPP_INT32>({AccessEvalStackTopExpression()}));");
			break;
		}
		case "conv.i4":
		{
			string _slotType = PeekSlotType();
			string _loadExpr = _slotType switch
			{
				"Float32" => $"chaos_load_float32({AccessEvalStackTopExpression()})",
				"Float64" => $"ChaosLoadFloat64({AccessEvalStackTopExpression()})",
				_ => AccessEvalStackTopExpression(),
			};
			builder.AppendLine($"{indentation}{AccessEvalStackTopExpression()} = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>({_loadExpr}));");
			UpdateSlotType("NativeInt");
			break;
		}
		case "conv.u4":
		{
			string _slotType = PeekSlotType();
			string _loadExpr = _slotType switch
			{
				"Float32" => $"chaos_load_float32({AccessEvalStackTopExpression()})",
				"Float64" => $"ChaosLoadFloat64({AccessEvalStackTopExpression()})",
				_ => AccessEvalStackTopExpression(),
			};
			builder.AppendLine($"{indentation}{AccessEvalStackTopExpression()} = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINT32>({_loadExpr}));");
			UpdateSlotType("NativeInt");
			break;
		}
		case "conv.i1":
		case "conv.u1":
		{
			string _slotType = PeekSlotType();
			string _loadExpr = _slotType switch
			{
				"Float32" => $"chaos_load_float32({AccessEvalStackTopExpression()})",
				"Float64" => $"ChaosLoadFloat64({AccessEvalStackTopExpression()})",
				_ => AccessEvalStackTopExpression(),
			};
			builder.AppendLine($"{indentation}{AccessEvalStackTopExpression()} = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT8>({_loadExpr}));");
			UpdateSlotType("NativeInt");
			break;
		}
		case "conv.i2":
		case "conv.u2":
		{
			string _slotType = PeekSlotType();
			string _loadExpr = _slotType switch
			{
				"Float32" => $"chaos_load_float32({AccessEvalStackTopExpression()})",
				"Float64" => $"ChaosLoadFloat64({AccessEvalStackTopExpression()})",
				_ => AccessEvalStackTopExpression(),
			};
			builder.AppendLine($"{indentation}{AccessEvalStackTopExpression()} = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT16>({_loadExpr}));");
			UpdateSlotType("NativeInt");
			break;
		}
		case "conv.i8":
		case "conv.u8":
		{
			string _slotType = PeekSlotType();
			string _loadExpr = _slotType switch
			{
				"Float32" => $"chaos_load_float32({AccessEvalStackTopExpression()})",
				"Float64" => $"ChaosLoadFloat64({AccessEvalStackTopExpression()})",
				_ => AccessEvalStackTopExpression(),
			};
			builder.AppendLine($"{indentation}{AccessEvalStackTopExpression()} = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>({_loadExpr}));");
			break;
		}
		case "conv.r4":
		{
			builder.AppendLine($"{indentation}{AccessEvalStackTopExpression()} = ChaosStoreFloat32(static_cast<float>({AccessEvalStackTopExpression()}));");
			break;
		}
		case "conv.r8":
		{
			builder.AppendLine($"{indentation}{AccessEvalStackTopExpression()} = ChaosStoreFloat64(static_cast<double>({AccessEvalStackTopExpression()}));");
			break;
		}
		case "conv.u":
		{
			string _slotType = PeekSlotType();
			string _loadExpr = _slotType switch
			{
				"Float32" => $"chaos_load_float32({AccessEvalStackTopExpression()})",
				"Float64" => $"ChaosLoadFloat64({AccessEvalStackTopExpression()})",
				_ => AccessEvalStackTopExpression(),
			};
			builder.AppendLine($"{indentation}{AccessEvalStackTopExpression()} = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>({_loadExpr}));");
			UpdateSlotType("NativeInt");
			break;
		}
		case "conv.i":
		{
			string _slotType = PeekSlotType();
			string _loadExpr = _slotType switch
			{
				"Float32" => $"chaos_load_float32({AccessEvalStackTopExpression()})",
				"Float64" => $"ChaosLoadFloat64({AccessEvalStackTopExpression()})",
				_ => AccessEvalStackTopExpression(),
			};
			builder.AppendLine($"{indentation}{AccessEvalStackTopExpression()} = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>({_loadExpr}));");
			UpdateSlotType("NativeInt");
			break;
		}
		case "ckfinite":
		{
			builder.AppendLine($"{indentation}{{");
			builder.AppendLine($"{indentation}    const auto chaos_fp = ChaosLoadFloat64({AccessEvalStackTopExpression()});");
			builder.AppendLine($"{indentation}    if (!std::isfinite(chaos_fp)) {{ CHAOS_IL2CPP_FAIL(); }}");
			builder.AppendLine($"{indentation}}}");
			break;
		}
		case "conv.ovf.i1":
		case "conv.ovf.u1":
		{
			string _slotType = PeekSlotType();
			string _loadExpr = _slotType switch
			{
				"Float32" => $"chaos_load_float32({AccessEvalStackTopExpression()})",
				"Float64" => $"ChaosLoadFloat64({AccessEvalStackTopExpression()})",
				_ => AccessEvalStackTopExpression(),
			};
			builder.AppendLine($"{indentation}{AccessEvalStackTopExpression()} = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT8>({_loadExpr}));");
			UpdateSlotType("NativeInt");
			break;
		}
		case "conv.ovf.i2":
		case "conv.ovf.u2":
		{
			string _slotType = PeekSlotType();
			string _loadExpr = _slotType switch
			{
				"Float32" => $"chaos_load_float32({AccessEvalStackTopExpression()})",
				"Float64" => $"ChaosLoadFloat64({AccessEvalStackTopExpression()})",
				_ => AccessEvalStackTopExpression(),
			};
			builder.AppendLine($"{indentation}{AccessEvalStackTopExpression()} = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT16>({_loadExpr}));");
			UpdateSlotType("NativeInt");
			break;
		}
		case "conv.ovf.i4":
		case "conv.ovf.u4":
		{
			string _slotType = PeekSlotType();
			string _loadExpr = _slotType switch
			{
				"Float32" => $"chaos_load_float32({AccessEvalStackTopExpression()})",
				"Float64" => $"ChaosLoadFloat64({AccessEvalStackTopExpression()})",
				_ => AccessEvalStackTopExpression(),
			};
			builder.AppendLine($"{indentation}{AccessEvalStackTopExpression()} = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>({_loadExpr}));");
			UpdateSlotType("NativeInt");
			break;
		}
		case "conv.ovf.i8":
		{
			string _slotType = PeekSlotType();
			string _loadExpr = _slotType switch
			{
				"Float32" => $"chaos_load_float32({AccessEvalStackTopExpression()})",
				"Float64" => $"ChaosLoadFloat64({AccessEvalStackTopExpression()})",
				_ => AccessEvalStackTopExpression(),
			};
			builder.AppendLine($"{indentation}{AccessEvalStackTopExpression()} = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>({_loadExpr}));");
			break;
		}
		case "conv.ovf.u8":
		{
			string _slotType = PeekSlotType();
			string _loadExpr = _slotType switch
			{
				"Float32" => $"chaos_load_float32({AccessEvalStackTopExpression()})",
				"Float64" => $"ChaosLoadFloat64({AccessEvalStackTopExpression()})",
				_ => AccessEvalStackTopExpression(),
			};
			builder.AppendLine($"{indentation}{AccessEvalStackTopExpression()} = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(static_cast<CHAOS_IL2CPP_UINT64>({_loadExpr})));");
			break;
		}
		case "conv.ovf.i":
		{
			string _slotType = PeekSlotType();
			string _loadExpr = _slotType switch
			{
				"Float32" => $"chaos_load_float32({AccessEvalStackTopExpression()})",
				"Float64" => $"ChaosLoadFloat64({AccessEvalStackTopExpression()})",
				_ => AccessEvalStackTopExpression(),
			};
			builder.AppendLine($"{indentation}{AccessEvalStackTopExpression()} = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>({_loadExpr}));");
			UpdateSlotType("NativeInt");
			break;
		}
		case "conv.ovf.i.un":
		{
			string _slotType = PeekSlotType();
			string _loadExpr = _slotType switch
			{
				"Float32" => $"chaos_load_float32({AccessEvalStackTopExpression()})",
				"Float64" => $"ChaosLoadFloat64({AccessEvalStackTopExpression()})",
				_ => $"static_cast<CHAOS_IL2CPP_UINTPTR>({AccessEvalStackTopExpression()})",
			};
			builder.AppendLine($"{indentation}{{");
			builder.AppendLine($"{indentation}    const auto chaos_val = {_loadExpr};");
			builder.AppendLine($"{indentation}    if (chaos_val > static_cast<CHAOS_IL2CPP_UINTPTR>(CHAOS_IL2CPP_INTPTR_MAX)) {{ CHAOS_IL2CPP_FAIL(); }}");
			builder.AppendLine($"{indentation}    {AccessEvalStackTopExpression()} = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_val);");
			builder.AppendLine($"{indentation}}}");
			break;
		}
		case "conv.ovf.u":
		{
			string _slotType = PeekSlotType();
			string _loadExpr = _slotType switch
			{
				"Float32" => $"chaos_load_float32({AccessEvalStackTopExpression()})",
				"Float64" => $"ChaosLoadFloat64({AccessEvalStackTopExpression()})",
				_ => AccessEvalStackTopExpression(),
			};
			builder.AppendLine($"{indentation}{AccessEvalStackTopExpression()} = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>({_loadExpr}));");
			break;
		}
		case "conv.ovf.i8.un":
		{
			string _slotType = PeekSlotType();
			string _loadExpr = _slotType switch
			{
				"Float32" => $"chaos_load_float32({AccessEvalStackTopExpression()})",
				"Float64" => $"ChaosLoadFloat64({AccessEvalStackTopExpression()})",
				_ => $"static_cast<CHAOS_IL2CPP_UINTPTR>({AccessEvalStackTopExpression()})",
			};
			builder.AppendLine($"{indentation}{{");
			builder.AppendLine($"{indentation}    const auto chaos_val = {_loadExpr};");
			builder.AppendLine($"{indentation}    if (chaos_val > static_cast<CHAOS_IL2CPP_UINTPTR>(CHAOS_IL2CPP_INT64_MAX)) {{ CHAOS_IL2CPP_FAIL(); }}");
			builder.AppendLine($"{indentation}    {AccessEvalStackTopExpression()} = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(chaos_val));");
			builder.AppendLine($"{indentation}}}");
			break;
		}
		case "conv.ovf.u8.un":
		{
			string _slotType = PeekSlotType();
			string _loadExpr = _slotType switch
			{
				"Float32" => $"chaos_load_float32({AccessEvalStackTopExpression()})",
				"Float64" => $"ChaosLoadFloat64({AccessEvalStackTopExpression()})",
				_ => $"static_cast<CHAOS_IL2CPP_UINTPTR>({AccessEvalStackTopExpression()})",
			};
			builder.AppendLine($"{indentation}{AccessEvalStackTopExpression()} = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(static_cast<CHAOS_IL2CPP_UINT64>({_loadExpr})));");
			break;
		}
		case "conv.ovf.i1.un":
		{
			string _slotType = PeekSlotType();
			string _loadExpr = _slotType switch
			{
				"Float32" => $"chaos_load_float32({AccessEvalStackTopExpression()})",
				"Float64" => $"ChaosLoadFloat64({AccessEvalStackTopExpression()})",
				_ => $"static_cast<CHAOS_IL2CPP_UINTPTR>({AccessEvalStackTopExpression()})",
			};
			builder.AppendLine($"{indentation}{{");
			builder.AppendLine($"{indentation}    const auto chaos_val = {_loadExpr};");
			builder.AppendLine($"{indentation}    if (chaos_val > static_cast<CHAOS_IL2CPP_UINTPTR>(CHAOS_IL2CPP_INT8_MAX)) {{ CHAOS_IL2CPP_FAIL(); }}");
			builder.AppendLine($"{indentation}    {AccessEvalStackTopExpression()} = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT8>(chaos_val));");
			builder.AppendLine($"{indentation}}}");
			break;
		}
		case "conv.ovf.i2.un":
		{
			string _slotType = PeekSlotType();
			string _loadExpr = _slotType switch
			{
				"Float32" => $"chaos_load_float32({AccessEvalStackTopExpression()})",
				"Float64" => $"ChaosLoadFloat64({AccessEvalStackTopExpression()})",
				_ => $"static_cast<CHAOS_IL2CPP_UINTPTR>({AccessEvalStackTopExpression()})",
			};
			builder.AppendLine($"{indentation}{{");
			builder.AppendLine($"{indentation}    const auto chaos_val = {_loadExpr};");
			builder.AppendLine($"{indentation}    if (chaos_val > static_cast<CHAOS_IL2CPP_UINTPTR>(CHAOS_IL2CPP_INT16_MAX)) {{ CHAOS_IL2CPP_FAIL(); }}");
			builder.AppendLine($"{indentation}    {AccessEvalStackTopExpression()} = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT16>(chaos_val));");
			builder.AppendLine($"{indentation}}}");
			break;
		}
		case "conv.ovf.i4.un":
		{
			string _slotType = PeekSlotType();
			string _loadExpr = _slotType switch
			{
				"Float32" => $"chaos_load_float32({AccessEvalStackTopExpression()})",
				"Float64" => $"ChaosLoadFloat64({AccessEvalStackTopExpression()})",
				_ => $"static_cast<CHAOS_IL2CPP_UINTPTR>({AccessEvalStackTopExpression()})",
			};
			builder.AppendLine($"{indentation}{{");
			builder.AppendLine($"{indentation}    const auto chaos_val = {_loadExpr};");
			builder.AppendLine($"{indentation}    if (chaos_val > static_cast<CHAOS_IL2CPP_UINTPTR>(CHAOS_IL2CPP_INT32_MAX)) {{ CHAOS_IL2CPP_FAIL(); }}");
			builder.AppendLine($"{indentation}    {AccessEvalStackTopExpression()} = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(chaos_val));");
			builder.AppendLine($"{indentation}}}");
			break;
		}
		case "conv.ovf.u1.un":
		{
			string _slotType = PeekSlotType();
			string _loadExpr = _slotType switch
			{
				"Float32" => $"chaos_load_float32({AccessEvalStackTopExpression()})",
				"Float64" => $"ChaosLoadFloat64({AccessEvalStackTopExpression()})",
				_ => $"static_cast<CHAOS_IL2CPP_UINTPTR>({AccessEvalStackTopExpression()})",
			};
			builder.AppendLine($"{indentation}{{");
			builder.AppendLine($"{indentation}    const auto chaos_val = {_loadExpr};");
			builder.AppendLine($"{indentation}    if (chaos_val > static_cast<CHAOS_IL2CPP_UINTPTR>(CHAOS_IL2CPP_UINT8_MAX)) {{ CHAOS_IL2CPP_FAIL(); }}");
			builder.AppendLine($"{indentation}    {AccessEvalStackTopExpression()} = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINT8>(chaos_val));");
			builder.AppendLine($"{indentation}}}");
			break;
		}
		case "conv.ovf.u2.un":
		{
			string _slotType = PeekSlotType();
			string _loadExpr = _slotType switch
			{
				"Float32" => $"chaos_load_float32({AccessEvalStackTopExpression()})",
				"Float64" => $"ChaosLoadFloat64({AccessEvalStackTopExpression()})",
				_ => $"static_cast<CHAOS_IL2CPP_UINTPTR>({AccessEvalStackTopExpression()})",
			};
			builder.AppendLine($"{indentation}{{");
			builder.AppendLine($"{indentation}    const auto chaos_val = {_loadExpr};");
			builder.AppendLine($"{indentation}    if (chaos_val > static_cast<CHAOS_IL2CPP_UINTPTR>(CHAOS_IL2CPP_UINT16_MAX)) {{ CHAOS_IL2CPP_FAIL(); }}");
			builder.AppendLine($"{indentation}    {AccessEvalStackTopExpression()} = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINT16>(chaos_val));");
			builder.AppendLine($"{indentation}}}");
			break;
		}
		case "conv.ovf.u4.un":
		{
			builder.AppendLine($"{indentation}{{");
			builder.AppendLine($"{indentation}    const auto chaos_val = static_cast<CHAOS_IL2CPP_UINTPTR>({AccessEvalStackTopExpression()});");
			builder.AppendLine($"{indentation}    if (chaos_val > static_cast<CHAOS_IL2CPP_UINTPTR>(CHAOS_IL2CPP_UINT32_MAX)) {{ CHAOS_IL2CPP_FAIL(); }}");
			builder.AppendLine($"{indentation}    {AccessEvalStackTopExpression()} = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINT32>(chaos_val));");
			builder.AppendLine($"{indentation}}}");
			break;
		}
		case "conv.ovf.u.un":
		{
			builder.AppendLine($"{indentation}{AccessEvalStackTopExpression()} = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>({AccessEvalStackTopExpression()}));");
			break;
		}

		case "conv.r.un":
		{
			builder.AppendLine($"{indentation}{AccessEvalStackTopExpression()} = ChaosStoreFloat32(static_cast<float>(static_cast<CHAOS_IL2CPP_UINTPTR>({AccessEvalStackTopExpression()})));");
			PushSlotType("Float32");
			break;
		}
		case "ldloca":
		{
			EmitEvalStackPush(builder, indentation, $"&chaos_locals[{GetRequiredIntOperand(instruction)}]");
			break;
		}
		case "ldarga":
		{
			EmitEvalStackPush(builder, indentation, $"&chaos_args[{GetRequiredIntOperand(instruction)}]");
			break;
		}
		case "box":
			EmitLinearBox(builder, instruction, indentation);
			break;
		case "unbox":
			EmitLinearUnbox(builder, instruction, indentation);
			break;
		case "unbox.any":
			EmitLinearUnboxAny(builder, instruction, indentation);
			break;
		case "castclass":
			EmitLinearCastClass(builder, instruction, indentation);
			break;
		case "isinst":
			EmitLinearIsInst(builder, instruction, indentation);
			break;
		case "initobj":
			EmitLinearInitObj(builder, instruction, indentation);
			break;
		case "newarr":
			EmitLinearNewArray(builder, instruction, indentation);
			break;
		case "ldelema":
			EmitLinearArrayElementAddress(builder, instruction, indentation);
			break;
		case "sizeof":
			EmitEvalStackPush(builder, indentation, "static_cast<CHAOS_IL2CPP_INTPTR>(4)");
			break;
		case "ldind.i1":
			EmitLinearLoadIndirect(builder, "CHAOS_IL2CPP_INT8", "static_cast<CHAOS_IL2CPP_INTPTR>(chaos_value)", indentation);
			break;
		case "ldind.u1":
			EmitLinearLoadIndirect(builder, "CHAOS_IL2CPP_UINT8", "static_cast<CHAOS_IL2CPP_INTPTR>(chaos_value)", indentation);
			break;
		case "ldind.i2":
			EmitLinearLoadIndirect(builder, "CHAOS_IL2CPP_INT16", "static_cast<CHAOS_IL2CPP_INTPTR>(chaos_value)", indentation);
			break;
		case "ldind.u2":
			EmitLinearLoadIndirect(builder, "CHAOS_IL2CPP_UINT16", "static_cast<CHAOS_IL2CPP_INTPTR>(chaos_value)", indentation);
			break;
		case "ldind.i4":
			EmitLinearLoadIndirect(builder, "CHAOS_IL2CPP_INT32", "static_cast<CHAOS_IL2CPP_INTPTR>(chaos_value)", indentation);
			break;
		case "ldind.u4":
			EmitLinearLoadIndirect(builder, "CHAOS_IL2CPP_UINT32", "static_cast<CHAOS_IL2CPP_INTPTR>(chaos_value)", indentation);
			break;
		case "ldind.i8":
			EmitLinearLoadIndirect(builder, "CHAOS_IL2CPP_INT64", "static_cast<CHAOS_IL2CPP_INT64>(chaos_value)", indentation);
			break;
		case "ldind.r4":
			EmitLinearLoadIndirect(builder, "CHAOS_IL2CPP_FLOAT32", "ChaosStoreFloat32(chaos_value)", indentation);
			PushSlotType("Float32");
			break;
		case "ldind.r8":
			EmitLinearLoadIndirect(builder, "CHAOS_IL2CPP_FLOAT64", "ChaosStoreFloat64(chaos_value)", indentation);
			PushSlotType("Float64");
			break;
		case "ldind.ref":
			EmitLinearLoadIndirect(builder, "CHAOS_IL2CPP_INTPTR", "static_cast<CHAOS_IL2CPP_INTPTR>(chaos_value)", indentation);
			break;
		case "ldind.i":
			EmitLinearLoadIndirect(builder, "CHAOS_IL2CPP_INTPTR", "static_cast<CHAOS_IL2CPP_INTPTR>(chaos_value)", indentation);
			break;
		case "stind.i1":
			EmitLinearStoreIndirect(builder, "CHAOS_IL2CPP_INT8", "static_cast<CHAOS_IL2CPP_INT8>(chaos_value_raw)", indentation, materializeString: false);
			break;
		case "stind.i2":
			EmitLinearStoreIndirect(builder, "CHAOS_IL2CPP_INT16", "static_cast<CHAOS_IL2CPP_INT16>(chaos_value_raw)", indentation, materializeString: false);
			break;
		case "stind.i4":
			EmitLinearStoreIndirect(builder, "CHAOS_IL2CPP_INT32", "static_cast<CHAOS_IL2CPP_INT32>(chaos_value_raw)", indentation, materializeString: false);
			break;
		case "stind.i8":
			EmitLinearStoreIndirect(builder, "CHAOS_IL2CPP_INT64", "static_cast<CHAOS_IL2CPP_INT64>(chaos_value_raw)", indentation, materializeString: false);
			break;
		case "stind.r4":
			EmitLinearStoreIndirect(builder, "CHAOS_IL2CPP_FLOAT32", "static_cast<CHAOS_IL2CPP_FLOAT32>(chaos_value_raw)", indentation, materializeString: false);
			break;
		case "stind.r8":
			EmitLinearStoreIndirect(builder, "CHAOS_IL2CPP_FLOAT64", "static_cast<CHAOS_IL2CPP_FLOAT64>(chaos_value_raw)", indentation, materializeString: false);
			break;
		case "stind.ref":
			EmitLinearStoreIndirect(builder, "CHAOS_IL2CPP_INTPTR", "static_cast<CHAOS_IL2CPP_INTPTR>(chaos_value_raw)", indentation, materializeString: true);
			break;
		case "stind.i":
			EmitLinearStoreIndirect(builder, "CHAOS_IL2CPP_INTPTR", "static_cast<CHAOS_IL2CPP_INTPTR>(chaos_value_raw)", indentation, materializeString: false);
			break;
		case "ldflda":
		case "ldsflda":
			EmitLinearFieldAddress(builder, instruction, indentation);
			break;
		case "stsfld":
		{
			var targetRef = GetRequiredTargetReference(instruction);
			if (targetRef.Kind != AotCoreIrReferenceKind.Field)
			{
				throw new NotSupportedException($"native-aot structured EH linear field store requires field target reference, got '{targetRef.Kind}'.");
			}
			builder.AppendLine($"{indentation}{{");
			builder.AppendLine($"{indentation}    auto chaos_value = {ConsumeEvalStackValueExpression()};");
			EmitStaticInitializationForField(builder, targetRef.SubjectId, indentation);
			builder.AppendLine($"{indentation}    {GetNativeStaticFieldSymbol(targetRef.SubjectId)} = chaos_value;");
			builder.AppendLine($"{indentation}}}");
			break;
		}
		case "stfld":
		{
			var targetRef = GetRequiredTargetReference(instruction);
			if (targetRef.Kind != AotCoreIrReferenceKind.Field)
			{
				throw new NotSupportedException($"native-aot structured EH linear field store requires field target reference, got '{targetRef.Kind}'.");
			}

			builder.AppendLine($"{indentation}{{");
			builder.AppendLine($"{indentation}    auto chaos_value = {ConsumeEvalStackValueExpression()};");
			if (_stringIdMapping is { Count: > 0 })
			{
				builder.AppendLine($"{indentation}    if (chaos_is_string_id(chaos_value))");
				builder.AppendLine($"{indentation}    {{");
				builder.AppendLine($"{indentation}        chaos_value = chaos_string_materialize(chaos_value);");
				builder.AppendLine($"{indentation}    }}");
			}

			string declaringTypeSubjectId = GetRequiredDeclaringTypeSubjectId(targetRef);
			if (targetRef.DeclaringTypeShape == AotCoreIrTypeShapeKind.ValueType)
			{
				builder.AppendLine($"{indentation}    auto* chaos_value_owner = chaos_resolve_managed_value_pointer<{GetNativeValueTypeSymbol(declaringTypeSubjectId)}>({ConsumeEvalStackValueExpression()});");
				builder.AppendLine($"{indentation}    chaos_value_owner->{GetNativeFieldMemberName(targetRef.SubjectId)} = chaos_value;");
			}
			else
			{
				builder.AppendLine($"{indentation}    auto* chaos_object = reinterpret_cast<{GetNativeTypeSymbol(declaringTypeSubjectId)}*>({ConsumeEvalStackValueExpression()});");
				builder.AppendLine($"{indentation}    chaos_object->{GetNativeFieldMemberName(targetRef.SubjectId)} = chaos_value;");
				builder.AppendLine($"{indentation}    chaos::il2cpp::runtime_core::DirtyCard(chaos_object);");
			}

			builder.AppendLine($"{indentation}}}");
			break;
		}
		case "cpobj":
			EmitLinearCopyObject(builder, instruction, indentation);
			break;
		case "cpblk":
			EmitLinearCopyBlock(builder, indentation);
			break;
		case "ldelem.i1":
			EmitLinearArrayLoad(builder, "static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT8>(chaos_element))", indentation);
			break;
		case "ldelem.u1":
			EmitLinearArrayLoad(builder, "static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINT8>(chaos_element))", indentation);
			break;
		case "ldelem.i2":
			EmitLinearArrayLoad(builder, "static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT16>(chaos_element))", indentation);
			break;
		case "ldelem.u2":
			EmitLinearArrayLoad(builder, "static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINT16>(chaos_element))", indentation);
			break;
		case "ldelem.i4":
			EmitLinearArrayLoad(builder, "static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(chaos_element))", indentation);
			break;
		case "ldelem.u4":
			EmitLinearArrayLoad(builder, "static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINT32>(chaos_element))", indentation);
			break;
		case "ldelem.i8":
			EmitLinearArrayLoad(builder, "static_cast<CHAOS_IL2CPP_INT64>(chaos_element)", indentation);
			break;
		case "ldelem.r4":
			EmitLinearArrayLoad(builder, "ChaosStoreFloat32(chaos_load_float32(chaos_element))", indentation);
			break;
		case "ldelem.r8":
			EmitLinearArrayLoad(builder, "ChaosStoreFloat64(ChaosLoadFloat64(chaos_element))", indentation);
			break;
		case "ldelem.ref":
			EmitLinearArrayLoad(builder, "chaos_element", indentation);
			break;
		case "ldelem":
		{
			var targetRef = instruction.TargetReference;
			if (targetRef == null || targetRef.Kind != AotCoreIrReferenceKind.Type)
			{
				// No type metadata: fall back to raw pointer load.
				EmitLinearArrayLoad(builder, "chaos_element", indentation);
				break;
			}

			string subjectId = HasArrayElementReference(targetRef)
				? targetRef.ArrayElementSubjectId!
				: targetRef.SubjectId;
			AotCoreIrTypeShapeKind typeShape = HasArrayElementReference(targetRef)
				? targetRef.ArrayElementTypeShape
				: targetRef.TypeShape;

			if (typeShape == AotCoreIrTypeShapeKind.ReferenceType || typeShape == AotCoreIrTypeShapeKind.InterfaceType)
				EmitLinearArrayLoad(builder, "chaos_element", indentation);
			else
				switch (subjectId)
				{
				case "System.Byte": case "System.SByte":
					EmitLinearArrayLoad(builder, "static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT8>(chaos_element))", indentation); break;
				case "System.Boolean":
					EmitLinearArrayLoad(builder, "static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINT8>(chaos_element))", indentation); break;
				case "System.Int16":
					EmitLinearArrayLoad(builder, "static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT16>(chaos_element))", indentation); break;
				case "System.UInt16": case "System.Char":
					EmitLinearArrayLoad(builder, "static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINT16>(chaos_element))", indentation); break;
				case "System.Int32":
					EmitLinearArrayLoad(builder, "static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(chaos_element))", indentation); break;
				case "System.UInt32":
					EmitLinearArrayLoad(builder, "static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINT32>(chaos_element))", indentation); break;
				case "System.Int64": case "System.UInt64":
					EmitLinearArrayLoad(builder, "static_cast<CHAOS_IL2CPP_INT64>(chaos_element)", indentation); break;
				case "System.Single":
					EmitLinearArrayLoad(builder, "ChaosStoreFloat32(chaos_load_float32(chaos_element))", indentation); break;
				case "System.Double":
					EmitLinearArrayLoad(builder, "ChaosStoreFloat64(ChaosLoadFloat64(chaos_element))", indentation); break;
				default:
					EmitLinearArrayLoad(builder, "chaos_element", indentation); break;
				}
			break;
		}
		case "stelem.i1":
			EmitLinearArrayStore(builder, "static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT8>(chaos_value_raw))", indentation, isReferenceElement: false);
			break;
		case "stelem.i2":
			EmitLinearArrayStore(builder, "static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT16>(chaos_value_raw))", indentation, isReferenceElement: false);
			break;
		case "stelem.i4":
			EmitLinearArrayStore(builder, "static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(chaos_value_raw))", indentation, isReferenceElement: false);
			break;
		case "stelem.i8":
			EmitLinearArrayStore(builder, "static_cast<CHAOS_IL2CPP_INT64>(chaos_value_raw)", indentation, isReferenceElement: false);
			break;
		case "stelem.r4":
			EmitLinearArrayStore(builder, "ChaosStoreFloat32(static_cast<CHAOS_IL2CPP_FLOAT32>(chaos_value_raw))", indentation, isReferenceElement: false);
			break;
		case "stelem.r8":
			EmitLinearArrayStore(builder, "ChaosStoreFloat64(static_cast<CHAOS_IL2CPP_FLOAT64>(chaos_value_raw))", indentation, isReferenceElement: false);
			break;
		case "stelem.ref":
			EmitLinearArrayStore(builder, "chaos_value", indentation, isReferenceElement: true);
			break;
		case "stelem":
		{
			var targetRef = instruction.TargetReference;
			if (targetRef == null || targetRef.Kind != AotCoreIrReferenceKind.Type)
			{
				// No type metadata: fall back to raw pointer store.
				// The element type is unknown at codegen time; the eval stack
				// already carries the correctly-typed value as CHAOS_IL2CPP_INTPTR.
				EmitLinearArrayStore(builder, "chaos_value_raw", indentation, isReferenceElement: false);
				break;
			}

			string subjectId = HasArrayElementReference(targetRef)
				? targetRef.ArrayElementSubjectId!
				: targetRef.SubjectId;
			AotCoreIrTypeShapeKind typeShape = HasArrayElementReference(targetRef)
				? targetRef.ArrayElementTypeShape
				: targetRef.TypeShape;

			if (typeShape == AotCoreIrTypeShapeKind.ReferenceType || typeShape == AotCoreIrTypeShapeKind.InterfaceType)
				EmitLinearArrayStore(builder, "chaos_value", indentation, isReferenceElement: true);
			else
				switch (subjectId)
				{
				case "System.Byte": case "System.SByte": case "System.Boolean":
					EmitLinearArrayStore(builder, "static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT8>(chaos_value_raw))", indentation, isReferenceElement: false); break;
				case "System.Int16": case "System.UInt16": case "System.Char":
					EmitLinearArrayStore(builder, "static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT16>(chaos_value_raw))", indentation, isReferenceElement: false); break;
				case "System.Int32": case "System.UInt32":
					EmitLinearArrayStore(builder, "static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(chaos_value_raw))", indentation, isReferenceElement: false); break;
				case "System.Int64": case "System.UInt64":
					EmitLinearArrayStore(builder, "static_cast<CHAOS_IL2CPP_INT64>(chaos_value_raw)", indentation, isReferenceElement: false); break;
				case "System.Single":
					EmitLinearArrayStore(builder, "ChaosStoreFloat32(static_cast<CHAOS_IL2CPP_FLOAT32>(chaos_value_raw))", indentation, isReferenceElement: false); break;
				case "System.Double":
					EmitLinearArrayStore(builder, "ChaosStoreFloat64(static_cast<CHAOS_IL2CPP_FLOAT64>(chaos_value_raw))", indentation, isReferenceElement: false); break;
				default:
					EmitLinearArrayStore(builder, "chaos_value_raw", indentation, isReferenceElement: false); break;
				}
			break;
		}
		case "ldobj":
			EmitLinearLoadObjectValue(builder, instruction, indentation);
			break;
		case "stobj":
			EmitLinearStoreObjectValue(builder, instruction, indentation);
			break;
		case "localloc":
			EmitLinearLocalAlloc(builder, indentation);
			break;
		case "brfalse":
		case "brtrue":
		{
			builder.AppendLine($"{indentation}// {instruction.Op} (structured EH branch)");
			break;
		}
		case "initblk":
		{
			builder.AppendLine($"{indentation}{{");
			builder.AppendLine($"{indentation}    const auto chaos_count = static_cast<CHAOS_IL2CPP_SIZE>({ConsumeEvalStackValueExpression()});");
			builder.AppendLine($"{indentation}    const auto chaos_value = static_cast<CHAOS_IL2CPP_UINT8>({ConsumeEvalStackValueExpression()});");
			builder.AppendLine($"{indentation}    auto* chaos_addr = reinterpret_cast<void*>({ConsumeEvalStackValueExpression()});");
			builder.AppendLine($"{indentation}    memset(chaos_addr, chaos_value, chaos_count);");
			builder.AppendLine($"{indentation}}}");
			break;
		}
		case "switch":
		{
			builder.AppendLine($"{indentation}// switch (handled via terminator in structured IR)");
			break;
		}
		case "arglist":
		{
			EmitEvalStackPush(builder, indentation, "reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_args)");
			break;
		}
		case "mkrefany":
		{
			string chaosTypeHandle = GetRequiredTypeHandleLiteral(instruction);
			builder.AppendLine($"{indentation}{{");
			builder.AppendLine($"{indentation}    const auto chaos_ptr = {ConsumeEvalStackValueExpression()};");
			EmitEvalStackPush(builder, indentation + "    ", chaosTypeHandle);
			EmitEvalStackPush(builder, indentation + "    ", "chaos_ptr");
			builder.AppendLine($"{indentation}}}");
			break;
		}
		case "refanyval":
		{
			builder.AppendLine($"{indentation}{{");
			builder.AppendLine($"{indentation}    const auto chaos_ptr = {ConsumeEvalStackValueExpression()};");
			builder.AppendLine($"{indentation}    static_cast<void>({ConsumeEvalStackValueExpression()});");
			EmitEvalStackPush(builder, indentation + "    ", "chaos_ptr");
			builder.AppendLine($"{indentation}}}");
			break;
		}
		case "refanytype":
		{
			builder.AppendLine($"{indentation}{{");
			builder.AppendLine($"{indentation}    static_cast<void>({ConsumeEvalStackValueExpression()});");
			builder.AppendLine($"{indentation}    const auto chaos_typeHandle = {ConsumeEvalStackValueExpression()};");
			EmitEvalStackPush(builder, indentation + "    ", "chaos_typeHandle");
			builder.AppendLine($"{indentation}}}");
			break;
		}
		case "ldvirtftn":
		{
			var targetRef = GetRequiredTargetReference(instruction);
			if (targetRef.Kind != AotCoreIrReferenceKind.Method)
			{
				throw new NotSupportedException($"native-aot structured EH linear ldvirtftn requires method target reference, got '{targetRef.Kind}'.");
			}
			string vtableSlotSig = GetMethodSignatureSuffix(targetRef.SubjectId);
			if (_vtableSlotMap == null || !_vtableSlotMap.TryGetValue(vtableSlotSig, out int vtableSlot))
			{
				throw new NotSupportedException($"native-aot structured EH linear ldvirtftn: vtable slot not found for '{targetRef.SubjectId}'.");
			}
			builder.AppendLine($"{indentation}{{");
			builder.AppendLine($"{indentation}    auto* chaos_object = reinterpret_cast<chaos_managed_object*>({ConsumeEvalStackValueExpression()});");
			builder.AppendLine($"{indentation}    if (chaos_object == nullptr) {{ CHAOS_IL2CPP_FAIL(); }}");
			builder.AppendLine($"{indentation}    auto* chaos_type = chaos_object_get_type_info(chaos_object);");
			builder.AppendLine($"{indentation}    auto chaos_fn = chaos_vtable_resolve(chaos_type->vtable_array, {vtableSlot}u);");
			EmitEvalStackPush(builder, indentation + "    ", "reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn)");
			builder.AppendLine($"{indentation}}}");
			break;
		}
		case "calli":
		{
			var calliParamAbis = CreateCallSiteParameterAbis(instruction);
			var calliReturnAbi = CreateCallSiteReturnAbi(instruction);
			string calliReturnType = MapAbiSlotReturnType(calliReturnAbi);
			string calliParamTypes = FormatAbiSlotParameterTypes(calliParamAbis);
			string calliFnType = $"{calliReturnType}(*)({calliParamTypes})";
			builder.AppendLine($"{indentation}{{");
			builder.AppendLine($"{indentation}    const auto chaos_fnptr = {ConsumeEvalStackValueExpression()};");
			for (int calliIdx = calliParamAbis.Count - 1; calliIdx >= 0; calliIdx--)
			{
				builder.AppendLine($"{indentation}    const auto chaos_raw_arg_{calliIdx} = {ConsumeEvalStackValueExpression()};");
				builder.AppendLine($"{indentation}    const auto chaos_arg_{calliIdx} = {FormatInboundAbiArgumentExpression(calliParamAbis[calliIdx], $"chaos_raw_arg_{calliIdx}")};");
			}
			string calliArgs = FormatAbiInvocationArgumentList(calliParamAbis);
			if (string.Equals(calliReturnType, "void", StringComparison.Ordinal))
			{
				builder.AppendLine($"{indentation}    reinterpret_cast<{calliFnType}>(chaos_fnptr)({calliArgs});");
			}
			else
			{
				builder.AppendLine($"{indentation}    const auto chaos_result = reinterpret_cast<{calliFnType}>(chaos_fnptr)({calliArgs});");
				EmitAbiReturnPush(builder, calliReturnAbi, "chaos_result", $"{indentation}    ");
			}
			builder.AppendLine($"{indentation}}}");
			break;
		}
		case "jmp":
		{
			var jmpTarget = ResolveDirectInvocationTarget(instruction);
			var jmpParamAbis = jmpTarget.ParameterAbis;
			if (jmpParamAbis.Count == 0)
			{
				builder.AppendLine($"{indentation}return {jmpTarget.TargetSymbol}();");
			}
			else
			{
				builder.AppendLine($"{indentation}{{");
				for (int jmpIdx = 0; jmpIdx < jmpParamAbis.Count; jmpIdx++)
				{
					builder.AppendLine($"{indentation}    const auto chaos_jmp_arg_{jmpIdx} = {FormatInboundAbiArgumentExpression(jmpParamAbis[jmpIdx], $"chaos_args[{jmpIdx}]")};");
				}
				string jmpArgs = FormatAbiInvocationArgumentList(jmpParamAbis);
				builder.AppendLine($"{indentation}    return {jmpTarget.TargetSymbol}({jmpArgs});");
				builder.AppendLine($"{indentation}}}");
			}
			break;
		}
		case "ret":
		{
			builder.AppendLine($"{indentation}// ret (handled via terminator in structured IR)");
			break;
		}
		default:
			throw new NotSupportedException("native-aot structured EH linear lowering does not support opcode '" + instruction.Op + "'.");
		}
	}

	private void EmitLinearNewArray(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string indentation)
	{
		AotCoreIrReferenceArtifact requiredTargetReference = GetRequiredTargetReference(instruction);
		if (requiredTargetReference.Kind != AotCoreIrReferenceKind.Type)
		{
			throw new NotSupportedException($"native-aot structured EH linear newarr requires type target reference, got '{requiredTargetReference.Kind}'.");
		}

		string subjectId = HasArrayElementReference(requiredTargetReference)
			? requiredTargetReference.ArrayElementSubjectId!
			: requiredTargetReference.SubjectId;
		AotCoreIrTypeShapeKind typeShape = HasArrayElementReference(requiredTargetReference)
			? requiredTargetReference.ArrayElementTypeShape
			: requiredTargetReference.TypeShape;

		builder.AppendLine($"{indentation}{{");
		builder.AppendLine($"{indentation}    const auto chaos_length = static_cast<CHAOS_IL2CPP_INT32>({ConsumeEvalStackValueExpression()});");
		builder.AppendLine($"{indentation}    if (chaos_length < 0)");
		builder.AppendLine($"{indentation}    {{");
		builder.AppendLine($"{indentation}        CHAOS_IL2CPP_FAIL();");
		builder.AppendLine($"{indentation}    }}");
		builder.AppendLine($"{indentation}    auto* chaos_array = CHAOS_IL2CPP_NEW_GC(chaos_managed_array, {{}});");
		builder.AppendLine($"{indentation}    chaos_array->header.type_info = &chaos_type_info_managed_array;");
		builder.AppendLine($"{indentation}    chaos_array->element_type_shape = {GetNativeTypeShapeValue(typeShape)};");
		builder.AppendLine($"{indentation}    chaos_array->element_type_info = {GetRuntimeTypeInfoExpression(subjectId)};");
		builder.AppendLine($"{indentation}    chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);");
		builder.AppendLine($"{indentation}    chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));");
		EmitEvalStackPush(builder, indentation + "    ", "reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array)");
		builder.AppendLine($"{indentation}}}");
	}

	private void EmitLinearArrayElementAddress(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string indentation)
	{
		AotCoreIrReferenceArtifact requiredTargetReference = GetRequiredTargetReference(instruction);
		if (requiredTargetReference.Kind != AotCoreIrReferenceKind.Type)
		{
			throw new NotSupportedException($"native-aot structured EH linear ldelema requires type target reference, got '{requiredTargetReference.Kind}'.");
		}

		builder.AppendLine($"{indentation}{{");
		builder.AppendLine($"{indentation}    const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>({ConsumeEvalStackValueExpression()});");
		builder.AppendLine($"{indentation}    auto* chaos_array = reinterpret_cast<chaos_managed_array*>({ConsumeEvalStackValueExpression()});");
		builder.AppendLine($"{indentation}    if (chaos_array == nullptr)");
		builder.AppendLine($"{indentation}    {{");
		builder.AppendLine($"{indentation}        CHAOS_IL2CPP_FAIL();");
		builder.AppendLine($"{indentation}    }}");
		builder.AppendLine($"{indentation}    if (chaos_index < 0 || static_cast<CHAOS_IL2CPP_INTPTR>(chaos_index) >= chaos_array->length)");
		builder.AppendLine($"{indentation}    {{");
		builder.AppendLine($"{indentation}        CHAOS_IL2CPP_FAIL();");
		builder.AppendLine($"{indentation}    }}");
		EmitEvalStackPush(builder, indentation + "    ", "reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)])");
		builder.AppendLine($"{indentation}}}");
	}

	private void EmitLinearLoadIndirect(StringBuilder builder, string nativeType, string pushedValueExpression, string indentation)
	{
		builder.AppendLine($"{indentation}{{");
		builder.AppendLine($"{indentation}    const auto chaos_address = {ConsumeEvalStackValueExpression()};");
		builder.AppendLine($"{indentation}    const auto chaos_value = chaos_load_indirect<{nativeType}>(chaos_address);");
		EmitEvalStackPush(builder, indentation + "    ", pushedValueExpression);
		builder.AppendLine($"{indentation}}}");
	}

	private void EmitLinearStoreIndirect(StringBuilder builder, string nativeType, string valueExpression, string indentation, bool materializeString)
	{
		builder.AppendLine($"{indentation}{{");
		builder.AppendLine($"{indentation}    auto chaos_value_raw = {ConsumeEvalStackValueExpression()};");
		if (materializeString && _stringIdMapping is { Count: > 0 })
		{
			builder.AppendLine($"{indentation}    if (chaos_is_string_id(chaos_value_raw))");
			builder.AppendLine($"{indentation}    {{");
			builder.AppendLine($"{indentation}        chaos_value_raw = chaos_string_materialize(chaos_value_raw);");
			builder.AppendLine($"{indentation}    }}");
		}
		builder.AppendLine($"{indentation}    const auto chaos_value = {valueExpression};");
		builder.AppendLine($"{indentation}    const auto chaos_address = {ConsumeEvalStackValueExpression()};");
		builder.AppendLine($"{indentation}    chaos_store_indirect<{nativeType}>(chaos_address, chaos_value);");
		builder.AppendLine($"{indentation}}}");
	}

	private void EmitLinearFieldAddress(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string indentation)
	{
		AotCoreIrReferenceArtifact requiredTargetReference = GetRequiredTargetReference(instruction);
		if (requiredTargetReference.Kind != AotCoreIrReferenceKind.Field)
		{
			throw new NotSupportedException($"native-aot structured EH linear field address requires field target reference, got '{requiredTargetReference.Kind}'.");
		}

		if (instruction.RuntimeServiceKind == AotCoreIrRuntimeServiceKind.LoadStaticField)
		{
			EmitStaticInitializationForField(builder, requiredTargetReference.SubjectId, indentation);
			EmitEvalStackPush(builder, indentation, $"reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&{GetNativeStaticFieldSymbol(requiredTargetReference.SubjectId)})");
			return;
		}

		builder.AppendLine($"{indentation}{{");
		string declaringTypeSubjectId = GetRequiredDeclaringTypeSubjectId(requiredTargetReference);
		if (requiredTargetReference.DeclaringTypeShape == AotCoreIrTypeShapeKind.ValueType)
		{
			builder.AppendLine($"{indentation}    auto* chaos_value = chaos_resolve_managed_value_pointer<{GetNativeValueTypeSymbol(declaringTypeSubjectId)}>({ConsumeEvalStackValueExpression()});");
			EmitEvalStackPush(builder, indentation + "    ", $"reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_value->{GetNativeFieldMemberName(requiredTargetReference.SubjectId)})");
		}
		else
		{
			builder.AppendLine($"{indentation}    auto* chaos_object = reinterpret_cast<{GetNativeTypeSymbol(declaringTypeSubjectId)}*>({ConsumeEvalStackValueExpression()});");
			EmitEvalStackPush(builder, indentation + "    ", $"reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_object->{GetNativeFieldMemberName(requiredTargetReference.SubjectId)})");
		}
		builder.AppendLine($"{indentation}}}");
	}

	private void EmitLinearArrayLoad(StringBuilder builder, string pushedValueExpression, string indentation)
	{
		builder.AppendLine($"{indentation}{{");
		builder.AppendLine($"{indentation}    const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>({ConsumeEvalStackValueExpression()});");
		builder.AppendLine($"{indentation}    auto* chaos_array = reinterpret_cast<chaos_managed_array*>({ConsumeEvalStackValueExpression()});");
		builder.AppendLine($"{indentation}    if (chaos_array == nullptr)");
		builder.AppendLine($"{indentation}    {{");
		builder.AppendLine($"{indentation}        CHAOS_IL2CPP_FAIL();");
		builder.AppendLine($"{indentation}    }}");
		builder.AppendLine($"{indentation}    if (chaos_index < 0 || static_cast<CHAOS_IL2CPP_INTPTR>(chaos_index) >= chaos_array->length)");
		builder.AppendLine($"{indentation}    {{");
		builder.AppendLine($"{indentation}        CHAOS_IL2CPP_FAIL();");
		builder.AppendLine($"{indentation}    }}");
		builder.AppendLine($"{indentation}    const auto chaos_element = chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)];");
		EmitEvalStackPush(builder, indentation + "    ", pushedValueExpression);
		builder.AppendLine($"{indentation}}}");
	}

	private void EmitLinearArrayStore(StringBuilder builder, string storedValueExpression, string indentation, bool isReferenceElement)
	{
		builder.AppendLine($"{indentation}{{");
		builder.AppendLine($"{indentation}    auto chaos_value_raw = {ConsumeEvalStackValueExpression()};");
		if (isReferenceElement && _stringIdMapping is { Count: > 0 })
		{
			builder.AppendLine($"{indentation}    if (chaos_is_string_id(chaos_value_raw))");
			builder.AppendLine($"{indentation}    {{");
			builder.AppendLine($"{indentation}        chaos_value_raw = chaos_string_materialize(chaos_value_raw);");
			builder.AppendLine($"{indentation}    }}");
		}
		if (isReferenceElement)
		{
			builder.AppendLine($"{indentation}    auto chaos_value = {storedValueExpression};");
		}
		else
		{
			builder.AppendLine($"{indentation}    const auto chaos_value = {storedValueExpression};");
		}
		builder.AppendLine($"{indentation}    const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>({ConsumeEvalStackValueExpression()});");
		builder.AppendLine($"{indentation}    auto* chaos_array = reinterpret_cast<chaos_managed_array*>({ConsumeEvalStackValueExpression()});");
		builder.AppendLine($"{indentation}    if (chaos_array == nullptr)");
		builder.AppendLine($"{indentation}    {{");
		builder.AppendLine($"{indentation}        CHAOS_IL2CPP_FAIL();");
		builder.AppendLine($"{indentation}    }}");
		builder.AppendLine($"{indentation}    if (chaos_index < 0 || static_cast<CHAOS_IL2CPP_INTPTR>(chaos_index) >= chaos_array->length)");
		builder.AppendLine($"{indentation}    {{");
		builder.AppendLine($"{indentation}        CHAOS_IL2CPP_FAIL();");
		builder.AppendLine($"{indentation}    }}");
		if (isReferenceElement)
		{
			builder.AppendLine($"{indentation}    if (!chaos_is_array_store_compatible(chaos_array, chaos_value))");
			builder.AppendLine($"{indentation}    {{");
			builder.AppendLine($"{indentation}        CHAOS_IL2CPP_FAIL();");
			builder.AppendLine($"{indentation}    }}");
		}
		builder.AppendLine($"{indentation}    chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)] = chaos_value;");
			if (isReferenceElement)
			{
				builder.AppendLine($"{indentation}    GC_END_STUBBORN_CHANGE(chaos_array);");
				builder.AppendLine($"{indentation}    chaos::il2cpp::runtime_core::DirtyCard(chaos_array);");
			}
		builder.AppendLine($"{indentation}}}");
	}

	private void EmitLinearBox(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string indentation)
	{
		AotCoreIrReferenceArtifact requiredTargetReference = GetRequiredTargetReference(instruction);
		if (requiredTargetReference.Kind != AotCoreIrReferenceKind.Type)
		{
			throw new NotSupportedException($"native-aot structured EH linear box requires type target reference, got '{requiredTargetReference.Kind}'.");
		}

		// Stack-allocate boxes for simple value types (scalar primitives like Int32, Int64, etc.).
		// For structured value types (requiring chaos_resolve_managed_value_pointer), fall back to heap.
		// Stack allocation eliminates ~20-30ns per call from C++ heap operator new.
		// NOTE: The storage is declared at the enclosing scope level (no inner {})
		// to ensure the pointer remains valid when consumed later by a call instruction.
		if (!RequiresStructuredValueTypePayload(requiredTargetReference))
		{
			string storageName = AllocateLinearScratchName("box_storage");
			string boxTypeName = GetNativeBoxTypeSymbol(requiredTargetReference.SubjectId);
			builder.AppendLine($"{indentation}{boxTypeName} {storageName}{{}};");
			builder.AppendLine($"{indentation}{{");
			builder.AppendLine($"{indentation}    const auto chaos_value = {ConsumeEvalStackValueExpression()};");
			builder.AppendLine($"{indentation}    {storageName}.header.type_info = &{GetNativeBoxTypeInfoSymbol(requiredTargetReference.SubjectId)};");
			builder.AppendLine($"{indentation}    {storageName}.value = chaos_value;");
			builder.AppendLine($"{indentation}}}");
			EmitEvalStackPush(builder, indentation, $"reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&{storageName})");
			return;
		}

		builder.AppendLine($"{indentation}{{");
		builder.AppendLine($"{indentation}    const auto chaos_value = {ConsumeEvalStackValueExpression()};");
		builder.AppendLine($"{indentation}    auto* chaos_boxed = CHAOS_IL2CPP_NEW_GC({GetNativeBoxTypeSymbol(requiredTargetReference.SubjectId)}, {{}});");
		builder.AppendLine($"{indentation}    chaos_boxed->header.type_info = &{GetNativeBoxTypeInfoSymbol(requiredTargetReference.SubjectId)};");
		if (RequiresStructuredValueTypePayload(requiredTargetReference))
		{
			builder.AppendLine($"{indentation}    auto* chaos_payload = chaos_resolve_managed_value_pointer<{GetNativeValueTypeSymbol(requiredTargetReference.SubjectId)}>(chaos_value);");
			builder.AppendLine($"{indentation}    chaos_boxed->value = *chaos_payload;");
		}
		else
		{
			builder.AppendLine($"{indentation}    chaos_boxed->value = chaos_value;");
		}
		EmitEvalStackPush(builder, indentation + "    ", "reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_boxed)");
		builder.AppendLine($"{indentation}}}");
	}

	private void EmitLinearUnbox(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string indentation)
	{
		AotCoreIrReferenceArtifact requiredTargetReference = GetRequiredTargetReference(instruction);
		if (requiredTargetReference.Kind != AotCoreIrReferenceKind.Type)
		{
			throw new NotSupportedException($"native-aot structured EH linear unbox requires type target reference, got '{requiredTargetReference.Kind}'.");
		}

		builder.AppendLine($"{indentation}{{");
		builder.AppendLine($"{indentation}    auto* chaos_boxed = reinterpret_cast<{GetNativeBoxTypeSymbol(requiredTargetReference.SubjectId)}*>({ConsumeEvalStackValueExpression()});");
		builder.AppendLine($"{indentation}    if (chaos_boxed == nullptr)");
		builder.AppendLine($"{indentation}    {{");
		builder.AppendLine($"{indentation}        CHAOS_IL2CPP_FAIL();");
		builder.AppendLine($"{indentation}    }}");
		EmitEvalStackPush(builder, indentation + "    ", "reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_boxed->value)");
		builder.AppendLine($"{indentation}}}");
	}

	private void EmitLinearUnboxAny(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string indentation)
	{
		AotCoreIrReferenceArtifact requiredTargetReference = GetRequiredTargetReference(instruction);
		if (requiredTargetReference.Kind != AotCoreIrReferenceKind.Type)
		{
			throw new NotSupportedException($"native-aot structured EH linear unbox.any requires type target reference, got '{requiredTargetReference.Kind}'.");
		}

		string? scratchName = null;
		if (RequiresStructuredValueTypePayload(requiredTargetReference))
		{
			scratchName = AllocateLinearScratchName("unbox_any_value");
			builder.AppendLine($"{indentation}{GetNativeValueTypeSymbol(requiredTargetReference.SubjectId)} {scratchName}{{}};");
		}

		builder.AppendLine($"{indentation}{{");
		builder.AppendLine($"{indentation}    auto* chaos_boxed = reinterpret_cast<{GetNativeBoxTypeSymbol(requiredTargetReference.SubjectId)}*>({ConsumeEvalStackValueExpression()});");
		builder.AppendLine($"{indentation}    if (chaos_boxed == nullptr)");
		builder.AppendLine($"{indentation}    {{");
		builder.AppendLine($"{indentation}        CHAOS_IL2CPP_FAIL();");
		builder.AppendLine($"{indentation}    }}");
		if (RequiresStructuredValueTypePayload(requiredTargetReference))
		{
			builder.AppendLine($"{indentation}    {scratchName} = chaos_boxed->value;");
			EmitEvalStackPush(builder, indentation + "    ", $"reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&{scratchName})");
		}
		else
		{
			EmitEvalStackPush(builder, indentation + "    ", "chaos_boxed->value");
		}
		builder.AppendLine($"{indentation}}}");
	}

	private void EmitLinearCastClass(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string indentation)
	{
		AotCoreIrReferenceArtifact requiredTargetReference = GetRequiredTargetReference(instruction);
		if (requiredTargetReference.Kind != AotCoreIrReferenceKind.Type)
		{
			throw new NotSupportedException($"native-aot structured EH linear castclass requires type target reference, got '{requiredTargetReference.Kind}'.");
		}

		string chaosValue = AccessEvalStackTopExpression();
		builder.AppendLine($"{indentation}{{");
		builder.AppendLine($"{indentation}    const auto chaos_value = {chaosValue};");
		builder.AppendLine($"{indentation}    if (chaos_value != static_cast<CHAOS_IL2CPP_INTPTR>(0))");
		builder.AppendLine($"{indentation}    {{");
		builder.AppendLine($"{indentation}        auto* chaos_header = reinterpret_cast<FatHeader*>(chaos_value);");
		if (HasArrayElementReference(requiredTargetReference))
		{
			builder.AppendLine($"{indentation}        if (chaos_object_get_type_info(chaos_header) != &chaos_type_info_managed_array)");
			builder.AppendLine($"{indentation}        {{");
			builder.AppendLine($"{indentation}            CHAOS_IL2CPP_FAIL();");
			builder.AppendLine($"{indentation}        }}");
			builder.AppendLine($"{indentation}        auto* chaos_array = reinterpret_cast<chaos_managed_array*>(chaos_value);");
			builder.AppendLine($"{indentation}        if (!chaos_is_array_type_compatible(chaos_array->element_type_shape, chaos_array->element_type_info, {GetNativeTypeShapeValue(requiredTargetReference.ArrayElementTypeShape)}, {GetRuntimeTypeInfoExpression(requiredTargetReference.ArrayElementSubjectId)}))");
		}
		else if (requiredTargetReference.TypeShape == AotCoreIrTypeShapeKind.InterfaceType)
		{
			builder.AppendLine($"{indentation}        if (!chaos_does_type_implement_interface(chaos_object_get_type_info(chaos_header), &{GetNativeTypeInfoSymbol(requiredTargetReference.SubjectId)}))");
		}
		else if (requiredTargetReference.TypeShape == AotCoreIrTypeShapeKind.ReferenceType)
		{
			builder.AppendLine($"{indentation}        if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &{GetNativeTypeInfoSymbol(requiredTargetReference.SubjectId)}))");
		}
		else
		{
			builder.AppendLine($"{indentation}        if (chaos_object_get_type_info(chaos_header) != &{GetNativeTypeInfoSymbol(requiredTargetReference.SubjectId)} && chaos_object_get_type_info(chaos_header)->stable_id != (&{GetNativeTypeInfoSymbol(requiredTargetReference.SubjectId)})->stable_id)");
		}
		builder.AppendLine($"{indentation}        {{");
		builder.AppendLine($"{indentation}            CHAOS_IL2CPP_FAIL();");
		builder.AppendLine($"{indentation}        }}");
		builder.AppendLine($"{indentation}    }}");
		builder.AppendLine($"{indentation}}}");
	}

	private void EmitLinearIsInst(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string indentation)
	{
		AotCoreIrReferenceArtifact requiredTargetReference = GetRequiredTargetReference(instruction);
		if (requiredTargetReference.Kind != AotCoreIrReferenceKind.Type)
		{
			throw new NotSupportedException($"native-aot structured EH linear isinst requires type target reference, got '{requiredTargetReference.Kind}'.");
		}

		builder.AppendLine($"{indentation}{{");
		builder.AppendLine($"{indentation}    const auto chaos_value = {ConsumeEvalStackValueExpression()};");
		builder.AppendLine($"{indentation}    auto chaos_matches = false;");
		builder.AppendLine($"{indentation}    if (chaos_value != static_cast<CHAOS_IL2CPP_INTPTR>(0))");
		builder.AppendLine($"{indentation}    {{");
		builder.AppendLine($"{indentation}        auto* chaos_header = reinterpret_cast<FatHeader*>(chaos_value);");
		if (HasArrayElementReference(requiredTargetReference))
		{
			builder.AppendLine($"{indentation}        if (chaos_object_get_type_info(chaos_header) == &chaos_type_info_managed_array)");
			builder.AppendLine($"{indentation}        {{");
			builder.AppendLine($"{indentation}            auto* chaos_array = reinterpret_cast<chaos_managed_array*>(chaos_value);");
			builder.AppendLine($"{indentation}            chaos_matches = chaos_is_array_type_compatible(chaos_array->element_type_shape, chaos_array->element_type_info, {GetNativeTypeShapeValue(requiredTargetReference.ArrayElementTypeShape)}, {GetRuntimeTypeInfoExpression(requiredTargetReference.ArrayElementSubjectId)});");
			builder.AppendLine($"{indentation}        }}");
		}
		else if (requiredTargetReference.TypeShape == AotCoreIrTypeShapeKind.InterfaceType)
		{
			builder.AppendLine($"{indentation}        chaos_matches = chaos_does_type_implement_interface(chaos_object_get_type_info(chaos_header), &{GetNativeTypeInfoSymbol(requiredTargetReference.SubjectId)});");
		}
		else if (requiredTargetReference.TypeShape == AotCoreIrTypeShapeKind.ReferenceType)
		{
			builder.AppendLine($"{indentation}        chaos_matches = chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &{GetNativeTypeInfoSymbol(requiredTargetReference.SubjectId)});");
		}
		else
		{
			builder.AppendLine($"{indentation}        chaos_matches = chaos_object_get_type_info(chaos_header) == &{GetNativeTypeInfoSymbol(requiredTargetReference.SubjectId)} || chaos_object_get_type_info(chaos_header)->stable_id == (&{GetNativeTypeInfoSymbol(requiredTargetReference.SubjectId)})->stable_id;");
		}
		builder.AppendLine($"{indentation}    }}");
		EmitEvalStackPush(builder, indentation + "    ", "chaos_matches ? chaos_value : static_cast<CHAOS_IL2CPP_INTPTR>(0)");
		builder.AppendLine($"{indentation}}}");
	}

	private void EmitLinearCopyObject(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string indentation)
	{
		AotCoreIrReferenceArtifact requiredTargetReference = GetRequiredTargetReference(instruction);
		if (requiredTargetReference.Kind != AotCoreIrReferenceKind.Type)
		{
			throw new NotSupportedException($"native-aot structured EH linear cpobj requires type target reference, got '{requiredTargetReference.Kind}'.");
		}

		builder.AppendLine($"{indentation}{{");
		if (RequiresStructuredValueTypePayload(requiredTargetReference))
		{
			builder.AppendLine($"{indentation}    auto* chaos_source = reinterpret_cast<{GetNativeValueTypeSymbol(requiredTargetReference.SubjectId)}*>({ConsumeEvalStackValueExpression()});");
			builder.AppendLine($"{indentation}    auto* chaos_destination = chaos_resolve_managed_value_pointer<{GetNativeValueTypeSymbol(requiredTargetReference.SubjectId)}>({ConsumeEvalStackValueExpression()});");
			builder.AppendLine($"{indentation}    if (chaos_source == nullptr)");
			builder.AppendLine($"{indentation}    {{");
			builder.AppendLine($"{indentation}        CHAOS_IL2CPP_FAIL();");
			builder.AppendLine($"{indentation}    }}");
			builder.AppendLine($"{indentation}    *chaos_destination = *chaos_source;");
		}
		else
		{
			builder.AppendLine($"{indentation}    const auto chaos_source = {ConsumeEvalStackValueExpression()};");
			builder.AppendLine($"{indentation}    auto* chaos_destination = chaos_resolve_managed_value_pointer<CHAOS_IL2CPP_INTPTR>({ConsumeEvalStackValueExpression()});");
			builder.AppendLine($"{indentation}    *chaos_destination = chaos_source;");
		}
		builder.AppendLine($"{indentation}}}");
	}

	private void EmitLinearCopyBlock(StringBuilder builder, string indentation)
	{
		builder.AppendLine($"{indentation}{{");
		builder.AppendLine($"{indentation}    const auto chaos_size = static_cast<CHAOS_IL2CPP_SIZE>(static_cast<CHAOS_IL2CPP_UINTPTR>({ConsumeEvalStackValueExpression()}));");
		builder.AppendLine($"{indentation}    const auto chaos_source = {ConsumeEvalStackValueExpression()};");
		builder.AppendLine($"{indentation}    const auto chaos_destination = {ConsumeEvalStackValueExpression()};");
		builder.AppendLine($"{indentation}    if (chaos_size != static_cast<CHAOS_IL2CPP_SIZE>(0))");
		builder.AppendLine($"{indentation}    {{");
		builder.AppendLine($"{indentation}        const auto chaos_resolve_cpblk_address = [](CHAOS_IL2CPP_INTPTR chaos_address) -> void*");
		builder.AppendLine($"{indentation}        {{");
		builder.AppendLine($"{indentation}            if ((chaos_address & CHAOS_IL2CPP_RAW_POINTER_TAG) != 0)");
		builder.AppendLine($"{indentation}            {{");
		builder.AppendLine($"{indentation}                return reinterpret_cast<void*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_address & ~CHAOS_IL2CPP_RAW_POINTER_TAG));");
		builder.AppendLine($"{indentation}            }}");
		builder.AppendLine($"{indentation}            if ((chaos_address & chaos_managed_pointer_local_slot_tag) != 0)");
		builder.AppendLine($"{indentation}            {{");
		builder.AppendLine($"{indentation}                return reinterpret_cast<void*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_address & ~chaos_managed_pointer_local_slot_tag));");
		builder.AppendLine($"{indentation}            }}");
		builder.AppendLine($"{indentation}            return reinterpret_cast<void*>(chaos_address);");
		builder.AppendLine($"{indentation}        }};");
		builder.AppendLine($"{indentation}        if (chaos_source == static_cast<CHAOS_IL2CPP_INTPTR>(0) || chaos_destination == static_cast<CHAOS_IL2CPP_INTPTR>(0))");
		builder.AppendLine($"{indentation}        {{");
		builder.AppendLine($"{indentation}            CHAOS_IL2CPP_FAIL();");
		builder.AppendLine($"{indentation}        }}");
		builder.AppendLine($"{indentation}        CHAOS_IL2CPP_MEMCPY(chaos_resolve_cpblk_address(chaos_destination), chaos_resolve_cpblk_address(chaos_source), chaos_size);");
		builder.AppendLine($"{indentation}    }}");
		builder.AppendLine($"{indentation}}}");
	}

	private void EmitLinearLocalAlloc(StringBuilder builder, string indentation)
	{
		builder.AppendLine($"{indentation}{{");
		builder.AppendLine($"{indentation}    const auto chaos_size = static_cast<CHAOS_IL2CPP_INTPTR>({ConsumeEvalStackValueExpression()});");
		builder.AppendLine($"{indentation}    if (chaos_size < 0)");
		builder.AppendLine($"{indentation}    {{");
		builder.AppendLine($"{indentation}        CHAOS_IL2CPP_FAIL();");
		builder.AppendLine($"{indentation}    }}");
		builder.AppendLine($"{indentation}    const auto chaos_byte_count = static_cast<CHAOS_IL2CPP_SIZE>(chaos_size);");
		builder.AppendLine($"{indentation}    void* chaos_block = CHAOS_IL2CPP_MALLOC(chaos_byte_count == static_cast<CHAOS_IL2CPP_SIZE>(0) ? static_cast<CHAOS_IL2CPP_SIZE>(1) : chaos_byte_count);");
		builder.AppendLine($"{indentation}    if (chaos_block == nullptr)");
		builder.AppendLine($"{indentation}    {{");
		builder.AppendLine($"{indentation}        CHAOS_IL2CPP_FAIL();");
		builder.AppendLine($"{indentation}    }}");
		EmitEvalStackPush(builder, indentation + "    ", "reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_block)");
		builder.AppendLine($"{indentation}}}");
	}

	private void EmitLinearLoadObjectValue(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string indentation)
	{
		AotCoreIrReferenceArtifact requiredTargetReference = GetRequiredTargetReference(instruction);
		if (requiredTargetReference.Kind != AotCoreIrReferenceKind.Type)
		{
			throw new NotSupportedException($"native-aot structured EH linear ldobj requires type target reference, got '{requiredTargetReference.Kind}'.");
		}

		string? scratchName = null;
		if (RequiresStructuredValueTypePayload(requiredTargetReference))
		{
			scratchName = AllocateLinearScratchName("ldobj_value");
			builder.AppendLine($"{indentation}{GetNativeValueTypeSymbol(requiredTargetReference.SubjectId)} {scratchName}{{}};");
		}

		builder.AppendLine($"{indentation}{{");
		if (RequiresStructuredValueTypePayload(requiredTargetReference))
		{
			builder.AppendLine($"{indentation}    auto* chaos_source = chaos_resolve_managed_value_pointer<{GetNativeValueTypeSymbol(requiredTargetReference.SubjectId)}>({ConsumeEvalStackValueExpression()});");
			builder.AppendLine($"{indentation}    {scratchName} = *chaos_source;");
			EmitEvalStackPush(builder, indentation + "    ", $"reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&{scratchName})");
		}
		else
		{
			builder.AppendLine($"{indentation}    auto* chaos_source = chaos_resolve_managed_value_pointer<CHAOS_IL2CPP_INTPTR>({ConsumeEvalStackValueExpression()});");
			EmitEvalStackPush(builder, indentation + "    ", "*chaos_source");
		}
		builder.AppendLine($"{indentation}}}");
	}

	private void EmitLinearStoreObjectValue(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string indentation)
	{
		AotCoreIrReferenceArtifact requiredTargetReference = GetRequiredTargetReference(instruction);
		if (requiredTargetReference.Kind != AotCoreIrReferenceKind.Type)
		{
			throw new NotSupportedException($"native-aot structured EH linear stobj requires type target reference, got '{requiredTargetReference.Kind}'.");
		}

		builder.AppendLine($"{indentation}{{");
		if (RequiresStructuredValueTypePayload(requiredTargetReference))
		{
			builder.AppendLine($"{indentation}    auto* chaos_source = reinterpret_cast<{GetNativeValueTypeSymbol(requiredTargetReference.SubjectId)}*>({ConsumeEvalStackValueExpression()});");
			builder.AppendLine($"{indentation}    auto* chaos_destination = chaos_resolve_managed_value_pointer<{GetNativeValueTypeSymbol(requiredTargetReference.SubjectId)}>({ConsumeEvalStackValueExpression()});");
			builder.AppendLine($"{indentation}    if (chaos_source == nullptr)");
			builder.AppendLine($"{indentation}    {{");
			builder.AppendLine($"{indentation}        CHAOS_IL2CPP_FAIL();");
			builder.AppendLine($"{indentation}    }}");
			builder.AppendLine($"{indentation}    *chaos_destination = *chaos_source;");
		}
		else
		{
			builder.AppendLine($"{indentation}    const auto chaos_value = {ConsumeEvalStackValueExpression()};");
			builder.AppendLine($"{indentation}    auto* chaos_destination = chaos_resolve_managed_value_pointer<CHAOS_IL2CPP_INTPTR>({ConsumeEvalStackValueExpression()});");
			builder.AppendLine($"{indentation}    *chaos_destination = chaos_value;");
		}
		builder.AppendLine($"{indentation}}}");
	}

	private void EmitLinearInitObj(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string indentation)
	{
		AotCoreIrReferenceArtifact requiredTargetReference = GetRequiredTargetReference(instruction);
		if (requiredTargetReference.Kind != AotCoreIrReferenceKind.Type)
		{
			throw new NotSupportedException($"native-aot structured EH linear initobj requires type target reference, got '{requiredTargetReference.Kind}'.");
		}

		builder.AppendLine($"{indentation}{{");
		if (RequiresStructuredValueTypePayload(requiredTargetReference))
		{
			builder.AppendLine($"{indentation}    auto* chaos_value = chaos_resolve_managed_value_pointer<{GetNativeValueTypeSymbol(requiredTargetReference.SubjectId)}>({ConsumeEvalStackValueExpression()});");
			builder.AppendLine($"{indentation}    *chaos_value = {GetNativeValueTypeSymbol(requiredTargetReference.SubjectId)}{{}};");
		}
		else
		{
			builder.AppendLine($"{indentation}    const auto chaos_address = {ConsumeEvalStackValueExpression()};");
			builder.AppendLine($"{indentation}    if ((chaos_address & chaos_managed_pointer_local_slot_tag) != 0)");
			builder.AppendLine($"{indentation}    {{");
			builder.AppendLine($"{indentation}        auto* chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_address & ~chaos_managed_pointer_local_slot_tag));");
			builder.AppendLine($"{indentation}        *chaos_slot = static_cast<CHAOS_IL2CPP_INTPTR>(0);");
			builder.AppendLine($"{indentation}    }}");
			builder.AppendLine($"{indentation}    else");
			builder.AppendLine($"{indentation}    {{");
			builder.AppendLine($"{indentation}        *reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(chaos_address) = static_cast<CHAOS_IL2CPP_INTPTR>(0);");
			builder.AppendLine($"{indentation}    }}");
		}
		builder.AppendLine($"{indentation}}}");
	}

	private void EmitLinearCall(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string indentation)
	{
		InvocationTarget invocationTarget = ResolveDirectInvocationTarget(instruction);

		// Inline shape expansion: substitute args into expression template at call site.
		if (invocationTarget.InlineCppExpression is { } inlineExpr)
		{
			builder.AppendLine(indentation + "{");
			var argExprs = new string[invocationTarget.ParameterAbis.Count];
			for (int i = invocationTarget.ParameterAbis.Count - 1; i >= 0; i--)
				argExprs[i] = ConsumeEvalStackValueExpression();
			string expr = inlineExpr;
			for (int i = 0; i < argExprs.Length; i++)
				expr = expr.Replace("{" + i + "}", argExprs[i]);
			if (invocationTarget.ReturnAbi.CarrierKindCode == AotCoreIrAbiCarrierKind.Void)
				builder.AppendLine(indentation + "    " + expr + ";");
			else
			{
				builder.AppendLine(indentation + "    const auto chaos_inline_result = " + expr + ";");
				EmitAbiReturnPush(builder, invocationTarget.ReturnAbi, "chaos_inline_result", indentation + "    ");
			}
			builder.AppendLine(indentation + "}");
			return;
		}

		if (_nativeSymbolToDispatchSlot?.TryGetValue(invocationTarget.TargetSymbol, out int slotIndex) == true)
		{
			EmitHotpatchResolvedInvocation(builder, slotIndex, invocationTarget.TargetSymbol, invocationTarget.ParameterAbis, invocationTarget.ReturnAbi, invocationTarget.RawArgumentIndices, indentation);
		}
		else if (invocationTarget.ExternalRuntimeTableIndex >= 0 || invocationTarget.DirectNativeSymbol != null)
		{
			EmitExternalRuntimeTableDispatch(builder, invocationTarget, indentation);
		}
		else
		{
			EmitLinearResolvedInvocation(builder, invocationTarget.TargetSymbol, invocationTarget.ParameterAbis, invocationTarget.ReturnAbi, invocationTarget.RawArgumentIndices, indentation, enforceInstanceNullCheck: false);
		}
	}

	private void EmitLinearCallVirt(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string indentation)
	{
		if (IsDelegateInvokeInstruction(instruction))
		{
			EmitLinearDelegateInvoke(builder, instruction, indentation);
			return;
		}

		// Phase 3: AOT Devirtualization fast-path for linear emission (inside branches of structured nodes)
		string devirtKey = instruction.Callee ?? instruction.TargetReference?.SubjectId ?? "";
		if (devirtKey.Length > 0 && _devirtualizationHints.TryGetValue(devirtKey, out DevirtualizationHint devirtHint) && devirtHint.CanDevirtualize)
		{
			AotCoreIrMethodArtifact devirtMethod = _methodsBySubjectId[devirtHint.ImplementationMethodSubjectId];
			IReadOnlyList<AotCoreIrAbiSlotArtifact> devirtParams = GetMethodAbiParameterSlots(devirtMethod);
			string devirtRet = MapAbiSlotReturnType(devirtMethod.ReturnAbi);
			string devirtSymbol = devirtMethod.NativeSymbol;
			// Pop arguments from eval stack and create converted variables (same as EmitLinearResolvedInvocation)
			builder.AppendLine($"{indentation}{{");
			for (int devirtIdx = devirtParams.Count - 1; devirtIdx >= 0; devirtIdx--)
			{
				builder.AppendLine($"{indentation}    const auto chaos_raw_arg_{devirtIdx} = {ConsumeEvalStackValueExpression()};");
				builder.AppendLine($"{indentation}    const auto chaos_arg_{devirtIdx} = {FormatInboundAbiArgumentExpression(devirtParams[devirtIdx], $"chaos_raw_arg_{devirtIdx}")};");
			}
			if (devirtParams.Count > 0)
			{
				builder.AppendLine($"{indentation}    if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))");
				builder.AppendLine($"{indentation}    {{");
				builder.AppendLine($"{indentation}        CHAOS_IL2CPP_FAIL();");
				builder.AppendLine($"{indentation}    }}");
			}
			string devirtArgs = FormatAbiInvocationArgumentList(devirtParams);
			if (string.Equals(devirtRet, "void", StringComparison.Ordinal))
			{
				builder.AppendLine($"{indentation}    {devirtSymbol}({devirtArgs});");
			}
			else
			{
				builder.AppendLine($"{indentation}    auto chaos_devirt_result = {devirtSymbol}({devirtArgs});");
				EmitAbiReturnPush(builder, devirtMethod.ReturnAbi, "chaos_devirt_result", $"{indentation}    ");
			}
			builder.AppendLine($"{indentation}}}");
			return;
		}

		switch (instruction.DispatchKindCode.GetValueOrDefault())
		{
		case HybridDispatchKind.None:
		case HybridDispatchKind.Direct:
		case HybridDispatchKind.ExternalRuntime:
		{
			InvocationTarget invocationTarget = ResolveDirectInvocationTarget(instruction);

		// Inline shape expansion: substitute args into expression template at call site.
			if (invocationTarget.InlineCppExpression is { } inlineExpr)
			{
				builder.AppendLine($"{indentation}{{");
				var argExprs = new string[invocationTarget.ParameterAbis.Count];
				for (int i = invocationTarget.ParameterAbis.Count - 1; i >= 0; i--)
					argExprs[i] = ConsumeEvalStackValueExpression();
				string expr = inlineExpr;
				for (int i = 0; i < argExprs.Length; i++)
					expr = expr.Replace($"{{{i}}}", argExprs[i]);
				if (invocationTarget.ReturnAbi.CarrierKindCode == AotCoreIrAbiCarrierKind.Void)
					builder.AppendLine($"{indentation}    {expr};");
				else
				{
					builder.AppendLine($"{indentation}    const auto chaos_inline_result = {expr};");
					EmitAbiReturnPush(builder, invocationTarget.ReturnAbi, "chaos_inline_result", indentation + "    ");
				}
				builder.AppendLine($"{indentation}}}");
				return;
			}

			if (_nativeSymbolToDispatchSlot?.TryGetValue(invocationTarget.TargetSymbol, out int slotIndex) == true)
			{
				EmitHotpatchResolvedInvocation(builder, slotIndex, invocationTarget.TargetSymbol, invocationTarget.ParameterAbis, invocationTarget.ReturnAbi, invocationTarget.RawArgumentIndices, indentation);
			}
			else if (invocationTarget.ExternalRuntimeTableIndex >= 0)
			{
				EmitExternalRuntimeTableDispatch(builder, invocationTarget, indentation);
			}
			else
			{
				EmitLinearResolvedInvocation(builder, invocationTarget.TargetSymbol, invocationTarget.ParameterAbis, invocationTarget.ReturnAbi, invocationTarget.RawArgumentIndices, indentation, enforceInstanceNullCheck: true);
			}
			return;
		}
		case HybridDispatchKind.Virtual:
		{
			EmitLinearVirtualDispatchCall(builder, instruction, indentation);
			return;
		}
		default:
			throw new NotSupportedException($"native-aot structured EH linear lowering does not support callvirt dispatch kind '{instruction.DispatchKindCode}'.");
		}
	}

	private void EmitLinearDelegateInvoke(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string indentation)
	{
		string methodDeclaringTypeSubjectId = GetMethodDeclaringTypeSubjectId(instruction.Callee!);
		IReadOnlyList<AotCoreIrAbiSlotArtifact> parameterAbis = ResolveDelegateInvokeParameterAbis(instruction);
		AotCoreIrAbiSlotArtifact returnAbi = ResolveDelegateInvokeReturnAbi(instruction);
		string returnType = MapAbiSlotReturnType(returnAbi);
		string sigCache = FormatAbiSlotParameterSignature(parameterAbis);
		string openFnType = string.IsNullOrEmpty(sigCache) ? (returnType + "(*)()") : string.Concat(returnType, "(*)(", sigCache, ")");
		string closedFnType = (string.IsNullOrEmpty(sigCache) ? (returnType + "(*)(CHAOS_IL2CPP_INTPTR chaos_delegate_target)") : (returnType + "(*)(CHAOS_IL2CPP_INTPTR chaos_delegate_target, " + sigCache + ")"));

		builder.AppendLine($"{indentation}{{");
		for (int i = parameterAbis.Count - 1; i >= 0; i--)
		{
			builder.AppendLine($"{indentation}    auto chaos_raw_arg_{i} = {ConsumeEvalStackValueExpression()};");
			if (_stringIdMapping is { Count: > 0 } && IsStringParameterSlot(parameterAbis[i]))
			{
				builder.AppendLine($"{indentation}    if (chaos_is_string_id(chaos_raw_arg_{i}))");
				builder.AppendLine($"{indentation}    {{");
				builder.AppendLine($"{indentation}        chaos_raw_arg_{i} = chaos_string_materialize(chaos_raw_arg_{i});");
				builder.AppendLine($"{indentation}    }}");
			}
			builder.AppendLine($"{indentation}    const auto chaos_arg_{i} = {FormatInboundAbiArgumentExpression(parameterAbis[i], $"chaos_raw_arg_{i}")};");
		}
		builder.AppendLine($"{indentation}    const auto chaos_delegate_value = {ConsumeEvalStackValueExpression()};");
		builder.AppendLine($"{indentation}    if (chaos_delegate_value == static_cast<CHAOS_IL2CPP_INTPTR>(0))");
		builder.AppendLine($"{indentation}    {{");
		builder.AppendLine($"{indentation}        CHAOS_IL2CPP_FAIL();");
		builder.AppendLine($"{indentation}    }}");
		builder.AppendLine($"{indentation}    auto* chaos_delegate = reinterpret_cast<{GetNativeTypeSymbol(methodDeclaringTypeSubjectId)}*>(chaos_delegate_value);");
		builder.AppendLine($"{indentation}    if (chaos_delegate->chaos_delegate_invocation_count > static_cast<CHAOS_IL2CPP_INTPTR>(0))");
		builder.AppendLine($"{indentation}    {{");
		builder.AppendLine($"{indentation}        const auto* chaos_invocation_list = reinterpret_cast<const CHAOS_IL2CPP_VECTOR(CHAOS_IL2CPP_INTPTR)*>(chaos_delegate->chaos_delegate_invocation_list);");
		builder.AppendLine($"{indentation}        if (chaos_invocation_list == nullptr ||");
		builder.AppendLine($"{indentation}            static_cast<CHAOS_IL2CPP_INTPTR>(chaos_invocation_list->size()) != chaos_delegate->chaos_delegate_invocation_count)");
		builder.AppendLine($"{indentation}        {{");
		builder.AppendLine($"{indentation}            CHAOS_IL2CPP_FAIL();");
		builder.AppendLine($"{indentation}        }}");
		builder.AppendLine();
		if (!string.Equals(returnType, "void", StringComparison.Ordinal))
		{
			builder.AppendLine($"{indentation}        {returnType} chaos_result{{}};");
		}
		builder.AppendLine($"{indentation}        for (CHAOS_IL2CPP_SIZE chaos_delegate_index = 0; chaos_delegate_index < chaos_invocation_list->size(); ++chaos_delegate_index)");
		builder.AppendLine($"{indentation}        {{");
		builder.AppendLine($"{indentation}            const auto chaos_invocation_delegate_value = (*chaos_invocation_list)[chaos_delegate_index];");
		builder.AppendLine($"{indentation}            if (chaos_invocation_delegate_value == static_cast<CHAOS_IL2CPP_INTPTR>(0))");
		builder.AppendLine($"{indentation}            {{");
		builder.AppendLine($"{indentation}                CHAOS_IL2CPP_FAIL();");
		builder.AppendLine($"{indentation}            }}");
		builder.AppendLine($"{indentation}            auto* chaos_invocation_delegate = reinterpret_cast<{GetNativeTypeSymbol(methodDeclaringTypeSubjectId)}*>(chaos_invocation_delegate_value);");
		builder.AppendLine($"{indentation}            if (chaos_invocation_delegate->chaos_delegate_method_ptr == static_cast<CHAOS_IL2CPP_INTPTR>(0))");
		builder.AppendLine($"{indentation}            {{");
		builder.AppendLine($"{indentation}                CHAOS_IL2CPP_FAIL();");
		builder.AppendLine($"{indentation}            }}");
		builder.AppendLine($"{indentation}            if (chaos_invocation_delegate->chaos_delegate_target == static_cast<CHAOS_IL2CPP_INTPTR>(0))");
		builder.AppendLine($"{indentation}            {{");
		builder.AppendLine($"{indentation}                const auto chaos_open_function = reinterpret_cast<{openFnType}>(chaos_invocation_delegate->chaos_delegate_method_ptr);");
		string openCall = "chaos_open_function(" + FormatAbiInvocationArgumentList(parameterAbis) + ")";
		if (string.Equals(returnType, "void", StringComparison.Ordinal))
		{
			builder.AppendLine($"{indentation}                {openCall};");
		}
		else
		{
			builder.AppendLine($"{indentation}                chaos_result = {openCall};");
		}
		builder.AppendLine($"{indentation}            }}");
		builder.AppendLine($"{indentation}            else");
		builder.AppendLine($"{indentation}            {{");
		builder.AppendLine($"{indentation}                const auto chaos_closed_function = reinterpret_cast<{closedFnType}>(chaos_invocation_delegate->chaos_delegate_method_ptr);");
		string closedCall = "chaos_closed_function(chaos_invocation_delegate->chaos_delegate_target" + ((parameterAbis.Count == 0) ? string.Empty : (", " + FormatAbiInvocationArgumentList(parameterAbis))) + ")";
		if (string.Equals(returnType, "void", StringComparison.Ordinal))
		{
			builder.AppendLine($"{indentation}                {closedCall};");
		}
		else
		{
			builder.AppendLine($"{indentation}                chaos_result = {closedCall};");
		}
		builder.AppendLine($"{indentation}            }}");
		builder.AppendLine($"{indentation}        }}");
		if (!string.Equals(returnType, "void", StringComparison.Ordinal))
		{
			EmitAbiReturnPush(builder, returnAbi, "chaos_result", $"{indentation}        ");
		}
		builder.AppendLine($"{indentation}    }}");
		builder.AppendLine($"{indentation}    else");
		builder.AppendLine($"{indentation}    {{");
		builder.AppendLine($"{indentation}        if (chaos_delegate->chaos_delegate_method_ptr == static_cast<CHAOS_IL2CPP_INTPTR>(0))");
		builder.AppendLine($"{indentation}        {{");
		builder.AppendLine($"{indentation}            CHAOS_IL2CPP_FAIL();");
		builder.AppendLine($"{indentation}        }}");
		builder.AppendLine($"{indentation}        if (chaos_delegate->chaos_delegate_target == static_cast<CHAOS_IL2CPP_INTPTR>(0))");
		builder.AppendLine($"{indentation}        {{");
		builder.AppendLine($"{indentation}            const auto chaos_open_function = reinterpret_cast<{openFnType}>(chaos_delegate->chaos_delegate_method_ptr);");
		if (string.Equals(returnType, "void", StringComparison.Ordinal))
		{
			builder.AppendLine($"{indentation}            {openCall};");
		}
		else
		{
			builder.AppendLine($"{indentation}            const auto chaos_result = {openCall};");
			EmitAbiReturnPush(builder, returnAbi, "chaos_result", $"{indentation}            ");
		}
		builder.AppendLine($"{indentation}        }}");
		builder.AppendLine($"{indentation}        else");
		builder.AppendLine($"{indentation}        {{");
		builder.AppendLine($"{indentation}            const auto chaos_closed_function = reinterpret_cast<{closedFnType}>(chaos_delegate->chaos_delegate_method_ptr);");
		if (string.Equals(returnType, "void", StringComparison.Ordinal))
		{
			builder.AppendLine($"{indentation}            {closedCall};");
		}
		else
		{
			builder.AppendLine($"{indentation}            const auto chaos_result = {closedCall};");
			EmitAbiReturnPush(builder, returnAbi, "chaos_result", $"{indentation}            ");
		}
		builder.AppendLine($"{indentation}        }}");
		builder.AppendLine($"{indentation}    }}");
		builder.AppendLine($"{indentation}}}");
	}

	private void EmitLinearNewObject(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string indentation)
	{
		AotCoreIrReferenceArtifact requiredTargetReference = GetRequiredTargetReference(instruction);
		if (requiredTargetReference.Kind != AotCoreIrReferenceKind.Type)
		{
			throw new NotSupportedException($"native-aot structured EH linear newobj requires type target reference, got '{requiredTargetReference.Kind}'.");
		}
		if (IsDelegateTypeSubjectId(requiredTargetReference.SubjectId, _referenceTypeBaseSubjectIds))
		{
			builder.AppendLine(indentation + "{");
			builder.AppendLine(indentation + $"    const auto chaos_method_ptr = {ConsumeEvalStackValueExpression()};");
			builder.AppendLine(indentation + $"    const auto chaos_target = {ConsumeEvalStackValueExpression()};");
			builder.AppendLine($"{indentation}    auto* chaos_object = CHAOS_IL2CPP_NEW_GC({GetNativeTypeSymbol(requiredTargetReference.SubjectId)}, {{}});");
			builder.AppendLine($"{indentation}    chaos_object->header.type_info = &{GetNativeTypeInfoSymbol(requiredTargetReference.SubjectId)};");
			if (_vtableTypes?.Contains(requiredTargetReference.SubjectId) == true)
			{
				builder.AppendLine($"{indentation}    chaos_object->header.vtable = {GetNativeVTableSymbol(requiredTargetReference.SubjectId)};");
			}
			builder.AppendLine(indentation + "    chaos_object->chaos_delegate_target = chaos_target;");
			builder.AppendLine(indentation + "    chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;");
			EmitEvalStackPush(builder, indentation + "    ", "reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object)");
			builder.AppendLine(indentation + "}");
			return;
		}
		if (requiredTargetReference.TypeShape == AotCoreIrTypeShapeKind.ValueType)
		{
			InvocationTarget invocationTarget = TryResolveDirectInvocationTarget(instruction.Callee) ?? throw new NotSupportedException("native-aot structured EH linear lowering requires constructor target for value-type newobj '" + (instruction.Callee ?? "<null>") + "'.");
			if (invocationTarget.ParameterAbis.Count == 0)
			{
				throw new NotSupportedException("native-aot structured EH linear lowering requires instance constructor ABI for '" + (instruction.Callee ?? "<null>") + "'.");
			}
			if (invocationTarget.ReturnAbi.CarrierKindCode != AotCoreIrAbiCarrierKind.Void)
			{
				throw new NotSupportedException("native-aot structured EH linear lowering requires void constructor return ABI for '" + (instruction.Callee ?? "<null>") + "'.");
			}
			builder.AppendLine(indentation + "{");
			for (int num = invocationTarget.ParameterAbis.Count - 1; num >= 1; num--)
			{
				builder.AppendLine($"{indentation}    const auto chaos_raw_arg_{num} = {ConsumeEvalStackValueExpression()};");
				builder.AppendLine(invocationTarget.RawArgumentIndices.Contains(num)
					? $"{indentation}    const auto chaos_arg_{num} = chaos_raw_arg_{num};"
					: $"{indentation}    const auto chaos_arg_{num} = {FormatInboundAbiArgumentExpression(invocationTarget.ParameterAbis[num], $"chaos_raw_arg_{num}")};");
			}
			builder.AppendLine(indentation + "    CHAOS_IL2CPP_INTPTR chaos_value = static_cast<CHAOS_IL2CPP_INTPTR>(0);");
			var ctorArgs0 = FormatAbiInvocationArgumentList(invocationTarget.ParameterAbis);
			if (ctorArgs0.StartsWith("chaos_arg_0", StringComparison.Ordinal))
				ctorArgs0 = "reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_value) | chaos_managed_pointer_local_slot_tag" + ctorArgs0.Substring(11);
			builder.AppendLine($"{indentation}    {invocationTarget.TargetSymbol}({ctorArgs0});");
			EmitEvalStackPush(builder, indentation + "    ", "chaos_value");
			builder.AppendLine(indentation + "}");
			return;
		}
		if (TryResolveDirectInvocationTarget(instruction.Callee) is { } constructorTarget)
		{
			if (constructorTarget.ParameterAbis.Count == 0)
			{
				throw new NotSupportedException("native-aot structured EH linear lowering requires instance constructor ABI for '" + (instruction.Callee ?? "<null>") + "'.");
			}
			if (constructorTarget.ReturnAbi.CarrierKindCode != AotCoreIrAbiCarrierKind.Void)
			{
				throw new NotSupportedException("native-aot structured EH linear lowering requires void constructor return ABI for '" + (instruction.Callee ?? "<null>") + "'.");
			}
			builder.AppendLine(indentation + "{");
			for (int num2 = constructorTarget.ParameterAbis.Count - 1; num2 >= 1; num2--)
			{
				builder.AppendLine($"{indentation}    const auto chaos_raw_arg_{num2} = {ConsumeEvalStackValueExpression()};");
				builder.AppendLine(constructorTarget.RawArgumentIndices.Contains(num2)
					? $"{indentation}    const auto chaos_arg_{num2} = chaos_raw_arg_{num2};"
					: $"{indentation}    const auto chaos_arg_{num2} = {FormatInboundAbiArgumentExpression(constructorTarget.ParameterAbis[num2], $"chaos_raw_arg_{num2}")};");
			}
			builder.AppendLine($"{indentation}    auto* chaos_object = CHAOS_IL2CPP_NEW_GC({GetNativeTypeSymbol(requiredTargetReference.SubjectId)}, {{}});");
			builder.AppendLine($"{indentation}    chaos_object->header.type_info = &{GetNativeTypeInfoSymbol(requiredTargetReference.SubjectId)};");
			if (_vtableTypes?.Contains(requiredTargetReference.SubjectId) == true)
			{
				builder.AppendLine($"{indentation}    chaos_object->header.vtable = {GetNativeVTableSymbol(requiredTargetReference.SubjectId)};");
			}
			var ctorArgs2 = FormatAbiInvocationArgumentList(constructorTarget.ParameterAbis);
			if (ctorArgs2.StartsWith("chaos_arg_0", StringComparison.Ordinal))
				ctorArgs2 = "reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object)" + ctorArgs2.Substring(11);
			builder.AppendLine($"{indentation}    {constructorTarget.TargetSymbol}({ctorArgs2});");
			EmitEvalStackPush(builder, indentation + "    ", "reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object)");
			builder.AppendLine(indentation + "}");
			return;
		}
		builder.AppendLine(indentation + "{");
		builder.AppendLine($"{indentation}    auto* chaos_object = CHAOS_IL2CPP_NEW_GC({GetNativeTypeSymbol(requiredTargetReference.SubjectId)}, {{}});");
		builder.AppendLine($"{indentation}    chaos_object->header.type_info = &{GetNativeTypeInfoSymbol(requiredTargetReference.SubjectId)};");
		if (_vtableTypes?.Contains(requiredTargetReference.SubjectId) == true)
		{
			builder.AppendLine($"{indentation}    chaos_object->header.vtable = {GetNativeVTableSymbol(requiredTargetReference.SubjectId)};");
		}
		EmitEvalStackPush(builder, indentation + "    ", "reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object)");
		builder.AppendLine(indentation + "}");
	}

	private void EmitLinearResolvedInvocation(StringBuilder builder, string targetSymbol, IReadOnlyList<AotCoreIrAbiSlotArtifact> parameterAbis, AotCoreIrAbiSlotArtifact returnAbi, IReadOnlySet<int> rawArgumentIndices, string indentation, bool enforceInstanceNullCheck)
	{
		string a = MapAbiSlotReturnType(returnAbi);
		StringBuilder stringBuilder = builder;
		StringBuilder stringBuilder2 = stringBuilder;
		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(1, 1, stringBuilder);
		handler.AppendFormatted(indentation);
		handler.AppendLiteral("{");
		stringBuilder2.AppendLine(ref handler);
		for (int num = parameterAbis.Count - 1; num >= 0; num--)
		{
			builder.AppendLine($"{indentation}    const auto chaos_raw_arg_{num} = {ConsumeEvalStackValueExpression()};");
			builder.AppendLine(rawArgumentIndices.Contains(num) ? $"{indentation}    const auto chaos_arg_{num} = chaos_raw_arg_{num};" : $"{indentation}    const auto chaos_arg_{num} = {FormatInboundAbiArgumentExpression(parameterAbis[num], $"chaos_raw_arg_{num}")};");
		}
		if (enforceInstanceNullCheck && parameterAbis.Count > 0)
		{
			builder.AppendLine(indentation + "    if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))");
			builder.AppendLine(indentation + "    {");
			builder.AppendLine(indentation + "        CHAOS_IL2CPP_FAIL();");
			builder.AppendLine(indentation + "    }");
		}
		string value = targetSymbol + "(" + FormatAbiInvocationArgumentList(parameterAbis) + ")";
		if (string.Equals(a, "void", StringComparison.Ordinal))
		{
			stringBuilder = builder;
			StringBuilder stringBuilder4 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(5, 2, stringBuilder);
			handler.AppendFormatted(indentation);
			handler.AppendLiteral("    ");
			handler.AppendFormatted(value);
			handler.AppendLiteral(";");
			stringBuilder4.AppendLine(ref handler);
		}
		else
		{
			stringBuilder = builder;
			StringBuilder stringBuilder5 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(31, 2, stringBuilder);
			handler.AppendFormatted(indentation);
			handler.AppendLiteral("    const auto chaos_result = ");
			handler.AppendFormatted(value);
			handler.AppendLiteral(";");
			stringBuilder5.AppendLine(ref handler);
			EmitAbiReturnPush(builder, returnAbi, "chaos_result", indentation + "    ");
		}
		stringBuilder = builder;
		StringBuilder stringBuilder6 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(1, 1, stringBuilder);
		handler.AppendFormatted(indentation);
		handler.AppendLiteral("}");
		stringBuilder6.AppendLine(ref handler);
	}

	private void EmitExternalRuntimeTableDispatch(StringBuilder builder, InvocationTarget invocationTarget, string indentation)
	{
		string returnType = MapAbiSlotReturnType(invocationTarget.ReturnAbi);
		string paramTypes = FormatAbiSlotParameterTypes(invocationTarget.ParameterAbis);

		// When a DirectNativeSymbol is available, emit a direct function call
		// instead of an indirect dispatch table call. This enables:
		//   1. Compiler inlining (the call is a known symbol at compile time)
		//   2. No function pointer dereference overhead
		//   3. Better code generation (the compiler sees the full call graph)
		if (invocationTarget.DirectNativeSymbol is { } nativeSymbol)
		{
			builder.AppendLine($"{indentation}{{");
			for (int i = invocationTarget.ParameterAbis.Count - 1; i >= 0; i--)
			{
				builder.AppendLine($"{indentation}    const auto chaos_raw_arg_{i} = {ConsumeEvalStackValueExpression()};");
				builder.AppendLine(invocationTarget.RawArgumentIndices.Contains(i)
					? $"{indentation}    const auto chaos_arg_{i} = chaos_raw_arg_{i};"
					: $"{indentation}    const auto chaos_arg_{i} = {FormatInboundAbiArgumentExpression(invocationTarget.ParameterAbis[i], $"chaos_raw_arg_{i}")};");
			}
			string directNativeArgs = FormatAbiInvocationArgumentList(invocationTarget.ParameterAbis);
			if (string.Equals(returnType, "void", StringComparison.Ordinal))
			{
				builder.AppendLine($"{indentation}    {nativeSymbol}({directNativeArgs});");
			}
			else
			{
				builder.AppendLine($"{indentation}    const auto chaos_result = {nativeSymbol}({directNativeArgs});");
				EmitAbiReturnPush(builder, invocationTarget.ReturnAbi, "chaos_result", indentation + "    ");
			}
			builder.AppendLine($"{indentation}}}");
			return;
		}

		string fnType = string.IsNullOrEmpty(paramTypes)
			? $"{returnType}(*)()"
			: $"{returnType}(*)({paramTypes})";
		int idx = invocationTarget.ExternalRuntimeTableIndex;

		builder.AppendLine($"{indentation}{{");
		for (int i = invocationTarget.ParameterAbis.Count - 1; i >= 0; i--)
		{
			builder.AppendLine($"{indentation}    const auto chaos_raw_arg_{i} = {ConsumeEvalStackValueExpression()};");
			builder.AppendLine(invocationTarget.RawArgumentIndices.Contains(i)
				? $"{indentation}    const auto chaos_arg_{i} = chaos_raw_arg_{i};"
				: $"{indentation}    const auto chaos_arg_{i} = {FormatInboundAbiArgumentExpression(invocationTarget.ParameterAbis[i], $"chaos_raw_arg_{i}")};");
		}
		string args = FormatAbiInvocationArgumentList(invocationTarget.ParameterAbis);
		if (string.Equals(returnType, "void", StringComparison.Ordinal))
		{
			builder.AppendLine($"{indentation}    reinterpret_cast<{fnType}>(kChaosExternalRuntimeFnTable[{idx}])({args});");
		}
		else
		{
			builder.AppendLine($"{indentation}    const auto chaos_result = reinterpret_cast<{fnType}>(kChaosExternalRuntimeFnTable[{idx}])({args});");
			EmitAbiReturnPush(builder, invocationTarget.ReturnAbi, "chaos_result", indentation + "    ");
		}
		builder.AppendLine($"{indentation}}}");
	}

	private void EmitLinearVirtualDispatchCall(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string indentation)
	{
		AotCoreIrMethodArtifact dispatchSlotMethod = ResolveRequiredDispatchSlotMethod(instruction);
		IReadOnlyList<AotCoreIrAbiSlotArtifact> paramAbis = GetMethodAbiParameterSlots(dispatchSlotMethod);
		string returnType = MapAbiSlotReturnType(dispatchSlotMethod.ReturnAbi);
		string vtableSlotSig = GetMethodSignatureSuffix(dispatchSlotMethod.SubjectId);
		bool isInterface = !string.IsNullOrEmpty(dispatchSlotMethod.Identity.DeclaringTypeSubjectId) &&
			_interfaceTypeSubjectIds != null &&
			_interfaceTypeSubjectIds.Contains(dispatchSlotMethod.Identity.DeclaringTypeSubjectId);

		builder.AppendLine($"{indentation}{{");
		for (int i = paramAbis.Count - 1; i >= 0; i--)
		{
			builder.AppendLine($"{indentation}    const auto chaos_raw_arg_{i} = {ConsumeEvalStackValueExpression()};");
			builder.AppendLine($"{indentation}    const auto chaos_arg_{i} = {FormatInboundAbiArgumentExpression(paramAbis[i], $"chaos_raw_arg_{i}")};");
		}
		// Null check
		builder.AppendLine($"{indentation}    if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))");
		builder.AppendLine($"{indentation}    {{");
		builder.AppendLine($"{indentation}        CHAOS_IL2CPP_FAIL();");
		builder.AppendLine($"{indentation}    }}");
		// Determine header kind for dispatch path
		string declaringTypeId = GetMethodDeclaringTypeSubjectId(instruction.Callee!);
		bool isFat = GetHeaderKind(declaringTypeId) == HeaderKind.Fat;
		// VTable resolve — Fat: 1-deref (header->vtable), others: 2-deref (ti->vtable_array)
		string vtableSource = isFat
			? $"reinterpret_cast<FatHeader*>(chaos_arg_0)->vtable"
			: $"chaos_object_get_type_info(reinterpret_cast<void*>(chaos_arg_0))->vtable_array";
		if (!string.Equals(returnType, "void", StringComparison.Ordinal))
		{
			builder.AppendLine($"{indentation}    {returnType} chaos_callvirt_result{{}};");
		}
		// VTable dispatch
		if (_vtableSlotMap != null && _vtableSlotMap.TryGetValue(vtableSlotSig, out int vtableSlot))
		{
			string vtableArgs = FormatAbiInvocationArgumentList(paramAbis, "chaos_arg_0");
			string vtableParamSig = FormatAbiSlotParameterSignature(paramAbis);
			string vtableFnType = string.IsNullOrEmpty(vtableParamSig)
				? $"{returnType}(*)()"
				: $"{returnType}(*)({vtableParamSig})";
			if (string.Equals(returnType, "void", StringComparison.Ordinal))
			{
				string fnCall = $"reinterpret_cast<{vtableFnType}>(chaos_vtable_resolve({vtableSource}, {vtableSlot}u))";
				builder.AppendLine($"{indentation}    (*{fnCall})({vtableArgs});");
			}
			else
			{
				string fnCall = $"reinterpret_cast<{vtableFnType}>(chaos_vtable_resolve({vtableSource}, {vtableSlot}u))";
				builder.AppendLine($"{indentation}    chaos_callvirt_result = (*{fnCall})({vtableArgs});");
			}
		}
		else
		{
			builder.AppendLine($"{indentation}    CHAOS_IL2CPP_FAIL();");
		}
		if (!string.Equals(returnType, "void", StringComparison.Ordinal))
		{
			EmitAbiReturnPush(builder, dispatchSlotMethod.ReturnAbi, "chaos_callvirt_result", indentation + "    ");
		}
		builder.AppendLine($"{indentation}}}");
	}

	private void EmitHotpatchResolvedInvocation(StringBuilder builder, int dispatchSlotIndex, string targetSymbol, IReadOnlyList<AotCoreIrAbiSlotArtifact> parameterAbis, AotCoreIrAbiSlotArtifact returnAbi, IReadOnlySet<int> rawArgumentIndices, string indentation)
	{
		string returnType = MapAbiSlotReturnType(returnAbi);
		int argBufferSize = CalculateArgBufferSize(parameterAbis);
		// Minimum 1 byte to avoid zero-length array error in C++
		if (argBufferSize < 1) argBufferSize = 1;
		bool hasReturn = !string.Equals(returnType, "void", StringComparison.Ordinal);
		builder.AppendLine($"{indentation}{{");
		for (int i = parameterAbis.Count - 1; i >= 0; i--)
		{
			builder.AppendLine($"{indentation}    const auto chaos_raw_arg_{i} = {ConsumeEvalStackValueExpression()};");
			builder.AppendLine(rawArgumentIndices.Contains(i)
				? $"{indentation}    const auto chaos_arg_{i} = chaos_raw_arg_{i};"
				: $"{indentation}    const auto chaos_arg_{i} = {FormatInboundAbiArgumentExpression(parameterAbis[i], $"chaos_raw_arg_{i}")};");
		}
		builder.AppendLine($"{indentation}    auto& _d{dispatchSlotIndex} = s_hotpatch_entries[{dispatchSlotIndex}];");
		if (hasReturn)
		{
			builder.AppendLine($"{indentation}    {returnType} _d_hpresult{{}};");
		}
		builder.AppendLine($"{indentation}    if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d{dispatchSlotIndex})");
			builder.AppendLine($"{indentation}        && !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d{dispatchSlotIndex}))");
		builder.AppendLine($"{indentation}    {{");
		builder.AppendLine($"{indentation}        alignas(16) uint8_t _d_ab[{argBufferSize}];");
		builder.AppendLine($"{indentation}        ArgBuffer _d_bw(_d_ab);");
		for (int i = 0; i < parameterAbis.Count; i++)
		{
			builder.AppendLine($"{indentation}        _d_bw.{GetArgBufferWriteCall(parameterAbis[i].CarrierKindCode, $"chaos_arg_{i}")};");
		}
		if (hasReturn)
		{
			builder.AppendLine($"{indentation}        ::chaos::il2cpp::runtime_core::InterpreterEntryDirect(");
			builder.AppendLine($"{indentation}            _d{dispatchSlotIndex}.method_key, _d_ab, &_d_hpresult);");
		}
		else
		{
			builder.AppendLine($"{indentation}        ::chaos::il2cpp::runtime_core::InterpreterEntryDirect(");
			builder.AppendLine($"{indentation}            _d{dispatchSlotIndex}.method_key, _d_ab, nullptr);");
		}
		builder.AppendLine($"{indentation}    }}");
		builder.AppendLine($"{indentation}    else");
		builder.AppendLine($"{indentation}    {{");
		string callExpr = $"{targetSymbol}({FormatAbiInvocationArgumentList(parameterAbis)})";
		if (hasReturn)
		{
			builder.AppendLine($"{indentation}        _d_hpresult = {callExpr};");
		}
		else
		{
			builder.AppendLine($"{indentation}        {callExpr};");
		}
		builder.AppendLine($"{indentation}    }}");
		if (hasReturn)
		{
			EmitAbiReturnPush(builder, returnAbi, "_d_hpresult", $"{indentation}    ");
		}
		builder.AppendLine($"{indentation}}}");
	}

	private void EmitLinearBinaryArithmetic(StringBuilder builder, string indentation, string helperName)
	{
		string _rType = PeekSlotType();
		string _rExpr = ConsumeEvalStackValueExpression();
		string _lType = PeekSlotType();
		string _lExpr = ConsumeEvalStackValueExpression();
		string _rLoad = (_rType == "Float32" || _rType == "Float64") ? $"ChaosLoadFloat64({_rExpr})" : $"static_cast<CHAOS_IL2CPP_INT32>({_rExpr})";
		string _lLoad = (_lType == "Float32" || _lType == "Float64") ? $"ChaosLoadFloat64({_lExpr})" : $"static_cast<CHAOS_IL2CPP_INT32>({_lExpr})";
		builder.AppendLine($"{indentation}{{");
		builder.AppendLine($"{indentation}    const auto chaos_right = {_rLoad};");
		builder.AppendLine($"{indentation}    const auto chaos_left = {_lLoad};");
		EmitEvalStackPush(builder, indentation + "    ", $"static_cast<CHAOS_IL2CPP_INTPTR>({helperName}(chaos_left, chaos_right))");
		builder.AppendLine($"{indentation}}}");
		PushSlotType("NativeInt");
	}

	private void EmitLinearBinaryBitwise(StringBuilder builder, string indentation, string operation)
	{
		builder.AppendLine($"{indentation}{{");
		builder.AppendLine($"{indentation}    const auto chaos_right = chaos_load_uint64({ConsumeEvalStackValueExpression()});");
		builder.AppendLine($"{indentation}    const auto chaos_left = chaos_load_uint64({ConsumeEvalStackValueExpression()});");
		EmitEvalStackPush(builder, indentation + "    ", $"chaos_store_uint64(chaos_left {operation} chaos_right)");
		builder.AppendLine($"{indentation}}}");
	}

	private void EmitLinearFieldLoad(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string indentation)
	{
		AotCoreIrReferenceArtifact requiredTargetReference = GetRequiredTargetReference(instruction);
		if (requiredTargetReference.Kind != AotCoreIrReferenceKind.Field)
		{
			throw new NotSupportedException($"native-aot structured EH linear field load requires field target reference, got '{requiredTargetReference.Kind}'.");
		}
		StringBuilder stringBuilder;
		StringBuilder.AppendInterpolatedStringHandler handler;
		if (instruction.RuntimeServiceKind == AotCoreIrRuntimeServiceKind.LoadStaticField)
		{
			EmitStaticInitializationForField(builder, requiredTargetReference.SubjectId, indentation);
			stringBuilder = builder;
			EmitEvalStackPush(builder, indentation, GetNativeStaticFieldSymbol(requiredTargetReference.SubjectId));
			return;
		}
		stringBuilder = builder;
		StringBuilder stringBuilder3 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(1, 1, stringBuilder);
		handler.AppendFormatted(indentation);
		handler.AppendLiteral("{");
		stringBuilder3.AppendLine(ref handler);
		if (requiredTargetReference.DeclaringTypeShape == AotCoreIrTypeShapeKind.ValueType)
		{
			string requiredDeclaringTypeSubjectId = GetRequiredDeclaringTypeSubjectId(requiredTargetReference);
			builder.AppendLine($"{indentation}    auto* chaos_value = chaos_resolve_managed_value_pointer<{GetNativeValueTypeSymbol(requiredDeclaringTypeSubjectId)}>({ConsumeEvalStackValueExpression()});");
			EmitEvalStackPush(builder, indentation + "    ", $"chaos_value->{GetNativeFieldMemberName(requiredTargetReference.SubjectId)}");
		}
		else
		{
			string requiredDeclaringTypeSubjectId2 = GetRequiredDeclaringTypeSubjectId(requiredTargetReference);
			builder.AppendLine($"{indentation}    auto* chaos_object = reinterpret_cast<{GetNativeTypeSymbol(requiredDeclaringTypeSubjectId2)}*>({ConsumeEvalStackValueExpression()});");
			EmitEvalStackPush(builder, indentation + "    ", $"chaos_object->{GetNativeFieldMemberName(requiredTargetReference.SubjectId)}");
		}
		stringBuilder = builder;
		StringBuilder stringBuilder8 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(1, 1, stringBuilder);
		handler.AppendFormatted(indentation);
		handler.AppendLiteral("}");
		stringBuilder8.AppendLine(ref handler);
	}

	private void EmitLinearLoadStringLiteral(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string indentation)
	{
		string requiredStringOperand = GetRequiredStringOperand(instruction);

		// When StringId mapping is available, emit a tagged StringId (zero allocation).
		// The runtime string_table resolves it, and all downstream consumers
		// (stfld, stind.ref, stelem.ref, delegate invoke, external runtime helpers)
		// already materialize StringId via chaos_string_materialize() or handle it internally.
		if (_stringIdMapping is { Count: > 0 } && TryGetStringId(requiredStringOperand, out _))
		{
			builder.AppendLine($"{indentation}{{{{");
			EmitEvalStackPush(builder, indentation + "    ", $"CHAOS_IL2CPP_STRING_ID({ToCppStringLiteral(requiredStringOperand)})");
			builder.AppendLine($"{indentation}}}}}");
			return;
		}

		StringBuilder stringBuilder = builder;
		StringBuilder stringBuilder2 = stringBuilder;
		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(1, 1, stringBuilder);
		handler.AppendFormatted(indentation);
		handler.AppendLiteral("{");
		stringBuilder2.AppendLine(ref handler);
		stringBuilder = builder;
		StringBuilder stringBuilder3 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(46, 2, stringBuilder);
		handler.AppendFormatted(indentation);
		handler.AppendLiteral("    auto* chaos_string = CHAOS_IL2CPP_NEW_GC(");
		handler.AppendFormatted(GetNativeTypeSymbol("System.Private.CoreLib/System.String"));
		handler.AppendLiteral(");");
		stringBuilder3.AppendLine(ref handler);
		stringBuilder = builder;
		StringBuilder stringBuilder4 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(36, 2, stringBuilder);
		handler.AppendFormatted(indentation);
		handler.AppendLiteral("    chaos_string->header.type_info = &");
		handler.AppendFormatted(GetNativeTypeInfoSymbol("System.Private.CoreLib/System.String"));
		handler.AppendLiteral(";");
		stringBuilder4.AppendLine(ref handler);
		if (_vtableTypes?.Contains("System.Private.CoreLib/System.String") == true)
		{
			stringBuilder = builder;
			StringBuilder stringBuilder_vt4 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(34, 1, stringBuilder);
			handler.AppendLiteral("    chaos_string->header.vtable = ");
			handler.AppendFormatted(GetNativeVTableSymbol("System.Private.CoreLib/System.String"));
			handler.AppendLiteral(";");
			stringBuilder_vt4.AppendLine(ref handler);
		}
		stringBuilder = builder;
		StringBuilder stringBuilder5 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(56, 2, stringBuilder);
		handler.AppendFormatted(indentation);
		handler.AppendLiteral("    chaos_string->length = static_cast<CHAOS_IL2CPP_INTPTR>(");
		handler.AppendFormatted(Encoding.UTF8.GetByteCount(requiredStringOperand));
		handler.AppendLiteral(");");
		stringBuilder5.AppendLine(ref handler);
		stringBuilder = builder;
		StringBuilder stringBuilder6 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(31, 2, stringBuilder);
		handler.AppendFormatted(indentation);
		handler.AppendLiteral("    chaos_string->utf8_data = ");
		handler.AppendFormatted(ToCppStringLiteral(requiredStringOperand));
		handler.AppendLiteral(";");
		stringBuilder6.AppendLine(ref handler);
		EmitEvalStackPush(builder, indentation + "    ", "reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_string)");
		stringBuilder = builder;
		StringBuilder stringBuilder8 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(1, 1, stringBuilder);
		handler.AppendFormatted(indentation);
		handler.AppendLiteral("}");
		stringBuilder8.AppendLine(ref handler);
	}

	private void EmitLinearLoadTypeToken(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string indentation)
	{
		EmitEvalStackPush(builder, indentation, GetRequiredTypeHandleLiteral(instruction));
	}

	private void EmitLinearShift(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string indentation, string helperName)
	{
		RequireInt32IntegralResultType(instruction);
		builder.AppendLine($"{indentation}{{");
		builder.AppendLine($"{indentation}    const auto chaos_shift = static_cast<CHAOS_IL2CPP_INT32>({ConsumeEvalStackValueExpression()});");
		builder.AppendLine($"{indentation}    const auto chaos_value = static_cast<CHAOS_IL2CPP_INT32>({ConsumeEvalStackValueExpression()});");
		EmitEvalStackPush(builder, indentation + "    ", $"static_cast<CHAOS_IL2CPP_INTPTR>({helperName}(chaos_value, chaos_shift))");
		builder.AppendLine($"{indentation}}}");
	}

	private void EmitLinearBitwiseNot(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string indentation)
	{
		RequireInt32IntegralResultType(instruction);
		builder.AppendLine($"{indentation}{{");
		builder.AppendLine($"{indentation}    const auto chaos_value = static_cast<CHAOS_IL2CPP_INT32>({AccessEvalStackTopExpression()});");
		builder.AppendLine($"{indentation}    {AccessEvalStackTopExpression()} = static_cast<CHAOS_IL2CPP_INTPTR>(~chaos_value);");
		builder.AppendLine($"{indentation}}}");
	}

	private void EmitLinearComparisonResult(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string indentation, string operandType, string comparisonOperator)
	{
		RequireInt32IntegralResultType(instruction);
		builder.AppendLine($"{indentation}{{");
		builder.AppendLine($"{indentation}    const auto chaos_right = static_cast<{operandType}>({ConsumeEvalStackValueExpression()});");
		builder.AppendLine($"{indentation}    const auto chaos_left = static_cast<{operandType}>({ConsumeEvalStackValueExpression()});");
		EmitEvalStackPush(builder, indentation + "    ", $"static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left {comparisonOperator} chaos_right ? 1 : 0)");
		builder.AppendLine($"{indentation}}}");
	}

	private static bool LegacyTryCreateCatchOnlyExceptionMethodShape(AotCoreIrMethodArtifact method, out CatchOnlyExceptionMethodShape? catchOnlyShape)
	{
		catchOnlyShape = null;
		if (method.ExceptionRegions.Count != 1)
		{
			return false;
		}
		AotCoreIrExceptionRegionArtifact exceptionRegion = method.ExceptionRegions[0];
		if (exceptionRegion.HandlingKindCode != AotCoreIrExceptionRegionKind.Catch || exceptionRegion.FilterOffset.HasValue || string.IsNullOrEmpty(exceptionRegion.CatchTypeSubjectId))
		{
			return false;
		}
		AotCoreIrInstructionArtifact[] array = method.Instructions.OrderBy(GetRequiredIlOffset).ToArray();
		if (array.Length == 0)
		{
			return false;
		}
		AotCoreIrInstructionArtifact[] array2;
		AotCoreIrInstructionArtifact[] array3;
		AotCoreIrInstructionArtifact[] array4;
		AotCoreIrInstructionArtifact[] array5;
		checked
		{
			int tryEndOffset = exceptionRegion.TryOffset + exceptionRegion.TryLength;
			int handlerEndOffset = exceptionRegion.HandlerOffset + exceptionRegion.HandlerLength;
			if (tryEndOffset != exceptionRegion.HandlerOffset)
			{
				return false;
			}
			array2 = array.Where((AotCoreIrInstructionArtifact instruction) => GetRequiredIlOffset(instruction) < exceptionRegion.TryOffset).ToArray();
			array3 = array.Where(delegate(AotCoreIrInstructionArtifact instruction)
			{
				int requiredIlOffset = GetRequiredIlOffset(instruction);
				return requiredIlOffset >= exceptionRegion.TryOffset && requiredIlOffset < tryEndOffset;
			}).ToArray();
			array4 = array.Where(delegate(AotCoreIrInstructionArtifact instruction)
			{
				int requiredIlOffset = GetRequiredIlOffset(instruction);
				return requiredIlOffset >= exceptionRegion.HandlerOffset && requiredIlOffset < handlerEndOffset;
			}).ToArray();
			array5 = array.Where((AotCoreIrInstructionArtifact instruction) => GetRequiredIlOffset(instruction) >= handlerEndOffset).ToArray();
		}
		if (array3.Length == 0 || array4.Length < 2 || array5.Length == 0 || array.Length != array2.Length + array3.Length + array4.Length + array5.Length)
		{
			return false;
		}
		if ((!string.Equals(array4[0].Op, "pop", StringComparison.Ordinal) && !string.Equals(array4[0].Op, "stloc", StringComparison.Ordinal)) || !string.Equals(array3[^1].Op, "leave", StringComparison.Ordinal) || !string.Equals(array4[^1].Op, "leave", StringComparison.Ordinal))
		{
			return false;
		}
		int[] array6 = array3.Where((AotCoreIrInstructionArtifact instruction) => string.Equals(instruction.Op, "leave", StringComparison.Ordinal)).Select(GetRequiredIntOperand).ToArray();
		int requiredIntOperand = GetRequiredIntOperand(array4[^1]);
		HashSet<int> tailOffsets = array5.Select(GetRequiredIlOffset).ToHashSet();
		if (array6.Length == 0 || array6.Any((int target) => !tailOffsets.Contains(target)) || !tailOffsets.Contains(requiredIntOperand))
		{
			return false;
		}
		if (array2.Any((AotCoreIrInstructionArtifact instruction) => IsUnsupportedStructuredExceptionControlFlow(instruction.Op)) || array3[..^1].Any((AotCoreIrInstructionArtifact instruction) => IsUnsupportedStructuredExceptionControlFlow(instruction.Op) && !string.Equals(instruction.Op, "leave", StringComparison.Ordinal)) || array4[1..^1].Any((AotCoreIrInstructionArtifact instruction) => IsUnsupportedStructuredExceptionControlFlow(instruction.Op)) || array5.Any((AotCoreIrInstructionArtifact instruction) => string.Equals(instruction.Op, "leave", StringComparison.Ordinal)))
		{
			return false;
		}
		catchOnlyShape = new CatchOnlyExceptionMethodShape(exceptionRegion, array2, array3, array4, array5);
		return true;
	}

	private static bool LegacyTryCreateFilterOnlyExceptionMethodShape(AotCoreIrMethodArtifact method, out FilterOnlyExceptionMethodShape? filterOnlyShape)
	{
		filterOnlyShape = null;
		if (method.ExceptionRegions.Count != 1)
		{
			return false;
		}
		AotCoreIrExceptionRegionArtifact filterRegion = method.ExceptionRegions[0];
		if (filterRegion.HandlingKindCode != AotCoreIrExceptionRegionKind.Filter || !filterRegion.FilterOffset.HasValue || !string.IsNullOrEmpty(filterRegion.CatchTypeSubjectId))
		{
			return false;
		}
		AotCoreIrInstructionArtifact[] array = method.Instructions.OrderBy(GetRequiredIlOffset).ToArray();
		if (array.Length == 0)
		{
			return false;
		}
		int filterOffset = filterRegion.FilterOffset.Value;
		AotCoreIrInstructionArtifact[] array2;
		AotCoreIrInstructionArtifact[] array3;
		AotCoreIrInstructionArtifact[] array4;
		AotCoreIrInstructionArtifact[] array5;
		AotCoreIrInstructionArtifact[] array6;
		checked
		{
			int num = filterRegion.TryOffset + filterRegion.TryLength;
			int handlerEndOffset = filterRegion.HandlerOffset + filterRegion.HandlerLength;
			if (num != filterOffset || filterOffset >= filterRegion.HandlerOffset)
			{
				return false;
			}
			array2 = array.Where((AotCoreIrInstructionArtifact instruction) => GetRequiredIlOffset(instruction) < filterRegion.TryOffset).ToArray();
			array3 = array.Where(delegate(AotCoreIrInstructionArtifact instruction)
			{
				int requiredIlOffset = GetRequiredIlOffset(instruction);
				return requiredIlOffset >= filterRegion.TryOffset && requiredIlOffset < filterOffset;
			}).ToArray();
			array4 = array.Where(delegate(AotCoreIrInstructionArtifact instruction)
			{
				int requiredIlOffset = GetRequiredIlOffset(instruction);
				return requiredIlOffset >= filterOffset && requiredIlOffset < filterRegion.HandlerOffset;
			}).ToArray();
			array5 = array.Where(delegate(AotCoreIrInstructionArtifact instruction)
			{
				int requiredIlOffset = GetRequiredIlOffset(instruction);
				return requiredIlOffset >= filterRegion.HandlerOffset && requiredIlOffset < handlerEndOffset;
			}).ToArray();
			array6 = array.Where((AotCoreIrInstructionArtifact instruction) => GetRequiredIlOffset(instruction) >= handlerEndOffset).ToArray();
		}
		if (array.Length != array2.Length + array3.Length + array4.Length + array5.Length + array6.Length || array3.Length == 0 || array4.Length == 0 || array5.Length < 2 || array6.Length == 0)
		{
			return false;
		}
		if ((!string.Equals(array5[0].Op, "pop", StringComparison.Ordinal) && !string.Equals(array5[0].Op, "stloc", StringComparison.Ordinal)) || !string.Equals(array3[^1].Op, "leave", StringComparison.Ordinal) || !string.Equals(array4[^1].Op, "endfilter", StringComparison.Ordinal) || !string.Equals(array5[^1].Op, "leave", StringComparison.Ordinal))
		{
			return false;
		}
		int[] array7 = array3.Where((AotCoreIrInstructionArtifact instruction) => string.Equals(instruction.Op, "leave", StringComparison.Ordinal)).Select(GetRequiredIntOperand).ToArray();
		int requiredIntOperand = GetRequiredIntOperand(array5[^1]);
		HashSet<int> tailOffsets = array6.Select(GetRequiredIlOffset).ToHashSet();
		if (array7.Length == 0 || array7.Any((int target) => !tailOffsets.Contains(target)) || !tailOffsets.Contains(requiredIntOperand))
		{
			return false;
		}
		if (array2.Any((AotCoreIrInstructionArtifact instruction) => IsUnsupportedStructuredExceptionControlFlow(instruction.Op)) || array3[..^1].Any((AotCoreIrInstructionArtifact instruction) => IsUnsupportedStructuredExceptionControlFlow(instruction.Op) && !string.Equals(instruction.Op, "leave", StringComparison.Ordinal)) || array5[1..^1].Any((AotCoreIrInstructionArtifact instruction) => IsUnsupportedStructuredExceptionControlFlow(instruction.Op)) || array6.Any((AotCoreIrInstructionArtifact instruction) => string.Equals(instruction.Op, "leave", StringComparison.Ordinal)))
		{
			return false;
		}
		filterOnlyShape = new FilterOnlyExceptionMethodShape(filterRegion, array2, array3, array4, array5, array6);
		return true;
	}

	private static bool LegacyTryCreateFinallyOnlyExceptionMethodShape(AotCoreIrMethodArtifact method, out FinallyOnlyExceptionMethodShape? finallyOnlyShape)
	{
		finallyOnlyShape = null;
		if (method.ExceptionRegions.Count == 0)
		{
			return false;
		}
		AotCoreIrExceptionRegionArtifact[] array = (from region in method.ExceptionRegions
			where region.HandlingKindCode == AotCoreIrExceptionRegionKind.Finally
			orderby region.TryLength
			select region).ToArray();
		if (array.Length == 0 || method.ExceptionRegions.Any((AotCoreIrExceptionRegionArtifact region) => region.HandlingKindCode != AotCoreIrExceptionRegionKind.Finally))
		{
			return false;
		}
		AotCoreIrInstructionArtifact[] array2 = method.Instructions.OrderBy(GetRequiredIlOffset).ToArray();
		if (array2.Length == 0)
		{
			return false;
		}
		int rootTryOffset = array[0].TryOffset;
		int firstHandlerOffset = array[0].HandlerOffset;
		int nextSegmentOffset = firstHandlerOffset;
		List<FinallyHandlerShape> list = new List<FinallyHandlerShape>();
		AotCoreIrExceptionRegionArtifact[] array3 = array;
		AotCoreIrInstructionArtifact[] array5;
		AotCoreIrInstructionArtifact[] array6;
		AotCoreIrInstructionArtifact[] array7;
		int num2;
		checked
		{
			foreach (AotCoreIrExceptionRegionArtifact finallyRegion in array3)
			{
				if (finallyRegion.TryOffset != rootTryOffset || rootTryOffset + finallyRegion.TryLength != finallyRegion.HandlerOffset || finallyRegion.HandlerOffset != nextSegmentOffset || finallyRegion.FilterOffset.HasValue || !string.IsNullOrEmpty(finallyRegion.CatchTypeSubjectId))
				{
					return false;
				}
				int handlerEndOffset = finallyRegion.HandlerOffset + finallyRegion.HandlerLength;
				AotCoreIrInstructionArtifact[] array4 = array2.Where(delegate(AotCoreIrInstructionArtifact instruction)
				{
					int requiredIlOffset2 = GetRequiredIlOffset(instruction);
					return requiredIlOffset2 >= finallyRegion.HandlerOffset && requiredIlOffset2 < handlerEndOffset;
				}).ToArray();
				if (array4.Length == 0 || !string.Equals(array4[^1].Op, "endfinally", StringComparison.Ordinal))
				{
					return false;
				}
				list.Add(new FinallyHandlerShape(finallyRegion, array4));
				nextSegmentOffset = handlerEndOffset;
			}
			array5 = array2.Where((AotCoreIrInstructionArtifact instruction) => GetRequiredIlOffset(instruction) < rootTryOffset).ToArray();
			array6 = array2.Where(delegate(AotCoreIrInstructionArtifact instruction)
			{
				int requiredIlOffset2 = GetRequiredIlOffset(instruction);
				return requiredIlOffset2 >= rootTryOffset && requiredIlOffset2 < firstHandlerOffset;
			}).ToArray();
			array7 = array2.Where((AotCoreIrInstructionArtifact instruction) => GetRequiredIlOffset(instruction) >= nextSegmentOffset).ToArray();
			num2 = list.Sum((FinallyHandlerShape shape) => shape.Instructions.Count);
		}
		if (array2.Length != array5.Length + array6.Length + num2 + array7.Length || array6.Length == 0 || array7.Length == 0 || !string.Equals(array6[^1].Op, "leave", StringComparison.Ordinal))
		{
			return false;
		}
		int[] array8 = array6.Where((AotCoreIrInstructionArtifact instruction) => string.Equals(instruction.Op, "leave", StringComparison.Ordinal)).Select(GetRequiredIntOperand).ToArray();
		HashSet<int> allOffsets = array2.Select(GetRequiredIlOffset).ToHashSet();
		HashSet<int> collection = array5.Select(GetRequiredIlOffset).ToHashSet();
		int requiredIlOffset = GetRequiredIlOffset(array6[0]);
		HashSet<int> allowedTargets = array6.Skip(1).Select(GetRequiredIlOffset).ToHashSet();
		HashSet<int> tailOffsets = array7.Select(GetRequiredIlOffset).ToHashSet();
		if (array8.Length == 0 || array8.Any((int target) => !tailOffsets.Contains(target)))
		{
			return false;
		}
		HashSet<int> hashSet = new HashSet<int>(collection);
		hashSet.Add(requiredIlOffset);
		foreach (int item in tailOffsets)
		{
			hashSet.Add(item);
		}
		if (array5.Any((AotCoreIrInstructionArtifact instruction) => string.Equals(instruction.Op, "leave", StringComparison.Ordinal)) || !DoBranchTargetsStayWithinAllowedOffsets(array5, allOffsets, hashSet) || array6[..^1].Any((AotCoreIrInstructionArtifact instruction) => IsUnsupportedStructuredExceptionControlFlow(instruction.Op) && !string.Equals(instruction.Op, "leave", StringComparison.Ordinal)) || !DoBranchTargetsStayWithinAllowedOffsets(array6.Where((AotCoreIrInstructionArtifact instruction) => !string.Equals(instruction.Op, "leave", StringComparison.Ordinal)).ToArray(), allOffsets, allowedTargets) || list.Any((FinallyHandlerShape shape) => !TryCreateFinallyHandlerEmissionPlan(shape, out _)) || !DoBranchTargetsStayWithinAllowedOffsets(array7, allOffsets, hashSet) || array7.Any((AotCoreIrInstructionArtifact instruction) => string.Equals(instruction.Op, "leave", StringComparison.Ordinal)))
		{
			return false;
		}
		finallyOnlyShape = new FinallyOnlyExceptionMethodShape(array5, array6, list, array7);
		return true;
	}

	private static bool LegacyTryCreateCatchAndFinallyExceptionMethodShape(AotCoreIrMethodArtifact method, out CatchAndFinallyExceptionMethodShape? catchAndFinallyShape)
	{
		catchAndFinallyShape = null;
		if (method.ExceptionRegions.Count < 2)
		{
			return false;
		}
		AotCoreIrExceptionRegionArtifact[] array = method.ExceptionRegions.Where((AotCoreIrExceptionRegionArtifact region) => region.HandlingKindCode == AotCoreIrExceptionRegionKind.Catch).ToArray();
		AotCoreIrExceptionRegionArtifact[] array2 = (from region in method.ExceptionRegions
			where region.HandlingKindCode == AotCoreIrExceptionRegionKind.Finally
			orderby region.TryLength
			select region).ToArray();
		if (array.Length != 1 || array2.Length == 0 || method.ExceptionRegions.Any((AotCoreIrExceptionRegionArtifact region) => region.HandlingKindCode != AotCoreIrExceptionRegionKind.Catch && region.HandlingKindCode != AotCoreIrExceptionRegionKind.Finally))
		{
			return false;
		}
		AotCoreIrExceptionRegionArtifact catchRegion = array[0];
		if (catchRegion.FilterOffset.HasValue || string.IsNullOrEmpty(catchRegion.CatchTypeSubjectId))
		{
			return false;
		}
		AotCoreIrInstructionArtifact[] orderedInstructions = method.Instructions.OrderBy(GetRequiredIlOffset).ToArray();
		if (orderedInstructions.Length == 0)
		{
			return false;
		}
		int rootTryOffset = catchRegion.TryOffset;
		FinallyHandlerShape? finallyHandlerShape;
		List<FinallyHandlerShape> list;
		AotCoreIrInstructionArtifact[] array9;
		AotCoreIrInstructionArtifact[] array10;
		AotCoreIrInstructionArtifact[] array11;
		AotCoreIrInstructionArtifact[] array12;
		AotCoreIrInstructionArtifact[] array13;
		AotCoreIrInstructionArtifact[] array14;
		checked
		{
			int catchHandlerEnd = catchRegion.HandlerOffset + catchRegion.HandlerLength;
			AotCoreIrExceptionRegionArtifact[] array4 = array2.Where((AotCoreIrExceptionRegionArtifact region) => region.HandlerOffset < catchRegion.HandlerOffset).ToArray();
			AotCoreIrExceptionRegionArtifact[] array5 = array2.Where((AotCoreIrExceptionRegionArtifact region) => region.HandlerOffset > catchRegion.HandlerOffset).ToArray();
			if (array4.Length > 1 || array2.Any((AotCoreIrExceptionRegionArtifact region) => region.HandlerOffset == catchRegion.HandlerOffset))
			{
				return false;
			}
			finallyHandlerShape = null;
			int innerFinallyTryOffset = catchRegion.TryOffset;
			int innerFinallyHandlerEnd = catchRegion.HandlerOffset;
			int innerTryEnd = catchRegion.HandlerOffset;
			if (array4.Length == 1)
			{
				AotCoreIrExceptionRegionArtifact innerFinallyRegion = array4[0];
				if (innerFinallyRegion.TryOffset < rootTryOffset || innerFinallyRegion.TryOffset + innerFinallyRegion.TryLength != innerFinallyRegion.HandlerOffset || innerFinallyRegion.FilterOffset.HasValue || !string.IsNullOrEmpty(innerFinallyRegion.CatchTypeSubjectId))
				{
					return false;
				}
				innerFinallyHandlerEnd = innerFinallyRegion.HandlerOffset + innerFinallyRegion.HandlerLength;
				if (innerFinallyHandlerEnd > catchRegion.HandlerOffset)
				{
					return false;
				}
				AotCoreIrInstructionArtifact[] array6 = orderedInstructions.Where(delegate(AotCoreIrInstructionArtifact instruction)
				{
					int requiredIlOffset = GetRequiredIlOffset(instruction);
					return requiredIlOffset >= innerFinallyRegion.HandlerOffset && requiredIlOffset < innerFinallyHandlerEnd;
				}).ToArray();
				if (array6.Length == 0 || !string.Equals(array6[^1].Op, "endfinally", StringComparison.Ordinal))
				{
					return false;
				}
				innerFinallyTryOffset = innerFinallyRegion.TryOffset;
				innerTryEnd = innerFinallyRegion.HandlerOffset;
				finallyHandlerShape = new FinallyHandlerShape(innerFinallyRegion, array6);
			}
			int nextSegmentOffset = catchHandlerEnd;
			list = new List<FinallyHandlerShape>();
			AotCoreIrExceptionRegionArtifact[] array7 = array5;
			foreach (AotCoreIrExceptionRegionArtifact outerFinallyRegion in array7)
			{
				if (outerFinallyRegion.TryOffset != rootTryOffset || rootTryOffset + outerFinallyRegion.TryLength != outerFinallyRegion.HandlerOffset || outerFinallyRegion.HandlerOffset != nextSegmentOffset || outerFinallyRegion.FilterOffset.HasValue || !string.IsNullOrEmpty(outerFinallyRegion.CatchTypeSubjectId))
				{
					return false;
				}
				int outerHandlerEnd = outerFinallyRegion.HandlerOffset + outerFinallyRegion.HandlerLength;
				AotCoreIrInstructionArtifact[] array8 = orderedInstructions.Where(delegate(AotCoreIrInstructionArtifact instruction)
				{
					int requiredIlOffset = GetRequiredIlOffset(instruction);
					return requiredIlOffset >= outerFinallyRegion.HandlerOffset && requiredIlOffset < outerHandlerEnd;
				}).ToArray();
				if (array8.Length == 0 || !string.Equals(array8[^1].Op, "endfinally", StringComparison.Ordinal))
				{
					return false;
				}
				list.Add(new FinallyHandlerShape(outerFinallyRegion, array8));
				nextSegmentOffset = outerHandlerEnd;
			}
			array9 = orderedInstructions.Where((AotCoreIrInstructionArtifact instruction) => GetRequiredIlOffset(instruction) < rootTryOffset).ToArray();
			array10 = orderedInstructions.Where(delegate(AotCoreIrInstructionArtifact instruction)
			{
				int requiredIlOffset = GetRequiredIlOffset(instruction);
				return requiredIlOffset >= rootTryOffset && requiredIlOffset < innerFinallyTryOffset;
			}).ToArray();
			array11 = orderedInstructions.Where(delegate(AotCoreIrInstructionArtifact instruction)
			{
				int requiredIlOffset = GetRequiredIlOffset(instruction);
				return requiredIlOffset >= innerFinallyTryOffset && requiredIlOffset < innerTryEnd;
			}).ToArray();
			array12 = orderedInstructions.Where(delegate(AotCoreIrInstructionArtifact instruction)
			{
				int requiredIlOffset = GetRequiredIlOffset(instruction);
				return requiredIlOffset >= innerFinallyHandlerEnd && requiredIlOffset < catchRegion.HandlerOffset;
			}).ToArray();
			array13 = orderedInstructions.Where(delegate(AotCoreIrInstructionArtifact instruction)
			{
				int requiredIlOffset = GetRequiredIlOffset(instruction);
				return requiredIlOffset >= catchRegion.HandlerOffset && requiredIlOffset < catchHandlerEnd;
			}).ToArray();
			array14 = orderedInstructions.Where((AotCoreIrInstructionArtifact instruction) => GetRequiredIlOffset(instruction) >= nextSegmentOffset).ToArray();
		}
		if (array9.Length + array10.Length + array11.Length + (finallyHandlerShape?.Instructions.Count ?? 0) + array12.Length + array13.Length + list.Sum((FinallyHandlerShape shape) => shape.Instructions.Count) + array14.Length != orderedInstructions.Length || array11.Length == 0 || array13.Length < 2 || array14.Length == 0)
		{
			return false;
		}
		if ((!string.Equals(array13[0].Op, "pop", StringComparison.Ordinal) && !string.Equals(array13[0].Op, "stloc", StringComparison.Ordinal)) || !string.Equals(array13[^1].Op, "leave", StringComparison.Ordinal))
		{
			return false;
		}
		HashSet<int> tailOffsets = array14.Select(GetRequiredIlOffset).ToHashSet();
		if (!tailOffsets.Contains(GetRequiredIntOperand(array13[^1])))
		{
			return false;
		}
		if (finallyHandlerShape is null)
		{
			int[] array15 = array11.Where((AotCoreIrInstructionArtifact instruction) => string.Equals(instruction.Op, "leave", StringComparison.Ordinal)).Select(GetRequiredIntOperand).ToArray();
			if (array15.Length == 0 || array15.Any((int target) => !tailOffsets.Contains(target)))
			{
				return false;
			}
		}
		else
		{
			int[] array16 = array12.Where((AotCoreIrInstructionArtifact instruction) => string.Equals(instruction.Op, "leave", StringComparison.Ordinal)).Select(GetRequiredIntOperand).ToArray();
			if (array16.Length == 0 || array16.Any((int target) => !tailOffsets.Contains(target)))
			{
				return false;
			}
		}
		if (array9.Any((AotCoreIrInstructionArtifact instruction) => !IsStructuredEhLinearInstructionSupported(instruction.Op)) || array10.Any((AotCoreIrInstructionArtifact instruction) => !IsStructuredEhLinearInstructionSupported(instruction.Op)) || (finallyHandlerShape is not null && !TryCreateFinallyHandlerEmissionPlan(finallyHandlerShape, out _)) || list.Any((FinallyHandlerShape shape) => !TryCreateFinallyHandlerEmissionPlan(shape, out _)) || array13[1..^1].Any((AotCoreIrInstructionArtifact instruction) => IsUnsupportedStructuredExceptionControlFlow(instruction.Op)) || array14.Any((AotCoreIrInstructionArtifact instruction) => string.Equals(instruction.Op, "leave", StringComparison.Ordinal)))
		{
			return false;
		}
		catchAndFinallyShape = new CatchAndFinallyExceptionMethodShape(catchRegion, array9, array10, array11, finallyHandlerShape, array12, array13, list.AsEnumerable().Reverse().ToArray(), array14);
		return true;
	}

	private static bool LegacyTryCreateFilterAndFinallyExceptionMethodShape(AotCoreIrMethodArtifact method, out FilterAndFinallyExceptionMethodShape? filterAndFinallyShape)
	{
		filterAndFinallyShape = null;
		if (method.ExceptionRegions.Count < 2)
		{
			return false;
		}
		AotCoreIrExceptionRegionArtifact[] array = method.ExceptionRegions.Where((AotCoreIrExceptionRegionArtifact region) => region.HandlingKindCode == AotCoreIrExceptionRegionKind.Filter).ToArray();
		AotCoreIrExceptionRegionArtifact[] array2 = (from region in method.ExceptionRegions
			where region.HandlingKindCode == AotCoreIrExceptionRegionKind.Finally
			orderby region.TryLength
			select region).ToArray();
		if (array.Length != 1 || array2.Length == 0 || method.ExceptionRegions.Any((AotCoreIrExceptionRegionArtifact region) => region.HandlingKindCode != AotCoreIrExceptionRegionKind.Filter && region.HandlingKindCode != AotCoreIrExceptionRegionKind.Finally))
		{
			return false;
		}
		AotCoreIrExceptionRegionArtifact filterRegion = array[0];
		if (!filterRegion.FilterOffset.HasValue)
		{
			return false;
		}
		AotCoreIrInstructionArtifact[] orderedInstructions = method.Instructions.OrderBy(GetRequiredIlOffset).ToArray();
		if (orderedInstructions.Length == 0)
		{
			return false;
		}
		int rootTryOffset = filterRegion.TryOffset;
		int filterOffset = filterRegion.FilterOffset.Value;
		AotCoreIrInstructionArtifact[] array4;
		AotCoreIrInstructionArtifact[] array5;
		AotCoreIrInstructionArtifact[] array6;
		AotCoreIrInstructionArtifact[] array7;
		FinallyHandlerShape[] source;
		AotCoreIrInstructionArtifact[] array8;
		checked
		{
			int filterHandlerEnd = filterRegion.HandlerOffset + filterRegion.HandlerLength;
			if (filterRegion.TryOffset + filterRegion.TryLength != filterOffset)
			{
				return false;
			}
			int nextSegmentOffset = filterHandlerEnd;
			AotCoreIrExceptionRegionArtifact[] array3 = array2;
			foreach (AotCoreIrExceptionRegionArtifact aotCoreIrExceptionRegionArtifact in array3)
			{
				if (aotCoreIrExceptionRegionArtifact.TryOffset != rootTryOffset || rootTryOffset + aotCoreIrExceptionRegionArtifact.TryLength != aotCoreIrExceptionRegionArtifact.HandlerOffset || aotCoreIrExceptionRegionArtifact.HandlerOffset != nextSegmentOffset || aotCoreIrExceptionRegionArtifact.FilterOffset.HasValue || !string.IsNullOrEmpty(aotCoreIrExceptionRegionArtifact.CatchTypeSubjectId))
				{
					return false;
				}
				nextSegmentOffset = aotCoreIrExceptionRegionArtifact.HandlerOffset + aotCoreIrExceptionRegionArtifact.HandlerLength;
			}
			array4 = orderedInstructions.Where((AotCoreIrInstructionArtifact instruction) => GetRequiredIlOffset(instruction) < rootTryOffset).ToArray();
			array5 = orderedInstructions.Where(delegate(AotCoreIrInstructionArtifact instruction)
			{
				int requiredIlOffset = GetRequiredIlOffset(instruction);
				return requiredIlOffset >= rootTryOffset && requiredIlOffset < filterOffset;
			}).ToArray();
			array6 = orderedInstructions.Where(delegate(AotCoreIrInstructionArtifact instruction)
			{
				int requiredIlOffset = GetRequiredIlOffset(instruction);
				return requiredIlOffset >= filterOffset && requiredIlOffset < filterRegion.HandlerOffset;
			}).ToArray();
			array7 = orderedInstructions.Where(delegate(AotCoreIrInstructionArtifact instruction)
			{
				int requiredIlOffset = GetRequiredIlOffset(instruction);
				return requiredIlOffset >= filterRegion.HandlerOffset && requiredIlOffset < filterHandlerEnd;
			}).ToArray();
			source = array2.Select(delegate(AotCoreIrExceptionRegionArtifact finallyRegion)
			{
				int handlerEnd = finallyRegion.HandlerOffset + finallyRegion.HandlerLength;
				AotCoreIrInstructionArtifact[] instructions = orderedInstructions.Where(delegate(AotCoreIrInstructionArtifact instruction)
				{
					int requiredIlOffset = GetRequiredIlOffset(instruction);
					return requiredIlOffset >= finallyRegion.HandlerOffset && requiredIlOffset < handlerEnd;
				}).ToArray();
				return new FinallyHandlerShape(finallyRegion, instructions);
			}).ToArray();
			array8 = orderedInstructions.Where((AotCoreIrInstructionArtifact instruction) => GetRequiredIlOffset(instruction) >= nextSegmentOffset).ToArray();
		}
		if (array4.Length + array5.Length + array6.Length + array7.Length + source.Sum((FinallyHandlerShape shape) => shape.Instructions.Count) + array8.Length != orderedInstructions.Length || array5.Length == 0 || array6.Length == 0 || array7.Length < 2 || array8.Length == 0)
		{
			return false;
		}
		if (!string.Equals(array6[^1].Op, "endfilter", StringComparison.Ordinal) || !string.Equals(array7[^1].Op, "leave", StringComparison.Ordinal) || source.Any(delegate(FinallyHandlerShape shape)
		{
			if (shape.Instructions.Count != 0)
			{
				IReadOnlyList<AotCoreIrInstructionArtifact> instructions = shape.Instructions;
				return !string.Equals(instructions[instructions.Count - 1].Op, "endfinally", StringComparison.Ordinal);
			}
			return true;
		}))
		{
			return false;
		}
		if (GetRequiredIntOperand(array7[^1]) != GetRequiredIlOffset(array8[0]))
		{
			return false;
		}
		if (array4.Any((AotCoreIrInstructionArtifact instruction) => !IsStructuredEhLinearInstructionSupported(instruction.Op)) || source.Any((FinallyHandlerShape shape) => !TryCreateFinallyHandlerEmissionPlan(shape, out _)))
		{
			return false;
		}
		filterAndFinallyShape = new FilterAndFinallyExceptionMethodShape(filterRegion, array4, array5, array6, array7, source.Reverse().ToArray(), array8);
		return true;
	}

	private static bool LegacyTryCreateFinallyHandlerEmissionPlan(FinallyHandlerShape handlerShape, out FinallyHandlerEmissionPlan? emissionPlan)
	{
		emissionPlan = null;
		IReadOnlyList<AotCoreIrInstructionArtifact> instructions = handlerShape.Instructions;
		if (instructions.Count != 0)
		{
			if (string.Equals(instructions[instructions.Count - 1].Op, "endfinally", StringComparison.Ordinal))
			{
				AotCoreIrInstructionArtifact[] array = instructions.Take(instructions.Count - 1).ToArray();
				if (array.Length == 0)
				{
					emissionPlan = new FinallyHandlerEmissionPlan(null, Array.Empty<AotCoreIrInstructionArtifact>());
					return true;
				}
				(AotCoreIrInstructionArtifact, int)[] array2 = (from entry in array.Select((AotCoreIrInstructionArtifact instruction, int index) => (instruction: instruction, index: index))
					where string.Equals(entry.instruction.Op, "brtrue", StringComparison.Ordinal) || string.Equals(entry.instruction.Op, "brfalse", StringComparison.Ordinal)
					select entry).ToArray();
				if (array2.Length == 0)
				{
					if (array.Any((AotCoreIrInstructionArtifact instruction) => !IsStructuredEhLinearInstructionSupported(instruction.Op)))
					{
						return false;
					}
					emissionPlan = new FinallyHandlerEmissionPlan(null, array);
					return true;
				}
				if (array2.Length != 1)
				{
					return false;
				}
				(AotCoreIrInstructionArtifact, int) tuple = array2[0];
				AotCoreIrInstructionArtifact item = tuple.Item1;
				int item2 = tuple.Item2;
				if (GetRequiredIntOperand(item) != GetRequiredIlOffset(instructions[instructions.Count - 1]))
				{
					return false;
				}
				AotCoreIrInstructionArtifact[] array3 = array.Take(item2).ToArray();
				AotCoreIrInstructionArtifact[] array4 = array.Skip(item2 + 1).ToArray();
				if (array3.Length == 0 || array3.Any((AotCoreIrInstructionArtifact instruction) => !IsStructuredEhLinearInstructionSupported(instruction.Op)) || array4.Any((AotCoreIrInstructionArtifact instruction) => !IsStructuredEhLinearInstructionSupported(instruction.Op)))
				{
					return false;
				}
				emissionPlan = new FinallyHandlerEmissionPlan(new FinallyHandlerGuardShape(array3, string.Equals(item.Op, "brtrue", StringComparison.Ordinal)), array4);
				return true;
			}
		}
		return false;
	}

	private static bool DoBranchTargetsStayWithinAllowedOffsets(IReadOnlyList<AotCoreIrInstructionArtifact> instructions, IReadOnlySet<int> allOffsets, IReadOnlySet<int> allowedTargets)
	{
		foreach (AotCoreIrInstructionArtifact instruction in instructions)
		{
			foreach (int item in EnumerateInstructionBranchTargets(instruction, allOffsets))
			{
				if (!allowedTargets.Contains(item))
				{
					return false;
				}
			}
		}
		return true;
	}

	private static IEnumerable<int> EnumerateInstructionBranchTargets(AotCoreIrInstructionArtifact instruction, IReadOnlySet<int> offsets)
	{
		string op = instruction.Op;
		if (op == null)
		{
			yield break;
		}
		switch (op.Length)
		{
		case 6:
			switch (op[1])
			{
			default:
				yield break;
			case 'r':
				if (!(op == "brtrue"))
				{
					yield break;
				}
				break;
			case 'g':
				if (!(op == "bge.un"))
				{
					yield break;
				}
				break;
			case 'n':
				if (!(op == "bne.un"))
				{
					yield break;
				}
				break;
			case 'w':
				if (!(op == "switch"))
				{
					yield break;
				}
				foreach (int requiredSwitchTarget in GetRequiredSwitchTargets(instruction, offsets))
				{
					yield return requiredSwitchTarget;
				}
				yield break;
			}
			goto IL_01a0;
		case 3:
			switch (op[1])
			{
			default:
				yield break;
			case 'e':
				if (!(op == "beq"))
				{
					yield break;
				}
				break;
			case 'l':
				if (!(op == "blt") && !(op == "ble"))
				{
					yield break;
				}
				break;
			case 'g':
				if (!(op == "bgt") && !(op == "bge"))
				{
					yield break;
				}
				break;
			}
			goto IL_01a0;
		case 2:
			if (!(op == "br"))
			{
				break;
			}
			goto IL_01a0;
		case 7:
			if (!(op == "brfalse"))
			{
				break;
			}
			goto IL_01a0;
		case 5:
			{
				if (!(op == "leave"))
				{
					break;
				}
				goto IL_01a0;
			}
			IL_01a0:
			yield return GetRequiredBranchTarget(instruction, offsets);
			break;
		}
	}

	private static bool IsUnsupportedStructuredExceptionControlFlow(string op)
	{
		if (!string.Equals(op, "br", StringComparison.Ordinal) && !string.Equals(op, "blt", StringComparison.Ordinal) && !string.Equals(op, "bne.un", StringComparison.Ordinal) && !string.Equals(op, "leave", StringComparison.Ordinal))
		{
			return string.Equals(op, "ret", StringComparison.Ordinal);
		}
		return true;
	}

	private static bool IsStructuredEhLinearInstructionSupported(string op)
	{
		switch (op)
		{
		case "ldc.i4":
		case "ldc.i8":
		case "ldsfld":
		case "ldnull":
		case "cgt.un":
		case "shr.un":
		case "ldloc":
		case "stloc":
		case "ldstr":
		case "ldarg":
		case "ldfld":
		case "ceq":
		case "cgt":
		case "add":
		case "and":
		case "sub":
		case "shl":
		case "shr":
		case "mul":
		case "div":
		case "rem":
		case "not":
		case "xor":
		case "ldtoken":
		case "call":
		case "callvirt":
		case "newobj":
		case "newarr":
		case "ldelema":
		case "ldobj":
		case "stobj":
		case "cpblk":
		case "localloc":
		case "ldind.i1":
		case "ldind.u1":
		case "ldind.i2":
		case "ldind.u2":
		case "ldind.i4":
		case "ldind.u4":
		case "ldind.i8":
		case "ldind.r4":
		case "ldind.r8":
		case "ldind.ref":
		case "stind.i1":
		case "stind.i2":
		case "stind.i4":
		case "stind.i8":
		case "stind.r4":
		case "stind.r8":
		case "stind.ref":
		case "ldflda":
		case "ldsflda":
		case "cpobj":
		case "ldelem":
		case "ldelem.i1":
		case "ldelem.u1":
		case "ldelem.i2":
		case "ldelem.u2":
		case "ldelem.i4":
		case "ldelem.u4":
		case "ldelem.i8":
		case "ldelem.r4":
		case "ldelem.r8":
		case "ldelem.ref":
		case "stelem":
		case "stelem.i1":
		case "stelem.i2":
		case "stelem.i4":
		case "stelem.i8":
		case "stelem.r4":
		case "stelem.r8":
		case "stelem.ref":
		case "box":
		case "unbox":
		case "unbox.any":
		case "castclass":
		case "isinst":
		case "initobj":
		case "or":
		case "stsfld":
		case "stfld":
		case "clt":
		case "div.un":
		case "rem.un":
		case "neg":
		case "dup":
		case "ldc.r4":
		case "ldc.r8":
		case "sizeof":
		case "ldlen":
		case "ldloca":
		case "ldarga":
		case "starg":
		case "conv.i4":
		case "conv.i8":
		case "conv.r4":
		case "conv.r8":
		case "conv.i":
		case "conv.u":
		case "conv.u4":
		case "conv.i1":
		case "conv.i2":
		case "conv.u1":
		case "conv.u2":
		case "conv.u8":
		case "add.ovf":
		case "sub.ovf":
		case "mul.ovf":
		case "clt.un":
		case "bne.un":
		case "blt.un":
		case "bgt.un":
		case "ble.un":
		case "bge.un":
		case "beq":
		case "brtrue":
		case "brfalse":
		case "ckfinite":
		case "initblk":
		case "conv.ovf.i2":
		case "conv.ovf.i4":
		case "conv.ovf.i8":
		case "conv.ovf.i8.un":
		case "conv.ovf.u2":
		case "conv.ovf.u4":
		case "conv.ovf.u8":
		case "conv.ovf.i":
		case "conv.ovf.i.un":
		case "conv.ovf.u":
		case "conv.ovf.u.un":
		case "conv.ovf.i1.un":
		case "conv.ovf.i2.un":
		case "conv.ovf.i4.un":
		case "conv.ovf.u1.un":
		case "conv.ovf.u2.un":
		case "conv.ovf.u4.un":
		case "add.ovf.un":
		case "sub.ovf.un":
		case "mul.ovf.un":
		case "ldind.i":
		case "stind.i":
		case "conv.r.un":
		case "switch":
		case "arglist":
		case "mkrefany":
		case "refanyval":
		case "refanytype":
		case "ldvirtftn":
		case "calli":
		case "jmp":
			return true;
		default:
			return false;
		}
	}

	private static int GetRequiredBranchTarget(AotCoreIrInstructionArtifact instruction, IReadOnlySet<int> offsets)
	{
		int requiredIntOperand = GetRequiredIntOperand(instruction);
		if (!offsets.Contains(requiredIntOperand))
		{
			throw new InvalidOperationException($"opcode '{instruction.Op}' targets missing IL offset {requiredIntOperand}");
		}
		return requiredIntOperand;
	}

	private static IReadOnlyList<int> GetRequiredSwitchTargets(AotCoreIrInstructionArtifact instruction, IReadOnlySet<int> offsets)
	{
		object? operand = instruction.Operand;
		IReadOnlyList<int> readOnlyList2;
		if (!(operand is int[] array))
		{
			if (!(operand is IReadOnlyList<int> readOnlyList))
			{
				if (!(operand is JsonElement { ValueKind: JsonValueKind.Array } jsonElement))
				{
					throw new InvalidOperationException("opcode '" + instruction.Op + "' requires an Int32[] operand for native-aot lowering");
				}
				readOnlyList2 = jsonElement.EnumerateArray().Select(delegate(JsonElement targetElement)
				{
					if (targetElement.ValueKind != JsonValueKind.Number || !targetElement.TryGetInt32(out var value))
					{
						throw new InvalidOperationException("opcode '" + instruction.Op + "' requires an Int32[] operand for native-aot lowering");
					}
					return value;
				}).ToArray();
			}
			else
			{
				readOnlyList2 = readOnlyList;
			}
		}
		else
		{
			readOnlyList2 = array;
		}
		IReadOnlyList<int> readOnlyList3 = readOnlyList2;
		foreach (int item in readOnlyList3)
		{
			if (!offsets.Contains(item))
			{
				throw new InvalidOperationException($"opcode '{instruction.Op}' targets missing IL offset {item}");
			}
		}
		return readOnlyList3;
	}

	private static int GetRequiredIlOffset(AotCoreIrInstructionArtifact instruction)
	{
		return instruction.IlOffset;
	}

	private static int GetRequiredIntOperand(AotCoreIrInstructionArtifact instruction)
	{
		object? operand = instruction.Operand;
		if (operand is int)
		{
			return (int)operand;
		}
		if (instruction.Operand is JsonElement { ValueKind: JsonValueKind.Number } jsonElement && jsonElement.TryGetInt32(out var value))
		{
			return value;
		}
		throw new InvalidOperationException("opcode '" + instruction.Op + "' requires an Int32 operand for native-aot lowering");
	}

	private static void RequireInt32IntegralResultType(AotCoreIrInstructionArtifact instruction)
	{
		if (string.Equals(instruction.ResultType, "System.Int32", StringComparison.Ordinal) || string.Equals(instruction.ResultType, "System.UInt32", StringComparison.Ordinal))
		{
			return;
		}
		throw new NotSupportedException($"native-aot lowering does not support opcode '{instruction.Op}' for result type '{instruction.ResultType ?? "<null>"}'.");
	}

	private static long GetRequiredInt64Operand(AotCoreIrInstructionArtifact instruction)
	{
		object? operand = instruction.Operand;
		if (operand is long)
		{
			return (long)operand;
		}
		if (instruction.Operand is int num)
		{
			return num;
		}
		if (instruction.Operand is JsonElement { ValueKind: JsonValueKind.Number } jsonElement && jsonElement.TryGetInt64(out var value))
		{
			return value;
		}
		throw new InvalidOperationException("opcode '" + instruction.Op + "' requires an Int64 operand for native-aot lowering");
	}

	private static float GetRequiredSingleOperand(AotCoreIrInstructionArtifact instruction)
	{
		object? operand = instruction.Operand;
		if (operand is float)
		{
			return (float)operand;
		}
		if (instruction.Operand is double num)
		{
			return (float)num;
		}
		if (instruction.Operand is JsonElement { ValueKind: JsonValueKind.Number } jsonElement && jsonElement.TryGetSingle(out var value))
		{
			return value;
		}
		throw new InvalidOperationException("opcode '" + instruction.Op + "' requires a Single operand for native-aot lowering");
	}

	private static double GetRequiredDoubleOperand(AotCoreIrInstructionArtifact instruction)
	{
		object? operand = instruction.Operand;
		if (operand is double)
		{
			return (double)operand;
		}
		if (instruction.Operand is float num)
		{
			return num;
		}
		if (instruction.Operand is JsonElement { ValueKind: JsonValueKind.Number } jsonElement && jsonElement.TryGetDouble(out var value))
		{
			return value;
		}
		throw new InvalidOperationException("opcode '" + instruction.Op + "' requires a Double operand for native-aot lowering");
	}

		private void EmitFlatGotoBody(
			StringBuilder builder,
			AotCoreIrMethodArtifact method,
			IReadOnlyList<AotCoreIrInstructionArtifact> instructions,
			IReadOnlySet<int> branchTargetOffsets)
		{
			foreach (var instruction in instructions)
			{
				int offset = GetRequiredIlOffset(instruction);
				if (branchTargetOffsets.Contains(offset))
				{
					builder.AppendLine($"chaos_label_{offset}:");
				}

				switch (instruction.Op)
				{
				case "br":
				case "leave":
				{
					int target = GetRequiredIntOperand(instruction);
					builder.AppendLine($"    goto chaos_label_{target};");
					break;
				}
				case "brfalse":
				{
					builder.AppendLine($"    if ({ConsumeEvalStackValueExpression()} == 0)");
					builder.AppendLine($"        goto chaos_label_{GetRequiredIntOperand(instruction)};");
					break;
				}
				case "brtrue":
				{
					builder.AppendLine($"    if ({ConsumeEvalStackValueExpression()} != 0)");
					builder.AppendLine($"        goto chaos_label_{GetRequiredIntOperand(instruction)};");
					break;
				}
				case "beq":
				{
					builder.AppendLine($"    {{");
					builder.AppendLine($"        const auto chaos_right = {ConsumeEvalStackValueExpression()};");
					builder.AppendLine($"        const auto chaos_left = {ConsumeEvalStackValueExpression()};");
					builder.AppendLine($"        if (chaos_left == chaos_right)");
					builder.AppendLine($"            goto chaos_label_{GetRequiredIntOperand(instruction)};");
					builder.AppendLine($"    }}");
					break;
				}
				case "bne.un":
				{
					builder.AppendLine($"    {{");
					builder.AppendLine($"        const auto chaos_right = {ConsumeEvalStackValueExpression()};");
					builder.AppendLine($"        const auto chaos_left = {ConsumeEvalStackValueExpression()};");
					builder.AppendLine($"        if (chaos_left != chaos_right)");
					builder.AppendLine($"            goto chaos_label_{GetRequiredIntOperand(instruction)};");
					builder.AppendLine($"    }}");
					break;
				}
				case "bge":
				{
					builder.AppendLine($"    {{");
					builder.AppendLine($"        const auto chaos_right = static_cast<CHAOS_IL2CPP_INT32>({ConsumeEvalStackValueExpression()});");
					builder.AppendLine($"        const auto chaos_left = static_cast<CHAOS_IL2CPP_INT32>({ConsumeEvalStackValueExpression()});");
					builder.AppendLine($"        if (chaos_left >= chaos_right)");
					builder.AppendLine($"            goto chaos_label_{GetRequiredIntOperand(instruction)};");
					builder.AppendLine($"    }}");
					break;
				}
				case "bge.un":
				{
					builder.AppendLine($"    {{");
					builder.AppendLine($"        const auto chaos_right = static_cast<CHAOS_IL2CPP_UINT32>({ConsumeEvalStackValueExpression()});");
					builder.AppendLine($"        const auto chaos_left = static_cast<CHAOS_IL2CPP_UINT32>({ConsumeEvalStackValueExpression()});");
					builder.AppendLine($"        if (chaos_left >= chaos_right)");
					builder.AppendLine($"            goto chaos_label_{GetRequiredIntOperand(instruction)};");
					builder.AppendLine($"    }}");
					break;
				}
				case "bgt":
				{
					builder.AppendLine($"    {{");
					builder.AppendLine($"        const auto chaos_right = static_cast<CHAOS_IL2CPP_INT32>({ConsumeEvalStackValueExpression()});");
					builder.AppendLine($"        const auto chaos_left = static_cast<CHAOS_IL2CPP_INT32>({ConsumeEvalStackValueExpression()});");
					builder.AppendLine($"        if (chaos_left > chaos_right)");
					builder.AppendLine($"            goto chaos_label_{GetRequiredIntOperand(instruction)};");
					builder.AppendLine($"    }}");
					break;
				}
				case "bgt.un":
				{
					builder.AppendLine($"    {{");
					builder.AppendLine($"        const auto chaos_right = static_cast<CHAOS_IL2CPP_UINT32>({ConsumeEvalStackValueExpression()});");
					builder.AppendLine($"        const auto chaos_left = static_cast<CHAOS_IL2CPP_UINT32>({ConsumeEvalStackValueExpression()});");
					builder.AppendLine($"        if (chaos_left > chaos_right)");
					builder.AppendLine($"            goto chaos_label_{GetRequiredIntOperand(instruction)};");
					builder.AppendLine($"    }}");
					break;
				}
				case "ble":
				{
					builder.AppendLine($"    {{");
					builder.AppendLine($"        const auto chaos_right = static_cast<CHAOS_IL2CPP_INT32>({ConsumeEvalStackValueExpression()});");
					builder.AppendLine($"        const auto chaos_left = static_cast<CHAOS_IL2CPP_INT32>({ConsumeEvalStackValueExpression()});");
					builder.AppendLine($"        if (chaos_left <= chaos_right)");
					builder.AppendLine($"            goto chaos_label_{GetRequiredIntOperand(instruction)};");
					builder.AppendLine($"    }}");
					break;
				}
				case "ble.un":
				{
					builder.AppendLine($"    {{");
					builder.AppendLine($"        const auto chaos_right = static_cast<CHAOS_IL2CPP_UINT32>({ConsumeEvalStackValueExpression()});");
					builder.AppendLine($"        const auto chaos_left = static_cast<CHAOS_IL2CPP_UINT32>({ConsumeEvalStackValueExpression()});");
					builder.AppendLine($"        if (chaos_left <= chaos_right)");
					builder.AppendLine($"            goto chaos_label_{GetRequiredIntOperand(instruction)};");
					builder.AppendLine($"    }}");
					break;
				}
				case "blt":
				{
					builder.AppendLine($"    {{");
					builder.AppendLine($"        const auto chaos_right = static_cast<CHAOS_IL2CPP_INT32>({ConsumeEvalStackValueExpression()});");
					builder.AppendLine($"        const auto chaos_left = static_cast<CHAOS_IL2CPP_INT32>({ConsumeEvalStackValueExpression()});");
					builder.AppendLine($"        if (chaos_left < chaos_right)");
					builder.AppendLine($"            goto chaos_label_{GetRequiredIntOperand(instruction)};");
					builder.AppendLine($"    }}");
					break;
				}
				case "blt.un":
				{
					builder.AppendLine($"    {{");
					builder.AppendLine($"        const auto chaos_right = static_cast<CHAOS_IL2CPP_UINT32>({ConsumeEvalStackValueExpression()});");
					builder.AppendLine($"        const auto chaos_left = static_cast<CHAOS_IL2CPP_UINT32>({ConsumeEvalStackValueExpression()});");
					builder.AppendLine($"        if (chaos_left < chaos_right)");
					builder.AppendLine($"            goto chaos_label_{GetRequiredIntOperand(instruction)};");
					builder.AppendLine($"    }}");
					break;
				}
				case "switch":
				{
					var targets = GetRequiredSwitchTargets(instruction, branchTargetOffsets);
					builder.AppendLine($"    {{");
					builder.AppendLine($"        const auto chaos_switch_val = static_cast<CHAOS_IL2CPP_UINT32>({ConsumeEvalStackValueExpression()});");
					builder.AppendLine($"        switch (chaos_switch_val)");
					builder.AppendLine($"        {{");
					for (int i = 0; i < targets.Count; i++)
					{
						builder.AppendLine($"        case {i}: goto chaos_label_{targets[i]};");
					}
					builder.AppendLine($"        default: break;");
					builder.AppendLine($"        }}");
					builder.AppendLine($"    }}");
					break;
				}
				case "ret":
				{
					EmitStructuredMethodReturn(builder, method.ReturnAbi, "    ");
					break;
				}
				case "throw":
				{
					builder.AppendLine($"    throw chaos_managed_exception{{{ConsumeEvalStackValueExpression()}}};");
					break;
				}
				case "rethrow":
				{
					builder.AppendLine($"    throw;");
					break;
				}
				case "endfilter":
				{
					builder.AppendLine($"    if ({ConsumeEvalStackValueExpression()} == 0)");
					builder.AppendLine($"        throw;");
					break;
				}
				case "endfinally":
				{
					break;
				}
				default:
				{
					EmitInstruction(builder, instruction, "    ");
					break;
				}
				}
			}
		}

}
