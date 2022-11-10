// Copyright (c) 2022 by Tyab. All rights reserved
//
#include <stdio.h>
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_set>
#include <filesystem>
#include "MMDatUtil.h"



void header()
{
    printf("Tyab's Manic Miners .DAT utility v%s\n", __DATE__);
}


class CommandLineParser
{
  public:
      CommandLineParser() {}
      ~CommandLineParser() {}

    // options start with -  --  /  any work. spaces separate options unless they are in ""
    // options are converted to lower case, parameters are left unchanged but "" are removed
    void Tokenize(std::string_view cmdline)
    {
        m_tokens.clear();   // make sure no tokens

        std::string::size_type pos = 0;
        std::string::size_type len = cmdline.size();
        std::string token;
        token.reserve(len+1);

        for (; pos < len; pos++)
        {
            if (std::isspace(cmdline[pos]))   // skip leading space
                continue;
            
            // have non-space char

            if ((cmdline[pos] == '-') || (cmdline[pos] == '/'))  // starting option
            {
                pos++;
                if ((pos < len) && cmdline[pos] == '-')  // we allow -- and even /- as option
                    pos++;
                if (pos >= len)   // TODO return empty option error
                {
                    m_bValid = false;
                    m_error = "empty option at end of line";
                    return;
                }
                // beginning of options. get chars until space
                token.clear();
                token += '-';   // pretend it started with a single -
                for (; pos < len && !std::isspace(cmdline[pos]); pos++)
                    token += cmdline[pos];
                m_tokens.push_back(toLower(token));     // save lower case of token
            }
            else if (cmdline[pos] == '"')
            {
                bool bEndQuote = false;
                token.clear();
                // scan until matching " 
                for (pos++; pos < len; pos++)
                {
                    if (cmdline[pos] == '"')
                    {
                        if ((token.length() > 0) && (token[token.length() - 1] == '\\'))  // check for \"
                            token[token.length() - 1] = '"';
                        else
                        {
                            bEndQuote = true;
                            break;
                        }
                    }
                    token += cmdline[pos];
                }
                if (!bEndQuote)  // TODO error string did not have final closing "
                {
                    m_bValid = false;
                    m_error = "missing ending \"";
                    return;
                }
                else
                    m_tokens.push_back(token);
            }
            else  // parameter just copy everything until next white space
            {
                for (token = cmdline[pos++]; (pos < len) && !isspace(cmdline[pos]); pos++)
                    token += cmdline[pos];
                m_tokens.push_back(token);
            }
        }
        m_bValid = true;
    }

    void buildOptions()
    {
        // first token (index 0) is the executing binary, so skip it.
        for (std::size_t i = 1; i < m_tokens.size() && m_bValid; i++)
        {
            if (m_tokens[i][0] == '-')   // token is an option
            {
                optionNameID tok(m_tokens[i], 0);  // id does not matter, just looking for string
                auto it = m_options.find(tok);
                if (it == m_options.cend())
                {
                    m_bValid = false;
                    m_error = "Unknown option: " + m_tokens[i];
                    return;   // error unknown option
                }

                // we have option. do its action
                switch ((*it).getId())
                {
                case 0: // help no parameters
                    m_cmdOptions.m_bHelp = true;
                    break;

                case 1: // srcmap  filename parameter
                    m_cmdOptions.m_srcmap = getStringParm(++i);
                    break;

                case 2: // outmap  filename parameter
                    m_cmdOptions.m_outmap = getStringParm(++i);
                    break;

                case 3: // script  filename parameter
                    m_cmdOptions.m_srcscript = getStringParm(++i);
                    break;

                case 4:  // mergeheight no parameter
                    m_cmdOptions.m_bMergeHeight = true;
                    break;

                case 5:  // mergecrystals  no parameter
                    m_cmdOptions.m_bMergeCrystal = true;
                    break;

                case 6:  // mergeore  no parameter
                    m_cmdOptions.m_bMergeOre = true;
                    break;

                case 7:  // mergetiles  no parameter
                    m_cmdOptions.m_bMergeHeight = true;
                    break;

                case 8:  // offsetrow  integer parameter
                    m_cmdOptions.m_nOffsetRow = getIntParm(++i, true);
                    break;

                case 9:  // offsetcol  integer parameter
                    m_cmdOptions.m_nOffsetCol = getIntParm(++i, true);
                    break;

                case 10:  // overwrite  no parameter
                    m_cmdOptions.m_bOverwrite = true;
                    break;

                case 11:  // resizerow  integer parameter
                    m_cmdOptions.m_nRowResize = getIntParm(++i, false);
                    break;

                case 12:  // resizecol  integer parameter
                    m_cmdOptions.m_nColResize = getIntParm(++i, false);
                    break;

                case 13:  // copysrc  no parameter
                    m_cmdOptions.m_bEraseOutMap = true;
                    m_cmdOptions.m_bOverwrite   = true;
                    break;

                case 14:  // deftile  integer parameter
                    m_cmdOptions.m_nDefTileID = getIntParm(++i, false);
                    if (m_bValid && ((m_cmdOptions.m_nDefTileID <= 0) || (m_cmdOptions.m_nDefTileID > 175)))
                    {
                        m_bValid = false;
                        m_error = "Invalid " + m_tokens[i - 1] + " TileID: " + std::to_string(m_cmdOptions.m_nDefTileID);
                        break;
                    }
                    break;

                case 15:  // defheight  integer parameter
                    m_cmdOptions.m_nDefHeight = getIntParm(++i, true);
                    break;

                case 16:  // decrystal  integer parameter
                    m_cmdOptions.m_nDefCrystal = getIntParm(++i, false);
                    break;

                case 17:  // defore  integer parameter
                    m_cmdOptions.m_nDefOre = getIntParm(++i, false);
                    break;

                case 18: // mapname  name of map parameter
                    m_cmdOptions.m_mapName = getStringParm(++i);
                    break;

                case 19: // creator
                    m_cmdOptions.m_creator = getStringParm(++i);
                    break;

                case 20:  // fix  no parameter
                    m_cmdOptions.m_bFix = true;
                    break;

                case 21:  // sincdirs  string parameter
                    m_cmdOptions.m_sincdirs = getStringParm(++i);
                    break;

                }
            }
        }
    }


    class optionNameID
    {
      public:
          optionNameID() {}
          optionNameID(std::string_view name, int id) : m_name(name), m_id(id) {}
          ~optionNameID() {}

          // comparision operator for hash. Only on the name
          bool operator() (const optionNameID& lhs, const optionNameID& rhs) const
          {
              return lhs.m_name == rhs.m_name;
          }

          // hash operator for hash - only on the name field
          std::size_t operator()(const optionNameID& s)const noexcept
          {
              return std::hash<std::string_view>{}(m_name);
          }

          int getId() const { return m_id; }

    protected:
        std::string_view m_name;
        int              m_id = 0;
    };

    bool isValid() const { return m_bValid; }
    const std::string& getError() const { return m_error; }


    const CommandLineOptions& getOptions() const { return m_cmdOptions; }



  protected:

    std::vector<std::string> m_tokens;  // parsed tokens

    const std::unordered_set<optionNameID,optionNameID,optionNameID> m_options =  // predefined command line options
    {
        { "-help",          0 },
        { "-srcmap",        1 },
        { "-outmap",        2 },
        { "-script",        3 },
        { "-mergeheight",   4 },
        { "-mergecrystal",  5 },
        { "-mergeore",      6 },
        { "-mergetile",     7 },
        { "-offsetrow",     8 },
        { "-offsetcol",     9 },
        { "-overwrite",    10 },
        { "-resizerow",    11 },
        { "-resizecol",    12 },
        { "-copysrc",      13 },
        { "-deftile",      14 },
        { "-defheight",    15 },
        { "-defcrystal",   16 },
        { "-defore",       17 },
        { "-mapname",      18 },
        { "-creator",      19 },
        { "-fix",          20 },
        { "-sincdirs",     21 },
    };

    std::string_view getStringParm(std::size_t i)
    {
        if (i < m_tokens.size())
            return m_tokens[i];

        m_bValid = false;
        m_error = "missing string for option: ";
        m_error += m_tokens[i - 1];
        return std::string_view();
    }

    int getIntParm(std::size_t i, bool bAllowNeg)
    {
        if (i < m_tokens.size())
            return getInteger(m_tokens[i], bAllowNeg, m_tokens[i - 1]);
        m_bValid = false;
        m_error = "missing integer for option: ";
        m_error += m_tokens[i - 1];
        return 0;
    }



    int getInteger(std::string_view str, bool bAllowNeg, std::string_view option)
    {
        if (isInteger(str, bAllowNeg))
            return stoi(str);
        m_bValid = false;
        m_error = "Invalid integer value: ";
        m_error += str;
        m_error += " for option: ";
        m_error += option;
        return 0;
    }

    CommandLineOptions  m_cmdOptions;
    std::string         m_error;
    bool                m_bValid = false;


};



// if null string returned, we already displayed the error message
std::string getTempFile()
{
    std::string retS;

    char apiPathbuff[MAX_PATH + 16] = { 0 };
    if ((GetTempPathA((DWORD)sizeof(apiPathbuff), apiPathbuff) > 0) && (apiPathbuff[0] != 0))
    {
        char apiTFile[MAX_PATH + 16] = { 0 };
        if ((GetTempFileNameA(apiPathbuff, "TMU", 0, apiTFile) > 0) && (apiTFile[0] != 0))
        {
            // make sure it works
            HANDLE han = CreateFileA(apiTFile, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
            if (han != INVALID_HANDLE_VALUE)
            {
                CloseHandle(han);
                retS = apiTFile;
            }
            else
            {
                printf("  ERROR unable to use temp file - TMP or TEMP environment vars may be invalid\n");
                printf("  File: %s\n", apiTFile);
            }
        }
        else
        {
            printf("  ERROR unable to use temp file - TMP or TEMP environment vars may be invalid\n");
            printf("  Dir:: %s\n", apiPathbuff);
        }
    }
    else
    {
        printf("  ERROR unable to get temporary path - TMP or TEMP environment vars may be invalid\n");
    }
    return retS;
}


class tempFileCleanup
{
  public:
    tempFileCleanup() = default;
    tempFileCleanup(const std::string& str)
    {
        m_str = str;
    }
    ~tempFileCleanup()
    {
        remove(m_str.c_str());  // cleanup temp file.
    }

    const std::string& get() const { return m_str; }

protected:
    std::string m_str;
};



void help()
{
    printf("\n");
    printf("  Usage:  <option> <parameter>\n");
    printf("    options begin with - or -- or / and end with a space\n");
    printf("    parameters are until the next space unless it is double quoted.\n");
    printf("    quoted strings may have a \\\" in them to embed a quote.\n");
    printf("    Option:\n");
    printf("      -help          display this help\n");
    printf("      -srcmap        file name of a source merge .DAT\n");
    printf("      -outmap        file name of a destination .DAT\n");
    printf("      -overwrite     allow changing existing outmap\n");
    printf("      -copysrc       outmap is recreated from srcmap, implies -overwrite\n");
    printf("      -mergeheight   merge height values from srcmap into outmap\n");
    printf("      -mergecrystal  merge crystals values from srcmap into outmap\n");
    printf("      -mergeore      merge ore values from srcmap into outmap\n");
    printf("      -mergetile     merge tile values from srcmap into outmap\n");
    printf("      -offsetrow     add row offset when merging/copying srcmap into outmap\n");
    printf("      -offsetcol     add col offset when merging/copying srcmap into outmap\n");
    printf("      -resizerow     resize outmap rows for tiles,height,resources\n");
    printf("      -resizecol     resize outmap cols for tiles,height,resources\n");
    printf("      -deftile       value for invalid tiles or resize, default 1\n");
    printf("      -defheight     value for invalid heights or resize, default 0\n");
    printf("      -defcrystal    value for invalid crystals or resize, default 0\n");
    printf("      -defore        value for invalid ore or resize, default 0\n");
    printf("      -mapname       levelname: value saved in outmap info section\n");
    printf("      -creator       creator: value saved in outmap info section\n");
    printf("      -fix           fix invalid/missing tile, height, crystal, ore values\n");
    printf("      -script        filename of script file to replace outmap's script. TODO\n");
    printf("      -sincdirs      ; separated list of paths to search for script includes\n");
    printf("\n");
    printf("  -srcmap is used to provide merge data unless -copysrc is used\n");
    printf("  -script will replace outmap's script with the contents of that file.\n");
    printf("  -copysrc may be combined with resize and offsets\n");
    printf("\n");
    printf(" see TyabMMDatUtil.pdf for more details and examples\n");
}


void showHelpOption()
{
    printf(" For available options, use -help\n");
}


int main(int , char* )
{
    header();
    
    std::string commandLine;
    LPSTR pCmdLine = GetCommandLineA();
    if (pCmdLine)
        commandLine = pCmdLine;

    CommandLineParser cmdParser;
    cmdParser.Tokenize(commandLine);
    if (!cmdParser.isValid())
    {
        printf(" ERROR: %s\n",cmdParser.getError().c_str());
        showHelpOption();
        return 1;
    }
    cmdParser.buildOptions();
    if (!cmdParser.isValid())
    {
        printf(" ERROR: %s\n", cmdParser.getError().c_str());
        showHelpOption();
        return 1;
    }

    if (cmdParser.getOptions().m_bHelp)
    {
        help();
        return 0;
    }

    // check for options that combined are invalid
    // in order to merge, we must have a source map.
    if (cmdParser.getOptions().m_srcmap.empty())
    {
        if (cmdParser.getOptions().m_bMergeCrystal ||
            cmdParser.getOptions().m_bMergeHeight ||
            cmdParser.getOptions().m_bMergeOre ||
            cmdParser.getOptions().m_bMergeTiles ||
            cmdParser.getOptions().m_bEraseOutMap ||
            cmdParser.getOptions().m_nOffsetCol ||
            cmdParser.getOptions().m_nOffsetRow ||
            cmdParser.getOptions().m_outmap.empty()     // both srcmap and outmap cannot be empty
            )
        {
            printf(" ERROR: options require a srcmap\n");
            showHelpOption();
            return 1;
        }
    }
    else
    {
        std::size_t npos = toLower(cmdParser.getOptions().m_srcmap).rfind(".dat");
        if (npos == std::string::npos)
        {
            printf(" ERROR: Source map: %s must have a .dat extension\n", cmdParser.getOptions().m_srcmap.c_str());
            showHelpOption();
            return 1;
        }
    }
    if (!cmdParser.getOptions().m_outmap.empty())
    {
        std::size_t npos = toLower(cmdParser.getOptions().m_outmap).rfind(".dat");
        if (npos == std::string::npos)
        {
            printf(" ERROR: outmap: %s must have a .dat extension\n", cmdParser.getOptions().m_srcmap.c_str());
            showHelpOption();
            return 1;
        }
    }
    // using eraseoutmap with merge options makes no sense
    if (cmdParser.getOptions().m_bEraseOutMap)
    {
        if (cmdParser.getOptions().m_bMergeCrystal ||
            cmdParser.getOptions().m_bMergeHeight ||
            cmdParser.getOptions().m_bMergeOre ||
            cmdParser.getOptions().m_bMergeTiles
            )
        {
            printf(" ERROR: merging options have no meaning when using -eraseoutmap\n");
            return 1;
        }
    }

    RRMap srcMap;
    RRMap outMap;
    ScriptFile scriptFile;

    // get srcmap if we want one
    if (!cmdParser.getOptions().m_srcmap.empty())
    {
        FILE* fp = nullptr;
        errno_t error = fopen_s(&fp, cmdParser.getOptions().m_srcmap.c_str(), "rb");
        if (error || !fp)
        {
            printf("ERROR: Unable to open for read\n");
            printf("  %s\n", cmdParser.getOptions().m_srcmap.c_str());
            showHelpOption();
            return 1;
        }
        printf("  Reading: %s\n", cmdParser.getOptions().m_srcmap.c_str());
        srcMap.getEncoding(fp);
        if (srcMap.getUnicode())
        {
            printf("  Unicode file: %s format.\n", srcMap.getUnicodeLE() ? "LE" : "BE");
        }
        else if (srcMap.getUTF8BOM())
        {
            printf("  UTF-8 file with BOM\n");
        }

        srcMap.readMap(fp, cmdParser.getOptions());
        fclose(fp);

        // display any warnings found
        for (int i = 0; i < srcMap.getError().getWarnings().size(); i++)
        {
            printf("  Warning: %s\n", srcMap.getError().getWarnings()[i].c_str());
            printf("     line: %d : %s\n", srcMap.getError().getWarnLines()[i].getLineNum(), std::string(srcMap.getError().getWarnLines()[i].getLine()).c_str());
        }

        if (!srcMap.valid())
        {
            for (int i = 0; i < srcMap.getError().getErrors().size(); i++)
            {
                printf("  Error: %s\n", srcMap.getError().getErrors()[i].c_str());
                printf("   line: %d : %s\n", srcMap.getError().getErrLines()[i].getLineNum(), std::string(srcMap.getError().getErrLines()[i].getLine()).c_str());
            }
            return 1;   // errors cannot be fixed
        }
        else   // normal processing
        {
            printf("  Rows: %d  Columns: %d  Total lines: %d\n", srcMap.height(), srcMap.width(), srcMap.lines());
        }
    }

    // have an output map
    if (!cmdParser.getOptions().m_outmap.empty())
    {
        bool bExists = fExists(cmdParser.getOptions().m_outmap);
        if (bExists)
        {
            if (!cmdParser.getOptions().m_bEraseOutMap && !cmdParser.getOptions().m_bOverwrite)
            {
                printf(" ERROR: to overwrite existing outmap use -overwrite or -eraseoutmap options\n");
                return 1;
            }
        }
        else // outmap file does not exist
        {
            if (!srcMap.valid() || !cmdParser.getOptions().m_bEraseOutMap)
            {
                printf(" ERROR: non-existent outmap requires a srcmap and -copysrc option\n");
                return 1;
            }
        }

        tempFileCleanup tempOut = getTempFile();  // temp file is auto deleted on this scope

        if (tempOut.get().empty())
        {
            return 1;   // error working with temp file.
        }

        // tempOutMap is a temp file and can be used.
        // two cases. If using -eraseoutmap then copy the srcmap.
        // otherwise, if we have an existing outmap then copy it.
        if (cmdParser.getOptions().m_bEraseOutMap)
        {
            BOOL retval = CopyFileA(cmdParser.getOptions().m_srcmap.c_str(), tempOut.get().c_str(), false);      // copy src to temp - this ensures we have disk space
            if (retval == 0)
            {
                printf(" ERROR %d unable to copy:\n \"%s\" to \"%s\"\n", GetLastError(), cmdParser.getOptions().m_srcmap.c_str(), tempOut.get().c_str());
                return 1;
            }
        }
        else if (bExists)
        {
            BOOL retval = CopyFileA(cmdParser.getOptions().m_outmap.c_str(), tempOut.get().c_str(), false);      // copy dest to temp - this ensures we have disk space
            if (retval == 0)
            {
                printf(" ERROR %d unable to copy:\n \"%s\" to \"%s\"\n", GetLastError(), cmdParser.getOptions().m_srcmap.c_str(), tempOut.get().c_str());
                return 1;
            }
        }

        if (cmdParser.getOptions().m_bEraseOutMap)
            outMap.copyFrom(srcMap, cmdParser.getOptions());     // outmap is now a copy of srcmap but using options like resize, offsets

        else if (bExists)
        {
            FILE* fp = nullptr;
            errno_t error = fopen_s(&fp, cmdParser.getOptions().m_outmap.c_str(), "rb");
            if (error || !fp)
            {
                printf("ERROR: Unable to open for read\n");
                printf("  %s\n", cmdParser.getOptions().m_outmap.c_str());
                return 1;
            }
            printf("  Reading: %s\n", cmdParser.getOptions().m_outmap.c_str());
            outMap.getEncoding(fp);
            if (outMap.getUnicode())
            {
                printf("  Unicode file: %s format.\n", outMap.getUnicodeLE() ? "LE" : "BE");
            }
            else if (outMap.getUTF8BOM())
            {
                printf("  UTF-8 file with BOM\n");
            }

            outMap.readMap(fp, cmdParser.getOptions());
            fclose(fp);

            // display any warnings found
            for (int i = 0; i < outMap.getError().getWarnings().size(); i++)
            {
                printf("  Warning: %s\n", outMap.getError().getWarnings()[i].c_str());
                printf("     line: %d : %s\n", outMap.getError().getWarnLines()[i].getLineNum(), std::string(outMap.getError().getWarnLines()[i].getLine()).c_str());
            }

            if (!outMap.valid())
            {
                for (int i = 0; i < outMap.getError().getErrors().size(); i++)
                {
                    printf("  Error: %s\n", outMap.getError().getErrors()[i].c_str());
                    printf("   line: %d : %s\n", outMap.getError().getErrLines()[i].getLineNum(), std::string(outMap.getError().getErrLines()[i].getLine()).c_str());
                }
                return 1;   // errors cannot be fixed
            }
            else   // normal processing
            {
                printf("  Rows: %d  Columns: %d  Total lines: %d\n", outMap.height(), outMap.width(), outMap.lines());
            }
        }

        // see if resizing
        if (cmdParser.getOptions().m_nRowResize || cmdParser.getOptions().m_nColResize)
        {
            outMap.resize(cmdParser.getOptions().m_nRowResize, cmdParser.getOptions().m_nColResize, cmdParser.getOptions().m_nDefTileID, cmdParser.getOptions().m_nDefHeight, cmdParser.getOptions().m_nDefCrystal, cmdParser.getOptions().m_nDefOre);
            printf(" Resizing outmap to Rows: %d, Columns: %d\n", outMap.height(), outMap.width());
        }

        // see if merging
        if (cmdParser.getOptions().m_bMergeTiles)
        {
            outMap.mergeTiles(srcMap, cmdParser.getOptions().m_nOffsetRow, cmdParser.getOptions().m_nOffsetCol);
            printf(" Merging srcmap tiles into outmap using offsets row: %d, columns: %d\n", cmdParser.getOptions().m_nOffsetRow, cmdParser.getOptions().m_nOffsetCol);
        }
        if (cmdParser.getOptions().m_bMergeHeight )
        {
            outMap.mergeHeight(srcMap, cmdParser.getOptions().m_nOffsetRow, cmdParser.getOptions().m_nOffsetCol);
            printf(" Merging srcmap Heights into outmap using offsets row: %d, columns: %d\n", cmdParser.getOptions().m_nOffsetRow, cmdParser.getOptions().m_nOffsetCol);
        }
        if (cmdParser.getOptions().m_bMergeCrystal)
        {
            outMap.mergeCrystal(srcMap, cmdParser.getOptions().m_nOffsetRow, cmdParser.getOptions().m_nOffsetCol);
            printf(" Merging srcmap crystals into outmap using offsets row: %d, columns: %d\n", cmdParser.getOptions().m_nOffsetRow, cmdParser.getOptions().m_nOffsetCol);
        }
        if (cmdParser.getOptions().m_bMergeOre)
        {
            outMap.mergeOre(srcMap, cmdParser.getOptions().m_nOffsetRow, cmdParser.getOptions().m_nOffsetCol);
            printf(" Merging srcmap ore into outmap using offsets row: %d, columns: %d\n", cmdParser.getOptions().m_nOffsetRow, cmdParser.getOptions().m_nOffsetCol);
        }

        if (!cmdParser.getOptions().m_mapName.empty())
        {
            outMap.setMapName(cmdParser.getOptions().m_mapName);
            printf(" Setting levelname to: %s\n", cmdParser.getOptions().m_mapName.c_str());
        }

        if (!cmdParser.getOptions().m_creator.empty())
        {
            outMap.setMapCreator(cmdParser.getOptions().m_creator);
            printf(" Setting creator to: %s\n", cmdParser.getOptions().m_creator.c_str());
        }


        // now process any scriptfile 
        if (!cmdParser.getOptions().m_srcscript.empty())
        {
            int err = scriptFile.readFile(cmdParser.getOptions().m_srcscript, cmdParser.getOptions().m_sincdirs, cmdParser.getOptions().m_outmap);

            for (int i = 0; i < scriptFile.getError().getWarnings().size(); i++)
            {
                printf("  Warning: %s\n", scriptFile.getError().getWarnings()[i].c_str());
                printf("  line: %d : %s\n", scriptFile.getError().getWarnLines()[i].getLineNum(), std::string(scriptFile.getError().getWarnLines()[i].getLine()).c_str());
            }


            if (err)
            {
                for (int i = 0; i < scriptFile.getError().getErrors().size(); i++)
                {
                    printf("  Error: %s\n", scriptFile.getError().getErrors()[i].c_str());
                    if (scriptFile.getError().getErrLines()[i].getLineNum() > 0)
                        printf("   line: %d. : %s\n", scriptFile.getError().getErrLines()[i].getLineNum(), std::string(scriptFile.getError().getErrLines()[i].getLine()).c_str());
                }
                return 1;
            }

            // no errors reading script. Lets put it into outmap now.
            outMap.replaceScript(scriptFile);
            printf(" Replacing outmap script. Read in %d lines total\n", scriptFile.size());
        }

        // all outmap modifications done. Time to save it.
        FILE* fp = nullptr;
        errno_t error = fopen_s(&fp, tempOut.get().c_str(), "wt");  // we will use text mode on write
        if (error || !fp)
        {
            printf("ERROR: Unable to open for write\n");
            printf("  %s\n", tempOut.get().c_str());
            return 1;
        }

        int err = outMap.writeMap(fp);
        fclose(fp);
        if (err)
        {
            printf("ERROR: failed to write\n");
            printf("  %s\n", tempOut.get().c_str());
            return 1;
        }

        // temp file has completed. Now copy to real destination
        BOOL retVal = CopyFileA(tempOut.get().c_str(), cmdParser.getOptions().m_outmap.c_str(), 0);
        if (retVal == 0)
        {
            printf(" ERROR %d: unable to copy \"%s\" to \"%s\"", GetLastError(), tempOut.get().c_str(), cmdParser.getOptions().m_outmap.c_str());
            return 1;
        }
        printf(" \"%s\" written. All done.\n", cmdParser.getOptions().m_outmap.c_str());
    }

    printf("\n");
    return 0;
}

