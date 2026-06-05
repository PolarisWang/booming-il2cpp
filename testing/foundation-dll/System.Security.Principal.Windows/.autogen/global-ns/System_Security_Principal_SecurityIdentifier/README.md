# AutoTestGenerator: System.Security.Principal.SecurityIdentifier

- Assembly: `System.Security.Principal.Windows`
- Type: `System.Security.Principal.SecurityIdentifier`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 7 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 6 |
| Benchmark-only (pure void / non-deterministic) | 1 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 10 |

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
