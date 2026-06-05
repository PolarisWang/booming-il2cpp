# AutoTestGenerator: System.Text.Json.JsonSerializerOptions

- Assembly: `System.Text.Json`
- Type: `System.Text.Json.JsonSerializerOptions`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 5 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 3 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 2 |
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

- `GetTypeInfo (unprobable return type: System.Text.Json.Serialization.Metadata.JsonTypeInfo)` — contains generic parameters, needs manual handling
- `TryGetTypeInfo (unprobable parameter type: System.Text.Json.Serialization.Metadata.JsonTypeInfo)` — contains generic parameters, needs manual handling
