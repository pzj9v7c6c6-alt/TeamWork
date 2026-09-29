$unity = ".\__unity_build.cpp"

$files = Get-ChildItem -Path ".\Application" -Recurse -Filter "*.cpp" |
    Sort-Object FullName |
    ForEach-Object {
        '#include "' + $_.FullName.Replace('\', '/') + '"'
    }

$files += '#include "PresentationCLILayer/Image_Filter_Implementation.cpp"'

[System.IO.File]::WriteAllLines($unity, $files)

& "C:\msys64\mingw64\bin\g++.exe" `
    -fdiagnostics-color=always `
    -g `
    $unity `
    -I".\Application" `
    -I"." `
    -lwinmm `
    -lshell32 `
    -o ".\ImageFilter.exe"

$code = $LASTEXITCODE

Remove-Item $unity -Force -ErrorAction SilentlyContinue

exit $code