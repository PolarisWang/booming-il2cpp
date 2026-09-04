#!/usr/bin/env bash
# notify-feishu.sh — send a production-grade Feishu interactive card via custom bot webhook.
#
# Three event types (via FEISHU_EVENT):
#   published  — release.sh publish completed (tag + nupkg + GitHub Release ok)
#   verify     — release.sh verify passed but not yet published
#   failed     — a release/verify/CI step failed
#
# Credentials (via env):
#   FEISHU_WEBHOOK   custom-bot webhook URL (required)
#   FEISHU_SECRET    optional signing secret (if enabled on the bot)
#   FEISHU_EVENT     published|verify|failed  [default: published]
#
# Card content (all optional, empty=skip that section):
#   FEISHU_VERSION          v0.2.3
#   FEISHU_ACTOR            PolarisWong
#   FEISHU_ENV              生产 | 验证 | CI
#   FEISHU_TIME_UTC         2026-09-04T12:00:00Z
#   FEISHU_REPO             PolarisWang/booming-il2cpp
#   FEISHU_RELEASE_URL      https://github.com/.../releases/tag/v0.2.3
#   FEISHU_LOG_URL           (failed card button)
#   FEISHU_HIGHLIGHTS       3 lines for highlights section (literal \n for newlines)
#   FEISHU_CHANGED_SUMMARY  "New Features 11 · Bug Fixes 20 · Performance 2"
#   FEISHU_ARTIFACTS        "chaos-il2cpp.0.2.3.nupkg (16MB) · SHA256SUMS"
#   FEISHU_STATS            "28 commits · 240 files · v0.2.2 → v0.2.3"
#   FEISHU_GATES            "governance ✓ · publish-smoke ✓ · unit ✓ · integrity ✓ · nupkg ✓"
#   FEISHU_FAILED_STAGE     "verify gate: unit" or "CI: build-windows"
#   FEISHU_FAILED_DETAILS   "3/5 unit tests failed"
#   FEISHU_PENDING_URL       (verify card button)
#
# Exit 0 on success (or when no webhook configured); never blocks the caller.

set -euo pipefail

WEBHOOK="${FEISHU_WEBHOOK:-}"
SECRET="${FEISHU_SECRET:-}"
EVENT="${FEISHU_EVENT:-published}"

if [ -z "$WEBHOOK" ]; then
    echo "  [feishu] no FEISHU_WEBHOOK configured — notification skipped"
    exit 0
fi

# Read all optional env
VERSION="${FEISHU_VERSION:-}"
ACTOR="${FEISHU_ACTOR:-}"
ENV="${FEISHU_ENV:-}"
TIME_UTC="${FEISHU_TIME_UTC:-}"
REPO="${FEISHU_REPO:-PolarisWang/booming-il2cpp}"
RELEASE_URL="${FEISHU_RELEASE_URL:-}"
LOG_URL="${FEISHU_LOG_URL:-}"
HIGHLIGHTS="${FEISHU_HIGHLIGHTS:-}"
CHANGED_SUMMARY="${FEISHU_CHANGED_SUMMARY:-}"
ARTIFACTS="${FEISHU_ARTIFACTS:-}"
STATS="${FEISHU_STATS:-}"
GATES="${FEISHU_GATES:-}"
FAILED_STAGE="${FEISHU_FAILED_STAGE:-}"
FAILED_DETAILS="${FEISHU_FAILED_DETAILS:-}"
PENDING_URL="${FEISHU_PENDING_URL:-}"

# ── Build card header ────────────────────────────────────────────────────
case "$EVENT" in
    published)
        HEADER_EMOJI="🚀"
        HEADER_TITLE="chaos-il2cpp 发布成功${VERSION:+ $VERSION}"
        HEADER_TEMPLATE="green"
        ;;
    verify)
        HEADER_EMOJI="✅"
        HEADER_TITLE="chaos-il2cpp 验证通过${VERSION:+ $VERSION}（待发布）"
        HEADER_TEMPLATE="blue"
        ;;
    failed)
        HEADER_EMOJI="🔴"
        HEADER_TITLE="chaos-il2cpp 发布失败${VERSION:+ $VERSION}"
        HEADER_TEMPLATE="red"
        ;;
    *)
        HEADER_EMOJI="📣"
        HEADER_TITLE="chaos-il2cpp 通知${VERSION:+ $VERSION}"
        HEADER_TEMPLATE="blue"
        ;;
esac

# ── Build elements (Python for safe JSON assembly) ───────────────────────
# NOTE: All data is read from environment variables, NOT from sys.argv.
# On Windows Git Bash, command-line arguments are encoded in the terminal
# codepage (GBK) by the time they reach Python, corrupting non-ASCII
# characters.  Environment variables are natively UTF-8 in Git Bash, so
# os.environ preserves the correct encoding.
export _FEISHU_HEADER_EMOJI="$HEADER_EMOJI"
export _FEISHU_HEADER_TITLE="$HEADER_TITLE"
export _FEISHU_HEADER_TEMPLATE="$HEADER_TEMPLATE"
PAYLOAD=$(python <<'PYEOF'
import json, time, hmac, hashlib, base64, os, sys

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
except Exception:
    pass

def _env(k, default=""):
    return os.environ.get(k, default)


webhook    = _env("FEISHU_WEBHOOK")
secret     = _env("FEISHU_SECRET")
event      = _env("FEISHU_EVENT", "published")
emoji      = _env("_FEISHU_HEADER_EMOJI")
title      = _env("_FEISHU_HEADER_TITLE")
template   = _env("_FEISHU_HEADER_TEMPLATE")
ver        = _env("FEISHU_VERSION")
actor      = _env("FEISHU_ACTOR")
env_name   = _env("FEISHU_ENV")
time_utc   = _env("FEISHU_TIME_UTC")
repo       = _env("FEISHU_REPO")
rel_url    = _env("FEISHU_RELEASE_URL")
log_url    = _env("FEISHU_LOG_URL")
highlights = _env("FEISHU_HIGHLIGHTS")
changed    = _env("FEISHU_CHANGED_SUMMARY")
artifacts  = _env("FEISHU_ARTIFACTS")
stats      = _env("FEISHU_STATS")
gates      = _env("FEISHU_GATES")
fail_stage = _env("FEISHU_FAILED_STAGE")
fail_detail = _env("FEISHU_FAILED_DETAILS")
pending_url = _env("FEISHU_PENDING_URL")

# Sign
ts = str(int(time.time()))
sig = ""
if secret:
    sig = base64.b64encode(hmac.new((ts + "\n" + secret).encode(), digestmod=hashlib.sha256).digest()).decode()

elements = []

# ── Audit / metadata row ──
meta_lines = []
if repo:
    meta_lines.append(f"**Repository**：{repo}")
if env_name:
    meta_lines.append(f"**Environment**：{env_name}")
if actor:
    meta_lines.append(f"**Actor**：{actor}")
if ver:
    meta_lines.append(f"**Version**：{ver}")
if time_utc:
    meta_lines.append(f"**Time (UTC)**：{time_utc}")
if meta_lines:
    elements.append({"tag": "div", "text": {"tag": "lark_md", "content": "\n".join(meta_lines)}})
    elements.append({"tag": "hr"})

# ── Highlights ──
if highlights:
    lines = highlights.replace("\\n", "\n").split("\n")
    content = "**🎯 Highlights**\n" + "\n".join(f"- {l}" for l in lines if l.strip())
    elements.append({"tag": "div", "text": {"tag": "lark_md", "content": content}})

# ── What's Changed (published only) ──
if event == "published" and changed:
    elements.append({"tag": "div", "text": {"tag": "lark_md", "content": f"**📋 What's Changed**\n{changed}"}})

# ── Artifacts (published only) ──
if event == "published" and artifacts:
    elements.append({"tag": "div", "text": {"tag": "lark_md", "content": f"**📦 Artifacts**\n{artifacts}"}})

# ── Gates (verify only) ──
if event == "verify" and gates:
    elements.append({"tag": "div", "text": {"tag": "lark_md", "content": f"**✅ Gates**\n{gates}"}})

# ── Failure info (failed only) ──
if event == "failed":
    fail_lines = []
    if fail_stage:
        fail_lines.append(f"**Stage**：{fail_stage}")
    if fail_detail:
        fail_lines.append(f"**Detail**：{fail_detail}")
    fail_lines.append("\n**Suggested action**：Fix the issue, then run `release.sh fix --from-main` and re-verify.")
    if fail_lines:
        elements.append({"tag": "div", "text": {"tag": "lark_md", "content": "\n".join(fail_lines)}})

# ── Stats ──
if stats:
    if elements and elements[-1].get("tag") != "hr":
        elements.append({"tag": "hr"})
    elements.append({"tag": "div", "text": {"tag": "lark_md", "content": f"**📊 Stats**\n{stats}"}})

# ── Action button ──
actions = []
if rel_url:
    actions.append({"tag": "button", "text": {"tag": "plain_text", "content": "🔗 View Release"}, "type": "primary", "url": rel_url})
if event == "verify" and pending_url:
    actions.append({"tag": "button", "text": {"tag": "plain_text", "content": "▶️ Proceed to Publish"}, "type": "default", "url": pending_url})
if event == "failed" and log_url:
    actions.append({"tag": "button", "text": {"tag": "plain_text", "content": "📋 View Logs"}, "type": "danger", "url": log_url})
if actions:
    elements.append({"tag": "action", "actions": actions})

# ── Assemble ──
card = {
    "msg_type": "interactive",
    "timestamp": ts,
    "card": {
        "config": {"wide_screen_mode": True},
        "header": {
            "title": {"tag": "plain_text", "content": f"{emoji} {title}"},
            "template": template
        },
        "elements": elements
    }
}
if sig:
    card["sign"] = sig

print(json.dumps(card, ensure_ascii=False))
PYEOF
)

# ── Send ──
RESP=$(curl -s -X POST "$WEBHOOK" -H "Content-Type: application/json; charset=utf-8" --data "$PAYLOAD" 2>/dev/null || true)
echo "  [feishu] response: ${RESP:0:300}"

if echo "$RESP" | grep -qE '"code"\s*:\s*0'; then
    echo "  [feishu] notification sent ($EVENT)"
else
    echo "  [feishu] warning: notification not acknowledged: $RESP" >&2
fi
exit 0