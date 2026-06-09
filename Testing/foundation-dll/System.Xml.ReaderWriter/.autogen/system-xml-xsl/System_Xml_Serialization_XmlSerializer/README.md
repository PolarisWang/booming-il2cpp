# AutoTestGenerator: System.Xml.Serialization.XmlSerializer

- Assembly: `System.Private.Xml`
- Type: `System.Xml.Serialization.XmlSerializer`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 20 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 8 |
| Benchmark-only (pure void / non-deterministic) | 12 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 35 |

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
