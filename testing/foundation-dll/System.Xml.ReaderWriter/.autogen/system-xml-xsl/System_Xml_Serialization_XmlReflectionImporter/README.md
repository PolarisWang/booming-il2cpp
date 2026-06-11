# AutoTestGenerator: System.Xml.Serialization.XmlReflectionImporter

- Assembly: `System.Private.Xml`
- Type: `System.Xml.Serialization.XmlReflectionImporter`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 10 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 2 |
| Benchmark-only (pure void / non-deterministic) | 8 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 28 |

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
