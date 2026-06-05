# AutoTestGenerator: System.Text.Json.JsonDocument

- Assembly: `System.Text.Json`
- Type: `System.Text.Json.JsonDocument`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 10 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 8 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 2 |
| Total Subjects | 11 |

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

- `TryParseValue (ref struct parameter: System.Text.Json.Utf8JsonReader)` — contains generic parameters, needs manual handling
- `ParseValue (ref struct parameter: System.Text.Json.Utf8JsonReader)` — contains generic parameters, needs manual handling
