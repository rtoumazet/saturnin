@echo off
@set xgettext_path=D:\Dev\Libraries\gettext-tools-static-0.18.1.1\
@set input_loc=--output-dir=D:\Dev\Sources\VS2017\saturnin-vs2017\saturnin\src
@set output_loc=--output=D:\Dev\Sources\VS2017\saturnin-vs2017\saturnin\lang\saturnin.po
@set files=--files-from=D:\Dev\Sources\VS2017\saturnin-vs2017\saturnin\lang\filelist.txt

@echo on
%xgettext_path%xgettext.exe %input_loc% %output_loc% %files% --keyword=translate
