# Generate resource_generated.h with integer macros and string macros for versioning.
# Writes ASCII file suitable for rc.exe.
# Always writes to the project's "Generated" folder. Creates folder if missing.

param(
    [string]$UnusedOutFile = ""   # accepted for compatibility but ignored - always use Generated folder
)

# Resolve project directory
$projDir = [Environment]::GetEnvironmentVariable('ProjectDir')
if ([string]::IsNullOrWhiteSpace($projDir)) {
    # fallback to script location if ProjectDir not provided by MSBuild
    $projDir = Split-Path -Parent $MyInvocation.MyCommand.Definition
}

# Target Generated folder under the project
$generatedDir = Join-Path -Path $projDir -ChildPath 'Generated'

# Create folder if not exists
if (-not (Test-Path -LiteralPath $generatedDir)) {
    New-Item -ItemType Directory -Force -Path $generatedDir | Out-Null
}

# Force output file into Generated folder
$OutFile = Join-Path -Path $generatedDir -ChildPath 'resource_generated.h'
$OutFile = [System.IO.Path]::GetFullPath($OutFile)

$d = Get-Date
$verStr = "{0}.{1}.{2}.{3}" -f $d.Year, $d.Month, $d.Day, $d.Hour
$fileVer = "{0},{1},{2},{3}" -f $d.Year, $d.Month, $d.Day, $d.Hour

$lines = @(
    "#pragma once",
    ("#define VER_MAJOR {0}" -f $d.Year),
    ("#define VER_MINOR {0}" -f $d.Month),
    ("#define VER_PATCH {0}" -f $d.Day),
    ("#define VER_BUILD {0}" -f $d.Hour),
    ("#define VER_FILEVERSION {0}" -f $fileVer),
    ("#define VER_PRODUCTVERSION {0}" -f $fileVer),
    ('#define VER_FILEVERSION_STR "{0}\0"' -f $verStr),
    ('#define VER_PRODUCTVERSION_STR "{0}\0"' -f $verStr)
)

# Ensure ASCII encoding (rc.exe expects plain ASCII compatible)
Set-Content -Path $OutFile -Value ($lines -join "`r`n") -Encoding Ascii

# Print where we wrote the file (helps debugging in build logs)
Write-Output "Generated resource header: $OutFile"
