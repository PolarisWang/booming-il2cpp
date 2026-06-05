# AutoTestGenerator: System.Xml.Serialization.XmlAnyElementAttributes

- Assembly: `System.Private.Xml`
- Type: `System.Xml.Serialization.XmlAnyElementAttributes`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 6 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 5 |
| Benchmark-only (pure void / non-deterministic) | 0 |
| Skipped (generic / unresolvable) | 1 |
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

## Skipped Methods

- `CopyTo (ICollection non-generic leak: XmlAnyElementAttribute[])` — contains generic parameters, needs manual handling
