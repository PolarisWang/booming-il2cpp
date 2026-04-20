from __future__ import annotations

import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
LOADER_STAGE_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Loader" / "LoaderStage.cs"
LOADER_CROSS_ASSEMBLY_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Loader" / "LoaderStage.CrossAssemblyInstantiation.cs"
LINKER_STAGE_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Linker" / "LinkerStage.cs"
LINKER_REACHABILITY_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Linker" / "LinkerStage.Reachability.cs"


class FullAssemblyClosureWithoutEntryPointTests(unittest.TestCase):
    def test_loader_allows_full_assembly_closure_to_continue_without_entry_point(self) -> None:
        loader_source = LOADER_STAGE_PATH.read_text(encoding="utf-8")
        cross_assembly_source = LOADER_CROSS_ASSEMBLY_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "var entryAssembly = ResolveEntryAssembly(loadedAssemblies, request.EntryPointSubjectIdOverride, request.FullAssemblyClosure);",
            'var entryPointSubjectId = !string.IsNullOrWhiteSpace(request.EntryPointSubjectIdOverride)',
            ': request.FullAssemblyClosure',
            '? string.Empty',
        ]:
            self.assertIn(required_fragment, loader_source)

        for required_fragment in [
            "string? entryPointSubjectIdOverride,",
            "bool fullAssemblyClosure)",
            "if (fullAssemblyClosure)",
            "return primaryAssembly;",
        ]:
            self.assertIn(required_fragment, cross_assembly_source)

    def test_linker_reachability_supports_full_assembly_closure_without_entry_seed(self) -> None:
        linker_source = LINKER_STAGE_PATH.read_text(encoding="utf-8")
        reachability_source = LINKER_REACHABILITY_PATH.read_text(encoding="utf-8")

        self.assertIn(
            '.OrderBy(method => string.IsNullOrWhiteSpace(semanticWorld.EntryPointSubjectId) ? 1 : string.Equals(method.SubjectId, semanticWorld.EntryPointSubjectId, StringComparison.Ordinal) ? 0 : 1)',
            linker_source,
        )

        for required_fragment in [
            "if (semanticWorld.FullAssemblyClosure &&",
            "semanticWorld.Assemblies.Count == 1)",
            "return new ReachableClosure(",
            "ManagedMethodModel? entryPointMethod = null;",
            "!string.IsNullOrWhiteSpace(semanticWorld.EntryPointSubjectId)",
            "if (entryPointMethod is not null)",
            "pendingMethods.Enqueue(entryPointMethod);",
        ]:
            self.assertIn(required_fragment, reachability_source)


if __name__ == "__main__":
    unittest.main()
