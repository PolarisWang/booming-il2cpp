# AutoTestGenerator: System.Xml.Schema.XmlSchemaCollection

- Assembly: `System.Private.Xml`
- Type: `System.Xml.Schema.XmlSchemaCollection`

## Coverage Summary

| Metric | Value |
|--------|-------|
| Total Methods | 10 |
| Auto-generated (Fact + Benchmark + HotUpdate) | 1 |
| Benchmark-only (pure void / non-deterministic) | 8 |
| Skipped (generic / unresolvable) | 1 |
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

## Skipped Methods

- `CopyTo (ICollection non-generic leak: XmlSchema[])` — contains generic parameters, needs manual handling
