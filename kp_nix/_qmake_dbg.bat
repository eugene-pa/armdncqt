SET PATH=%PATH%;C:\Qt\Qt5.9.1\5.9.1\mingw53_32\bin;C:\Qt\Qt5.9.1\Tools\mingw530_32\bin
qmake.exe kp_nix.pro -spec win32-g++ "CONFIG+=debug"
mingw32-make.exe -f MakeFile.Debug >> log.txt
pause




