# dotnet-foundation

Repository-owned reference bundle for `SolutionCorePack` binary-reference archetypes.

Contents:

- `lib/ReferenceGreeter.dll`
  - Small repository-owned managed assembly consumed through `<Reference>` / `HintPath`.
- `net8.0/*.dll`
  - Selected .NET 8 reference assemblies mirrored from the local SDK pack for the `CoreLibReferenceSolution` archetype.
- `src/ReferenceGreeter/`
  - Source project used to rebuild `ReferenceGreeter.dll` when the bundle changes.

This bundle is intentionally small. It exists to validate repository-owned binary-reference closure without binding tests to machine-global SDK paths.
