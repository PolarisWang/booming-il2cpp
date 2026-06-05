# AutoTestGenerator: System.Threading.Tasks.Parallel

- Assembly: `System.Threading.Tasks.Parallel`
- Type: `System.Threading.Tasks.Parallel`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 43 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 41 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 2 |
| Total Subjects | 112 |

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

- `Invoke` — contains generic parameters, needs manual handling
- `Invoke` — contains generic parameters, needs manual handling
