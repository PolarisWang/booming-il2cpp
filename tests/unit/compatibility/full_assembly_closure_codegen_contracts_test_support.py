from __future__ import annotations

import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
ARTIFACT_MODELS_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Contracts" / "ManagedClosureArtifactModels.cs"
CODEGEN_STAGE_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "CodeGenStage.cs"
NATIVE_REFERENCE_EMITTER_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "NativeReferenceProofEmitter.cs"
NATIVE_AOT_EMITTER_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "NativeAotEmitter.cs"
RUNTIME_SKELETON_INTERFACE_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.InterfaceDispatchMessageStub.cpp.scriban"
)
RUNTIME_SKELETON_PAGE_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeletonPage.cpp.scriban"
)
RUNTIME_SKELETON_STATIC_INT_FORWARDER_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.StaticIntForwarderStub.cpp.scriban"
)
RUNTIME_SKELETON_STATIC_BOOL_IDENTITY_FORWARDER_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.StaticBoolIdentityForwarderStub.cpp.scriban"
)
RUNTIME_SKELETON_STATIC_BOOL_PRODUCER_FORWARDER_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.StaticBoolProducerForwarderStub.cpp.scriban"
)
RUNTIME_SKELETON_STATIC_STRING_RETURN_FORWARDER_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.StaticStringReturnForwarderStub.cpp.scriban"
)
RUNTIME_SKELETON_STATIC_PRIMITIVE_CONVERT_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.StaticPrimitiveConvertStub.cpp.scriban"
)
RUNTIME_SKELETON_STATIC_CHECKED_PRIMITIVE_CONVERT_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.StaticCheckedByteConvertStub.cpp.scriban"
)
RUNTIME_SKELETON_STATIC_BYTE_FORWARDER_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.StaticByteForwarderStub.cpp.scriban"
)
RUNTIME_SKELETON_STATIC_EXCEPTION_THROW_STRING_PRODUCER_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.StaticExceptionThrowStringProducerStub.cpp.scriban"
)
RUNTIME_SKELETON_STRING_MANAGED_INVOKE_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.StringManagedInvokeStub.cpp.scriban"
)
RUNTIME_SKELETON_UNSAFE_MANAGED_INVOKE_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.UnsafeManagedInvokeStub.cpp.scriban"
)
RUNTIME_SKELETON_MEMORY_EXTENSIONS_MANAGED_INVOKE_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.MemoryExtensionsManagedInvokeStub.cpp.scriban"
)
RUNTIME_SKELETON_SPANHELPERS_KERNEL_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.SpanHelpersKernelStub.cpp.scriban"
)
RUNTIME_SKELETON_ARRAY_AND_MEMORYMARSHAL_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.ArrayAndMemoryMarshalStub.cpp.scriban"
)
RUNTIME_SKELETON_MARSHAL_PLATFORM_FASTPATH_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.MarshalPlatformFastPathStub.cpp.scriban"
)
RUNTIME_SKELETON_TASK_KERNEL_FASTPATH_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.TaskKernelFastPathStub.cpp.scriban"
)
RUNTIME_SKELETON_TASK_MANAGED_INVOKE_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.TaskManagedInvokeStub.cpp.scriban"
)
RUNTIME_SKELETON_NATIVE_RUNTIME_EVENTSOURCE_MANAGED_INVOKE_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.NativeRuntimeEventSourceManagedInvokeStub.cpp.scriban"
)
RUNTIME_SKELETON_IDENTITY_STRUCT_MANAGED_INVOKE_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.IdentityStructManagedInvokeStub.cpp.scriban"
)
RUNTIME_SKELETON_HALF_MANAGED_INVOKE_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.HalfManagedInvokeStub.cpp.scriban"
)
RUNTIME_SKELETON_FLOATING_SCALAR_MANAGED_INVOKE_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.FloatingScalarManagedInvokeStub.cpp.scriban"
)
RUNTIME_SKELETON_NUMBER_MANAGED_INVOKE_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.NumberManagedInvokeStub.cpp.scriban"
)
RUNTIME_SKELETON_DECIMAL_MANAGED_INVOKE_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.DecimalManagedInvokeStub.cpp.scriban"
)
RUNTIME_SKELETON_CHAR_MANAGED_INVOKE_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.CharManagedInvokeStub.cpp.scriban"
)
RUNTIME_SKELETON_COMPANION_MANAGED_INVOKE_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.CompanionManagedInvokeStub.cpp.scriban"
)
RUNTIME_SKELETON_DATETIME_SEMANTIC_ENGINE_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.DateTimeSemanticEngineStub.cpp.scriban"
)
RUNTIME_SKELETON_GLOBALIZATION_DATETIME_SUPPORT_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.GlobalizationDateTimeSupportStub.cpp.scriban"
)
RUNTIME_SKELETON_VALUETYPE_KERNEL_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.ValueTypeKernelStub.cpp.scriban"
)
RUNTIME_SKELETON_VECTOR_CAPABILITY_QUERY_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.VectorCapabilityQueryStub.cpp.scriban"
)
RUNTIME_SKELETON_ENUM_MANAGED_INVOKE_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.EnumManagedInvokeStub.cpp.scriban"
)

class FullAssemblyClosureCodegenContractTestSupport(unittest.TestCase):
    pass
