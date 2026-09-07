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
    # Fallback to local-only token file (~/.config/chaos-il2cpp/jira_token)
    _tf = os.path.expanduser("~/.config/chaos-il2cpp/jira_token")
    try:
        with open(_tf, encoding="utf-8") as _fh:
            for _ln in _fh:
                _ln = _ln.strip()
                if _ln and not _ln.startswith("#"):
                    JIRA_TOKEN = _ln
                    break
    except FileNotFoundError:
        pass

if not JIRA_TOKEN:
    print("ERROR: 请设置 JIRA_TOKEN 环境变量，或写入 ~/.config/chaos-il2cpp/jira_token")
    sys.exit(1)

if "--apply" in sys.argv:
    # Phase 2: 读取 agent 输出结果，写回 Jira
    import glob
    results = []
    skipped_files = 0
    for f in sorted(glob.glob(".enrich-ctx/results-*.json")):
        try:
            with open(f, encoding="utf-8") as fh:
                d = json.load(fh)
                results.extend(d.get("issues", []))
        except (OSError, ValueError) as e:
            # #2: 任一文件损坏/半写入，整份静默丢弃会掩盖失败——打印告警并计数，
            #     而不只是靠 'Loaded N' 显得一切正常。
            skipped_files += 1
            print(f"  [WARN] Skipping unreadable {f}: {type(e).__name__}: {e}")
    print(f"Loaded {len(results)} enriched descriptions"
          + (f" ({skipped_files} result file(s) skipped due to read error)" if skipped_files else ""))

    ok_count = 0
    fail_count = 0
    skipped_empty = 0
    failed_keys: set[str] = set()
    for item in results:
        key = item.get("key")
        desc = item.get("description", "")
        if not key or not desc:
            # #3: 空 key/desc 不可静默跳过——计数+日志，供事后追溯哪个 issue 未更新。
            skipped_empty += 1
            print(f"  [SKIP-empty] key={key!r} desc_len={len(desc)} (skipped: no {'' if key else 'key'}"
                  f"{'' if desc else 'description'})")
            continue
        body = json.dumps({"fields": {"description": desc}}, ensure_ascii=False).encode("utf-8")
        req = urllib.request.Request(f"{JIRA_URL}/rest/api/2/issue/{key}", data=body, method="PUT")
        req.add_header("Authorization", f"Bearer {JIRA_TOKEN}")
        req.add_header("Content-Type", "application/json; charset=UTF-8")
        ok = False
        # #1: 除 HTTPError 外还会抛 URLError / TimeoutError / ConnectionResetError 等瞬时网络异常，
        #     未捕获会中断整个 --apply 循环导致后续 key 不回写且无续跑断点。
        #     统一拦截 (HTTPError, URLError, TimeoutError, OSError)。
        for attempt in range(3):
            try:
                urllib.request.urlopen(req, timeout=30)
                print(f"  [OK] {key}")
                ok = True
                break
            except urllib.error.HTTPError as e:
                err = e.read().decode("utf-8", "replace")[:200]
                # #8: HTTP 429 限流应做更长退避而非 1s 固定重试 3 次
                if e.code == 429 and attempt == 0:
                    print(f"  [WARN] {key}: HTTP 429 rate-limited — longer backoff")
                    time.sleep(5)
                    continue
                if attempt < 2:
                    time.sleep(1)
                else:
                    print(f"  [FAIL] {key}: HTTP {e.code} {err}")
            except (urllib.error.URLError, TimeoutError, OSError) as e:
                if attempt < 2:
                    time.sleep(2)
                else:
                    print(f"  [FAIL] {key}: network error {type(e).__name__}: {e}")
        if not ok:
            fail_count += 1
            failed_keys.add(key)
        else:
            ok_count += 1
        time.sleep(0.2)  # #8: 0.1s ≈10QPS 突发偏高，降至 ~5QPS 并带重试
    print(f"Done: {ok_count} ok, {fail_count} fail, {skipped_empty} empty-skip")
    # #1 续跑：失败 key 落盘供重试（具体追踪了每个 FAIL 的 key）。
    if fail_count and failed_keys:
        failed = [it for it in results
                  if it.get("key") in failed_keys and it.get("description", "")]
        if failed:
            with open(".enrich-ctx/retry-failed.json", "w", encoding="utf-8") as fh:
                json.dump(failed, fh, ensure_ascii=False)
            print(f"  Saved {len(failed)} failed keys to .enrich-ctx/retry-failed.json for retry")
    sys.exit(1 if fail_count else 0)

elif "--generate" in sys.argv:
    # Phase 1: 生成 10 组分批数据供 agent 使用
    # #7: 使用 with 上下文管理器确保句柄关闭；捕获 FileNotFoundError/JSONDecodeError
    try:
        with open(".rows-v2.json", encoding="utf-8") as fh:
            rows = json.load(fh)
    except (FileNotFoundError, ValueError) as e:
        print(f"ERROR: Cannot read .rows-v2.json: {e}")
        sys.exit(1)
    try:
        with open(".cur.json", encoding="utf-8") as fh:
            cur = json.load(fh)
    except (FileNotFoundError, ValueError) as e:
        print(f"ERROR: Cannot read .cur.json: {e}")
        sys.exit(1)
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
