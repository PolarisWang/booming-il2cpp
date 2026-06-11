# AutoTestGenerator: System.Xml.Schema.XmlSchemaValidator

- Assembly: `System.Private.Xml`
- Type: `System.Xml.Schema.XmlSchemaValidator`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 19 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 11 |
| Benchmark-only (pure void / non-deterministic) | 8 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 30 |

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
