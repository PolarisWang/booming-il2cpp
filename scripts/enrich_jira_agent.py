#!/usr/bin/env python3
"""
enrich_jira_agent.py — 批量精修 Jira 描述。

两步走：
  1. python enrich_jira_agent.py --generate   # 把 10 组数据写入 .enrich-ctx/ 供 agent 食用
  2. python enrich_jira_agent.py --apply      # 读取 .enrich-ctx/results-*.json 并写入 Jira
"""
import json, os, sys, urllib.request, urllib.error, time

JIRA_URL = os.environ.get("JIRA_BASE_URL", "https://jira.boomingtechs.cn")
JIRA_TOKEN = os.environ.get("JIRA_TOKEN", "")

if not JIRA_TOKEN:
    print("ERROR: 请设置 JIRA_TOKEN 环境变量")
    sys.exit(1)

if "--apply" in sys.argv:
    # Phase 2: 读取 agent 输出结果，写回 Jira
    import glob
    results = []
    for f in sorted(glob.glob(".enrich-ctx/results-*.json")):
        try:
            with open(f, encoding="utf-8") as fh:
                d = json.load(fh)
                results.extend(d.get("issues", []))
        except:
            pass
    print(f"Loaded {len(results)} enriched descriptions")

    for item in results:
        key = item.get("key")
        desc = item.get("description", "")
        if not key or not desc:
            continue
        body = json.dumps({"fields": {"description": desc}}, ensure_ascii=False).encode("utf-8")
        req = urllib.request.Request(f"{JIRA_URL}/rest/api/2/issue/{key}", data=body, method="PUT")
        req.add_header("Authorization", f"Bearer {JIRA_TOKEN}")
        req.add_header("Content-Type", "application/json; charset=UTF-8")
        for attempt in range(3):
            try:
                urllib.request.urlopen(req, timeout=30)
                print(f"  [OK] {key}")
                break
            except urllib.error.HTTPError as e:
                err = e.read().decode("utf-8", "replace")[:200]
                if attempt < 2:
                    time.sleep(1)
                else:
                    print(f"  [FAIL] {key}: {err}")
        time.sleep(0.1)

    print("Done")
    sys.exit(0)

elif "--generate" in sys.argv:
    # Phase 1: 生成 10 组分批数据供 agent 使用
    rows = json.load(open(".rows-v2.json", encoding="utf-8"))
    cur = json.load(open(".cur.json", encoding="utf-8"))
    keys = [r for r in rows if r.get("key")]
    for r in keys:
        r["cur"] = cur.get(r["key"], "")
    import shutil
    shutil.rmtree(".enrich-ctx", ignore_errors=True)
    os.makedirs(".enrich-ctx", exist_ok=True)
    buckets = [[] for _ in range(10)]
    for idx, r in enumerate(keys):
        buckets[idx % 10].append(r)
    for i, b in enumerate(buckets, 1):
        fn = f".enrich-ctx/grp-{i:02d}.json"
        with open(fn, "w", encoding="utf-8") as f:
            json.dump({"bucket": i, "items": b}, f, ensure_ascii=False)
        print(f"  {fn}: {len(b)} items")
    print(f"Total keys: {len(keys)}. Ready for agent to process.")
    sys.exit(0)

else:
    print("Usage: python enrich_jira_agent.py --generate | --apply")
    sys.exit(1)