$gModDir = (Get-Location).Path
Write-Host Set current directory to $gModDir
cmake -B build
cd build/_deps/zlib-src
cmake -A Win32 -DCMAKE_INSTALL_PREFIX="$($gModDir)\bin\zlib" .
cmake --build . --config Release
cmake --install . --config Release
cd $gModDir
cd build/_deps/libzip-src
cmake -DZLIB_LIBRARY="$($gModDir)\bin\zlib\lib\zlibstaticd.lib" -DZLIB_INCLUDE_DIR="$($gModDir)\bin\zlib\include" .
cd $gModDir
