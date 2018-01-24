@echo off
for /f "delims=" %%x in (config.txt) do (set "%%x")
@set input_loc=--output-dir=%src_path%
@set output_loc=--output=%lang_path%\saturnin.po
@set files=--files-from=%lang_path%\filelist.txt

@echo on
%xgettext_path%\xgettext.exe %input_loc% %output_loc% %files% --keyword=tr
