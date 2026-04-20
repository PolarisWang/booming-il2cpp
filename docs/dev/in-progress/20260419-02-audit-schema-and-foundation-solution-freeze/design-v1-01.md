# Audit Schema And Foundation Solution Freeze Design v1.01

## 1. Goal

Freeze the minimum Phase 0 contract for auditable DLL translation:

- A machine-readable audit packet schema.
- A machine-readable codegen review schema.
- A user-visible `FoundationDllTranslationSolution.sln`.
- Registration in `SolutionCorePack` engineering scenarios.

## 2. Boundary

This child task does not claim any DLL is fully translated.

It only creates the stable contract and subject entry needed by later tasks:

- `20260419-03-system-private-corelib-full-verification`
- ordered Complex BCL per-DLL tasks

## 3. Schema Placement

Schemas live under:

```text
contracts/artifacts/v0/schemas/
```

New schemas:

- `foundation-dll-audit-packet.schema.json`
- `foundation-codegen-review.schema.json`

## 4. Subject Placement

The new solution lives under:

```text
subjects/SolutionCorePack/source/EngineeringScenarios/FoundationDllTranslationSolution/
```

The solution starts with a single App project that compiles and describes the current controlled foundation inputs. It intentionally does not reference every future DLL yet; per-DLL tasks will expand the subject source when their inputs are promoted and verified.

## 5. Registration

`subjects/SolutionCorePack/subject.features.json` gains:

```json
{
  "code": 5,
  "id": "solution-foundation-dll-translation",
  "sourcePath": "subjects/SolutionCorePack/source/EngineeringScenarios/FoundationDllTranslationSolution"
}
```

The main `SolutionCorePack.sln` and host project reference the new App project so the user can build it from both the scenario solution and the canonical subject solution.

