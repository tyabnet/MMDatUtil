// Copyright (c) 2022 by Tyab. All rights reserved
//
#include <stdio.h>
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_set>
#include <filesystem>
#include "MMDatUtil.h"
#include "MMScript.h"


void header()
{
    printf("Tyab's Manic Miners .DAT utility v%s\n", __DATE__);
}


bool fExists(const std::string& FileName)
{
    std::error_code ec;
    return std::filesystem::exists(std::filesystem::path(FileName), ec);
}

// utility that will search a list of paths and ; separated paths and an array of already found paths for given file
// file may have a relative path already or may be just a name.
std::string searchPaths(const std::string& filename, const std::string& incDirs, const std::vector<std::filesystem::path>& paths)
{
    std::error_code ec;
    const std::filesystem::path fileName = filename;
    if (std::filesystem::exists(fileName, ec))
        return filename;

    std::filesystem::path fileNameOnly = fileName.filename();           // get just the filename
    std::filesystem::path filePathOnly = fileName;
    filePathOnly.remove_filename();    // get just the path

    std::filesystem::path temp;

    // first search the paths already found
    for (std::size_t i = 0; i < paths.size(); i++)
    {
        temp = paths[i] / fileNameOnly;
        if (std::filesystem::exists(temp, ec))
            return temp.string();
        if (!filePathOnly.empty())
        {
            temp = paths[i] / fileName;
            if (std::filesystem::exists(temp, ec))
                return temp.string();
        }
    }

    // if that fails, now we try each of the paths in incDirs
    std::size_t spos = 0;
    std::size_t epos = 0;
    std::string_view dirs = incDirs;
    for (; spos < dirs.size(); )
    {
        epos = dirs.find(';', spos);
        std::string_view sub;
        if (epos != std::string_view::npos)
            sub = dirs.substr(spos, epos - spos);
        else
            sub = dirs.substr(spos);
        if (!sub.empty())
        {
            temp = sub / fileNameOnly;
            if (std::filesystem::exists(temp, ec))
                return temp.string();
            if (!filePathOnly.empty())
            {
                temp = sub / fileName;
                if (std::filesystem::exists(temp, ec))
                    return temp.string();
            }
        }
        if (epos == std::string_view::npos)
            break;
        spos = epos + 1;
    }
    return std::string();   // unable to find
}



// returns string that has the short form of the git commit for the given path.
// nullstr returned if any error - no git, file not under git control, etc.
std::string getGitCommit(std::string_view path)
{
    class saveRestoreCWD
    {
    public:
        saveRestoreCWD()
        {
            std::error_code ec;
            m_cwd = std::filesystem::current_path(ec);    // get current working directory
        }
        ~saveRestoreCWD()
        {
            std::error_code ec;
            std::filesystem::current_path(m_cwd, ec);   // restore current working directory
        }
    protected:
        std::filesystem::path m_cwd;
    };

    std::string retS;
    saveRestoreCWD cwd;     // auto save and restore the current working directory

    std::error_code ec;

    std::filesystem::path fpath = path;             // get the full filename path
    std::filesystem::path fname = fpath.filename(); // get just the filename
    fpath.remove_filename();                        // just the path

    std::filesystem::current_path(fpath, ec); // change current dir so git will be in the correct directory

    FILE* fp = _popen(("git log --no-color -1 --abbrev-commit " + fname.string()).c_str(), "r");
    if (fp)
    {
        char buffer[128] = { 0 };
        fgets(buffer, sizeof(buffer) - 1, fp);      // first string will be commit shorthash
        _pclose(fp);
        retS = buffer;
        std::size_t pos = retS.find("commit ");
        if (pos == 0)   // it found it
        {
            pos += 7;   // should be start of commit
            std::size_t epos = pos;
            for (; epos < retS.size(); epos++)
            {
                int ch = tolower(retS[epos]);
                if ((ch >= '0') && (ch <= '9'))     // make sure in range
                    continue;
                if ((ch >= 'a') && (ch <= 'f'))     // make sure in range
                    continue;
                break;
            }
            if ((pos < retS.size()) && (epos >= pos + 4)) // min short hash is 4 chars
            {
                retS = retS.substr(pos, epos - pos);  // short hash
            }
        }
    }

    return retS;
}

// returns string specified by format for the last modified time of the given path
// null string if any error
// format chars are Y is year, M is month, D is day, H is hour, N is minute, S is second. G is git commit support
// years is 4 digits always. month, day, hour, minute, second are 2 digit 0 filled
// git commit is the short form as determined by git
// all other chars are copied as is to output preserving case.
std::string getDateStr(std::string_view path, std::string_view format)
{
    std::string sdate;
    std::filesystem::path fp = path;
    std::error_code ec;
    std::filesystem::file_time_type lwt = std::filesystem::last_write_time(path, ec);
    if (ec.value() == 0)
    {
        time_t tfile = std::chrono::system_clock::to_time_t(std::chrono::clock_cast<std::chrono::system_clock>(lwt));
        struct tm tlf;
        localtime_s(&tlf, &tfile);
        char buffer[128] = { 0 };
        for (std::size_t pos = 0; pos < format.size(); pos++)
        {
            buffer[0] = 0;
            switch (tolower(format[pos]))
            {
            case 'y':   // year
                sprintf_s(buffer, "%04d", tlf.tm_year + 1900);
                sdate += buffer;
                break;

            case 'm':   // month
                sprintf_s(buffer, "%02d", tlf.tm_mon);
                sdate += buffer;
                break;

            case 'd':   // day
                sprintf_s(buffer, "%02d", tlf.tm_mday);
                sdate += buffer;
                break;

            case 'h':   // hour
                sprintf_s(buffer, "%02d", tlf.tm_hour);
                sdate += buffer;
                break;

            case 'n':   // minute
                sprintf_s(buffer, "%02d", tlf.tm_min);
                sdate += buffer;
                break;

            case 's':   // sec
                sprintf_s(buffer, "%02d", tlf.tm_sec);
                sdate += buffer;
                break;

            case 'g':   // git commit
                sdate += getGitCommit(path);
                break;

            default:
                sdate += format[pos];
                break;
            }
        }
    }
    return sdate;
}


bool isEmptyStr(std::string_view str)
{
    // skip leading space
    std::size_t pos;
    std::size_t len = str.length();
    for (pos = 0; (pos < len) && (str[pos] <= ' '); pos++); // skip leading space
    return (pos == len) || (str[pos] == 0);
}

// must return a new string
std::string toLower(std::string str)
{
    std::string ret;
    std::size_t len = str.length();
    ret.reserve(len + 1);

    for (int i = 0; i < (int)len; i++)
    {
        ret += (char)tolower(str[i]);
    }
    return ret;
}

// must return a new string
std::string toLower(std::string_view str)
{
    std::string ret;
    std::size_t len = str.length();
    ret.reserve(len + 1);

    for (int i = 0; i < (int)len; i++)
    {
        ret += (char)tolower(str[i]);
    }
    return ret;
}

std::string_view removeLeadingWhite(std::string_view str)
{
    std::size_t pos;
    std::size_t len = str.length();
    for (pos = 0; (pos < len) && (str[pos] <= ' '); pos++); // skip leading space
    return str.substr(pos);
}

// first char must be alpha, followed by only alpha or digits. No leading/trailing spaces allowed
// this is not for script, but for keys in other sections
bool isValidVarName(std::string_view name)
{
    int len = (int)name.length();
    bool valid = (len > 0) && isalpha(name[0]);
    for (int i = 1; i < len && valid; i++)
    {
        valid = (isalpha(name[i]) || isdigit(name[i]));
    }
    return valid;
}

bool isInteger(std::string_view val, bool bAllowNeg)
{
    bool valid = true;
    std::size_t i = 0;
    if (val[0] == '-')
    {
        if (!bAllowNeg)
            return false;
        i++;
    }
    std::size_t len = val.length();
    for (; i < len && valid; i++)
    {
        valid = isdigit(val[i]);
    }
    return valid;
}

// standard does not provide a stoi for string_view, so here is one
int stoi(std::string_view val)
{
    int sum = 0;
    if (val.length() > 0)
    {
        int sign = 1;
        std::size_t i = 0;
        if (val[0] == '-')
        {
            sign = -1;
            i++;
        }
        for (; (i < val.length()) && isdigit(val[i]); i++)
        {
            sum = (sum * 10) + val[i] - '0';
        }
        sum *= sign;
    }
    return sum;
}

// returns string
std::string readLineUTF8(FILE* fp)  // read from a multibyte source, return std::string
{
    std::string line;
    line.reserve(8192);	// so += is fast
    for (; !feof(fp);)
    {
        int ch = fgetc(fp);
        if (ch == EOF)
            break;
        ch &= 0xff;
        if (ch == 0)
            break;
        if (ch == '\r')         // ignore carriage returns, only care about line feeds.
            continue;
        if (ch == '\n')
            break;
        if (ch == 26)  // CTRL-Z
            break;
        line += (uint8_t)ch;
    }
    line.shrink_to_fit();
    return line;
}


// note that we are passing in strings - not string_views - conversion needs null term string
std::wstring utf8_to_wstring(const std::string& str)
{
    std::wstring ret;
    if (!str.empty())
    {
#if (_MSVC_LANG >= 201703L) || (__cplusplus >= 201703L)  // c++17 and greater no longer support, so we just use windows
        std::size_t size = str.length() * 10;    // way more than worst case avoids having to first query.
        wchar_t* utf16 = (wchar_t*)calloc(size, 1);
        if (utf16)
        {
            MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, utf16, (int)((size / 2) - 1));
            ret = utf16;
            free(utf16);
        }
#else
        std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
        ret = myconv.from_bytes(str);
#endif
    }
    return ret;
}

// note that we are passing in strings - not string_view - conversion needs null term string
std::string wstring_to_utf8(const std::wstring& str)
{
    std::string ret;
    if (!str.empty())
    {
#if (_MSVC_LANG >= 201703L) || (__cplusplus >= 201703L)  // c++17 and greater no longer support, so we just use windows
        std::size_t size = str.length() * 8;    // way more than worst case avoids having to first query.
        char* utf8 = (char*)calloc(size, 1);
        if (utf8)
        {
            WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, utf8, (int)(size - 1), NULL, NULL);
            ret = utf8;
            free(utf8);
        }
#else
        std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
        ret = myconv.to_bytes(str);
#endif
    }
    return ret;
}

std::string readLineUTF16(FILE* fp, bool lbf)  // read from unicode save into utf8 in std::string.
{
    std::wstring wline;
    wline.reserve(8192);	// so += is fast
    for (; !feof(fp);)
    {
        int ch = fgetc(fp);
        if ((ch == EOF) || feof(fp))
            break;
        int ch1 = fgetc(fp);
        if (ch == EOF)
            break;
        ch &= 0xff;
        ch1 &= 0xff;
        if (lbf)
            ch |= ch1 << 8;
        else
            ch = (ch << 8) | ch1;
        if (ch == 0)
            break;
        if (ch == '\r')         // ignore carriage returns, only care about line feeds.
            continue;
        if (ch == 26)     // CTRL-Z
            break;
        if (ch == '\n')
            break;
        wline += (wchar_t)ch;
    }

    // now convert to UTF8
    return wstring_to_utf8(wline);
}

void getFileEncoding(FILE* fp, bool& bUTF8BOM, bool& bUTF16LE, bool& bUnicode)  // get encoding flags
{
    // maps can have UTF-8 encoding Byte Order Mark (BOM) which is 3 bytes or they can be unicode which has a 2 byte BOM
    if (feof(fp))
        return;   // nothing to read
    int ch1 = fgetc(fp);
    if (feof(fp))
        return;   // nothing to read
    int ch2 = fgetc(fp);
    if (feof(fp))
        return;   // nothing to read
    int ch3 = fgetc(fp);
    if (feof(fp))
        return;   // nothing to read

    bUTF8BOM = false;
    bUTF16LE = false;
    bUnicode = false;

    if ((ch1 == 0xff) && (ch2 == 0xfe))  // low byte first unicode
        bUTF16LE = bUnicode = true;
    if ((ch1 == 0xfe) && (ch2 == 0xff))  // high byte first unicode
        bUnicode = true;
    if (bUnicode)
    {
        fseek(fp, 0, 0);
        fseek(fp, 2, 0);
    }
    else  // check for UTF-8 mark
    {
        if ((ch1 == 0xEF) && (ch2 == 0xBB) && (ch3 == 0xBF))  // we have 3 byte BOM, so just ignore it.
            bUTF8BOM = true;
        else
            fseek(fp, 0, 0);  // none of the above, so reset back to beginning
    }
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
                        m_error = "missing name=value for option " + m_tokens[i - 1];
                        break;
                    }
                    std::size_t epos = m_tokens[i].find('=');
                    if ((epos == std::string::npos) || (epos < 1) || ((epos+1) >= m_tokens[i].size()))
                    {
                        m_bValid = false;
                        m_error = "missing name=value for option " + m_tokens[i - 1];
                        break;
                    }
                    std::string_view key = std::string_view(m_tokens[i]).substr(0, epos);
                    std::string_view value = std::string_view(m_tokens[i]).substr(epos+1);
                    if (key.empty() || value.empty())
                    {
                        m_bValid = false;
                        m_error = "missing name=value for option " + m_tokens[i - 1];
                        break;
                    }
                    if (value[0] == '"')    // value starts with a string, it needs to end with one
                    {
                        if (value[value.size()-1] != '"')
                        {
                            m_bValid = false;
                            m_error = "missing or invalid position of closing \" for option " + m_tokens[i - 1];
                            break;
                        }
                    }

                    m_cmdOptions.addKeyValue(key, value);
                    break;
                }
                case 25:  // sdatefmd  string parameter - we automatically add double quotes to it.
                    m_cmdOptions.m_datestr = '"' + std::string(getStringParm(++i)) + '"';
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
              return std::hash<std::string_view>{}(s.m_name);
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
        { "-sfixspace",    22 },
        { "-snocomment",   23 },
        { "-sdefine",      24 },
        { "-sdatefmt",     25 },
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
    printf("    strings may have a \\\" in them to embed a quote.\n");
    printf("    Option:\n");
    printf("      -help         display this help\n");
    printf("      -srcmap       file name of a source merge .DAT\n");
    printf("      -outmap       file name of a destination .DAT\n");
    printf("      -overwrite    allow changing existing outmap\n");
    printf("      -copysrc      outmap is recreated from srcmap, implies -overwrite\n");
    printf("      -mergeheight  merge height values from srcmap into outmap\n");
    printf("      -mergecrystal merge crystals values from srcmap into outmap\n");
    printf("      -mergeore     merge ore values from srcmap into outmap\n");
    printf("      -mergetile    merge tile values from srcmap into outmap\n");
    printf("      -offsetrow    add row offset when merging/copying srcmap into outmap\n");
    printf("      -offsetcol    add col offset when merging/copying srcmap into outmap\n");
    printf("      -resizerow    resize outmap rows for tiles,height,resources\n");
    printf("      -resizecol    resize outmap cols for tiles,height,resources\n");
    printf("      -deftile      value for invalid tiles or resize, default 1\n");
    printf("      -defheight    value for invalid heights or resize, default 0\n");
    printf("      -defcrystal   value for invalid crystals or resize, default 0\n");
    printf("      -defore       value for invalid ore or resize, default 0\n");
    printf("      -mapname      levelname: value saved in outmap info section\n");
    printf("      -creator      creator: value saved in outmap info section\n");
    printf("      -fix          fix invalid/missing tile, height, crystal, ore values\n");
    printf("      -script       filename of script file to replace outmap's script\n");
    printf("      -sincdirs     ; separated list of paths to search for script includes\n");
    printf("      -sfixspace    automatically remove spaces where not allowed in scripts\n");
    printf("      -snocomment   remove all comments in script except #.\n");
    printf("      -sdefine      name=value   define script subsitution\n");
    printf("      -sdatefmt     format for TyabScript{Inc}Date, default \"y.m.d\"");
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
    ScriptProcessor sfProcess;  // used to process scripts

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
        ScriptProcessor sp;
        sp.setScriptFile(srcMap.getScript());  // copies the script, does raw parsing
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
                printf("     line: %d: %s\n", scriptFile.getError().getWarnLines()[i].getLineNum(), std::string(scriptFile.getError().getWarnLines()[i].getLine()).c_str());
            }

            if (err)
            {
                for (int i = 0; i < scriptFile.getError().getErrors().size(); i++)
                {
                    printf("  Error: %s\n", scriptFile.getError().getErrors()[i].c_str());
                    if (scriptFile.getError().getErrLines()[i].getLineNum() > 0)
                        printf("    line: %d: %s\n", scriptFile.getError().getErrLines()[i].getLineNum(), std::string(scriptFile.getError().getErrLines()[i].getLine()).c_str());
                }
                return 1;
            }

            sfProcess.setScriptFile(scriptFile);  // copies the script, does raw parsing
            err = sfProcess.addCmdDefines(cmdParser.getOptions(), outMap);  // process any -sdefine options
            if (!err)
                err = sfProcess.ProcessScript(cmdParser.getOptions());

            for (int i = 0; i < sfProcess.getError().getWarnings().size(); i++)
            {
                printf("  Warning: %s\n", sfProcess.getError().getWarnings()[i].c_str());
                printf("     line: %d: %s\n", sfProcess.getError().getWarnLines()[i].getLineNum(), std::string(sfProcess.getError().getWarnLines()[i].getLine()).c_str());
            }

            if (err)
            {
                for (int i = 0; i < sfProcess.getError().getErrors().size(); i++)
                {
                    printf("  Error: %s\n", sfProcess.getError().getErrors()[i].c_str());
                    if (sfProcess.getError().getErrLines()[i].getLineNum() > 0)
                        printf("    line: %d: %s\n", sfProcess.getError().getErrLines()[i].getLineNum(), std::string(sfProcess.getError().getErrLines()[i].getLine()).c_str());
                }
                return 1;

            }


            // TODO check/display errors/warnings, etc.

            scriptFile = sfProcess.getScriptFile();

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

