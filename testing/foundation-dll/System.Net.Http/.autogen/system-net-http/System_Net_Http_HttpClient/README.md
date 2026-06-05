# AutoTestGenerator: System.Net.Http.HttpClient

- Assembly: `System.Net.Http`
- Type: `System.Net.Http.HttpClient`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 45 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 45 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 63 |

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
