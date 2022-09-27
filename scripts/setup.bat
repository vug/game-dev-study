
REM directory changes won't effect outside of this batch script
setlocal
cd ..\dependencies

REM from https://stackoverflow.com/questions/4619088/windows-batch-file-file-download-from-a-url
powershell -Command "(New-Object Net.WebClient).DownloadFile('https://github.com/libsdl-org/SDL/releases/download/release-2.24.0/SDL2-devel-2.24.0-VC.zip', 'SDL2-devel-2.24.0-VC.zip')"

REM from https://learn.microsoft.com/en-us/powershell/module/microsoft.powershell.archive/expand-archive?view=powershell-7.2
powershell -Command "Expand-Archive -Path SDL2-devel-2.24.0-VC.zip -DestinationPath ."

del SDL2-devel-2.24.0-VC.zip