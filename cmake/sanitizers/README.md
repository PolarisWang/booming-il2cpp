# Sanitizer Configurations

| Config | Flag | When to use |
|--------|------|-------------|
| ASan   | `/fsanitize=address` | Daily dev, PR gate (smoke + unit) |
| RTC    | `/RTC1` | Release builds, lightweight checks |
