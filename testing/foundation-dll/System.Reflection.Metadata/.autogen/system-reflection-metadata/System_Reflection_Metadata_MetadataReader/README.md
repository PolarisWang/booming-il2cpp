# AutoTestGenerator: System.Reflection.Metadata.MetadataReader

- Assembly: `System.Reflection.Metadata`
- Type: `System.Reflection.Metadata.MetadataReader`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 49 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 49 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 50 |

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
