# AutoTestGenerator: System.RuntimeMethodHandle

- Assembly: `System.Private.CoreLib`
- Type: `System.RuntimeMethodHandle`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 4 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 0 |
| Benchmark-only (pure void / non-deterministic) | 3 |
| Skipped (generic / unresolvable) | 1 |
| Total Subjects | 4 |

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
