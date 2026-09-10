Add-Type -AssemblyName System.Xml.Linq
$doc = [System.Xml.Linq.XDocument]::Load('strict_coverage.xml')
$coverage = $doc.Descendants('Coverage') | Select-Object -First 1
$covered = $coverage.Attribute('CoveredLines').Value
$total = $coverage.Attribute('TotalLines').Value
$pct = 100.0 * [int]$covered / [int]$total
Write-Host ("Strict coverage: {0:N2}% ({1}/{2})" -f $pct, $covered, $total)
