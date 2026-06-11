# AutoTestGenerator: System.Xml.XmlDocument

- Assembly: `System.Private.Xml`
- Type: `System.Xml.XmlDocument`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 39 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 13 |
| Benchmark-only (pure void / non-deterministic) | 26 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 85 |

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
