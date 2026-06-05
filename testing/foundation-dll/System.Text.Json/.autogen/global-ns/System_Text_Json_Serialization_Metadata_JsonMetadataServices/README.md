# AutoTestGenerator: System.Text.Json.Serialization.Metadata.JsonMetadataServices

- Assembly: `System.Text.Json`
- Type: `System.Text.Json.Serialization.Metadata.JsonMetadataServices`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 30 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 27 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 3 |
| Total Subjects | 31 |

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

- `CreateIAsyncEnumerableInfo` — contains generic parameters, needs manual handling
- `CreateIDictionaryInfo` — contains generic parameters, needs manual handling
- `CreatePropertyInfo (unprobable return type: System.Text.Json.Serialization.Metadata.JsonPropertyInfo)` — contains generic parameters, needs manual handling
