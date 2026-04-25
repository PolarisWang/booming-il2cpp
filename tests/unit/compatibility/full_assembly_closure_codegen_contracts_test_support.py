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
RUNTIME_SKELETON_STATIC_CHECKED_BYTE_CONVERT_TEMPLATE_PATH = (
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

class FullAssemblyClosureCodegenContractTestSupport(unittest.TestCase):
    pass
