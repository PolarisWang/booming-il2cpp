# AutoTestGenerator: System.Xml.Serialization.CodeIdentifiers

- Assembly: `System.Private.Xml`
- Type: `System.Xml.Serialization.CodeIdentifiers`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 10 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 5 |
| Benchmark-only (pure void / non-deterministic) | 5 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 18 |

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
