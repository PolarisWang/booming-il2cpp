"""Dummy — file was deleted. Recreated for reference."""
import json, sys
p = r"D:\agent\booming-il2cpp\build\testing\results\p2-baseline-summary.json"
try:
    with open(p) as f: d = json.load(f)
    passes = sum(1 for v in d.values() if v["status"] == "PASS")
    fails = sum(1 for v in d.values() if v["status"] == "FAIL")
    print(f"{passes} PASS / {fails} FAIL / {len(d)} total")
except: print("No summary found")
