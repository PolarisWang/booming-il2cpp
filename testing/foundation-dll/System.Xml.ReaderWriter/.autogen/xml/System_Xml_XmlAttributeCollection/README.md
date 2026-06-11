# AutoTestGenerator: System.Xml.XmlAttributeCollection

- Assembly: `System.Private.Xml`
- Type: `System.Xml.XmlAttributeCollection`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 9 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 0 |
| Benchmark-only (pure void / non-deterministic) | 8 |
| Skipped (generic / unresolvable) | 1 |
| Total Subjects | 9 |

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

- `CopyTo (ICollection non-generic leak: XmlAttribute[])` — contains generic parameters, needs manual handling
