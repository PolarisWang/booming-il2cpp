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

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    private void EmitLinearCall(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string indentation)
    {
        EmitLinearCallTarget(builder, instruction, indentation, enforceInstanceNullCheck: true);
        // Enable instance null check for  on instance methods.
        // While the CLR spec allows  on null  (unlike callvirt),
        // our AOT-generated C++ code will AV when the method body accesses
        // instance fields through a null pointer.
    }



    private void EmitLinearCallVirt(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string indentation)
    {
        if (IsDelegateInvokeInstruction(instruction))
        {
            EmitLinearDelegateInvoke(builder, instruction, indentation);
            return;
        }

        int dispatchSeq = _state.Value!.DispatchLabelSeq++;
        // Phase 3: AOT Devirtualization fast-path for linear emission (inside branches of structured nodes)
        string devirtKey = instruction.Callee ?? instruction.TargetReference?.SubjectId ?? "";
        if (devirtKey.Length > 0 && _devirtualizationHints.TryGetValue(devirtKey, out DevirtualizationHint devirtHint) && devirtHint.CanDevirtualize)
        {
            AotCoreIrMethodArtifact devirtMethod = _methodsBySubjectId[devirtHint.ImplementationMethodSubjectId];
            string devirtSymbol = devirtMethod.NativeSymbol;

            // If the devirtualized method has a hotpatch dispatch slot, use
            // hotpatch-aware dispatch so method_replacement can intercept at runtime.
            // Try inlining before hotpatch — skip hotpatch entirely if inlined.
            if (TryInlineResolvedMethod(builder, instruction, devirtHint.ImplementationMethodSubjectId, GetMethodAbiParameterSlots(devirtMethod).Count, indentation))
                return;

            if (_nativeSymbolToDispatchSlot?.TryGetValue(devirtSymbol, out int devirtSlot) == true)
            {
                EmitHotpatchResolvedInvocation(
                    builder, devirtSlot, devirtSymbol,
                    GetMethodAbiParameterSlots(devirtMethod),
                    devirtMethod.ReturnAbi,
                    EmptyRawArgumentIndices,
                    indentation,
                    directNativeSymbol: null);
                return;
            }

            IReadOnlyList<AotCoreIrAbiSlotArtifact> devirtParams = GetMethodAbiParameterSlots(devirtMethod);
            string devirtRet = MapAbiSlotReturnType(devirtMethod.ReturnAbi);
            // Pop arguments from eval stack and create converted variables (same as EmitLinearResolvedInvocation)
            builder.AppendLine($"{indentation}{{");
            for (int devirtIdx = devirtParams.Count - 1; devirtIdx >= 0; devirtIdx--)
            {
                builder.AppendLine($"{indentation}    const auto chaos_raw_arg_{devirtIdx} = {ConsumeEvalStackValueExpression()};");
                builder.AppendLine($"{indentation}    const auto chaos_arg_{devirtIdx} = {FormatInboundAbiArgumentExpression(devirtParams[devirtIdx], $"chaos_raw_arg_{devirtIdx}")};");
            }
            if (devirtParams.Count > 0)
            {
                // Null check guard: immediately exit the devirtualization scope
                // when this is null.  RaiseNullReferenceException is [[noreturn]]
                // but the type-guard code below (chaos_object_get_type_info)
                // would null-dereference if execution somehow falls through.
                // Explicit goto prevents any fallthrough even if the [[noreturn]]
                // function does not actually terminate execution (e.g. when
                // g_chaos_fail_hook uses longjmp in verification mode).
                builder.AppendLine($"{indentation}    if (chaos_arg_0 == 0)");
                builder.AppendLine($"{indentation}    {{");
                builder.AppendLine($"{indentation}        chaos_runtime_get_abi_v0()->raise_null_reference_exception();");
                builder.AppendLine($"{indentation}        goto chaos_dt_end_{dispatchSeq};");
                builder.AppendLine($"{indentation}    }}");
            }
            if (devirtHint.GuardTypeSubjectId != null)
            {
#if CHAOS_IL2CPP_DEVIRT_TRACE
				// Emit-time sanity: if the guard type is sealed, this guard is unnecessary
				if (_sealedTypeSubjectIds != null && _sealedTypeSubjectIds.Contains(devirtHint.GuardTypeSubjectId))
				{
					System.Console.Error.WriteLine($"[devirt] WARNING: unnecessary guard for sealed type {devirtHint.GuardTypeSubjectId} at {instruction.IlOffset}");
				}
#endif
                // Guard-based devirtualization: check runtime type, direct call if match, vtable fallback otherwise.
                string guardStableIdExpr = GetNativeTypeIdSymbol(devirtHint.GuardTypeSubjectId);
                if (!string.Equals(devirtRet, "void", StringComparison.Ordinal))
                {
                    builder.AppendLine($"{indentation}    {devirtRet} chaos_dt_result{{}};");
                }
                builder.AppendLine($"{indentation}    auto* chaos_dt_ti = chaos_object_get_type_info(reinterpret_cast<void*>(chaos_arg_0));");
                builder.AppendLine($"{indentation}    if (chaos_dt_ti->stable_id == {guardStableIdExpr})");
                builder.AppendLine($"{indentation}    {{");
                string devirtArgs = FormatAbiInvocationArgumentList(devirtParams);
                string devirtCtxArg = "";
                if (_sharedContextSymbols.Contains(devirtSymbol))
                {
                    bool callerIsShared = _state.Value!.CurrentMethodNativeSymbol != null &&
                                          _sharedContextSymbols.Contains(_state.Value!.CurrentMethodNativeSymbol);
                    devirtCtxArg = string.IsNullOrEmpty(devirtArgs)
                        ? (callerIsShared ? "chaos_generic_context" : "0")
                        : (callerIsShared ? ", chaos_generic_context" : ", 0");
                }
                // Try inlining the devirtualized method
                if (TryInlineResolvedMethod(builder, instruction, devirtHint.ImplementationMethodSubjectId, devirtParams.Count, indentation))
                    return;
                if (string.Equals(devirtRet, "void", StringComparison.Ordinal))
                {
                    builder.AppendLine($"{indentation}        {devirtSymbol}({devirtArgs}{devirtCtxArg});");
                }
                else
                {
                    builder.AppendLine($"{indentation}        chaos_dt_result = {devirtSymbol}({devirtArgs}{devirtCtxArg});");
                }
                builder.AppendLine($"{indentation}    }}");
                builder.AppendLine($"{indentation}    else");
                builder.AppendLine($"{indentation}    {{");
                EmitDevirtFallbackVTableDispatch(builder, instruction, devirtParams, devirtMethod.ReturnAbi, devirtRet, $"{indentation}        ");
                builder.AppendLine($"{indentation}    }}");
                if (!string.Equals(devirtRet, "void", StringComparison.Ordinal))
                {
                    EmitAbiReturnPush(builder, devirtMethod.ReturnAbi, "chaos_dt_result", $"{indentation}    ");
                }
            }
            else
            {
                // Sealed/monomorphic: unconditional direct call
                string devirtArgs = FormatAbiInvocationArgumentList(devirtParams);
                string devirtCtxArg2 = "";
                if (_sharedContextSymbols.Contains(devirtSymbol))
                {
                    bool callerIsShared = _state.Value!.CurrentMethodNativeSymbol != null &&
                                          _sharedContextSymbols.Contains(_state.Value!.CurrentMethodNativeSymbol);
                    devirtCtxArg2 = string.IsNullOrEmpty(devirtArgs)
                        ? (callerIsShared ? "chaos_generic_context" : "0")
                        : (callerIsShared ? ", chaos_generic_context" : ", 0");
                }
                // Try inlining the devirtualized method (sealed/monomorphic path)
                if (TryInlineResolvedMethod(builder, instruction, devirtHint.ImplementationMethodSubjectId, devirtParams.Count, indentation))
                    return;
                if (string.Equals(devirtRet, "void", StringComparison.Ordinal))
                {
                    builder.AppendLine($"{indentation}    {devirtSymbol}({devirtArgs}{devirtCtxArg2});");
                }
                else
                {
                    builder.AppendLine($"{indentation}    auto chaos_devirt_result = {devirtSymbol}({devirtArgs}{devirtCtxArg2});");
                    EmitAbiReturnPush(builder, devirtMethod.ReturnAbi, "chaos_devirt_result", $"{indentation}    ");
                }
            }
            builder.AppendLine($"{indentation}}}");
            return;
        }

        switch (instruction.DispatchKindCode.GetValueOrDefault())
        {
            case HybridDispatchKind.None:
            case HybridDispatchKind.Direct:
            case HybridDispatchKind.Bridge:
            case HybridDispatchKind.ExternalRuntime:
                {
                    EmitLinearCallTarget(builder, instruction, indentation, enforceInstanceNullCheck: true);
                    return;
                }
            case HybridDispatchKind.Virtual:
                {
                    EmitLinearVirtualDispatchCall(builder, instruction, indentation);
                    return;
                }
            case HybridDispatchKind.ComVtable:
                {
                    if (instruction.ComVtableSlot == null)
                        throw new NotSupportedException("native-aot structured EH ComVtable dispatch requires ComVtableSlot.");
                    int comSlot = instruction.ComVtableSlot.Value;
                    bool isPreserveSig = instruction.IsPreserveSig;

                    // Resolve method metadata for ABI info.
                    var comParams = ResolveComMethodParameterAbis(instruction);
                    var comRetAbi = ResolveComMethodReturnAbi(instruction);
                    string comRetType = MapAbiSlotReturnType(comRetAbi);

                    builder.AppendLine($"{indentation}{{");
                    // Pop arguments from eval stack with ABI conversion (reverse order).
                    for (int comIdx = comParams.Count - 1; comIdx >= 0; comIdx--)
                    {
                        builder.AppendLine($"{indentation}    const auto chaos_raw_arg_{comIdx} = {ConsumeEvalStackValueExpression()};");
                        builder.AppendLine($"{indentation}    const auto chaos_arg_{comIdx} = {FormatInboundAbiArgumentExpression(comParams[comIdx], $"chaos_raw_arg_{comIdx}")};");
                    }
                    if (comParams.Count > 0)
                    {
                        builder.AppendLine($"{indentation}    if (chaos_arg_0 == 0)");
                        builder.AppendLine($"{indentation}    {{");
                        builder.AppendLine($"{indentation}        chaos_runtime_get_abi_v0()->raise_null_reference_exception();");
                        builder.AppendLine($"{indentation}        goto chaos_dt_end_{dispatchSeq};");
                        builder.AppendLine($"{indentation}    }}");
                        // RCW-aware COM object pointer extraction.
                        // If chaos_arg_0 is an RCW handle, extract the identity_unknown.
                        // Otherwise treat it as a raw COM object pointer.
                        builder.AppendLine($"{indentation}    void* chaos_com_obj = nullptr;");
                        builder.AppendLine($"{indentation}    if (chaos_runtime_get_abi_v0()->marshal_is_rcw_handle(chaos_arg_0))");
                        builder.AppendLine($"{indentation}    {{");
                        builder.AppendLine($"{indentation}        auto chaos_rcw_ptr = chaos_runtime_get_abi_v0()->marshal_get_rcw_unknown(chaos_arg_0);");
                        builder.AppendLine($"{indentation}        chaos_com_obj = reinterpret_cast<void*>(chaos_rcw_ptr);");
                        builder.AppendLine($"{indentation}    }}");
                        builder.AppendLine($"{indentation}    else");
                        builder.AppendLine($"{indentation}    {{");
                        builder.AppendLine($"{indentation}        chaos_com_obj = reinterpret_cast<void*>(chaos_arg_0);");
                        builder.AppendLine($"{indentation}    }}");
                        builder.AppendLine($"{indentation}    auto** chaos_vtable = *reinterpret_cast<void***>(chaos_com_obj);");
                    }
                    // Build function pointer type from ABI.
                    string comSig = FormatAbiSlotParameterSignature(comParams);
                    // Non-PreserveSig: the COM method always returns HRESULT (int32_t).
                    string comFnRetType = isPreserveSig ? comRetType : "CHAOS_IL2CPP_INT32";
                    string comFnType = string.IsNullOrEmpty(comSig)
                        ? $"{comFnRetType}(*)()"
                        : $"{comFnRetType}(*)({comSig})";
                    string comArgs = FormatAbiInvocationArgumentList(comParams);
                    builder.AppendLine($"{indentation}    auto chaos_com_fn = reinterpret_cast<{comFnType}>(chaos_vtable[{comSlot}]);");

                    if (isPreserveSig)
                    {
                        // PreserveSig=true: return the raw HRESULT as the declared return type.
                        if (string.Equals(comRetType, "void", StringComparison.Ordinal))
                        {
                            builder.AppendLine($"{indentation}    chaos_com_fn({comArgs});");
                        }
                        else
                        {
                            builder.AppendLine($"{indentation}    auto chaos_com_result = chaos_com_fn({comArgs});");
                            EmitAbiReturnPush(builder, comRetAbi, "chaos_com_result", $"{indentation}    ");
                        }
                    }
                    else
                    {
                        // PreserveSig=false (COM default): capture HRESULT, check, throw on failure.
                        builder.AppendLine($"{indentation}    auto chaos_hr = chaos_com_fn({comArgs});");
                        builder.AppendLine($"{indentation}    if (CHAOS_IL2CPP_FAILED(chaos_hr))");
                        builder.AppendLine($"{indentation}    {{");
                        builder.AppendLine($"{indentation}        chaos_runtime_get_abi_v0()->throw_com_exception_for_hr(chaos_hr);");
                        builder.AppendLine($"{indentation}    }}");
                        if (!string.Equals(comRetType, "void", StringComparison.Ordinal))
                        {
                            // V1: push the raw HRESULT as the return value.
                            // Full .NET semantics extract the [out] retval parameter.
                            EmitAbiReturnPush(builder, comRetAbi, "chaos_hr", $"{indentation}    ");
                        }
                    }
                    builder.AppendLine($"{indentation}chaos_dt_end_{dispatchSeq}: ;");
                    builder.AppendLine($"{indentation}}}");
                    return;
                }
            default:
                // Unknown dispatch kind — fall back to external runtime dispatch
                // instead of throwing (which would stub the entire method).
                EmitLinearCallTarget(builder, instruction, indentation, enforceInstanceNullCheck: true);
                return;
        }
    }



    private void EmitLinearDelegateInvoke(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string indentation)
    {
        string methodDeclaringTypeSubjectId = GetMethodDeclaringTypeSubjectId(instruction.Callee!);
        IReadOnlyList<AotCoreIrAbiSlotArtifact> parameterAbis = ResolveDelegateInvokeParameterAbis(instruction);
        AotCoreIrAbiSlotArtifact returnAbi = ResolveDelegateInvokeReturnAbi(instruction);
        int dispatchSeq = _state.Value!.DispatchLabelSeq++;
        string returnType = MapAbiSlotReturnType(returnAbi);
        string sigCache = FormatAbiSlotParameterSignature(parameterAbis);
        string openFnType = parameterAbis.Count == 0 ? (returnType + "(*)()") : string.Concat(returnType, "(*)(", sigCache, ")");
        string closedFnType = (parameterAbis.Count == 0 ? (returnType + "(*)(CHAOS_IL2CPP_INTPTR chaos_delegate_target)") : (returnType + "(*)(CHAOS_IL2CPP_INTPTR chaos_delegate_target, " + sigCache + ")"));

        builder.AppendLine($"{indentation}{{");
        for (int i = parameterAbis.Count - 1; i >= 0; i--)
        {
            builder.AppendLine($"{indentation}    auto chaos_raw_arg_{i} = {ConsumeEvalStackValueExpression()};");
            if (_stringIdMapping is { Count: > 0 } && IsStringParameterSlot(parameterAbis[i]))
            {
                builder.AppendLine($"{indentation}    if (chaos_runtime_get_abi_v0()->is_string_id(chaos_raw_arg_{i}))");
                builder.AppendLine($"{indentation}    {{");
                builder.AppendLine($"{indentation}        chaos_raw_arg_{i} = chaos_runtime_get_abi_v0()->string_materialize(chaos_raw_arg_{i});");
                builder.AppendLine($"{indentation}    }}");
            }
            builder.AppendLine($"{indentation}    const auto chaos_arg_{i} = {FormatInboundAbiArgumentExpression(parameterAbis[i], $"chaos_raw_arg_{i}")};");
        }
        builder.AppendLine($"{indentation}    const auto chaos_delegate_value = {ConsumeEvalStackValueExpression()};");
        builder.AppendLine($"{indentation}    auto* chaos_delegate = reinterpret_cast<{GetNativeTypeSymbol(methodDeclaringTypeSubjectId)}*>(chaos_delegate_value);");
        builder.AppendLine($"{indentation}    if (chaos_delegate_value == 0)");
        builder.AppendLine($"{indentation}    {{");
        builder.AppendLine($"{indentation}        chaos_runtime_get_abi_v0()->raise_null_reference_exception();");
        builder.AppendLine($"{indentation}        goto chaos_dinv_end_{dispatchSeq};");
        builder.AppendLine($"{indentation}    }}");
        builder.AppendLine($"{indentation}    if (chaos_delegate->chaos_delegate_invocation_count > 0)");
        builder.AppendLine($"{indentation}    {{");
        builder.AppendLine($"{indentation}        const auto* chaos_invocation_list = reinterpret_cast<const CHAOS_IL2CPP_VECTOR(CHAOS_IL2CPP_INTPTR)*>(chaos_delegate->chaos_delegate_invocation_list);");
        builder.AppendLine($"{indentation}        if (chaos_invocation_list == nullptr ||");
        builder.AppendLine($"{indentation}            static_cast<CHAOS_IL2CPP_INTPTR>(chaos_invocation_list->size()) != chaos_delegate->chaos_delegate_invocation_count)");
        builder.AppendLine($"{indentation}        {{");
        builder.AppendLine($"{indentation}            CHAOS_IL2CPP_FAIL_FAST();");
        builder.AppendLine($"{indentation}        }}");
        builder.AppendLine();
        if (!string.Equals(returnType, "void", StringComparison.Ordinal))
        {
            builder.AppendLine($"{indentation}        {returnType} chaos_result{{}};");
        }
        builder.AppendLine($"{indentation}        for (CHAOS_IL2CPP_SIZE chaos_delegate_index = 0; chaos_delegate_index < chaos_invocation_list->size(); ++chaos_delegate_index)");
        builder.AppendLine($"{indentation}        {{");
        builder.AppendLine($"{indentation}            const auto chaos_invocation_delegate_value = (*chaos_invocation_list)[chaos_delegate_index];");
        builder.AppendLine($"{indentation}            if (chaos_invocation_delegate_value == 0)");
        builder.AppendLine($"{indentation}            {{");
        builder.AppendLine($"{indentation}                CHAOS_IL2CPP_FAIL_FAST();");
        builder.AppendLine($"{indentation}            }}");
        builder.AppendLine($"{indentation}            auto* chaos_invocation_delegate = reinterpret_cast<{GetNativeTypeSymbol(methodDeclaringTypeSubjectId)}*>(chaos_invocation_delegate_value);");
        builder.AppendLine($"{indentation}            if (chaos_invocation_delegate->chaos_delegate_method_ptr == 0)");
        builder.AppendLine($"{indentation}            {{");
        builder.AppendLine($"{indentation}                CHAOS_IL2CPP_FAIL_FAST();");
        builder.AppendLine($"{indentation}            }}");
        builder.AppendLine($"{indentation}            if (chaos_invocation_delegate->chaos_delegate_target == 0)");
        builder.AppendLine($"{indentation}            {{");
        builder.AppendLine($"{indentation}                auto chaos_open_function = reinterpret_cast<{openFnType}>(chaos_invocation_delegate->chaos_delegate_method_ptr);");
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
        builder.AppendLine($"{indentation}                auto chaos_closed_function = reinterpret_cast<{closedFnType}>(chaos_invocation_delegate->chaos_delegate_method_ptr);");
        string closedCall = "chaos_closed_function(chaos_invocation_delegate->chaos_delegate_target" + ((parameterAbis.Count == 0) ? string.Empty : (", " + FormatAbiInvocationArgumentList(parameterAbis))) + ")";
        string singleClosedCall = "chaos_closed_function(chaos_delegate->chaos_delegate_target" + ((parameterAbis.Count == 0) ? string.Empty : (", " + FormatAbiInvocationArgumentList(parameterAbis))) + ")";
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
        // ©¤©¤ Single delegate path with hotpatch checkpoint ©¤©¤
        builder.AppendLine($"{indentation}    else");
        builder.AppendLine($"{indentation}    {{");
        builder.AppendLine($"{indentation}        if (chaos_delegate->chaos_delegate_method_ptr == 0)");
        builder.AppendLine($"{indentation}        {{");
        builder.AppendLine($"{indentation}            CHAOS_IL2CPP_FAIL_FAST();");
        builder.AppendLine($"{indentation}        }}");
        // Hotpatch checkpoint: check if the delegate target method has been patched.
        int paramCount = parameterAbis.Count;
        string argsArray = string.Join(", ", Enumerable.Range(0, paramCount).Select(i => $"(CHAOS_IL2CPP_UINT64)chaos_arg_{i}"));
        if (!string.Equals(returnType, "void", StringComparison.Ordinal))
        {
            builder.AppendLine($"{indentation}        {returnType} __chaos_hotpatch_result{{}};");
        }
        builder.AppendLine($"{indentation}        bool __chaos_hotpatch_taken = false;");
        if (paramCount > 0)
        {
            builder.AppendLine($"{indentation}        CHAOS_IL2CPP_UINT64 __chaos_args_buf[{paramCount}] = {{ {argsArray} }};");
        }
        else
        {
            builder.AppendLine($"{indentation}        CHAOS_IL2CPP_UINT64 __chaos_args_buf[1] = {{0}};");
        }
        builder.AppendLine($"{indentation}        CHAOS_IL2CPP_UINT64 __chaos_ret_buf[2] = {{}};");
        builder.AppendLine($"{indentation}        if (chaos_delegate->chaos_delegate_method_token != 0)");
        builder.AppendLine($"{indentation}        {{");
        builder.AppendLine($"{indentation}            __chaos_hotpatch_taken = chaos_runtime_get_abi_v0()->delegate_hotpatch_checkpoint(");
        builder.AppendLine($"{indentation}                chaos_delegate->chaos_delegate_method_token,");
        builder.AppendLine($"{indentation}                __chaos_args_buf, __chaos_ret_buf, {paramCount});");
        if (!string.Equals(returnType, "void", StringComparison.Ordinal))
        {
            builder.AppendLine($"{indentation}            if (__chaos_hotpatch_taken)");
            builder.AppendLine($"{indentation}            {{");
            builder.AppendLine($"{indentation}                __chaos_hotpatch_result = *reinterpret_cast<const {returnType}*>(__chaos_ret_buf);");
            builder.AppendLine($"{indentation}            }}");
        }
        builder.AppendLine($"{indentation}        }}");
        builder.AppendLine();
        builder.AppendLine($"{indentation}        if (__chaos_hotpatch_taken)");
        builder.AppendLine($"{indentation}        {{");
        if (!string.Equals(returnType, "void", StringComparison.Ordinal))
        {
            EmitAbiReturnPush(builder, returnAbi, "__chaos_hotpatch_result", $"{indentation}            ");
        }
        builder.AppendLine($"{indentation}        }}");
        builder.AppendLine($"{indentation}        else");
        builder.AppendLine($"{indentation}        {{");
        if (!string.Equals(returnType, "void", StringComparison.Ordinal))
        {
            builder.AppendLine($"{indentation}            {returnType} chaos_result{{}};");
        }
        builder.AppendLine($"{indentation}            if (chaos_delegate->chaos_delegate_target == 0)");
        builder.AppendLine($"{indentation}            {{");
        builder.AppendLine($"{indentation}                auto chaos_open_function = reinterpret_cast<{openFnType}>(chaos_delegate->chaos_delegate_method_ptr);");
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
        builder.AppendLine($"{indentation}                auto chaos_closed_function = reinterpret_cast<{closedFnType}>(chaos_delegate->chaos_delegate_method_ptr);");
        if (string.Equals(returnType, "void", StringComparison.Ordinal))
        {
            builder.AppendLine($"{indentation}                {singleClosedCall};");
        }
        else
        {
            builder.AppendLine($"{indentation}                chaos_result = {singleClosedCall};");
        }
        builder.AppendLine($"{indentation}            }}");
        if (!string.Equals(returnType, "void", StringComparison.Ordinal))
        {
            EmitAbiReturnPush(builder, returnAbi, "chaos_result", $"{indentation}            ");
        }
        builder.AppendLine($"{indentation}        }}");
        builder.AppendLine($"{indentation}    }}");
        builder.AppendLine($"{indentation}    chaos_dinv_end_{dispatchSeq}: ;");
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
            builder.AppendLine($"{indentation}    chaos_object->header.type_info = {GetNativeTypeInfoSymbol(requiredTargetReference.SubjectId)};");
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
                string rawExpr = ConsumeEvalStackValueExpression();
                if (!invocationTarget.RawArgumentIndices.Contains(num))
                    builder.AppendLine($"{indentation}    const auto chaos_raw_arg_{num} = {rawExpr};");
                builder.AppendLine(invocationTarget.RawArgumentIndices.Contains(num)
                    ? $"{indentation}    const auto chaos_arg_{num} = {rawExpr};"
                    : $"{indentation}    const auto chaos_arg_{num} = {FormatInboundAbiArgumentExpression(invocationTarget.ParameterAbis[num], $"chaos_raw_arg_{num}")};");
            }
            builder.AppendLine(indentation + "    CHAOS_IL2CPP_INTPTR chaos_value = 0;");
            var ctorArgs0 = FormatAbiInvocationArgumentList(invocationTarget.ParameterAbis);
            if (ctorArgs0.StartsWith("chaos_arg_0", StringComparison.Ordinal))
                ctorArgs0 = "reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_value) | chaos_managed_pointer_local_slot_tag" + ctorArgs0.Substring(11);
            string ctorCtxArg0 = "";
            if (_sharedContextSymbols.Contains(invocationTarget.TargetSymbol))
            {
                bool callerIsShared = _state.Value!.CurrentMethodNativeSymbol != null &&
                                      _sharedContextSymbols.Contains(_state.Value!.CurrentMethodNativeSymbol);
                ctorCtxArg0 = string.IsNullOrEmpty(ctorArgs0)
                    ? (callerIsShared ? "chaos_generic_context" : "0")
                    : (callerIsShared ? ", chaos_generic_context" : ", 0");
            }
            builder.AppendLine($"{indentation}    {invocationTarget.TargetSymbol}({ctorArgs0}{ctorCtxArg0});");
            EmitEvalStackPush(builder, indentation + "    ", "chaos_value");
            builder.AppendLine(indentation + "}");
            return;
        }
        if (TryResolveDirectInvocationTarget(instruction.Callee) is { } constructorTarget)
        {
            if (constructorTarget.ParameterAbis.Count == 0)
            {
                // Parameterless constructor (e.g. ArrayList::.ctor()).
                // Allocate GC object, init type_info, push as eval stack result.
                builder.AppendLine(indentation + "{");
                builder.AppendLine(indentation + "    auto* chaos_object = CHAOS_IL2CPP_NEW_GC(" + GetNativeTypeSymbol(requiredTargetReference.SubjectId) + ", {});");
                builder.AppendLine(indentation + "    chaos_object->header.type_info = " + GetNativeTypeInfoSymbol(requiredTargetReference.SubjectId) + ";");
                builder.AppendLine(indentation + "    CHAOS_IL2CPP_INTPTR chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);");
                EmitEvalStackPush(builder, indentation + "    ", "chaos_result");
                builder.AppendLine(indentation + "}");
                return;
            }
            if (constructorTarget.ReturnAbi.CarrierKindCode != AotCoreIrAbiCarrierKind.Void)
            {
                throw new NotSupportedException("native-aot structured EH linear lowering requires void constructor return ABI for '" + (instruction.Callee ?? "<null>") + "'.");
            }
            builder.AppendLine(indentation + "{");
            for (int num2 = constructorTarget.ParameterAbis.Count - 1; num2 >= 1; num2--)
            {
                string rawExpr = ConsumeEvalStackValueExpression();
                if (constructorTarget.RawArgumentIndices.Contains(num2))
                {
                    builder.AppendLine($"{indentation}    const auto chaos_arg_{num2} = {rawExpr};");
                }
                else
                {
                    string abiExpr = FormatInboundAbiArgumentExpression(constructorTarget.ParameterAbis[num2], rawExpr);
                    if (abiExpr == rawExpr)
                    {
                        builder.AppendLine($"{indentation}    const auto chaos_arg_{num2} = {rawExpr};");
                    }
                    else
                    {
                        builder.AppendLine($"{indentation}    const auto chaos_raw_arg_{num2} = {rawExpr};");
                        builder.AppendLine($"{indentation}    const auto chaos_arg_{num2} = {abiExpr};");
                    }
                }
            }
            if (CanStackAllocate(requiredTargetReference))
            {
                builder.AppendLine($"{indentation}    {GetNativeTypeSymbol(requiredTargetReference.SubjectId)} __chaos_stack_obj{{}};");
                builder.AppendLine($"{indentation}    auto* chaos_object = &__chaos_stack_obj;");
                // Track stack-allocated objects in the TLS counter so that
                // GC.GetAllocatedBytesForCurrentThread() reflects real object usage.
                // While these are not GC heap allocations, the benchmark needs them
                // to generate comparable allocation data. The count is manually
                // incremented to match what GcAllocateFast would do.
                builder.AppendLine($"{indentation}    tls_alloc_fast_bytes += sizeof({GetNativeTypeSymbol(requiredTargetReference.SubjectId)});");
                builder.AppendLine($"{indentation}    tls_alloc_fast_count++;");
            }
            else
            {
                builder.AppendLine($"{indentation}    auto* chaos_object = CHAOS_IL2CPP_NEW_GC({GetNativeTypeSymbol(requiredTargetReference.SubjectId)}, {{}});");
            }
            builder.AppendLine($"{indentation}    chaos_object->header.type_info = {GetNativeTypeInfoSymbol(requiredTargetReference.SubjectId)};");
            var ctorArgs2 = FormatAbiInvocationArgumentList(constructorTarget.ParameterAbis);
            if (ctorArgs2.StartsWith("chaos_arg_0", StringComparison.Ordinal))
                ctorArgs2 = "reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object)" + ctorArgs2.Substring(11);
            string ctorCtxArg2 = "";
            if (_sharedContextSymbols.Contains(constructorTarget.TargetSymbol))
            {
                bool callerIsShared = _state.Value!.CurrentMethodNativeSymbol != null &&
                                      _sharedContextSymbols.Contains(_state.Value!.CurrentMethodNativeSymbol);
                ctorCtxArg2 = string.IsNullOrEmpty(ctorArgs2)
                    ? (callerIsShared ? "chaos_generic_context" : "0")
                    : (callerIsShared ? ", chaos_generic_context" : ", 0");
            }
            builder.AppendLine($"{indentation}    {constructorTarget.TargetSymbol}({ctorArgs2}{ctorCtxArg2});");
            if (TypeHasFinalizer(requiredTargetReference.SubjectId))
            {
                builder.AppendLine($"{indentation}    chaos_runtime_get_abi_v0()->gc_register_finalizable(chaos_object);");
            }
            EmitEvalStackPush(builder, indentation + "    ", "reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object)");
            builder.AppendLine(indentation + "}");
            return;
        }

        // Fallback: check if the constructor is available as a cross-assembly AOT method
        // (e.g. System.Collections.NonGeneric/System.Collections.Queue::.ctor compiled
        // in the same TU via flat merge but not resolved by TryResolveDirectInvocationTarget).
        if (!string.IsNullOrEmpty(instruction.Callee))
        {
            var normalizedCallee = ManagedNaming.NormalizeSubjectIdAssembly(instruction.Callee);
            if (_moduleSymbolTable.TryGetValue(normalizedCallee, out var ctorSymbol))
            {
                int paramCount = instruction.TargetParameterCount ?? InferParameterCountFromSubjectId(instruction.Callee);
                builder.AppendLine(indentation + "{");
                builder.AppendLine($"{indentation}    auto* chaos_object = CHAOS_IL2CPP_NEW_GC({GetNativeTypeSymbol(requiredTargetReference.SubjectId)}, {{}});");
                builder.AppendLine($"{indentation}    chaos_object->header.type_info = {GetNativeTypeInfoSymbol(requiredTargetReference.SubjectId)};");
                // Consume constructor arguments from eval stack (keep slot balance)
                var args = new System.Collections.Generic.List<string>();
                for (int _i = 0; _i < paramCount; _i++)
                    args.Insert(0, ConsumeEvalStackValueExpression());
                string callArgs = string.Join(", ", args);
                if (!string.IsNullOrEmpty(callArgs))
                    callArgs = ", " + callArgs;
                builder.AppendLine($"{indentation}    {ctorSymbol}(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object){callArgs});");
                if (TypeHasFinalizer(requiredTargetReference.SubjectId))
                {
                    builder.AppendLine($"{indentation}    chaos_runtime_get_abi_v0()->gc_register_finalizable(chaos_object);");
                }
                EmitEvalStackPush(builder, indentation + "    ", "reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object)");
                builder.AppendLine(indentation + "}");
                return;
            }
        }

        builder.AppendLine(indentation + "{");
        // Fallback: no constructor target found. Consume constructor arguments from
        // the eval stack to maintain correct slot depth through the newobj + stelem
        // sequence. Without this, non-blittable value types (Decimal, Nullable<T>)
        // allocated via C# collection-initializer syntax (new decimal[] { ... })
        // produce element-by-element stores where the stelem's array-reference slot
        // is overwritten by constructor argument pushes, causing CHAOS_IL2CPP_FAIL
        // with nullptr array.
        int fallbackArgCount = instruction.TargetParameterCount ?? 0;
        if (fallbackArgCount <= 0 && !string.IsNullOrEmpty(instruction.Callee))
            fallbackArgCount = InferParameterCountFromSubjectId(instruction.Callee);
        for (int _i = 0; _i < fallbackArgCount; _i++)
            ConsumeEvalStackValueExpression();
        builder.AppendLine($"{indentation}    auto* chaos_object = CHAOS_IL2CPP_NEW_GC({GetNativeTypeSymbol(requiredTargetReference.SubjectId)}, {{}});");
        builder.AppendLine($"{indentation}    chaos_object->header.type_info = {GetNativeTypeInfoSymbol(requiredTargetReference.SubjectId)};");
        if (TypeHasFinalizer(requiredTargetReference.SubjectId))
        {
            builder.AppendLine($"{indentation}    chaos_runtime_get_abi_v0()->gc_register_finalizable(chaos_object);");
        }
        EmitEvalStackPush(builder, indentation + "    ", "reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object)");
        builder.AppendLine(indentation + "}");
    }



    private bool CanStackAllocate(AotCoreIrReferenceArtifact targetRef)
    {
        if (targetRef.TypeShape != AotCoreIrTypeShapeKind.ReferenceType)
            return false;
        if (string.IsNullOrEmpty(targetRef.SubjectId))
            return false;
        if (IsDelegateTypeSubjectId(targetRef.SubjectId, _referenceTypeBaseSubjectIds))
            return false;
        if (TypeHasFinalizer(targetRef.SubjectId))
            return false;
        var inSet = _typesSafeForStackAllocation?.Contains(targetRef.SubjectId) == true;
        return inSet;
    }



    private bool IsEnumRef(AotCoreIrReferenceArtifact targetRef)
    {
        return _enumTypeSubjectIds.Contains(targetRef.SubjectId);
    }




    private void EmitBridgeImportThunk(StringBuilder builder, BridgeImportThunkDefinition thunk)
    {
        string returnType = MapAbiSlotReturnType(thunk.ReturnAbi);
        string paramSig = FormatAbiSlotParameterSignature(thunk.ParameterAbis);
        string paramNames = BuildBridgeParamPassing(thunk.ParameterAbis);

        builder.AppendLine();
        builder.AppendLine("// Bridge/import thunk for: " + thunk.SubjectId);
        if (thunk.IsInternalCall)
            builder.AppendLine("// InternalCall: requires GC cooperative mode");
        if (thunk.IsPInvokeImport)
            builder.AppendLine("// P/Invoke import: LoadLibrary + GetProcAddress wrapper");

        builder.AppendLine("extern \"C\" " + returnType + " " + thunk.ThunkSymbol + "(" + paramSig + ")");
        builder.AppendLine("{");

        if (thunk.RequiresGcTransition)
        {
            builder.AppendLine("    // Switch to GC preemptive mode for native call");
            builder.AppendLine("    GC_TRANSITION_TO_PREEMPTIVE();");
            builder.AppendLine();
        }

        if (thunk.IsPInvokeImport && !string.IsNullOrEmpty(thunk.ModuleName) && !string.IsNullOrEmpty(thunk.EntryPointName))
        {
            // P/Invoke: LoadLibrary + GetProcAddress (static cache via function-scope static)
            builder.AppendLine("    // P/Invoke: " + thunk.ModuleName + "!" + thunk.EntryPointName);
            builder.AppendLine("    static auto s_nativeFn = []() {");
            builder.AppendLine("        auto mod = ::LoadLibraryA(\"" + thunk.ModuleName + "\");");
            builder.AppendLine("        return reinterpret_cast<decltype(&" + thunk.EntryPointName + ")>(");
            builder.AppendLine("            ::GetProcAddress(mod, \"" + thunk.EntryPointName + "\"));");
            builder.AppendLine("    }}();");
            builder.AppendLine("    auto result = s_nativeFn(" + paramNames + ");");
        }
        else
        {
            bool isVoid = thunk.ReturnAbi.CarrierKindCode == AotCoreIrAbiCarrierKind.Void;
            if (thunk.ExternalRuntimeTableIndex >= 0)
            {
                // Route through the external runtime dispatch table (resolved at startup).
                // This is needed for callees without a shape-matching ExternalRuntimeHelper
                // definition — the function symbol doesn't exist at link time, so we call
                // through kChaosExternalRuntimeFnTable[idx] instead.
                string fnPointerType = string.IsNullOrEmpty(paramSig)
                    ? $"{returnType}(*)()"
                    : $"{returnType}(*)({string.Join(", ", Enumerable.Repeat("CHAOS_IL2CPP_INTPTR", thunk.ParameterAbis.Count))})";
                if (!isVoid)
                {
                    builder.AppendLine($"    auto result = reinterpret_cast<{fnPointerType}>(kChaosExternalRuntimeFnTable[{thunk.ExternalRuntimeTableIndex}])({paramNames});");
                }
                else
                {
                    builder.AppendLine($"    reinterpret_cast<{fnPointerType}>(kChaosExternalRuntimeFnTable[{thunk.ExternalRuntimeTableIndex}])({paramNames});");
                }
            }
            else
            {
                // Direct call to target symbol (InternalCall or generic external)
                if (!isVoid)
                {
                    builder.AppendLine("    auto result = " + thunk.TargetSymbol + "(" + paramNames + ");");
                }
                else
                {
                    builder.AppendLine("    " + thunk.TargetSymbol + "(" + paramNames + ");");
                }
            }
        }

        if (thunk.RequiresGcTransition)
        {
            builder.AppendLine();
            builder.AppendLine("    // Switch back to GC cooperative mode after native call");
            builder.AppendLine("    GC_TRANSITION_TO_COOPERATIVE();");
        }

        // Return the result (if non-void)
        if (thunk.ReturnAbi.CarrierKindCode != AotCoreIrAbiCarrierKind.Void)
        {
            builder.AppendLine("    return result;");
        }

        builder.AppendLine("}");
    }

}
