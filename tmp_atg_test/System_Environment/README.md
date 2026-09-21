# AutoTestGenerator: System.Environment

- Assembly: `System.Private.CoreLib`
- Type: `System.Environment`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 14 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 5 |
| Benchmark-only (pure void / non-deterministic) | 7 |
| Skipped (generic / unresolvable) | 2 |
| Total Subjects | 28 |

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

- `GetFolderPath (ref struct parameter: Environment.SpecialFolder)` — contains generic parameters, needs manual handling
- `GetFolderPath (ref struct parameter: Environment.SpecialFolder)` — contains generic parameters, needs manual handling
