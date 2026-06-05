# AutoTestGenerator: System.Text.Json.Serialization.JsonConverter<System.Int32>

- Assembly: `System.Text.Json`
- Type: `System.Text.Json.Serialization.JsonConverter<System.Int32>`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 5 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 3 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 2 |
| Total Subjects | 5 |

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

- `Read (ref struct parameter: System.Text.Json.Utf8JsonReader)` — contains generic parameters, needs manual handling
- `ReadAsPropertyName (ref struct parameter: System.Text.Json.Utf8JsonReader)` — contains generic parameters, needs manual handling
