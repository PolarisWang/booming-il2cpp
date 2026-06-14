param(
    [string]$RepoRoot,
    [switch]$Verbose
)

if (-not $RepoRoot) {
    $RepoRoot = git -C $PSScriptRoot rev-parse --show-toplevel
}

$entryStubPath = Join-Path $RepoRoot '.claude/skills/dev-il2cpp/SKILL.md'
$libraryEntry   = Join-Path $RepoRoot 'skills/library/skills/dev-il2cpp/SKILL.md'

Write-Host "[skill-sync] Verifying routing stub and library consistency..."
Write-Host "[skill-sync] (Junction mechanism removed; routing via skill registry)"

$ok = $true

if (-not (Test-Path -LiteralPath $entryStubPath)) {
    Write-Error "[skill-sync] MISSING: $entryStubPath"
    $ok = $false
} else {
    Write-Host "[skill-sync]  ✓ Routing stub exists"
}

if (-not (Test-Path -LiteralPath $libraryEntry)) {
    Write-Error "[skill-sync] MISSING: $libraryEntry"
    $ok = $false
} else {
    Write-Host "[skill-sync]  ✓ Library entry SKILL.md exists"
}

if ($ok) {
    Write-Host "[skill-sync] Routing stub and library are consistent."
    exit 0
} else {
    Write-Error "[skill-sync] Consistency check FAILED."
    exit 1
}
