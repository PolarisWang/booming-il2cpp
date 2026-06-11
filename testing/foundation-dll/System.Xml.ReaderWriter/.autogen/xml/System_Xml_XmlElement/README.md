# AutoTestGenerator: System.Xml.XmlElement

- Assembly: `System.Private.Xml`
- Type: `System.Xml.XmlElement`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 22 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 5 |
| Benchmark-only (pure void / non-deterministic) | 17 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 48 |

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
