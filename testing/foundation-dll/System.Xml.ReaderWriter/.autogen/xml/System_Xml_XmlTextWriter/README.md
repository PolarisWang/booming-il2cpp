# AutoTestGenerator: System.Xml.XmlTextWriter

- Assembly: `System.Private.Xml`
- Type: `System.Xml.XmlTextWriter`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 28 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 21 |
| Benchmark-only (pure void / non-deterministic) | 7 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 70 |

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
