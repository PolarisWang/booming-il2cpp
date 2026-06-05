# AutoTestGenerator: System.Text.Json.Nodes.JsonArray

- Assembly: `System.Text.Json`
- Type: `System.Text.Json.Nodes.JsonArray`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 12 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 11 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 1 |
| Total Subjects | 14 |

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

- `Create` — contains generic parameters, needs manual handling
