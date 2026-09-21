# AutoTestGenerator: System.WeakReference<System.String>

- Assembly: `System.Private.CoreLib`
- Type: `System.WeakReference<System.String>`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 3 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 1 |
| Benchmark-only (pure void / non-deterministic) | 1 |
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
