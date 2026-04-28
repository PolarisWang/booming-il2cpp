from __future__ import annotations

import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]

NATIVE_REFERENCE_PLANNER_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "NativeReferenceLoweringPlanner.cs"
NATIVE_REFERENCE_EMITTER_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "NativeReferenceProofEmitter.cs"
NATIVE_AOT_EMITTER_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "NativeAotEmitter.cs"
NATIVE_AOT_PLANNER_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "NativeAotLoweringPlanner.cs"

NATIVE_REFERENCE_CATALOG_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "ReferenceProof"
    / "NativeReferenceProofCatalog.cs"
)
NATIVE_AOT_TEMPLATE_CATALOG_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Emission"
    / "NativeAot"
    / "NativeAotTemplateCatalog.cs"
)
NATIVE_REFERENCE_ENGINE_AND_FAMILY_SELECTION_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "ReferenceProof"
    / "NativeReferenceLoweringPlanner.EngineAndFamilySelection.cs"
)
NATIVE_REFERENCE_PLAN_BUILDERS_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "ReferenceProof"
    / "NativeReferenceLoweringPlanner.PlanBuilders.cs"
)
NATIVE_REFERENCE_PLAN_GENERATED_PLUGIN_EXPORTS_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "ReferenceProof"
    / "NativeReferenceLoweringPlanner.GeneratedPluginExports.cs"
)
NATIVE_REFERENCE_SHAPE_VALIDATION_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "ReferenceProof"
    / "NativeReferenceLoweringPlanner.ShapeValidation.cs"
)
NATIVE_REFERENCE_EMITTER_CONVERTLIKE_FAMILY_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "ReferenceProof"
    / "NativeReferenceProofEmitter.ConvertLikeFamily.cs"
)
NATIVE_REFERENCE_EMITTER_CONVERT_FAMILY_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "ReferenceProof"
    / "NativeReferenceProofEmitter.ConvertFamily.cs"
)
NATIVE_REFERENCE_EMITTER_STRING_MANAGED_INVOKE_FAMILY_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "ReferenceProof"
    / "NativeReferenceProofEmitter.StringManagedInvokeFamily.cs"
)
NATIVE_REFERENCE_EMITTER_UNSAFE_MANAGED_INVOKE_FAMILY_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "ReferenceProof"
    / "NativeReferenceProofEmitter.UnsafeManagedInvokeFamily.cs"
)
NATIVE_REFERENCE_EMITTER_MEMORY_EXTENSIONS_MANAGED_INVOKE_FAMILY_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "ReferenceProof"
    / "NativeReferenceProofEmitter.MemoryExtensionsManagedInvokeFamily.cs"
)
NATIVE_REFERENCE_EMITTER_TASK_CONTINUATION_FAMILY_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "ReferenceProof"
    / "NativeReferenceProofEmitter.TaskContinuationFamily.cs"
)
NATIVE_REFERENCE_EMITTER_GENERATED_PLUGIN_EXPORTS_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "ReferenceProof"
    / "NativeReferenceProofEmitter.GeneratedPluginExports.cs"
)
NATIVE_REFERENCE_EMITTER_SPANHELPERS_KERNEL_FAMILY_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "ReferenceProof"
    / "NativeReferenceProofEmitter.SpanHelpersKernelFamily.cs"
)
NATIVE_REFERENCE_EMITTER_ARRAY_AND_MEMORYMARSHAL_FAMILY_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "ReferenceProof"
    / "NativeReferenceProofEmitter.ArrayAndMemoryMarshalFamily.cs"
)
NATIVE_REFERENCE_EMITTER_MARSHAL_PLATFORM_CAPABILITY_FAMILY_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "ReferenceProof"
    / "NativeReferenceProofEmitter.MarshalPlatformCapabilityFamily.cs"
)
NATIVE_REFERENCE_EMITTER_INTEROP_KERNEL32_PLATFORM_CAPABILITY_FAMILY_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "ReferenceProof"
    / "NativeReferenceProofEmitter.InteropKernel32PlatformCapabilityFamily.cs"
)
NATIVE_REFERENCE_EMITTER_NATIVE_RUNTIME_EVENTSOURCE_PLATFORM_CAPABILITY_FAMILY_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "ReferenceProof"
    / "NativeReferenceProofEmitter.NativeRuntimeEventSourcePlatformCapabilityFamily.cs"
)
NATIVE_REFERENCE_EMITTER_HALF_MANAGED_INVOKE_FAMILY_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "ReferenceProof"
    / "NativeReferenceProofEmitter.HalfManagedInvokeFamily.cs"
)
NATIVE_REFERENCE_EMITTER_FLOATING_SCALAR_MANAGED_INVOKE_FAMILY_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "ReferenceProof"
    / "NativeReferenceProofEmitter.FloatingScalarManagedInvokeFamily.cs"
)
NATIVE_REFERENCE_EMITTER_NUMBER_MANAGED_INVOKE_FAMILY_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "ReferenceProof"
    / "NativeReferenceProofEmitter.NumberManagedInvokeFamily.cs"
)
NATIVE_REFERENCE_EMITTER_DECIMAL_MANAGED_INVOKE_FAMILY_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "ReferenceProof"
    / "NativeReferenceProofEmitter.DecimalManagedInvokeFamily.cs"
)
NATIVE_REFERENCE_EMITTER_CHAR_MANAGED_INVOKE_FAMILY_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "ReferenceProof"
    / "NativeReferenceProofEmitter.CharManagedInvokeFamily.cs"
)
NATIVE_REFERENCE_EMITTER_COMPANION_MANAGED_INVOKE_FAMILY_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "ReferenceProof"
    / "NativeReferenceProofEmitter.CompanionManagedInvokeFamily.cs"
)
NATIVE_REFERENCE_EMITTER_DATETIME_SEMANTIC_ENGINE_FAMILY_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "ReferenceProof"
    / "NativeReferenceProofEmitter.DateTimeSemanticEngineFamily.cs"
)
NATIVE_REFERENCE_EMITTER_GLOBALIZATION_DATETIME_SUPPORT_FAMILY_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "ReferenceProof"
    / "NativeReferenceProofEmitter.GlobalizationDateTimeSupportFamily.cs"
)
NATIVE_REFERENCE_EMITTER_VALUETYPE_KERNEL_FAMILY_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "ReferenceProof"
    / "NativeReferenceProofEmitter.ValueTypeKernelFamily.cs"
)
NATIVE_REFERENCE_EMITTER_VECTOR_KERNEL_FAMILY_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "ReferenceProof"
    / "NativeReferenceProofEmitter.VectorKernelFamily.cs"
)
NATIVE_REFERENCE_EMITTER_VECTOR_MANAGED_INVOKE_FAMILY_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "ReferenceProof"
    / "NativeReferenceProofEmitter.VectorManagedInvokeFamily.cs"
)
NATIVE_REFERENCE_EMITTER_ENUM_MANAGED_INVOKE_FAMILY_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "ReferenceProof"
    / "NativeReferenceProofEmitter.EnumManagedInvokeFamily.cs"
)
RUNTIME_SKELETON_MANAGED_INVOKE_CORE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "ReferenceProof"
    / "RuntimeSkeletonManagedInvokeCore.cs"
)
RUNTIME_SKELETON_VALUE_TYPE_MANAGED_INVOKE_CORE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "ReferenceProof"
    / "RuntimeSkeletonValueTypeManagedInvokeCore.cs"
)
RUNTIME_SKELETON_VECTOR_MANAGED_INVOKE_ABI_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "ReferenceProof"
    / "RuntimeSkeletonVectorManagedInvokeAbi.cs"
)
RUNTIME_SKELETON_SEMANTIC_REGISTRY_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "ReferenceProof"
    / "RuntimeSkeletonSemanticRegistry.cs"
)
RUNTIME_SKELETON_BACKEND_SELECTOR_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "ReferenceProof"
    / "RuntimeSkeletonBackendSelector.cs"
)
RUNTIME_SKELETON_KERNEL_CORE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "ReferenceProof"
    / "RuntimeSkeletonKernelCore.cs"
)
RUNTIME_SKELETON_VECTOR_KERNEL_CORE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "ReferenceProof"
    / "RuntimeSkeletonVectorKernelCore.cs"
)
RUNTIME_SKELETON_MARSHAL_PLATFORM_CORE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "ReferenceProof"
    / "RuntimeSkeletonMarshalPlatformCore.cs"
)
RUNTIME_SKELETON_MARSHAL_MANAGED_INVOKE_ABI_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "ReferenceProof"
    / "RuntimeSkeletonMarshalManagedInvokeAbi.cs"
)
RUNTIME_SKELETON_TASK_PLATFORM_CORE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "ReferenceProof"
    / "RuntimeSkeletonTaskPlatformCore.cs"
)
RUNTIME_SKELETON_TASK_MANAGED_INVOKE_ABI_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "ReferenceProof"
    / "RuntimeSkeletonTaskManagedInvokeAbi.cs"
)
CODEGEN_PLUGIN_ATTRIBUTES_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Pluginization"
    / "CodeGenPluginAttributes.cs"
)
CODEGEN_PLUGIN_REGISTRIES_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Pluginization"
    / "CodeGenPluginRegistries.cs"
)
CODEGEN_TEMPLATE_BUNDLE_PLUGINS_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Pluginization"
    / "CodeGenTemplateBundlePlugins.cs"
)
CODEGEN_RUNTIME_PROVIDER_PLUGINS_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Pluginization"
    / "CodeGenRuntimeProviderPlugins.cs"
)
CODEGEN_RUNTIME_PROVIDER_CATALOG_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Pluginization"
    / "CodeGenRuntimeProviderCatalog.cs"
)
CODEGEN_GENERATOR_PROJECT_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen.Generators"
    / "Chaos.IL2CPP.CodeGen.Generators.csproj"
)
CODEGEN_GENERATOR_SOURCE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen.Generators"
    / "CodeGenPluginRegistryGenerator.cs"
)
RUNTIME_SKELETON_INTEROP_KERNEL32_PLATFORM_CORE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "ReferenceProof"
    / "RuntimeSkeletonInteropKernel32PlatformCore.cs"
)
RUNTIME_SKELETON_INTEROP_KERNEL32_MANAGED_INVOKE_ABI_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "ReferenceProof"
    / "RuntimeSkeletonInteropKernel32ManagedInvokeAbi.cs"
)
RUNTIME_SKELETON_NATIVE_RUNTIME_EVENTSOURCE_MANAGED_INVOKE_ABI_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "ReferenceProof"
    / "RuntimeSkeletonNativeRuntimeEventSourceManagedInvokeAbi.cs"
)
NATIVE_REFERENCE_EMITTER_STRING_MICRO_FAMILY_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "ReferenceProof"
    / "NativeReferenceProofEmitter.StringMicroFamily.cs"
)
SCRIBAN_RENDERER_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templating"
    / "ScribanTemplateRenderer.cs"
)
NATIVE_REFERENCE_AUDIT_SUMMARY_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "AssemblyFullClosureAuditSummary.cpp.scriban"
)
ASSEMBLY_FULL_CLOSURE_AUDIT_EMITTER_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "AssemblyFullClosureAuditEmitter.cs"
)
NATIVE_REFERENCE_RUNTIME_SKELETON_SUMMARY_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeletonSummary.cpp.scriban"
)
NATIVE_REFERENCE_RUNTIME_SKELETON_PAGE_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeletonPage.cpp.scriban"
)
NATIVE_REFERENCE_RUNTIME_SKELETON_CONSOLE_WRITE_LINE_STUB_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.ConsoleWriteLineStub.cpp.scriban"
)
NATIVE_REFERENCE_RUNTIME_SKELETON_MARSHALING_UTF8_EXPORT_STUB_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.MarshalingUtf8ExportStub.cpp.scriban"
)
NATIVE_REFERENCE_RUNTIME_SKELETON_PINVOKE_DIRECT_CALL_STUB_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.PInvokeDirectCallStub.cpp.scriban"
)
NATIVE_REFERENCE_RUNTIME_SKELETON_STATIC_CALL_CTOR_GETTER_STUB_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.StaticCallCtorGetterStub.cpp.scriban"
)
NATIVE_REFERENCE_RUNTIME_SKELETON_CONSTRUCTOR_THEN_INSTANCE_CALL_STUB_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.ConstructorThenInstanceCallStub.cpp.scriban"
)
NATIVE_REFERENCE_RUNTIME_SKELETON_DELEGATE_CLOSED_TARGET_RELAY_STUB_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.DelegateClosedTargetRelayStub.cpp.scriban"
)
NATIVE_REFERENCE_RUNTIME_SKELETON_INTERFACE_DISPATCH_MESSAGE_STUB_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.InterfaceDispatchMessageStub.cpp.scriban"
)
NATIVE_REFERENCE_RUNTIME_SKELETON_REFLECTION_INTEROP_CLOSURE_STUB_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.ReflectionInteropClosureStub.cpp.scriban"
)
NATIVE_REFERENCE_RUNTIME_SKELETON_EXCEPTION_THROW_CATCH_FINALLY_STUB_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.ExceptionThrowCatchFinallyStub.cpp.scriban"
)
NATIVE_REFERENCE_RUNTIME_SKELETON_NESTED_EXCEPTION_THROW_CATCH_FINALLY_STUB_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.NestedExceptionThrowCatchFinallyStub.cpp.scriban"
)
NATIVE_REFERENCE_RUNTIME_SKELETON_ARRAY_BOXING_REFERENCE_ARRAY_STUB_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.ArrayBoxingReferenceArrayStub.cpp.scriban"
)
NATIVE_REFERENCE_RUNTIME_SKELETON_MARSHAL_PLATFORM_FASTPATH_STUB_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.MarshalPlatformFastPathStub.cpp.scriban"
)
NATIVE_REFERENCE_RUNTIME_SKELETON_TASK_KERNEL_FASTPATH_STUB_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.TaskKernelFastPathStub.cpp.scriban"
)
NATIVE_REFERENCE_RUNTIME_SKELETON_TASK_MANAGED_INVOKE_STUB_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.TaskManagedInvokeStub.cpp.scriban"
)
NATIVE_REFERENCE_RUNTIME_SKELETON_NATIVE_RUNTIME_EVENTSOURCE_MANAGED_INVOKE_STUB_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.NativeRuntimeEventSourceManagedInvokeStub.cpp.scriban"
)
NATIVE_AOT_RUNTIME_PRELUDE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "RuntimeSupport"
    / "NativeAotLoweringPlanner.RuntimeSupport.cs"
)
NATIVE_AOT_COLLECTION_RUNTIME_PRELUDE_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeAot.CollectionRuntimePrelude.cpp.scriban"
)
NATIVE_AOT_MONITOR_RUNTIME_PRELUDE_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeAot.MonitorRuntimePrelude.cpp.scriban"
)
NATIVE_AOT_THREAD_RUNTIME_PRELUDE_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeAot.ThreadRuntimePrelude.cpp.scriban"
)
NATIVE_AOT_ASYNC_RUNTIME_PRELUDE_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeAot.AsyncRuntimePrelude.cpp.scriban"
)
NATIVE_AOT_RUNTIME_PRELUDE_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeAot.RuntimePrelude.cpp.scriban"
)
NATIVE_AOT_SPAN_RUNTIME_PRELUDE_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeAot.SpanRuntimePrelude.cpp.scriban"
)
NATIVE_AOT_RUNTIME_PRELUDE_AGGREGATOR_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "RuntimeSupport"
    / "NativeAotLoweringPlanner.RuntimePrelude.cs"
)
NATIVE_AOT_STATIC_INITIALIZATION_EMISSION_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "RuntimeSupport"
    / "NativeAotLoweringPlanner.StaticInitializationEmission.cs"
)
NATIVE_AOT_STATIC_INITIALIZATION_DEFINITION_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeAot.StaticInitializationDefinition.cpp.scriban"
)
NATIVE_AOT_STATIC_INITIALIZATION_ACTION_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeAot.StaticInitializationAction.cpp.scriban"
)
NATIVE_AOT_STATIC_INITIALIZATION_CALL_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeAot.StaticInitializationCall.cpp.scriban"
)
NATIVE_AOT_INVOCATION_ABI_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "RuntimeSupport"
    / "NativeAotLoweringPlanner.ExternalRuntimeHelpers.InvocationAbi.cs"
)
NATIVE_AOT_METHOD_RETURN_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeAot.MethodReturn.cpp.scriban"
)
NATIVE_AOT_ABI_RETURN_PUSH_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeAot.AbiReturnPush.cpp.scriban"
)
NATIVE_AOT_ABI_ARGUMENT_INITIALIZATION_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeAot.AbiArgumentInitialization.cpp.scriban"
)
NATIVE_AOT_STRING_AND_PLATFORM_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "RuntimeSupport"
    / "NativeAotLoweringPlanner.ExternalRuntimeHelpers.StringAndPlatform.cs"
)
NATIVE_AOT_STRING_JOIN_INT32_ENUMERABLE_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeAot.StringJoinInt32Enumerable.cpp.scriban"
)
NATIVE_AOT_STRING_JOIN_STRING_ENUMERABLE_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeAot.StringJoinStringEnumerable.cpp.scriban"
)
NATIVE_AOT_SIMPLE_EXTERNAL_RUNTIME_HELPER_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeAot.SimpleExternalRuntimeHelper.cpp.scriban"
)
NATIVE_AOT_TESTFRAMEWORK_EQUAL_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeAot.TestFrameworkEqual.cpp.scriban"
)
NATIVE_AOT_METADATA_SUPPORT_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Planning"
    / "NativeAotLoweringPlanner.MetadataSupport.cs"
)
NATIVE_AOT_INVOCATION_PLANNING_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Planning"
    / "NativeAotLoweringPlanner.InvocationPlanning.cs"
)
NATIVE_AOT_METHOD_EMISSION_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Emission"
    / "NativeAotLoweringPlanner.MethodEmission.cs"
)
NATIVE_AOT_EXCEPTION_EMISSION_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Emission"
    / "NativeAotLoweringPlanner.ExceptionEmission.cs"
)
NATIVE_AOT_EXTERNAL_RUNTIME_HELPERS_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "RuntimeSupport"
    / "NativeAotLoweringPlanner.ExternalRuntimeHelpers.cs"
)
NATIVE_AOT_OBJECT_MODEL_EMISSION_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Emission"
    / "NativeAotLoweringPlanner.ObjectModelEmission.cs"
)
NATIVE_AOT_OBJECT_EQUALITY_EMISSION_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Emission"
    / "NativeAotLoweringPlanner.ObjectEqualityEmission.cs"
)
NATIVE_AOT_REFLECTION_OBJECT_EMISSION_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Emission"
    / "NativeAotLoweringPlanner.ReflectionObjectEmission.cs"
)
NATIVE_AOT_OBJECT_MODEL_UTILITIES_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Emission"
    / "NativeAotLoweringPlanner.ObjectModelUtilities.cs"
)
NATIVE_AOT_EXTERNAL_RUNTIME_STRING_AND_PLATFORM_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "RuntimeSupport"
    / "NativeAotLoweringPlanner.ExternalRuntimeHelpers.StringAndPlatform.cs"
)
NATIVE_AOT_EXTERNAL_RUNTIME_COLLECTION_AND_REFLECTION_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "RuntimeSupport"
    / "NativeAotLoweringPlanner.ExternalRuntimeHelpers.CollectionAndReflection.cs"
)
NATIVE_AOT_EXTERNAL_RUNTIME_TYPE_RESOLUTION_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "RuntimeSupport"
    / "NativeAotLoweringPlanner.ExternalRuntimeHelpers.TypeResolution.cs"
)
NATIVE_AOT_EXTERNAL_RUNTIME_INVOCATION_ABI_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "RuntimeSupport"
    / "NativeAotLoweringPlanner.ExternalRuntimeHelpers.InvocationAbi.cs"
)
NATIVE_AOT_EXTERNAL_RUNTIME_ASSERTIONS_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "RuntimeSupport"
    / "NativeAotLoweringPlanner.ExternalRuntimeHelpers.Assertions.cs"
)
TEST_FRAMEWORK_SUBJECT_FAMILY_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "TestFrameworkSubjectFamily.cs"
)
LOADER_STAGE_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Loader" / "LoaderStage.cs"
LOADER_STAGE_CROSS_ASSEMBLY_PATH = (
    REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Loader" / "LoaderStage.CrossAssemblyInstantiation.cs"
)
LOADER_STAGE_ASSEMBLY_LOADING_PATH = (
    REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Loader" / "LoaderStage.AssemblyLoading.cs"
)
LOADER_STAGE_INSTRUCTION_DECODING_PATH = (
    REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Loader" / "LoaderStage.InstructionDecoding.cs"
)
LOADER_STAGE_METADATA_RESOLUTION_PATH = (
    REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Loader" / "LoaderStage.MetadataResolution.cs"
)
LOADER_STAGE_GENERIC_INSTANTIATION_PROJECTION_PATH = (
    REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Loader" / "LoaderStage.GenericInstantiationProjection.cs"
)
LINKER_STAGE_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Linker" / "LinkerStage.cs"
LINKER_STAGE_REACHABILITY_PATH = (
    REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Linker" / "LinkerStage.Reachability.cs"
)
LINKER_STAGE_OPTIMIZATION_FACTS_PATH = (
    REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Linker" / "LinkerStage.OptimizationFacts.cs"
)
LINKER_STAGE_DISPATCH_RESOLUTION_PATH = (
    REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Linker" / "LinkerStage.DispatchResolution.cs"
)
LINKER_STAGE_OUTPUT_PROJECTION_PATH = (
    REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Linker" / "LinkerStage.OutputProjection.cs"
)
MANAGED_CLOSURE_CONTRACTS_PATH = (
    REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Contracts" / "ManagedClosureContracts.cs"
)
MANAGED_NAMING_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Contracts" / "ManagedNaming.cs"
MANAGED_CLOSURE_MODELS_PATH = (
    REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Contracts" / "ManagedClosureModels.cs"
)
MANAGED_METHOD_IDENTITY_CONTRACTS_PATH = (
    REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Contracts" / "ManagedMethodIdentityContracts.cs"
)
MANAGED_SEMANTIC_WORLD_CONTRACTS_PATH = (
    REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Contracts" / "ManagedSemanticWorldContracts.cs"
)
TYPED_IL_AND_AOT_CORE_IR_CONTRACTS_PATH = (
    REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Contracts" / "TypedIlAndAotCoreIrContracts.cs"
)
MANAGED_CLOSURE_ARTIFACT_MODELS_PATH = (
    REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Contracts" / "ManagedClosureArtifactModels.cs"
)

class Il2CppCodeGenStructureGovernanceTestSupport(unittest.TestCase):
    pass
