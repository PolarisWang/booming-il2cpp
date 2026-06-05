# AutoTestGenerator: System.Reflection.Metadata.BlobWriter

- Assembly: `System.Reflection.Metadata`
- Type: `System.Reflection.Metadata.BlobWriter`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 43 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 42 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 1 |
| Total Subjects | 87 |

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

- `WriteInt16` — contains generic parameters, needs manual handling
