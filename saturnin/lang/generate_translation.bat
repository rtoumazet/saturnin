@echo off
set /p country_code="Enter country code: "
mkdir .\po\%country_code%

echo Extracting strings ...
@set dirs=..\src;..\src\video\
findstr /m /p /d:%dirs% tr( * > filelist_tmp.txt

rem Creating / updating POT template
for /f "delims=" %%x in (config.txt) do (set "%%x")
@set input_loc=-D %src_path%
@set output_loc=--output=%lang_path%\po\saturnin.pot
@set files=--files-from=%lang_path%\filelist.txt
@set options=--keyword=tr  --sort-by-file --msgid-bugs-address=saturnin@runik.info --copyright-holder="Renaud TOUMAZET" --package-name=Saturnin
if exist .\po\saturnin.pot (
    echo Updating template file
    %xgettext_path%\xgettext.exe %input_loc% %output_loc% %files% %options% --join-existing
) else (
    echo Creating template file
    %xgettext_path%\xgettext.exe %input_loc% %output_loc% %files% %options%
)

rem Creating / updating PO file from POT template
if exist .\po\%country_code%\saturnin.po (
    echo Updating saturnin.po for '%country_code%' country
    %xgettext_path%\msgmerge.exe -U po/%country_code%/saturnin.po po/saturnin.pot
) else (
    echo Creating saturnin.po for '%country_code%' country
    %xgettext_path%\msginit.exe --input=po/saturnin.pot --locale=%country_code% --output=po/%country_code%/saturnin.po
)

set /p build_mo="Do you want to build saturnin.mo ? (y/n): "
if /i "%build_mo%"=="y" (
    rem Building MO file
    echo Building saturnin.mo for '%country_code%' country
    %xgettext_path%\msgfmt.exe --output-file=po/%country_code%/saturnin.mo po/%country_code%/saturnin.po

    rem Directory generation
    mkdir .\exe\%country_code%\LC_MESSAGES
    xcopy .\po\%country_code%\saturnin.mo .\exe\%country_code%\LC_MESSAGES\saturnin.mo /Y
)
echo Done