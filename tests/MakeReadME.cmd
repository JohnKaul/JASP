@echo off

set file=AcmeAnvilReadME.txt
echo .
echo First, we create a text file called %file% and allow printing.
..\bin\JASP testReadME.txt -p -d ANVILS,"BUGGY-WHIP" -d anvil,"Buggy-Whip" -o %file%

set file=
echo .
echo Second, we will omit the printing and the file output options.
echo NOTE: This will print the results to the command line (useful 
echo       for pipelining--look that up if you don't know what it is 
echo       and why it is good.)
echo .
echo === Begin ===
..\bin\JASP testReadME.txt -d ANVILS,"BUGGY-WHIP" -d anvil,"Buggy-Whip"
echo === End ===
echo .
pause
