// (c)2022 by Tyab
// Manic Miners .DAT file utilities
// Contains classes and methods to read/write entire .DAT files and break up into logical sections
//

#ifndef MMDatUtil_H
#define MMDatUtil_H

#include <stdio.h>
#include <string>
#include <vector>
#include <codecvt>
#include <unordered_set>
#include <filesystem>

#if _MSVC_LANG >= 201703L  // detect for utf16-utf8 conversions
#include <Windows.h>
#endif

bool fExists(const std::string& FileName); // code in MMDatUtil.cpp

// utility that will search a list of paths and ; separated paths and an array of already found paths for given file
// file may have a relative path already or may be just a name.
std::string searchPaths(const std::string& filename, const std::string& incDirs, const std::vector<std::filesystem::path>& paths); // code in MMDatUtil.cpp


// returns string that has the short form of the git commit for the given path.
// nullstr returned if any error - no git, file not under git control, etc.
std::string getGitCommit(std::string_view path);    // code in MMDatUtil.cpp


// returns string specified by format for the last modified time of the given path
// null string if any error
// format chars are Y is year, M is month, D is day, H is hour, N is minute, S is second. G is git commit support
// years is 4 digits always. month, day, hour, minute, second are 2 digit 0 filled
// git commit is the short form as determined by git
// all other chars are copied as is to output preserving case.
std::string getDateStr(std::string_view path, std::string_view format);   // code in MMDatUtil.cpp

bool isEmptyStr(std::string_view str);

// must return a new string
std::string toLower(std::string str);

// must return a new string
std::string toLower(std::string_view str);

std::string_view removeLeadingWhite(std::string_view str);

// first char must be alpha, followed by only alpha or digits. No leading/trailing spaces allowed
// this is not for script, but for keys in other sections
bool isValidVarName(std::string_view name);

bool isInteger(std::string_view val, bool bAllowNeg);
bool isFloat(std::string_view val, bool bAllowNeg);

// standard does not provide a stoi for string_view, so here is one
int stoi(std::string_view val);

// returns string
std::string readLineUTF8(FILE* fp);  // read from a multibyte source, return std::string

// note that we are passing in strings - not string_views - conversion needs null term string
std::wstring utf8_to_wstring(const std::string& str);

// note that we are passing in strings - not string_view - conversion needs null term string
std::string wstring_to_utf8(const std::wstring& str);

std::string readLineUTF16(FILE* fp, bool lbf = true);  // read from unicode save into utf8 in std::string.

void getFileEncoding(FILE* fp, bool& bUTF8BOM, bool& bUTF16LE, bool& bUnicode);  // get encoding flags


// options we pass
class CommandLineOptions
{
public:
    class keyValue
    {
    public:
        keyValue() = default;
        keyValue(std::string_view key, std::string_view value) : m_key(key), m_value(value) {}
        ~keyValue() = default;

        const std::string& key()   const { return m_key; }
        const std::string& value() const { return m_value; }

    protected:
        std::string m_key;
        std::string m_value;
    };
    void addKeyValue(std::string_view key, std::string_view value)
    {
        m_defines.push_back(keyValue(key, value));
    }

    std::string  m_srcmap;
    std::string  m_outmap;
    std::string  m_srcscript;
    std::string  m_mapName;             // override map name in info section
    std::string  m_creator;             // override curator name in info section
    std::string  m_sincdirs;            // list of paths to search for script includes
    std::string  m_datestr = "\"y.m.d\""; // TyabScriptDate and TyabScriptIncDate format

    std::vector<keyValue> m_defines;

    int          m_nOffsetRow  = 0;
    int          m_nOffsetCol  = 0;
    int          m_nRowResize  = 0;
    int          m_nColResize  = 0;
    int          m_nDefTileID  = 1;
    int          m_nDefHeight  = 0;
    int          m_nDefCrystal = 0;
    int          m_nDefOre     = 0;

    int          m_srow = 0;    // rect start row
    int          m_scol = 0;    // rect start col
    int          m_erow = 0;    // rect end row
    int          m_ecol = 0;    // rect end col
    int          m_flathighval = 0;  // flatten high height to check
    int          m_flathighnewheight = 0;  // flatten high new height
    int          m_flatlowval = 0;  // flatten low height to check
    int          m_flatlownewheight = 0;  // flatten low new height
    int          m_flatBetweenLow = 0;  // flatten between low height to check
    int          m_flatBetweenHigh = 0;  // flatten between high height to check
    int          m_flatBetweenVal = 0;   // flatten between new height
    int          m_BorderHeight = 0;    // set all borders heights to this value

    bool         m_bMergeHeight   = false;
    bool         m_bMergeCrystal  = false;
    bool         m_bMergeOre      = false;
    bool         m_bMergeTiles    = false;
    bool         m_bOverwrite     = false;
    bool         m_bEraseOutMap   = false;
    bool         m_bHelp          = false;
    bool         m_bFix           = false;
    bool         m_bScrFixSpace   = false;  // automatic fix spaces where not allowed
    bool         m_bScrNoComments = false;  // remove all non #. comments
    bool         m_bMergeRect     = false;  // if true merge src is a subregion
    bool         m_bFlattenHigh = false;
    bool         m_bFlattenLow    = false;
    bool         m_bFlattenBetween= false;
    bool         m_bBorderHeight  = false;
};


// every line read in has the line and the line number.
// either store a real string or we store a view to it.
class InputLine
{
  public:
    InputLine() = default;
    InputLine(const std::string    & line, int linenum) { setLine(line, linenum); }  // store string copy
    InputLine(      std::string_view line, int linenum) { setLine(line, linenum); }  // store view into existing string
    InputLine(const InputLine& src)  // must have copy constructor since we store string_view into ourself
    {
        copy(src);
    }

    ~InputLine() = default;

    InputLine& operator = (const InputLine& rhs)
    {
        return copy(rhs);
    }

    void clear()
    {
        m_string.clear();
        m_strview = m_string;
        m_linenum = 0;
    }

    std::string_view getLine()    const { return m_strview; }  // views are read only
    int              getLineNum() const { return m_linenum; }

    // used to store the real string
    void setLine(const std::string & line, int linenum)
    {
        m_string  = line;
        m_strview = m_string;  // view to this line we are storing
        m_linenum = linenum;
    }

    // storing a view to already existing string
    void setLine(std::string_view line, int linenum)
    {
        m_string.clear();       // only storing view
        m_strview = line;       // save view into existing line owned by someone else
        m_linenum = linenum;
    }


protected:
    InputLine& copy(const InputLine& rhs)
    {
        m_string  = rhs.m_string;
        m_strview = rhs.m_strview;
        m_linenum = rhs.m_linenum;
        if (!m_string.empty())      // if copying a string
            m_strview = m_string;   // we must reset the strview
        return *this;
    }


    std::string      m_string;      // can be used to store either a raw string
    std::string_view m_strview;     // or we store a string view, depends on constructor or setLine used
    int              m_linenum = 0;
};

// stores errors and warnings
class ErrorWarning
{
  public:

    ErrorWarning() = default;
    ErrorWarning(const InputLine& line, const std::string & errMsg, const std::string &warnMsg )
    {
        setError(line, errMsg);
        setWarning(line, warnMsg);
    }
    ~ErrorWarning() = default;

    // add in everything in rhs to us
    const ErrorWarning &operator += (const ErrorWarning& rhs)
    {
        m_errlines.insert(m_errlines.end(), rhs.m_errlines.begin(), rhs.m_errlines.end());
        m_errors.insert(m_errors.end(), rhs.m_errors.begin(), rhs.m_errors.end());
        m_warnlines.insert(m_warnlines.end(), rhs.m_warnlines.begin(), rhs.m_warnlines.end());
        m_warnings.insert(m_warnings.end(), rhs.m_warnings.begin(), rhs.m_warnings.end());
        return *this;
    }

    void clear()
    {
        m_errlines.clear();
        m_errors.clear();
        m_warnlines.clear();
        m_warnings.clear();
    }

    void setError(const InputLine & line, const std::string &errMsg)
    {
        if (!errMsg.empty())
        {
            m_errlines.push_back(line);
            m_errors.push_back(errMsg);
        }
    }

    void setWarning(const InputLine& line, const std::string& warnMsg)
    {
        if (!warnMsg.empty())
        {
            m_warnlines.push_back(line);
            m_warnings.push_back(warnMsg);
        }
    }

    const std::vector<InputLine>&   getErrLines() const { return m_errlines; }
    const std::vector<std::string>& getErrors()   const { return m_errors; }
    const std::vector<InputLine>&   getWarnLines() const { return m_warnlines; }
    const std::vector<std::string>& getWarnings()  const { return m_warnings; }


  protected:
    std::vector<InputLine>   m_errlines;
    std::vector<std::string> m_errors;
    std::vector<InputLine>   m_warnlines;
    std::vector<std::string> m_warnings;
};


// script file holds a complete script file with all embedded includes
class ScriptFile
{
public:
    class ScriptLine
    {
    public:
        ScriptLine() = default;
        ScriptLine(const InputLine& line) : m_line(line) {}
        ScriptLine(const InputLine& line, std::string_view fileName) : m_line(line), m_fileName(fileName) {}

        ~ScriptLine() = default;

        void setLine(const InputLine& line) { m_line = line; }
        void setFileName(std::string_view fileName) { m_fileName = fileName; }

        const InputLine& getInputLine() const { return m_line; }
        std::string_view getFileName() const { return m_fileName; }

    protected:
        InputLine        m_line;
        std::string_view m_fileName;        // filename associated with this line
    };

    ScriptFile() = default;
    ~ScriptFile() = default;

    ScriptFile& operator = (const ScriptFile& rhs)   // need custom = since the string views point into the strings saved in the set
    {
        // we have to change every string_view to point into our copy of the set
        m_fileNames = rhs.m_fileNames;   // copy the string storage
        m_lines = rhs.m_lines;

        // now we have to fix the string views in ScriptLine to point into our set and not the rhs set
        for (auto it = m_lines.begin(); it != m_lines.end(); it++)
        {
            auto itt = m_fileNames.find(std::string((*it).getFileName()));
            (*it).setFileName(std::string_view(*itt));        // make new string view pointing into our set
        }
        return *this;
    }
    int size() const { return (int)m_lines.size(); }

    const ErrorWarning& getError() const { return m_errors; }
    const std::vector<ScriptLine> & getLines() const { return m_lines; }

    void addLine(const InputLine& line, const std::string& fileName)
    {
        auto it = m_fileNames.find(fileName);
        if (it == m_fileNames.cend())
        {
            auto p = m_fileNames.insert(fileName);
            it = p.first;
        }
        m_lines.push_back(ScriptLine(line, std::string_view(*it)));
    }

    // used for writing back to map, get all of the input lines.
    std::vector<std::string> getWriteLines() const
    {
        std::vector<std::string> retLines;
        retLines.reserve(m_lines.size());       // estimated size if no embedded ones
        for (auto it = m_lines.cbegin(); it != m_lines.cend(); it++)
        {
            retLines.push_back(std::string((*it).getInputLine().getLine()));
        }
        return retLines;
    }

    // return 0 for no error, otherwise m_errors is filled in
    int readFile(const std::string& fileName, const std::string& incdirs, const std::string& outmap )
    {
        std::vector<std::filesystem::path> paths;
        std::unordered_set<std::string>    duplicates;  // we try and prevent same include multiple times.

        // look at the destination map. If it has a path, use that as the first path to try to resolve scripts
        if (!outmap.empty())
        {
            std::filesystem::path outpath = outmap;
            outpath.remove_filename();
            if (!outpath.empty())
                paths.push_back(outpath);
        }
        return ireadFile(fileName, incdirs, paths, InputLine(), 1, duplicates);
    }

protected:

    // this may be called recursively as we process #pragma include comments to include other script files
    int ireadFile(const std::string& fileName, const std::string& incdirs, std::vector<std::filesystem::path>& paths, const InputLine & ilineerr, int indent, std::unordered_set<std::string> &duplicates)
    {
        static const std::string_view sinclude = "#pragma include ";

        // find the file via our search system
        std::string fname = searchPaths(fileName, incdirs, paths);
        if (fname.empty())
        {
            m_errors.setError(ilineerr, "Unable to find script: " + fileName);
            return 1;
        }
        // if we have already included fname, don't do it again to prevent circular or duplicate reference. Log it as a warning
        // we only look at filename part and only use lower case for this check
        std::string nameonly = toLower(std::filesystem::path(fname).filename().string());
        if (duplicates.find(nameonly) != duplicates.cend())
        {
            m_errors.setWarning(ilineerr, "Duplicate or circular include ignored: " + nameonly);
            return 0;   // ignore it, but no error
        }
        duplicates.insert(nameonly);

        std::filesystem::path fpath = fname;
        fpath.remove_filename();
        if (!fpath.empty())       // only add if it is unique
        {
            bool bFound = false;
            for (std::size_t i = 0; i < paths.size(); i++)
            {
                if (paths[i] == fpath)
                {
                    bFound = true;
                    break;
                }
            }
            if (!bFound)
                paths.push_back(fpath);   // add to list of paths found
        }
        FILE* fp = nullptr;
        errno_t error = fopen_s(&fp, fname.c_str(), "rt");
        if (error || !fp)
        {
            m_errors.setError(ilineerr, "error " + std::to_string(error) + " opening script: " + fname);
            return 1;
        }
        for (int i = 0; i < indent; i++) printf("  ");
        printf("Reading script file: %s\n", fname.c_str());

        bool bUTF8BOM = false;
        bool bUTF16LE = false;
        bool bUnicode = false;
        getFileEncoding(fp, bUTF8BOM, bUTF16LE, bUnicode);

        int linenum = 0;
        InputLine iline;
        std::string line;
        std::string includeFname;
        for (; !feof(fp) && !error;)
        {
            linenum++;
            if (bUnicode)
                line = readLineUTF16(fp, bUTF16LE);
            else
                line = readLineUTF8(fp);

            iline.setLine(line, linenum);
            // here we need to check for #pragma include. if we find it, change it to be ##pragma include so it does not get processed again.
            // and then take the value after the include, treat it as a string and spawn this recursivly
            if (!line.empty())
            {
                std::string_view stline = line;
                if (sinclude == toLower(stline.substr(0, sinclude.size())))  // we have the pragma to include another script
                {
                    std::size_t spos = sinclude.size();
                    for (; spos < stline.size() && isspace(stline[spos]); spos++);  // skip leading space
                    if (spos < stline.size())
                    {
                        std::size_t epos = spos + 1;
                        if (stline[spos] == '"')
                        {
                            spos = epos;
                            for (; (epos < stline.size()) && (stline[epos] != '"'); epos++);
                            if (epos >= stline.size())
                                spos = epos;   // prevent any string - missing ending "
                        }
                        else
                        {
                            for (; (epos < stline.size()) && !isspace(stline[epos]); epos++);
                        }
                        if (spos < stline.size())
                            includeFname = stline.substr(spos, epos - spos);
                    }
                    if (includeFname.empty())
                    {
                        m_errors.setError(iline, "\"" + fname + "\": Invalid pragma include, missing or invalid filename");
                        error = 1;
                    }
                    else
                    {
                        addLine(InputLine('#' + line, linenum), fname);
                        error = ireadFile(includeFname, incdirs, paths, iline, indent+1, duplicates);
                    }
                    continue;
                }
            }
            addLine(iline, fname);
        }
        fclose(fp);
        if (!error)
        {
            for (int i = 0; i < indent; i++) printf("  ");
            printf("Read %d lines from: %s\n", linenum, fname.c_str());
        }
        return error;
    }

    std::unordered_set<std::string> m_fileNames;   // storage for all embedded file names since lines hold string_views. These are the names used to open the file.
    std::vector<ScriptLine>         m_lines;       // all the lines it holds, every line holds a str_view to the filename it is associated with.
    ErrorWarning                    m_errors;
};



// section contains the name and all of the strings that make up the section.
// strings are copies of the ones in the map since we have the ability to modify them
class MapSection
{
public:
    // for info section, we break it into key:value pairs
    class infoSection
    {
    public:

        infoSection() = default;
        ~infoSection() = default;

        // call this for every line in section. if they are key:value pairs, it gets added
        void addLine(const InputLine & line, ErrorWarning & errWarning)
        {
            std::string::size_type pos = line.getLine().find_first_of(':');
            if (pos != std::string::npos)
            {
                std::string key = toLower(line.getLine().substr(0, pos));  // own copy of lower case name
                std::string_view value;   
                if (pos < (line.getLine().size() - 1))
                    value = line.getLine().substr(pos + 1);
                if (isValidVarName(key))
                {
                    m_options.push_back(keyValue(key, value, line));
                }
                else
                    errWarning.setError(line, "info section. Invalid key name: " + key);
            }
            else
                errWarning.setError(line, "info section. Missing colon");
        }

        // return strings for all keys to write
        std::vector<std::string> makeLines() const
        {
            std::vector<std::string> output;
            for (std::size_t i = 0; i < m_options.size(); i++)
            {
                std::string retLine;
                retLine.reserve(m_options[i].getKey().size() + m_options[i].getValue().size() + 2);
                retLine = m_options[i].getKey();
                retLine += ":";
                retLine += m_options[i].getValue();
                output.push_back(retLine);
            }
            return output;
        }

        // if key does not exist you get a null string
        std::string_view getValue(std::string_view key) const
        {
            for (std::size_t i = 0; i < m_options.size(); i++)
            {
                if (key == m_options[i].getKey())
                {
                    return m_options[i].getValue();
                }
            }
            return std::string_view();
        }

        // change the value for the key, or add a new keyvalue 
        void setKeyValue(std::string_view key, std::string_view value)
        {
            for (std::size_t i = 0; i < m_options.size(); i++)
            {
                if (key == m_options[i].getKey())
                {
                    m_options[i].setValue(value);
                    return;
                }
            }
            // add a new key/value pair. There is no source input line associated with it.
            m_options.push_back(keyValue(key, value, InputLine()));
        }

        InputLine getInputLine(std::string_view key) const
        {
            for (std::size_t i = 0; i < m_options.size(); i++)
            {
                if (key == m_options[i].getKey())
                {

                    return m_options[i].getLine();
                }
            }
            return InputLine();  // key not found, return empty
        }


    protected:

        // key:value storage  Used for info. We own the strings we store
        class keyValue
        {
        public:
            keyValue() = default;
            keyValue(std::string_view key, std::string_view value, const InputLine & line) { setKeyValue(key, value, line); }
            ~keyValue() = default;

            void setKeyValue(std::string_view key, std::string_view value, const InputLine & line)
            {
                m_key   = key;        // make full copy of strings
                m_value = value;
                m_line  = line;
            }
            void setValue(std::string_view value) { m_value = value; }  // make full copy
            std::string_view getKey()   const { return m_key; }
            std::string_view getValue() const { return m_value; } 
            const InputLine &getLine()  const { return m_line; }      // used in case of some sort of value error so we can show incorrect line that generated it.
        protected:
            std::string m_key;      // own the key string
            std::string m_value;    // own the value string
            InputLine   m_line;     // input line so we can display correct error messages
        };

        std::vector<keyValue>    m_options;    // parsed key/value pairs
    };

    // used for tiles, height, and resources
    class arrayItem
    {
    public:
        arrayItem() = default;
        arrayItem(int width, int height) { setSize(width, height); }
        ~arrayItem() = default;

        bool isValid() const
        {
            return ((std::size_t)m_width * m_height) == m_data.size();
        }

        void setSize(int width, int height)
        {
            m_height = height;
            m_width = width;
            m_data.clear();
            m_data.reserve((std::size_t)width * height);
        }

        void resize(int width, int height, int defvalue)  // resize to use this new width/height.
        {
            std::vector<int> data;      // new data
            data.reserve((std::size_t)width * height);
            for (std::size_t row = 0; row < height; row++)
            {
                for (std::size_t col = 0; col < width; col++)
                {
                    int val = ((row < m_height) && (col < m_width)) ?
                        m_data[row * m_width + col] : defvalue;
                    data.push_back(val);
                }
            }
            m_data   = data;    // replace old with this new data for this new size
            m_height = height;  // save the new array dimensions
            m_width  = width;
        }

        // merge in data from src using the given offset, clipping to our boundary - it does not grow the data
        // negative offsets will clip src
        // be careful, offsets can be negative don't use std::size_t since its unsigned
        void merge(arrayItem const &src, int srow, int scol, int erow, int ecol, int rowOffset, int colOffset )  // merge in values from src that are in range
        {
            for (intmax_t row = srow; row <= erow; row++)
            {
                for (intmax_t col = scol; col <= ecol; col++)
                {
                    if (((row + rowOffset) >= 0) && ((row + rowOffset) < m_height) && ((col + colOffset) >= 0) && ((col + colOffset) < m_width))
                    {
                        int val = src.m_data[row * src.m_width + col];
                        m_data[(row + rowOffset) * m_width + (col + colOffset)] = val;
                    }
                }
            }
        }

        // if error, no longer abort. Instead log it and use default value
        void addLine(const InputLine & iline, bool bAllowNeg, ErrorWarning & errorWarning, const std::string &errwarnpre, bool bFix, int defValue)
        {
            bool bInvalidInt = false;
            std::vector<int> lineValues;
            lineValues.reserve(m_width + 16);    // we need at least m_width but we see maps with 2 extra, so give us a bit of extra space for performance

            std::size_t spos = 0;
            std::size_t epos = 0;

            // parse every int into lineValues for this line. If not enough, generate error. If to many, generate warning
            for (; (epos = iline.getLine().find(',', spos)) != std::string::npos;)
            {
                std::string_view val = iline.getLine().substr(spos, epos - spos);
                int ival = defValue;
                if (val.empty() || !isInteger(val, bAllowNeg))
                {
                    if (!bInvalidInt)   // only log first one
                    {
                        bInvalidInt = true;
                        std::string msg = errwarnpre + " column: " + std::to_string(spos) + " invalid integer: " + std::string(val) + " using: " + std::to_string(defValue);
                        bFix ? errorWarning.setWarning(iline, msg) : errorWarning.setError(iline, msg);
                    }
                }
                else
                    ival = stoi(val);

                lineValues.push_back(stoi(val));
                spos = epos + 1;        // beginning of next value
            }
            if (m_data.size() < ((std::size_t)m_width * m_height))  // have room for more
            {
                if (lineValues.size() < m_width)
                {
                    std::string msg = errwarnpre + " not enough columns. Found " + std::to_string(lineValues.size()) + " expecting " + std::to_string(m_width) + " Filling with: " + std::to_string(defValue);
                    bFix ? errorWarning.setWarning(iline, msg) : errorWarning.setError(iline, msg);
                }
                for (std::size_t i = 0; i < m_width; i++)
                {
                    int ival = (i < lineValues.size()) ? lineValues[i] : defValue;
                    m_data.push_back(ival);
                }
                if ((lineValues.size() > m_width) && !m_bWarnCol) 
                {
                    errorWarning.setWarning(iline, errwarnpre + "too many columns. Found " + std::to_string(lineValues.size()) + " expecting " + std::to_string(m_width) + " ignoring extra");
                    m_bWarnCol = true;
                }
            }
            else if (!m_bWarnRow)  //
            {
                m_bWarnRow = true;
                errorWarning.setWarning(iline, errwarnpre + "too many rows. Only expecting " + std::to_string(m_height) + " ignoring extra");
            }
        }

        // called after all lines are sent, generate warning if not enough rows. We will fill in with default value
        void enoughRows(const InputLine& iline, ErrorWarning& errorWarning, const std::string& errwarnpre, bool bFix, int defValue)
        {
            std::size_t arrSize = (std::size_t)m_width * m_height;
            if (m_data.size() < arrSize)
            {
                std::string msg = errwarnpre + "not enough rows. Found " + std::to_string(m_data.size() / m_width) + " expecting " + std::to_string(m_height) + ". Filling with: " + std::to_string(defValue);
                bFix ? errorWarning.setWarning(iline, msg) : errorWarning.setError(iline, msg);

                for (std::size_t i = m_data.size(); i < arrSize; i++)
                    m_data.push_back(defValue);
            }
        }

        // returning back array of strings for output
        std::vector<std::string> makeLines() const
        {
            std::vector<std::string> output;
            std::string line;
            line.reserve((std::size_t)m_width * 6);    // help prevent reallocations
            for (std::size_t row = 0; row < m_height; row++)
            {
                line.clear();
                for (std::size_t col = 0; col < m_width; col++)
                {
                    std::size_t index = (row * m_width) + col;
                    line += std::to_string(m_data[index]);
                    line += ",";
                }
                output.push_back(line);
            }
            return output;   // return array of lines for output
        }

        void clear(int defValue)
        {
            m_data.clear();     // erase everything
            m_data.reserve((std::size_t)m_height * m_width);  // have enough room
            for (std::size_t row = 0; row < m_height; row++)
            {
                for (std::size_t col = 0; col < m_width; col++)
                {
                    m_data.push_back(defValue);
                }
            }
        }

        // used for tiles to ensure we have the proper borders
        // used to force border heights to default value
        void setBorders(int defValue)
        {
            for (std::size_t col = 0; col < m_width; col++)
            {
                m_data[col] = defValue;                                         // top row
                m_data[((std::size_t)m_height-1) * m_width + col] = defValue;   // bottom row
            }

            for (std::size_t row = 0; row < m_height; row++)
            {
                m_data[row * m_width ] = defValue;              // left column
                m_data[row * m_width + m_width-1] = defValue;   // right column
            }
        }

        void flattenHigh(int testVal, int newValue)  // change all values above this to given value
        {
            for (std::size_t row = 0; row < m_height; row++)
            {
                for (std::size_t col = 0; col < m_width; col++)
                {
                    std::size_t index = (row * m_width) + col;
                    if (m_data[index] > testVal)
                        m_data[index] = newValue;
                }
            }
        }

        void flattenLow(int testVal, int newValue)  // change all values below this to given value
        {
            for (std::size_t row = 0; row < m_height; row++)
            {
                for (std::size_t col = 0; col < m_width; col++)
                {
                    std::size_t index = (row * m_width) + col;
                    if (m_data[index] < testVal)
                        m_data[index] = newValue;
                }
            }
        }

        void flattenBetween(int testLow, int testHigh, int newValue)  // change all values testLow <= value <=testHigh to newValue
        {
            for (std::size_t row = 0; row < m_height; row++)
            {
                for (std::size_t col = 0; col < m_width; col++)
                {
                    std::size_t index = (row * m_width) + col;
                    int val = m_data[index];
                    if ((val >= testLow) && (val <= testHigh))
                        m_data[index] = newValue;
                }
            }
        }



    protected:
        std::vector<int> m_data;
        int       m_height=0;
        int       m_width=0;
        bool      m_bWarnCol = false;
        bool      m_bWarnRow = false;
    };


    MapSection() = default;
    MapSection(std::string_view str) { m_name = str; }  // only reason for this is hash comparision
    ~MapSection() = default;

    std::string_view name() const { return m_name; }

    // add until we have an error or a completed section. Ignore any starting blank lines,
    // sections start with a name{ and end with a final }.
    // Both start and end must be at beginning of line
    void addLine(InputLine const& line)
    {
        if (m_state == eStateWaitingforName)   // skip any blank lines
        {
            if (isEmptyStr(line.getLine()))
                return;
            std::size_t pos = line.getLine().find('{');
            if (pos > 0 && pos != (std::string::npos))
            {
                m_nameLine = line;  // save line for open {
                m_name = toLower(line.getLine().substr(0, pos));  // save copy of the lower case name
                if (isValidVarName(m_name))
                {
                    m_state = eStateHaveName;
                }
                else
                {
                    m_errWarn.setError(line, "invalid section name: " + m_name);
                    m_state = eStateError;
                }
            }
            else
            {
                m_errWarn.setError( line, "invalid section start, missing {");
                m_state = eStateError;
            }
        }
        else if (m_state == eStateHaveName)  // have the name, waiting for final }
        {
            if (!line.getLine().empty() && ('}' == line.getLine()[0]))
            {
                m_closeLine = line;
                m_state = eStateEnd;        // all done
            }
            else
            {
                m_data.push_back(line);
            }
        }
    }

    // verify that we had a final closing }
    void verifyClosed()
    {
        if (m_state == eStateHaveName)
        {
            m_errWarn.setError(m_data.back(), "section: " + m_name + " missing closing }");
            m_state = eStateError;
        }
    }

    bool valid() const { return m_state == eStateEnd; }
    bool done() const  { return (m_state == eStateEnd) || (m_state == eStateError); }
    bool empty() const { return m_data.empty(); }
    int length() const { return (int)m_data.size(); }
    
    const ErrorWarning &getError() const { return m_errWarn;  }

    const InputLine &openLine() const { return m_nameLine; }     // line that started section
    const InputLine &closeLine() const { return m_closeLine; }   // line that ended section

    const std::vector<InputLine>& getLines() const { return m_data; }  // by default, we return what we stored.
    const std::vector<std::string> getWriteLines() const
    {
        std::vector<std::string> output;
        output.reserve(m_data.size());
        for (int i = 0; i < m_data.size(); i++)
            output.push_back(std::string(m_data[i].getLine()));
        return output;
    }

    // std::unordered_set hash and comparision methods only
    class compHash
    {
    public:
        // comparision operator for hash. Only on the name
        bool operator() (const MapSection& lhs, const MapSection& rhs) const
        {
            return lhs.getName() == rhs.getName();
        }

        // hash operator - only on the name field
        std::size_t operator()(const MapSection& s)const noexcept
        {
            return std::hash<std::string>{}(s.getName());
        }
    };

    const std::string& getName() const { return m_name; }


protected:


    enum stateMachine
    {
        eStateWaitingforName = 0,  // init, have not seen name. Will stay here if empty initial lines.
        eStateHaveName,            // we have name{ and waiting for ending }
        eStateEnd,                 // we have ending } and are done.
        eStateError,               // error processing. name without {
    };
    ErrorWarning            m_errWarn;
    std::string             m_name;       // name converted to lower case
    std::vector<InputLine>  m_data;	      // lines read in for interior of section
    InputLine               m_nameLine;   // this is the line that contained the name and open {
    InputLine               m_closeLine;  // this is the line that contained the closing }
    stateMachine            m_state = eStateWaitingforName;
};


class RRMap
{
  public:
    RRMap() = default;
    ~RRMap() = default;

    void copyFrom(const RRMap& rhs, const CommandLineOptions& options)
    {
        // first do a copy.
        copy(rhs);      // complete copy

        // now process any resize on tiles, height, crystals, ore
        resize(options.m_nRowResize, options.m_nColResize, options.m_nDefTileID, options.m_nDefHeight, options.m_nDefCrystal, options.m_nDefOre);

        // now clear out the tiles, height, crystals and ore. Doing this since we will merge below
        m_tileSection.clear  (options.m_nDefTileID);
        m_heightSection.clear(options.m_nDefHeight);
        m_crystals.clear     (options.m_nDefCrystal);
        m_ore.clear          (options.m_nDefOre);

        // now merge from source applying any offset
        m_tileSection.merge(rhs.m_tileSection, options.m_srow, options.m_scol, options.m_erow, options.m_ecol, options.m_nOffsetCol, options.m_nOffsetRow);
        m_heightSection.merge(rhs.m_heightSection, options.m_srow, options.m_scol, options.m_erow, options.m_ecol, options.m_nOffsetCol, options.m_nOffsetRow);
        m_crystals.merge(rhs.m_crystals, options.m_srow, options.m_scol, options.m_erow, options.m_ecol, options.m_nOffsetCol, options.m_nOffsetRow);
        m_ore.merge(rhs.m_ore, options.m_srow, options.m_scol, options.m_erow, options.m_ecol, options.m_nOffsetCol, options.m_nOffsetRow);

    }

    bool valid() const { return m_bValid; }
    const ErrorWarning & getError() const { return m_error; }

    const ScriptFile& getScript() const { return m_script; }

    int height() const { return m_Height; }
    int width() const { return m_Width; }
    int lines() const { return m_numLines; }

    bool getUTF8BOM() const { return m_bUTF8BOM; }
    bool getUnicodeLE() const { return m_bUTF16LE; }
    bool getUnicode() const { return m_bUnicode; }

    void mergeTiles(const RRMap& src, int srow, int scol, int erow, int ecol, int rowOffset, int colOffset)   // will copy tiles and height into map
    {
        m_tileSection.merge(src.m_tileSection, srow, scol, erow, ecol, rowOffset, colOffset);
        m_tileSection.setBorders(38);    // make sure the border is properly defined  38 is solid rock regular
    }

    void mergeHeight(const RRMap& src, int srow, int scol, int erow, int ecol, int rowOffset, int colOffset)   // will copy tiles and height into map
    {
        m_heightSection.merge(src.m_heightSection, srow, scol, erow, ecol, rowOffset, colOffset);
    }

    void mergeCrystal(const RRMap& src, int srow, int scol, int erow, int ecol, int rowOffset, int colOffset)   // will copy tiles and height into map
    {
        m_crystals.merge(src.m_crystals, srow, scol, erow, ecol, rowOffset, colOffset);
    }

    void mergeOre(const RRMap & src, int srow, int scol, int erow, int ecol, int rowOffset, int colOffset)   // will copy tiles and height into map
    {
        m_ore.merge(src.m_ore, srow, scol, erow, ecol, rowOffset, colOffset);
    }

        // resize to given height and width. If 0, use existing value. Modify tiles, height, crystals, ore and map size in info section
    void resize(int rowSize, int colSize, int defTile, int defHeight, int defCrystal, int defOre )
    {
        if (!rowSize)
            rowSize = m_Height;
        if (!colSize)
            colSize = m_Width;
        m_tileSection.resize(colSize, rowSize, defTile);
        m_heightSection.resize(colSize+1, rowSize+1, defHeight);  // heights have corners so 1 extra
        m_crystals.resize(colSize, rowSize, defCrystal);
        m_ore.resize(colSize, rowSize, defOre);

        m_tileSection.setBorders(38);    // make sure the tile border is properly defined  38 is solid rock regular

        // update map size in info
        m_Height = rowSize;
        m_Width  = colSize;
        m_infoSection.setKeyValue(scolcount, std::to_string(colSize));
        m_infoSection.setKeyValue(srowcount, std::to_string(rowSize));
    }

    void flattenHeightHigh(int highval, int val)
    {
        m_heightSection.flattenHigh(highval, val);
    }
    
    void flattenHeightLow(int lowval, int val)
    {
        m_heightSection.flattenLow(lowval, val);
    }
    
    void flattenHeightBetween(int low, int high, int val)
    {
        m_heightSection.flattenBetween(low, high, val);
    }
    
    void borderHeight(int val)
    {
        m_heightSection.setBorders(val);
    }

    void setMapName(const std::string& name)
    {
        m_infoSection.setKeyValue(slevelname, name);
    }

    void setMapCreator(const std::string& creator)
    {
        m_infoSection.setKeyValue(screator, creator);
    }

    int writeMap(FILE* fp)
    {
        int err = 0;
        for (auto it = m_mapOrder.cbegin(); it != m_mapOrder.cend() && (err == 0); it++)
        {
            // start off each section with name{
            if (fprintf(fp, "%s{\n", std::string((*it).getName()).c_str()) < 0)
            {
                err = ferror(fp);
                break;
            }
            MapSection* ms = nullptr;
            switch ((*it).getID())
            {
            case 1:     // info,
                err = writeArray(fp, m_infoSection.makeLines());
                break;

            case 2:     // tiles
                err = writeArray(fp, m_tileSection.makeLines());
                break;

            case 3:     // height
                err = writeArray(fp, m_heightSection.makeLines());
                break;

            case 4:     // resources
                if (fprintf(fp, "crystals:\n") < 0)
                {
                    err = ferror(fp);
                    break;
                }
                err = writeArray(fp, m_crystals.makeLines());
                if (err)
                    break;
                if (fprintf(fp, "ore:\n") < 0)
                {
                    err = ferror(fp);
                    break;
                }
                err = writeArray(fp, m_ore.makeLines());
                break;

            case 16:    // script  TODO once we start changing script

                err = writeArray(fp, m_script.getWriteLines());
                break;

             // we do nothing for these, just read in and write out as is. For new map, we write empty sections
            case 0:     // comments,
            case 5:     // sobjectives
            case 6:     // buildings
            case 7:     // landslidefrequency
            case 8:     // lavaspread
            case 9:     // miners
            case 10:    // briefing
            case 11:    // briefingsuccess
            case 12:    // briefingfailure
            case 13:    // vehicles
            case 14:    // creatures
            case 15:    // blocks
                ms = findSection((*it).getName());
                if (ms)
                {
                    err = writeArray( fp, ms->getLines() );  // by default, we return what we stored.
                }
                break;
            }
            if (err == 0)
            {
                // every section ends with a single }
                if (fprintf(fp, "}\n") < 0)
                {
                    err = ferror(fp);
                }
            }
        }
        return err;
    }

    void getEncoding( FILE *fp )  // get encoding flags
    {
        getFileEncoding(fp, m_bUTF8BOM, m_bUTF16LE, m_bUnicode);
    }

    // read in every line of the map, we will process it later
    void readMap(FILE* fp, const CommandLineOptions &options )
    {
        InputLine iline;
        std::string line;

        for (; !feof(fp);)
        {
            m_numLines++;
            if (m_bUnicode)
                iline.setLine(readLineUTF16(fp, m_bUTF16LE), m_numLines);
            else
                iline.setLine(readLineUTF8(fp), m_numLines);
                
            m_lines.push_back(iline);
        }
        processMap(options);
    }

    const MapSection* findSection(const std::string & str) const
    {
        MapSection ms(str);
        MapSectionSet::const_iterator it = m_sections.find(ms);
        if (it != m_sections.cend())
        {
            return &(*it);
        }
        return nullptr;
    }

    MapSection* findSection(std::string_view str)
    {
        MapSection ms(str);
        MapSectionSet::iterator it = m_sections.find(ms);
        if (it != m_sections.end())
        {
            return const_cast<MapSection *>(& (*it));  // not sure why this is const from iterator...
        }
        return nullptr;
    }

    // used when we read a script from external file
    void replaceScript(const ScriptFile& sf)
    {
        m_script = sf;
    }


  protected:

    int writeArray(FILE *fp, const std::vector<std::string>& lines)
    {
        int err = 0;
        for (auto it = lines.cbegin(); it != lines.cend(); it++)
        {
            if (fprintf(fp, "%s\n", (*it).c_str()) < 0)
            {
                err = ferror(fp);
                break;
            }
        }
        return err;
    }

    // for sections we don't do anything special, we can just output the InputLines
    int writeArray(FILE* fp, const std::vector<InputLine> lines)
    {
        int err = 0;
        for (auto it = lines.cbegin(); it != lines.cend(); it++)
        {
            if (fprintf(fp, "%s\n", std::string((*it).getLine()).c_str()) < 0)
            {
                err = ferror(fp);
                break;
            }
        }
        return err;
    }


    // entire file is read in, now break it into the sections
    void processMap(const CommandLineOptions &options)
    {
        m_sections.reserve(32);     // more than needed
        m_sections.clear();         // should already be clear

        m_bValid = true;	// assume it is valid
        for (auto it = m_lines.cbegin(); m_bValid && (it != m_lines.cend());)  // process all lines
        {
            // now process lines for this section
            MapSection section;  // start a section and add lines to it until section is done
            bool bEmpty = true;
            InputLine iline;
            for (; !section.done() && it != m_lines.cend();)   // add lines to section until it is done
            {
                iline.setLine((*it).getLine(), (*it).getLineNum());  // this is now a string_view to the master saved line
                if (!isEmptyStr(iline.getLine()))
                    bEmpty = false;
                section.addLine(iline);
                it++;
            }
            section.verifyClosed();
            m_bValid = section.valid();
            if (m_bValid)
            {
                auto itt = m_sections.find(section);  // name must not already exist
                if (itt == m_sections.cend())
                {
                    m_sections.insert(section);
                }
                else
                {
                    m_bValid = false;
                    m_error.setError(iline, "section: " + std::string(section.name()) + " duplicate section name");
                    return;
                }
            }
            m_error += section.getError();   // copy any generated errors or warnings

            if (bEmpty)  // read last empty lines - nothing, all done
            {
                m_bValid = true;
                break;
            }
        }
        if (!m_bValid)
            return;

        // make sure all the sections exist, and process those that we do stuff with
        for (std::size_t i = 0; i < m_mapOrder.size(); i++)
        {
            if (findSection(m_mapOrder[i].getName()))
            {
                switch (m_mapOrder[i].getID())
                {
                case 1:     // info,
                    processInfo();
                    break;

                case 2:     // tiles
                    processTiles(options);
                    break;

                case 3:     // height
                    processHeight(options);
                    break;

                case 4:     // resources
                    processResources(options);
                    break;

                case 16:    // script
                    processScript();
                    break;

                // we do nothing for these, just read in and write out as is. If the sections were missing (older maps), we write empty ones.
                case 0:     // comments,
                case 5:     // sobjectives
                case 6:     // buildings
                case 7:     // landslidefrequency
                case 8:     // lavaspread
                case 9:     // miners
                case 10:    // briefing
                case 11:    // briefingsuccess
                case 12:    // briefingfailure
                case 13:    // vehicles
                case 14:    // creatures
                case 15:    // blocks
                    break;
                }
                if (!m_bValid)
                    return;
            }
            else
            {
                if (m_mapOrder[i].getOptional() == false)
                {
                    m_error.setError(m_lines.back(), "required section missing: " + std::string(m_mapOrder[i].getName()));
                    m_bValid = false;
                    return;
                }
            }
        }
        // one final check. Lets see if there are sections we know nothing about - treat them as a warning
        for (auto it = m_sections.cbegin(); it != m_sections.cend(); it++)
        {
            bool bFound = false;
            for (auto sit = m_mapOrder.cbegin(); sit != m_mapOrder.cend(); sit++)
            {
                if ((*sit).getName() == (*it).name())
                {
                    bFound = true;
                    break;
                }
            }
            if (!bFound)
            {
                m_error.setWarning((*it).openLine(), "unknown section name: " + std::string((*it).name()));
            }
        }

    }

    // get info into key:value pairs, look for the rowcount and colcount fields and get their values
    void processInfo()
    {
        // working on info section only
        MapSection* ms = findSection(sinfo);
        if (ms)
        {
            std::size_t len = ms->getLines().size();
            for (std::size_t i = 0; i < len; i++)
            {
                m_infoSection.addLine(ms->getLines()[i], m_error);
                if (!m_error.getErrors().empty())
                {
                    m_bValid = false;
                    return;
                }
            }
            // info section loaded - now we can get values
            std::string_view value = m_infoSection.getValue(srowcount);
            if (!value.empty() && isInteger(value, false))
            {
                m_Height = stoi(value);
            }
            else if (!value.empty())
            {
                m_error.setError(ms->getLines().back(), "section info. Missing " + std::string(srowcount));
                m_bValid = false;
                return;
            }
            else
            {
                m_error.setError(m_infoSection.getInputLine(srowcount), "section info. Invalid integer rowcount:" + std::string(value));
                m_bValid = false;
                return;
            }
            value = m_infoSection.getValue(scolcount);
            if (!value.empty() && isInteger(value, false))
            {
                m_Width = stoi(value);
            }
            else if (!value.empty())
            {
                m_error.setError(ms->getLines().back(), "section info. Missing " + std::string(scolcount));
                m_bValid = false;
                return;
            }
            else
            {
                m_error.setError(m_infoSection.getInputLine(srowcount), "section info. Invalid integer colcount:" + std::string(value));
                m_bValid = false;
                return;
            }
        }
    }

    void processTiles(const CommandLineOptions &options)
    {
        MapSection* ms = findSection(stiles);
        if (ms)
        {
            m_tileSection.setSize(m_Width, m_Height);
            std::size_t len = ms->getLines().size();
            for (std::size_t i = 0; i < len; i++)
            {
                m_tileSection.addLine(ms->getLines()[i], false, m_error, "section tiles ", options.m_bFix, options.m_nDefTileID);
                if (!m_error.getErrors().empty())
                {
                    m_bValid = false;
                    return;
                }
            }
            m_tileSection.enoughRows(ms->getLines().back(), m_error, "section tiles ", options.m_bFix, options.m_nDefTileID);
        }
    }

    void processHeight(const CommandLineOptions& options)
    {
        MapSection* ms = findSection(sheight);
        if (ms)
        {
            m_heightSection.setSize(m_Width+1, m_Height+1);  // height needs extra row/col since we have height per cell corner.
            std::size_t len = ms->getLines().size();
            for (std::size_t i = 0; i < len; i++)
            {
                m_heightSection.addLine(ms->getLines()[i], true, m_error,"section height ",options.m_bFix, options.m_nDefHeight);
                if (!m_error.getErrors().empty())
                {
                    m_bValid = false;
                    return;
                }
            }
            m_heightSection.enoughRows(ms->getLines().back(), m_error, "section height ", options.m_bFix, options.m_nDefHeight);
        }
    }

    void processResources(const CommandLineOptions& options)
    {
        MapSection* ms = findSection(sresources);
        if (ms)
        {
            std::string_view sres;
            bool bOre = false;
            bool bCrystals = false;
            m_crystals.setSize(m_Width, m_Height);
            m_ore.setSize(m_Width, m_Height);
            int filling = 0;
            for (auto it = ms->getLines().cbegin(); it != ms->getLines().cend(); it++)
            {
                if (toLower((*it).getLine()) == scrystals)
                {
                    filling = 1;
                    if (bCrystals)
                    {
                        m_error.setError(*it, "section resources, duplicate crystals:");
                        m_bValid = false;
                        return;
                    }
                    bCrystals = true;
                }
                else if (toLower((*it).getLine()) == sore)
                {
                    filling = 2;
                    if (bOre)
                    {
                        m_error.setError(*it, "section resources, duplicate ore:");
                    }
                    bOre = true;
                }
                else if (filling == 1) // processing crystals
                {
                    m_crystals.addLine(*it, false, m_error, "section resources, crystals ", options.m_bFix, options.m_nDefCrystal);
                    if (!m_error.getErrors().empty())
                    {
                        m_bValid = false;
                        return;
                    }
                }
                else if (filling == 2) // processing ore
                {
                    m_ore.addLine(*it, false, m_error, "section resources, ore ", options.m_bFix, options.m_nDefOre);
                    if (!m_error.getErrors().empty())
                    {
                        m_bValid = false;
                        return;
                    }
                }
                else  // unknown data
                {
                    m_error.setError(*it, "section resources, not in ore or crystals data");
                }
            }
            if (!bCrystals)
            {
                m_error.setError(ms->closeLine(), "section resource missing crystals");
                m_bValid = false;
                return;
            }
            if (!bOre)
            {
                m_error.setError(ms->closeLine(), "section resource missing ore");
                m_bValid = false;
                return;
            }
            m_crystals.enoughRows(ms->getLines().back(), m_error, "section resources, crystals ", options.m_bFix, options.m_nDefCrystal);
            m_ore.enoughRows(ms->getLines().back(), m_error, "section resources, ore ", options.m_bFix, options.m_nDefOre);

        }
    }

    void processScript()
    {
        MapSection* ms = findSection(sscript);
        if (ms)
        {
            std::size_t len = ms->getLines().size();
            for (std::size_t i = 0; i < len; i++)
            {
                m_script.addLine(ms->getLines()[i], std::string() );  // put all the script lines into script section
            }
        }
    }

  private:
    RRMap& copy(const RRMap& rhs)  // copy everything except the const strings which are auto initialized
    {
        m_lines         = rhs.m_lines;
        m_sections      = rhs.m_sections;
        m_infoSection   = rhs.m_infoSection;
        m_tileSection   = rhs.m_tileSection;      // array of tiles
        m_heightSection = rhs.m_heightSection;    // array of heights
        m_crystals      = rhs.m_crystals;         // array of crystals
        m_ore           = rhs.m_ore;              // array of crystals
        m_script        = rhs.m_script;

        m_error    = rhs.m_error;
        m_Width    = rhs.m_Width;
        m_Height   = rhs.m_Height;
        m_numLines = rhs.m_numLines;

        m_bValid   = rhs.m_bValid;
        m_bUTF8BOM = rhs.m_bUTF8BOM;
        m_bUTF16LE = rhs.m_bUTF16LE;
        m_bUnicode = rhs.m_bUnicode;
        return *this;
    }
      
    typedef std::unordered_set<MapSection, MapSection::compHash, MapSection::compHash> MapSectionSet;

    std::vector<InputLine>         m_lines;            // base lines read in
    MapSectionSet                  m_sections;         // sections - in a map for faster lookup
    MapSection::infoSection        m_infoSection;      // for info section, this is the parsed key:value pairs
    MapSection::arrayItem          m_tileSection;      // array of tiles
    MapSection::arrayItem          m_heightSection;    // array of heights
    MapSection::arrayItem          m_crystals;         // array of crystals
    MapSection::arrayItem          m_ore;              // array of crystals
    ScriptFile                     m_script;           // save script into this for future processing and external files

    ErrorWarning                   m_error;
    int m_Width  = 0;
    int m_Height = 0;
    int m_numLines = 0;

    bool  m_bValid = false;
    bool  m_bUTF8BOM = false;
    bool  m_bUTF16LE = false;
    bool  m_bUnicode = false;

    // commonly used strings 
    const std::string_view scolcount  = "colcount";
    const std::string_view srowcount  = "rowcount";
    const std::string_view screator   = "creator";
    const std::string_view slevelname = "levelname";


    const std::string_view scrystals  = "crystals:";
    const std::string_view sore       = "ore:";

    // all of the sections
    const std::string_view scomments           = "comments";
    const std::string_view sinfo               = "info";
    const std::string_view stiles              = "tiles";
    const std::string_view sheight             = "height";
    const std::string_view sresources          = "resources";
    const std::string_view sobjectives         = "objectives";
    const std::string_view sbuildings          = "buildings";
    const std::string_view slandslidefrequency = "landslidefrequency";
    const std::string_view slavaspread         = "lavaspread";
    const std::string_view sminers             = "miners";
    const std::string_view sbriefing           = "briefing";
    const std::string_view sbriefingsuccess    = "briefingsuccess";
    const std::string_view sbriefingfailure    = "briefingfailure";
    const std::string_view svehicles           = "vehicles";
    const std::string_view screatures          = "creatures";
    const std::string_view sblocks             = "blocks";
    const std::string_view sscript             = "script";

    // order of sections in a map

    class mapOrder
    {
      public:
          mapOrder(std::string_view name, int id, bool optional) : m_name(name), m_id(id), m_optional(optional) {}
          ~mapOrder() {}

          std::string_view getName()     const { return m_name; }
          int              getID()       const { return m_id;   }
          bool             getOptional() const { return m_optional; }
      protected:

        std::string_view m_name;       // name of section
        int              m_id;         // order index
        bool             m_optional;   // if section is required during read. On write we always write an empty section if it has no data
    };

    const std::vector<mapOrder> m_mapOrder =
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
        { sscript,            16, true  },
    };

}; // class RRMap


#endif  // MMDatUtil_H
