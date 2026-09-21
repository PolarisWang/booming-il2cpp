# AutoTestGenerator: System.AppDomain

- Assembly: `System.Private.CoreLib`
- Type: `System.AppDomain`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 42 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 11 |
| Benchmark-only (pure void / non-deterministic) | 30 |
| Skipped (generic / unresolvable) | 1 |
| Total Subjects | 116 |

## Build

```bash
# Build with verification (Assert enabled)
dotnet build -p:DefineConstants=VERIFY

# Build for benchmark (Assert eliminated)
dotnet build
```

## Run

```bash
# Run with verification
dotnet run --project .
```

## Skipped Methods

- `ExecuteAssembly (unprobable parameter type: System.Configuration.Assemblies.AssemblyHashAlgorithm)` — contains generic parameters, needs manual handling
