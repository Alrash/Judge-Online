@echo off
::ʹ��utf-8����
chcp 65001
for /r . %%n in (*.cpp,*.h) do @(
    for /f "delims=" %%i in (%%n) do @(echo. %%i) >> %%n_tmp
    del %%n
)
ren *.cpp_tmp *.cpp
ren *.h_tmp *.h