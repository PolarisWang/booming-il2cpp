# AutoTestGenerator: System.Boolean

- Assembly: `System.Private.CoreLib`
- Type: `System.Boolean`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 8 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 4 |
| Benchmark-only (pure void / non-deterministic) | 1 |
| Skipped (generic / unresolvable) | 3 |
| Total Subjects | 8 |

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

- `TryFormat (ref struct)` — contains generic parameters, needs manual handling
- `Parse (ref struct)` — contains generic parameters, needs manual handling
- `TryParse (ref struct)` — contains generic parameters, needs manual handling
