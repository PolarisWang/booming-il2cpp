# AutoTestGenerator: System.StringComparer

- Assembly: `System.Private.CoreLib`
- Type: `System.StringComparer`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 7 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 0 |
| Benchmark-only (pure void / non-deterministic) | 5 |
| Skipped (generic / unresolvable) | 2 |
| Total Subjects | 13 |

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

- `Create` — contains generic parameters, needs manual handling
- `Create` — contains generic parameters, needs manual handling
