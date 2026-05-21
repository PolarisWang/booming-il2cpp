from __future__ import annotations

import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
METHOD_EMISSION_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "Emission"
    / "NativeAotLoweringPlanner.MethodEmission.cs"
)
CONTRACT_MODELS_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Contracts"
    / "ManagedClosureModels.cs"
)
ARTIFACT_CONTRACTS_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Contracts"
    / "TypedIlAndAotCoreIrContracts.cs"
)
METADATA_RESOLUTION_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Loader"
    / "LoaderStage.MetadataResolution.cs"
)
EXCEPTION_EMISSION_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "Emission"
    / "NativeAotLoweringPlanner.ExceptionEmission.cs"
)
INVOCATION_PLANNING_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "Planning"
    / "NativeAotLoweringPlanner.InvocationPlanning.cs"
)
LOWERING_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "AotCoreIrLowering.cs"
)
NATIVE_LIBRARY_PATH = (
    REPO_ROOT
    / "src"
    / "native"
    / "runtime-core"
    / "native_library.cpp"
)
ENGINE_BINDING_PATH = (
    REPO_ROOT
    / "src"
    / "native"
    / "runtime-core"
    / "engine_binding.h"
)
RUNTIME_SUPPORT_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.RuntimeSupport"
    / "PInvokeResolverRegistry.cs"
)


class PInvokeCodegenFixTests(unittest.TestCase):
    """Verify the three P/Invoke DllImport capability fixes:
    1. GC_TRANSITION macro insertion
    2. __Internal link support
    3. SuppressGCTransition attribute support
    """

    def test_managed_import_model_exposes_issuppressgctransition(self) -> None:
        source = CONTRACT_MODELS_PATH.read_text(encoding="utf-8")
        self.assertIn("IsSuppressGCTransition", source)

    def test_artifact_exposes_isinternallink(self) -> None:
        source = ARTIFACT_CONTRACTS_PATH.read_text(encoding="utf-8")
        self.assertIn("IsInternalLink", source)

    def test_artifact_exposes_issuppressgctransition(self) -> None:
        source = ARTIFACT_CONTRACTS_PATH.read_text(encoding="utf-8")
        self.assertIn("IsSuppressGCTransition", source)

    def test_metadata_resolves_internal_module_name(self) -> None:
        source = METADATA_RESOLUTION_PATH.read_text(encoding="utf-8")
        self.assertIn('moduleName = ""', source)

    def test_metadata_resolves_suppressgctransition_attribute(self) -> None:
        source = METADATA_RESOLUTION_PATH.read_text(encoding="utf-8")
        self.assertIn("SuppressGCTransitionAttribute", source)
        self.assertIn("IsSuppressGCTransition", source)

    def test_lowering_passes_isinternallink(self) -> None:
        source = LOWERING_PATH.read_text(encoding="utf-8")
        self.assertIn("IsInternalLink", source)

    def test_lowering_passes_issuppressgctransition(self) -> None:
        source = LOWERING_PATH.read_text(encoding="utf-8")
        self.assertIn("IsSuppressGCTransition", source)

    def test_codegen_emits_gc_transition_macros(self) -> None:
        source = METHOD_EMISSION_PATH.read_text(encoding="utf-8")
        self.assertIn("GC_TRANSITION_TO_PREEMPTIVE", source)
        self.assertIn("GC_TRANSITION_TO_COOPERATIVE", source)

    def test_codegen_gates_gc_transition_on_flag(self) -> None:
        source = METHOD_EMISSION_PATH.read_text(encoding="utf-8")
        self.assertIn("bool needsGcTransition = !method.IsSuppressGCTransition", source)
        self.assertIn("if (needsGcTransition)", source)

    def test_codegen_has_internal_guard_for_loadlibrary(self) -> None:
        source = METHOD_EMISSION_PATH.read_text(encoding="utf-8")
        self.assertIn("if (!isInternal)", source)
        self.assertIn("NativeLibraryLoad", source)
        self.assertIn("NativeLibraryGetProcAddress", source)

    def test_codegen_has_extern_c_declaration(self) -> None:
        source = METHOD_EMISSION_PATH.read_text(encoding="utf-8")
        self.assertIn("extern", source)

    def test_codegen_has_internal_path(self) -> None:
        source = METHOD_EMISSION_PATH.read_text(encoding="utf-8")
        self.assertIn("if (isInternal)", source)

    # ── DllImportResolver (P/Invoke Override) tests ────────────────────

    def test_artifact_exposes_declaring_assembly_name(self) -> None:
        source = ARTIFACT_CONTRACTS_PATH.read_text(encoding="utf-8")
        self.assertIn("DeclaringAssemblyName", source)

    def test_codegen_emits_try_resolve_dllimport(self) -> None:
        source = METHOD_EMISSION_PATH.read_text(encoding="utf-8")
        self.assertIn("TryResolveDllImport", source)

    def test_codegen_gates_resolver_on_known_assembly(self) -> None:
        source = METHOD_EMISSION_PATH.read_text(encoding="utf-8")
        self.assertIn("isDeclaringAssemblyKnown", source)

    def test_native_has_register_pinvoke_resolver_callback(self) -> None:
        source = ENGINE_BINDING_PATH.read_text(encoding="utf-8")
        self.assertIn("RegisterPInvokeResolverCallback", source)

    def test_native_has_try_resolve_dllimport(self) -> None:
        source = ENGINE_BINDING_PATH.read_text(encoding="utf-8")
        self.assertIn("TryResolveDllImport", source)

    def test_native_extern_c_wrapper_exists(self) -> None:
        """Verify native_library.cpp has an extern \"C\" wrapper for
        RegisterPInvokeResolverCallback (for __Internal P/Invoke access)."""
        source = NATIVE_LIBRARY_PATH.read_text(encoding="utf-8")
        self.assertIn("extern \"C\" void RegisterPInvokeResolverCallback", source)

    def test_managed_resolver_registry_exists(self) -> None:
        source = RUNTIME_SUPPORT_PATH.read_text(encoding="utf-8")
        self.assertIn("PInvokeResolverRegistry", source)
        self.assertIn("SetDllImportResolver", source)
        self.assertIn("UnmanagedCallersOnly", source)
        self.assertIn("TryResolveCallback", source)


    # ── COM Interop (P2) tests ─────────────────────────────────────────

    def test_artifact_exposes_ispreservesig(self) -> None:
        source = ARTIFACT_CONTRACTS_PATH.read_text(encoding="utf-8")
        self.assertIn("IsPreserveSig", source)

    def test_codegen_com_vtable_dispatch_in_emission(self) -> None:
        source = EXCEPTION_EMISSION_PATH.read_text(encoding="utf-8")
        self.assertIn("HybridDispatchKind.ComVtable", source)

    def test_codegen_com_vtable_planning_layer(self) -> None:
        source = INVOCATION_PLANNING_PATH.read_text(encoding="utf-8")
        self.assertIn("HybridDispatchKind.ComVtable", source)


if __name__ == "__main__":
    unittest.main()
