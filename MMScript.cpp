// CompressScript.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "MMDatUtil.h"
#include "MMScript.h"


class ScriptUserVariable
{
public:
    ScriptUserVariable() = default;
    ~ScriptUserVariable() = default;


    bool operator==(const ScriptUserVariable& rhs) const
    {
        return m_token.getTokenLC() == rhs.m_token.getTokenLC();    // compare lower case strings
    }

    bool operator==(std::string_view sv) const
    {
        return sv == m_token.getTokenLC();
    }

    // comparision operator for hash
    bool operator() (const ScriptUserVariable & lhs, const ScriptUserVariable& rhs) const
    {
        return lhs.m_token.getTokenLC() == rhs.m_token.getTokenLC();
    }

    // hash operator for hash - just the lower case
    std::size_t operator()(ScriptUserVariable const& s)const noexcept
    {
        return std::hash<std::string_view>{}(s.m_token.getTokenLC());
    }


protected:

    TokenAndId  m_token;        // name, lower case, id
    uint64_t    m_flags = 0;    // flags identifying what type of variable it is
    int         m_usageCnt = 0; // number of times it has been referenced.
};



#if 0

// this is base storage for every line. It has the original input line and will be filled in with raw tokens
class ScriptLineAndTokens
{
  public:
    typedef std::vector<TokenAndId> LineTokens_t;

    ScriptLineAndTokens() {}
    ScriptLineAndTokens(const InputLine& iline) { setInputLine(iline); }

    void setInputLine(const InputLine& iline) { m_line = iline; }
    const LineTokens_t &getTokens() const { return m_tokens; }
          LineTokens_t& getTokens()       { return m_tokens; }
    void addToken(const TokenAndId& token)
    {
        m_tokens.push_back(token);
    }

    CTokenStrRef getLineStr() const { return std::string(m_line.getLine()); }

  protected:
    LineTokens_t   m_tokens;    // tokens in order for this line
    InputLine      m_line;      // base line
};

#endif

#if 0

// this class holds every line of a script as well as parsed tokens for each line
class ScriptSection
{
  public:
    ScriptSection() {}
    ScriptSection(const MapSection& ms) { setMapSectionData(ms); }

    void setMapSectionData(const MapSection& ms)
    {
        int len = (int)ms.getLines().size();
        for (int i = 0; i < len; i++)
        {
            m_lines.push_back(Script1Line(ms.getLines()[i]));
        }
    }

    void tokenize()  // tokenize every line
    {
        ScriptTokenizer st;

        int len = (int)m_lines.size();
        for (int i = 0; i < len; i++)
        {
            m_lines[i].tokenize(st);   // TODO how to detect errors
        }
    }

    void parseScript()
    {
        LangDef ld;   // so we have builtin hash

        tokenize();   // first tokenize the input

        // first thing we will do is building up list of names. Look for variables being defined and events being defined
        // If the first token for each line is eTokenName then match it to predefined values. Other tokens will be matched later
        // based on context
        for (int lines = 0; lines < m_lines.size(); lines++)
        {
            Script1Line & s1l = m_lines[lines];
            ScriptLineAndTokens& slat = s1l.getLine();
            ScriptLineAndTokens::LineTokens_t & tokens = slat.getTokens();
            if (!tokens.empty())
            {
                TokenAndId& token = tokens[0];
                uint64_t id = token.getTokenId();   // id for the scanned token.
                switch (id)
                {
                  case 0:
                  {             // unknown token - TODO error
                      break;    
                  }

                  case LangDef::eTokenBlankLine:
                  case LangDef::eTokenComment:
                      continue;  // ignore this line completely
                  
                  case LangDef::eTokenName: // we expect some sort of name
                  {
                      // first see if it is a normal builtin
                      uint64_t bid = ld.getBuiltInId(token.getToken(), 0);
                      if (bid & (LangDef::eOccurance | LangDef::eDefObjective))
                          continue;        // when, if, init, objective do not define a user name - we will parse them later

                      else if (bid &
                          ( LangDef::eDefArrow
                          | LangDef::eDefMiner
                          | LangDef::eDefVehicle
                          | LangDef::eDefBuilding
                          | LangDef::eDefInt
                          | LangDef::eDefFloat
                          | LangDef::eDefString
                          | LangDef::eDefArray
                          | LangDef::eDefBool))  // all are variable definations
                      {
                          // next token must be a space
                          if (tokens.size() < 3)
                          {
                              // generate error
                              continue;
                          }
                          
                          if (tokens[1].getTokenId() != LangDef::eTokenSpace)
                          {
                              // generate error
                              continue;
                          }

                          if (tokens[2].getTokenId() != LangDef::eTokenName)
                          {
                              // generate error
                              continue;
                          }

                          // we have a name. It cannot be ANY builtin name. Compare to three types
                          if (ld.getBuiltInId(tokens[2].getToken(), 0)
                              || ld.getBuiltInId(tokens[2].getToken(), LangDef::eMacro)
                              || ld.getBuiltInId(tokens[2].getToken(), LangDef::eTrigger))
                          {
                              // error - reserved name
                              continue;
                          }

                          // name is not builtin. Check to see if we have this name already
                          UserName username(tokens[2].getToken(), tokens[2].getTokenLC());
                          auto it = m_UserNames.find(username);
                          if (it != m_UserNames.end())
                          {
                              // error defining same name twice
                              continue;
                          }
                          // have a good name. We will add it if the rest of the line is ok
                          // if anything more we either have a comment  or =
                          if (tokens.size() > 3)
                          {
                              if (tokens[3].getTokenId() != LangDef::eTokenComment && tokens[3].getTokenId() != LangDef::eTokenAssignment)
                              {
                                  // syntax error
                                  continue;
                              }
                              if (tokens[3].getTokenId() == LangDef::eTokenAssignment)
                              {
                                  // next token must be
                              }

                          }









                          username.incCount();  // count is one.
                          m_UserNames.insert(username);


                      }
                      else if (bid & LangDef::eDefInit) // predefined init event
                      {
                      }
                      else { // may be event chain. See if next token is ::

                      }
                      break;
                  }
                  
                  
                  default:
                      break;
                      // ERROR invalid syntax
                } // switch
            }
        }

    }

protected:
    class Script1Line
    {
      public:
        Script1Line() {}
        Script1Line(const InputLine& iline) : m_line(iline) {}

        void tokenize(ScriptTokenizer& st)
        {
            st.rawParse(m_line);
        }

        ScriptLineAndTokens& getLine() { return m_line; }

      protected:
        ScriptLineAndTokens m_line;

        // lots more to come

        bool                m_eventChain = false;   // true if this line is part of an event chain (or defines one)
    };

    // used to hold user defined names - variables and event chains and a count.
    // counts are used when redefining names, we do so based on usage
    class UserName
    {
    public:
        UserName() {}
        UserName(const std::string& name, const std::string& namelc) { setName(name, namelc); }
        UserName(const std::string& name) { setName(name); }

        void incCount() { m_count++; }  // name has been seen again, inc the count

        // these are defined below after LangDef
        bool operator==(const UserName& rhs) const
        {
            return m_namelc == rhs.m_namelc;
        }

        // comparision operator for hash. Note that a key to us is both the string and just the macro/event/trigger bits
        bool operator() (UserName const& lhs, UserName const& rhs) const
        {
            return lhs.m_namelc == rhs.m_namelc;
        }

        // hash operator for hash
        std::size_t operator()(UserName const& s)const noexcept
        {
            return std::hash<std::string>{}(s.m_namelc);
        }


    private:
        void setName(const std::string& name, const std::string& namelc)
        {
            m_name = name;
            m_namelc = namelc;
            m_count = 1;
        }

        void setName(const std::string& name)
        {
            m_name = name;
            m_namelc = toLower(name);
            m_count = 1;
        }

        std::string m_name;              // name (original case)
        std::string m_namelc;            // lower case name
        int         m_count = 0;         // number of times we have seen this name (at least 1)
    };

    std::vector<Script1Line>                         m_lines;     // the entire script
    std::unordered_set<UserName, UserName, UserName> m_UserNames; // track user names
};

#endif




void ScriptProcessor::ScriptProcessLine::rawParse()
{
    std::string_view input = m_line.getInputLine().getLine();
    std::size_t inputLen = input.length();

    // check for comment
    std::size_t commentpos = input.find('#');
    if (commentpos == 0)
    {
        m_tokens.push_back(TokenAndId(input, LangDef::eTokenComment));     // entire line is comment
        m_bComment = true;
        return;   // full length comment
    }
    if ((commentpos > 0) && (commentpos < inputLen))  // see if all blank before comment
    {
        bool bWhite = true;
        for (std::size_t i = 0; i < commentpos; i++)
        {
            if (isSpace(input[i]))
                continue;
            else
            {
                bWhite = false;
                break;
            }
        }
        if (bWhite)
        {
            m_tokens.push_back(TokenAndId(input, LangDef::eTokenComment));     // entire line is comment
            m_bComment = true;
            return;
        }

        // now start backwards from comment, find first non-space - this was we don't have space tokens before comment.
        std::size_t i = commentpos - 1;
        for (; i > 0; i--)
        {
            if (!isSpace(input[i]))
            {
                break;
            }
        }
        commentpos = i + 1; // either # char or beginning of white space prior to comment
    }
    else
    {
        // see if entire line is empty
        if (isEmptyStr(input))
        {
            m_tokens.push_back(TokenAndId(input, LangDef::eTokenBlankLine));   // entire line is empty - it will terminate an event chain
            return;
        }
        commentpos = inputLen;    // pretend we have a comment after end of line - this allows us to use this to stop parsing
    }

    // now sit in loop parsing tokens
    std::string_view token;
    for (std::size_t npos = 0; npos < commentpos;)
    {
        std::size_t epos = npos + 1;
        // if we have an alpha, treat as some sort of name for the remaining alpha and digits
        uint8_t ch = input[npos];
        if (isalpha(ch))   // have an alpha char. Continue while more alpha or digits
        {
            for (; epos < commentpos; epos++)
            {
                if (!isdigit(input[epos]) && !isalpha(input[epos])) // this char ends the token
                    break;
            }
            token = input.substr(npos, epos - npos);
            m_tokens.push_back(TokenAndId(token, LangDef::eTokenName));
        }
        
        else
        {
            switch (ch)
            {
            case ' ':  // space   see if single space or sequence of spaces
            case 9:    //tab
            {
                for (; (epos < commentpos) && isSpace(input[epos]); epos++)
                    ;
                token = input.substr(npos, epos - npos);
                m_tokens.push_back(TokenAndId(token, token.length() > 1 ? LangDef::eTokenSpaces : LangDef::eTokenSpace));
                break;
            }

            case '.':   // this may be a dot not a number
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':  // starting number, float, or variable
            {
                // because variables can start with a number, we need to scan to see if we only have numbers and at least one alpha.
                // anything that is non-alpha before the alpha means a number. Once you see an alpha, continue with only
                // alpha and numbers.
                if (ch != '.')
                {
                    for (; (epos < commentpos) && isdigit(input[epos]); epos++)
                        ;  // scan until first non-number
                    if ((epos < commentpos) && isalpha(input[epos]))  // we have an alpha, so treat as an name
                    {
                        for (epos++; epos < commentpos && (isalpha(input[epos]) || isdigit(input[epos])); epos++)
                            ;
                        token = input.substr(npos, epos - npos);
                        m_tokens.push_back(TokenAndId(token, LangDef::eTokenName));
                        break;
                    }
                    epos = npos + 1; // back so we can build number or float
                }

                bool bFloat = false;
                if (ch == '.')                  // floats can being with a .
                {
                    if ((epos < commentpos) && isdigit(input[epos]))  // next is a number so treat as a float
                        bFloat = true;
                    else                       // just a dot char, so add to tokens as a dot
                    {
                        token = input.substr(npos, 1);
                        m_tokens.push_back(TokenAndId(token, LangDef::eTokenDot));
                        break;
                    }
                }
                for (; epos < commentpos; epos++)  // now we treat it as either an int or float
                {
                    if (!isdigit(input[epos]))
                    {
                        if (input[epos] == '.')
                        {
                            if (!bFloat)
                                bFloat = true;
                            else
                                break;
                        }
                        else
                            break;
                    }
                }
                // token is from [spos,epos)
                token = input.substr(npos, epos - npos);
                m_tokens.push_back(TokenAndId(token, bFloat ? LangDef::eTokenFloat : LangDef::eTokenInt));
                break;
            }

            case ';':
            {
                token = input.substr(npos, 1);
                m_tokens.push_back(TokenAndId(token, LangDef::eTokenSemi));
                break;
            }

            case ':':  // single or double
            {
                uint64_t tag = LangDef::eTokenColon;
                if ((epos < commentpos) && (input[epos] == ':'))
                {
                    tag = LangDef::eTokenDColon;
                    epos++;
                }
                token = input.substr(npos, epos - npos);
                m_tokens.push_back(TokenAndId(token, tag));
                break;
            }

            case '(':  // single or double brace
            {
                uint64_t tag = LangDef::eTokenOBrace;
                if ((epos < commentpos) && (input[epos] == '('))
                {
                    tag = LangDef::eTokenDOBrace;
                    epos++;
                }
                token = input.substr(npos, epos - npos);
                m_tokens.push_back(TokenAndId(token, tag));
                break;
            }

            case ')':  // single or double brace
            {
                uint64_t tag = LangDef::eTokenCBrace;
                if ((epos < commentpos) && (input[epos] == ')'))
                {
                    tag = LangDef::eTokenDCBrace;
                    epos++;
                }
                token = input.substr(npos, epos - npos);
                m_tokens.push_back(TokenAndId(token, tag));
                break;
            }

            case '[':
            {
                token = input.substr(npos, 1);
                m_tokens.push_back(TokenAndId(token, LangDef::eTokenOBracket));
                break;
            }

            case ']':
            {
                token = input.substr(npos, 1);
                m_tokens.push_back(TokenAndId(token, LangDef::eTokenCBracket));
                break;
            }

            case '+':  // addition or asignment
            {
                uint64_t tag = LangDef::eTokenPlus;
                if ((epos < commentpos) && (input[epos] == '='))
                {
                    tag = LangDef::eTokenPlusAssign;
                    epos++;
                }
                token = input.substr(npos, epos - npos);
                m_tokens.push_back(TokenAndId(token, tag));
                break;
            }

            case '-':  // addition or asignment
            {
                uint64_t tag = LangDef::eTokenMinus;
                if ((epos < commentpos) && (input[epos] == '='))
                {
                    tag = LangDef::eTokenMinusAssign;
                    epos++;
                }
                token = input.substr(npos, epos - npos);
                m_tokens.push_back(TokenAndId(token, tag));
                break;
            }

            case '*':  // addition or asignment
            {
                uint64_t tag = LangDef::eTokenMultiply;
                if ((epos < commentpos) && (input[epos] == '='))
                {
                    tag = LangDef::eTokenMultiplyAssign;
                    epos++;
                }
                token = input.substr(npos, epos - npos);
                m_tokens.push_back(TokenAndId(token, tag));
                break;
            }

            case '/':  // can be divide, objective, assignment
            {
                uint64_t tag = LangDef::eTokenFslash;
                if (epos < commentpos)   // possible to have 2nd char
                {
                    if (input[epos] == '/')
                    {
                        epos++;
                        tag = LangDef::eTokenDivide;
                    }
                    else if (input[epos] == '=')
                    {
                        epos++;
                        tag = LangDef::eTokenDivideAssign;
                    }
                }
                token = input.substr(npos, epos - npos);
                m_tokens.push_back(TokenAndId(token, tag));
                if (tag == LangDef::eTokenFslash)  // if objective, everything after the / is object and treat as string
                {
                    token = input.substr(epos, commentpos - epos);
                    m_tokens.push_back(TokenAndId(token, LangDef::eTokenObjStr));
                    epos = commentpos;  // this forces the end of processing
                }
                break;
            }

            case '<':  // < or <=
            {
                uint64_t tag = LangDef::eTokenLess;
                if ((epos < commentpos) && (input[epos] == '='))
                {
                    tag = LangDef::eTokenLessEqual;
                    epos++;
                }
                token = input.substr(npos, epos - npos);
                m_tokens.push_back(TokenAndId(token, tag));
                break;
            }

            case '>':  // > or >=
            {
                uint64_t tag = LangDef::eTokenGreater;
                if ((epos < commentpos) && (input[epos] == '='))
                {
                    tag = LangDef::eTokenGreaterEqual;
                    epos++;
                }
                token = input.substr(npos, epos - npos);
                m_tokens.push_back(TokenAndId(token, tag));
                break;
            }

            case '!':  // ! or !=    ! is current an error
            {
                uint64_t tag = LangDef::eTokenNot;
                if ((epos < commentpos) && (input[epos] == '='))
                {
                    tag = LangDef::eTokenNotEqual;
                    epos++;
                }
                token = input.substr(npos, epos - npos);
                m_tokens.push_back(TokenAndId(token, tag));
                break;
            }

            case '=':  // = or ==   
            {
                uint64_t tag = LangDef::eTokenAssignment;
                if ((epos < commentpos) && (input[epos] == '='))
                {
                    tag = LangDef::eTokenEqual;
                    epos++;
                }
                token = input.substr(npos, epos - npos);
                m_tokens.push_back(TokenAndId(token, tag));
                break;
            }


            case ',':
            {
                token = input.substr(npos, 1);
                m_tokens.push_back(TokenAndId(token, LangDef::eTokenComma));
                break;
            }

            case '"':  // start quoted string. We want to keep the quotes
            {
                for (; epos < commentpos && input[epos] != '"'; epos++)
                    ;
                if ((epos >= commentpos) || (input[epos] != '"'))   // missing ending quote
                {
                    ;   // we need to do some sort of error
                }
                if (epos == npos)
                    token = "\"\"";
                else
                    token = input.substr(npos, epos++ - npos + 1);
                m_tokens.push_back(TokenAndId(token, LangDef::eTokenString));

                break;
            }

            default:  // no idea. Posssible a unicode variable name (TODO - does MM engine deal with this case?)
            {
                // for now take all the unknown chars and make a token out of it (may have to also allow alpha and digits if mm engine supports unicode var names)
                for (; (epos < commentpos) && !isKnown(input[epos]); epos++)
                    ;
                token = input.substr(npos, epos - npos);
                m_tokens.push_back(TokenAndId(token, LangDef::eUnknown));
                break;
            }

            } // switch
        } // else
        npos = epos;
    } // for
    // we have parsed everything not a comment. If we have a comment, now add it
    if (commentpos < inputLen)
    {
        m_tokens.push_back(TokenAndId(input.substr(commentpos), LangDef::eTokenComment));
    }
} // rawParse


// pass 0, nothing has been parsed. We are looking for pragma comments, and we clean up blanks lines to really be blank
void  ScriptProcessor::ScriptPass0(ScriptProcessLine& pl, const CommandLineOptions& options)  // process tokens as part of pass 1
{
    if (pl.processed())     // line has already been processed, no need to process more
        return;

    std::vector<TokenAndId>& tokens = pl.getTokens();

    if (tokens.empty()) // TODO I don't think this can ever happen, but just in case
    {
        pl.setProcessed(true);
        return;
    }

    for (size_t tokennum = 0; tokennum < tokens.size(); tokennum++)
    {
        switch (tokens[tokennum].getTokenId())
        {
        case LangDef::eTokenBlankLine:  // parser ensured this is only token in the line
        {
            pl.setProcessed(true);
            tokens[tokennum].clrString();  // removed any spaces, it is now really a blank line
            break;
        }
        case LangDef::eTokenComment:
        {
            Pass0Comment(tokennum, tokens[tokennum], pl, options);
            break;
        }
        };
    }


}

// pass 0 have a comment.
// if comment starts wtih #pragma, treat as pragma. If stipping comments, strip anything not a #pragma or #. If a valid pragma save back as ##pragma
void ScriptProcessor::Pass0Comment( std::size_t tokennum,  TokenAndId& token, ScriptProcessLine& pl, const CommandLineOptions& options)
{
    if (pl.commentLine())       // entire line is comment
        pl.setProcessed(true);  // so no need to process this line again

    std::string_view comment = token.getToken();      // mixed case version  need this for the value
    std::string_view commentLC = token.getTokenLC();  // lower case version used to determine if this is a pragma and the type and name

    std::string_view prespaces; // this will be set to any prespaces
    std::size_t spos = 0;
    for (; spos < comment.size() && pl.isSpace(comment[spos]); spos++);
    prespaces = comment.substr(0, spos);
    comment = comment.substr(spos);     // should start with #, tokenizer ensured this
    commentLC = commentLC.substr(spos); // should start with #, tokenizer ensured this

    spos = commentLC.find("##pragma", 0);
    if (spos == 0)
    {
        return;          // already processed #pragma, just skip
    }

    std::string_view pragmastr = "#pragma";
    spos = commentLC.find(pragmastr, 0);
    if (spos != std::string_view::npos)
    {
        if ((spos != 0) || (tokennum != 0))
        {
            m_errors.setWarning(pl.getLine().getInputLine(), pl.getFileName() + ": #pragma must be a full line comment and at beginning");
            return;
        }

        // we have a valid #pragma. Get the pragma name. This entire token is comments so we have to parse it manually
        for (spos += pragmastr.size(); spos < commentLC.size() && pl.isSpace(commentLC[spos]); spos++);  // skip spaces to start of name
        std::size_t epos = spos;
        for (; epos < commentLC.size() && (pl.isSpace(commentLC[epos]) == false); epos++);  // find end of pragma name
        if (spos >= commentLC.size())
        {
            m_errors.setWarning(pl.getLine().getInputLine(), pl.getFileName() + ": #pragma name invalid, ignoring");
            return;
        }
        std::string_view pragmaname = commentLC.substr(spos, epos - spos);
        if (pragmaname == "define")    // define pragma
        {
            for (spos = epos; spos < commentLC.size() && pl.isSpace(commentLC[spos]); spos++);  // skip spaces to start of name
            epos = spos;
            for (; epos < commentLC.size() && (commentLC[epos] != '='); epos++);  // find = value
            if (epos + 1 >= commentLC.size())
            {
                m_errors.setWarning(pl.getLine().getInputLine(), pl.getFileName() + ": #pragma define name=value invalid, ignoring");
                return;
            }
            std::string_view keyname(commentLC.substr(spos, epos - spos));
            spos = epos + 1;
            if (comment[spos] == '"')
            {
                for (epos = spos + 1; epos < comment.size() && (comment[epos] != '"'); epos++);  // find = value
                epos++; // allows the closing quote to be included, we will catch missing " down below
            }
            else
            {
                for (epos = spos; epos < comment.size() && (pl.isSpace(comment[epos]) == false); epos++);  // find = value
            }
            std::string_view value(comment.substr(spos, epos - spos));

            // values must be one of the following types
            // - quoted string. Will be valid to be used where a quoted string is valid
            // - Alphanumeric name. First char is alpha, remaining chars are either alpha or numeric.
            // - positive int number.  Note that MM engine does not allow negative int literal values.
            // - positive floating point number. Note that the MM engine does not allow negative float literal values
            // if the value starts with a quote, it must end in a quote
            if (value[0] == '"')
            {
                if ((value.size() < 2) || value[value.size() - 1] != '"')
                {
                    m_errors.setWarning(pl.getLine().getInputLine(), pl.getFileName() + ": #pragma define name=value invalid missing closing quote, ignoring");
                    return;
                }
            }
            else if (!isValidVarName(value) || !isFloat(value, false) || !isInteger(value, false))
            {
                m_errors.setWarning(pl.getLine().getInputLine(), pl.getFileName() + ": Invalid value: " + std::string(value) + "Must be string, alphanumeric name, float, int");
                return;
            }

            if (!isValidVarName(keyname))
            {
                m_errors.setWarning(pl.getLine().getInputLine(), pl.getFileName() + ": #pragma define name=value invalid, ignoring");
                return;
            }


            // make sure it is unique
            if (m_langdef.contains(keyname))
            {
                m_errors.setWarning(pl.getLine().getInputLine(), pl.getFileName() + ": #pragma define cannot use reserved name, ignoring");
                return;
            }
            if (m_defines.contains(keyname))
            {
                m_errors.setWarning(pl.getLine().getInputLine(), pl.getFileName() + ": #pragma define cannot redefine, ignoring");
                return;
            }
            m_defines.addKeyValue(keyname, value);

            // now we need to change the comment so it does not get processed again
            token.changeString(std::string(prespaces) + '#' + std::string(comment));
        }
        else if (pragmaname == "tyabscriptdate" || pragmaname == "tyabscriptincdate")    // modified time for main or included script, follows is the format
        {
            std::string filename;
            std::string format;
            filename = m_lines[0].getFileName();
            if (pragmaname == "tyabscriptincdate")
            {
                filename = pl.getFileName();
            }

            // next parameter is the format script
            for (spos = epos; spos < comment.size() && pl.isSpace(comment[spos]); spos++);  // skip spaces to start of format
            if (spos < comment.size())
            {
                if (comment[spos] == '"')
                {
                    for (epos = spos + 1; epos < comment.size() && (comment[epos] != '"'); epos++);  // find = value
                    if (comment[epos] == '"')
                        epos++;
                }
                else
                {
                    for (epos = spos; epos < comment.size() && (pl.isSpace(comment[epos]) == false); epos++);  // find = value
                }
            }
            if ((spos >= comment.size()) || (epos > comment.size()))
            {
                m_errors.setWarning(pl.getLine().getInputLine(), pl.getFileName() + ": #pragma " + std::string(pragmaname) + " date format invalid, ignoring");
            }
            else
            {
                format = comment.substr(spos, epos - spos);
                // now we need to change the comment so it does not get processed again and include the date info.
                token.changeString(std::string(prespaces) + '#' + std::string(comment) + getDateStr(filename, format));
            }
        }
        else
        {
            m_errors.setWarning(pl.getLine().getInputLine(), pl.getFileName() + ": #pragma name unknown: " + std::string(pragmaname) + " ignoring");
        }
    }
    else    // this is just a comment token.  If we are stipping comments, see if the first non-space is #. and if so we keep it
    {
        if (options.m_bScrNoComments)   // removing comments  epos is the starting first non-white space char of the comment.
        {
            if (comment.find("#.",0)==0)
            {
                return;     // keep comment
            }
            token.clrString();  // comment is removed
        }

    }
}


// return true if we match the defination which is the first item in defination. If we match, we return true even if we then find errors.
// we skip the tokens first item if it is spaces, we have already either fixed or generated an error for that.
bool ScriptProcessor::matchDefination(const LangDef::Defination& defination, const std::vector<TokenAndId>& tokens, ScriptProcessLine& /*pl*/, const CommandLineOptions& /*options*/)
{
    std::size_t tokenNum = 0;
    // skip any leading spaces, we have already fixed them or logged an error
    if (tokens[0].getTokenId() == LangDef::TokenTypes::eTokenSpace || tokens[0].getTokenId() == LangDef::TokenTypes::eTokenSpaces)
        tokenNum++;

    for (std::size_t defIndex = 0; defIndex < defination.size(); defIndex++)
    {
        switch (defination[defIndex].tokenid)
        {
        case LangDef::TokenTypes::eTokenName:       // we are expecting a name
        {
            if (tokens[tokenNum].getTokenId() == LangDef::TokenTypes::eTokenName)   // input tokens match the type
            {
                // see if name matches one of our macros
                //bool bDefine = m_defines.contains(tokens[tokenNum].getTokenLC());

            }
        }



        }

    }





    return false;
}

// pass 1. Find all variable declarations, build list of all user variables and event chain names.
// this will catch all duplicate names, use of reserved names,
// and subsitute variable declarations initialization values that use the #pragma define values
// tag all variable declarations as processed
void  ScriptProcessor::ScriptPass1(ScriptProcessLine& pl, const CommandLineOptions& options)  // process tokens as part of pass 1
{
    if (pl.processed())     // line has already been processed, no need to process more
        return;

    std::vector<TokenAndId>& tokens = pl.getTokens();
    std::size_t tokennum = 0;

    // first thing to handle is any leading space. Tag as an error if we are not fixing
    if ((tokens[0].getTokenId() == LangDef::TokenTypes::eTokenSpace) || (tokens[0].getTokenId() == LangDef::TokenTypes::eTokenSpaces))
    {
        if (options.m_bScrFixSpace)
        {
            tokens[0].clrString();  // we fix by clearing the token strings to nothing
            tokennum++;             // move to next token
        }
        else
        {
            m_errors.setError(pl.getLine().getInputLine(), pl.getFileName() + ": leading whitespace not allowed");
        }
    }

    // in pass 1 we are looking for the following:
    //  - variables (int, bool, float, etc)
    //  - variable:
    //  - triggers (if, when)
    //  - event chain names (name followed by ::
    // only generate errors if we know we are one of these and its invalid. Eventchain name is the only one we can't fully parse here

    uint64_t id = tokens[tokennum].getTokenId();
    if (id == LangDef::eTokenName)
    {
        std::string_view tokenlc = tokens[tokennum].getTokenLC();   // get lower case name

        // first thing we do is see if this can be replaced by a define
        if (m_defines.contains(tokenlc))
        {
            // now get the value. It itself can be another define, so recurive process until we get a final result. Check for circular definations
            bool bError = false;
            std::string value = processDefines(tokenlc, pl, bError);
            if (!bError)
            {
                tokens[tokennum].changeString(value);
                tokenlc = tokens[tokennum].getTokenLC();   // get lower case name of subsituted token
            }
        }

        // see if this could start an event chain.
        if (nextTokenId(tokennum, tokens, false) == LangDef::eTokenDColon)   // this looks like we are defining an event chain
        {
            id = nextTokenId(tokennum, tokens, true);
            if ((id == LangDef::eTokenSpace) || (id == LangDef::eTokenSpaces))  // if there is space(s)
            {
                if (options.m_bScrFixSpace)     // and we can fix the space(s)
                {
                    tokens[tokennum + 1].clrString();   // fix them.
                }
                else
                    m_errors.setError(pl.getLine().getInputLine(), pl.getFileName() + ": whitespace not allowed");
            }

            if (m_langdef.contains(tokenlc))
            {
                m_errors.setError(pl.getLine().getInputLine(), pl.getFileName() + ": Cannot use reserved name for eventchain name: " + std::string(tokenlc));
                return;
            }

            if (containsUsrName(tokenlc))
            {
                m_errors.setError(pl.getLine().getInputLine(), pl.getFileName() + ": Eventchain name is already user defined as var/eventchain " + std::string(tokenlc) );
                return;
            }

            if (!isValidVarName(tokenlc))
            {
                m_errors.setError(pl.getLine().getInputLine(), pl.getFileName() + ": Eventchain name is invalid " + std::string(tokenlc));
                return;
            }
            // name is valid and unique, add it
            addUsrName(tokens[tokennum]);
            return;


        }
        else  // not an eventchain, so check for the cases we allow
        {
            switch (id)
            {
            case LangDef::eDefMiner:        // miner
            {
            }
            break;

            case LangDef::eDefBuilding:     // building
            {
            }
            break;

            case LangDef::eDefVehicle:      // vehicle
            {
            }
            break;

            case LangDef::eDefInt:          // int
            {
            }
            break;

            case LangDef::eDefFloat:        // float
            {
            }
            break;

            case LangDef::eDefString:       // string
            {
            }
            break;

            case LangDef::eDefArray:        // intarray
            {
            }
            break;

            case LangDef::eDefBool:         // bool 
            {
            }
            break;

            case LangDef::eDefObjective:    // variable:
            {
            }
            break;

            case LangDef::eDefArrow:        // arrow
            {
            }
            break;

            case LangDef::eOccurance:       // if and when
            {
            }
            break;







            }

        }
        // see if this matches 
    }
}

// input is lower case of name that does match items in defines. Track all of the redefinations to check for circular reference
std::string ScriptProcessor::processDefines(std::string_view token, const ScriptProcessLine& pl, bool& bError)
{
    std::unordered_set<std::string_view> redefs;
    redefs.reserve(32);   // prevent excessive rehashing on growth

    std::string rets = processDefines(token, bError, redefs);

    if (bError)
    {
        m_errors.setError(pl.getLine().getInputLine(), "Circular define value detected: " + rets);
    }
    else if (!isValidVarName(rets))
    {
        bError = true;
        m_errors.setError(pl.getLine().getInputLine(), "Invalid define value detected: " + rets);
    }

    return rets;
}

std::string ScriptProcessor::processDefines(std::string_view token, bool& bError, std::unordered_set<std::string_view> & redefs)
{
    std::string tokenlc = toLower(token);
    if (redefs.contains(tokenlc))       // check for circular reference
    {
        bError = true;                  // found it
        return std::string(tokenlc);    // we return the name that was already replaced.
    }
    redefs.insert(tokenlc);             // no circular refence so add this one for next recursion

    return m_defines.contains(tokenlc) ? processDefines(m_defines.getValue(tokenlc), bError, redefs) : std::string(token);
}





std::size_t ScriptProcessor::nextTokenId(std::size_t tokennum, const std::vector<TokenAndId> & tokens, bool bAllowSpace)
{
    tokennum++;
    if (tokennum < tokens.size())
    {
        uint64_t id = tokens[tokennum].getTokenId();
        if ((id == LangDef::eTokenSpace) || (id == LangDef::eTokenSpaces))
        {
            if (bAllowSpace)
                return id;
            tokennum++;
            if (tokennum < tokens.size())
            {
                return tokens[tokennum].getTokenId();
            }
        }

    }
    return 0;
}

void  ScriptProcessor::ScriptPass2(ScriptProcessLine& pl, const CommandLineOptions& /*options*/)  // process tokens as part of pass 2
{
    if (pl.processed())
        return;

    //const std::vector<TokenAndId>& tokens = pl.getTokens();


}
