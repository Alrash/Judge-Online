@echo off
@echo off
::使用utf-8编码
chcp 65001
for /r . %%n in (*.cpp,*.h,Makefile,help) do @(
    ::文件换行
    for /f "delims=" %%i in (%%n) do @(echo.%%i) >> %%n_tmp
    ::删除原文件
    del %%n
)
::重命名文件
ren *.cpp_tmp *.cpp
ren *.h_tmp *.h
ren Makefile_tmp Makefile
ren help_tmp helpfile

If Not Exist UTF82ANSI.vbs (
    For /F "usebackq skip=27 tokens=*" %%i In ("%~0") Do Echo %%i>>UTF82ANSI.vbs
)
For /R %%i In (*.h *.cpp) Do (
    UTF82ANSI.vbs "%%i"
)
Echo 转换完成!
Del UTF82ANSI.vbs 
Pause>nul
Del %0
Exit
' *============================================================================*
' * 批量编码转换包括GB2312，UTF-8，Unicode，支持拖动，可扩展到右菜单（发送到） *
' * CodeChange.vbs     BY: yongfa365 http://www.yongfa365.com     2007-10-04 *
' * UTF82ANSI.vbs   BY: fastslz   http://bbs.cn-dos.net          2007-12-03 *
' *============================================================================*
aCode = "UTF-8"
bCode = "GB2312"
Set objArgs = WScript.Arguments
    For I = 0 To objArgs.Count - 1
        FileUrl = objArgs(I)
        Call CheckCode (FileUrl)
        Call WriteToFile(FileUrl, ReadFile(FileUrl, aCode), bCode)
    Next
Function ReadFile(FileUrl, CharSet)
    Dim Str
    Set stm = CreateObject("Adodb.Stream")
    stm.Type = 2
    stm.mode = 3
    stm.charset = CharSet
    stm.Open
    stm.loadfromfile FileUrl
    Str = stm.readtext
    stm.Close
    Set stm = Nothing
    ReadFile = Str
End Function
Function WriteToFile (FileUrl, Str, CharSet)
    Set stm = CreateObject("Adodb.Stream")
    stm.Type = 2
    stm.mode = 3
    stm.charset = CharSet
    stm.Open
    stm.WriteText Str
    stm.SaveToFile FileUrl, 2
    stm.flush
    stm.Close
    Set stm = Nothing
End Function
Function CheckCode (FileUrl)
    Dim slz
    set slz = CreateObject("Adodb.Stream") 
    slz.Type = 1
    slz.Mode = 3
    slz.Open
    slz.Position = 0
    slz.Loadfromfile FileUrl
    Bin=slz.read(2)
    if AscB(MidB(Bin,1,1))=&HEF and AscB(MidB(Bin,2,1))=&HBB Then
       Codes="UTF-8"
       elseif AscB(MidB(Bin,1,1))=&HFF and AscB(MidB(Bin,2,1))=&HFE Then
              Codes="Unicode"
              else
              Codes="GB2312"
    end if
    slz.Close
    set slz = Nothing
End Function