from __future__ import annotations

import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]

NATIVE_REFERENCE_PLANNER_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Generator" / "NativeReferenceLoweringPlanner.cs"
NATIVE_REFERENCE_EMITTER_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Generator" / "NativeReferenceProofEmitter.cs"
NATIVE_AOT_EMITTER_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Generator" / "NativeAotEmitter.cs"
NATIVE_AOT_PLANNER_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Generator" / "NativeAotLoweringPlanner.cs"

NATIVE_REFERENCE_CATALOG_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "ReferenceProof"
    / "NativeReferenceProofCatalog.cs"
)
NATIVE_AOT_TEMPLATE_CATALOG_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "Emission"
    / "NativeAot"
    / "NativeAotTemplateCatalog.cs"
)
NATIVE_REFERENCE_ENGINE_AND_FAMILY_SELECTION_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "ReferenceProof"
    / "NativeReferenceLoweringPlanner.EngineAndFamilySelection.cs"
)
NATIVE_REFERENCE_PLAN_BUILDERS_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "ReferenceProof"
    / "NativeReferenceLoweringPlanner.PlanBuilders.cs"
)
NATIVE_REFERENCE_PLAN_GENERATED_PLUGIN_EXPORTS_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "ReferenceProof"
    / "NativeReferenceLoweringPlanner.GeneratedPluginExports.cs"
)
NATIVE_REFERENCE_SHAPE_VALIDATION_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "ReferenceProof"
    / "NativeReferenceLoweringPlanner.ShapeValidation.cs"
)
NATIVE_REFERENCE_EMITTER_CONVERTLIKE_FAMILY_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "ReferenceProof"
    / "NativeReferenceProofEmitter.ConvertLikeFamily.cs"
)
NATIVE_REFERENCE_EMITTER_CONVERT_FAMILY_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "ReferenceProof"
    / "NativeReferenceProofEmitter.ConvertFamily.cs"
)
NATIVE_REFERENCE_EMITTER_STRING_MANAGED_INVOKE_FAMILY_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "ReferenceProof"
    / "NativeReferenceProofEmitter.StringManagedInvokeFamily.cs"
)
NATIVE_REFERENCE_EMITTER_UNSAFE_MANAGED_INVOKE_FAMILY_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "ReferenceProof"
    / "NativeReferenceProofEmitter.UnsafeManagedInvokeFamily.cs"
)
NATIVE_REFERENCE_EMITTER_MEMORY_EXTENSIONS_MANAGED_INVOKE_FAMILY_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "ReferenceProof"
    / "NativeReferenceProofEmitter.MemoryExtensionsManagedInvokeFamily.cs"
)
NATIVE_REFERENCE_EMITTER_TASK_CONTINUATION_FAMILY_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "ReferenceProof"
    / "NativeReferenceProofEmitter.TaskContinuationFamily.cs"
)
NATIVE_REFERENCE_EMITTER_GENERATED_PLUGIN_EXPORTS_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "ReferenceProof"
    / "NativeReferenceProofEmitter.GeneratedPluginExports.cs"
)
NATIVE_REFERENCE_EMITTER_SPANHELPERS_KERNEL_FAMILY_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "ReferenceProof"
    / "NativeReferenceProofEmitter.SpanHelpersKernelFamily.cs"
)
NATIVE_REFERENCE_EMITTER_ARRAY_AND_MEMORYMARSHAL_FAMILY_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "ReferenceProof"
    / "NativeReferenceProofEmitter.ArrayAndMemoryMarshalFamily.cs"
)
NATIVE_REFERENCE_EMITTER_MARSHAL_PLATFORM_CAPABILITY_FAMILY_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "ReferenceProof"
    / "NativeReferenceProofEmitter.MarshalPlatformCapabilityFamily.cs"
)
NATIVE_REFERENCE_EMITTER_INTEROP_KERNEL32_PLATFORM_CAPABILITY_FAMILY_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "ReferenceProof"
    / "NativeReferenceProofEmitter.InteropKernel32PlatformCapabilityFamily.cs"
)
NATIVE_REFERENCE_EMITTER_NATIVE_RUNTIME_EVENTSOURCE_PLATFORM_CAPABILITY_FAMILY_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "ReferenceProof"
    / "NativeReferenceProofEmitter.NativeRuntimeEventSourcePlatformCapabilityFamily.cs"
)
NATIVE_REFERENCE_EMITTER_HALF_MANAGED_INVOKE_FAMILY_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "ReferenceProof"
    / "NativeReferenceProofEmitter.HalfManagedInvokeFamily.cs"
)
NATIVE_REFERENCE_EMITTER_FLOATING_SCALAR_MANAGED_INVOKE_FAMILY_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "ReferenceProof"
    / "NativeReferenceProofEmitter.FloatingScalarManagedInvokeFamily.cs"
)
NATIVE_REFERENCE_EMITTER_NUMBER_MANAGED_INVOKE_FAMILY_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "ReferenceProof"
    / "NativeReferenceProofEmitter.NumberManagedInvokeFamily.cs"
)
NATIVE_REFERENCE_EMITTER_DECIMAL_MANAGED_INVOKE_FAMILY_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "ReferenceProof"
    / "NativeReferenceProofEmitter.DecimalManagedInvokeFamily.cs"
)
NATIVE_REFERENCE_EMITTER_CHAR_MANAGED_INVOKE_FAMILY_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "ReferenceProof"
    / "NativeReferenceProofEmitter.CharManagedInvokeFamily.cs"
)
NATIVE_REFERENCE_EMITTER_COMPANION_MANAGED_INVOKE_FAMILY_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "ReferenceProof"
    / "NativeReferenceProofEmitter.CompanionManagedInvokeFamily.cs"
)
NATIVE_REFERENCE_EMITTER_DATETIME_SEMANTIC_ENGINE_FAMILY_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "ReferenceProof"
    / "NativeReferenceProofEmitter.DateTimeSemanticEngineFamily.cs"
)
NATIVE_REFERENCE_EMITTER_GLOBALIZATION_DATETIME_SUPPORT_FAMILY_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "ReferenceProof"
    / "NativeReferenceProofEmitter.GlobalizationDateTimeSupportFamily.cs"
)
NATIVE_REFERENCE_EMITTER_VALUETYPE_KERNEL_FAMILY_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "ReferenceProof"
    / "NativeReferenceProofEmitter.ValueTypeKernelFamily.cs"
)
NATIVE_REFERENCE_EMITTER_VECTOR_KERNEL_FAMILY_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "ReferenceProof"
    / "NativeReferenceProofEmitter.VectorKernelFamily.cs"
)
NATIVE_REFERENCE_EMITTER_VECTOR_MANAGED_INVOKE_FAMILY_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "ReferenceProof"
    / "NativeReferenceProofEmitter.VectorManagedInvokeFamily.cs"
)
NATIVE_REFERENCE_EMITTER_ENUM_MANAGED_INVOKE_FAMILY_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "ReferenceProof"
    / "NativeReferenceProofEmitter.EnumManagedInvokeFamily.cs"
)
RUNTIME_SKELETON_MANAGED_INVOKE_CORE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "ReferenceProof"
    / "RuntimeSkeletonManagedInvokeCore.cs"
)
RUNTIME_SKELETON_VALUE_TYPE_MANAGED_INVOKE_CORE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "ReferenceProof"
    / "RuntimeSkeletonValueTypeManagedInvokeCore.cs"
)
RUNTIME_SKELETON_VECTOR_MANAGED_INVOKE_ABI_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "ReferenceProof"
    / "RuntimeSkeletonVectorManagedInvokeAbi.cs"
)
RUNTIME_SKELETON_SEMANTIC_REGISTRY_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "ReferenceProof"
    / "RuntimeSkeletonSemanticRegistry.cs"
)
RUNTIME_SKELETON_BACKEND_SELECTOR_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "ReferenceProof"
    / "RuntimeSkeletonBackendSelector.cs"
)
RUNTIME_SKELETON_KERNEL_CORE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "ReferenceProof"
    / "RuntimeSkeletonKernelCore.cs"
)
RUNTIME_SKELETON_VECTOR_KERNEL_CORE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "ReferenceProof"
    / "RuntimeSkeletonVectorKernelCore.cs"
)
RUNTIME_SKELETON_MARSHAL_PLATFORM_CORE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "ReferenceProof"
    / "RuntimeSkeletonMarshalPlatformCore.cs"
)
RUNTIME_SKELETON_MARSHAL_MANAGED_INVOKE_ABI_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "ReferenceProof"
    / "RuntimeSkeletonMarshalManagedInvokeAbi.cs"
)
RUNTIME_SKELETON_TASK_PLATFORM_CORE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "ReferenceProof"
    / "RuntimeSkeletonTaskPlatformCore.cs"
)
RUNTIME_SKELETON_TASK_MANAGED_INVOKE_ABI_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "ReferenceProof"
    / "RuntimeSkeletonTaskManagedInvokeAbi.cs"
)
CODEGEN_PLUGIN_ATTRIBUTES_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "Pluginization"
    / "CodeGenPluginAttributes.cs"
)
CODEGEN_PLUGIN_REGISTRIES_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "Pluginization"
    / "CodeGenPluginRegistries.cs"
)
CODEGEN_TEMPLATE_BUNDLE_PLUGINS_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "Pluginization"
    / "CodeGenTemplateBundlePlugins.cs"
)
CODEGEN_RUNTIME_PROVIDER_PLUGINS_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "Pluginization"
    / "CodeGenRuntimeProviderPlugins.cs"
)
CODEGEN_RUNTIME_PROVIDER_CATALOG_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "Pluginization"
    / "CodeGenRuntimeProviderCatalog.cs"
)
CODEGEN_GENERATOR_PROJECT_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator.Generators"
    / "Chaos.IL2CPP.Generator.Generators.csproj"
)
CODEGEN_GENERATOR_SOURCE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator.Generators"
    / "CodeGenPluginRegistryGenerator.cs"
)
RUNTIME_SKELETON_INTEROP_KERNEL32_PLATFORM_CORE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "ReferenceProof"
    / "RuntimeSkeletonInteropKernel32PlatformCore.cs"
)
RUNTIME_SKELETON_INTEROP_KERNEL32_MANAGED_INVOKE_ABI_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "ReferenceProof"
    / "RuntimeSkeletonInteropKernel32ManagedInvokeAbi.cs"
)
RUNTIME_SKELETON_NATIVE_RUNTIME_EVENTSOURCE_MANAGED_INVOKE_ABI_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "ReferenceProof"
    / "RuntimeSkeletonNativeRuntimeEventSourceManagedInvokeAbi.cs"
)
NATIVE_REFERENCE_EMITTER_STRING_MICRO_FAMILY_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "ReferenceProof"
    / "NativeReferenceProofEmitter.StringMicroFamily.cs"
)
SCRIBAN_RENDERER_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "Templating"
    / "ScribanTemplateRenderer.cs"
)
NATIVE_REFERENCE_AUDIT_SUMMARY_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "Templates"
    / "AssemblyFullClosureAuditSummary.cpp.scriban"
)
ASSEMBLY_FULL_CLOSURE_AUDIT_EMITTER_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "AssemblyFullClosureAuditEmitter.cs"
)
NATIVE_REFERENCE_RUNTIME_SKELETON_SUMMARY_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeletonSummary.cpp.scriban"
)
NATIVE_REFERENCE_RUNTIME_SKELETON_PAGE_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeletonPage.cpp.scriban"
)
NATIVE_REFERENCE_RUNTIME_SKELETON_CONSOLE_WRITE_LINE_STUB_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.ConsoleWriteLineStub.cpp.scriban"
)
NATIVE_REFERENCE_RUNTIME_SKELETON_MARSHALING_UTF8_EXPORT_STUB_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.MarshalingUtf8ExportStub.cpp.scriban"
)
NATIVE_REFERENCE_RUNTIME_SKELETON_PINVOKE_DIRECT_CALL_STUB_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.PInvokeDirectCallStub.cpp.scriban"
)
NATIVE_REFERENCE_RUNTIME_SKELETON_STATIC_CALL_CTOR_GETTER_STUB_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.StaticCallCtorGetterStub.cpp.scriban"
)
NATIVE_REFERENCE_RUNTIME_SKELETON_CONSTRUCTOR_THEN_INSTANCE_CALL_STUB_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.ConstructorThenInstanceCallStub.cpp.scriban"
)
NATIVE_REFERENCE_RUNTIME_SKELETON_DELEGATE_CLOSED_TARGET_RELAY_STUB_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.DelegateClosedTargetRelayStub.cpp.scriban"
)
NATIVE_REFERENCE_RUNTIME_SKELETON_INTERFACE_DISPATCH_MESSAGE_STUB_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.InterfaceDispatchMessageStub.cpp.scriban"
)
NATIVE_REFERENCE_RUNTIME_SKELETON_REFLECTION_INTEROP_CLOSURE_STUB_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.ReflectionInteropClosureStub.cpp.scriban"
)
NATIVE_REFERENCE_RUNTIME_SKELETON_EXCEPTION_THROW_CATCH_FINALLY_STUB_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.ExceptionThrowCatchFinallyStub.cpp.scriban"
)
NATIVE_REFERENCE_RUNTIME_SKELETON_NESTED_EXCEPTION_THROW_CATCH_FINALLY_STUB_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.NestedExceptionThrowCatchFinallyStub.cpp.scriban"
)
NATIVE_REFERENCE_RUNTIME_SKELETON_ARRAY_BOXING_REFERENCE_ARRAY_STUB_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.ArrayBoxingReferenceArrayStub.cpp.scriban"
)
NATIVE_REFERENCE_RUNTIME_SKELETON_MARSHAL_PLATFORM_FASTPATH_STUB_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.MarshalPlatformFastPathStub.cpp.scriban"
)
NATIVE_REFERENCE_RUNTIME_SKELETON_TASK_KERNEL_FASTPATH_STUB_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.TaskKernelFastPathStub.cpp.scriban"
)
NATIVE_REFERENCE_RUNTIME_SKELETON_TASK_MANAGED_INVOKE_STUB_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.TaskManagedInvokeStub.cpp.scriban"
)
NATIVE_REFERENCE_RUNTIME_SKELETON_NATIVE_RUNTIME_EVENTSOURCE_MANAGED_INVOKE_STUB_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.NativeRuntimeEventSourceManagedInvokeStub.cpp.scriban"
)
NATIVE_AOT_RUNTIME_PRELUDE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "RuntimeSupport"
    / "NativeAotLoweringPlanner.RuntimeSupport.cs"
)
NATIVE_AOT_COLLECTION_RUNTIME_PRELUDE_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "Templates"
    / "NativeAot.CollectionRuntimePrelude.cpp.scriban"
)
NATIVE_AOT_MONITOR_RUNTIME_PRELUDE_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "Templates"
    / "NativeAot.MonitorRuntimePrelude.cpp.scriban"
)
NATIVE_AOT_THREAD_RUNTIME_PRELUDE_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "Templates"
    / "NativeAot.ThreadRuntimePrelude.cpp.scriban"
)
NATIVE_AOT_ASYNC_RUNTIME_PRELUDE_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "Templates"
    / "NativeAot.AsyncRuntimePrelude.cpp.scriban"
)
NATIVE_AOT_RUNTIME_PRELUDE_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "Templates"
    / "NativeAot.RuntimePrelude.cpp.scriban"
)
NATIVE_AOT_SPAN_RUNTIME_PRELUDE_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "Templates"
    / "NativeAot.SpanRuntimePrelude.cpp.scriban"
)
NATIVE_AOT_RUNTIME_PRELUDE_AGGREGATOR_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "RuntimeSupport"
    / "NativeAotLoweringPlanner.RuntimePrelude.cs"
)
NATIVE_AOT_STATIC_INITIALIZATION_EMISSION_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "RuntimeSupport"
    / "NativeAotLoweringPlanner.StaticInitializationEmission.cs"
)
NATIVE_AOT_STATIC_INITIALIZATION_DEFINITION_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "Templates"
    / "NativeAot.StaticInitializationDefinition.cpp.scriban"
)
NATIVE_AOT_STATIC_INITIALIZATION_ACTION_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "Templates"
    / "NativeAot.StaticInitializationAction.cpp.scriban"
)
NATIVE_AOT_STATIC_INITIALIZATION_CALL_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "Templates"
    / "NativeAot.StaticInitializationCall.cpp.scriban"
)
NATIVE_AOT_INVOCATION_ABI_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "RuntimeSupport"
    / "NativeAotLoweringPlanner.ExternalRuntimeHelpers.InvocationAbi.cs"
)
NATIVE_AOT_METHOD_RETURN_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "Templates"
    / "NativeAot.MethodReturn.cpp.scriban"
)
NATIVE_AOT_ABI_RETURN_PUSH_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "Templates"
    / "NativeAot.AbiReturnPush.cpp.scriban"
)
NATIVE_AOT_ABI_ARGUMENT_INITIALIZATION_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "Templates"
    / "NativeAot.AbiArgumentInitialization.cpp.scriban"
)
NATIVE_AOT_STRING_AND_PLATFORM_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "RuntimeSupport"
    / "NativeAotLoweringPlanner.ExternalRuntimeHelpers.StringAndPlatform.cs"
)
NATIVE_AOT_STRING_JOIN_INT32_ENUMERABLE_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "Templates"
    / "NativeAot.StringJoinInt32Enumerable.cpp.scriban"
)
NATIVE_AOT_STRING_JOIN_STRING_ENUMERABLE_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "Templates"
    / "NativeAot.StringJoinStringEnumerable.cpp.scriban"
)
NATIVE_AOT_SIMPLE_EXTERNAL_RUNTIME_HELPER_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "Templates"
    / "NativeAot.SimpleExternalRuntimeHelper.cpp.scriban"
)
NATIVE_AOT_TESTFRAMEWORK_EQUAL_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "Templates"
    / "NativeAot.TestFrameworkEqual.cpp.scriban"
)
NATIVE_AOT_METADATA_SUPPORT_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "Planning"
    / "NativeAotLoweringPlanner.MetadataSupport.cs"
)
NATIVE_AOT_INVOCATION_PLANNING_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "Planning"
    / "NativeAotLoweringPlanner.InvocationPlanning.cs"
)
NATIVE_AOT_METHOD_EMISSION_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "Emission"
    / "NativeAotLoweringPlanner.MethodEmission.cs"
)
NATIVE_AOT_EXCEPTION_EMISSION_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "Emission"
    / "NativeAotLoweringPlanner.ExceptionEmission.cs"
)
NATIVE_AOT_EXTERNAL_RUNTIME_HELPERS_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "RuntimeSupport"
    / "NativeAotLoweringPlanner.ExternalRuntimeHelpers.cs"
)
NATIVE_AOT_RUNTIME_HELPER_SHAPE_REGISTRY_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "RuntimeSupport"
    / "RuntimeHelperShapeRegistry.cs"
)
NATIVE_AOT_OBJECT_MODEL_EMISSION_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "Emission"
    / "NativeAotLoweringPlanner.ObjectModelEmission.cs"
)
NATIVE_AOT_OBJECT_EQUALITY_EMISSION_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "Emission"
    / "NativeAotLoweringPlanner.ObjectEqualityEmission.cs"
)
NATIVE_AOT_REFLECTION_OBJECT_EMISSION_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "Emission"
    / "NativeAotLoweringPlanner.ReflectionObjectEmission.cs"
)
NATIVE_AOT_OBJECT_MODEL_UTILITIES_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "Emission"
    / "NativeAotLoweringPlanner.ObjectModelUtilities.cs"
)
NATIVE_AOT_EXTERNAL_RUNTIME_STRING_AND_PLATFORM_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "RuntimeSupport"
    / "NativeAotLoweringPlanner.ExternalRuntimeHelpers.StringAndPlatform.cs"
)
NATIVE_AOT_EXTERNAL_RUNTIME_COLLECTION_AND_REFLECTION_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "RuntimeSupport"
    / "NativeAotLoweringPlanner.ExternalRuntimeHelpers.CollectionAndReflection.cs"
)
NATIVE_AOT_EXTERNAL_RUNTIME_TYPE_RESOLUTION_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "RuntimeSupport"
    / "NativeAotLoweringPlanner.ExternalRuntimeHelpers.TypeResolution.cs"
)
NATIVE_AOT_EXTERNAL_RUNTIME_INVOCATION_ABI_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "RuntimeSupport"
    / "NativeAotLoweringPlanner.ExternalRuntimeHelpers.InvocationAbi.cs"
)
NATIVE_AOT_EXTERNAL_RUNTIME_ASSERTIONS_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "RuntimeSupport"
    / "NativeAotLoweringPlanner.ExternalRuntimeHelpers.Assertions.cs"
)
TEST_FRAMEWORK_SUBJECT_FAMILY_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
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

NATIVE_REFERENCE_EMITTER_COLLECTIONS_KERNEL_FAMILY_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "ReferenceProof"
    / "NativeReferenceProofEmitter.CollectionsKernelFamily.cs"
)
NATIVE_REFERENCE_EMITTER_COLLECTIONS_KERNEL_ABI_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "ReferenceProof"
    / "RuntimeSkeletonCollectionsKernelAbi.cs"
)
NATIVE_REFERENCE_EMITTER_BUFFER_MEMORY_KERNEL_FAMILY_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "ReferenceProof"
    / "NativeReferenceProofEmitter.BufferMemoryKernelFamily.cs"
)
NATIVE_REFERENCE_EMITTER_BUFFER_MEMORY_KERNEL_ABI_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "ReferenceProof"
    / "RuntimeSkeletonBufferMemoryKernelAbi.cs"
)
NATIVE_REFERENCE_EMITTER_COLLECTIONS_MANAGED_INVOKE_FAMILY_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "ReferenceProof"
    / "NativeReferenceProofEmitter.CollectionsManagedInvokeFamily.cs"
)
NATIVE_REFERENCE_EMITTER_COLLECTIONS_MANAGED_INVOKE_ABI_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "ReferenceProof"
    / "RuntimeSkeletonCollectionsManagedInvokeAbi.cs"
)
NATIVE_REFERENCE_RUNTIME_SKELETON_COLLECTIONS_KERNEL_STUB_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.CollectionsKernelStub.cpp.scriban"
)
NATIVE_REFERENCE_RUNTIME_SKELETON_BUFFER_MEMORY_KERNEL_STUB_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.BufferMemoryKernelStub.cpp.scriban"
)

NATIVE_AOT_GENERIC_REGISTRATION_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "Emission"
    / "NativeAotLoweringPlanner.GenericRegistration.cs"
)

CODEGEN_BRIDGE_HEADER_PATH = (
    REPO_ROOT
    / "contracts"
    / "native"
    / "v0"
    / "codegen_bridge.h"
)

class Il2CppCodeGenStructureGovernanceTestSupport(unittest.TestCase):
    pass
