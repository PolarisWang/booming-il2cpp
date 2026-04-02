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

$expected = Read-TraceDocument -Path $ExpectedPath
$actual = Read-TraceDocument -Path $ActualPath

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
