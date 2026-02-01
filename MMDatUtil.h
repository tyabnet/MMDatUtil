// (c)2022 by Tyab
// Manic Miners .DAT file utilities
// Contains classes and methods to read/write entire .DAT files and break up into logical sections
//

#ifndef MMDatUtil_H
#define MMDatUtil_H

#include <cstdint>
#include <stdio.h>
#include <cwctype>
#include <string>
#include <string_view>
#include <unordered_set>
#include <unordered_map>
#include <array>
#include <deque>
#include <filesystem>
#include <chrono>
#include <regex>

#include <windows.h>

#include "mmunicode.h"

#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max

#if !defined(UNICODE) or !defined(_UNICODE)
#error define unicode in project settings to catch incorrect API usage
#endif


constexpr char getDateIso(const char *input, size_t index )
{
    const unsigned int compileYear = (input[7] - '0') * 1000 + (input[8] - '0') * 100 + (input[9] - '0') * 10 + (input[10] - '0');
    const unsigned int compileDay = (input[4] == ' ') ? (input[5] - '0') : (input[4] - '0') * 10 + (input[5] - '0');

    auto getcompileMonth = [](const char *input) -> unsigned int
    {
        auto monval = [](const char *month) -> unsigned int { return ((unsigned int)month[2] << 16) | ((unsigned int)month[1] << 8) | ((unsigned int)month[0]); };

        unsigned int month;
        switch (monval(input))
        {
            case monval("Jan"): { month = 1; break; }
            case monval("Feb"): { month = 2; break; }
            case monval("Mar"): { month = 3; break; }
            case monval("Apr"): { month = 4; break; }
            case monval("May"): { month = 5; break; }
            case monval("Jun"): { month = 6; break; }
            case monval("Jul"): { month = 7; break; }
            case monval("Aug"): { month = 8; break; }
            case monval("Sep"): { month = 9; break; }
            case monval("Oct"): { month = 10; break; }
            case monval("Nov"): { month = 11; break; }
            case monval("Dec"): { month = 12; break; }
            default: { month = 0; break; }
        };
        return month;
    };

    auto toch = []( unsigned int ch ) -> char { return (char)ch + '0'; };
    char output[11] =
    {
        toch(compileYear / 1000), toch((compileYear % 1000) / 100), toch((compileYear % 100) / 10), toch(compileYear % 10),
        '-',
        toch(getcompileMonth(input) / 10), toch(getcompileMonth(input) % 10),
        '-',
        toch(compileDay / 10), toch(compileDay % 10),
        0
    };

    return output[index];
    
};


// compile time, like __DATE__ but has in ISO format   yyyy-mm-dd
static constexpr char DATE_ISO[11] =
{
    getDateIso(__DATE__,0),     // y
    getDateIso(__DATE__,1),     // y
    getDateIso(__DATE__,2),     // y
    getDateIso(__DATE__,3),     // y
    getDateIso(__DATE__,4),     // -
    getDateIso(__DATE__,5),     // m
    getDateIso(__DATE__,6),     // m
    getDateIso(__DATE__,7),     // -
    getDateIso(__DATE__,8),     // d
    getDateIso(__DATE__,9),     // d
    getDateIso(__DATE__,10)     // 0
};


// MAP files are converted to UTF8 for processing.
// MAP Input format map be ANSI, UTF8, UTF16, UTF32, MSB/LSB formats supported, optional BOM formats supported
// output format by default is UTF8 but can be overriden to other formats
// command line is processed in unicode format
// all command line output is unicode format

namespace MMUtil
{
    // remove all double quotes. Used to insert macro value into an existing string
    static inline std::string removeAllDoubleQuotes(const std::string& str)
    {
        std::string rets;
        rets.reserve(str.size());
        for (std::size_t i = 0; i < str.size(); i++)
            if (str[i] != '\"')
                rets += str[i];
        return rets;
    }

    // remove leading blank chars from string view, return new view
    static inline std::string_view removeLeadingWhite(std::string_view str)
    {
        std::size_t pos;
        std::size_t len = str.length();
        for (pos = 0; (pos < len) && std::isblank((unsigned char)str[pos]); pos++); // skip leading space
        return str.substr(pos);
    }

    // remove training blank chars from string view, return new view
    static inline std::string_view removeTrailingWhite(std::string_view str)
    {
        std::size_t len = str.length();
        if (len)
        {
            for (std::size_t pos = len - 1;; pos--)
            {
                if (!std::isblank((unsigned char)str[pos]))
                {
                    return str.substr(0, pos + 1);
                }
                if (pos == 0)   // entire string is blank
                    break;
            }
        }
        return std::string_view();     // return empty
    }

    // remove both leading and trailing blank chars from string view, return new view
    static inline std::string_view removeLeadingAndTrailingWhite(std::string_view str)
    {
        return removeLeadingWhite(removeTrailingWhite(str));
    }

    // convert view to lower case returning a new string
    static inline std::string toLower(std::string_view str)
    {
        std::string ret;
        std::size_t len = str.length();
        ret.reserve(len + 1);

        for (int i = 0; i < (int)len; i++)
        {
            ret += (char)std::tolower((unsigned char)str[i]);
        }
        return ret;
    }

    // convert wide view to lower case, returning a new wide string. Used for command line processing
    static inline std::wstring toLower(std::wstring_view str)
    {
        std::wstring ret;
        std::size_t len = str.length();
        ret.reserve(len);

        for (int i = 0; i < (int)len; i++)
        {
            ret += (wchar_t)std::towlower(str[i]);
        }
        return ret;
    }

    //  return true if view is en empty string after removing all leading blanks
    static inline bool isEmptyStr(std::string_view str)
    {
        return removeLeadingWhite(str).empty();
    }

    // check that string starts with alpha/underbar and only contains alpha/digits/underbar
    static inline bool isAlphaNumericName(std::string_view str)
    {
        size_t len = str.length();
        if ((len > 0) && (std::isalpha((unsigned char)str[0]) || (str[0]=='_')))
        {
            for (std::size_t i = 1; i < len; i++)
            {
                if (!std::isalnum((unsigned char)str[i]) && (str[i] != '_'))
                    return false;
            }
            return true;
        }
        return false;
    }

    // standard does not provide a stoi for string_view, so here is one
    static inline int stoi(std::string_view val)
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
            for (; (i < val.length()) && std::isdigit((unsigned char)val[i]); i++)
            {
                sum = (sum * 10) + val[i] - '0';
            }
            sum *= sign;
        }
        return sum;
    }

    // return true if view is an integer number. must have at least a single digit with nothing but digits and an optional leading -sign.
    // leading/training blank space must have already been removed
    static inline bool isInteger(std::string_view val, bool bAllowNeg)
    {
        bool valid = false; // ensures we must have at least a single digit
        if (!val.empty())
        {
            std::size_t i = 0;
            if (val[0] == '-')
            {
                if (!bAllowNeg)
                    return false;
                i++;
            }
            std::size_t len = val.length();
            for (; i < len; i++)
            {
                valid = std::isdigit((unsigned char)val[i]);
                if (!valid)
                    break;
            }
        }
        return valid;
    }

    // return true if view is a float (or integer). must have as last a single digit with nothing but digits and an optional leading - sign and up to a single decimal point.
    // leading/training blank space must have already been removed
    static inline bool isFloat(std::string_view val, bool bAllowNeg)
    {
        bool valid = false; // start off false ensures we must have at least a single digit
        if (!val.empty())
        {
            std::size_t i = 0;
            if (val[0] == '-')
            {
                if (!bAllowNeg)
                    return false;
                i++;
            }
            bool bDecimalpoint = false;
            std::size_t len = val.length();
            for (; i < len; i++)
            {
                if (val[i] == '.')
                {
                    if (!bDecimalpoint)
                        bDecimalpoint = true;
                    else
                        return false;  // cant have more than 1 decimal point
                }
                else
                {
                    valid = std::isdigit((unsigned char)val[i]);
                    if (!valid)
                        break;
                }
            }
        }
        return valid;
    }

    static std::string to_string_hex(int32_t val)
    {
        std::string retval;
        do {
            char digit = (char) val & 0xf;    // get lower digit
            char ch = (digit < 10) ? '0'+digit : 'a'+digit-10;
            retval = ch + retval;
            val >>= 4;  // next higher hex digit
        }
        while (val);
        return retval;
    }

    // return 0 if no 8 bit char found, else return first 8 bit char found
    static char find8BitChar(std::string_view str)
    {
        for (char ch : str)
        {
            if ((unsigned char)ch > 0x7f)
                return ch;
        }
        return 0;
    }

    // manage a rectangular region, used for copy, merge and flatten operations
    class Region
    {
      public:
        Region() { clear(); }
        Region( int xlo, int ylo, int xhi, int yhi) : m_xlo(xlo), m_ylo(ylo), m_xhi(xhi), m_yhi(yhi) { checkfixEmpty(); }

        int xlo() const { return m_xlo; }
        int ylo() const { return m_ylo; }
        int xhi() const { return m_xhi; }
        int yhi() const { return m_yhi; }

        void clear()  // empty is max inverted values
        {
            m_xlo = m_ylo = std::numeric_limits<int>::max();
            m_xhi = m_yhi = std::numeric_limits<int>::lowest();
        } 
        bool empty() const { return (m_xhi < m_xlo); } // we only check one since we keep it valid all the time.
        void unionWith(const Region& other)
        {
            m_xlo = std::min(m_xlo, other.m_xlo);
            m_ylo = std::min(m_ylo, other.m_ylo);
            m_xhi = std::max(m_xhi, other.m_xhi);
            m_yhi = std::max(m_yhi, other.m_yhi);
        }

        void intersect(const Region& other)
        {
            m_xlo = std::max(m_xlo, other.m_xlo);
            m_ylo = std::max(m_ylo, other.m_ylo);
            m_xhi = std::min(m_xhi, other.m_xhi);
            m_yhi = std::min(m_yhi, other.m_yhi);
            checkfixEmpty();
        }

      protected:
        void checkfixEmpty()
        {
            if ((m_xhi < m_xlo) || (m_yhi < m_ylo))
                clear();
        }
          
        int m_xlo;
        int m_ylo;
        int m_xhi;
        int m_yhi;
    };

    std::string GetLastErrorAsString()
    {
        std::wstring wstring;
        DWORD errorCode = GetLastError();
        if (errorCode != 0)
        {
            LPWSTR messageBuffer = nullptr;

            size_t size = FormatMessageW(
                FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPWSTR)&messageBuffer, 0, NULL);

            wstring = std::wstring(messageBuffer, size);

            LocalFree(messageBuffer);
        }
        return Unicode::wstring_to_utf8( wstring );
    }


};  // namespace MMUtil


// options we pass
class CommandLineOptions
{
public:
    class keyValue
    {
    public:
        keyValue() = default;
        keyValue(std::string_view key, std::string_view value) : m_key(key), m_value(value) {}
        keyValue(const std::string & key, const std::string & value) : m_key(key), m_value(value) {}
        ~keyValue() = default;

        const std::string & key()   const { return m_key; }
        const std::string & value() const { return m_value; }

    protected:
        std::string m_key;
        std::string m_value;
    }; // class keyValue

    void addKeyValue(std::string_view key, std::string_view value)
    {
        m_defines.emplace_back(key, value);
    }
    void addKeyValue(const std::string & key, const std::string &value)
    {
        m_defines.emplace_back(key, value);
    }


    // all the options    
    std::filesystem::path             m_srcmap;              // input source map
    std::filesystem::path             m_outmap;              // output source map (can also be input)
    std::filesystem::path             m_srcscript;           // input script file to override script section
    std::string                       m_mapName;             // override map name in info section
    std::string                       m_creator;             // override curator name in info section
    std::deque<std::filesystem::path> m_sincdirs;            // list of paths to search for script includes
    std::string                       m_datestr = "\"y.m.d\""; // TyabScriptDate and TyabScriptIncDate format

    std::deque<keyValue> m_defines;     // collection of key=value pairs

    int          m_nOffsetRow  = 0;
    int          m_nOffsetCol  = 0;
    int          m_nRowResize  = 0;
    int          m_nColResize  = 0;
    int          m_nDefTileID  = 1;
    int          m_nDefHeight  = 0;
    int          m_nDefCrystal = 0;
    int          m_nDefOre     = 0;

    MMUtil::Region m_mergeRegion; // merge region
    MMUtil::Region m_flattenRegion; // flatten region

    int          m_flathighval       = 0;  // flatten high height to check
    int          m_flathighnewheight = 0;  // flatten high new height
    int          m_flatlowval        = 0;  // flatten low height to check
    int          m_flatlownewheight  = 0;  // flatten low new height
    int          m_flatBetweenLow    = 0;  // flatten between low height to check
    int          m_flatBetweenHigh   = 0;  // flatten between high height to check
    int          m_flatBetweenVal    = 0;  // flatten between new height
    int          m_BorderHeight      = 0;  // set all borders heights to this value

    bool         m_bMergeHeight   = false;
    bool         m_bMergeCrystal  = false;
    bool         m_bMergeOre      = false;
    bool         m_bMergeTiles    = false;
    bool         m_bOverwrite     = false;
    bool         m_bCopySrc   = false;
    bool         m_bHelp          = false;
    bool         m_bFix           = false;
    bool         m_bScrFixSpace   = false;  // automatic fix spaces where not allowed
    bool         m_bScrNoComments = false;  // remove all non #. comments
    bool         m_bMergeRect     = false;  // if true merge src is a subregion
    bool         m_bFlattenRect   = false;  // if true merge src is a subregion
    bool         m_bFlattenHigh   = false;
    bool         m_bFlattenLow    = false;
    bool         m_bFlattenBetween= false;
    bool         m_bBorderHeight  = false;
    bool         m_bUTF8          = false;  // if set UTF8 format (will be default if none set)
    bool         m_bUTF16         = false;  // if set UTF16 format
    bool         m_bUTF32         = false;  // if set output format is UTF32, either big or little endian
    bool         m_bBigEndian     = false;  // if set output format is big endian, default false is little endian.
    bool         m_bBOM           = false;  // if set, output will have Byte Order Marker
    bool         m_bNoBOM         = false;  // if set no BOM will be output.  NEW
    bool         m_bReadANSI      = false;  // if set, assume 8 byte input with no BOM is ANSI
    bool         m_bReadUTF8      = false;  // if set, assume 8 byte input with no BOM is UTF8 NEW
    bool         m_bOptimizeNames = false;  // if set optimize variable and event chain names
    bool         m_bOptimizeBlank = false;
    bool         m_backup         = false;
    bool         m_bANSI          = false;  // if set output format is windows code page.
    bool         m_bSrcANSI7      = false;  // if set warn if any input file has 8 bit characters
    std::filesystem::path m_briefing;  // contents of this file will be used for briefing message          (empty not used)
    std::filesystem::path m_success;   // contents of this file will be used for briefing success message  (empty not used)
    std::filesystem::path m_failure;   // contents of this file will be used for briefing failure message  (empty not used)
}; // class CommandLineOptions


// every line read in has the line and the line number and optional filename
// filename is only used for error reporting and must have been converted to utf8 prior
// complete strings are stored, no references avoiding ownership issues.
class InputLine
{
  public:
    InputLine() = default;
    InputLine(std::string const& line, int linenum) : m_string(line), m_linenum(linenum) {}
    InputLine(std::string const& line, int linenum, const std::string &filename) : m_string(line), m_linenum(linenum), m_filename(filename) {}
    ~InputLine() = default;

    std::string const &getLine()    const { return m_string; } 
    int                getLineNum() const { return m_linenum; }
    std::string const &getFileName()const { return m_filename; } 

    bool empty() const { return m_filename.empty() && m_string.empty(); }

    // change line to be this line. Used during macro substitution
    void setLine(std::string const & line)
    {
        m_string  = line;
    }

    // change/set line and linenum only
    void setLine(std::string const & line, int linenum)
    {
        m_string  = line;
        m_linenum = linenum;
    }
    
    // change line, linenum, and filename
    void setLine(const std::string & line, int linenum, const std::string & filename)
    {
        m_string  = line;
        m_linenum = linenum;
        m_filename = filename;
    }

protected:
    std::string      m_string;      // copy of input line
    std::string      m_filename;    // copy of optional filename
    int              m_linenum = 0; // line number for input line
};

using InputLinePtr = std::shared_ptr<InputLine>;

// stores errors and warnings and console message
// all strings are utf8.  Every error/warning also has a copy of its InputLine
// this is a collection class, can store unlimited errors/warnings.
// Other error/warnings can be appended with +=
// setError / setWarning will add a single error/warning
// printing of errors/warnings is in unicode to console.
class ErrorWarning
{
  public:

    ErrorWarning() = default;
    ErrorWarning(const InputLinePtr& line, const std::string & errMsg, const std::string &warnMsg, const std::string &consoleMsg )
    {
        setError(line, errMsg);
        setWarning(line, warnMsg);
        setConsole(line, consoleMsg);
    }
    ~ErrorWarning() = default;

    // add in everything in rhs to us
    const ErrorWarning &operator += (const ErrorWarning& rhs)
    {
        m_errlines.insert(m_errlines.end(), rhs.m_errlines.begin(), rhs.m_errlines.end());
        m_errors.insert(m_errors.end(), rhs.m_errors.begin(), rhs.m_errors.end());
        m_warnlines.insert(m_warnlines.end(), rhs.m_warnlines.begin(), rhs.m_warnlines.end());
        m_warnings.insert(m_warnings.end(), rhs.m_warnings.begin(), rhs.m_warnings.end());
        m_consolelines.insert(m_consolelines.end(), rhs.m_consolelines.begin(), rhs.m_consolelines.end());
        m_console.insert(m_console.end(), rhs.m_console.begin(), rhs.m_console.end());
        return *this;
    }

    void clear()
    {
        m_errlines.clear();
        m_errors.clear();
        m_warnlines.clear();
        m_warnings.clear();
        m_consolelines.clear();
        m_console.clear();
    }

    void setError(const InputLinePtr & line, const std::string &errMsg)
    {
        if (!errMsg.empty())
        {
            m_errlines.push_back(line);
            m_errors.push_back(errMsg);
        }
    }

    void setWarning(const InputLinePtr & line, const std::string& warnMsg)
    {
        if (!warnMsg.empty())
        {
            m_warnlines.push_back(line);
            m_warnings.push_back(warnMsg);
        }
    }

    void setConsole(const InputLinePtr & line, const std::string& consoleMsg)
    {
        if (!consoleMsg.empty())
        {
            m_consolelines.push_back(line);
            m_console.push_back(consoleMsg);
        }
    }


    void printErrors() const
    {
        for (std::size_t index = 0; index < m_errors.size(); index++)
        {
            if (!m_errors[index].empty())
            {
                wprintf((L"Error: " + Unicode::utf8_to_wstring(m_errors[index]) + L"\n").c_str());
                if (m_errlines[index])  // skip nullptrs
                {
                    if (!m_errlines[index]->getFileName().empty())
                    {
                        wprintf((L" File: " + Unicode::utf8_to_wstring(m_errlines[index]->getFileName()) + L"\n").c_str());
                    }
                    if (m_errlines[index]->getLineNum()) // skip line 0
                    {
                        wprintf((L" Line: " + std::to_wstring(m_errlines[index]->getLineNum()) + L": " + Unicode::utf8_to_wstring(m_errlines[index]->getLine()) + L"\n").c_str());
                    }
                }
            }

        }
    }

    void printWarnings() const
    {
        for (std::size_t index = 0; index < m_warnings.size(); index++)
        {
            if (!m_warnings[index].empty())
            {
                wprintf((L"Warning: " + Unicode::utf8_to_wstring(m_warnings[index]) + L"\n").c_str());
                if (m_warnlines[index])     // skip nullptrs
                {
                    if (!m_warnlines[index]->getFileName().empty())
                    {
                        wprintf((L" File: " + Unicode::utf8_to_wstring(m_warnlines[index]->getFileName()) + L"\n").c_str());
                    }
                    if (m_warnlines[index]->getLineNum()) // skip line 0
                    {
                        wprintf((L" Line: " + std::to_wstring(m_warnlines[index]->getLineNum()) + L": " + Unicode::utf8_to_wstring(m_warnlines[index]->getLine()) + L"\n").c_str());
                    }
                }
            }

        }
    }

    void printConsols() const
    {
        for (std::size_t index = 0; index < m_console.size(); index++)
        {
            if (!m_console[index].empty())
            {
                wprintf((L"  " + Unicode::utf8_to_wstring(m_console[index]) + L"\n").c_str());
                if (m_consolelines[index])     // skip nullptrs
                {
                    if (!m_consolelines[index]->getFileName().empty())
                    {
                        wprintf((L" File: " + Unicode::utf8_to_wstring(m_consolelines[index]->getFileName()) + L"\n").c_str());
                    }
                    if (m_consolelines[index]->getLineNum())  // skip line 0
                    {
                        wprintf((L" Line: " + std::to_wstring(m_consolelines[index]->getLineNum()) + L": " + Unicode::utf8_to_wstring(m_consolelines[index]->getLine()) + L"\n").c_str());
                    }
                }
            }
        }
    }

    bool emptyErrors() const { return m_errors.empty(); }
    bool emptyWarnings() const { return m_warnings.empty(); }

    const std::deque<InputLinePtr>& getErrLines()  const { return m_errlines; }
    const std::deque<std::string>&  getErrors()    const { return m_errors; }
    const std::deque<InputLinePtr>& getWarnLines() const { return m_warnlines; }
    const std::deque<std::string>&  getWarnings()  const { return m_warnings; }

  protected:
    std::deque<InputLinePtr> m_errlines;
    std::deque<std::string>  m_errors;

    std::deque<InputLinePtr> m_warnlines;
    std::deque<std::string>  m_warnings;

    std::deque<InputLinePtr> m_consolelines;
    std::deque<std::string>  m_console; // strings to display to console not associated with any line number

};



// everything related to file i/o for maps. Deals with UTF8/UTF16/UTF32 and BOM's
// we use the wide versions of the open functions to ensure unicode paths are supported
// all paths are std::filesystem::paths
// Be careful. Under windows, assigning an std::string to filesystem::path will assume ANSI and
// use the system code page - it does not assume UTF8.
// Under windows it is best to assign wstring to path.
// Another cross platform way is to assign u8string to path, that will assume utf8
class FileIO
{
  public:
    FileIO() = default;
    ~FileIO() = default;

    void setFileName( const std::filesystem::path & filename ) { m_filename = filename; }
    const std::filesystem::path & getFileName() const { return m_filename; }
    bool exists() const
    {
        std::error_code ec;
        return std::filesystem::exists(m_filename, ec);
    }


    // files without a BOM will be scanned to see if what appears to be unicode chars are used. If so, it will assume UnicodeLE
    // Always write UTF8 without a BOM unless forced to unicode where a UnicodeLE BOM is written.
    enum FileEncoding
    {               // 4 byte auto detection from JSON Try both 32's, then 16, then default to ANSI or UTF8 NO BOM depending on options
        ANSI = 0,   // multibyte via ANSI system code page
        UTF8,       // EF BB BF                         xx xx xx xx
        UTF16LE,    // FF FE                            xx 00 xx 00
        UTF16BE,    // FE FF                            00 xx 00 xx
        UTF32LE,    // FF FE 00 00                      xx 00 00 00
        UTF32BE,    // 00 00 FE FF                      00 00 00 xx
    };

    bool hasBOM() const { return m_bBOM; }
    FileEncoding getEncoding() const { return m_encoding; }

    // use this on input files to determine if 8 bit with no BOM is either UTF or ANSI. True is ANSI, false is UTF8
    void setAnsi( bool bAnsi ) { m_bAnsi = bAnsi; }
    
    static std::string getEncodingStr( FileEncoding encoding )
    {
        static constexpr std::string_view encodings[] = { "ANSI", "UTF8", "UTF16LE", "UTF16BE", "UTF32LE", "UTF32BE" };
        return std::string(encodings[encoding]);
    }

    static std::string getEncodingStr( FileEncoding encoding, bool bBOM )
    {
        return getEncodingStr(encoding) + ((encoding==ANSI) ? "" : (bBOM ? " BOM" : " no BOM"));
    }

    std::string getEncodingStr() const
    {
        return getEncodingStr(m_encoding, m_bBOM);
    }

    // set bAnsi to true if you want 8 bit input format without BOM to assume ANSI code page
    // read every line from the file, returning a collection of InputLines.
    std::deque<InputLinePtr> readFile(ErrorWarning &errors)
    {
        std::deque<InputLinePtr> ilines;
        std::string filename = Unicode::wstring_to_utf8(m_filename.wstring());

        FILE *fp = nullptr;
        errno_t err = _wfopen_s( &fp, m_filename.wstring().c_str(), L"rb");
        if (err || fp == nullptr)
        {
            std::string winError = MMUtil::GetLastErrorAsString();
            errors.setError(std::make_shared<InputLine>(),"Unable to open: " + filename + " Win Error: " + winError);

            return ilines;
        }
        determineEncoding(fp);         // check for BOM's determine type of encoding used by file.
        fseek(fp,m_BOMsize,SEEK_SET);  // reset back to beginning or skip from start the BOM 

        int linenum = 0;

        std::string errmsg;

        while (!feof(fp))
        {
            std::string line;
            switch (m_encoding)
            {
                case ANSI:
                case UTF8:
                    line = readLineUTF8(fp, errmsg);
                    break;
                case UTF16LE:
                case UTF16BE:
                    line = readLineUTF16(fp, errmsg);
                    break;
                case UTF32LE:
                case UTF32BE:
                    line = readLineUTF32(fp,errmsg);
                    break;
            }
            linenum++;
            if (line.size() || !feof(fp))   // read something or not at end of file
                ilines.push_back(std::make_shared<InputLine>(line,linenum,filename));
            if (!errmsg.empty())
            {
                errors.setError(ilines.back(),errmsg);
                break;
            }
        }
        fclose(fp);
        return ilines;
    }

    // write the array of lines to output, using provided encoding and BOM override.
    // return true if ok, false if error
    bool writeFile(const std::deque<std::string>& lines, ErrorWarning & errors, FileEncoding encoding, bool forceBOM)
    {
        std::string filename = Unicode::wstring_to_utf8(m_filename.wstring());  // build filename in case of error
        FILE *fp = nullptr;
        errno_t err = _wfopen_s( &fp, m_filename.wstring().c_str(), L"wb");
        if (err || fp == nullptr)
        {
            std::string winError = MMUtil::GetLastErrorAsString();
            errors.setError(std::make_shared<InputLine>(),"Unable to create: " + filename + " Win error: " + winError);
            return false;
        }

        bool retval = true;     // assume it will write completely

        if (forceBOM)
        {
            uint8_t bom[4];   // up to 4 bytes for BOM
            int bomsize = 0;
            switch (encoding)
            {
                case UTF8:
                    bom[0] = 0xef; bom[1] = 0xbb; bom[2] = 0xbf;
                    bomsize = 3;
                    break;
                case UTF16LE:
                    bom[0] = 0xff; bom[1] = 0xfe;
                    bomsize = 2;
                    break;
                case UTF16BE:
                    bom[0] = 0xfe; bom[1] = 0xff;
                    bomsize = 2;
                    break;
                case UTF32LE:
                    bom[0] = 0xff; bom[1] = 0xfe; bom[2] = 0x00; bom[3] = 0x00;
                    bomsize = 4;
                    break;
                case UTF32BE:
                    bom[0] = 0x00; bom[1] = 0x00; bom[2] = 0xfe; bom[3] = 0xff;
                    bomsize = 4;
                    break;
            }
            if (fwrite(bom, bomsize, 1, fp) != 1)
            {
                retval = false;     // stop processing
                std::string winError = MMUtil::GetLastErrorAsString();
                errors.setError(std::make_shared<InputLine>(), "Error writing: " + filename + " Win error: " + winError);
            }
        }
        if (retval)
        {
            for (auto const &it : lines)
            {
                retval = false;
                switch (encoding)
                {
                    case ANSI:  // output ANSI windows current code page
                        retval = writeLineUTF8(fp,Unicode::utf8_to_ansi_string(it));
                        break;
                    case UTF8:
                        retval = writeLineUTF8(fp,it);
                        break;
                    case UTF16LE:
                        retval = writeLineUTF16(fp,it,false);
                        break;
                    case UTF16BE:
                        retval = writeLineUTF16(fp,it,true);
                        break;
                    case UTF32LE:
                        retval = writeLineUTF32(fp,it,false);
                        break;
                    case UTF32BE:
                        retval = writeLineUTF32(fp,it,true);
                        break;
                }
                if (!retval)
                {
                    std::string winError = MMUtil::GetLastErrorAsString();
                    errors.setError(std::make_shared<InputLine>(), "Error writing: " + filename + " Win error: " + winError);
                    break;
                }
            }
        }
        fclose(fp);
        return retval;
    }

    // utility that will search a list of paths and an array of already found paths for given file
    // file may have a relative path already or may be just a name.
    // caller needs to determine if the found file is allowed to be used (script circular references).
    static std::filesystem::path searchPaths(const std::filesystem::path &filename, const std::deque<std::filesystem::path> &incdirs, const std::deque<std::filesystem::path>& paths)
    {
        std::error_code ec;
        if (std::filesystem::exists(filename, ec))
            return filename;

        std::filesystem::path fileName = filename;
        std::filesystem::path fileNameOnly = fileName.filename();           // get just the filename
        std::filesystem::path filePathOnly = fileName;
        filePathOnly.remove_filename();    // get just the path

        std::filesystem::path temp;

        // first search the paths already found
        for (auto const & it : paths)
        {
            temp = it;
            if (!temp.empty())      // should never be empty but just in case
            {
                temp.replace_filename( fileNameOnly );
                if (std::filesystem::exists(temp, ec))
                    return temp;        // return path found

                // try adding on the provided name to an existing found one
                temp = it;
                temp.replace_filename( filename );      // try using this 
                if (std::filesystem::exists(temp, ec))
                    return temp;
            }
        }

        // still not found, try all the paths in inc dirs
        for (auto const & it : incdirs )
        {
            temp = it / fileNameOnly;
            if (std::filesystem::exists(temp, ec))
                return temp;

            temp = it;
            temp.replace_filename( filename );
            if (std::filesystem::exists(temp, ec))
               return temp;
        }
        return std::filesystem::path();   // unable to find, return empty
    }

    // convert input path to absolute path resolving any relative path parts.
    static std::filesystem::path make_absolute_windowsPath(const std::filesystem::path& input)
    {
        std::wstring src = input.wstring();

        // Start with a reasonable buffer size
        DWORD size = 256;
        std::wstring buffer(size, L'\0');

        while (true)
        {
            DWORD result = GetFullPathNameW(
                src.c_str(),
                size,
                buffer.data(),
                nullptr
            );

            if (result == 0)  // API error return empty path
            {
                buffer.clear();
                break;
            }

            if (result < size)
                break;

            // Buffer too small - grow and retry. This should only happen once and only with long paths
            size = result + 16;
            buffer.resize(size);
        }
        return std::filesystem::path(buffer);
    }

    // read a line of wide chars until newline or eof, return as wstring. Does not include newline in result.
    static std::wstring read_wline(FILE* fp)
    {
        std::wstring result;

        if (fp)
        {
            while (true)  // stay in loop until we hit newline or eof
            {
                wint_t wc = fgetwc(fp);

                if (!wc || (wc == WEOF) || (wc == L'\n') || (wc == L'\r'))
                    break;

                result.push_back(static_cast<wchar_t>(wc));
            }
        }
        return result;
    }

    // returns string that has the short form of the git commit for the given path.
    // empty str returned if any error - no git, file not under git control, etc.
    // Note that git is case sensitive on file names even under windows.
    static std::string getGitCommit(const std::filesystem::path & path)
    {
        class saveRestoreCWD  // helper class to auto save and restore current working directory
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

        std::filesystem::path fpath = make_absolute_windowsPath(path);  // get the full filename path with relative resolved.
        if (fpath.empty())  // if no input path, return empty
            return retS;

        std::filesystem::path fname = fpath.filename(); // get just the filename
        fpath.remove_filename();                        // just the path

        std::filesystem::current_path(fpath, ec); // change current dir so git will be in the correct directory

        // using windows wide version - piped output will be unicode and needs to be converted back to utf8
        FILE* fp = _wpopen((L"git log --no-color -1 --abbrev-commit " + fname.wstring()).c_str(), L"r");
        if (fp)
        {
            retS = Unicode::wstring_to_utf8(read_wline(fp));
            _pclose(fp);

            std::size_t pos = retS.find("commit ");
            if (pos == 0)   // it found it
            {
                pos += 7;   // should be start of commit
                std::size_t epos = pos;
                for (; epos < retS.size(); epos++)
                {
                    int ch = std::tolower((unsigned char)retS[epos]);
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
    static std::string getDateStr(const std::filesystem::path & path, std::string_view format)
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
                    sprintf_s(buffer, "%02d", tlf.tm_mon+1);
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

  protected:

    // callers need to reset back to beginning then then skip the m_BOMsize number of bytes
    // if bomSize is 0, there was no detected BOM
    void determineEncoding(FILE* fp)
    {
        m_BOMsize = 0;      // assume no bom
        m_bBOM = false;
        m_encoding = ANSI;  // asssuming ansi
        m_hbf = false;      // asssum low byte first

        // work with all known text formats. Start by looking for BOM's
        // if we have an error reading first 4 bytes, just default to ANSI with no BOM.
        int byte1 = fgetc(fp);
        if (byte1 == EOF)
            return;
        int byte2 = fgetc(fp);
        if (byte2 == EOF)
            return;
        int byte3 = fgetc(fp);
        if (byte3 == EOF)
            return;
        int byte4 = fgetc(fp);
        if (byte4 == EOF)
            return;

        if ((byte1 == 0xef) && (byte2 == 0xbb) && (byte3 == 0xbf))  // check for 3 byte legacy UTF8 BOM
        {
            m_BOMsize = 3;
            m_bBOM = true;
            m_encoding = UTF8;
        }
        else if ((byte1 == 0x00) && (byte2 == 0x00) && (byte3 == 0xfe) && (byte4 == 0xff))
        {
            m_BOMsize = 4;
            m_bBOM = true;
            m_encoding = UTF32BE;
            m_hbf = true;
        }
        else if ((byte1 == 0xff) && (byte2 == 0xfe) && (byte3 == 0x00) && (byte4 == 0x00))
        {
            m_BOMsize = 4;
            m_bBOM = true;
            m_encoding = UTF32LE;
        }
        else if ((byte1 == 0xff) && (byte2 == 0xfe))
        {
            m_BOMsize = 2;
            m_bBOM = true;
            m_encoding = UTF16LE;
        }
        else if ((byte1 == 0xfe) && (byte2 == 0xff))
        {
            m_BOMsize = 2;
            m_bBOM = true;
            m_encoding = UTF16BE;
            m_hbf = true;
        }
        else if ((byte1 == 0x00) && (byte2 == 0x00) && (byte3 == 0x00) && (byte4 != 0x00))       // no BOM, check the same way JSON does.
        {
            m_encoding = UTF32BE;
            m_hbf = true;
        }
        else if ((byte1 != 0x00) && (byte2 == 0x00) && (byte3 == 0x00) && (byte4 == 0x00))
        {
            m_encoding = UTF32LE;
        }
        else if ((byte1 == 0x00) && (byte2 != 0x00) && (byte3 == 0x00) && (byte4 != 0x00))
        {
            m_encoding = UTF16BE;
            m_hbf = true;
        }
        else if ((byte1 != 0x00) && (byte2 == 0x00) && (byte3 != 0x00) && (byte4 == 0x00))
        {
            m_encoding = UTF16LE;
        }
        else  // not UTF32, not UTF16, no bom, use either ANSI or UTF8
            m_encoding = m_bAnsi ? ANSI : UTF8;
    }

    // returns string
    std::string readLineUTF8(FILE* fp,std::string &errmsg)  // read from a multibyte source, return wide string
    {
        std::string line;   // reading utf8!
        line.reserve(4096);	// so += is fast
        for (; !feof(fp);)
        {
            int ch = fgetc(fp);
            if (ch == EOF)
                break;
            ch &= 0xff;
            if (ch == 0)
            {
                errmsg = "Unexpected 0 byte - possibly mismatched UTF8/UTF16 encoding";
                break;
            }
            if (ch == '\r')         // ignore carriage returns, only care about line feeds.
                continue;
            if (ch == '\n')
                break;
            if (ch == 26)  // CTRL-Z
                break;
            line += (uint8_t)ch;
        }
        // if callers wants to use ANSI format, have to convert ANSI to UTF8.
        if (m_encoding == ANSI)
        {
            return Unicode::ansi_to_utf8_string( line ); // convert ansi to wide, then back to utf8
        }
        return line;
    }

    // this supports both utf16 formats so it reads a byte at a time
    std::string readLineUTF16(FILE* fp, std::string &)  // read from unicode save into wstring
    {
        std::u16string line;
        line.reserve(4096);	// so += is fast
        for (; !feof(fp);)
        {
            int ch1 = fgetc(fp);
            if (ch1 == EOF)
                break;
            int ch2 = fgetc(fp);
            if (ch1 == EOF)
                break;
            ch1 &= 0xff;
            ch2 &= 0xff;
            int ch;
            if (m_hbf)
                ch = (ch1 << 8) | ch2;
            else
                ch = (ch2 << 8) | ch1;
            if (ch == 0)
                break;
            if (ch == '\r')   // ignore carriage returns, only care about line feeds.
                continue;
            if (ch == 26)     // CTRL-Z is treated as end of line in this context
                break;
            if (ch == '\n')
                break;
            line += (char16_t)ch;
        }
        return Unicode::utf16_to_utf8_string( line );
    }

    // this supports both utf32 formats so it reads a byte at a time
    std::string readLineUTF32(FILE* fp, std::string &)  // read from unicode save into wstring
    {
        std::u32string u32line;
        u32line.reserve(4096);	// so += is fast
        for (; !feof(fp);)
        {
            int ch1 = fgetc(fp);
            if (ch1 == EOF)
                break;
            int ch2 = fgetc(fp);
            if (ch1 == EOF)
                break;
            int ch3 = fgetc(fp);
            if (ch3 == EOF)
                break;
            int ch4 = fgetc(fp);
            if (ch4 == EOF)
                break;
            ch1 &= 0xff;
            ch2 &= 0xff;
            ch3 &= 0xff;
            ch4 &= 0xff;
            uint32_t ch;
            if (m_hbf)
                ch = (ch1 << 24) | (ch2 < 16) | (ch3 < 8) | ch4;
            else
                ch = (ch4 << 24) | (ch3 < 16) | (ch2 < 8) | ch1;
            if (ch == 0)
                break;
            if (ch == '\r')   // ignore carriage returns, only care about line feeds.
                continue;
            if (ch == 26)     // CTRL-Z is treated as end of line in this context
                break;
            if (ch == '\n')
                break;
            u32line += ch;
        }
        return Unicode::utf32_to_utf8_string( u32line );
    }

    // this is also used to write out ANSI data once it is converted to ansi 8 bit.
    bool writeLineUTF8(FILE* fp, const std::string &line )  // write utf line
    {
        std::size_t retval = 1;
        if (line.size())    // as long as not a null string
            retval = fwrite(line.c_str(), line.size(), 1, fp);  // write the line and it should return 1
        if (retval == 1)
        {
            char crlf[] = "\r\n";
            retval = fwrite(crlf,2,1,fp);
        }
        return retval == 1;
    }

    bool writeLineUTF16(FILE* fp, const std::string &line, bool hbf )  // write utf line
    {
        std::u16string u16line = Unicode::utf8_to_utf16_string( line );
        u16line += '\r';
        u16line += '\n';

        for (auto const & it : u16line)
        {
            uint32_t ch = it;
            uint8_t  out[2];
            if (hbf)
            {
                out[1] = ch & 0xff;
                out[0] = (ch >> 8) & 0xff;
            }
            else
            {
                out[0] = ch & 0xff;
                out[1] = (ch >> 8) & 0xff;
            }
            if (fwrite(out,2,1,fp) != 1)
                return false;
        }
        return true;
    }

    bool writeLineUTF32(FILE* fp, const std::string &line, bool hbf )  // write utf line
    {
        std::u32string u32line = Unicode::utf8_to_utf32_string(line);
        u32line += '\r';
        u32line += '\n';

        for (auto const & it : u32line)
        {
            uint32_t ch = it;
            uint8_t  out[4];
            if (hbf)
            {
                out[3] = ch & 0xff;
                out[2] = (ch >> 8) & 0xff;
                out[1] = (ch >> 16) & 0xff;
                out[0] = (ch >> 24) & 0xff;
            }
            else
            {
                out[0] = ch & 0xff;
                out[1] = (ch >> 8) & 0xff;
                out[2] = (ch >> 16) & 0xff;
                out[3] = (ch >> 24) & 0xff;
            }
            if (fwrite(out,4,1,fp) != 1)
                return false;
        }
        return true;
    }


    std::filesystem::path m_filename;   // filename for this file.

    int          m_BOMsize  = 0;      // how many bytes for BOM. 0 if no BOM
    FileEncoding m_encoding = UTF8;
    bool         m_hbf      = false;  // true if utf16/utf32 is high byte first storage format during read
    bool         m_bAnsi    = false;  // true means 8 bit input with no BOM is assumed to use ANSI code page.
    bool         m_bBOM     = false;  // true means a read detected a BOM. On write, it forces a BOM to be generated for UTF formats.
};


// base class of all map sections, sections with unique processing derive from this.
// contains basic operations for all sections.
// 
class MapSection
{
public:
    MapSection()  = default;
    virtual ~MapSection() = default;
    //MapSection( const MapSection &ref ) = default;

    virtual MapSection* clone()
    {
        return new MapSection( *this );
    }

    void                 setSectionName(const std::string & name) { m_name = name; }
    const std::string & getSectionName() const { return m_name; }

    bool operator == (MapSection const& rhs) const  // for collections, we only use the section name
    {
        return m_name == rhs.m_name;
    }

    bool operator < (MapSection const& rhs) const  // for collections, we only use the section name
    {
        return m_name < rhs.m_name;
    }

    // return true if no error
    virtual bool serializeInLine(InputLinePtr const& line, ErrorWarning & )  // caller does this for every line it has determined is part of the section
    {
        m_lines.push_back(line);
        return true;
    }

    // return reference to list of lines to write. Does not include section name begin/end, caller will do that.
    virtual const std::deque<std::string> & serializeOut()
    {
        // build the output lines
        m_outputLines.clear();
        for (auto it = m_lines.cbegin(); it != m_lines.cend(); ++it)
        {
            m_outputLines.emplace_back((*it)->getLine());
        }

        // return copy of lines to write to file. Does not include the section name and begin/end markers. Caller does that.
        return m_outputLines;
    }

    const std::deque<InputLinePtr>& getLines() const
    {
        return m_lines;
    }

    void setLines(const std::deque<InputLinePtr>& lines)
    {
        m_lines = lines;
    }

    // methods that are section dependent, default they do nothing
    virtual std::string          getValue([[maybe_unused]] const std::string & key) const { return std::string(); }
    virtual void                 AddorModifyItem([[maybe_unused]] const std::string& key, [[maybe_unused]] const std::string& value) {}
    virtual void                 resize([[maybe_unused]] int height, [[maybe_unused]] int width, [[maybe_unused]] int defvalue1, [[maybe_unused]] int defvalue2) {}
    virtual void                 setBorders([[maybe_unused]] int defValue) {}
    virtual void                 flattenHigh([[maybe_unused]] MMUtil::Region const &, [[maybe_unused]] int testVal, [[maybe_unused]] int newValue) {}  // change all values above this to given value
    virtual void                 flattenLow([[maybe_unused]] MMUtil::Region const &, [[maybe_unused]] int testVal, [[maybe_unused]] int newValue) {}  // change all values above this to given value
    virtual void                 flattenBetween([[maybe_unused]] MMUtil::Region const &, [[maybe_unused]] int testLow, [[maybe_unused]] int testHigh, [[maybe_unused]] int newValue) {}  // change all values testLow <= value <=testHigh to newValue
    virtual bool                 verifyBounds([[maybe_unused]] int rows, [[maybe_unused]] int cols, [[maybe_unused]] bool bFix, [[maybe_unused]] const InputLinePtr& iline, [[maybe_unused]] ErrorWarning& errorWarning, [[maybe_unused]] const std::string& errwarnpre) { return true; }
    virtual void                 merge([[maybe_unused]] const MapSection* src, [[maybe_unused]] MMUtil::Region const &region, [[maybe_unused]] int rowOffset, [[maybe_unused]] int colOffset, [[maybe_unused]] bool bAllow1, [[maybe_unused]] bool bAllow2 ) {}  // merge in values from src that are in range
    virtual void                 setTo([[maybe_unused]] int defValue1, [[maybe_unused]] int defValue2) {}  // change all arrays to this value

protected:
    std::string                  m_name;           // name of section
    std::deque<InputLinePtr>     m_lines;          // lines read in for that section
    std::deque<std::string>      m_outputLines;    // lines ready to write
};

class KeyValueSection : public MapSection
{
  protected:
      // key:value storage  Used for info. We own the strings we store
    class keyValue
    {
    public:
        keyValue() = default;
        keyValue(const std::string& key, const std::string& value) : m_key(key), m_value(value), m_lckey(MMUtil::toLower(key)) {}
        ~keyValue() = default;

        void setKeyValue(const std::string & key, const std::string & value)
        {
            m_key   = key;        
            m_lckey = MMUtil::toLower( key );
            m_value = value;
        }

        void setValue(const std::string & value) { m_value = value; }

        const std::string & getKey()   const { return m_key; }
        const std::string & getlcKey() const { return m_lckey; }  // key used in map
        const std::string & getValue() const { return m_value; } 

        bool operator == (const keyValue & rhs) const    // for collections
        {
            return m_lckey == rhs.m_lckey;
        }
        bool operator < (const keyValue & rhs) const    // for collections
        {
            return m_lckey < rhs.m_lckey;
        }

        // first char must be alpha, followed by only alpha or digits. No leading/trailing spaces allowed
        static bool isValidKeyName(std::string_view name)
        {
            int len = (int)name.length();
            bool valid = (len > 0) && iswalpha(name[0]);
            for (int i = 1; (i < len) && valid; i++)
            {
                valid = (iswalpha(name[i]) || iswdigit(name[i]));
            }
            return valid;
        }


        // build line for this key/value pair. MM format is key:value
        std::string serializeOut() const
        {
            std::string rets;
            rets.reserve(m_key.length()+m_value.length()+1);
            rets = m_key;
            rets += L':';
            rets += m_value;
            return rets;
        }

      protected:
        std::string     m_key;       // key name
        std::string     m_lckey;     // converted to lower case - used for comparisions
        std::string     m_value;     // value string
    };

  public:
    KeyValueSection()
    {
        m_optmap.reserve( 32 );
    }
    virtual ~KeyValueSection() = default;

    virtual MapSection* clone() override
    {
        return new KeyValueSection( *this );
    }

    // return empty string if key does not exist
    virtual std::string getValue(const std::string & key) const override
    {
        std::string keylc = MMUtil::toLower(key);
        std::string retval;

        auto it = m_optmap.find(keylc);
        if (it != m_optmap.cend())
        {
            std::size_t index = (*it).second;       // index in the queue
            retval = m_options[index].getValue();
        }
        return retval;
    }

    // add the given key,value or modify existing one with this new value
    virtual void AddorModifyItem(const std::string & key, const std::string & value ) override
    {
        keyValue item( key, value );

        auto it = m_optmap.find(item.getlcKey());
        if (it == m_optmap.end())      // adding a new key
        {
            std::size_t index = m_options.size();
            m_options.push_back( item );
            m_optmap.emplace( item.getlcKey(), index );
        }
        else  // already exists, get index from map, and modify value
        {
            std::size_t index = (*it).second;
            m_options[index].setValue( value );
        }
    }

      // return reference to list of lines to write. Does not include section name begin/end, caller will do that.
    virtual std::deque<std::string> const & serializeOut() override
    {
        // build the output lines
        m_outputLines.clear();
        for (auto it = m_options.cbegin(); it != m_options.cend(); ++it) // process every key/value pair
        {
            m_outputLines.push_back((*it).serializeOut());
        }

        // return copy of lines to write to file. Does not include the section name and begin/end markers. Caller does that.
        return m_outputLines;
    }

    // no need to save input lines, we save key:value pair
    virtual bool serializeInLine(InputLinePtr const& line, ErrorWarning &error) override  // caller does this for every line it has determined is part of the section
    {
        if (!line->getLine().empty())    // ignore blank lines
        {
            std::string_view key;
            std::string_view value;

            // format for lines are key:value. 
            bool retval = getKeyValueFromLine( key, value, line->getLine() );
            if (!retval)
            {
                error.setError(line, m_name + " section. Missing colon");
                return false;
            }
            // make sure keys are valid
            if (!keyValue::isValidKeyName(key))
            {
                error.setError(line, m_name + " section. Invalid key name: " + std::string(key));
                return false;
            }
            if (value.empty())  // we will treat empty values as warnings. They may be errors later
            {
                error.setWarning(line, m_name + " section. Empty value for key: " + std::string(key));
            }
            keyValue kv;
            kv.setKeyValue(std::string(key), std::string(value) );

            auto it = m_optmap.find( kv.getlcKey() ); // check map
            if (it != m_optmap.cend())                // already exists
            {
                error.setError(line, m_name + " section. Duplicate key: " + std::string(key));
                return false;
            }
            std::size_t index = m_options.size();
            m_options.push_back(kv);
            m_optmap.emplace( kv.getlcKey(), index );
        }
        return true;
    }

  protected:

      // generic parse key:value from line. Return false on error
    bool getKeyValueFromLine(std::string_view& key, std::string_view& value, std::string_view line) const
    {
        std::size_t pos = line.find(':');
        if (pos == std::string_view::npos)
            return false;   // todo error code
        key = line.substr(0,pos);
        value = line.substr(pos+1);
        key = MMUtil::removeLeadingAndTrailingWhite(key);     // remove leading and trailing white spaces
        value = MMUtil::removeLeadingWhite(value);
        return true;
    }

    std::deque<keyValue>                         m_options;     // parsed key/value pairs
    std::unordered_map<std::string, std::size_t> m_optmap;      // lc key, index in m_options
};

// used to store an array of values, resources will have two of these, one for crystals and one for ore
class IntArraySection : public MapSection
{
  public:
    IntArraySection() = default;
    virtual ~IntArraySection() = default;

    virtual MapSection* clone() override
    {
        return new IntArraySection( *this );
    }


    virtual bool serializeInLine(InputLinePtr const& line, ErrorWarning& error) override  // caller does this for every line it has determined is part of the section
    {
        std::deque<int> row = processLine(line,error);
        if (row.empty())
            return false;   // no data was returned, error
        m_data.push_back(row);
        m_lines.push_back(line); // save line in case we have size warning/error later when verifing bounds
        return true;
    }

    virtual const std::deque<std::string> & serializeOut() override
    {
        m_outputLines.clear();
        std::string row;
        row.reserve(m_data[0].size()*5);            // guess of max row size (4 chars + , per int)

        for (auto itr = m_data.cbegin(); itr != m_data.cend(); itr++)
        {
            row.clear();
            const std::deque<int> & rowvec = *itr;
            for (auto itc = rowvec.cbegin(); itc != rowvec.cend(); itc++)
            {
                row += std::to_string(*itc);
                row += ',';
            }
            m_outputLines.push_back(row);
        }
        return m_outputLines;
    }

    virtual void resize(int height, int width, int defvalue, [[maybe_unused]] int defvalue2) override  // resize to use this new width/height.
    {
        if (height != m_data.size())       // change in number of rows
        {
            if (height > m_data.size())    // growing.
            {
                std::deque<int> row;
                for (std::size_t i = 0; i < width; row.push_back(defvalue), i++);   // make default row
                for (std::size_t i = m_data.size(); i < height; m_data.push_back(row),i++); // add missing rows
            }
            else  // shrinking, remove entries from end
            {
                for (std::size_t i = m_data.size(); i > height; m_data.pop_back(), i--);
            }
        }
        // fix columns. this handles non-consistant columns
        for (std::size_t row = 0; row < height; row++)
        {
            std::deque<int> & rowvec = m_data[row];
            if (width != rowvec.size())
            {
                if (width > rowvec.size())  // growing
                {
                    for (std::size_t i = rowvec.size(); i < width; rowvec.push_back(defvalue),i++);
                }
                else   // shrinking
                {
                    for (std::size_t i = rowvec.size(); i > width; rowvec.pop_back(), i--);
                }
            }
        }
    }

    // called after all lines are sent, generate warning/error if columns or rows are not correct.
    // only a single warning/error is generated
    // if bFix is set, then correct the array size
    // input line usually points to the trailing } closing the section
    virtual bool verifyBounds( int rows, int cols, bool bFix, const InputLinePtr& iline, ErrorWarning& errorWarning,const std::string& errwarnpre ) override
    {
        if (m_data.size() > rows)   // too many rows
        {
            std::string msg = errwarnpre + " extra rows. Found: " + std::to_string( m_data.size()) + " Expecting: " + std::to_string(rows) + " -fix to correct";
            errorWarning.setWarning(iline,msg);
        }
        else if (m_data.size() < rows)  // missing rows
        {
            std::string msg = errwarnpre + " missing rows. Found: " + std::to_string( m_data.size()) + " Expecting: " + std::to_string(rows) + " -fix to correct";
            if (!bFix)
            {
                errorWarning.setError(iline,msg);
                return false;
            }
            errorWarning.setWarning(iline,msg);
        }

        bool bColWarning = false;
        for (int i = 0; i < (int)m_data.size(); i++)
        {
            std::deque<int> & rowvec = m_data[i];
            if (rowvec.size() > cols)   // too much data
            {
                if (!bColWarning)   // only show warning once
                {
                    bColWarning = true;
                    std::string msg = errwarnpre + " extra columns. Found: " + std::to_string(rowvec.size()) + " Expecting: " + std::to_string(cols) + " -fix to correct";
                    errorWarning.setWarning(iline, msg);
                }
            }
            else if (rowvec.size() < cols)  // not enough data
            {
                InputLinePtr thisline = iline;
                if (i < m_lines.size())
                    thisline = m_lines[i];

                std::string msg = errwarnpre + " missing columns. Found: " + std::to_string(rowvec.size()) + " Expecting: " + std::to_string(cols) + " -fix to correct";
                if (!bFix)
                {
                    errorWarning.setError(thisline, msg);
                    return false;
                }
                errorWarning.setWarning(thisline, msg);
                break;
            }
        }

        return true;
    }

    // merge in data from src using the given offset, clipping to our boundary - it does not grow the data
    // negative offsets will clip src
    // be careful, offsets can be negative don't use std::size_t since its unsigned
    virtual void merge(const MapSection *srcp, MMUtil::Region const & region, int rowOffset, int colOffset, bool bAllow1, [[maybe_unused]] bool bAllow2 ) override  // merge in values from src that are in range
    {
        if (bAllow1)
        {
            const IntArraySection * src = dynamic_cast<const IntArraySection *>(srcp);
            MMUtil::Region tregion = region;
            tregion.intersect(MMUtil::Region(0, 0, src->getCols() - 1, src->getRows() - 1 )); // clip to source size
            if (!tregion.empty())
            {
                int numrows = getRows();
                int numcols = getCols();
                for (int row = tregion.ylo(); row <= tregion.yhi(); row++)
                {
                    for (int col = tregion.xlo(); col <= tregion.xhi(); col++)
                    {
                        if (((row + rowOffset) >= 0) && ((row + rowOffset) < numrows) && ((col + colOffset) >= 0) && ((col + colOffset) < numcols))
                        {
                            int val = src->m_data[row][col];
                            m_data[row + rowOffset][col + colOffset] = val;
                        }
                    }
                }
            }
        }
    }

    virtual void setTo(int defValue1, [[maybe_unused]] int defValue2)    // change all value to this value. Used when clearing out a map ready to receive merge data
    {
        for (auto &row : m_data)
        {
            for (auto cit = row.begin(); cit != row.end(); cit++)
            {
                *cit = defValue1;
            }
        }
    }

    int getRows() const { return (int)m_data.size(); }
    int getCols() const { return (int)(m_data.empty() ? 0 : m_data[0].size()); }

  protected:
    void setNegAllowed( bool val ) { m_bAllowNeg = val; }   // used by height section


    std::deque<int> processLine(const InputLinePtr& line, ErrorWarning& error)
    {
        std::deque<int> output;
        std::string_view token;
        std::string_view sline = line->getLine();
        std::size_t startpos = 0;
        std::size_t endpos = sline.length();    // index after end

        while (startpos < endpos)   // ensures something is left to process
        {
            std::size_t pos = sline.find(',',startpos);  // look for next ,
            if (pos == std::string_view::npos)          // did not find
            {
                pos = endpos;                       // set to past end of string
                token = sline.substr(startpos);     // token is remaining line
            }
            else
            {
                token = sline.substr(startpos, pos - startpos); // everything up to but not including ,
            }
            token = MMUtil::removeLeadingAndTrailingWhite(token);
            if (!MMUtil::isInteger(token, m_bAllowNeg))
            {
                error.setError(line,"Invalid integer");
                output.clear();
                break;
            }
            int val = MMUtil::stoi(token);
            output.push_back(val);
            startpos=pos+1;
        }
        return output;
    }

    std::deque<std::deque<int>> m_data;   // array of col values.
    bool m_bAllowNeg = false;             // set to true if negative values allowed

};

// resource section contains two IntArraySections
class ResourceSection : public MapSection
{
public:
    ResourceSection() = default;
    virtual ~ResourceSection() = default;

    virtual MapSection* clone() override
    {
        return new ResourceSection( *this );
    }

    virtual bool serializeInLine(InputLinePtr const& line, ErrorWarning& error) override  // caller does this for every line it has determined is part of the section
    {
        std::string_view sline = line->getLine();
        sline = MMUtil::removeLeadingAndTrailingWhite(sline);
        if (!sline.empty())
        {
            if (sline == scrystals)
            {
                if (m_bCrystals)
                {
                    error.setError(line, "Duplicate "+ std::string(scrystals));
                    return false;
                }
                m_readstate = 1;
                m_bCrystals = true;
            }
            else if (sline == sore)
            {
                if (m_bOre)
                {
                    error.setError(line, "Duplicate " + std::string(sore));
                    return false;
                }
                m_readstate = 2;
                m_bOre = true;
            }
            else
            {
                if (m_readstate == 0)
                {
                    error.setError(line, "Missing " + std::string(scrystals) + " or " + std::string(sore));
                    return false;
                }
                bool retval = (m_readstate == 1) ?
                    m_crystals.serializeInLine(line,error) :
                    m_ore.serializeInLine(line,error);
                if (!retval)
                    return false;
            }
        }

        return true;
    }

    virtual const std::deque<std::string> & serializeOut() override
    {
        m_outputLines.clear();

        std::deque<std::string> crystaloutput = m_crystals.serializeOut();
        std::deque<std::string> oreoutput     = m_ore.serializeOut();

        m_outputLines.push_back( std::string(scrystals) );
        m_outputLines.insert(m_outputLines.end(),crystaloutput.begin(),crystaloutput.end());
        m_outputLines.push_back( std::string(sore) );
        m_outputLines.insert(m_outputLines.end(),oreoutput.begin(),oreoutput.end());

        return m_outputLines;
    }

    virtual void resize(int height, int width, int defcrystal, int defore) override  // resize to use this new width/height.
    {
        m_crystals.resize( height, width, defcrystal, defcrystal );
        m_ore.resize( height, width, defore, defore );
    }

    virtual bool verifyBounds(int rows, int cols, bool bFix, const InputLinePtr& iline, ErrorWarning& errorWarning, const std::string& errwarnpre) override
    {
        if (!m_crystals.verifyBounds( rows, cols, bFix, iline, errorWarning, errwarnpre + " " + std::string(scrystals)))
            return false;
        return m_ore.verifyBounds( rows, cols, bFix, iline, errorWarning, errwarnpre + " " + std::string(sore));
    }

    virtual void merge(const MapSection* srcp, MMUtil::Region const & region, int rowOffset, int colOffset, bool bAllow1, bool bAllow2) override  // merge in values from src that are in range
    {
        const ResourceSection *src = dynamic_cast<const ResourceSection *>(srcp);
        if (bAllow1)
            m_crystals.merge( &src->m_crystals, region, rowOffset, colOffset, true, false);
        if (bAllow2)
            m_ore.merge( &src->m_ore, region, rowOffset, colOffset, true, false);
    }

    virtual void setTo(int defcrystals, int defore)    // change all value to this value. Used when clearing out a map ready to receive merge data
    {
        m_crystals.setTo(defcrystals,0);
        m_ore.setTo(defore,0);
    }


protected:
    IntArraySection  m_crystals;
    IntArraySection  m_ore;

    int m_readstate = 0;        // for tracking what we are adding to. 1 = crystals, 2 = ore
    bool m_bCrystals = false;   // set once section hit
    bool m_bOre = false;        // set one section hit

    static constexpr char scrystals[] = "crystals:";
    static constexpr char sore[]      = "ore:";
};

class TileSection : public IntArraySection
{
  public:
    TileSection() = default;
    virtual ~TileSection() = default;

    virtual MapSection* clone() override
    {
        return new TileSection( *this );
    }

    // used for tiles to ensure we have the proper borders
    // used to force border heights to default value
    virtual void setBorders(int defValue) override
    {
        for (std::size_t col = 0; col < m_data[0].size(); col++)  // top/bottom column loop
        {
            m_data[0][col] = defValue;                     // top row
            m_data[m_data.size()-1][col] = defValue;       // bottom
        }

        for (std::size_t row = 0; row < m_data.size(); row++)  // left/right row loop
        {
            m_data[row][0] = defValue;                      // left column
            m_data[row][m_data[row].size()-1] = defValue;   // right column
        }
    }
};

// height section has extra row/col since we store the height for each corner of a tile
class HeightSection : public IntArraySection  // heights are allowed to be negative
{
  public:
    HeightSection() { setNegAllowed(true); }
    virtual ~HeightSection() = default;

    virtual MapSection* clone() override
    {
        return new HeightSection( *this );
    }

    // we do not change the border values
    virtual void flattenHigh(MMUtil::Region const &region, int testVal, int newValue) override  // change all values above this to given value
    {
        MMUtil::Region tregion( 1, 1, getCols()-2, getRows()-2 ); // region of this data excluding borders
        tregion.intersect(region);                                // ensure the required region is fully contained
        if (!tregion.empty())   // something to do
        {
            for (int rowindex = tregion.ylo(); rowindex <= tregion.yhi(); rowindex++)
            {
                std::deque<int>& rowvec = m_data[rowindex];
                for (int colindex = tregion.xlo(); colindex <= tregion.xhi(); colindex++)
                {
                    int& val = rowvec[colindex];
                    if (val >= testVal)
                        val = newValue;
                }
            }
        }
    }

    virtual void flattenLow(MMUtil::Region const &region,int testVal, int newValue) override  // change all values above this to given value
    {
        MMUtil::Region tregion( 1, 1, getCols()-2, getRows()-2 ); // region of this data excluding borders
        tregion.intersect(region);                                // ensure the required region is fully contained
        if (!tregion.empty())   // something to do
        {
            for (int rowindex = tregion.ylo(); rowindex <= tregion.yhi(); rowindex++)
            {
                std::deque<int>& rowvec = m_data[rowindex];
                for (int colindex = tregion.xlo(); colindex <= tregion.xhi(); colindex++)
                {
                    int& val = rowvec[colindex];
                    if (val <= testVal)
                        val = newValue;
                }
            }
        }
    }

    virtual void flattenBetween(MMUtil::Region const &region,int testLow, int testHigh, int newValue) override  // change all values testLow <= value <=testHigh to newValue
    {
        MMUtil::Region tregion( 1, 1, getCols()-2, getRows()-2 ); // region of this data excluding borders
        tregion.intersect(region);                                // ensure the required region is fully contained
        if (!tregion.empty())   // something to do
        {
            for (int rowindex = tregion.ylo(); rowindex <= tregion.yhi(); rowindex++)
            {
                std::deque<int>& rowvec = m_data[rowindex];
                for (int colindex = tregion.xlo(); colindex <= tregion.xhi(); colindex++)
                {
                    int& val = rowvec[colindex];
                    if ((val >= testLow) && (val <= testHigh))
                        val = newValue;
                }
            }
        }
    }

    // heights have 1 more in row,cols since they are a cell and we have last cell corners.
    virtual bool verifyBounds(int rows, int cols, bool bFix, const InputLinePtr& iline, ErrorWarning& errorWarning, const std::string& errwarnpre) override
    {
        return IntArraySection::verifyBounds(rows+1, cols+1, bFix, iline, errorWarning, errwarnpre);
    }

    virtual void resize(int height, int width, int defheight, int ) override  // resize to use this new width/height.
    {
        IntArraySection::resize(height+1, width+1, defheight, 0);
    }

    virtual void merge(const MapSection* srcp, MMUtil::Region const & region, int rowOffset, int colOffset, bool bAllow1, [[maybe_unused]] bool bAllow2) override  // merge in values from src that are in range
    {
        if (bAllow1)
        {
            MMUtil::Region tregion(region.xlo(), region.ylo(), region.xhi() + 1, region.yhi() + 1); // expand region width and height by 1 for height data
            const HeightSection* src = dynamic_cast<const HeightSection*>(srcp);
            IntArraySection::merge(src, tregion, rowOffset, colOffset, true, false);
        }
    }
};

class MMMap
{
  public:
    MMMap()
    {
        //m_sections.reserve(32); // really only need 17
    }
    ~MMMap()
    {
        deleteSections();
    }

    MMMap( const MMMap & rhs ) = delete;        // no copy constructor
    MMMap( MMMap && rhs ) noexcept = delete;    // no move constructor

    void setFileName(const std::filesystem::path& filename)
    {
        m_file.setFileName( filename );
    }
    bool fexists() const
    {
        return m_file.exists();
    }

    const ErrorWarning & getErrors() { return m_errors; }

    // open and read in the map using the filename previously set
    // return true = ok, false is an error
    bool readMap( bool bReadANSI )
    {
        if (!m_file.exists())
        {
            std::string file = Unicode::wstring_to_utf8(m_file.getFileName().wstring());
            m_errors.setError(std::make_shared<InputLine>(), "Unable to locate source: " + file);
            return false;
        }
        m_file.setAnsi( bReadANSI );
        m_inlines = m_file.readFile(m_errors);    // read in entire map.
        return !m_inlines.empty() && m_errors.getErrors().empty(); // have somthing read and no errors
    }

    bool writeMap( FileIO::FileEncoding encoding, bool forceBOM )
    {
        return m_file.writeFile( m_outlines, m_errors, encoding, forceBOM);
    }

    
    // return encoded string including BOM
    // also returns encoding and hasBOM via reference
    std::string getEncoding(FileIO::FileEncoding& encoding, bool& hasBOM) const
    {
        encoding = m_file.getEncoding();
        hasBOM = m_file.hasBOM();
        return m_file.getEncodingStr();
    }

    int getNumLinesRead() const  { return (int)m_inlines.size(); }
    int getNumLinesWrite() const { return (int)m_outlines.size(); }

    int getHeight() const { return m_height; }
    int getWidth()  const { return m_width; }

    // build but do not extract or test data inside of sections
    bool serializeInSections()
    {
        std::string sectionName;

        MapSection *ms = nullptr;
        for (auto const &iline : m_inlines)
        {
            const std::string & linestr = iline->getLine(); // get ref to string data
            std::string_view lineview = linestr;            // get view to string data
            if (ms)     // already building a section - wait until ending }
            {
                std::size_t pos = lineview.find('}');
                if (pos == 0)   // section is finished. It was already added to the map and it contains all if its info
                {
                    ms = nullptr;
                }
                else   // add to existing section
                {
                    if (!ms->serializeInLine(iline, m_errors))     // if an error returned
                        return false;                           // exit out
                }
            }
            else   // not yet building a section, skip empty lines wait for section start
            {
                if (MMUtil::isEmptyStr(lineview))  // skip empty lines
                    continue;
                std::size_t pos = lineview.find('{');
                if (pos == std::string_view::npos)
                {
                    m_errors.setError(iline, "Missing section start {");
                    return false;
                }
                std::string_view secname = lineview.substr(0, pos);
                std::string lcsecname = MMUtil::toLower(secname);          // only work in lower case

                // section name must be one of the allowed names
                auto nit = m_mapLookup.find(lcsecname);
                if (nit == m_mapLookup.end())
                {
                    m_errors.setWarning(iline, "Unknown section name: " + lcsecname);
                }
                // we can only have a single instance of a section
                if (m_sections.find(lcsecname) != m_sections.cend())  // its already in the map
                {
                    m_errors.setError(iline, "Duplicate section name: " + lcsecname);
                    return false;
                }

                // its a known name. Make the correct class type
                switch ((*nit).second)  // ones unknown will all be generic MapSection
                {
                    case 1:     // info
                        ms = new KeyValueSection();
                        break;
                    case 2:     // tiles
                        ms = new TileSection();
                        break;
                    case 3:     // height
                        ms = new HeightSection();
                        break;
                    case 4:     // resources
                        ms = new ResourceSection();
                        break;

                    default:
                    case 0:     // comments
                    case 5:     // objectives
                    case 6:     // buildings
                    case 7:     // landslidefequency
                    case 8:     // lavaspread
                    case 9:     // miners
                    case 10:    // briefing
                    case 11:    // briefingsuccess
                    case 12:    // briefingfailure
                    case 13:    // vehicles
                    case 14:    // creatures
                    case 15:    // blocks
                    case 16:    // script
                        ms = new MapSection();
                        break;
                } // switch
                ms->setSectionName(lcsecname);

                m_sections.emplace(lcsecname,ms);
            }
        }
        return true;
    }

    // serialize out all the streams. Missing streams will have no data inside the section.
    // we serialize out always in a fixed order, ignoring any input order.
    void serializeOutSections()
    {
        m_outlines.clear();

        std::string line;
        line.reserve( (m_width+1) * 5 );    // estimate of worse case array output but it could be longer

        for (auto const &it : m_mapOrder)
        {
            std::string secnamestr = it.getName();

            line = secnamestr + '{';
            m_outlines.push_back(line);

            auto its = m_sections.find(secnamestr);
            if (its != m_sections.cend())
            {
                MapSection* ms = (*its).second;

                std::deque<std::string> strs = ms->serializeOut();
                m_outlines.insert(m_outlines.end(),strs.cbegin(), strs.cend() );
            }
            line = '}';                // end of section
            m_outlines.push_back(line);
        }
    }

    // with the map read in, now process all of the sections. Make sure those that are required exist,
    // extract out imporant info, and fix any array errors
    bool processSections( bool bFix, int deftile, int defheight, int defcrystal, int defore)
    {
        // look at all sections and make sure the requires ones are there.
        for (auto const & it : m_mapOrder)
        {
            bool  optional = it.getOptional();
            if (!optional)
            {
                std::string name = it.getName();
                MapSection *ms = findMapSection( name );
                if (!ms)
                {
                    m_errors.setError(std::make_shared<InputLine>(),"Missing required section: " + name);
                    return false;
                }
            }
        }

        int intval = 0;
        // now get imporant info from info section
        MapSection *ms = findMapSection( sinfo );
        std::string value;
        value = ms->getValue(std::string(scolcount));
        intval = std::stoi(value);
        if (value.empty() || !MMUtil::isInteger(value, false) || (intval < 3))
        {
            m_errors.setError(std::make_shared<InputLine>(),"info section. Missing or invalid: " + std::string(scolcount));
            return false;
        }
        m_width = intval;
        value = ms->getValue(std::string(srowcount));
        intval = std::stoi(value);
        if (value.empty() || !MMUtil::isInteger(value, false) || (intval < 3))
        {
            m_errors.setError(std::make_shared<InputLine>(),"Info section. Missing or invalid: " + std::string(srowcount));
            return false;
        }
        m_height = intval;

        // now need to make sure all the array sizes are valid.
        ms = findMapSection( stiles );
        if (!ms->verifyBounds( m_height, m_width, bFix, std::make_shared<InputLine>(), m_errors, ms->getSectionName()))
            return false;
        ms->resize( m_height, m_width, deftile, deftile);
        //ms->setBorders( 38 );   // make sure borders are solid rock regular

        ms = findMapSection( sheight );
        if (!ms->verifyBounds( m_height, m_width, bFix, std::make_shared<InputLine>(), m_errors, ms->getSectionName()))
            return false;
        ms->resize( m_height, m_width, defheight, defheight);

        ms = findMapSection( sresources );
        if (!ms->verifyBounds( m_height, m_width, bFix, std::make_shared<InputLine>(), m_errors, ms->getSectionName()))
            return false;
        ms->resize( m_height, m_width, defcrystal, defore);

        return true;
    }

    void copyFrom(const MMMap& rhs, const CommandLineOptions& options)
    {
        // first do a copy.
        copy( rhs );    // copy input lines, sections, unicode settings

        // now process any resize on tiles, height, crystals, ore
        resize(options.m_nRowResize, options.m_nColResize, options.m_nDefTileID, options.m_nDefHeight, options.m_nDefCrystal, options.m_nDefOre);

        // now clear out the tiles, height, crystals and ore.
        // merge from source applying any offset
        MapSection *ms = findMapSection( stiles );
        ms->setTo( options.m_nDefTileID, 0 ); // TODO rethink this
        ms->merge(rhs.findMapSection( stiles ), options.m_mergeRegion, options.m_nOffsetCol, options.m_nOffsetRow, true, false);

        ms = findMapSection( sheight );
        ms->setTo( options.m_nDefHeight, 0 );  // TODO rethink this
        ms->merge(rhs.findMapSection( sheight ), options.m_mergeRegion, options.m_nOffsetCol, options.m_nOffsetRow, true, false);

        ms = findMapSection( sresources );
        ms->setTo( options.m_nDefCrystal, options.m_nDefOre );  // TODO rethink this
        ms->merge(rhs.findMapSection( sresources ), options.m_mergeRegion, options.m_nOffsetCol, options.m_nOffsetRow, true, true);
    }

    // after reading source map, if output will be replaced with processed source, copy from the source to output
    // inputlines, sections, width/height and unicode settings
    MMMap& copy(const MMMap& rhs)
    {
        copySections( rhs.m_sections );

        m_inlines       = rhs.m_inlines;

        m_width    = rhs.m_width;
        m_height   = rhs.m_height;

        m_bUTF16 = rhs.m_bUTF16;
        m_bUTF16LE = rhs.m_bUTF16LE;
        return *this;
    }

    // resize to given height and width. If 0, use existing value. Modify tiles, height, crystals, ore and map size in info section
    void resize(int rowSize, int colSize, int defTile, int defHeight, int defCrystal, int defOre )
    {
        if (!rowSize)
            rowSize = m_height;
        if (!colSize)
            colSize = m_width;

        MapSection *ms = findMapSection( stiles );
        ms->resize(rowSize, colSize, defTile, 0);
        ms->setBorders( 38 );                           // tile borders all set to solid rock regular

        ms = findMapSection( sheight );
        ms->resize( rowSize, colSize, defHeight, 0 );     // heigh section auto deals with extra row/col

        ms = findMapSection( sresources );
        ms->resize( rowSize, colSize, defCrystal, defOre );

        m_height = rowSize;
        m_width = colSize;

        // update map size in info
        ms = findMapSection( sinfo );
        ms->AddorModifyItem( scolcount, std::to_string(colSize));
        ms->AddorModifyItem( srowcount, std::to_string(rowSize));
    }

    bool merging(const MMMap& srcMap, MMUtil::Region const &region, int nOffsetRow, int nOffsetCol, bool bMergeTiles, bool bMergeHeight, bool bMergeCrystal, bool bMergeOre)
    {
        // see if merging
        if (bMergeTiles)
        {
            findMapSection( stiles )->merge( srcMap.findMapSection(stiles), region, nOffsetRow, nOffsetCol, true, false);
        }

        if (bMergeHeight)
        {
            findMapSection( sheight )->merge( srcMap.findMapSection(sheight), region, nOffsetRow, nOffsetCol, true, false);
        }

        if (bMergeCrystal || bMergeOre)
        {
            findMapSection(sresources)->merge(srcMap.findMapSection(sresources), region, nOffsetRow, nOffsetCol, bMergeCrystal, bMergeOre);
        }
        return true;
    }

    void flattenHeightHigh( MMUtil::Region const &region, int testVal, int newValue)    // change all values above this to given value
    {
        MapSection *ms = findMapSection( sheight );
        ms->flattenHigh( region, testVal, newValue);
    }

    void flattenHeightLow( MMUtil::Region const &region, int testVal, int newValue)      // change all values above this to given value
    {
        MapSection *ms = findMapSection( sheight );
        ms->flattenLow( region, testVal, newValue);
    }
    
    void flattenHeightBetween( MMUtil::Region const &region, int testLow, int testHigh, int newValue)    // change all values testLow <= value <=testHigh to newValue
    {
        MapSection *ms = findMapSection( sheight );
        ms->flattenBetween( region, testLow, testHigh, newValue);
    }

    void borderHeight(int height)
    {
        MapSection *ms = findMapSection( sheight );
        ms->setBorders( height );
    }

    void setMapName(const std::string & mapname)
    {
        MapSection *ms = findMapSection( sinfo );
        ms->AddorModifyItem( slevelname, mapname );
    }

    void setMapCreator(const std::string& creator)
    {
        MapSection *ms = findMapSection( sinfo );
        ms->AddorModifyItem( screator, creator );
    }

    const std::deque<InputLinePtr> & getSectionLines( std::string const & name ) const
    {
        const MapSection *ms = findMapSection( name );
        if (ms)
        {
            return ms->getLines();
        }
        static std::deque<InputLinePtr> empty;
        return empty;
    }

    const std::deque<InputLinePtr> & getScriptLines() const
    {
        return getSectionLines(sscript);
    }

    const std::deque<InputLinePtr> & getObjectiveLines() const
    {
        return getSectionLines(sobjectives);
    }

    const std::deque<InputLinePtr> & getBlockLines() const
    {
        return getSectionLines(sblocks);
    }

    // return number of lines
    int setScriptLines(const std::deque<InputLinePtr>& lines)
    {
        MapSection *ms = findMapSection( sscript );
        if (ms)
        {
            ms->setLines( lines );
        }
        return (int)lines.size();
    }

    std::string getMapName() const
    {
        const MapSection *ms = findMapSection( sinfo );
        return ms->getValue( slevelname );
    }

    // return number of lines replaced, 0 if unchanged
    // bAnsi = true means read 8 bit input with no BOM as ANSI, false = UTF-8
    // bAnsi7 = true means check input for 7 bit ANSI, set m_warning for first 8 bit char on each line found
    int briefing(std::filesystem::path const& filename, bool bAnsi, bool bAnsi7 )
    {
        if (!filename.empty())
        {
            FileIO file;
            file.setFileName(filename);
            file.setAnsi(bAnsi);
            if (file.exists())
            {
                std::deque<InputLinePtr> ilp = file.readFile( m_errors );
                if (bAnsi7)
                    checkFor8bit(ilp);

                if (m_errors.emptyErrors() && !ilp.empty())
                {
                    MapSection *mp = findMapSection(sbriefing);
                    if (mp)
                    {
                        mp->setLines(ilp);
                        m_errors.setConsole(nullptr, std::string("Read briefing file: ") + filename.string() );
                        m_errors.setConsole(nullptr, std::string("Encoding: ")+file.getEncodingStr() + ", Lines read: " + std::to_string(ilp.size()) );
                        return (int)ilp.size();  // number of lines
                    }
                }
            }
            else
                m_errors.setWarning( InputLinePtr(), std::string("Unable to open briefing file: ") + filename.string());
        }
        return 0;
    }

    // return number of lines replaced, 0 if unchanged
    int success(std::filesystem::path const& filename, bool bAnsi, bool bAnsi7 )
    {
        if (!filename.empty())
        {
            FileIO file;
            file.setFileName(filename);
            file.setAnsi(bAnsi);
            if (file.exists())
            {
                std::deque<InputLinePtr> ilp = file.readFile( m_errors );
                if (bAnsi7)
                    checkFor8bit(ilp);

                if (m_errors.emptyErrors() && !ilp.empty())
                {
                    MapSection *mp = findMapSection(sbriefingsuccess);
                    if (mp)
                    {
                        mp->setLines(ilp);
                        m_errors.setConsole(nullptr, std::string("Read success file: ") + filename.string() );
                        m_errors.setConsole(nullptr, std::string("Encoding: ")+file.getEncodingStr() + ", Lines read: " + std::to_string(ilp.size()) );
                        return (int)ilp.size();  // number of lines
                    }
                }
            }
            else
                m_errors.setWarning( InputLinePtr(), std::string("Unable to open briefingsuccess file: ") + filename.string());
        }
        return 0;
    }

    // return number of lines replaced, 0 if unchanged
    int failure(std::filesystem::path const& filename, bool bAnsi, bool bAnsi7)
    {
        if (!filename.empty())
        {
            FileIO file;
            file.setFileName(filename);
            file.setAnsi(bAnsi);
            if (file.exists())
            {
                std::deque<InputLinePtr> ilp = file.readFile(m_errors);
                if (bAnsi7)
                    checkFor8bit(ilp);

                if (m_errors.emptyErrors() && !ilp.empty())
                {
                    MapSection* mp = findMapSection(sbriefingfailure);
                    if (mp)
                    {
                        mp->setLines(ilp);
                        m_errors.setConsole(nullptr, std::string("Read failure file: ") + filename.string());
                        m_errors.setConsole(nullptr, std::string("Encoding: ") + file.getEncodingStr() + ", Lines read: " + std::to_string(ilp.size()));
                        return (int)ilp.size();  // number of lines
                    }
                }
            }
            else
                m_errors.setWarning(InputLinePtr(), std::string("Unable to open briefingfailure file: ") + filename.string());
        }
        return 0;
    }



    void clearErrorsWarnings()
    {
        m_errors.clear();
    }

protected:
    void checkFor8bit( const std::deque<InputLinePtr>& lines )
    {
        for (auto const & line : lines)
        {
            char ch = MMUtil::find8BitChar(line->getLine());
            if (ch)
            {
                char str[] = { ch, ')', 0};
                m_errors.setWarning(line, std::string("8 bit character found in srcansi7 mode: (") + str );
            }
        }
    }


    void deleteSections()
    {
        for (auto & it : m_sections)
        {
            MapSection* ms = it.second;
            it.second = nullptr;
            if (ms)
                delete ms;
        }
        m_sections.clear();
    }

    void copySections( const std::unordered_map<std::string, MapSection *> &rhs)
    {
        deleteSections();   // delete any section data
        for (auto const & it : rhs)
        {
            MapSection *msclone = it.second->clone();
            m_sections.emplace( it.first, msclone  );
        }
    }


    MapSection* findMapSection(const std::string &name)
    {
        MapSection *ms = nullptr;
        auto it = m_sections.find(name);
        if (it != m_sections.cend())
        {
            ms = (*it).second;
        }
        return ms;
    }

    const MapSection* findMapSection(const std::string &name) const
    {
        const MapSection *ms = nullptr;
        auto it = m_sections.find(name);
        if (it != m_sections.cend())
        {
            ms = (*it).second;
        }
        return ms;
    }



    FileIO       m_file;      // everything for reading and writing.
    ErrorWarning m_errors;    // errors

    std::deque<InputLinePtr> m_inlines;     // lines read in
    std::deque<std::string>  m_outlines;    // lines ready to write to file

    std::unordered_map<std::string, MapSection *> m_sections;  // holds all of the data for all sections

    int  m_height = 0;      // map height from info section
    int  m_width = 0;       // map width from info section

    bool m_bUTF16 = false;
    bool m_bUTF16LE = false;

    // commonly used strings 
    static constexpr char scolcount[]  = "colcount";
    static constexpr char srowcount[]  = "rowcount";
    static constexpr char screator[]   = "creator";
    static constexpr char slevelname[] = "levelname";

    // all of the sections
    static constexpr char scomments[]           = "comments";
    static constexpr char sinfo[]               = "info";
    static constexpr char stiles[]              = "tiles";
    static constexpr char sheight[]             = "height";
    static constexpr char sresources[]          = "resources";
    static constexpr char sobjectives[]         = "objectives";
    static constexpr char sbuildings[]          = "buildings";
    static constexpr char slandslidefrequency[] = "landslidefrequency";
    static constexpr char slavaspread[]         = "lavaspread";
    static constexpr char sminers[]             = "miners";
    static constexpr char sbriefing[]           = "briefing";
    static constexpr char sbriefingsuccess[]    = "briefingsuccess";
    static constexpr char sbriefingfailure[]    = "briefingfailure";
    static constexpr char svehicles[]           = "vehicles";
    static constexpr char screatures[]          = "creatures";
    static constexpr char sblocks[]             = "blocks";
    static constexpr char sscript[]             = "script";
    // order of sections in a map

    class mapOrder
    {
      public:
          mapOrder() = default;
          mapOrder(const std::string &name, int id, bool optional) : m_name(name), m_id(id), m_optional(optional) {}
          ~mapOrder() {}

          const std::string &  getName()     const { return m_name; }
          int                  getID()       const { return m_id;   }
          bool                 getOptional() const { return m_optional; }
      protected:

        std::string  m_name;       // name of section
        int          m_id;         // order index. write in order 0 - 16
        bool         m_optional;   // true if section optional on read, false is required. On write output empty if it has no data.
    };

    // this vector must in order, it defines the order that maps are created
    std::deque<mapOrder> m_mapOrder =
    {
        { scomments,           0, true  },
        { sinfo,               1, false },
        { stiles,              2, false },
        { sheight,             3, false },
        { sresources,          4, false },
        { sobjectives,         5, true  },
        { sbuildings,          6, true  },
        { slandslidefrequency, 7, true  },
        { slavaspread,         8, true  },
        { sminers,             9, true  },
        { sbriefing,          10, true  },
        { sbriefingsuccess,   11, true  },
        { sbriefingfailure,   12, true  },
        { svehicles,          13, true  },
        { screatures,         14, true  },
        { sblocks,            15, true  },
        { sscript,            16, true  }
    };

    // fast index from name to array order
    std::unordered_map<std::string,int> m_mapLookup =
    {
        { scomments,           0 },
        { sinfo,               1 },
        { stiles,              2 },
        { sheight,             3 },
        { sresources,          4 },
        { sobjectives,         5 },
        { sbuildings,          6 },
        { slandslidefrequency, 7 },
        { slavaspread,         8 },
        { sminers,             9 },
        { sbriefing,          10 },
        { sbriefingsuccess,   11 },
        { sbriefingfailure,   12 },
        { svehicles,          13 },
        { screatures,         14 },
        { sblocks,            15 },
        { sscript,            16 }
    };

}; // class MMMap


#pragma pop_macro("min")
#pragma pop_macro("max")


#endif  // MMDatUtil_H
