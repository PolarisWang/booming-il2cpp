import json
with open('D:/agent/booming-il2cpp/testing/foundation-dll/System.Private.CoreLib/convert-char/unified-verification-report.json', encoding='utf-8') as f:
    r = json.load(f)
b = r.get('stages', {}).get('benchmark', {})
print("benchmark stage keys:", list(b.keys()))
print("status:", b.get('status'))
d = b.get('details', {})
print("details keys:", list(d.keys()))
if isinstance(d, dict):
    for k, v in d.items():
        if isinstance(v, dict):
            print(f"  {k}: keys={list(v.keys())[:10]}")
        else:
            print(f"  {k}: {v}")
