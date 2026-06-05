# AutoTestGenerator: System.Text.Json.Serialization.Metadata.JsonTypeInfoResolver

- Assembly: `System.Text.Json`
- Type: `System.Text.Json.Serialization.Metadata.JsonTypeInfoResolver`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 3 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 2 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 1 |
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

- `Combine (ref struct parameter: System.ReadOnlySpan<System.Text.Json.Serialization.Metadata.IJsonTypeInfoResolver>)` — contains generic parameters, needs manual handling
