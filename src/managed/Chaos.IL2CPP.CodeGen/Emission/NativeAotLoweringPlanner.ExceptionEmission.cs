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
	private void EmitCatchOnlyExceptionMethodBody(StringBuilder builder, AotCoreIrMethodArtifact method, CatchOnlyExceptionMethodShape catchOnlyShape, IReadOnlyDictionary<int, int?> nextOffsetsByIlOffset, IReadOnlySet<int> offsets)
	{
		EmitLinearInstructionSequence(builder, catchOnlyShape.PrefixInstructions, "    ");
		if (catchOnlyShape.PrefixInstructions.Count > 0)
		{
			builder.AppendLine();
		}
		builder.AppendLine("    try");
		builder.AppendLine("    {");
		StringBuilder stringBuilder = builder;
		StringBuilder stringBuilder2 = stringBuilder;
		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(23, 1, stringBuilder);
		handler.AppendLiteral("        goto chaos_ip_");
		handler.AppendFormatted(GetRequiredIlOffset(catchOnlyShape.TryInstructions[0]));
		handler.AppendLiteral(";");
		stringBuilder2.AppendLine(ref handler);
		builder.AppendLine();
		EmitStructuredInstructionRange(builder, method, catchOnlyShape.TryInstructions, nextOffsetsByIlOffset, offsets);
		builder.AppendLine("    }");
		builder.AppendLine("    catch (const chaos_managed_exception& chaos_exception)");
		builder.AppendLine("    {");
		builder.AppendLine("        auto* chaos_header = reinterpret_cast<chaos_object_header*>(chaos_exception.object_value);");
		builder.AppendLine("        if (chaos_header == nullptr)");
		builder.AppendLine("        {");
		builder.AppendLine("            throw;");
		builder.AppendLine("        }");
		stringBuilder = builder;
		StringBuilder stringBuilder3 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(63, 1, stringBuilder);
		handler.AppendLiteral("        if (!chaos_is_type_compatible(chaos_header->type_info, &");
		handler.AppendFormatted(GetNativeTypeInfoSymbol(catchOnlyShape.ExceptionRegion.CatchTypeSubjectId));
		handler.AppendLiteral("))");
		stringBuilder3.AppendLine(ref handler);
		builder.AppendLine("        {");
		builder.AppendLine("            throw;");
		builder.AppendLine("        }");
		builder.AppendLine("        chaos_eval_stack[chaos_stack_top++] = chaos_exception.object_value;");
		stringBuilder = builder;
		StringBuilder stringBuilder4 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(23, 1, stringBuilder);
		handler.AppendLiteral("        goto chaos_ip_");
		handler.AppendFormatted(GetRequiredIlOffset(catchOnlyShape.HandlerInstructions[0]));
		handler.AppendLiteral(";");
		stringBuilder4.AppendLine(ref handler);
		builder.AppendLine();
		EmitStructuredInstructionRange(builder, method, catchOnlyShape.HandlerInstructions, nextOffsetsByIlOffset, offsets);
		builder.AppendLine("    }");
		builder.AppendLine();
		EmitStructuredInstructionRange(builder, method, catchOnlyShape.TailInstructions, nextOffsetsByIlOffset, offsets);
	}

	private void EmitFilterOnlyExceptionMethodBody(StringBuilder builder, AotCoreIrMethodArtifact method, FilterOnlyExceptionMethodShape filterOnlyShape, IReadOnlyDictionary<int, int?> nextOffsetsByIlOffset, IReadOnlySet<int> offsets)
	{
		EmitLinearInstructionSequence(builder, filterOnlyShape.PrefixInstructions, "    ");
		if (filterOnlyShape.PrefixInstructions.Count > 0)
		{
			builder.AppendLine();
		}
		EmitFilterTryCatchCore(builder, method, filterOnlyShape.TryInstructions, filterOnlyShape.FilterInstructions, filterOnlyShape.HandlerInstructions, nextOffsetsByIlOffset, offsets, "    ");
		builder.AppendLine();
		EmitStructuredInstructionRange(builder, method, filterOnlyShape.TailInstructions, nextOffsetsByIlOffset, offsets);
	}

	private void EmitCatchAndFinallyExceptionMethodBody(StringBuilder builder, AotCoreIrMethodArtifact method, CatchAndFinallyExceptionMethodShape catchAndFinallyShape, IReadOnlyDictionary<int, int?> nextOffsetsByIlOffset, IReadOnlySet<int> offsets)
	{
		EmitLinearInstructionSequence(builder, catchAndFinallyShape.PrefixInstructions, "    ");
		if (catchAndFinallyShape.PrefixInstructions.Count > 0)
		{
			builder.AppendLine();
		}
		EmitCatchAndFinallyOuterFinallyScopes(builder, method, catchAndFinallyShape, 0, nextOffsetsByIlOffset, offsets);
		builder.AppendLine();
		EmitStructuredInstructionRange(builder, method, catchAndFinallyShape.TailInstructions, nextOffsetsByIlOffset, offsets);
	}

	private void EmitFinallyOnlyExceptionMethodBody(StringBuilder builder, AotCoreIrMethodArtifact method, FinallyOnlyExceptionMethodShape finallyOnlyShape, IReadOnlyDictionary<int, int?> nextOffsetsByIlOffset, IReadOnlySet<int> offsets)
	{
		EmitStructuredInstructionRange(builder, method, finallyOnlyShape.PrefixInstructions, nextOffsetsByIlOffset, offsets);
		if (finallyOnlyShape.PrefixInstructions.Count > 0)
		{
			builder.AppendLine();
		}
		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(10, 1, builder);
		handler.AppendLiteral("chaos_ip_");
		handler.AppendFormatted(GetRequiredIlOffset(finallyOnlyShape.TryInstructions[0]));
		handler.AppendLiteral(":");
		builder.AppendLine(ref handler);
		EmitFinallyOnlyScopes(builder, method, finallyOnlyShape, 0, nextOffsetsByIlOffset, offsets);
		builder.AppendLine();
		EmitStructuredInstructionRange(builder, method, finallyOnlyShape.TailInstructions, nextOffsetsByIlOffset, offsets);
	}

	private void EmitFinallyOnlyScopes(StringBuilder builder, AotCoreIrMethodArtifact method, FinallyOnlyExceptionMethodShape finallyOnlyShape, int finallyIndex, IReadOnlyDictionary<int, int?> nextOffsetsByIlOffset, IReadOnlySet<int> offsets)
	{
		if (finallyIndex >= finallyOnlyShape.FinallyHandlers.Count)
		{
			AotCoreIrInstructionArtifact instruction = finallyOnlyShape.TryInstructions[0];
			int requiredIlOffset = GetRequiredIlOffset(instruction);
			EmitInstruction(builder, method, instruction, nextOffsetsByIlOffset[requiredIlOffset], offsets);
			if (finallyOnlyShape.TryInstructions.Count > 1)
			{
				builder.AppendLine();
				EmitStructuredInstructionRange(builder, method, finallyOnlyShape.TryInstructions.Skip(1).ToArray(), nextOffsetsByIlOffset, offsets);
			}
		}
		else
		{
			builder.AppendLine("{");
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(73, 1, builder);
			handler.AppendLiteral("    auto chaos_finally_only_guard_");
			handler.AppendFormatted(finallyIndex);
			handler.AppendLiteral(" = chaos_make_finally_scope_guard([&]()");
			builder.AppendLine(ref handler);
			builder.AppendLine("    {");
			EmitStructuredFinallyHandlerSequence(builder, finallyOnlyShape.FinallyHandlers[finallyIndex], "        ");
			builder.AppendLine("    });");
			EmitFinallyOnlyScopes(builder, method, finallyOnlyShape, finallyIndex + 1, nextOffsetsByIlOffset, offsets);
			builder.AppendLine("}");
		}
	}

	private void EmitCatchAndFinallyOuterFinallyScopes(StringBuilder builder, AotCoreIrMethodArtifact method, CatchAndFinallyExceptionMethodShape catchAndFinallyShape, int finallyIndex, IReadOnlyDictionary<int, int?> nextOffsetsByIlOffset, IReadOnlySet<int> offsets)
	{
		if (finallyIndex >= catchAndFinallyShape.OuterFinallyHandlers.Count)
		{
			EmitCatchAndFinallyTryCatchCore(builder, method, catchAndFinallyShape, nextOffsetsByIlOffset, offsets);
			return;
		}
		builder.AppendLine("    {");
		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(78, 1, builder);
		handler.AppendLiteral("        auto chaos_catch_finally_guard_");
		handler.AppendFormatted(finallyIndex);
		handler.AppendLiteral(" = chaos_make_finally_scope_guard([&]()");
		builder.AppendLine(ref handler);
		builder.AppendLine("        {");
		EmitStructuredFinallyHandlerSequence(builder, catchAndFinallyShape.OuterFinallyHandlers[finallyIndex], "            ");
		builder.AppendLine("        });");
		EmitCatchAndFinallyOuterFinallyScopes(builder, method, catchAndFinallyShape, finallyIndex + 1, nextOffsetsByIlOffset, offsets);
		builder.AppendLine("    }");
	}

	private void EmitCatchAndFinallyTryCatchCore(StringBuilder builder, AotCoreIrMethodArtifact method, CatchAndFinallyExceptionMethodShape catchAndFinallyShape, IReadOnlyDictionary<int, int?> nextOffsetsByIlOffset, IReadOnlySet<int> offsets)
	{
		builder.AppendLine("        try");
		builder.AppendLine("        {");
		if (catchAndFinallyShape.PreInnerFinallyInstructions.Count > 0)
		{
			EmitLinearInstructionSequence(builder, catchAndFinallyShape.PreInnerFinallyInstructions, "            ");
			builder.AppendLine();
		}
		StringBuilder stringBuilder;
		StringBuilder.AppendInterpolatedStringHandler handler;
		if ((object)catchAndFinallyShape.InnerFinallyHandler == null)
		{
			stringBuilder = builder;
			StringBuilder stringBuilder2 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(27, 1, stringBuilder);
			handler.AppendLiteral("            goto chaos_ip_");
			handler.AppendFormatted(GetRequiredIlOffset(catchAndFinallyShape.InnerTryInstructions[0]));
			handler.AppendLiteral(";");
			stringBuilder2.AppendLine(ref handler);
			builder.AppendLine();
			EmitStructuredInstructionRange(builder, method, catchAndFinallyShape.InnerTryInstructions, nextOffsetsByIlOffset, offsets);
		}
		else
		{
			builder.AppendLine("            {");
			builder.AppendLine("                auto chaos_inner_finally_guard = chaos_make_finally_scope_guard([&]()");
			builder.AppendLine("                {");
			EmitStructuredFinallyHandlerSequence(builder, catchAndFinallyShape.InnerFinallyHandler, "                    ");
			builder.AppendLine("                });");
			stringBuilder = builder;
			StringBuilder stringBuilder3 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(31, 1, stringBuilder);
			handler.AppendLiteral("                goto chaos_ip_");
			handler.AppendFormatted(GetRequiredIlOffset(catchAndFinallyShape.InnerTryInstructions[0]));
			handler.AppendLiteral(";");
			stringBuilder3.AppendLine(ref handler);
			builder.AppendLine();
			EmitStructuredInstructionRange(builder, method, catchAndFinallyShape.InnerTryInstructions, nextOffsetsByIlOffset, offsets);
			builder.AppendLine("            }");
			if (catchAndFinallyShape.PostInnerTryInstructions.Count > 0)
			{
				builder.AppendLine();
				EmitStructuredInstructionRange(builder, method, catchAndFinallyShape.PostInnerTryInstructions, nextOffsetsByIlOffset, offsets);
			}
		}
		builder.AppendLine("        }");
		builder.AppendLine("        catch (const chaos_managed_exception& chaos_exception)");
		builder.AppendLine("        {");
		builder.AppendLine("            auto* chaos_header = reinterpret_cast<chaos_object_header*>(chaos_exception.object_value);");
		builder.AppendLine("            if (chaos_header == nullptr)");
		builder.AppendLine("            {");
		builder.AppendLine("                throw;");
		builder.AppendLine("            }");
		stringBuilder = builder;
		StringBuilder stringBuilder4 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(67, 1, stringBuilder);
		handler.AppendLiteral("            if (!chaos_is_type_compatible(chaos_header->type_info, &");
		handler.AppendFormatted(GetNativeTypeInfoSymbol(catchAndFinallyShape.CatchRegion.CatchTypeSubjectId));
		handler.AppendLiteral("))");
		stringBuilder4.AppendLine(ref handler);
		builder.AppendLine("            {");
		builder.AppendLine("                throw;");
		builder.AppendLine("            }");
		builder.AppendLine("            chaos_eval_stack[chaos_stack_top++] = chaos_exception.object_value;");
		stringBuilder = builder;
		StringBuilder stringBuilder5 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(27, 1, stringBuilder);
		handler.AppendLiteral("            goto chaos_ip_");
		handler.AppendFormatted(GetRequiredIlOffset(catchAndFinallyShape.HandlerInstructions[0]));
		handler.AppendLiteral(";");
		stringBuilder5.AppendLine(ref handler);
		builder.AppendLine();
		EmitStructuredInstructionRange(builder, method, catchAndFinallyShape.HandlerInstructions, nextOffsetsByIlOffset, offsets);
		builder.AppendLine("        }");
	}

	private void EmitFilterAndFinallyExceptionMethodBody(StringBuilder builder, AotCoreIrMethodArtifact method, FilterAndFinallyExceptionMethodShape filterAndFinallyShape, IReadOnlyDictionary<int, int?> nextOffsetsByIlOffset, IReadOnlySet<int> offsets)
	{
		EmitLinearInstructionSequence(builder, filterAndFinallyShape.PrefixInstructions, "    ");
		if (filterAndFinallyShape.PrefixInstructions.Count > 0)
		{
			builder.AppendLine();
		}
		EmitNestedFinallyScopes(builder, method, filterAndFinallyShape, 0, nextOffsetsByIlOffset, offsets);
		builder.AppendLine();
		EmitStructuredInstructionRange(builder, method, filterAndFinallyShape.TailInstructions, nextOffsetsByIlOffset, offsets);
	}

	private void EmitNestedFinallyScopes(StringBuilder builder, AotCoreIrMethodArtifact method, FilterAndFinallyExceptionMethodShape filterAndFinallyShape, int finallyIndex, IReadOnlyDictionary<int, int?> nextOffsetsByIlOffset, IReadOnlySet<int> offsets)
	{
		if (finallyIndex >= filterAndFinallyShape.FinallyHandlers.Count)
		{
			EmitFilterAndFinallyTryCatchCore(builder, method, filterAndFinallyShape, nextOffsetsByIlOffset, offsets);
			return;
		}
		builder.AppendLine("    {");
		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(72, 1, builder);
		handler.AppendLiteral("        auto chaos_finally_guard_");
		handler.AppendFormatted(finallyIndex);
		handler.AppendLiteral(" = chaos_make_finally_scope_guard([&]()");
		builder.AppendLine(ref handler);
		builder.AppendLine("        {");
		EmitStructuredFinallyHandlerSequence(builder, filterAndFinallyShape.FinallyHandlers[finallyIndex], "            ");
		builder.AppendLine("        });");
		EmitNestedFinallyScopes(builder, method, filterAndFinallyShape, finallyIndex + 1, nextOffsetsByIlOffset, offsets);
		builder.AppendLine("    }");
	}

	private void EmitFilterAndFinallyTryCatchCore(StringBuilder builder, AotCoreIrMethodArtifact method, FilterAndFinallyExceptionMethodShape filterAndFinallyShape, IReadOnlyDictionary<int, int?> nextOffsetsByIlOffset, IReadOnlySet<int> offsets)
	{
		EmitFilterTryCatchCore(builder, method, filterAndFinallyShape.TryInstructions, filterAndFinallyShape.FilterInstructions, filterAndFinallyShape.HandlerInstructions, nextOffsetsByIlOffset, offsets, "        ");
	}

	private void EmitFilterTryCatchCore(StringBuilder builder, AotCoreIrMethodArtifact method, IReadOnlyList<AotCoreIrInstructionArtifact> tryInstructions, IReadOnlyList<AotCoreIrInstructionArtifact> filterInstructions, IReadOnlyList<AotCoreIrInstructionArtifact> handlerInstructions, IReadOnlyDictionary<int, int?> nextOffsetsByIlOffset, IReadOnlySet<int> offsets, string indentation)
	{
		StringBuilder stringBuilder = builder;
		StringBuilder stringBuilder2 = stringBuilder;
		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(3, 1, stringBuilder);
		handler.AppendFormatted(indentation);
		handler.AppendLiteral("try");
		stringBuilder2.AppendLine(ref handler);
		stringBuilder = builder;
		StringBuilder stringBuilder3 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(1, 1, stringBuilder);
		handler.AppendFormatted(indentation);
		handler.AppendLiteral("{");
		stringBuilder3.AppendLine(ref handler);
		stringBuilder = builder;
		StringBuilder stringBuilder4 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(19, 2, stringBuilder);
		handler.AppendFormatted(indentation);
		handler.AppendLiteral("    goto chaos_ip_");
		handler.AppendFormatted(GetRequiredIlOffset(tryInstructions[0]));
		handler.AppendLiteral(";");
		stringBuilder4.AppendLine(ref handler);
		builder.AppendLine();
		EmitStructuredInstructionRange(builder, method, tryInstructions, nextOffsetsByIlOffset, offsets);
		stringBuilder = builder;
		StringBuilder stringBuilder5 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(1, 1, stringBuilder);
		handler.AppendFormatted(indentation);
		handler.AppendLiteral("}");
		stringBuilder5.AppendLine(ref handler);
		stringBuilder = builder;
		StringBuilder stringBuilder6 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(54, 1, stringBuilder);
		handler.AppendFormatted(indentation);
		handler.AppendLiteral("catch (const chaos_managed_exception& chaos_exception)");
		stringBuilder6.AppendLine(ref handler);
		stringBuilder = builder;
		StringBuilder stringBuilder7 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(1, 1, stringBuilder);
		handler.AppendFormatted(indentation);
		handler.AppendLiteral("{");
		stringBuilder7.AppendLine(ref handler);
		stringBuilder = builder;
		StringBuilder stringBuilder8 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(71, 1, stringBuilder);
		handler.AppendFormatted(indentation);
		handler.AppendLiteral("    chaos_eval_stack[chaos_stack_top++] = chaos_exception.object_value;");
		stringBuilder8.AppendLine(ref handler);
		stringBuilder = builder;
		StringBuilder stringBuilder9 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(19, 2, stringBuilder);
		handler.AppendFormatted(indentation);
		handler.AppendLiteral("    goto chaos_ip_");
		handler.AppendFormatted(GetRequiredIlOffset(filterInstructions[0]));
		handler.AppendLiteral(";");
		stringBuilder9.AppendLine(ref handler);
		builder.AppendLine();
		EmitFilterInstructionRange(builder, method, filterInstructions, handlerInstructions, nextOffsetsByIlOffset, offsets);
		stringBuilder = builder;
		StringBuilder stringBuilder10 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(1, 1, stringBuilder);
		handler.AppendFormatted(indentation);
		handler.AppendLiteral("}");
		stringBuilder10.AppendLine(ref handler);
	}

	private void EmitFilterInstructionRange(StringBuilder builder, AotCoreIrMethodArtifact method, IReadOnlyList<AotCoreIrInstructionArtifact> filterInstructions, IReadOnlyList<AotCoreIrInstructionArtifact> handlerInstructions, IReadOnlyDictionary<int, int?> nextOffsetsByIlOffset, IReadOnlySet<int> offsets)
	{
		int requiredIlOffset = GetRequiredIlOffset(handlerInstructions[0]);
		foreach (AotCoreIrInstructionArtifact filterInstruction in filterInstructions)
		{
			int requiredIlOffset2 = GetRequiredIlOffset(filterInstruction);
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder2 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(10, 1, stringBuilder);
			handler.AppendLiteral("chaos_ip_");
			handler.AppendFormatted(requiredIlOffset2);
			handler.AppendLiteral(":");
			stringBuilder2.AppendLine(ref handler);
			if (string.Equals(filterInstruction.Op, "endfilter", StringComparison.Ordinal))
			{
				builder.AppendLine("    if (chaos_eval_stack[--chaos_stack_top] == static_cast<CHAOS_IL2CPP_INTPTR>(0))");
				builder.AppendLine("    {");
				builder.AppendLine("        throw;");
				builder.AppendLine("    }");
				builder.AppendLine("    chaos_eval_stack[chaos_stack_top++] = chaos_exception.object_value;");
				stringBuilder = builder;
				StringBuilder stringBuilder3 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(19, 1, stringBuilder);
				handler.AppendLiteral("    goto chaos_ip_");
				handler.AppendFormatted(requiredIlOffset);
				handler.AppendLiteral(";");
				stringBuilder3.AppendLine(ref handler);
			}
			else
			{
				EmitInstruction(builder, method, filterInstruction, nextOffsetsByIlOffset[requiredIlOffset2], offsets);
			}
			builder.AppendLine();
		}
		EmitStructuredInstructionRange(builder, method, handlerInstructions, nextOffsetsByIlOffset, offsets);
	}

	private void EmitLinearInstructionSequence(StringBuilder builder, IReadOnlyList<AotCoreIrInstructionArtifact> instructions, string indentation)
	{
		foreach (AotCoreIrInstructionArtifact instruction in instructions)
		{
			EmitLinearInstruction(builder, instruction, indentation);
		}
	}

	private void EmitStructuredFinallyHandlerSequence(StringBuilder builder, FinallyHandlerShape handlerShape, string indentation)
	{
		if (!TryCreateFinallyHandlerEmissionPlan(handlerShape, out FinallyHandlerEmissionPlan emissionPlan) || emissionPlan is null)
		{
			throw new NotSupportedException($"native-aot finally handler at IL offset {handlerShape.ExceptionRegion.HandlerOffset} does not support current structured emission shape");
		}
		if ((object)emissionPlan.Guard == null)
		{
			EmitLinearInstructionSequence(builder, emissionPlan.BodyInstructions, indentation);
			return;
		}
		EmitLinearInstructionSequence(builder, emissionPlan.Guard.ConditionInstructions, indentation);
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
		EmitLinearInstructionSequence(builder, emissionPlan.BodyInstructions, indentation + "        ");
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

	private void EmitLinearInstruction(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string indentation)
	{
		switch (instruction.Op)
		{
		case "ldc.i4":
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder12 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(67, 2, stringBuilder);
			handler.AppendFormatted(indentation);
			handler.AppendLiteral("chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(");
			handler.AppendFormatted(FormatInt32Literal(GetRequiredIntOperand(instruction)));
			handler.AppendLiteral(");");
			stringBuilder12.AppendLine(ref handler);
			break;
		}
		case "ldc.i8":
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder11 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(58, 2, stringBuilder);
			handler.AppendFormatted(indentation);
			handler.AppendLiteral("chaos_eval_stack[chaos_stack_top++] = chaos_store_int64(");
			handler.AppendFormatted(FormatInt64Literal(GetRequiredInt64Operand(instruction)));
			handler.AppendLiteral(");");
			stringBuilder11.AppendLine(ref handler);
			break;
		}
		case "ldarg":
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder10 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(51, 2, stringBuilder);
			handler.AppendFormatted(indentation);
			handler.AppendLiteral("chaos_eval_stack[chaos_stack_top++] = chaos_args[");
			handler.AppendFormatted(GetRequiredIntOperand(instruction));
			handler.AppendLiteral("];");
			stringBuilder10.AppendLine(ref handler);
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
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder9 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(53, 2, stringBuilder);
			handler.AppendFormatted(indentation);
			handler.AppendLiteral("chaos_eval_stack[chaos_stack_top++] = chaos_locals[");
			handler.AppendFormatted(GetRequiredIntOperand(instruction));
			handler.AppendLiteral("];");
			stringBuilder9.AppendLine(ref handler);
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
		case "stloc":
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder8 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(53, 2, stringBuilder);
			handler.AppendFormatted(indentation);
			handler.AppendLiteral("chaos_locals[");
			handler.AppendFormatted(GetRequiredIntOperand(instruction));
			handler.AppendLiteral("] = chaos_eval_stack[--chaos_stack_top];");
			stringBuilder8.AppendLine(ref handler);
			break;
		}
		case "pop":
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder8 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(31, 1, stringBuilder);
			handler.AppendFormatted(indentation);
			handler.AppendLiteral("chaos_stack_top--;");
			stringBuilder8.AppendLine(ref handler);
			break;
		}
		case "ldnull":
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder7 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(68, 1, stringBuilder);
			handler.AppendFormatted(indentation);
			handler.AppendLiteral("chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);");
			stringBuilder7.AppendLine(ref handler);
			break;
		}
		case "cgt.un":
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder2 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(1, 1, stringBuilder);
			handler.AppendFormatted(indentation);
			handler.AppendLiteral("{");
			stringBuilder2.AppendLine(ref handler);
			stringBuilder = builder;
			StringBuilder stringBuilder3 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(94, 1, stringBuilder);
			handler.AppendFormatted(indentation);
			handler.AppendLiteral("    const auto chaos_right = static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_eval_stack[--chaos_stack_top]);");
			stringBuilder3.AppendLine(ref handler);
			stringBuilder = builder;
			StringBuilder stringBuilder4 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(93, 1, stringBuilder);
			handler.AppendFormatted(indentation);
			handler.AppendLiteral("    const auto chaos_left = static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_eval_stack[--chaos_stack_top]);");
			stringBuilder4.AppendLine(ref handler);
			stringBuilder = builder;
			StringBuilder stringBuilder5 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(103, 1, stringBuilder);
			handler.AppendFormatted(indentation);
			handler.AppendLiteral("    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left > chaos_right ? 1 : 0);");
			stringBuilder5.AppendLine(ref handler);
			stringBuilder = builder;
			StringBuilder stringBuilder6 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(1, 1, stringBuilder);
			handler.AppendFormatted(indentation);
			handler.AppendLiteral("}");
			stringBuilder6.AppendLine(ref handler);
			break;
		}
		case "ceq":
			EmitLinearComparisonResult(builder, instruction, indentation, "CHAOS_IL2CPP_INTPTR", "==");
			break;
		case "cgt":
			EmitLinearComparisonResult(builder, instruction, indentation, "CHAOS_IL2CPP_INT32", ">");
			break;
		case "clt":
			EmitLinearComparisonResult(builder, instruction, indentation, "CHAOS_IL2CPP_INT32", "<");
			break;
		case "add":
			EmitLinearBinaryArithmetic(builder, indentation, "chaos_wrap_add");
			break;
		case "sub":
			EmitLinearBinaryArithmetic(builder, indentation, "chaos_wrap_sub");
			break;
		case "mul":
			EmitLinearBinaryArithmetic(builder, indentation, "chaos_wrap_mul");
			break;
		case "div":
			EmitLinearBinaryArithmetic(builder, indentation, "chaos_div");
			break;
		case "rem":
			EmitLinearBinaryArithmetic(builder, indentation, "chaos_rem");
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
		default:
			throw new NotSupportedException("native-aot structured EH linear lowering does not support opcode '" + instruction.Op + "'.");
		}
	}

	private void EmitLinearCall(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string indentation)
	{
		InvocationTarget invocationTarget = ResolveDirectInvocationTarget(instruction);
		EmitLinearResolvedInvocation(builder, invocationTarget.TargetSymbol, invocationTarget.ParameterAbis, invocationTarget.ReturnAbi, invocationTarget.RawArgumentIndices, indentation, enforceInstanceNullCheck: false);
	}

	private void EmitLinearCallVirt(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string indentation)
	{
		if (IsDelegateInvokeInstruction(instruction))
		{
			throw new NotSupportedException("native-aot structured EH linear lowering does not support delegate callvirt.");
		}
		switch (instruction.DispatchKindCode.GetValueOrDefault())
		{
		case HybridDispatchKind.None:
		case HybridDispatchKind.Direct:
		case HybridDispatchKind.ExternalRuntime:
		{
			InvocationTarget invocationTarget = ResolveDirectInvocationTarget(instruction);
			EmitLinearResolvedInvocation(builder, invocationTarget.TargetSymbol, invocationTarget.ParameterAbis, invocationTarget.ReturnAbi, invocationTarget.RawArgumentIndices, indentation, enforceInstanceNullCheck: true);
			return;
		}
		default:
			throw new NotSupportedException($"native-aot structured EH linear lowering does not support callvirt dispatch kind '{instruction.DispatchKindCode}'.");
		}
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
			builder.AppendLine(indentation + "    const auto chaos_method_ptr = chaos_eval_stack[--chaos_stack_top];");
			builder.AppendLine(indentation + "    const auto chaos_target = chaos_eval_stack[--chaos_stack_top];");
			builder.AppendLine($"{indentation}    auto* chaos_object = new {GetNativeTypeSymbol(requiredTargetReference.SubjectId)}{{}};");
			builder.AppendLine($"{indentation}    chaos_object->header.type_info = &{GetNativeTypeInfoSymbol(requiredTargetReference.SubjectId)};");
			if (_vtableTypes?.Contains(requiredTargetReference.SubjectId) == true)
			{
				builder.AppendLine($"{indentation}    chaos_object->header.vtable = {GetNativeVTableSymbol(requiredTargetReference.SubjectId)};");
			}
			builder.AppendLine(indentation + "    chaos_object->chaos_delegate_target = chaos_target;");
			builder.AppendLine(indentation + "    chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;");
			builder.AppendLine(indentation + "    chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);");
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
				builder.AppendLine($"{indentation}    const auto chaos_raw_arg_{num} = chaos_eval_stack[--chaos_stack_top];");
				builder.AppendLine(invocationTarget.RawArgumentIndices.Contains(num)
					? $"{indentation}    const auto chaos_arg_{num} = chaos_raw_arg_{num};"
					: $"{indentation}    const auto chaos_arg_{num} = {FormatInboundAbiArgumentExpression(invocationTarget.ParameterAbis[num], $"chaos_raw_arg_{num}")};");
			}
			builder.AppendLine(indentation + "    CHAOS_IL2CPP_INTPTR chaos_value = static_cast<CHAOS_IL2CPP_INTPTR>(0);");
			builder.AppendLine(indentation + "    const auto chaos_arg_0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_value) | chaos_managed_pointer_local_slot_tag;");
			builder.AppendLine($"{indentation}    {invocationTarget.TargetSymbol}({FormatAbiInvocationArgumentList(invocationTarget.ParameterAbis)});");
			builder.AppendLine(indentation + "    chaos_eval_stack[chaos_stack_top++] = chaos_value;");
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
				builder.AppendLine($"{indentation}    const auto chaos_raw_arg_{num2} = chaos_eval_stack[--chaos_stack_top];");
				builder.AppendLine(constructorTarget.RawArgumentIndices.Contains(num2)
					? $"{indentation}    const auto chaos_arg_{num2} = chaos_raw_arg_{num2};"
					: $"{indentation}    const auto chaos_arg_{num2} = {FormatInboundAbiArgumentExpression(constructorTarget.ParameterAbis[num2], $"chaos_raw_arg_{num2}")};");
			}
			builder.AppendLine($"{indentation}    auto* chaos_object = new {GetNativeTypeSymbol(requiredTargetReference.SubjectId)}{{}};");
			builder.AppendLine($"{indentation}    chaos_object->header.type_info = &{GetNativeTypeInfoSymbol(requiredTargetReference.SubjectId)};");
			if (_vtableTypes?.Contains(requiredTargetReference.SubjectId) == true)
			{
				builder.AppendLine($"{indentation}    chaos_object->header.vtable = {GetNativeVTableSymbol(requiredTargetReference.SubjectId)};");
			}
			builder.AppendLine(indentation + "    const auto chaos_arg_0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);");
			builder.AppendLine($"{indentation}    {constructorTarget.TargetSymbol}({FormatAbiInvocationArgumentList(constructorTarget.ParameterAbis)});");
			builder.AppendLine(indentation + "    chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);");
			builder.AppendLine(indentation + "}");
			return;
		}
		builder.AppendLine(indentation + "{");
		builder.AppendLine($"{indentation}    auto* chaos_object = new {GetNativeTypeSymbol(requiredTargetReference.SubjectId)}{{}};");
		builder.AppendLine($"{indentation}    chaos_object->header.type_info = &{GetNativeTypeInfoSymbol(requiredTargetReference.SubjectId)};");
		if (_vtableTypes?.Contains(requiredTargetReference.SubjectId) == true)
		{
			builder.AppendLine($"{indentation}    chaos_object->header.vtable = {GetNativeVTableSymbol(requiredTargetReference.SubjectId)};");
		}
		builder.AppendLine(indentation + "    chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);");
		builder.AppendLine(indentation + "}");
	}

	private static void EmitLinearResolvedInvocation(StringBuilder builder, string targetSymbol, IReadOnlyList<AotCoreIrAbiSlotArtifact> parameterAbis, AotCoreIrAbiSlotArtifact returnAbi, IReadOnlySet<int> rawArgumentIndices, string indentation, bool enforceInstanceNullCheck)
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
			stringBuilder = builder;
			StringBuilder stringBuilder3 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(68, 2, stringBuilder);
			handler.AppendFormatted(indentation);
			handler.AppendLiteral("    const auto chaos_raw_arg_");
			handler.AppendFormatted(num);
			handler.AppendLiteral(" = chaos_eval_stack[--chaos_stack_top];");
			stringBuilder3.AppendLine(ref handler);
			builder.AppendLine(rawArgumentIndices.Contains(num) ? $"{indentation}    const auto chaos_arg_{num} = chaos_raw_arg_{num};" : $"{indentation}    const auto chaos_arg_{num} = {FormatInboundAbiArgumentExpression(parameterAbis[num], $"chaos_raw_arg_{num}")};");
		}
		if (enforceInstanceNullCheck && parameterAbis.Count > 0)
		{
			builder.AppendLine(indentation + "    if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))");
			builder.AppendLine(indentation + "    {");
			builder.AppendLine(indentation + "        CHAOS_IL2CPP_ABORT();");
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

	private static void EmitLinearBinaryArithmetic(StringBuilder builder, string indentation, string helperName)
	{
		StringBuilder stringBuilder = builder;
		StringBuilder stringBuilder2 = stringBuilder;
		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(1, 1, stringBuilder);
		handler.AppendFormatted(indentation);
		handler.AppendLiteral("{");
		stringBuilder2.AppendLine(ref handler);
		stringBuilder = builder;
		StringBuilder stringBuilder3 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(92, 1, stringBuilder);
		handler.AppendFormatted(indentation);
		handler.AppendLiteral("    const auto chaos_right = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);");
		stringBuilder3.AppendLine(ref handler);
		stringBuilder = builder;
		StringBuilder stringBuilder4 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(91, 1, stringBuilder);
		handler.AppendFormatted(indentation);
		handler.AppendLiteral("    const auto chaos_left = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);");
		stringBuilder4.AppendLine(ref handler);
		stringBuilder = builder;
		StringBuilder stringBuilder5 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(96, 2, stringBuilder);
		handler.AppendFormatted(indentation);
		handler.AppendLiteral("    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(");
		handler.AppendFormatted(helperName);
		handler.AppendLiteral("(chaos_left, chaos_right));");
		stringBuilder5.AppendLine(ref handler);
		stringBuilder = builder;
		StringBuilder stringBuilder6 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(1, 1, stringBuilder);
		handler.AppendFormatted(indentation);
		handler.AppendLiteral("}");
		stringBuilder6.AppendLine(ref handler);
	}

	private static void EmitLinearBinaryBitwise(StringBuilder builder, string indentation, string operation)
	{
		StringBuilder stringBuilder = builder;
		StringBuilder stringBuilder2 = stringBuilder;
		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(1, 1, stringBuilder);
		handler.AppendFormatted(indentation);
		handler.AppendLiteral("{");
		stringBuilder2.AppendLine(ref handler);
		stringBuilder = builder;
		StringBuilder stringBuilder3 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(84, 1, stringBuilder);
		handler.AppendFormatted(indentation);
		handler.AppendLiteral("    const auto chaos_right = chaos_load_uint64(chaos_eval_stack[--chaos_stack_top]);");
		stringBuilder3.AppendLine(ref handler);
		stringBuilder = builder;
		StringBuilder stringBuilder4 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(83, 1, stringBuilder);
		handler.AppendFormatted(indentation);
		handler.AppendLiteral("    const auto chaos_left = chaos_load_uint64(chaos_eval_stack[--chaos_stack_top]);");
		stringBuilder4.AppendLine(ref handler);
		stringBuilder = builder;
		StringBuilder stringBuilder5 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(86, 2, stringBuilder);
		handler.AppendFormatted(indentation);
		handler.AppendLiteral("    chaos_eval_stack[chaos_stack_top++] = chaos_store_uint64(chaos_left ");
		handler.AppendFormatted(operation);
		handler.AppendLiteral(" chaos_right);");
		stringBuilder5.AppendLine(ref handler);
		stringBuilder = builder;
		StringBuilder stringBuilder6 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(1, 1, stringBuilder);
		handler.AppendFormatted(indentation);
		handler.AppendLiteral("}");
		stringBuilder6.AppendLine(ref handler);
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
			StringBuilder stringBuilder2 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(39, 2, stringBuilder);
			handler.AppendFormatted(indentation);
			handler.AppendLiteral("chaos_eval_stack[chaos_stack_top++] = ");
			handler.AppendFormatted(GetNativeStaticFieldSymbol(requiredTargetReference.SubjectId));
			handler.AppendLiteral(";");
			stringBuilder2.AppendLine(ref handler);
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
			stringBuilder = builder;
			StringBuilder stringBuilder4 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(99, 2, stringBuilder);
			handler.AppendFormatted(indentation);
			handler.AppendLiteral("    auto* chaos_value = chaos_resolve_managed_value_pointer<");
			handler.AppendFormatted(GetNativeValueTypeSymbol(requiredDeclaringTypeSubjectId));
			handler.AppendLiteral(">(chaos_eval_stack[--chaos_stack_top]);");
			stringBuilder4.AppendLine(ref handler);
			stringBuilder = builder;
			StringBuilder stringBuilder5 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(56, 2, stringBuilder);
			handler.AppendFormatted(indentation);
			handler.AppendLiteral("    chaos_eval_stack[chaos_stack_top++] = chaos_value->");
			handler.AppendFormatted(GetNativeFieldMemberName(requiredTargetReference.SubjectId));
			handler.AppendLiteral(";");
			stringBuilder5.AppendLine(ref handler);
		}
		else
		{
			string requiredDeclaringTypeSubjectId2 = GetRequiredDeclaringTypeSubjectId(requiredTargetReference);
			stringBuilder = builder;
			StringBuilder stringBuilder6 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(82, 2, stringBuilder);
			handler.AppendFormatted(indentation);
			handler.AppendLiteral("    auto* chaos_object = reinterpret_cast<");
			handler.AppendFormatted(GetNativeTypeSymbol(requiredDeclaringTypeSubjectId2));
			handler.AppendLiteral("*>(chaos_eval_stack[--chaos_stack_top]);");
			stringBuilder6.AppendLine(ref handler);
			stringBuilder = builder;
			StringBuilder stringBuilder7 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(57, 2, stringBuilder);
			handler.AppendFormatted(indentation);
			handler.AppendLiteral("    chaos_eval_stack[chaos_stack_top++] = chaos_object->");
			handler.AppendFormatted(GetNativeFieldMemberName(requiredTargetReference.SubjectId));
			handler.AppendLiteral(";");
			stringBuilder7.AppendLine(ref handler);
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
		StringBuilder stringBuilder = builder;
		StringBuilder stringBuilder2 = stringBuilder;
		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(1, 1, stringBuilder);
		handler.AppendFormatted(indentation);
		handler.AppendLiteral("{");
		stringBuilder2.AppendLine(ref handler);
		stringBuilder = builder;
		StringBuilder stringBuilder3 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(32, 2, stringBuilder);
		handler.AppendFormatted(indentation);
		handler.AppendLiteral("    auto* chaos_string = new ");
		handler.AppendFormatted(GetNativeTypeSymbol("System.Private.CoreLib/System.String"));
		handler.AppendLiteral("{};");
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
		stringBuilder = builder;
		StringBuilder stringBuilder7 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(88, 1, stringBuilder);
		handler.AppendFormatted(indentation);
		handler.AppendLiteral("    chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_string);");
		stringBuilder7.AppendLine(ref handler);
		stringBuilder = builder;
		StringBuilder stringBuilder8 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(1, 1, stringBuilder);
		handler.AppendFormatted(indentation);
		handler.AppendLiteral("}");
		stringBuilder8.AppendLine(ref handler);
	}

	private static void EmitLinearLoadTypeToken(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string indentation)
	{
		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(39, 2, builder);
		handler.AppendFormatted(indentation);
		handler.AppendLiteral("chaos_eval_stack[chaos_stack_top++] = ");
		handler.AppendFormatted(GetRequiredTypeHandleLiteral(instruction));
		handler.AppendLiteral(";");
		builder.AppendLine(ref handler);
	}

	private static void EmitLinearShift(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string indentation, string helperName)
	{
		RequireInt32IntegralResultType(instruction);
		StringBuilder stringBuilder = builder;
		StringBuilder stringBuilder2 = stringBuilder;
		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(1, 1, stringBuilder);
		handler.AppendFormatted(indentation);
		handler.AppendLiteral("{");
		stringBuilder2.AppendLine(ref handler);
		stringBuilder = builder;
		StringBuilder stringBuilder3 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(92, 1, stringBuilder);
		handler.AppendFormatted(indentation);
		handler.AppendLiteral("    const auto chaos_shift = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);");
		stringBuilder3.AppendLine(ref handler);
		stringBuilder = builder;
		StringBuilder stringBuilder4 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(92, 1, stringBuilder);
		handler.AppendFormatted(indentation);
		handler.AppendLiteral("    const auto chaos_value = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);");
		stringBuilder4.AppendLine(ref handler);
		stringBuilder = builder;
		StringBuilder stringBuilder5 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(97, 2, stringBuilder);
		handler.AppendFormatted(indentation);
		handler.AppendLiteral("    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(");
		handler.AppendFormatted(helperName);
		handler.AppendLiteral("(chaos_value, chaos_shift));");
		stringBuilder5.AppendLine(ref handler);
		stringBuilder = builder;
		StringBuilder stringBuilder6 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(1, 1, stringBuilder);
		handler.AppendFormatted(indentation);
		handler.AppendLiteral("}");
		stringBuilder6.AppendLine(ref handler);
	}

	private static void EmitLinearBitwiseNot(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string indentation)
	{
		RequireInt32IntegralResultType(instruction);
		StringBuilder stringBuilder = builder;
		StringBuilder stringBuilder2 = stringBuilder;
		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(1, 1, stringBuilder);
		handler.AppendFormatted(indentation);
		handler.AppendLiteral("{");
		stringBuilder2.AppendLine(ref handler);
		stringBuilder = builder;
		StringBuilder stringBuilder3 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(94, 1, stringBuilder);
		handler.AppendFormatted(indentation);
		handler.AppendLiteral("    const auto chaos_value = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[chaos_stack_top - 1]);");
		stringBuilder3.AppendLine(ref handler);
		stringBuilder = builder;
		StringBuilder stringBuilder4 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(85, 1, stringBuilder);
		handler.AppendFormatted(indentation);
		handler.AppendLiteral("    chaos_eval_stack[chaos_stack_top - 1] = static_cast<CHAOS_IL2CPP_INTPTR>(~chaos_value);");
		stringBuilder4.AppendLine(ref handler);
		stringBuilder = builder;
		StringBuilder stringBuilder5 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(1, 1, stringBuilder);
		handler.AppendFormatted(indentation);
		handler.AppendLiteral("}");
		stringBuilder5.AppendLine(ref handler);
	}

	private static void EmitLinearComparisonResult(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string indentation, string operandType, string comparisonOperator)
	{
		RequireInt32IntegralResultType(instruction);
		StringBuilder stringBuilder = builder;
		StringBuilder stringBuilder2 = stringBuilder;
		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(1, 1, stringBuilder);
		handler.AppendFormatted(indentation);
		handler.AppendLiteral("{");
		stringBuilder2.AppendLine(ref handler);
		stringBuilder = builder;
		StringBuilder stringBuilder3 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(80, 2, stringBuilder);
		handler.AppendFormatted(indentation);
		handler.AppendLiteral("    const auto chaos_right = static_cast<");
		handler.AppendFormatted(operandType);
		handler.AppendLiteral(">(chaos_eval_stack[--chaos_stack_top]);");
		stringBuilder3.AppendLine(ref handler);
		stringBuilder = builder;
		StringBuilder stringBuilder4 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(79, 2, stringBuilder);
		handler.AppendFormatted(indentation);
		handler.AppendLiteral("    const auto chaos_left = static_cast<");
		handler.AppendFormatted(operandType);
		handler.AppendLiteral(">(chaos_eval_stack[--chaos_stack_top]);");
		stringBuilder4.AppendLine(ref handler);
		stringBuilder = builder;
		StringBuilder stringBuilder5 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(102, 2, stringBuilder);
		handler.AppendFormatted(indentation);
		handler.AppendLiteral("    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left ");
		handler.AppendFormatted(comparisonOperator);
		handler.AppendLiteral(" chaos_right ? 1 : 0);");
		stringBuilder5.AppendLine(ref handler);
		stringBuilder = builder;
		StringBuilder stringBuilder6 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(1, 1, stringBuilder);
		handler.AppendFormatted(indentation);
		handler.AppendLiteral("}");
		stringBuilder6.AppendLine(ref handler);
	}

	private static IReadOnlyDictionary<int, int?> CreateNextOffsets(IReadOnlyList<AotCoreIrInstructionArtifact> instructions)
	{
		Dictionary<int, int?> dictionary = new Dictionary<int, int?>(instructions.Count);
		for (int i = 0; i < instructions.Count; i++)
		{
			int requiredIlOffset = GetRequiredIlOffset(instructions[i]);
			dictionary[requiredIlOffset] = ((i + 1 < instructions.Count) ? new int?(GetRequiredIlOffset(instructions[i + 1])) : ((int?)null));
		}
		return dictionary;
	}

	private static bool TryCreateCatchOnlyExceptionMethodShape(AotCoreIrMethodArtifact method, out CatchOnlyExceptionMethodShape? catchOnlyShape)
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

	private static bool TryCreateFilterOnlyExceptionMethodShape(AotCoreIrMethodArtifact method, out FilterOnlyExceptionMethodShape? filterOnlyShape)
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

	private static bool TryCreateFinallyOnlyExceptionMethodShape(AotCoreIrMethodArtifact method, out FinallyOnlyExceptionMethodShape? finallyOnlyShape)
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
		if (array5.Any((AotCoreIrInstructionArtifact instruction) => string.Equals(instruction.Op, "leave", StringComparison.Ordinal)) || !DoBranchTargetsStayWithinAllowedOffsets(array5, allOffsets, hashSet) || array6[..^1].Any((AotCoreIrInstructionArtifact instruction) => IsUnsupportedStructuredExceptionControlFlow(instruction.Op) && !string.Equals(instruction.Op, "leave", StringComparison.Ordinal)) || !DoBranchTargetsStayWithinAllowedOffsets(array6.Where((AotCoreIrInstructionArtifact instruction) => !string.Equals(instruction.Op, "leave", StringComparison.Ordinal)).ToArray(), allOffsets, allowedTargets) || list.Any((FinallyHandlerShape shape) => !TryCreateFinallyHandlerEmissionPlan(shape, out FinallyHandlerEmissionPlan _)) || !DoBranchTargetsStayWithinAllowedOffsets(array7, allOffsets, hashSet) || array7.Any((AotCoreIrInstructionArtifact instruction) => string.Equals(instruction.Op, "leave", StringComparison.Ordinal)))
		{
			return false;
		}
		finallyOnlyShape = new FinallyOnlyExceptionMethodShape(array5, array6, list, array7);
		return true;
	}

	private static bool TryCreateCatchAndFinallyExceptionMethodShape(AotCoreIrMethodArtifact method, out CatchAndFinallyExceptionMethodShape? catchAndFinallyShape)
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
		if (array9.Any((AotCoreIrInstructionArtifact instruction) => !IsStructuredEhLinearInstructionSupported(instruction.Op)) || array10.Any((AotCoreIrInstructionArtifact instruction) => !IsStructuredEhLinearInstructionSupported(instruction.Op)) || ((object)finallyHandlerShape != null && !TryCreateFinallyHandlerEmissionPlan(finallyHandlerShape, out FinallyHandlerEmissionPlan _)) || list.Any((FinallyHandlerShape shape) => !TryCreateFinallyHandlerEmissionPlan(shape, out FinallyHandlerEmissionPlan _)) || array13[1..^1].Any((AotCoreIrInstructionArtifact instruction) => IsUnsupportedStructuredExceptionControlFlow(instruction.Op)) || array14.Any((AotCoreIrInstructionArtifact instruction) => string.Equals(instruction.Op, "leave", StringComparison.Ordinal)))
		{
			return false;
		}
		catchAndFinallyShape = new CatchAndFinallyExceptionMethodShape(catchRegion, array9, array10, array11, finallyHandlerShape, array12, array13, list.AsEnumerable().Reverse().ToArray(), array14);
		return true;
	}

	private static bool TryCreateFilterAndFinallyExceptionMethodShape(AotCoreIrMethodArtifact method, out FilterAndFinallyExceptionMethodShape? filterAndFinallyShape)
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
		if (array4.Any((AotCoreIrInstructionArtifact instruction) => !IsStructuredEhLinearInstructionSupported(instruction.Op)) || source.Any((FinallyHandlerShape shape) => !TryCreateFinallyHandlerEmissionPlan(shape, out FinallyHandlerEmissionPlan _)))
		{
			return false;
		}
		filterAndFinallyShape = new FilterAndFinallyExceptionMethodShape(filterRegion, array4, array5, array6, array7, source.Reverse().ToArray(), array8);
		return true;
	}

	private static bool TryCreateFinallyHandlerEmissionPlan(FinallyHandlerShape handlerShape, out FinallyHandlerEmissionPlan? emissionPlan)
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
		case "or":
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
		object operand = instruction.Operand;
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
		object operand = instruction.Operand;
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
		object operand = instruction.Operand;
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
		object operand = instruction.Operand;
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
		object operand = instruction.Operand;
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
}
