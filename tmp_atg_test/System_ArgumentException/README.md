# AutoTestGenerator: System.ArgumentException

- Assembly: `System.Private.CoreLib`
- Type: `System.ArgumentException`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 3 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 2 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 1 |
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

- `GetObjectData` — contains generic parameters, needs manual handling
