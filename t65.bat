@echo off
set binfile="%temp%/a.nes"
set objfile="%temp%/a.o"
set configfile="%temp%\a.txt"
echo MEMORY {CODE:     start = $8000, size = $8000-6, fill = yes;}SEGMENTS {CODE:        load = CODE, type = rw;} > %configfile%
ca65 %1 -o %objfile% || goto :error
ld65 %objfile% -o %binfile% --config %configfile% || goto :error
t65sim %binfile%
goto :eof
:error
echo Build failed
exit /b %errorlevel%
:eof
