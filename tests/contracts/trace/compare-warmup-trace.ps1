[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$ExpectedPath,

    [Parameter(Mandatory = $true)]
    [string]$ActualPath
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Read-TraceDocument {
    param([string]$Path)

    $resolvedPath = (Resolve-Path $Path).Path
    $jsonText = [System.IO.File]::ReadAllText($resolvedPath)
    return ConvertFrom-Json -InputObject $jsonText
}

function Get-MinimalEvents {
    param($Sample)

    return @(
        foreach ($event in $Sample.events) {
            [PSCustomObject]@{
                eventName   = [string]$event.eventName
                phase       = [string]$event.phase
                subjectKind = [string]$event.subjectKind
                subjectId   = [string]$event.subjectId
                order       = [int]$event.order
                status      = [string]$event.status
            }
        }
    )
}

function Test-TracePropertyExists {
    param(
        $Value,
        [string]$PropertyName
    )

    if (-not ($Value -is [pscustomobject])) {
        return $false
    }

    return $null -ne $Value.PSObject.Properties[$PropertyName]
}

function Assert-RuntimeTraceProvenance {
    param($Actual)

    if (-not (Test-TracePropertyExists -Value $Actual -PropertyName "traceSource")) {
        throw "runtime trace provenance missing: traceSource"
    }

    if ($Actual.traceSource -ne "host-embedding-session") {
        throw "runtime trace provenance mismatch: traceSource must be 'host-embedding-session'"
    }

    if (-not (Test-TracePropertyExists -Value $Actual -PropertyName "sessionTrace")) {
        throw "runtime trace provenance missing: sessionTrace"
    }

    if ($null -eq $Actual.sessionTrace) {
        throw "runtime trace provenance missing: sessionTrace"
    }

    $sessionTrace = @($Actual.sessionTrace)
    foreach ($requiredEntry in @(
        "host:start",
        "registration:assemblies",
        "registration:managed-exports",
        "registration:callbacks",
        "managed-entry:begin",
        "managed-entry:end")) {
        if ($sessionTrace -notcontains $requiredEntry) {
            throw "runtime trace provenance missing session entry: $requiredEntry"
        }
    }

    foreach ($sample in $Actual.samples) {
        foreach ($event in $sample.events) {
            $expectedMarker = Get-WarmupSessionMarker -SampleId ([string]$sample.sampleId) -Event $event
            if ($sessionTrace -notcontains $expectedMarker) {
                throw "runtime trace provenance missing warmup session marker: $expectedMarker"
            }
        }
    }
}

function Get-WarmupSessionMarker {
    param(
        [string]$SampleId,
        $Event
    )

    $encodedSubjectId = [Convert]::ToBase64String([System.Text.Encoding]::UTF8.GetBytes([string]$Event.subjectId))
    return "warmup-trace|sampleId=$SampleId|order=$($Event.order)|eventName=$($Event.eventName)|phase=$($Event.phase)|subjectKind=$($Event.subjectKind)|subjectId=$encodedSubjectId|status=$($Event.status)"
}

$expected = Read-TraceDocument -Path $ExpectedPath
$actual = Read-TraceDocument -Path $ActualPath

Assert-RuntimeTraceProvenance -Actual $actual

foreach ($fieldName in @("formatVersion", "traceName", "targetPlatform")) {
    if ($expected.$fieldName -ne $actual.$fieldName) {
        throw "trace field mismatch: $fieldName expected '$($expected.$fieldName)' actual '$($actual.$fieldName)'"
    }
}

if ($expected.samples.Count -ne $actual.samples.Count) {
    throw "sample count mismatch: expected $($expected.samples.Count) actual $($actual.samples.Count)"
}

for ($sampleIndex = 0; $sampleIndex -lt $expected.samples.Count; $sampleIndex++) {
    $expectedSample = $expected.samples[$sampleIndex]
    $actualSample = $actual.samples[$sampleIndex]

    if ($expectedSample.sampleId -ne $actualSample.sampleId) {
        throw "sampleId mismatch at index ${sampleIndex}: expected '$($expectedSample.sampleId)' actual '$($actualSample.sampleId)'"
    }

    if ($expectedSample.scenario -ne $actualSample.scenario) {
        throw "scenario mismatch at index ${sampleIndex}: expected '$($expectedSample.scenario)' actual '$($actualSample.scenario)'"
    }

    $expectedEvents = Get-MinimalEvents -Sample $expectedSample
    $actualEvents = Get-MinimalEvents -Sample $actualSample

    if ($expectedEvents.Count -ne $actualEvents.Count) {
        throw "event count mismatch for sample '$($expectedSample.sampleId)': expected $($expectedEvents.Count) actual $($actualEvents.Count)"
    }

    for ($eventIndex = 0; $eventIndex -lt $expectedEvents.Count; $eventIndex++) {
        $expectedEvent = $expectedEvents[$eventIndex]
        $actualEvent = $actualEvents[$eventIndex]

        foreach ($fieldName in @("eventName", "phase", "subjectKind", "subjectId", "order", "status")) {
            if ($expectedEvent.$fieldName -ne $actualEvent.$fieldName) {
                throw "event mismatch for sample '$($expectedSample.sampleId)' at event index ${eventIndex} field '$fieldName': expected '$($expectedEvent.$fieldName)' actual '$($actualEvent.$fieldName)'"
            }
        }
    }
}

Write-Host "Warmup trace compare passed: $ActualPath"
