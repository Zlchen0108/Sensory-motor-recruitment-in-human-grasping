param(
    [Parameter(Mandatory = $true)]
    [string]$PortName,

    [int]$BaudRate = 115200
)

$desktop = [Environment]::GetFolderPath("Desktop")
$output = Join-Path $desktop "motor11_motion_data.csv"

$serial = New-Object System.IO.Ports.SerialPort $PortName, $BaudRate, "None", 8, "One"
$serial.NewLine = "`n"
$serial.ReadTimeout = 1000

$writer = New-Object System.IO.StreamWriter $output, $false, [System.Text.Encoding]::UTF8
$headerWritten = $false

try {
    $serial.Open()
    Write-Host "Logging M11 data from $PortName at $BaudRate baud..."
    Write-Host "Output: $output"
    Write-Host "Press Ctrl+C to stop."

    while ($true) {
        try {
            $line = $serial.ReadLine().Trim()
        }
        catch [System.TimeoutException] {
            continue
        }

        if (-not $line.StartsWith("M11,")) {
            continue
        }

        if ($line -eq "M11,END,0,0,0" -or $line -eq "M11,STOP,0,0,0") {
            Write-Host "Motion finished. File saved: $output"
            break
        }

        $csvLine = $line.Substring(4)
        if ($csvLine -eq "sample,cycle,target,pos") {
            if (-not $headerWritten) {
                $writer.WriteLine($csvLine)
                $headerWritten = $true
            }
        }
        else {
            $writer.WriteLine($csvLine)
        }
        $writer.Flush()
    }
}
finally {
    $writer.Close()
    if ($serial.IsOpen) {
        $serial.Close()
    }
}
