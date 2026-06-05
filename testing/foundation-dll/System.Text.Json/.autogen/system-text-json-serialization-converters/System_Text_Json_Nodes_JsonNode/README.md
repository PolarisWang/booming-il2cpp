# AutoTestGenerator: System.Text.Json.Nodes.JsonNode

- Assembly: `System.Text.Json`
- Type: `System.Text.Json.Nodes.JsonNode`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 18 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 16 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 2 |
| Total Subjects | 20 |

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

- `Parse (ref struct parameter: System.Text.Json.Utf8JsonReader)` — contains generic parameters, needs manual handling
- `Parse (ref struct parameter: System.ReadOnlySpan<System.Byte>)` — contains generic parameters, needs manual handling
