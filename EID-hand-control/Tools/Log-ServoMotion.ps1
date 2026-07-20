param(
    [Parameter(Mandatory = $true)]
    [string]$PortName,

    [int]$BaudRate = 115200
)

$desktop = [Environment]::GetFolderPath("Desktop")
$timestamp = Get-Date -Format "yyyyMMdd_HHmmss"
$csvOutput = Join-Path $desktop "servo_motion_data_$timestamp.csv"
$xlsxOutput = Join-Path $desktop "servo_motion_data_$timestamp.xlsx"

$serial = New-Object System.IO.Ports.SerialPort $PortName, $BaudRate, "None", 8, "One"
$serial.NewLine = "`n"
$serial.ReadTimeout = 1000

$utf8Bom = New-Object System.Text.UTF8Encoding $true
$writer = New-Object System.IO.StreamWriter $csvOutput, $false, $utf8Bom
$writer.WriteLine("time,source,target,position,status,raw")
$writer.Flush()

function Try-SaveXlsx {
    param(
        [string]$CsvPath,
        [string]$XlsxPath
    )

    try {
        $excel = New-Object -ComObject Excel.Application
        $excel.Visible = $false
        $workbook = $excel.Workbooks.Open($CsvPath)
        $xlOpenXMLWorkbook = 51
        $workbook.SaveAs($XlsxPath, $xlOpenXMLWorkbook)
        $workbook.Close($false)
        $excel.Quit()
        [System.Runtime.InteropServices.Marshal]::ReleaseComObject($workbook) | Out-Null
        [System.Runtime.InteropServices.Marshal]::ReleaseComObject($excel) | Out-Null
        Write-Host "XLSX saved: $XlsxPath"
    }
    catch {
        Write-Host "CSV saved. XLSX conversion skipped because Excel COM is unavailable."
    }
}

try {
    $serial.Open()
    Write-Host "Logging servo data from $PortName at $BaudRate baud..."
    Write-Host "CSV output: $csvOutput"
    Write-Host "Press Ctrl+C to stop."

    while ($true) {
        try {
            $line = $serial.ReadLine().Trim()
        }
        catch [System.TimeoutException] {
            continue
        }

        if ($line -notmatch '^(M11|M11S|M24),') {
            continue
        }

        $parts = $line.Split(',')
        $source = $parts[0]
        $now = Get-Date -Format "yyyy-MM-dd HH:mm:ss.fff"

        if ($parts.Count -ge 2 -and ($parts[1] -eq "START" -or $parts[1] -eq "END" -or $parts[1] -eq "STOP")) {
            $writer.WriteLine("$now,$source,,,$($parts[1]),$line")
            $writer.Flush()

            if ($parts[1] -eq "END" -or $parts[1] -eq "STOP") {
                break
            }
            continue
        }

        if ($parts.Count -ge 3 -and $parts[1] -eq "PAUSE") {
            $position = $parts[2]
            $writer.WriteLine("$now,$source,,$position,PAUSE,$line")
            $writer.Flush()
            continue
        }

        if ($parts.Count -ge 3) {
            $target = $parts[1]
            $position = $parts[2]
            $writer.WriteLine("$now,$source,$target,$position,DATA,$line")
            $writer.Flush()
        }
    }
}
finally {
    $writer.Close()
    if ($serial.IsOpen) {
        $serial.Close()
    }
}

Try-SaveXlsx -CsvPath $csvOutput -XlsxPath $xlsxOutput
Write-Host "Finished. CSV saved: $csvOutput"
