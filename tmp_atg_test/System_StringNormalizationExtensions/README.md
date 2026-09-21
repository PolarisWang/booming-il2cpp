# AutoTestGenerator: System.StringNormalizationExtensions

- Assembly: `System.Private.CoreLib`
- Type: `System.StringNormalizationExtensions`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 7 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 0 |
| Benchmark-only (pure void / non-deterministic) | 2 |
| Skipped (generic / unresolvable) | 5 |
| Total Subjects | 5 |

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

- `IsNormalized (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `Normalize` — contains generic parameters, needs manual handling
- `Normalize` — contains generic parameters, needs manual handling
- `TryNormalize (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
- `GetNormalizedLength (ref struct parameter: System.ReadOnlySpan<System.Char>)` — contains generic parameters, needs manual handling
