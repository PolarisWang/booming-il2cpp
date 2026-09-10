#!/usr/bin/env python3
"""docs-linkcheck.py — reusable wiki/docs link integrity checker.

Deterministically verifies relative markdown links in wiki/ and docs/ resolve
to a real file, handling CJK filenames and UTF-8 percent-encoding. Designed to be
used BEFORE and AFTER any doc move / rename to prevent broken links.

Usage:
    python tests/runner/docs-linkcheck.py                   # audit wiki/ + docs/
    python tests/runner/docs-linkcheck.py --root wiki        # only wiki
    python tests/runner/docs-linkcheck.py --strict           # also flag orphans

Exits nonzero if any UNRESOLVED link is found. Encoding-aware:
- decodes %XX in markdown link targets
- resolves relative to the referencing file's directory
- tolerates anchors (#...) and external (http) / bare references
"""
from __future__ import annotations

import argparse
import os
import re
import sys
import urllib.parse
from pathlib import Path

LINK_RE = re.compile(r'\]\(([^)]+)\)')


def collect_md(root: str) -> dict:
    out = {}
    for dp, _, fs in os.walk(root):
        for f in fs:
            if f.endswith('.md'):
                p = os.path.normpath(os.path.join(dp, f))
                out[p] = open(p, encoding='utf-8', errors='replace').read()
    return out


def resolve_target(src_file: str, raw: str, all_md: set) -> bool:
    """Return True if the raw target resolves (or is a false-positive to skip)."""
    target = raw.split('#')[0].split('?')[0].strip()
    if not target:
        return True  # pure anchor
    dec = urllib.parse.unquote(target)
    # skip external / non-filesystem refs
    if dec.startswith(('http://', 'https://', 'mailto:', '//')):
        return True
    # skip absolute Windows/Linux paths (prose in historical docs, not links)
    if re.match(r'^[a-zA-Z]:[\\/]', dec) or dec.startswith('/'):
        return True
    # skip directory references (link to a folder, valid in GitHub rendering)
    if dec.endswith('/') or (not dec.endswith('.md') and not dec.endswith(
            ('.json', '.html', '.jpg', '.png', '.yaml', '.yml', '.cs', '.py',
             '.cpp', '.h', '.txt', '.sh', '.cmake', '.ps1', '.csproj'))):
        return True
    if dec.startswith(('wiki/', 'docs/')):
        full = os.path.normpath(dec)
    else:
        full = os.path.normpath(os.path.join(os.path.dirname(src_file), dec))
    return full in all_md


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument('--root', default='.')
    ap.add_argument('--strict', action='store_true', help='also report orphan md')
    ar = ap.parse_args()

    ndups = 0
    all_md = set()
    # collect md from both wiki/ and docs/ under root (repo root when root='.')
    for base in ['wiki', 'docs']:
        p = os.path.join(ar.root, base)
        if os.path.isdir(p):
            all_md.update(collect_md(p).keys())
    # also .ai/skills md (they can reference docs)
    skills = os.path.join(ar.root, '.ai', 'skills')
    if os.path.isdir(skills):
        all_md.update(collect_md(skills).keys())

    broken = []
    scanned = {}
    for base in ['wiki', 'docs']:
        p = os.path.join(ar.root, base)
        if not os.path.isdir(p):
            continue
        for src, text in collect_md(p).items():
            scanned[src] = True
            for m in LINK_RE.finditer(text):
                raw = m.group(1)
                if not resolve_target(src, raw, all_md):
                    broken.append((os.path.relpath(src), raw))

    # orphans (strict): md files not in all_md-as-consumed except INDEX
    orphans = []
    if ar.strict:
        consumed = set()
        for src, text in ((s, open(s, encoding='utf-8', errors='replace').read()) for s in scanned):
            for m in LINK_RE.finditer(text):
                raw = m.group(1)
                target = raw.split('#')[0].split('?')[0]
                dec = urllib.parse.unquote(target)
                if dec.startswith('http') or not dec.endswith('.md'):
                    continue
                full = os.path.normpath(os.path.join(os.path.dirname(src), dec))
                consumed.add(full)
        for src in scanned:
            b = os.path.basename(src).upper()
            if b == 'INDEX.MD':
                continue
            if src not in consumed and src not in all_md - set() or src not in consumed:
                if src not in consumed:
                    orphans.append(src)

    print(f"sources scanned: {len(scanned)}, md files on disk: {len(all_md)}")
    print(f"broken links: {len(broken)}")
    for s, t in sorted(set(broken))[:40]:
        print(f"  {s} -> {t}")
    if ar.strict:
        print(f"orphans (non-INDEX, unreferenced): {len(orphans)}")
        for o in orphans[:30]:
            print("  ", o)

    return 1 if broken else 0


if __name__ == '__main__':
    sys.exit(main())
