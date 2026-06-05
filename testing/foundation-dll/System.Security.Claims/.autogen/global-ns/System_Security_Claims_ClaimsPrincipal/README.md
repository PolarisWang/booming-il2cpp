# AutoTestGenerator: System.Security.Claims.ClaimsPrincipal

- Assembly: `System.Security.Claims`
- Type: `System.Security.Claims.ClaimsPrincipal`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 11 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 8 |
| Benchmark-only (pure void / non-deterministic) | 3 |
| Skipped (generic / unresolvable) | 0 |
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
