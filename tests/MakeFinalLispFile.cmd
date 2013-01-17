@echo off
:: 
echo .
echo Creating a file which uses library code to print strings.
..\bin\JASP mainfile.ls -i .\inc\ -o mylispfile-library.lsp -p

echo .
echo Creating a file which uses other code to print strings.
..\bin\JASP mainfile.ls -i .\inc\ -d USEALERT -d MYTEST,"(defun SETTEST () (alert \"Test Passed!\"))" -print -o mylispfile-alert.lsp


echo .
echo Creating a file which uses other code to print strings.
..\bin\JASP mainfile.ls -i .\inc\ -d USEALERT -d MYTEST,"(defun SETTEST () (alert \"Test Passed!\"))"

:: pause for effect
pause
