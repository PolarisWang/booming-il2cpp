# AutoTestGenerator: System.Reflection.Metadata.BlobBuilder

- Assembly: `System.Reflection.Metadata`
- Type: `System.Reflection.Metadata.BlobBuilder`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 49 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 0 |
| Benchmark-only (pure void / non-deterministic) | 47 |
| Skipped (generic / unresolvable) | 2 |
| Total Subjects | 94 |

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

- `GetBlobs (ref struct return type: BlobBuilder.Blobs)` — contains generic parameters, needs manual handling
- `WriteInt16` — contains generic parameters, needs manual handling
