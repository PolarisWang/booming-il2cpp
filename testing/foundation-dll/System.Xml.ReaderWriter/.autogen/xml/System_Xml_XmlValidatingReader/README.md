# AutoTestGenerator: System.Xml.XmlValidatingReader

- Assembly: `System.Private.Xml`
- Type: `System.Xml.XmlValidatingReader`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 21 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 1 |
| Benchmark-only (pure void / non-deterministic) | 20 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 46 |

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
