"""Extract JSON results from pipeline logs and save as unified reports."""
import json, re, sys
from pathlib import Path

logs_dir = Path("D:/agent/booming-il2cpp/testing/foundation-dll/results")
results_base = Path("D:/agent/booming-il2cpp/testing/foundation-dll/results/foundation-dll/System.Private.CoreLib")

count = 0
errors = []
for log_file in sorted(logs_dir.glob("*-pipeline.log")):
    slug = log_file.name.replace("-pipeline.log", "")
    text = log_file.read_bytes()

    # Find the final JSON block (starts with standalone "{" and contains "overall_status")
    for m in re.finditer(rb'^\{\s*$', text, re.MULTILINE):
        pos = m.start()
        if b"overall_status" not in text[pos:pos+500]:
            continue

        # Extract matching JSON
        depth = 0
        end_pos = pos
        for j in range(pos, len(text)):
            if text[j:j+1] == b"{":
                depth += 1
            elif text[j:j+1] == b"}":
                depth -= 1
                if depth == 0:
                    end_pos = j + 1
                    break

        raw = text[pos:end_pos].decode("utf-8", errors="replace")
        try:
            data = json.loads(raw)
        except json.JSONDecodeError as e:
            errors.append(f"  SKIP {slug}: JSON error {e}")
            break

        # Write unified report
        report_dir = results_base / slug
        report_dir.mkdir(parents=True, exist_ok=True)
        report_path = report_dir / "unified-verification-report.json"
        report_path.write_text(json.dumps(data, indent=2, ensure_ascii=False), encoding="utf-8")
        count += 1
        break

for e in errors:
    print(e)
print(f"Saved {count} unified reports to {results_base}")
