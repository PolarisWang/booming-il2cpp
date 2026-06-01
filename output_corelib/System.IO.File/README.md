# AutoTestGenerator: System.IO.File

- Assembly: `System.Private.CoreLib`
- Type: `System.IO.File`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 105 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 1 |
| Benchmark-only (pure void / non-deterministic) | 98 |
| Skipped (generic / unresolvable) | 6 |
| Total Subjects | 220 |

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

- `WriteAllText (ref struct)` — contains generic parameters, needs manual handling
- `WriteAllText (ref struct)` — contains generic parameters, needs manual handling
- `WriteAllBytes (ref struct)` — contains generic parameters, needs manual handling
- `AppendAllBytes (ref struct)` — contains generic parameters, needs manual handling
- `AppendAllText (ref struct)` — contains generic parameters, needs manual handling
- `AppendAllText (ref struct)` — contains generic parameters, needs manual handling
