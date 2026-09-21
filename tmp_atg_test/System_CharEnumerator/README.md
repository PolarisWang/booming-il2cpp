# AutoTestGenerator: System.CharEnumerator

- Assembly: `System.Private.CoreLib`
- Type: `System.CharEnumerator`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 4 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 0 |
| Benchmark-only (pure void / non-deterministic) | 1 |
| Skipped (generic / unresolvable) | 3 |
| Total Subjects | 1 |

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

- `MoveNext (enumerator method projected by MLC)` — contains generic parameters, needs manual handling
- `Dispose (enumerator type itself)` — contains generic parameters, needs manual handling
- `Reset (enumerator method projected by MLC)` — contains generic parameters, needs manual handling
