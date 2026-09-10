# Optimization Campaign Phase 0 Report
## Generated: 2026-06-08

## Results

| Chunk | Subjects | Fact | Bench Methods | Total Duration | Max Method | Alloc | Priority |
|-------|----------|------|---------------|----------------|------------|-------|----------|
| threading | 246 | 246/246 | 602 | 269.7ms | 50.3ms | 19MB | HIGH |
| collections | 45 | 45/45 | 46 | 8.9ms | 0.5ms | 2.8MB | MEDIUM |
| system | 99 | 97/99 | 11 | 0.3ms | 0.0ms | 0B | LOW |
| text-json | 158 | 158/158 | 15 | 0.4ms | 0.0ms | 0B | LOW |
| collections-generic | 70 | 69/70 | 1 | 0.0ms | 0.0ms | 0B | LOW |

## Top Candidate: threading (602 methods, 269.7ms)

The threading chunk has the highest optimization potential with 602 benchmark methods
and 269.7ms total benchmark time. Recommended approach:
  1. Run profile analysis on threading to identify hot methods
  2. Profile scope data shows methods taking up to 50.3ms each
  3. Focus optimization on the top 10% slowest methods
