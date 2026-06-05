# AutoTestGenerator: System.Reflection.Metadata.BlobReader

- Assembly: `System.Reflection.Metadata`
- Type: `System.Reflection.Metadata.BlobReader`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 33 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 32 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 1 |
| Total Subjects | 43 |

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

- `Reset (enumerator method projected by MLC)` — contains generic parameters, needs manual handling
