#!/usr/bin/env python3
"""
sync_feishu_to_jira.py — 从飞书路线图表同步到 Jira。

核心设计：
- 单一真源 = 飞书表（方案C：8 里程碑行 + 71 子任务行）
- 里程碑 Task 已建（ENG-34859 ~ ENG-34866），脚本只同步子任务
- 幂等：按 parent+summary 查重，不重复创建
- 创建成功自动回写 Jira Key/Jira URL/父Jira Key 到飞书

用法:
  python scripts/sync_feishu_to_jira.py --dry-run       # 预览
  python scripts/sync_feishu_to_jira.py                 # 全量同步
  python scripts/sync_feishu_to_jira.py --single M1-1    # 单条同步 (按序号)

前置:
  export JIRA_BASE_URL=https://jira.boomingtechs.cn
  export JIRA_TOKEN=<personal_access_token>
"""

import json
import os
import sys
import urllib.request
import urllib.error
import subprocess
import tempfile
from collections import defaultdict

# ── Windows GBK console fix ──
if sys.stdout.encoding and sys.stdout.encoding.upper() in ("GBK", "GB2312", "CP936"):
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.dont_write_bytecode = True

# ── Config ──
JIRA_URL = os.environ.get("JIRA_BASE_URL", "https://jira.boomingtechs.cn")
JIRA_TOKEN = os.environ.get("JIRA_TOKEN", "")
JIRA_PROJECT = "ENG"
JIRA_EPIC = "ENG-34824"

# Issuetypes
ISSUETYPE_SUBTASK = {"id": "11804"}  # 子任务
BACKLOG_VERSION = {"id": "12444"}
PRIORITY_MAP = {"P0": {"id": "10003"}, "P1": {"id": "10002"}, "P2": {"id": "10004"}}

# Feishu
FEISHU_BASE = "XOogbqoWZadOz5sfwRwcF6hKn3f"
FEISHU_TABLE = "tblyATjbIPJNHxen"

# Milestone label -> Jira Key mapping (from already-built milestones)
MS_JIRA_MAP = {
    "M1 GC稳定": "ENG-34859",
    "M2 Harness首绿": "ENG-34860",
    "M3 CoreLib验证": "ENG-34861",
    "M4 Foundation验证": "ENG-34862",
    "M5 引擎端到端": "ENG-34863",
    "M6 Windows GA": "ENG-34864",
    "M7 Linux全流程": "ENG-34865",
    "M8 移动/主机": "ENG-34866",
}

MILESTONE_VERSION = {
    "M1 GC稳定": "v0.2.5", "M2 Harness首绿": "v0.2.6",
    "M3 CoreLib验证": "v0.3.0", "M4 Foundation验证": "v0.4.0",
    "M5 引擎端到端": "v0.5.0", "M6 Windows GA": "v1.0.0",
    "M7 Linux全流程": "v1.1.0", "M8 移动/主机": "v1.2.0",
}

# Field IDs in feishu table (from field-list)
# 超链接字段: Jira及父Jira更新时需写成 markdown 链接格式 "[key](url)"
FLD = {
    "任务": "fldXZ9TY6T", "Jira": "fldBWomGOJ",
    "里程碑": "flduId1RzT", "类型": "fld4sbwrEt",
    "估算(天)": "fld3pTi11T", "状态": "fldLFVvklB",
    "发布版本号": "fldsPNuQC8", "父Jira": "fld6lL7hgR",
    "决策门": "fldOtNI2aJ", "优先级": "fldJ3TxpoD",
    "验证类型": "fldjL2OHsd", "序号": "fldXmdtzFC",
    "备注": "fldBNx4YQ5", "阶段": "fld6XJJgfO",
}
# 派生 helper：从 Jira 字段(URL)截取 key；Url 字段写入需要 markdown 格式
BROWSE = "https://jira.boomingtechs.cn/browse/"

def jira_browse_url(key):
    return f"{BROWSE}{key}"

def jira_md_link(key):
    """返回写入 Url 超链接字段的 markdown 格式."""
    return f"[{key}]({BROWSE}{key})"

def key_from_browse(url):
    """从 markdown 链接或纯 URL 还原 key."""
    if not url:
        return None
    # "[ENG-34859](https://...)" -> "ENG-34859"
    if "]" in str(url):
        return str(url).split("[", 1)[1].split("]")[0]
    # "https://.../ENG-34859" -> "ENG-34859"
    return str(url).rstrip("/").rsplit("/", 1)[-1]


# ═══════════════════════════════════════════════════════════════
# 工具
# ═══════════════════════════════════════════════════════════════

def jira_headers():
    return {
        "Authorization": f"Bearer {JIRA_TOKEN}",
        "Content-Type": "application/json; charset=UTF-8",
        "Accept": "application/json",
    }


def jira_request(method, path, body=None):
    url = f"{JIRA_URL}{path}"
    data = json.dumps(body, ensure_ascii=False).encode("utf-8") if body else None
    req = urllib.request.Request(url, data=data, method=method)
    for k, v in jira_headers().items():
        req.add_header(k, v)
    try:
        resp = urllib.request.urlopen(req)
        raw = resp.read().decode("utf-8")
        return {"ok": True, "body": json.loads(raw) if raw else {}}
    except urllib.error.HTTPError as e:
        err = e.read().decode("utf-8")
        return {"ok": False, "code": e.code, "err": err[:500]}


def feishu_batch_update(record_id, patch):
    """更新飞书表一条记录的多个字段。patch = {field_id: value}.
    lark-cli 只接受相对当前目录的文件路径，故写到 cwd 下具名临时文件."""
    import uuid
    tmp = f".feishu-tmp-{uuid.uuid4().hex}.json"
    with open(tmp, "wb") as f:
        f.write(json.dumps(payload := {
            "record_id_list": [record_id], "patch": patch,
        }, ensure_ascii=False).encode("utf-8"))
    try:
        result = subprocess.run(
            ["lark-cli", "base", "+record-batch-update",
             "--base-token", FEISHU_BASE, "--table-id", FEISHU_TABLE,
             "--json", f"@{tmp}", "--format", "json"],
            capture_output=True, timeout=15)
        out = json.loads(result.stdout.decode("utf-8", errors="replace") or "{}")
        if out.get("ok"):
            return True
        else:
            err = out.get("error", {}).get("message", "unknown")
            print(f"    [WARN] feishu update failed: {err}")
            return False
    finally:
        os.unlink(tmp)


# ═══════════════════════════════════════════════════════════════
# 飞书数据读取
# ═══════════════════════════════════════════════════════════════

def _feishu_list_rows():
    """实时调 lark-cli 读取全表, 返回 record 数组(list of dict)."""
    rid = subprocess.run(
        ["lark-cli", "base", "+record-list", "--base-token", FEISHU_BASE,
         "--table-id", FEISHU_TABLE, "--limit", "200", "--format", "json"],
        capture_output=True, timeout=60)
    d = json.loads(rid.stdout.decode("utf-8", "replace") or "{}")
    if not d.get("ok"):
        print("ERROR: feishu read failed", d.get("error"))
        sys.exit(1)
    data = d["data"]
    fids = data["field_id_list"]
    rows = data["data"]
    ids = data["record_id_list"]
    # map field_id -> position
    def cell(rec, fid):
        if fid not in fids:
            return None
        return rec[fids.index(fid)]
    out = []
    for idx, rid_ in enumerate(ids):
        rec = rows[idx]
        def g(fid, flat=True):
            v = cell(rec, fid)
            if isinstance(v, list):
                # select/url/人 字段可能返回数组元素
                if len(v) == 0:
                    return "" if flat else []
                v = v[0]
            return v
        out.append({
            "rid": rid_,
            "type": g(FLD["类型"]),
            "title": g(FLD["任务"]) or "",
            "seq": g(FLD["序号"]) or "",
            "ms_label": g(FLD["里程碑"]) or "",
            "pri": g(FLD["优先级"]) or "",
            "note": g(FLD["备注"]) or "",
            "est": g(FLD["估算(天)"], flat=False) or 0,
        })
        # 超链接字段值可能是 {"link":..,"text":..} 对象或 markdown/纯URL
        def url_val(fid):
            v = cell(rec, fid)
            if not v:
                return ""
            if isinstance(v, list):
                v = v[0] if v else ""
            if isinstance(v, dict):
                return v.get("link", "") or v.get("text", "")
            return str(v)
        out[-1]["jira_url"] = url_val(FLD["Jira"])
        out[-1]["jira_key"] = key_from_browse(out[-1]["jira_url"])
        out[-1]["parent_key"] = key_from_browse(url_val(FLD["父Jira"]))
    return out


def load_feishu_tasks():
    """实时读飞书, 返回 (milestone_rows, task_rows). 单一真源 = 飞书."""
    raw = _feishu_list_rows()
    ms_rows = [x for x in raw if x.get("type") == "里程碑"]
    task_rows = [x for x in raw if x.get("type") != "里程碑"]
    return ms_rows, task_rows


# ═══════════════════════════════════════════════════════════════
# 幂等查重
# ═══════════════════════════════════════════════════════════════

def find_existing_subtask(title, parent_key):
    """在指定的 parent 下按 summary 模糊查重。返回 key 或 None."""
    safe = title.replace("\\", "\\\\").replace('"', '\\"')
    jql = (f'project={JIRA_PROJECT} AND parent={parent_key} '
           f'AND summary~"{safe}"')
    from urllib.parse import urlencode
    qs = urlencode({"jql": jql, "fields": "summary", "maxResults": 5})
    result = jira_request("GET", f"/rest/api/2/search?{qs}")
    if not result["ok"]:
        print(f"    [QUERY-FAIL] 查重失败: {result.get('err')}")
        return "QUERY_FAIL"
    issues = result["body"].get("issues", [])
    if issues:
        return issues[0]["key"]
    return None


# ═══════════════════════════════════════════════════════════════
# 创建 Sub-task
# ═══════════════════════════════════════════════════════════════

def create_subtask(title, milestone, priority, note, estimate_days, parent_key):
    """创建一条 Sub-task 并返回 key."""
    pri_id = PRIORITY_MAP.get(priority, PRIORITY_MAP["P1"])["id"]
    version = MILESTONE_VERSION.get(milestone, "v0.x")
    label = f"roadmap-{milestone.split()[0].lower()}" if milestone else "roadmap"

    body = {
        "fields": {
            "project": {"key": JIRA_PROJECT},
            "summary": title,
            "description": (
                f"## 来源\n\n"
                f"Chaos IL2CPP 发布路线图\n"
                f"里程碑: {milestone}\n"
                f"目标版本: {version}\n\n"
                f"## 任务描述\n\n{note}\n\n"
                f"## 估算\n\n{estimate_days}天"
            ),
            "issuetype": ISSUETYPE_SUBTASK,
            "priority": {"id": pri_id},
            "fixVersions": [BACKLOG_VERSION],
            "labels": ["il2cpp", "roadmap", label],
            "parent": {"key": parent_key},
        }
    }
    result = jira_request("POST", "/rest/api/2/issue", body)
    if result["ok"]:
        key = result["body"]["key"]
        return key
    else:
        print(f"    [FAIL] {result.get('err')}", file=sys.stderr)
        return None


# ═══════════════════════════════════════════════════════════════
# 主流程
# ═══════════════════════════════════════════════════════════════

def main():
    dry_run = "--dry-run" in sys.argv
    single = None
    for arg in sys.argv[1:]:
        if arg.startswith("--single="):
            single = arg.split("=", 1)[1]

    if not JIRA_TOKEN:
        print("ERROR: 请设置 JIRA_TOKEN 环境变量")
        sys.exit(1)

    # 加载飞书数据
    ms_rows, task_rows = load_feishu_tasks()
    print(f"Loaded {len(ms_rows)} milestones, {len(task_rows)} tasks from feishu")

    # 筛选单条
    if single:
        task_rows = [t for t in task_rows if t.get("seq") == single]
        if not task_rows:
            print(f"ERROR: 未找到序号 {single} 的任务")
            sys.exit(1)
        print(f"Single mode: {single}")

    # 按里程碑分组
    by_ms = defaultdict(list)
    for t in task_rows:
        by_ms[t.get("ms_label", "")].append(t)

    total_created = 0
    total_skipped = 0
    total_failed = 0

    for ms_label, tasks in sorted(by_ms.items()):
        parent_key = MS_JIRA_MAP.get(ms_label)
        if not parent_key:
            print(f"  [SKIP] 里程碑 '{ms_label}' 无对应 Jira Key，跳过 {len(tasks)} 条")
            total_skipped += len(tasks)
            continue

        print(f"\n{'='*60}")
        print(f"  [{ms_label}] parent={parent_key} ({len(tasks)} tasks)")
        print(f"{'='*60}")

        for t in tasks:
            title = t["title"]
            seq = t.get("seq", "?")
            pri = t.get("pri", "P1")
            note = t.get("note", "")
            est = t.get("est", 0)
            rid = t.get("rid", "")
            existing_key = t.get("jira_key", "")

            # 已有 Jira Key → 跳过
            if existing_key:
                print(f"  [SKIP] {seq} {title[:40]:40s} 已有 Jira: {existing_key}")
                total_skipped += 1
                continue

            # 幂等查重
            dup = find_existing_subtask(title, parent_key)
            if dup == "QUERY_FAIL":
                print(f"  [SKIP] {seq} {title[:40]:40s} 查重中断")
                total_skipped += 1
                continue
            if dup:
                print(f"  [SKIP] {seq} {title[:40]:40s} 已存在: {dup}")
                total_skipped += 1
                # 回写已存在的 key (markdown 链接)
                if not dry_run and rid:
                    feishu_batch_update(rid, {
                        FLD["Jira"]: jira_md_link(dup),
                        FLD["父Jira"]: jira_md_link(parent_key),
                    })
                continue

            # 创建
            if dry_run:
                print(f"  [DRY-RUN] {seq} {title[:40]:40s} → {parent_key}")
                total_skipped += 1
                continue

            key = create_subtask(title, ms_label, pri, note, est, parent_key)
            if key:
                print(f"  [OK]   {seq} {key}: {title[:40]}")
                total_created += 1
                # 回写飞书 (markdown 链接格式写入 Url 字段)
                if rid:
                    feishu_batch_update(rid, {
                        FLD["Jira"]: jira_md_link(key),
                        FLD["父Jira"]: jira_md_link(parent_key),
                    })
            else:
                print(f"  [FAIL] {seq} {title[:40]:40s}")
                total_failed += 1

    print(f"\n{'='*60}")
    print(f"Summary: {total_created} created, {total_skipped} skipped, {total_failed} failed")
    if dry_run:
        print("(dry-run, nothing created)")


if __name__ == "__main__":
    main()