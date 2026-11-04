param([switch]$PackagingOnly)
$ErrorActionPreference = 'Stop'
if ($env:GITHUB_ACTIONS -ne 'true' -or $env:RUNNER_ENVIRONMENT -ne 'github-hosted' -or -not $IsWindows) {
    throw 'This script requires a disposable GitHub-hosted Windows runner.'
}
New-ItemProperty 'HKLM:\SYSTEM\CurrentControlSet\Control\FileSystem' -Name LongPathsEnabled -Value 1 -PropertyType DWord -Force | Out-Null
New-Item 'HKLM:\SOFTWARE\Microsoft\Windows\CurrentVersion\AppModelUnlock' -Force | Out-Null
New-ItemProperty 'HKLM:\SOFTWARE\Microsoft\Windows\CurrentVersion\AppModelUnlock' -Name AllowDevelopmentWithoutDevLicense -Value 1 -PropertyType DWord -Force | Out-Null
foreach ($tool in @('python', 'git', 'zstd', '7z')) {
    if (-not (Get-Command $tool -ErrorAction SilentlyContinue)) { throw "Missing required tool: $tool" }
}
if (-not $PackagingOnly) {
    # Remove unrelated preinstalled SDKs, leaving Visual Studio and Windows Kits.
    $unused = @(
        'C:\Android', 'C:\Program Files\Android', 'C:\Program Files\Java',
        'C:\ghcup', 'C:\Miniconda', 'C:\Strawberry', 'C:\Selenium',
        'C:\Program Files\LLVM', 'C:\Program Files\dotnet',
        'C:\Program Files\Docker', 'C:\Program Files\MongoDB',
        'C:\Program Files\PostgreSQL', 'C:\Program Files\R',
        "$env:USERPROFILE/.rustup", "$env:USERPROFILE/.cargo"
    )
    if ($env:RUNNER_TOOL_CACHE) {
        foreach ($tool in @('CodeQL', 'go', 'Ruby', 'node', 'Java_Temurin-Hotspot_jdk', 'Java_Microsoft_jdk', 'Java_Zulu_jdk')) {
            $unused += Join-Path $env:RUNNER_TOOL_CACHE $tool
        }
    }
    foreach ($path in $unused) {
        if (Test-Path -LiteralPath $path) { Remove-Item -LiteralPath $path -Recurse -Force }
    }
    New-Item -ItemType Directory -Force 'C:\t\cache' | Out-Null
    python infra/ci/windows_host.py bootstrap --cache 'C:\t\cache' --report "$env:RUNNER_TEMP/runner.json"
    if ($LASTEXITCODE -ne 0) { throw 'Windows toolchain provisioning failed.' }
}
Get-PSDrive -PSProvider FileSystem | Select-Object Name, Used, Free
