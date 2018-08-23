@echo off
for /f "delims=" %%x in (config.txt) do (set "%%x")
rem @set input_loc=--output-dir=%src_path%
@set input_loc=-D %src_path%
@set output_loc=--output=%lang_path%\po\saturnin.pot
@set files=--files-from=%lang_path%\filelist.txt

@echo on
%xgettext_path%\xgettext.exe %input_loc% %output_loc% %files% --keyword=tr --join-existing --sort-by-file
