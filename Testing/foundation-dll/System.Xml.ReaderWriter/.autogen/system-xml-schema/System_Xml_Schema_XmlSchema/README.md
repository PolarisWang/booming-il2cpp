# AutoTestGenerator: System.Xml.Schema.XmlSchema

- Assembly: `System.Private.Xml`
- Type: `System.Xml.Schema.XmlSchema`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 11 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 8 |
| Benchmark-only (pure void / non-deterministic) | 3 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 17 |

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
