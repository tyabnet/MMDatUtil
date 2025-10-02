// Copyright (c) 2022 by Tyab. All rights reserved
//
#include <stdio.h>
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_set>
#include <filesystem>
#include <tchar.h>
#include <chrono>
#include <format>
#include <cctype>
#include <algorithm>
#include <cwctype>

#include "MMDatUtil.h"
#include "MMScript.h"


void header()
{
    wprintf(L"Tyab's Manic Miners .DAT utility v%s\n", Unicode::utf8_to_wstring(DATE_ISO).c_str());
}

class CommandLineParser
{
  public:
    CommandLineParser() = default;
    ~CommandLineParser() = default;

    // options start with -  --  /  any work. spaces separate options unless they are in ""
    // options are converted to lower case, parameters are left unchanged but "" are removed
    void Tokenize(std::wstring_view cmdline)
    {
        m_tokens.clear();   // make sure no tokens

        std::wstring::size_type pos = 0;
        std::wstring::size_type len = cmdline.size();
        std::wstring token;
        token.reserve(len+1);

        for (; pos < len; pos++)
        {
            if (std::iswspace(cmdline[pos]))   // skip leading space
                continue;
            
            // have non-space char

            if ((cmdline[pos] == '-') || (cmdline[pos] == '/'))  // starting option
            {
                pos++;
                if ((pos < len) && cmdline[pos] == '-')  // we allow -- and even /- as option
                    pos++;
                if (pos >= len) 
                {
                    m_bValid = false;
                    m_error = L"empty option at end of line";
                    return;
                }
                // beginning of options. get chars until space
                token = '-';    // pretend it started with a single -
                for (; pos < len && !std::iswspace(cmdline[pos]); pos++)
                    token += cmdline[pos];
                m_tokens.push_back(MMUtil::toLower(token));     // save lower case of token
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
                        if (!token.empty() && (token.back() == '\\'))  // check for \"
                            token[token.length() - 1] = '"';    // replace / char with "
                        else
                        {
                            bEndQuote = true;
                            break;
                        }
                    }
                    token += cmdline[pos];
                }
                if (!bEndQuote)  // missing ending "
                {
                    m_bValid = false;
                    m_error = L"missing ending \"";
                    return;
                }
                else
                    m_tokens.push_back(token);
            }
            else  // parameter just copy everything until next white space or ,
            {
                for (token = cmdline[pos++]; (pos < len) && !iswspace(cmdline[pos]) && (cmdline[pos] != ','); pos++)
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
                    m_error = L"Unknown option: " + m_tokens[i];
                    return;   // error unknown option
                }

                // we have option. do its action
                switch ((*it).getId())
                {
                case 0: // help no parameters
                    m_cmdOptions.m_bHelp = true;
                    break;

                case 1: // srcmap  filename parameter
                {
                    m_cmdOptions.m_srcmap = FilenameParameter(getStringParm(++i));
                    if (m_cmdOptions.m_srcmap.empty())
                    {
                        m_bValid = false;
                        m_error = L" -srcmap invalid filename";
                    }
                    break;
                }

                case 2: // outmap  filename parameter
                    m_cmdOptions.m_outmap = FilenameParameter(getStringParm(++i));
                    if (m_cmdOptions.m_outmap.empty())
                    {
                        m_bValid = false;
                        m_error = L" -outmap invalid filename";
                    }
                    break;

                case 3: // script  filename parameter
                    m_cmdOptions.m_srcscript = FilenameParameter(getStringParm(++i));
                    if (m_cmdOptions.m_srcscript.empty())
                    {
                        m_bValid = false;
                        m_error = L" -script invalid filename";
                    }
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
                    m_cmdOptions.m_bMergeTiles = true;
                    break;

                case 8:  // offsetrow  integer parameter
                    m_cmdOptions.m_nOffsetRow = getIntParm(++i, true, tok.getName());
                    break;

                case 9:  // offsetcol  integer parameter
                    m_cmdOptions.m_nOffsetCol = getIntParm(++i, true, tok.getName());
                    break;

                case 10:  // overwrite  no parameter
                    m_cmdOptions.m_bOverwrite = true;
                    break;

                case 11:  // resizerow  integer parameter
                    m_cmdOptions.m_nRowResize = getIntParm(++i, false, tok.getName());
                    break;

                case 12:  // resizecol  integer parameter
                    m_cmdOptions.m_nColResize = getIntParm(++i, false, tok.getName());
                    break;

                case 13:  // copysrc  no parameter
                    m_cmdOptions.m_bCopySrc = true;
                    m_cmdOptions.m_bOverwrite   = true;
                    break;

                case 14:  // deftile  integer parameter
                    m_cmdOptions.m_nDefTileID = getIntParm(++i, false, tok.getName());
                    if (m_bValid && ((m_cmdOptions.m_nDefTileID <= 0) || (m_cmdOptions.m_nDefTileID > 175)))
                    {
                        m_bValid = false;
                        m_error = L"Invalid " + m_tokens[i - 1] + L" TileID: " + std::to_wstring(m_cmdOptions.m_nDefTileID);
                        break;
                    }
                    break;

                case 15:  // defheight  integer parameter
                    m_cmdOptions.m_nDefHeight = getIntParm(++i, true, tok.getName());
                    break;

                case 16:  // decrystal  integer parameter
                    m_cmdOptions.m_nDefCrystal = getIntParm(++i, false, tok.getName());
                    break;

                case 17:  // defore  integer parameter
                    m_cmdOptions.m_nDefOre = getIntParm(++i, false, tok.getName());
                    break;

                case 18: // mapname  name of map parameter
                    m_cmdOptions.m_mapName = Unicode::wstring_to_utf8(getStringParm(++i));
                    break;
                case 19: // creator
                    m_cmdOptions.m_creator = Unicode::wstring_to_utf8(getStringParm(++i));
                    break;

                case 20:  // fix  no parameter
                    m_cmdOptions.m_bFix = true;
                    break;

                case 21:  // sincdirs  string parameter. Build an array of paths from the string
                {
                    std::wstring_view token(getStringParm(++i));   // entire token
                    std::size_t ts = 0;     // start of token
                    for (std::size_t te; (ts < token.size()) && (te = token.find(';',ts)) != token.npos; ts = te + 1)
                    {
                        if (te > ts) // not empty range
                        {
                            std::wstring singledir = FilenameParameter(token.substr(ts, te - ts));
                            if (!singledir.empty())
                                m_cmdOptions.m_sincdirs.push_back(singledir);
                        }
                    }
                    if (ts < token.size())
                    {
                        std::wstring singledir = FilenameParameter(token.substr(ts));
                        if (!singledir.empty())
                            m_cmdOptions.m_sincdirs.push_back(singledir);
                    }
                    if (m_cmdOptions.m_sincdirs.empty())
                    {
                        m_bValid = false;
                        m_error = L" -sincdirs invalid path(s)";
                    }
                    break;
                }

                case 22:  // sfixspace  no parameter
                    m_cmdOptions.m_bScrFixSpace = true;
                    break;

                case 23:  // snocomments  no parameter
                    m_cmdOptions.m_bScrNoComments = true;
                    break;
 
                case 24:  // sdefine key=value
                {
                    if (++i >= m_tokens.size())
                    {
                        m_bValid = false;
                        m_error = L"missing name=value for option " + m_tokens[i - 1];
                        break;
                    }
                    std::size_t epos = m_tokens[i].find('=');
                    if ((epos == std::wstring::npos) || (epos < 1) || ((epos+1) >= m_tokens[i].size()))
                    {
                        m_bValid = false;
                        m_error = L"missing name=value for option " + m_tokens[i - 1];
                        break;
                    }
                    std::wstring_view key = std::wstring_view(m_tokens[i]).substr(0, epos);
                    std::wstring_view value = std::wstring_view(m_tokens[i]).substr(epos+1);
                    if (key.empty() || value.empty())
                    {
                        m_bValid = false;
                        m_error = L"missing name=value for option " + m_tokens[i - 1];
                        break;
                    }
                    std::string keys = Unicode::wstring_to_utf8(removeQuotes(key));
                    if (!MMUtil::isAlphaNumericName(keys))
                    {
                        m_bValid = false;
                        m_error = L"invalid macro name for option " + m_tokens[i - 1];
                        break;
                    }
                    std::string values = Unicode::wstring_to_utf8(removeQuotes(value));

                    m_cmdOptions.addKeyValue(keys, values);
                    break;
                }
                case 25:  // sdatefmd  string parameter - we automatically add double quotes to it.
                    m_cmdOptions.m_datestr = '"' + Unicode::wstring_to_utf8(getStringParm(++i)) + '"';
                    break;

                case 26:  // mergerect
                {   // subregion. startrow,startcol,endrow,endcol. Each will be in its own token
                    m_cmdOptions.m_srow = getIntParm(++i, false, tok.getName());
                    if (m_error.empty())
                        m_cmdOptions.m_scol = getIntParm(++i, false, tok.getName());
                    if (m_error.empty())
                        m_cmdOptions.m_erow = getIntParm(++i, false, tok.getName());
                    if (m_error.empty())
                        m_cmdOptions.m_ecol = getIntParm(++i, false, tok.getName());
                    if (m_error.empty())
                    {
                        if ((m_cmdOptions.m_srow <= m_cmdOptions.m_erow) && (m_cmdOptions.m_scol <= m_cmdOptions.m_ecol))
                            m_cmdOptions.m_bMergeRect = true;
                        else
                        {
                            m_bValid = false;
                            m_error = L"invalid -mergerect values";
                        }
                    }
                    break;
                }

                case 27: // flattenhigh
                {
                    m_cmdOptions.m_flathighval       = getIntParm(++i, true, tok.getName());
                    m_cmdOptions.m_flathighnewheight = getIntParm(++i, true, tok.getName());
                    m_cmdOptions.m_bFlattenHigh      = true;
                    break;
                }

                case 28: // flattenlow
                {
                    m_cmdOptions.m_flatlowval       = getIntParm(++i, true, tok.getName());
                    m_cmdOptions.m_flatlownewheight = getIntParm(++i, true, tok.getName());
                    m_cmdOptions.m_bFlattenLow      = true;
                    break;
                }

                case 29: //flattenbetween
                {
                    m_cmdOptions.m_flatBetweenLow  = getIntParm(++i, true, tok.getName());
                    m_cmdOptions.m_flatBetweenHigh = getIntParm(++i, true, tok.getName());
                    m_cmdOptions.m_flatBetweenVal  = getIntParm(++i, true, tok.getName());
                    m_cmdOptions.m_bFlattenBetween = true;
                    break;
                }

                case 30: // borderheight
                {
                    m_cmdOptions.m_BorderHeight  = getIntParm(++i, true, tok.getName());
                    m_cmdOptions.m_bBorderHeight = true;
                    break;
                }

                case 31: // unicode
                {
                    m_cmdOptions.m_bUTF16 = true;
                    break;
                }

                case 32: // Byte Order Marker
                {
                    m_cmdOptions.m_bBOM = true;
                    break;
                }

                case 33: // 8 byte input with no BOM assume ANSI
                {
                    m_cmdOptions.m_bReadANSI = true;
                    break;
                }

                case 34:  // UTF16BE
                {
                    m_cmdOptions.m_bUTF16     = true;    // if set output UTF16LE format
                    m_cmdOptions.m_bBigEndian = true;    // if set output format is big endian, default false is little endian.
                    break;
                }

                case 35:  // UTF32LE
                {
                    m_cmdOptions.m_bUTF32 = true;    // if set output format is UTF32, either big or little endian
                    break;
                }

                case 36:  // UTF32BE
                {
                    m_cmdOptions.m_bBigEndian = true;    // if set output format is big endian, default false is little endian.
                    m_cmdOptions.m_bUTF32     = true;    // if set output format is UTF32, either big or little endian
                    break;
                }
                case 37:  // UTF8
                {
                    m_cmdOptions.m_bUTF8     = true;    // if set output format is UTF8. This is the default if no other are used.
                    break;
                }

                case 38: // optimize script variables and chain names
                {
                    m_cmdOptions.m_bOptimizeNames = true;
                    break;
                }

                case 39: // remove blank script lines
                {
                    m_cmdOptions.m_bOptimizeBlank = true;
                    break;
                }

                case 40: // briefing  filename parameter
                    m_cmdOptions.m_briefing = FilenameParameter(getStringParm(++i));
                    if (m_cmdOptions.m_briefing.empty())
                    {
                        m_bValid = false;
                        m_error = L" -briefing invalid filename";
                    }

                    break;

                case 41: // briefingsuccess  filename parameter
                    m_cmdOptions.m_success = FilenameParameter(getStringParm(++i));
                    if (m_cmdOptions.m_success.empty())
                    {
                        m_bValid = false;
                        m_error = L" -success invalid filename";
                    }
                    break;

                case 42: // backup  filename parameter
                    m_cmdOptions.m_backup = true;
                    break;

                case 43: // -nobom do not output UTF BOM
                    m_cmdOptions.m_bNoBOM = true;
                    break;

                case 44:  // -srcUTF8  8 bit source not BOM is considered UTF8
                    m_cmdOptions.m_bReadUTF8 = true;
                    break;

                case 45:   // -ansi. Output format is windows code page
                    m_cmdOptions.m_bANSI = true;
                    break;

                case 46:   // -ansi. Output format is windows code page
                    m_cmdOptions.m_bSrcANSI7 = true;
                    break;


                } // switch 
            }
        }
    }


    class optionNameID
    {
      public:
          optionNameID() = default;
          optionNameID(std::wstring_view name, int id) : m_name(name), m_id(id) {}
          ~optionNameID() = default;

          // comparision operator for hash. Only on the name
          bool operator() (const optionNameID& lhs, const optionNameID& rhs) const
          {
              return lhs.m_name == rhs.m_name;
          }

          // hash operator for hash - only on the name field
          std::size_t operator()(const optionNameID& s)const noexcept
          {
              return std::hash<std::wstring_view>{}(s.m_name);
          }

          int getId() const { return m_id; }
          std::wstring_view getName() const { return m_name; }

    protected:
        std::wstring_view m_name;
        int               m_id = 0;
    };

    bool isValid() const { return m_bValid; }
    const std::wstring& getError() const { return m_error; }


    const CommandLineOptions& getOptions() const { return m_cmdOptions; }
          CommandLineOptions& getOptions()       { return m_cmdOptions; }



  protected:
      // return true if the path appears to be a valid filename
      // surrounding quotes must have already been removed
      // leading and trail space must have already been removed
      bool isValidWindowsPath(const std::wstring& input)
      {
          if (input.empty())
              return false;

          // Check for illegal characters
          if (input.find_first_of(L"<>:\"|?*") != std::wstring::npos)
              return false;

          // Check for reserved device names (case-insensitive)
          static const std::wregex reserved(L"(?:^|\\\\)(CON|PRN|AUX|NUL|COM[1-9]|LPT[1-9])(?:\\.|$)", std::regex::icase);
          if (std::regex_search(input, reserved))
              return false;

          return true;
      }

      std::wstring removeLeadingTrailingWhite(const std::wstring& input)
      {
          auto is_not_space = [](wchar_t ch) { return !std::iswspace(ch); };

          auto start = std::find_if(input.begin(), input.end(), is_not_space);
          auto end = std::find_if(input.rbegin(), input.rend(), is_not_space).base();

          if (start >= end)
              return L""; // all whitespace or empty

          return std::wstring(start, end);
      }

      // empty string returned if invalid
      std::wstring FilenameParameter(std::wstring_view str)
      {
          std::wstring filename = removeLeadingTrailingWhite( removeQuotes( str ) );
          if (!isValidWindowsPath(filename))
              filename.clear();
          return filename;
      }


      // remove leading/trailing quotes. look for \" to escape a quote and replace with a quote
      // this is used for other purposes than filenames which is why embedded quotes are allowed
    std::wstring removeQuotes(std::wstring_view str)
    {
        std::wstring rets(str);

        if ((rets.size() > 1) && (rets.front() == '\"') && (rets.back() == '\"'))
        {
            std::size_t spos = 1;
            std::size_t epos = rets.size() - 2;
            rets = removeQuotes(str.substr(spos, epos - spos));
        }

        // now replace any \" with "
        for (std::size_t spos; (spos = rets.find(L"\\\"")) != rets.npos; rets = rets.replace(spos, spos+1, L"\"") );
        return rets;
    }

    std::wstring removeQuotes(const std::wstring & str)
    {
        return removeQuotes(std::wstring_view(str));
    }

    std::deque<std::wstring> m_tokens;  // we are parsing wstring input data

    const std::unordered_set<optionNameID,optionNameID,optionNameID> m_options =  // predefined command line options
    {
        { L"-help",            0 },
        { L"-srcmap",          1 },
        { L"-outmap",          2 },
        { L"-script",          3 },
        { L"-mergeheight",     4 },
        { L"-mergecrystal",    5 },
        { L"-mergeore",        6 },
        { L"-mergetile",       7 },
        { L"-offsetrow",       8 },
        { L"-offsetcol",       9 },
        { L"-overwrite",      10 },
        { L"-resizerow",      11 },
        { L"-resizecol",      12 },
        { L"-copysrc",        13 },
        { L"-deftile",        14 },
        { L"-defheight",      15 },
        { L"-defcrystal",     16 },
        { L"-defore",         17 },
        { L"-mapname",        18 },
        { L"-creator",        19 },
        { L"-fix",            20 },
        { L"-sincdirs",       21 },
        { L"-sfixspace",      22 },
        { L"-snocomment",     23 },
        { L"-sdefine",        24 },
        { L"-sdatefmt",       25 },
        { L"-mergerect",      26 },
        { L"-flattenabove",   27 },
        { L"-flattenbelow",   28 },
        { L"-flattenbetween", 29 },
        { L"-borderheight",   30 },
        { L"-unicode",        31 },     // output format is UTF16LE
        { L"-bom",            32 },     // request BOM market
        { L"-srcansi",        33 },     // assume ansi code page input if 8 byte without BOM
        { L"-utf16le",        31 },     // output format is UTF16LE
        { L"-utf16be",        34 },     // output format is UTF16BE
        { L"-utf32le",        35 },     // output format is UTF32LE
        { L"-utf32be",        36 },     // output format is UTF32BE
        { L"-utf8",           37 },     // output format is UTF8
        { L"-soptnames",      38 },
        { L"-soptblank",      39 },
        { L"-briefing",       40 },     // filename contents will replace briefing section
        { L"-success",        41 },     // filename contents will replace briefing success section
        { L"-backup",         42 },     // backup output file name first.
        { L"-nobom",          43 },     // do not output UTF BOM
        { L"-srcutf8",        44 },     // 8 bit source no BOM is considered UTF8
        { L"-ansi",           45 },     // output format is 8 bit windows code page
        { L"-srcansi7",       46 }      // warn if any input is non-ANSI 7 bit



    };

    std::wstring_view getStringParm(std::size_t i)
    {
        if (i < m_tokens.size())
            return m_tokens[i];

        m_bValid = false;
        m_error = L"missing string for option: ";
        m_error += m_tokens[i - 1];
        return std::wstring_view();
    }

    int getIntParm(std::size_t i, bool bAllowNeg, std::wstring_view option)
    {
        if (i < m_tokens.size())
            return getInteger(m_tokens[i], bAllowNeg, option);
        m_bValid = false;
        m_error = L"missing integer for option: ";
        m_error += option;
        return 0;
    }

    int getInteger(std::wstring_view wstr, bool bAllowNeg, std::wstring_view option)
    {
        std::string str = Unicode::wstring_to_utf8(wstr);
        if (MMUtil::isInteger(str, bAllowNeg))
            return MMUtil::stoi(str);
        m_bValid = false;
        m_error = L"Invalid integer value: ";
        m_error += wstr;
        m_error += L" for option: ";
        m_error += option;
        return 0;
    }

    CommandLineOptions   m_cmdOptions;
    std::wstring         m_error;           // using wstring since it will be displayed
    bool                 m_bValid = false;
};



// if null string returned, we already displayed the error message
std::filesystem::path getTempFile()
{
    std::filesystem::path   retS;

    std::error_code ec;
    std::filesystem::path tempPath = std::filesystem::temp_directory_path( ec );

    wchar_t apiTFile[MAX_PATH + 16] = { 0 };
    if (!tempPath.empty())
    {
        UINT retval = GetTempFileNameW(tempPath.wstring().c_str(), L"TDU", 0, apiTFile);    // will create a unique empty file starting with TDU
        if (retval > 0 && apiTFile[0])
        {
            retS = apiTFile; // API returns full path
        }
    }
    if (retS.empty())
    {
        wprintf(L"  ERROR unable to obtain a temporary file - TMP or TEMP environment vars may be invalid\n");
        wprintf(L"  Temp Path: %s\n", tempPath.wstring().c_str());
    }
    return retS;
}


class tempFileCleanup
{
  public:
    tempFileCleanup() = default;
    tempFileCleanup(const std::filesystem::path& str)
    {
        m_path = str;
    }
    ~tempFileCleanup()
    {
        std::filesystem::remove(m_path);  // cleanup temp file.
    }

    const std::filesystem::path & get() const { return m_path; }

protected:
    std::filesystem::path m_path;
};



void help()
{
    //               10|       20|       30|       40|       50|       60|       70|       80| 
    //        12345678901234567890123456789012345678901234567890123456789012345678901234567890
    wprintf(L"\n");
    wprintf(L"  Usage:  <option> <parameter>\n");
    wprintf(L"    options begin with - or -- or / and end with a space\n");
    wprintf(L"    parameters are until the next space unless it is double quoted.\n");
    wprintf(L"    strings may have a \\\" in them to embed a quote.\n");
    wprintf(L"    Option:\n");
    wprintf(L"      -ansi           Output .dat is windows current code page\n");
    wprintf(L"      -backup         Output filename will be backed up prior to overwritting\n");
    wprintf(L"      -bom            output .dat has BOM Byte Order Marker (default)\n");
    wprintf(L"      -borderheight   force height borders to this value\n");
    wprintf(L"      -briefing       filename contents will replace briefing text\n");
    wprintf(L"      -copysrc        outmap is recreated from srcmap, implies -overwrite\n");
    wprintf(L"      -creator        creator: value saved in outmap info section\n");
    wprintf(L"      -deftile        value for invalid tiles or resize, default 1\n");
    wprintf(L"      -defcrystal     value for invalid crystals or resize, default 0\n");
    wprintf(L"      -defheight      value for invalid heights or resize, default 0\n");
    wprintf(L"      -defore         value for invalid ore or resize, default 0\n");
    wprintf(L"      -flattenabove   height,newheight. Heights > height set to newheight\n");
    wprintf(L"      -flattenbelow   height,newheight. Heights < height set to newheight\n");
    wprintf(L"      -flattenbetween low,high,value. low <= Heights <= high set to value\n");
    wprintf(L"      -fix            fix invalid/missing tile, height, crystal, ore values\n");
    wprintf(L"      -help           display this help\n");
    wprintf(L"      -mapname        levelname: value saved in outmap info section\n");
    wprintf(L"      -mergecrystal   merge crystals values from srcmap into outmap\n");
    wprintf(L"      -mergeheight    merge height values from srcmap into outmap\n");
    wprintf(L"      -mergerect      startrow,startcol,endrow,endcol for merge\n");
    wprintf(L"      -mergetile      merge tile values from srcmap into outmap\n");
    wprintf(L"      -mergeore       merge ore values from srcmap into outmap\n");
    wprintf(L"      -nobom          output .dat will not have BOM\n");
    wprintf(L"      -outmap         file name of a destination .DAT\n");
    wprintf(L"      -offsetcol      add col offset when merging/copying srcmap into outmap\n");
    wprintf(L"      -offsetrow      add row offset when merging/copying srcmap into outmap\n");
    wprintf(L"      -overwrite      allow changing existing outmap\n");
    wprintf(L"      -resizecol      resize outmap cols for tiles,height,resources\n");
    wprintf(L"      -resizerow      resize outmap rows for tiles,height,resources\n");
    wprintf(L"      -script         filename of script file to replace outmap's script\n");
    wprintf(L"      -sdatefmt       format for TyabScript{Inc}Date, default \"y.m.d\"\n");
    wprintf(L"      -sdefine        name=value   define script subsitution\n");
    wprintf(L"      -sfixspace      automatically remove spaces where not allowed in scripts\n");
    wprintf(L"      -sincdirs       ; separated list of paths to search for script includes\n");
    wprintf(L"      -snocomment     remove all comments in script except #.\n");
    wprintf(L"      -soptnames      Optimize script variable and event chain names\n");
    wprintf(L"      -soptblank      Remove script blank lines\n");
    wprintf(L"      -srcansi        8 bit input files without BOM assume ANSI\n");
    wprintf(L"      -srcmap         file name of a source map\n");
    wprintf(L"      -srcutf8        8 bit input files without BOM assume UTF8\n");
    wprintf(L"      -success        filename contents will replace briefingsuccess text\n");
    wprintf(L"      -unicode        output .dat is UTF16LE format\n");
    wprintf(L"      -utf8           output .dat is UTF8 format (default)\n");
    wprintf(L"      -utf16BE        output .dat is UTF16BE format\n");
    wprintf(L"      -utf16LE        output .dat is UTF16LE format\n");
    wprintf(L"      -utf32BE        output .dat is UTF32EE format\n");
    wprintf(L"      -utf32LE        output .dat is UTF32LE format\n");
    wprintf(L"\n");
    wprintf(L" see TyabMMDatUtil.pdf for more details and examples\n");
}


void showHelpOption()
{
    wprintf(L" For available options, use -help\n");
}

// given an input full path, return a backup filename
std::filesystem::path createCustomPath(const std::filesystem::path& filename, const std::wstring& userSuffix, size_t sequence)
{
    // Get current date
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm localTime;
    localtime_s(&localTime,&t);

    // Format date as YYYY_MM_DD
    std::wstring dateStr = std::format(L"{:04}{:02}{:02}", 
        localTime.tm_year + 1900, 
        localTime.tm_mon + 1, 
        localTime.tm_mday);

    // Build new filename
    std::filesystem::path newFilename = filename.stem();    // get the filename without extension and no path
    newFilename += L"_";
    newFilename += userSuffix;
    newFilename += L"_";
    newFilename += dateStr;
    newFilename += L"_" + std::to_wstring(sequence);
    newFilename += filename.extension();

    // Combine with original directory
    return filename.parent_path() / newFilename;
}

bool BackupFile(std::filesystem::path const& filename)
{
    std::error_code ec;
    if (std::filesystem::exists(filename, ec))
    {
        // need to find a unique file name:
        std::filesystem::path backup;
        for (size_t sequence = 1;; sequence++)
        {
            backup = createCustomPath( filename, L"Backup", sequence);
            if (!std::filesystem::exists(backup))
                break;
        }
        // backup is the unique backup file. Copy to it.
        BOOL retVal = CopyFileW(filename.wstring().c_str(), backup.wstring().c_str(), 0);
        if (!retVal)
        {
            wprintf(L"Error: %d creating backup file\n", GetLastError() );
            wprintf(L"   Trying to backup: %s\n", filename.wstring().c_str());
            wprintf(L"   To unique backup: %s\n", backup.wstring().c_str());
            wprintf(L"Aborted. Output file not modified\n");
            return false;
        }
        wprintf(L"  Unique Backup created: %s\n", backup.wstring().c_str());

    }
    else
    {
        wprintf(L"  Destination map does not exist. No backup needed\n");
    }
    return true;
}

// --- main - program entry
int wmain(int , wchar_t ** )   // ignore all passed in parameters
{
    header();
    
    std::wstring commandLine = GetCommandLineW();

    CommandLineParser cmdParser;
    cmdParser.Tokenize(commandLine);
    if (!cmdParser.isValid())
    {
        wprintf(L" ERROR: %s\n",cmdParser.getError().c_str());
        showHelpOption();
        return 1;
    }
    cmdParser.buildOptions();
    if (!cmdParser.isValid())
    {
        wprintf(L" ERROR: %s\n", cmdParser.getError().c_str());
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
        if (   cmdParser.getOptions().m_bMergeCrystal
            || cmdParser.getOptions().m_bMergeHeight
            || cmdParser.getOptions().m_bMergeOre
            || cmdParser.getOptions().m_bMergeTiles
            || cmdParser.getOptions().m_bCopySrc
            || cmdParser.getOptions().m_bMergeRect
            || cmdParser.getOptions().m_nOffsetCol
            || cmdParser.getOptions().m_nOffsetRow
            || cmdParser.getOptions().m_outmap.empty()     // both srcmap and outmap cannot be empty
            )
        {
            wprintf(L" ERROR: options require a srcmap\n");
            showHelpOption();
            return 1;
        }
    }
    else
    {
        std::size_t npos = MMUtil::toLower(cmdParser.getOptions().m_srcmap.extension().wstring()).rfind(L".dat");
        if (npos == std::wstring::npos)
        {
            wprintf(L" ERROR: Source map: %s must have a .dat extension\n", cmdParser.getOptions().m_srcmap.c_str());
            showHelpOption();
            return 1;
        }
    }
    // check for incompatible values
    if (cmdParser.getOptions().m_bReadANSI && cmdParser.getOptions().m_bReadUTF8)
    {
        wprintf(L" ERROR: input cannot have both -srcansi and -srcutf8");
        return 1;
    }
    cmdParser.getOptions().m_bReadANSI = !cmdParser.getOptions().m_bReadUTF8;  // set default 8 bit no BOM to be either ANSI (default) or UTF8

    if (cmdParser.getOptions().m_bBOM && cmdParser.getOptions().m_bNoBOM)
    {
        wprintf(L" ERROR: output cannot have both -bom and -nobom");
        return 1;
    }
    if (cmdParser.getOptions().m_bBOM && cmdParser.getOptions().m_bANSI)
    {
        wprintf(L" ERROR: output cannot have both -bom and -ansi");
        return 1;
    }
    if (cmdParser.getOptions().m_bANSI)
        cmdParser.getOptions().m_bBOM = false;
    else
        cmdParser.getOptions().m_bBOM = !cmdParser.getOptions().m_bNoBOM;  // set BOM (default) or no bom.

    if ((int)cmdParser.getOptions().m_bANSI + (int)cmdParser.getOptions().m_bUTF8 + (int)cmdParser.getOptions().m_bUTF16 + (int)cmdParser.getOptions().m_bUTF32 > 1)
    {
        wprintf(L" ERROR: only one format allowed. One of -ansi, -utf8, -utf16, -utf32");
        return 1;
    }

    if (!cmdParser.getOptions().m_outmap.empty())
    {
        std::size_t npos = MMUtil::toLower(cmdParser.getOptions().m_outmap.extension().wstring()).rfind(L".dat");
        if (npos == std::string::npos)
        {
            wprintf(L" ERROR: outmap: %s must have a .dat extension\n", cmdParser.getOptions().m_outmap.wstring().c_str());
            showHelpOption();
            return 1;
        }
    }
    else
    {
        if (cmdParser.getOptions().m_bANSI || cmdParser.getOptions().m_bUTF8 || cmdParser.getOptions().m_bUTF16 || cmdParser.getOptions().m_bUTF32 || cmdParser.getOptions().m_bBOM || cmdParser.getOptions().m_bNoBOM  )
        {
            wprintf(L" ERROR: output format and BOM options require -outmap\n");
            showHelpOption();
            return 1;
        }
    }

    // using eraseoutmap with merge options makes no sense
    if (cmdParser.getOptions().m_bCopySrc)
    {
        if (cmdParser.getOptions().m_bMergeCrystal ||
            cmdParser.getOptions().m_bMergeHeight ||
            cmdParser.getOptions().m_bMergeOre ||
            cmdParser.getOptions().m_bMergeTiles
            )
        {
            wprintf(L" ERROR: merging options have no meaning when using -copysrc\n");
            return 1;
        }
    }

    MMMap srcMap;
    MMMap outMap;


    // get srcmap if we want one
    if (!cmdParser.getOptions().m_srcmap.empty())
    {
        srcMap.setFileName(cmdParser.getOptions().m_srcmap);
        if (!srcMap.readMap(cmdParser.getOptions().m_bReadANSI))
        {
            srcMap.getErrors().printErrors();
            return 1;
        }

        wprintf((L"  Reading srcmap: " + cmdParser.getOptions().m_srcmap.wstring() + L"\n").c_str());
        FileIO::FileEncoding encoding;
        bool hasBOM;
        std::wstring const &encodingstr = Unicode::utf8_to_wstring(srcMap.getEncoding( encoding, hasBOM ));
        wprintf((L"  Encoding: " + encodingstr + L"\n").c_str());

        if (!srcMap.serializeInSections())
        {
            srcMap.getErrors().printErrors();
            return 1;
        }

        if (!srcMap.processSections(cmdParser.getOptions().m_bFix, cmdParser.getOptions().m_nDefTileID, cmdParser.getOptions().m_nDefHeight, cmdParser.getOptions().m_nDefCrystal, cmdParser.getOptions().m_nDefOre) )
        {
            srcMap.getErrors().printErrors();
            return 1;
        }

        wprintf(L"  Rows: %d  Columns: %d  Total lines: %d\n", srcMap.getHeight(), srcMap.getWidth(), srcMap.getNumLinesRead());

        if (cmdParser.getOptions().m_bMergeRect)
        {
            if (cmdParser.getOptions().m_erow >= srcMap.getHeight() || cmdParser.getOptions().m_ecol >= srcMap.getWidth())
            {
                wprintf(L" Error: -mergerect values are outside srcmap");
                return 1;
            }
        }
        else
        {      // if not using subregion, set region to entire source
            cmdParser.getOptions().m_srow = 0;
            cmdParser.getOptions().m_scol = 0;
            cmdParser.getOptions().m_erow = srcMap.getHeight() - 1;
            cmdParser.getOptions().m_ecol = srcMap.getWidth() - 1;
        }
        srcMap.getErrors().printWarnings();     // display any warnings
    }

    // if no output map, we still want to scan scripts
    if (cmdParser.getOptions().m_outmap.empty())
    {
        ScriptEngine scrEngine;     // script processing engine
        scrEngine.setSize(srcMap.getHeight(), srcMap.getWidth());
        scrEngine.setFileName(cmdParser.getOptions().m_srcscript);    // set script filename or empty
        scrEngine.addCmdDefines(cmdParser.getOptions(), srcMap.getHeight(), srcMap.getWidth(), srcMap.getMapName());
        scrEngine.loadScript(srcMap.getScriptLines(), cmdParser.getOptions().m_sincdirs, cmdParser.getOptions().m_bReadANSI);
        scrEngine.collectObjectiveVars(srcMap.getObjectiveLines());
        scrEngine.collectBlockNames(srcMap.getBlockLines());

        if (!scrEngine.getErrors().emptyErrors())
        {
            scrEngine.getErrors().printErrors();
            return 1;
        }

        scrEngine.processInputLines(
            cmdParser.getOptions().m_bScrFixSpace, false );

        if (!scrEngine.getErrors().emptyErrors())
        {
            scrEngine.getErrors().printErrors();
            return 1;
        }
        scrEngine.getErrors().printWarnings();

        ScriptEngine::ScriptNameStats stats = scrEngine.getScriptNameStats();
        wprintf(L"  Script Info. Variables: %d, Event Chains: %d\n",(int)stats.m_numvars, (int)stats.m_numchains);

    }

    // have an output map
    else
    {
        outMap.setFileName(cmdParser.getOptions().m_outmap);
        bool bExists = outMap.fexists();
        if (bExists)
        {
            if (!cmdParser.getOptions().m_bCopySrc && !cmdParser.getOptions().m_bOverwrite)
            {
                wprintf(L" ERROR: to overwrite existing outmap use -overwrite or -copysrc options\n");
                return 1;
            }
        }
        else // outmap file does not exist
        {
            if (cmdParser.getOptions().m_srcmap.empty() || !cmdParser.getOptions().m_bCopySrc)
            {
                wprintf(L" ERROR: non-existent outmap requires a srcmap and -copysrc option\n");
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
        if (cmdParser.getOptions().m_bCopySrc)
        {
            BOOL retval = CopyFileW(cmdParser.getOptions().m_srcmap.c_str(), tempOut.get().c_str(), false);      // copy src to temp - this ensures we have disk space
            if (retval == 0)
            {
                wprintf(L" ERROR %d unable to copy:\n \"%s\" to \"%s\"\n", GetLastError(), cmdParser.getOptions().m_srcmap.c_str(), tempOut.get().c_str());
                return 1;
            }
        }
        else if (bExists)
        {
            BOOL retval = CopyFileW(cmdParser.getOptions().m_outmap.c_str(), tempOut.get().c_str(), false);      // copy dest to temp - this ensures we have disk space
            if (retval == 0)
            {
                wprintf(L" ERROR %d unable to copy:\n \"%s\" to \"%s\"\n", GetLastError(), cmdParser.getOptions().m_srcmap.c_str(), tempOut.get().c_str());
                return 1;
            }
        }

        if (cmdParser.getOptions().m_bCopySrc)
        {
            outMap.copyFrom(srcMap, cmdParser.getOptions());     // outmap is now a copy of srcmap but using options like resize, offsets
        }
        else if (bExists)
        {
            outMap.setFileName(cmdParser.getOptions().m_outmap);
            if (!outMap.readMap(cmdParser.getOptions().m_bReadANSI))
            {
                outMap.getErrors().printErrors();
                return 1;
            }

            wprintf((L"  Reading outmap: " + cmdParser.getOptions().m_outmap.wstring() + L"\n").c_str());
            FileIO::FileEncoding encoding;
            bool hasBOM;
            std::wstring const &encodingstr = Unicode::utf8_to_wstring(outMap.getEncoding( encoding, hasBOM ));
            wprintf((L"  Encoding: " + encodingstr + L"\n").c_str());

            if (!outMap.serializeInSections())
            {
                outMap.getErrors().printErrors();
                return 1;
            }

            if (!outMap.processSections(cmdParser.getOptions().m_bFix, cmdParser.getOptions().m_nDefTileID, cmdParser.getOptions().m_nDefHeight, cmdParser.getOptions().m_nDefCrystal, cmdParser.getOptions().m_nDefOre) )
            {
                outMap.getErrors().printErrors();
                return 1;
            }

            if (!outMap.getErrors().emptyWarnings())
            {
                outMap.getErrors().printWarnings();
            }

            wprintf(L"  Rows: %d  Columns: %d  Total lines: %d\n", outMap.getHeight(), outMap.getWidth(), outMap.getNumLinesRead());
        }

        // check for briefing and briefing success replacements.
        outMap.clearErrorsWarnings();
        outMap.briefing(cmdParser.getOptions().m_briefing, cmdParser.getOptions().m_bReadANSI, cmdParser.getOptions().m_bSrcANSI7 );
        outMap.success ( cmdParser.getOptions().m_success, cmdParser.getOptions().m_bReadANSI, cmdParser.getOptions().m_bSrcANSI7 );
        outMap.getErrors().printErrors();
        outMap.getErrors().printWarnings();
        outMap.getErrors().printConsols();

        // see if resizing
        if (cmdParser.getOptions().m_nRowResize || cmdParser.getOptions().m_nColResize)
        {
            outMap.resize(cmdParser.getOptions().m_nRowResize, cmdParser.getOptions().m_nColResize, cmdParser.getOptions().m_nDefTileID, cmdParser.getOptions().m_nDefHeight, cmdParser.getOptions().m_nDefCrystal, cmdParser.getOptions().m_nDefOre);
            wprintf(L"  Resizing outmap to Rows: %d, Columns: %d\n", outMap.getHeight(), outMap.getWidth());
        }

        // see if using subregion
        if (cmdParser.getOptions().m_bMergeRect)
        {
            wprintf(L"  Using srcmap subregion: [%d,%d] to [%d,%d]\n", cmdParser.getOptions().m_srow, cmdParser.getOptions().m_scol, cmdParser.getOptions().m_erow, cmdParser.getOptions().m_ecol);
        }

        outMap.merging(srcMap, cmdParser.getOptions().m_srow, cmdParser.getOptions().m_scol, cmdParser.getOptions().m_erow, cmdParser.getOptions().m_ecol, cmdParser.getOptions().m_nOffsetRow, cmdParser.getOptions().m_nOffsetCol,
                       cmdParser.getOptions().m_bMergeTiles, cmdParser.getOptions().m_bMergeHeight, cmdParser.getOptions().m_bMergeCrystal, cmdParser.getOptions().m_bMergeOre );

        // display merging performed
        if (cmdParser.getOptions().m_bMergeTiles)
        {
            wprintf(L"  Merging srcmap tiles into outmap using offsets row: %d, columns: %d\n", cmdParser.getOptions().m_nOffsetRow, cmdParser.getOptions().m_nOffsetCol);
        }
        if (cmdParser.getOptions().m_bMergeHeight)
        {
            wprintf(L"  Merging srcmap Heights into outmap using offsets row: %d, columns: %d\n", cmdParser.getOptions().m_nOffsetRow, cmdParser.getOptions().m_nOffsetCol);
        }
        if (cmdParser.getOptions().m_bMergeCrystal)
        {
            wprintf(L"  Merging srcmap crystals into outmap using offsets row: %d, columns: %d\n", cmdParser.getOptions().m_nOffsetRow, cmdParser.getOptions().m_nOffsetCol);
        }
        if (cmdParser.getOptions().m_bMergeOre)
        {
            wprintf(L"  Merging srcmap ore into outmap using offsets row: %d, columns: %d\n", cmdParser.getOptions().m_nOffsetRow, cmdParser.getOptions().m_nOffsetCol);
        }

        // process any height changes
        if (cmdParser.getOptions().m_bFlattenHigh)
        {
            outMap.flattenHeightHigh(cmdParser.getOptions().m_flathighval, cmdParser.getOptions().m_flathighnewheight);
            wprintf(L"  Heights over %d set to %d\n", cmdParser.getOptions().m_flathighval, cmdParser.getOptions().m_flathighnewheight);
        }
        if (cmdParser.getOptions().m_bFlattenLow)
        {
            outMap.flattenHeightLow(cmdParser.getOptions().m_flatlowval, cmdParser.getOptions().m_flatlownewheight);
            wprintf(L"  Heights below %d set to %d\n", cmdParser.getOptions().m_flatlowval, cmdParser.getOptions().m_flatlownewheight);
        }
        if (cmdParser.getOptions().m_bFlattenBetween)
        {
            outMap.flattenHeightBetween(cmdParser.getOptions().m_flatBetweenLow, cmdParser.getOptions().m_flatBetweenHigh, cmdParser.getOptions().m_flatBetweenVal);
            wprintf(L"  Heights: [%d,%d] set to %d\n", cmdParser.getOptions().m_flatBetweenLow, cmdParser.getOptions().m_flatBetweenHigh, cmdParser.getOptions().m_flatBetweenVal);
        }

        if (cmdParser.getOptions().m_bBorderHeight)
        {
            outMap.borderHeight(cmdParser.getOptions().m_BorderHeight);
            wprintf(L"  border heights set to: %d\n", cmdParser.getOptions().m_BorderHeight);
        }

        if (!cmdParser.getOptions().m_mapName.empty())
        {
            outMap.setMapName(cmdParser.getOptions().m_mapName);
            wprintf(L"  Setting levelname to: %s\n", Unicode::utf8_to_wstring(cmdParser.getOptions().m_mapName).c_str());
        }

        if (!cmdParser.getOptions().m_creator.empty())
        {
            outMap.setMapCreator(cmdParser.getOptions().m_creator);
            wprintf(L"  Setting creator to: %s\n", Unicode::utf8_to_wstring(cmdParser.getOptions().m_creator).c_str());
        }

        ScriptEngine scrEngine;     // script processing engine
        scrEngine.setSize( outMap.getHeight(), outMap.getWidth() );
        scrEngine.setFileName( cmdParser.getOptions().m_srcscript );    // set script filename or empty
        scrEngine.addCmdDefines(cmdParser.getOptions(), outMap.getHeight(), outMap.getWidth(), outMap.getMapName() );
        scrEngine.loadScript( outMap.getScriptLines(), cmdParser.getOptions().m_sincdirs, cmdParser.getOptions().m_bReadANSI );
        scrEngine.collectObjectiveVars( outMap.getObjectiveLines() );
        scrEngine.collectBlockNames( outMap.getBlockLines() );

        if (!scrEngine.getErrors().emptyErrors())
        {
            scrEngine.getErrors().printErrors();
            return 1;
        }

        scrEngine.processInputLines(
            cmdParser.getOptions().m_bScrFixSpace,
            cmdParser.getOptions().m_bOptimizeNames
        );

        if (!scrEngine.getErrors().emptyErrors())
        {
            scrEngine.getErrors().printErrors();
            return 1;
        }

        ScriptEngine::ScriptNameStats stats = scrEngine.getScriptNameStats();
        wprintf(L"  Script Info. Variables: %d, Event Chains: %d\n",(int)stats.m_numvars, (int)stats.m_numchains);

        int outscriptlen = outMap.setScriptLines(scrEngine.buildOutputLines(
            cmdParser.getOptions().m_bScrNoComments,
            cmdParser.getOptions().m_bOptimizeNames,
            cmdParser.getOptions().m_bOptimizeBlank
        ));

        if (!scrEngine.getErrors().emptyErrors())
        {
            scrEngine.getErrors().printErrors();
            return 1;
        }
        scrEngine.getErrors().printWarnings();

        wprintf(L"  Replacing outmap script. %d lines total\n", outscriptlen);

        wprintf(L"  Writing temporary outmap: \"%s\"",tempOut.get().wstring().c_str());
        // all outmap modifications done, save to temp file, then copy to real file
        outMap.serializeOutSections();
        outMap.setFileName( tempOut.get() );

        FileIO::FileEncoding encoding = FileIO::FileEncoding::ANSI;     // default to ANSI output
        if (cmdParser.getOptions().m_bANSI)
            encoding = FileIO::FileEncoding::ANSI;     // default to ANSI output
        else if (cmdParser.getOptions().m_bUTF16)
            encoding = cmdParser.getOptions().m_bBigEndian ? FileIO::FileEncoding::UTF16BE : FileIO::FileEncoding::UTF16LE;
        else if (cmdParser.getOptions().m_bUTF32)
            encoding = cmdParser.getOptions().m_bBigEndian ? FileIO::FileEncoding::UTF32BE : FileIO::FileEncoding::UTF32LE;
        else if (cmdParser.getOptions().m_bUTF8 || cmdParser.getOptions().m_bBOM)
            encoding = FileIO::FileEncoding::UTF8;

        std::wstring const &encodingstr = Unicode::utf8_to_wstring(FileIO::getEncodingStr(encoding,cmdParser.getOptions().m_bBOM));
        wprintf((L"  Encoding: " + encodingstr + L"\n").c_str());

        if (!outMap.writeMap(encoding, cmdParser.getOptions().m_bBOM))
        {
            outMap.getErrors().printErrors();
            return 1;
        }
        if (!outMap.getErrors().emptyWarnings())
            outMap.getErrors().printWarnings();

        wprintf(L"  %d lines written for map file\n",outMap.getNumLinesWrite());

        // temp file has completed. Now copy to real destination
        // First check to see if -backup is set, if so we need to make a backup first
        if (cmdParser.getOptions().m_backup)
        {
            bool bSuccess = BackupFile(cmdParser.getOptions().m_outmap );
            if (!bSuccess)
                return 1;
        }
        wprintf(L"  Copying \"%s\" to \"%s\"\n",tempOut.get().wstring().c_str(),cmdParser.getOptions().m_outmap.wstring().c_str());
        BOOL retVal = CopyFileW(tempOut.get().c_str(), cmdParser.getOptions().m_outmap.c_str(), 0);
        if (retVal == 0)
        {
            wprintf(L" ERROR %d: unable to copy \"%s\" to \"%s\"", GetLastError(), tempOut.get().wstring().c_str(), cmdParser.getOptions().m_outmap.wstring().c_str());
            return 1;
        }
        wprintf(L"  \"%s\" written. All done.\n", cmdParser.getOptions().m_outmap.c_str());

    }

    wprintf(L"\n");
    return 0;
}

