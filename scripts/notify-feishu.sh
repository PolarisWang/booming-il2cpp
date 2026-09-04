#!/usr/bin/env bash
# notify-feishu.sh — send a Feishu group notification via a custom bot webhook.
#
# Used by the release pipeline to broadcast release events to a chat group.
#
# Supported events (via FFEISHU_EVENT):
#   published    — release.sh publish completed (tag + nupkg + GitHub Release ok)
#   verify       — release.sh verify passed but not yet published
#   failed       — a release/verify/CI step failed
#
# Credentials (via env):
#   FEISHU_WEBHOOK   custom-bot webhook URL (https://open.feishu.cn/open-apis/bot/v2/hook/<token>)
#   FEISHU_SECRET    optional "sign" secret if the bot has signature verification enabled
#   FEISHU_EVENT     one of published|verify|failed   [default: published]
#
# Rich content (optional, all env-overridable):
#   FEISHU_VERSION     release version, e.g. v0.2.3
#   FEISHU_URL         release page / run URL
#   FEISHU_HIGHLIGHTS  first stanza of generated release notes, or a short heading
#   FEISHU_NOTES       longer body text (may contain \n)
#   FEISHU_REPO        repo slug, e.g. PolarisWang/booming-il2cpp
#
# Usage:
#   FEISHU_WEBHOOK=... FEISHU_EVENT=published FEISHU_VERSION=v0.2.3 scripts/notify-feishu.sh
#
# Exit 0 on success (or when no webhook configured / disabled); nonzero on a hard
# send failure.  Hard-fails are logged but never block the caller (belt & braces).

set -euo pipefail

WEBHOOK="${FEISHU_WEBHOOK:-}"
SECRET="${FEISHU_SECRET:-}"
EVENT="${FEISHU_EVENT:-published}"

# If no webhook is configured, this is a no-op (release works without chat notify).
if [ -z "$WEBHOOK" ]; then
    echo "  [feishu] no FEISHU_WEBHOOK configured — notification skipped"
    exit 0
fi

VERSION="${FEISHU_VERSION:-}"
URL="${FEISHU_URL:-}"
REPO="${FEISHU_REPO:-PolarisWang/booming-il2cpp}"
HL="${FEISHU_HIGHLIGHTS:-}"
NOTES="${FEISHU_NOTES:-}"

# ── Build the card title / text per event ────────────────────────────────
title=""
color=""
case "$EVENT" in
    published)
        title="🚀 chaos-il2cpp 发布成功${VERSION:+ $VERSION}"
        color="green"
        ;;
    verify)
        title="✅ chaos-il2cpp 验证通过${VERSION:+ $VERSION}（待发布）"
        color="blue"
        ;;
    failed)
        title="🔴 chaos-il2cpp 发布失败/告警${VERSION:+ $VERSION}"
        color="red"
        ;;
    *)
        title="📣 chaos-il2cpp 通知${VERSION:+ $VERSION}"
        color="blue"
        ;;
esac

# Escape text for JSON (Feishu card/text uses plain text; newlines preserved)
escape_json() {
    python -c "import json,sys; print(json.dumps(sys.argv[1]))" "$1"
}

# ── Compute sign if a secret is present ─────────────────────────────────
# Feishu signature scheme: base64(hmac_sha256(key, timestamp + "\n" + secret))
SIGN_PART=""
if [ -n "$SECRET" ]; then
    # python used for hmac/base64 (cross-platform reliable)
    SIGN_PART=$(python - "$SECRET" <<'PYEOF'
import base64, hmac, hashlib, sys, time
secret = sys.argv[1]
ts = str(int(time.time()))
string_to_sign = ts + "\n" + secret
sig = base64.b64encode(hmac.new(string_to_sign.encode("utf-8"), digestmod=hashlib.sha256).digest()).decode()
print(f"{ts},{sig}")
PYEOF
    )
fi

# Assemble content lines.
body_lines=""
body_lines="${body_lines}${REPO:+仓库：}$REPO\\n"
[ -n "$VERSION" ] && body_lines="${body_lines}版本：${VERSION}\\n"
[ -n "$URL" ] && body_lines="${body_lines}地址：${URL}\\n"
if [ -n "$NOTES" ]; then
    body_lines="${body_lines}\\n${NOTES}\\n"
elif [ -n "$HL" ]; then
    body_lines="${body_lines}\\n${HL}\\n"
fi

# Build Feishu text payload.  event/failed==red -> error_code style card is
# overkill; a plain text message with emoji title is consistent & robust.
ts_part="${SIGN_PART%,*}"
sig_part="${SIGN_PART#*,}"
payload_generated=$(python - "$title" "$body_lines" <<'PYEOF'
import json, sys
content = sys.argv[1] + "\n\n" + sys.argv[2]
print(json.dumps({
  "msg_type": "interactive",
  "card": {
     "header": {"title": {"tag": "plain_text", "content": content.split(chr(10))[0]}, "template": "green"},
     "elements": [{"tag": "div", "text": {"tag": "lark_md", "content": content}}]
  }
}))
PYEOF
)

# The card-as-plain more complex; simpler: use the "text"/"post" message type
# which every custom bot accepts without card schema risk.
# Rebuild as a rich text (post) message with the title as first line.
payload=$(python - "$title" "$body_lines" <<'PYEOF'
import json, sys
title, body = sys.argv[1], sys.argv[2]
print(json.dumps({
  "msg_type": "text",
  "content": {"text": title + "\n" + body}
}))
PYEOF
)

# Send.
curl_send() {
    local sign_args=()
    if [ -n "$SIGN_PART" ]; then
        sign_args=(--data-urlencode "timestamp=${ts_part}" --data-urlencode "sign=${sig_part}")
    fi
    if [ -n "${sign_args[*]:-}" ]; then
        # Feishu sign is in the body, not query, for custom bots: embed sign &
        # timestamp into the JSON payload is NOT standard; instead use the header:
        # https://open.feishu.cn/document/client-docs/bot-v3/add-custom-bot#756b82f3
        curl -s -X POST "$WEBHOOK" \
            -H "Content-Type: application/json; charset=utf-8" \
             --data "$(python - "$payload" "${ts_part}" "${sig_part}" <<'PYEOF'
import json,sys
p=json.loads(sys.argv[1]); p["timestamp"]=sys.argv[2]; p["sign"]=sys.argv[3]
print(json.dumps(p))
PYEOF
            )" || true
    else
        curl -s -X POST "$WEBHOOK" -H "Content-Type: application/json; charset=utf-8" \
            --data "$payload" || true
    fi
}

RESP=$(curl_send)
echo "  [feishu] webhook response: ${RESP:0:200}"

# Feishu returns code=0 on success.
if echo "$RESP" | grep -q '"code":0'; then
    echo "  [feishu] notification sent ($EVENT)"
    exit 0
else
    echo "  [feishu] warning: notification not acknowledged: $RESP" >&2
    # Non-blocking by design: do not fail the release because a chat bot is down.
    exit 0
fi
