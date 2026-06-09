# AutoTestGenerator: System.Xml.XmlSecureResolver

- Assembly: `System.Private.Xml`
- Type: `System.Xml.XmlSecureResolver`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 3 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 0 |
| Benchmark-only (pure void / non-deterministic) | 3 |
| Skipped (generic / unresolvable) | 0 |
| Total Subjects | 6 |

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
