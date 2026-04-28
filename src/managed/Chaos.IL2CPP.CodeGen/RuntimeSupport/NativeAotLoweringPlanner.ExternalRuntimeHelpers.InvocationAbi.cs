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
using Scriban.Runtime;

namespace Chaos.IL2CPP.CodeGen;

public sealed partial class NativeAotLoweringPlanner
{
	private bool IsTrackedValueTypeSubjectId(string subjectId)
	{
		if (!string.IsNullOrEmpty(subjectId))
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
		return $"reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&reinterpret_cast<{nativeBoxTypeSymbol}*>({instanceExpression})->value)";
	}

	private string GetRequiredFunctionPointerTargetSymbol(AotCoreIrInstructionArtifact instruction)
	{
		if (!string.IsNullOrEmpty(instruction.Callee) && _methodsBySubjectId.TryGetValue(instruction.Callee, out AotCoreIrMethodArtifact value))
		{
			return TryGetInstantiationStubSymbol(value) ?? value.NativeSymbol;
		}
		if (!string.IsNullOrEmpty(instruction.TargetSymbol))
		{
			return instruction.TargetSymbol;
		}
		throw new NotSupportedException("native-aot lowering does not support unresolved function pointer target '" + (instruction.Callee ?? "<null>") + "'.");
	}

	private static string GetRequiredTargetSymbol(AotCoreIrInstructionArtifact instruction)
	{
		if (TryGetInstantiationStubSymbol(instruction.TargetReference?.InstantiationStubId) is { } text)
		{
			return text;
		}
		if (!string.IsNullOrEmpty(instruction.TargetSymbol))
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
		string[] returnLines;
		switch (returnAbi.CarrierKindCode)
		{
		case AotCoreIrAbiCarrierKind.Void:
			returnLines =
			[
				"    return;"
			];
			break;
		case AotCoreIrAbiCarrierKind.Int32:
			returnLines =
			[
				"    return static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);"
			];
			break;
		case AotCoreIrAbiCarrierKind.Int8:
			returnLines =
			[
				"    return static_cast<CHAOS_IL2CPP_INT8>(chaos_eval_stack[--chaos_stack_top]);"
			];
			break;
		case AotCoreIrAbiCarrierKind.UInt8:
			returnLines =
			[
				"    return static_cast<CHAOS_IL2CPP_UINT8>(chaos_eval_stack[--chaos_stack_top]);"
			];
			break;
		case AotCoreIrAbiCarrierKind.Int16:
			returnLines =
			[
				"    return static_cast<CHAOS_IL2CPP_INT16>(chaos_eval_stack[--chaos_stack_top]);"
			];
			break;
		case AotCoreIrAbiCarrierKind.UInt16:
			returnLines =
			[
				"    return static_cast<CHAOS_IL2CPP_UINT16>(chaos_eval_stack[--chaos_stack_top]);"
			];
			break;
		case AotCoreIrAbiCarrierKind.Float32:
			returnLines =
			[
				"    return chaos_load_float32(chaos_eval_stack[--chaos_stack_top]);"
			];
			break;
		case AotCoreIrAbiCarrierKind.Float64:
			returnLines =
			[
				"    return chaos_load_float64(chaos_eval_stack[--chaos_stack_top]);"
			];
			break;
		case AotCoreIrAbiCarrierKind.Int64:
			returnLines =
			[
				"    return chaos_load_int64(chaos_eval_stack[--chaos_stack_top]);"
			];
			break;
		case AotCoreIrAbiCarrierKind.UInt64:
			returnLines =
			[
				"    return chaos_load_uint64(chaos_eval_stack[--chaos_stack_top]);"
			];
			break;
		case AotCoreIrAbiCarrierKind.NativeInt:
			returnLines =
			[
				"    return chaos_eval_stack[--chaos_stack_top];"
			];
			break;
		case AotCoreIrAbiCarrierKind.ValueTypeByValue:
			returnLines =
			[
				$"    return *chaos_resolve_managed_value_pointer<{GetRequiredAbiValueTypeSymbol(returnAbi)}>(chaos_eval_stack[--chaos_stack_top]);"
			];
			break;
		default:
			throw new NotSupportedException($"native-aot lowering does not support ABI return carrier '{returnAbi.CarrierKindCode}'.");
		}

		builder.AppendLine(
			ScribanTemplateRenderer.RenderTemplate(
				NativeAotTemplateCatalog.GetMethodReturnTemplate(),
				new ScriptObject
				{
					["lines"] = returnLines,
				}).TrimEnd());
	}

	private static void EmitAbiReturnPush(StringBuilder builder, AotCoreIrAbiSlotArtifact returnAbi, string resultExpression, string indentation)
	{
		string[] pushLines;
		switch (returnAbi.CarrierKindCode)
		{
		case AotCoreIrAbiCarrierKind.Int32:
		case AotCoreIrAbiCarrierKind.NativeInt:
		case AotCoreIrAbiCarrierKind.Int8:
		case AotCoreIrAbiCarrierKind.UInt8:
		case AotCoreIrAbiCarrierKind.Int16:
		case AotCoreIrAbiCarrierKind.UInt16:
			pushLines =
			[
				$"{indentation}chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>({resultExpression});"
			];
			break;
		case AotCoreIrAbiCarrierKind.Float32:
			pushLines =
			[
				$"{indentation}chaos_eval_stack[chaos_stack_top++] = chaos_store_float32({resultExpression});"
			];
			break;
		case AotCoreIrAbiCarrierKind.Float64:
			pushLines =
			[
				$"{indentation}chaos_eval_stack[chaos_stack_top++] = chaos_store_float64({resultExpression});"
			];
			break;
		case AotCoreIrAbiCarrierKind.Int64:
			pushLines =
			[
				$"{indentation}chaos_eval_stack[chaos_stack_top++] = chaos_store_int64({resultExpression});"
			];
			break;
		case AotCoreIrAbiCarrierKind.UInt64:
			pushLines =
			[
				$"{indentation}chaos_eval_stack[chaos_stack_top++] = chaos_store_uint64({resultExpression});"
			];
			break;
		case AotCoreIrAbiCarrierKind.ValueTypeByValue:
			pushLines =
			[
				$"{indentation}auto* chaos_result_storage = new {GetRequiredAbiValueTypeSymbol(returnAbi)}{{}};",
				$"{indentation}*chaos_result_storage = {resultExpression};",
				$"{indentation}chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_result_storage);",
			];
			break;
		default:
			throw new NotSupportedException($"native-aot lowering does not support pushing ABI return carrier '{returnAbi.CarrierKindCode}'.");
		}

		builder.AppendLine(
			ScribanTemplateRenderer.RenderTemplate(
				NativeAotTemplateCatalog.GetAbiReturnPushTemplate(),
				new ScriptObject
				{
					["lines"] = pushLines,
				}).TrimEnd());
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
			if (!string.IsNullOrEmpty(returnType))
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
			if (!string.IsNullOrEmpty(typeName))
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
			AotCoreIrAbiCarrierKind.Int32 => "CHAOS_IL2CPP_INT32", 
			AotCoreIrAbiCarrierKind.Int8 => "CHAOS_IL2CPP_INT8", 
			AotCoreIrAbiCarrierKind.UInt8 => "CHAOS_IL2CPP_UINT8", 
			AotCoreIrAbiCarrierKind.Int16 => "CHAOS_IL2CPP_INT16", 
			AotCoreIrAbiCarrierKind.UInt16 => "CHAOS_IL2CPP_UINT16", 
			AotCoreIrAbiCarrierKind.Float32 => "float", 
			AotCoreIrAbiCarrierKind.Float64 => "double", 
			AotCoreIrAbiCarrierKind.Int64 => "CHAOS_IL2CPP_INT64", 
			AotCoreIrAbiCarrierKind.UInt64 => "CHAOS_IL2CPP_UINT64", 
			AotCoreIrAbiCarrierKind.NativeInt => "CHAOS_IL2CPP_INTPTR", 
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
		var lines = new List<string>();
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
				lines.Add($"    chaos_args[{i}] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_arg_{i});");
				break;
			case AotCoreIrAbiCarrierKind.Float32:
				lines.Add($"    chaos_args[{i}] = chaos_store_float32(chaos_arg_{i});");
				break;
			case AotCoreIrAbiCarrierKind.Float64:
				lines.Add($"    chaos_args[{i}] = chaos_store_float64(chaos_arg_{i});");
				break;
			case AotCoreIrAbiCarrierKind.Int64:
				lines.Add($"    chaos_args[{i}] = chaos_store_int64(chaos_arg_{i});");
				break;
			case AotCoreIrAbiCarrierKind.UInt64:
				lines.Add($"    chaos_args[{i}] = chaos_store_uint64(chaos_arg_{i});");
				break;
			case AotCoreIrAbiCarrierKind.ValueTypeByValue:
				lines.Add($"    auto chaos_abi_param_{i} = chaos_arg_{i};");
				lines.Add($"    chaos_args[{i}] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_abi_param_{i});");
				break;
			default:
				throw new NotSupportedException($"native-aot lowering does not support ABI parameter carrier '{aotCoreIrAbiSlotArtifact.CarrierKindCode}'.");
			}
		}

		builder.AppendLine(
			ScribanTemplateRenderer.RenderTemplate(
				NativeAotTemplateCatalog.GetAbiArgumentInitializationTemplate(),
				new ScriptObject
				{
					["lines"] = lines.ToArray(),
				}).TrimEnd());
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
			AotCoreIrAbiCarrierKind.Int32 => "static_cast<CHAOS_IL2CPP_INT32>(" + sourceName + ")", 
			AotCoreIrAbiCarrierKind.Int8 => "static_cast<CHAOS_IL2CPP_INT8>(" + sourceName + ")", 
			AotCoreIrAbiCarrierKind.UInt8 => "static_cast<CHAOS_IL2CPP_UINT8>(" + sourceName + ")", 
			AotCoreIrAbiCarrierKind.Int16 => "static_cast<CHAOS_IL2CPP_INT16>(" + sourceName + ")", 
			AotCoreIrAbiCarrierKind.UInt16 => "static_cast<CHAOS_IL2CPP_UINT16>(" + sourceName + ")", 
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
			AotCoreIrAbiCarrierKind.Int32 => "CHAOS_IL2CPP_INT32", 
			AotCoreIrAbiCarrierKind.Int8 => "CHAOS_IL2CPP_INT8", 
			AotCoreIrAbiCarrierKind.UInt8 => "CHAOS_IL2CPP_UINT8", 
			AotCoreIrAbiCarrierKind.Int16 => "CHAOS_IL2CPP_INT16", 
			AotCoreIrAbiCarrierKind.UInt16 => "CHAOS_IL2CPP_UINT16", 
			AotCoreIrAbiCarrierKind.Float32 => "float", 
			AotCoreIrAbiCarrierKind.Float64 => "double", 
			AotCoreIrAbiCarrierKind.Int64 => "CHAOS_IL2CPP_INT64", 
			AotCoreIrAbiCarrierKind.UInt64 => "CHAOS_IL2CPP_UINT64", 
			AotCoreIrAbiCarrierKind.NativeInt => "CHAOS_IL2CPP_INTPTR", 
			AotCoreIrAbiCarrierKind.ValueTypeByValue => GetRequiredAbiValueTypeSymbol(abiSlot), 
			_ => throw new NotSupportedException($"native-aot lowering does not support ABI parameter carrier '{abiSlot.CarrierKindCode}'."), 
		};
	}

	private static string GetRequiredAbiValueTypeSymbol(AotCoreIrAbiSlotArtifact abiSlot)
	{
		if (abiSlot.CarrierKindCode != AotCoreIrAbiCarrierKind.ValueTypeByValue || string.IsNullOrEmpty(abiSlot.TypeSubjectId))
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
		return "CHAOS_IL2CPP_NUMERIC_LIMITS_MIN(CHAOS_IL2CPP_INT32)";
	}

	private static string FormatInt64Literal(long value)
	{
		if (value != long.MinValue)
		{
			return value.ToString(CultureInfo.InvariantCulture) + "LL";
		}
		return "CHAOS_IL2CPP_NUMERIC_LIMITS_MIN(CHAOS_IL2CPP_INT64)";
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
		if (string.IsNullOrEmpty(value))
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

	private static string? TryGetInstantiationStubSymbol(InstantiationStubId? instantiationStubId)
	{
		if (instantiationStubId is null)
		{
			return null;
		}

		return ManagedNaming.CreateInstantiationStubSymbol(instantiationStubId);
	}

}
