# AutoTestGenerator: System.TimeZoneInfo

- Assembly: `System.Private.CoreLib`
- Type: `System.TimeZoneInfo`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 33 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 1 |
| Benchmark-only (pure void / non-deterministic) | 32 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 73 |

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
