using System;
using System.Collections;
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
	private bool IsTrackedValueTypeSubjectId(string subjectId)
	{
		if (!string.IsNullOrWhiteSpace(subjectId))
		{
			return _valueTypeSubjectIds.Contains(subjectId);
		}
		return false;
	}

	private string GetVirtualDispatchTargetTypeIdSymbol(string subjectId)
	{
		if (!IsTrackedValueTypeSubjectId(subjectId))
		{
			return GetNativeTypeIdSymbol(subjectId);
		}
		return GetNativeBoxTypeIdSymbol(subjectId);
	}

	private string GetVirtualDispatchInstanceExpression(string subjectId, string instanceExpression)
	{
		if (!IsTrackedValueTypeSubjectId(subjectId))
		{
			return instanceExpression;
		}
		string nativeBoxTypeSymbol = GetNativeBoxTypeSymbol(subjectId);
		if (!IsStructuredValueTypeSubjectId(subjectId))
		{
			return $"reinterpret_cast<{nativeBoxTypeSymbol}*>({instanceExpression})->value";
		}
		return $"reinterpret_cast<std::intptr_t>(&reinterpret_cast<{nativeBoxTypeSymbol}*>({instanceExpression})->value)";
	}

	private string GetRequiredFunctionPointerTargetSymbol(AotCoreIrInstructionArtifact instruction)
	{
		if (!string.IsNullOrWhiteSpace(instruction.TargetSymbol))
		{
			return instruction.TargetSymbol;
		}
		if (!string.IsNullOrWhiteSpace(instruction.Callee) && _methodsBySubjectId.TryGetValue(instruction.Callee, out AotCoreIrMethodArtifact value))
		{
			return value.NativeSymbol;
		}
		throw new NotSupportedException("native-aot lowering does not support unresolved function pointer target '" + (instruction.Callee ?? "<null>") + "'.");
	}

	private static string GetRequiredTargetSymbol(AotCoreIrInstructionArtifact instruction)
	{
		if (!string.IsNullOrWhiteSpace(instruction.TargetSymbol))
		{
			return instruction.TargetSymbol;
		}
		throw new NotSupportedException("native-aot lowering does not support unresolved call target '" + (instruction.Callee ?? "<null>") + "'");
	}

	private static int GetRequiredTargetParameterCount(AotCoreIrInstructionArtifact instruction)
	{
		int? targetParameterCount = instruction.TargetParameterCount;
		if (targetParameterCount.HasValue)
		{
			int valueOrDefault = targetParameterCount.GetValueOrDefault();
			if (valueOrDefault >= 0)
			{
				return valueOrDefault;
			}
		}
		throw new NotSupportedException("native-aot lowering does not support call target '" + (instruction.TargetSymbol ?? instruction.Callee ?? "<null>") + "' without parameter metadata");
	}

	private static AotCoreIrReferenceArtifact GetRequiredTargetReference(AotCoreIrInstructionArtifact instruction)
	{
		if ((object)instruction.TargetReference != null)
		{
			return instruction.TargetReference;
		}
		throw new NotSupportedException("native-aot lowering does not support opcode '" + instruction.Op + "' without target reference metadata");
	}

	private static string GetRequiredTypeHandleLiteral(AotCoreIrInstructionArtifact instruction)
	{
		AotCoreIrReferenceArtifact requiredTargetReference = GetRequiredTargetReference(instruction);
		return requiredTargetReference.Kind switch
		{
			AotCoreIrReferenceKind.Type => GetTypeHandleLiteral(requiredTargetReference.SubjectId), 
			AotCoreIrReferenceKind.Field => GetFieldHandleLiteral(requiredTargetReference.SubjectId), 
			AotCoreIrReferenceKind.Method => GetMethodHandleLiteral(requiredTargetReference.SubjectId), 
			_ => throw new NotSupportedException($"native-aot lowering does not support ldtoken target kind '{requiredTargetReference.Kind}'."), 
		};
	}

	private static void EmitMethodReturn(StringBuilder builder, AotCoreIrAbiSlotArtifact returnAbi)
	{
		switch (returnAbi.CarrierKindCode)
		{
		case AotCoreIrAbiCarrierKind.Void:
			builder.AppendLine("    return;");
			break;
		case AotCoreIrAbiCarrierKind.Int32:
			builder.AppendLine("    return static_cast<std::int32_t>(chaos_eval_stack[--chaos_stack_top]);");
			break;
		case AotCoreIrAbiCarrierKind.Int8:
			builder.AppendLine("    return static_cast<std::int8_t>(chaos_eval_stack[--chaos_stack_top]);");
			break;
		case AotCoreIrAbiCarrierKind.UInt8:
			builder.AppendLine("    return static_cast<std::uint8_t>(chaos_eval_stack[--chaos_stack_top]);");
			break;
		case AotCoreIrAbiCarrierKind.Int16:
			builder.AppendLine("    return static_cast<std::int16_t>(chaos_eval_stack[--chaos_stack_top]);");
			break;
		case AotCoreIrAbiCarrierKind.UInt16:
			builder.AppendLine("    return static_cast<std::uint16_t>(chaos_eval_stack[--chaos_stack_top]);");
			break;
		case AotCoreIrAbiCarrierKind.Float32:
			builder.AppendLine("    return chaos_load_float32(chaos_eval_stack[--chaos_stack_top]);");
			break;
		case AotCoreIrAbiCarrierKind.Float64:
			builder.AppendLine("    return chaos_load_float64(chaos_eval_stack[--chaos_stack_top]);");
			break;
		case AotCoreIrAbiCarrierKind.Int64:
			builder.AppendLine("    return chaos_load_int64(chaos_eval_stack[--chaos_stack_top]);");
			break;
		case AotCoreIrAbiCarrierKind.UInt64:
			builder.AppendLine("    return chaos_load_uint64(chaos_eval_stack[--chaos_stack_top]);");
			break;
		case AotCoreIrAbiCarrierKind.NativeInt:
			builder.AppendLine("    return chaos_eval_stack[--chaos_stack_top];");
			break;
		case AotCoreIrAbiCarrierKind.ValueTypeByValue:
		{
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(87, 1, builder);
			handler.AppendLiteral("    return *chaos_resolve_managed_value_pointer<");
			handler.AppendFormatted(GetRequiredAbiValueTypeSymbol(returnAbi));
			handler.AppendLiteral(">(chaos_eval_stack[--chaos_stack_top]);");
			builder.AppendLine(ref handler);
			break;
		}
		default:
			throw new NotSupportedException($"native-aot lowering does not support ABI return carrier '{returnAbi.CarrierKindCode}'.");
		}
	}

	private static void EmitAbiReturnPush(StringBuilder builder, AotCoreIrAbiSlotArtifact returnAbi, string resultExpression, string indentation)
	{
		switch (returnAbi.CarrierKindCode)
		{
		case AotCoreIrAbiCarrierKind.Int32:
		case AotCoreIrAbiCarrierKind.NativeInt:
		case AotCoreIrAbiCarrierKind.Int8:
		case AotCoreIrAbiCarrierKind.UInt8:
		case AotCoreIrAbiCarrierKind.Int16:
		case AotCoreIrAbiCarrierKind.UInt16:
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder9 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(67, 2, stringBuilder);
			handler.AppendFormatted(indentation);
			handler.AppendLiteral("chaos_eval_stack[chaos_stack_top++] = static_cast<std::intptr_t>(");
			handler.AppendFormatted(resultExpression);
			handler.AppendLiteral(");");
			stringBuilder9.AppendLine(ref handler);
			break;
		}
		case AotCoreIrAbiCarrierKind.Float32:
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder8 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(60, 2, stringBuilder);
			handler.AppendFormatted(indentation);
			handler.AppendLiteral("chaos_eval_stack[chaos_stack_top++] = chaos_store_float32(");
			handler.AppendFormatted(resultExpression);
			handler.AppendLiteral(");");
			stringBuilder8.AppendLine(ref handler);
			break;
		}
		case AotCoreIrAbiCarrierKind.Float64:
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder7 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(60, 2, stringBuilder);
			handler.AppendFormatted(indentation);
			handler.AppendLiteral("chaos_eval_stack[chaos_stack_top++] = chaos_store_float64(");
			handler.AppendFormatted(resultExpression);
			handler.AppendLiteral(");");
			stringBuilder7.AppendLine(ref handler);
			break;
		}
		case AotCoreIrAbiCarrierKind.Int64:
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder6 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(58, 2, stringBuilder);
			handler.AppendFormatted(indentation);
			handler.AppendLiteral("chaos_eval_stack[chaos_stack_top++] = chaos_store_int64(");
			handler.AppendFormatted(resultExpression);
			handler.AppendLiteral(");");
			stringBuilder6.AppendLine(ref handler);
			break;
		}
		case AotCoreIrAbiCarrierKind.UInt64:
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder5 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(59, 2, stringBuilder);
			handler.AppendFormatted(indentation);
			handler.AppendLiteral("chaos_eval_stack[chaos_stack_top++] = chaos_store_uint64(");
			handler.AppendFormatted(resultExpression);
			handler.AppendLiteral(");");
			stringBuilder5.AppendLine(ref handler);
			break;
		}
		case AotCoreIrAbiCarrierKind.ValueTypeByValue:
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder2 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(36, 2, stringBuilder);
			handler.AppendFormatted(indentation);
			handler.AppendLiteral("auto* chaos_result_storage = new ");
			handler.AppendFormatted(GetRequiredAbiValueTypeSymbol(returnAbi));
			handler.AppendLiteral("{};");
			stringBuilder2.AppendLine(ref handler);
			stringBuilder = builder;
			StringBuilder stringBuilder3 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(25, 2, stringBuilder);
			handler.AppendFormatted(indentation);
			handler.AppendLiteral("*chaos_result_storage = ");
			handler.AppendFormatted(resultExpression);
			handler.AppendLiteral(";");
			stringBuilder3.AppendLine(ref handler);
			stringBuilder = builder;
			StringBuilder stringBuilder4 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(92, 1, stringBuilder);
			handler.AppendFormatted(indentation);
			handler.AppendLiteral("chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<std::intptr_t>(chaos_result_storage);");
			stringBuilder4.AppendLine(ref handler);
			break;
		}
		default:
			throw new NotSupportedException($"native-aot lowering does not support pushing ABI return carrier '{returnAbi.CarrierKindCode}'.");
		}
	}

	private static bool CanEmitMethodBody(AotCoreIrMethodArtifact method)
	{
		return method.Instructions.Count > 0;
	}

	private static IReadOnlyList<AotCoreIrAbiSlotArtifact> GetMethodAbiParameterSlots(AotCoreIrMethodArtifact method)
	{
		ArgumentNullException.ThrowIfNull(method, "method");
		if (method.IsStatic)
		{
			return method.ParameterAbis;
		}
		List<AotCoreIrAbiSlotArtifact> list = new List<AotCoreIrAbiSlotArtifact>(checked(method.ParameterAbis.Count + 1));
		list.Add(CreateNativeIntAbiSlot(method.Identity.DeclaringTypeSubjectId, AotCoreIrTypeShapeKind.ReferenceType));
		list.AddRange(method.ParameterAbis);
		return list;
	}

	private static AotCoreIrAbiSlotArtifact GetRequiredMethodAbiParameterSlot(AotCoreIrMethodArtifact method, int argumentIndex)
	{
		IReadOnlyList<AotCoreIrAbiSlotArtifact> methodAbiParameterSlots = GetMethodAbiParameterSlots(method);
		if (argumentIndex < 0 || argumentIndex >= methodAbiParameterSlots.Count)
		{
			throw new InvalidOperationException($"native-aot lowering could not resolve argument slot {argumentIndex} for '{method.SubjectId}'.");
		}
		return methodAbiParameterSlots[argumentIndex];
	}

	private static IReadOnlyList<AotCoreIrAbiSlotArtifact> CreateLegacyAbiParameterSlots(int parameterCount)
	{
		if (parameterCount < 0)
		{
			throw new NotSupportedException("native-aot lowering requires a non-negative parameter count.");
		}
		if (parameterCount == 0)
		{
			return Array.Empty<AotCoreIrAbiSlotArtifact>();
		}
		return (from _ in Enumerable.Range(0, parameterCount)
			select CreateNativeIntAbiSlot()).ToArray();
	}

	private static IReadOnlyList<AotCoreIrAbiSlotArtifact> CreateCallSiteParameterAbis(AotCoreIrInstructionArtifact instruction)
	{
		if (instruction.CallSiteSignature?.ParameterTypes == null)
		{
			throw new NotSupportedException("native-aot lowering requires call-site signature metadata for '" + instruction.Op + "'.");
		}
		return instruction.CallSiteSignature.ParameterTypes.Select(CreateLegacyAbiSlot).ToArray();
	}

	private static AotCoreIrAbiSlotArtifact CreateCallSiteReturnAbi(AotCoreIrInstructionArtifact instruction)
	{
		if ((object)instruction.CallSiteSignature == null)
		{
			throw new NotSupportedException("native-aot lowering requires call-site signature metadata for '" + instruction.Op + "'.");
		}
		return CreateLegacyReturnAbiSlot(instruction.CallSiteSignature.ReturnType);
	}

	private static AotCoreIrAbiSlotArtifact CreateLegacyReturnAbiSlot(string? returnType)
	{
		switch (returnType)
		{
		case "System.Void":
			return new AotCoreIrAbiSlotArtifact
			{
				CarrierKindCode = AotCoreIrAbiCarrierKind.Void,
				TypeShape = (AotCoreIrTypeShapeKind)0
			};
		case "System.Int32":
			return new AotCoreIrAbiSlotArtifact
			{
				CarrierKindCode = AotCoreIrAbiCarrierKind.Int32,
				TypeShape = AotCoreIrTypeShapeKind.ValueType
			};
		case "System.SByte":
			return new AotCoreIrAbiSlotArtifact
			{
				CarrierKindCode = AotCoreIrAbiCarrierKind.Int8,
				TypeShape = AotCoreIrTypeShapeKind.ValueType
			};
		case "System.Byte":
			return new AotCoreIrAbiSlotArtifact
			{
				CarrierKindCode = AotCoreIrAbiCarrierKind.UInt8,
				TypeShape = AotCoreIrTypeShapeKind.ValueType
			};
		case "System.Int16":
			return new AotCoreIrAbiSlotArtifact
			{
				CarrierKindCode = AotCoreIrAbiCarrierKind.Int16,
				TypeShape = AotCoreIrTypeShapeKind.ValueType
			};
		case "System.UInt16":
			return new AotCoreIrAbiSlotArtifact
			{
				CarrierKindCode = AotCoreIrAbiCarrierKind.UInt16,
				TypeShape = AotCoreIrTypeShapeKind.ValueType
			};
		case "System.Single":
			return new AotCoreIrAbiSlotArtifact
			{
				CarrierKindCode = AotCoreIrAbiCarrierKind.Float32,
				TypeShape = AotCoreIrTypeShapeKind.ValueType
			};
		case "System.Double":
			return new AotCoreIrAbiSlotArtifact
			{
				CarrierKindCode = AotCoreIrAbiCarrierKind.Float64,
				TypeShape = AotCoreIrTypeShapeKind.ValueType
			};
		case "System.Int64":
			return new AotCoreIrAbiSlotArtifact
			{
				CarrierKindCode = AotCoreIrAbiCarrierKind.Int64,
				TypeShape = AotCoreIrTypeShapeKind.ValueType
			};
		case "System.UInt64":
			return new AotCoreIrAbiSlotArtifact
			{
				CarrierKindCode = AotCoreIrAbiCarrierKind.UInt64,
				TypeShape = AotCoreIrTypeShapeKind.ValueType
			};
		default:
			if (!string.IsNullOrWhiteSpace(returnType))
			{
				return CreateLegacyAbiSlot(returnType);
			}
			throw new NotSupportedException("native-aot lowering does not support unresolved legacy return type '" + (returnType ?? "<null>") + "'.");
		}
	}

	private static AotCoreIrAbiSlotArtifact CreateLegacyAbiSlot(string? typeName)
	{
		switch (typeName)
		{
		case "System.Boolean":
			return CreateNativeIntAbiSlot();
		case "System.Byte":
			return new AotCoreIrAbiSlotArtifact
			{
				CarrierKindCode = AotCoreIrAbiCarrierKind.UInt8,
				TypeShape = AotCoreIrTypeShapeKind.ValueType
			};
		case "System.SByte":
			return new AotCoreIrAbiSlotArtifact
			{
				CarrierKindCode = AotCoreIrAbiCarrierKind.Int8,
				TypeShape = AotCoreIrTypeShapeKind.ValueType
			};
		case "System.Int16":
			return new AotCoreIrAbiSlotArtifact
			{
				CarrierKindCode = AotCoreIrAbiCarrierKind.Int16,
				TypeShape = AotCoreIrTypeShapeKind.ValueType
			};
		case "System.UInt16":
			return new AotCoreIrAbiSlotArtifact
			{
				CarrierKindCode = AotCoreIrAbiCarrierKind.UInt16,
				TypeShape = AotCoreIrTypeShapeKind.ValueType
			};
		case "System.Int32":
			return CreateInt32AbiSlot();
		case "System.UInt32":
			return new AotCoreIrAbiSlotArtifact
			{
				CarrierKindCode = AotCoreIrAbiCarrierKind.Int32,
				TypeShape = AotCoreIrTypeShapeKind.ValueType
			};
		case "System.Single":
			return new AotCoreIrAbiSlotArtifact
			{
				CarrierKindCode = AotCoreIrAbiCarrierKind.Float32,
				TypeShape = AotCoreIrTypeShapeKind.ValueType
			};
		case "System.Double":
			return new AotCoreIrAbiSlotArtifact
			{
				CarrierKindCode = AotCoreIrAbiCarrierKind.Float64,
				TypeShape = AotCoreIrTypeShapeKind.ValueType
			};
		case "System.Int64":
			return new AotCoreIrAbiSlotArtifact
			{
				CarrierKindCode = AotCoreIrAbiCarrierKind.Int64,
				TypeShape = AotCoreIrTypeShapeKind.ValueType
			};
		case "System.UInt64":
			return new AotCoreIrAbiSlotArtifact
			{
				CarrierKindCode = AotCoreIrAbiCarrierKind.UInt64,
				TypeShape = AotCoreIrTypeShapeKind.ValueType
			};
		default:
			if (!string.IsNullOrWhiteSpace(typeName))
			{
				return CreateNativeIntAbiSlot();
			}
			throw new NotSupportedException("native-aot lowering does not support unresolved legacy ABI type '" + (typeName ?? "<null>") + "'.");
		}
	}

	private static AotCoreIrAbiSlotArtifact CreateNativeIntAbiSlot(string? typeSubjectId = null, AotCoreIrTypeShapeKind typeShape = (AotCoreIrTypeShapeKind)0)
	{
		return new AotCoreIrAbiSlotArtifact
		{
			CarrierKindCode = AotCoreIrAbiCarrierKind.NativeInt,
			TypeSubjectId = typeSubjectId,
			TypeShape = typeShape
		};
	}

	private static AotCoreIrAbiSlotArtifact CreateInt32AbiSlot(string? typeSubjectId = null, AotCoreIrTypeShapeKind typeShape = (AotCoreIrTypeShapeKind)0)
	{
		return new AotCoreIrAbiSlotArtifact
		{
			CarrierKindCode = AotCoreIrAbiCarrierKind.Int32,
			TypeSubjectId = typeSubjectId,
			TypeShape = typeShape
		};
	}

	private static string MapAbiSlotReturnType(AotCoreIrAbiSlotArtifact abiSlot)
	{
		return abiSlot.CarrierKindCode switch
		{
			AotCoreIrAbiCarrierKind.Void => "void", 
			AotCoreIrAbiCarrierKind.Int32 => "std::int32_t", 
			AotCoreIrAbiCarrierKind.Int8 => "std::int8_t", 
			AotCoreIrAbiCarrierKind.UInt8 => "std::uint8_t", 
			AotCoreIrAbiCarrierKind.Int16 => "std::int16_t", 
			AotCoreIrAbiCarrierKind.UInt16 => "std::uint16_t", 
			AotCoreIrAbiCarrierKind.Float32 => "float", 
			AotCoreIrAbiCarrierKind.Float64 => "double", 
			AotCoreIrAbiCarrierKind.Int64 => "std::int64_t", 
			AotCoreIrAbiCarrierKind.UInt64 => "std::uint64_t", 
			AotCoreIrAbiCarrierKind.NativeInt => "std::intptr_t", 
			AotCoreIrAbiCarrierKind.ValueTypeByValue => GetRequiredAbiValueTypeSymbol(abiSlot), 
			_ => throw new NotSupportedException($"native-aot lowering does not support ABI return carrier '{abiSlot.CarrierKindCode}'."), 
		};
	}

	private static string FormatAbiSlotParameterSignature(IReadOnlyList<AotCoreIrAbiSlotArtifact> abiSlots)
	{
		ArgumentNullException.ThrowIfNull(abiSlots, "abiSlots");
		if (abiSlots.Count == 0)
		{
			return "void";
		}
		return string.Join(", ", abiSlots.Select((AotCoreIrAbiSlotArtifact slot, int index) => $"{MapAbiSlotParameterType(slot)} chaos_arg_{index}"));
	}

	private static void EmitAbiArgumentInitialization(StringBuilder builder, IReadOnlyList<AotCoreIrAbiSlotArtifact> abiSlots)
	{
		for (int i = 0; i < abiSlots.Count; i++)
		{
			AotCoreIrAbiSlotArtifact aotCoreIrAbiSlotArtifact = abiSlots[i];
			switch (aotCoreIrAbiSlotArtifact.CarrierKindCode)
			{
			case AotCoreIrAbiCarrierKind.Int32:
			case AotCoreIrAbiCarrierKind.NativeInt:
			case AotCoreIrAbiCarrierKind.Int8:
			case AotCoreIrAbiCarrierKind.UInt8:
			case AotCoreIrAbiCarrierKind.Int16:
			case AotCoreIrAbiCarrierKind.UInt16:
			{
				StringBuilder stringBuilder = builder;
				StringBuilder stringBuilder8 = stringBuilder;
				StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(58, 2, stringBuilder);
				handler.AppendLiteral("    chaos_args[");
				handler.AppendFormatted(i);
				handler.AppendLiteral("] = static_cast<std::intptr_t>(chaos_arg_");
				handler.AppendFormatted(i);
				handler.AppendLiteral(");");
				stringBuilder8.AppendLine(ref handler);
				break;
			}
			case AotCoreIrAbiCarrierKind.Float32:
			{
				StringBuilder stringBuilder = builder;
				StringBuilder stringBuilder7 = stringBuilder;
				StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(51, 2, stringBuilder);
				handler.AppendLiteral("    chaos_args[");
				handler.AppendFormatted(i);
				handler.AppendLiteral("] = chaos_store_float32(chaos_arg_");
				handler.AppendFormatted(i);
				handler.AppendLiteral(");");
				stringBuilder7.AppendLine(ref handler);
				break;
			}
			case AotCoreIrAbiCarrierKind.Float64:
			{
				StringBuilder stringBuilder = builder;
				StringBuilder stringBuilder6 = stringBuilder;
				StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(51, 2, stringBuilder);
				handler.AppendLiteral("    chaos_args[");
				handler.AppendFormatted(i);
				handler.AppendLiteral("] = chaos_store_float64(chaos_arg_");
				handler.AppendFormatted(i);
				handler.AppendLiteral(");");
				stringBuilder6.AppendLine(ref handler);
				break;
			}
			case AotCoreIrAbiCarrierKind.Int64:
			{
				StringBuilder stringBuilder = builder;
				StringBuilder stringBuilder5 = stringBuilder;
				StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(49, 2, stringBuilder);
				handler.AppendLiteral("    chaos_args[");
				handler.AppendFormatted(i);
				handler.AppendLiteral("] = chaos_store_int64(chaos_arg_");
				handler.AppendFormatted(i);
				handler.AppendLiteral(");");
				stringBuilder5.AppendLine(ref handler);
				break;
			}
			case AotCoreIrAbiCarrierKind.UInt64:
			{
				StringBuilder stringBuilder = builder;
				StringBuilder stringBuilder4 = stringBuilder;
				StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(50, 2, stringBuilder);
				handler.AppendLiteral("    chaos_args[");
				handler.AppendFormatted(i);
				handler.AppendLiteral("] = chaos_store_uint64(chaos_arg_");
				handler.AppendFormatted(i);
				handler.AppendLiteral(");");
				stringBuilder4.AppendLine(ref handler);
				break;
			}
			case AotCoreIrAbiCarrierKind.ValueTypeByValue:
			{
				StringBuilder stringBuilder = builder;
				StringBuilder stringBuilder2 = stringBuilder;
				StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(39, 2, stringBuilder);
				handler.AppendLiteral("    auto chaos_abi_param_");
				handler.AppendFormatted(i);
				handler.AppendLiteral(" = chaos_arg_");
				handler.AppendFormatted(i);
				handler.AppendLiteral(";");
				stringBuilder2.AppendLine(ref handler);
				stringBuilder = builder;
				StringBuilder stringBuilder3 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(70, 2, stringBuilder);
				handler.AppendLiteral("    chaos_args[");
				handler.AppendFormatted(i);
				handler.AppendLiteral("] = reinterpret_cast<std::intptr_t>(&chaos_abi_param_");
				handler.AppendFormatted(i);
				handler.AppendLiteral(");");
				stringBuilder3.AppendLine(ref handler);
				break;
			}
			default:
				throw new NotSupportedException($"native-aot lowering does not support ABI parameter carrier '{aotCoreIrAbiSlotArtifact.CarrierKindCode}'.");
			}
		}
	}

	private static string FormatAbiInvocationArgumentList(IReadOnlyList<AotCoreIrAbiSlotArtifact> abiSlots, string? firstArgumentOverride = null)
	{
		if (abiSlots.Count == 0)
		{
			return string.Empty;
		}
		return string.Join(", ", abiSlots.Select(delegate(AotCoreIrAbiSlotArtifact slot, int index)
		{
			string sourceName = ((index == 0 && !string.IsNullOrWhiteSpace(firstArgumentOverride)) ? firstArgumentOverride : $"chaos_arg_{index}");
			return FormatAbiArgumentExpression(slot, sourceName);
		}));
	}

	private static string FormatAbiArgumentExpression(AotCoreIrAbiSlotArtifact abiSlot, string sourceName)
	{
		return abiSlot.CarrierKindCode switch
		{
			AotCoreIrAbiCarrierKind.Int32 => "static_cast<std::int32_t>(" + sourceName + ")", 
			AotCoreIrAbiCarrierKind.Int8 => "static_cast<std::int8_t>(" + sourceName + ")", 
			AotCoreIrAbiCarrierKind.UInt8 => "static_cast<std::uint8_t>(" + sourceName + ")", 
			AotCoreIrAbiCarrierKind.Int16 => "static_cast<std::int16_t>(" + sourceName + ")", 
			AotCoreIrAbiCarrierKind.UInt16 => "static_cast<std::uint16_t>(" + sourceName + ")", 
			AotCoreIrAbiCarrierKind.Float32 => "chaos_load_float32(" + sourceName + ")", 
			AotCoreIrAbiCarrierKind.Float64 => "chaos_load_float64(" + sourceName + ")", 
			AotCoreIrAbiCarrierKind.Int64 => "chaos_load_int64(" + sourceName + ")", 
			AotCoreIrAbiCarrierKind.UInt64 => "chaos_load_uint64(" + sourceName + ")", 
			AotCoreIrAbiCarrierKind.NativeInt => sourceName, 
			AotCoreIrAbiCarrierKind.ValueTypeByValue => $"*chaos_resolve_managed_value_pointer<{GetRequiredAbiValueTypeSymbol(abiSlot)}>({sourceName})", 
			_ => throw new NotSupportedException($"native-aot lowering does not support ABI argument carrier '{abiSlot.CarrierKindCode}'."), 
		};
	}

	private static string FormatInboundAbiArgumentExpression(AotCoreIrAbiSlotArtifact abiSlot, string sourceName)
	{
		if (abiSlot.CarrierKindCode == AotCoreIrAbiCarrierKind.NativeInt)
		{
			return "chaos_normalize_native_int_argument(" + sourceName + ")";
		}
		return sourceName;
	}

	private static string MapAbiSlotParameterType(AotCoreIrAbiSlotArtifact abiSlot)
	{
		return abiSlot.CarrierKindCode switch
		{
			AotCoreIrAbiCarrierKind.Int32 => "std::int32_t", 
			AotCoreIrAbiCarrierKind.Int8 => "std::int8_t", 
			AotCoreIrAbiCarrierKind.UInt8 => "std::uint8_t", 
			AotCoreIrAbiCarrierKind.Int16 => "std::int16_t", 
			AotCoreIrAbiCarrierKind.UInt16 => "std::uint16_t", 
			AotCoreIrAbiCarrierKind.Float32 => "float", 
			AotCoreIrAbiCarrierKind.Float64 => "double", 
			AotCoreIrAbiCarrierKind.Int64 => "std::int64_t", 
			AotCoreIrAbiCarrierKind.UInt64 => "std::uint64_t", 
			AotCoreIrAbiCarrierKind.NativeInt => "std::intptr_t", 
			AotCoreIrAbiCarrierKind.ValueTypeByValue => GetRequiredAbiValueTypeSymbol(abiSlot), 
			_ => throw new NotSupportedException($"native-aot lowering does not support ABI parameter carrier '{abiSlot.CarrierKindCode}'."), 
		};
	}

	private static string GetRequiredAbiValueTypeSymbol(AotCoreIrAbiSlotArtifact abiSlot)
	{
		if (abiSlot.CarrierKindCode != AotCoreIrAbiCarrierKind.ValueTypeByValue || string.IsNullOrWhiteSpace(abiSlot.TypeSubjectId))
		{
			throw new NotSupportedException($"native-aot lowering requires a value-type ABI slot with subject metadata, got '{abiSlot.CarrierKindCode}'.");
		}
		return GetNativeValueTypeSymbol(abiSlot.TypeSubjectId);
	}

	private static string FormatInt32Literal(int value)
	{
		if (value != int.MinValue)
		{
			return value.ToString(CultureInfo.InvariantCulture);
		}
		return "std::numeric_limits<std::int32_t>::min()";
	}

	private static string FormatInt64Literal(long value)
	{
		if (value != long.MinValue)
		{
			return value.ToString(CultureInfo.InvariantCulture) + "LL";
		}
		return "std::numeric_limits<std::int64_t>::min()";
	}

	private static string FormatFloat32Literal(float value)
	{
		return value.ToString("R", CultureInfo.InvariantCulture) + "f";
	}

	private static string FormatFloat64Literal(double value)
	{
		return value.ToString("R", CultureInfo.InvariantCulture);
	}

	private static void RequireStringField(string? value, string fieldName)
	{
		if (string.IsNullOrWhiteSpace(value))
		{
			throw new InvalidOperationException("native-aot lowering plan requires non-empty field '" + fieldName + "'");
		}
	}

	private static T LoadRequiredJson<T>(string path)
	{
		if (!File.Exists(path))
		{
			throw new FileNotFoundException("required native-aot lowering plan is missing: " + path, path);
		}
		return JsonSerializer.Deserialize<T>(File.ReadAllText(path), JsonOptions) ?? throw new InvalidOperationException("failed to deserialize native-aot lowering plan: " + path);
	}

}
