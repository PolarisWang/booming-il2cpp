# AutoTestGenerator: System.Xml.XmlNamedNodeMap

- Assembly: `System.Private.Xml`
- Type: `System.Xml.XmlNamedNodeMap`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 7 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 0 |
| Benchmark-only (pure void / non-deterministic) | 7 |
| Skipped (generic / unresolvable) | 0 |
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
