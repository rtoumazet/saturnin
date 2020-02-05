Saturnin uses the [spirit-po](https://github.com/cbeck88/spirit-po) library to handle translations, meaning that only PO files are used, without the need of a compiled MO file.

# Generating the translation file #
In directory **./saturnin/lang** you'll find a batch file named **generate_translation.bat**. Running if will do the following :
- Prompt you for a [country code](https://www.gnu.org/software/gettext/manual/html_node/Country-Codes.html) 
- Extracts all the strings of the source to filelist.txt
- Generates / updates the template file (./saturnin/lang/po/saturnin.pot
