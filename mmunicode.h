#ifndef _MMUNICODE
#define _MMUNICODE

#include <cstdint>
#include <string>


// based from:  https://android.googlesource.com/platform/external/id3lib/+/master/unicode.org/ConvertUTF.c

namespace Unicode
{
    static constexpr uint32_t UNI_REPLACEMENT_CHAR = 0x0000FFFD;
    static constexpr uint32_t UNI_MAX_BMP          = 0x0000FFFF;
    static constexpr uint32_t UNI_MAX_UTF16        = 0x0010FFFF;
    static constexpr uint32_t UNI_MAX_UTF32        = 0x7FFFFFFF;
    static constexpr uint32_t UNI_MAX_LEGAL_UTF32  = 0x0010FFFF;

    static constexpr int halfShift                 = 10; /* used for shifting by 10 bits */
    static constexpr uint32_t halfBase             = 0x0010000;
    static constexpr uint32_t halfMask             = 0x3FF;
    static constexpr uint32_t UNI_SUR_HIGH_START   = 0xD800;
    static constexpr uint32_t UNI_SUR_HIGH_END     = 0xDBFF;
    static constexpr uint32_t UNI_SUR_LOW_START    = 0xDC00;
    static constexpr uint32_t UNI_SUR_LOW_END      = 0xDFFF;

    static constexpr uint32_t byteMask = 0xBF;
    static constexpr uint32_t byteMark = 0x80; 

    /*
    * Index into the table below with the first byte of a UTF-8 sequence to
    * get the number of trailing bytes that are supposed to follow it.
    * Note that *legal* UTF-8 values can't have 4 or 5-bytes. The table is
    * left as-is for anyone who may want to do such conversion, which was
    * allowed in earlier algorithms.
    */
    static constexpr uint8_t trailingBytesForUTF8[256] = {
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5
    };

    /*
    * Magic values subtracted from a buffer value during UTF8 conversion.
    * This table contains as many values as there might be trailing bytes
    * in a UTF-8 sequence.
    */
    static constexpr uint32_t offsetsFromUTF8[6] =
    {
        0x00000000UL,
        0x00003080UL,
        0x000E2080UL, 
        0x03C82080UL,
        0xFA082080UL,   // legacy no longer valid
        0x82082080UL    // legacy no longer valid
    };


    /*
    * Once the bits are split out into bytes of UTF-8, this is a mask OR-ed
    * into the first byte, depending on how many bytes follow.  There are
    * as many entries in this table as there are UTF-8 sequence types.
    * (I.e., one byte sequence, two byte... etc.). Remember that sequencs
    * for *legal* UTF-8 will be 4 or fewer bytes total.
    */
    static constexpr uint8_t firstByteMark[7] =
    {
        0x00,       // not used
        0x00,       // 1 byte sequence
        0xC0,       // 2 byte sequence
        0xE0,       // 3 byte sequence
        0xF0,       // 4 byte sequence
        0xF8,       // legacy no longer valid
        0xFC        // legacy no longer valid
    };


    /*
    * Utility routine to tell whether a sequence of bytes is legal UTF-8.
    * This must be called with the length pre-determined by the first byte.
    * If not calling this from ConvertUTF8to*, then the length can be set by:
    *  length = trailingBytesForUTF8[*source]+1;
    * and the sequence is illegal right away if there aren't that many bytes
    * available.
    * If presented with a length > 4, this returns false.  The Unicode
    * definition of UTF-8 goes up to 4-byte sequences.
    */
    // PORT NOTE: almost impossible to understand without complete knowlege of the UTF8 code points.
    // was written back when hand unrolling produced better code
    // when inside of a code point, every extra byte is between 0x80 and 0xBF. that is why
    // utf8 never conflicts with ansi chars.
    // the code point itself is between 0xC2 and 0xF4
    // after the code point, the 2nd byte has many ranges based on the first byte
    // the input to this is based on the output of the trailingBytesForUTF8 array.

    static inline bool isLegalUTF8(std::string_view src )
    {
        uint8_t a;
        std::size_t index = src.size(); // 1 past the last 0 based index
        switch (index)
        {
            default: return false;
                /* Everything else falls through when "true"... */
            case 4: if ((a = (uint8_t)src[--index]) < 0x80 || a > 0xBF) return false;
            case 3: if ((a = (uint8_t)src[--index]) < 0x80 || a > 0xBF) return false;
            case 2: if ((a = (uint8_t)src[--index]) > 0xBF) return false;
                switch ((uint8_t)src[0])
                {
                /* no fall-through in this inner switch */
                    case 0xE0: if (a < 0xA0) return false; break;
                    case 0xED: if (a > 0x9F) return false; break;
                    case 0xF0: if (a < 0x90) return false; break;
                    case 0xF4: if (a > 0x8F) return false; break;
                    default:   if (a < 0x80) return false;
                }
            case 1:
                if (((uint8_t)src[0] >= 0x80) && ((uint8_t)src[0] < 0xC2)) return false;
                if ((uint8_t)src[0] > 0xF4) return false;
        }
        return true;
    }

    static inline void utf32ch_to_utf8(uint32_t ch, std::string& utf8)
    {
        /*
        * based on char, convert to 1->4 bytes. Turn any
        * illegally large UTF32 things (> Plane 17) into replacement chars.
        */
        if (ch < 0x80)
        {
            utf8 += (char)((ch) | firstByteMark[1]);
        }
        else if (ch < 0x800)
        {
            utf8 += (char)((ch >> 6) | firstByteMark[2]);
            utf8 += (char)((ch | byteMark) & byteMask);
        }
        else if (ch < 0x10000)
        {
            utf8 += (char)((ch >> 12) | firstByteMark[3]);
            utf8 += (char)(((ch >> 6) | byteMark) & byteMask);
            utf8 += (char)((ch | byteMark) & byteMask);
        }
        else if (ch <= UNI_MAX_LEGAL_UTF32)
        {
            utf8 += (char)((ch >> 18) | firstByteMark[4]);
            utf8 += (char)(((ch >>12) | byteMark) & byteMask);
            utf8 += (char)(((ch >>6) | byteMark) & byteMask);
            utf8 += (char)((ch | byteMark) & byteMask);
        }
        else
        {
            ch = UNI_REPLACEMENT_CHAR;      // invalid sequence
            utf8 += (char)((ch >> 12) | firstByteMark[3]);
            utf8 += (char)(((ch >> 6) | byteMark) & byteMask);
            utf8 += (char)((ch | byteMark) & byteMask);
        }

    }

    // invalid sequences are replaced with replacement char
    static inline std::string utf16_to_utf8( std::u16string_view str)
    {
        std::string utf8;
        utf8.reserve(str.length()*4);

        for (std::size_t index = 0; index < str.size();)
        {
            uint32_t ch = str[index++];
            /* If we have a surrogate pair, convert to UTF32 first. */
            if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_HIGH_END) {
                /* If the 16 bits following the high surrogate are in the source buffer... */
                if (index < str.size())
                {
                    uint32_t ch2 = str[index++];
                    /* If it's a low surrogate, convert to UTF32. */
                    if (ch2 >= UNI_SUR_LOW_START && ch2 <= UNI_SUR_LOW_END)
                    {
                        ch = ((ch - UNI_SUR_HIGH_START) << halfShift)
                            + (ch2 - UNI_SUR_LOW_START) + halfBase;
                    }
                    else  /* it's an unpaired high surrogate */
                    {
                        ch = UNI_REPLACEMENT_CHAR;
                    }
                }
                else  // no more source, missing pair to high surrogate
                {
                    break;  // end string
                }
            }
            else
            {
                /* UTF-16 surrogate values are illegal in UTF-32 */
                if (ch >= UNI_SUR_LOW_START && ch <= UNI_SUR_LOW_END)
                {
                    ch = UNI_REPLACEMENT_CHAR;
                }
            }

            // TPN: substitute all control characters except for NULL, TAB, LF or CR
            // TODO: why is this not done in the utf32 logic?
            if (ch && (ch != 0x09)  && (ch != 0x0a)  && (ch != 0x0d)  && (ch < 0x20) )  {
                ch = 0x3f; 
            } 
            // TPN: filter out byte order marks and invalid character 0xFFFF
            if((ch == 0xFEFF) || (ch == 0xFFFE)|| (ch == 0xFFFF))
            {
                continue;
            }

            utf32ch_to_utf8( ch, utf8);
        }
        utf8.shrink_to_fit();
        return utf8;
    }

    static inline void utf32ch_to_utf16(uint32_t ch, std::u16string& rets)
    {
        // turn utf32 char into utf16 
        if (ch <= UNI_MAX_BMP)   /* Target is a character <= 0xFFFF */
        {
            if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_LOW_END)  /* UTF-16 surrogate values are illegal in UTF-32 */
                rets += UNI_REPLACEMENT_CHAR;
            else
                rets += (char16_t)ch; /* normal case */
        }
        else if (ch > UNI_MAX_UTF16)  // nothing over 0x10ffff allowed
            rets += UNI_REPLACEMENT_CHAR;
        else
        {
            /* target is a character in range 0xFFFF - 0x10FFFF. */
            ch -= halfBase;
            rets += (char16_t)((ch >> halfShift) + UNI_SUR_HIGH_START);
            rets += (char16_t)((ch & halfMask) + UNI_SUR_LOW_START);
        }
    }

    static inline std::u16string utf8_to_utf16(std::string_view str)
    {
        std::u16string utf16;
        utf16.reserve(str.length()); // will never grow from this

        for (std::size_t i = 0; i < str.size(); )
        {
            int extraBytesToRead = trailingBytesForUTF8[(uint8_t)str[i]]; // 0 -> 3
            if (i+extraBytesToRead >= str.size())
                break;  // not enough source, invalid, end the string

            // old format no longer valid or invalid sequence turn into replacement char
            if ((extraBytesToRead >= 4) || !isLegalUTF8(str.substr(i, extraBytesToRead + 1)))
            {
                utf16 += UNI_REPLACEMENT_CHAR;
                i += extraBytesToRead+1;
                continue;
            }

            // turn into utf32 value 
            uint32_t ch = 0;
            for (int cnt = 1; cnt < extraBytesToRead; cnt++)
            {
                ch += (uint8_t)str[i++];
                ch <<= 6;
            }
            ch += (uint8_t)str[i++];
            ch -= offsetsFromUTF8[extraBytesToRead];

            utf32ch_to_utf16(ch, utf16);
        }
        utf16.shrink_to_fit();
        return utf16;
    }

    static inline std::u16string utf32_to_u16string(std::u32string_view str  )
    {
        std::u16string utf16;
        utf16.reserve(str.length()); // might grow beyond this, but at least this length

        for (std::size_t i = 0; i < str.size(); i++)
        {
            uint32_t ch = str[i];

            utf32ch_to_utf16(ch, utf16);
        }
        return utf16;
    }

    static inline std::u32string utf16_to_utf32(std::u16string_view str)
    {
        std::u32string utf32;
        utf32.reserve(str.length());

        for (std::size_t i = 0; i < str.size(); )
        {
            uint32_t ch = str[i++];
            /* If we have a surrogate pair, convert to UTF32 first. */
            if ((ch >= UNI_SUR_HIGH_START) && (ch <= UNI_SUR_HIGH_END))
            {
                if (i < str.size()-1)   // must have another char to process
                {
                    uint32_t ch2 = str[i++];    // get the char
                    /* If it's a low surrogate, convert to UTF32. */
                    if ((ch2 >= UNI_SUR_LOW_START) && (ch2 <= UNI_SUR_LOW_END))
                    {
                        ch = ((ch - UNI_SUR_HIGH_START) << halfShift)
                            + (ch2 - UNI_SUR_LOW_START) + halfBase;
                    }
                    else     /* it's an unpaired high surrogate */
                        ch = UNI_REPLACEMENT_CHAR;
                }
                else  /* We don't have the 16 bits following the high surrogate. */
                    break;  // all done
            }

            /*
            * UTF-16 surrogate values are illegal in UTF-32, and anything
            * over Plane 17 (> 0x10FFFF) is illegal.
            */
            if ((ch > UNI_MAX_LEGAL_UTF32) || ((ch >= UNI_SUR_HIGH_START) && (ch <= UNI_SUR_LOW_END)))
                ch = UNI_REPLACEMENT_CHAR;

            utf32 += ch;    // save either direct convert or the merged surrogate
        }
        return utf32;
    }

    static inline std::string utf32_to_utf8 (std::u32string_view str )
    {
        std::string utf8;
        utf8.reserve(str.length()*4);

        for (auto it : str)
        {
            uint32_t ch = it;

            /*
            * UTF-16 surrogate values are illegal in UTF-32, and anything
            * over Plane 17 (> 0x10FFFF) is illegal.
            */
            if ((ch > UNI_MAX_LEGAL_UTF32) || ((ch >= UNI_SUR_HIGH_START) && (ch <= UNI_SUR_LOW_END)))
                ch = UNI_REPLACEMENT_CHAR;

            utf32ch_to_utf8(ch, utf8);
        }
        utf8.shrink_to_fit();
        return utf8;
    }

    // all invalid sequences are filled in with replacement char.
    static inline std::u32string utf8_to_utf32 ( std::string_view str )
    {
        std::u32string utf32;
        utf32.reserve(str.length()); // will never grow from this

        for (std::size_t i = 0; i < str.size(); )
        {
            int extraBytesToRead = trailingBytesForUTF8[(uint8_t)str[i]]; // 0 -> 3
            if (i+extraBytesToRead >= str.size())
                break;  // not enough source, invalid, end the string

            // if old format no longer valid or invalid sequence, skip
            if ((extraBytesToRead >= 4) || !isLegalUTF8(str.substr(i, extraBytesToRead + 1)))   // old format no longer valid
            {
                utf32 += UNI_REPLACEMENT_CHAR;
                i += extraBytesToRead+1;
                continue;
            }
            // build utf32 value
            uint32_t ch = 0;
            for (int cnt = 1; cnt < extraBytesToRead; cnt++)
            {
                ch += (uint8_t)str[i++];
                ch <<= 6;
            }
            ch += (uint8_t)str[i++];
            ch -= offsetsFromUTF8[extraBytesToRead];

            /*
            * UTF-16 surrogate values are illegal in UTF-32, and anything
            * over Plane 17 (> 0x10FFFF) is illegal.
            */
            if ((ch > UNI_MAX_LEGAL_UTF32) || ((ch >= UNI_SUR_HIGH_START) && (ch <= UNI_SUR_LOW_END)))
                utf32 += UNI_REPLACEMENT_CHAR;
            else
                utf32 += ch;     // save utf32 char
        }
        return utf32;
    }

    // conversion between utf16 and wstring. same size on win
    static inline std::u16string wstring_to_utf16(std::wstring_view str)
    {
        std::u16string ret;
        ret.reserve( str.length() );
        for (auto it : str)
            ret += (char16_t)it;
        return ret;
    }

    // conversion between utf16 and wstring. same size on win
    static inline std::wstring utf16_to_wstring(std::u16string_view str)
    {
        std::wstring ret;
        ret.reserve( str.length() );
        for (auto it : str)
            ret += (wchar_t)it;
        return ret;
    }

    static inline std::string wstring_to_utf8(std::wstring_view str)
    {
        return utf16_to_utf8(wstring_to_utf16(str));
    }

    static inline std::wstring utf8_to_wstring(std::string_view str)
    {
        return utf16_to_wstring(utf8_to_utf16(str));
    }

    // input string is assumed to be in system ansi code page. We are returning UTF16 data.
    // this will then have to converted to utf16 and then utf8
    static inline std::wstring ansi_to_wstring(std::string const & str)
    {
        std::wstring rets;

        if (!str.empty())
        {
            int buffSize = ((int)str.size() + 1) * 4;     // this will handle utf32
            wchar_t* buff = (wchar_t*)malloc( buffSize );
            buff[0] = 0;
            int retval = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buff, buffSize); // convert using ANSI system code page
            if ((retval > 0) && buff[0])
                rets = buff;
            free(buff);
        }
        return rets;
    }

    // convert UTF16 data to ansi current code page. Ignoring data loss from characters that cannot be translated
    // return ANSI string
    static inline std::string wstring_to_ansi(std::wstring const& wstr)
    {
        std::string rets;
        if (!wstr.empty())
        {
            int buffSize = (int)(wstr.length()+1)*2; // giving room for utf16 output for double default chars
            char * buff = (char *)malloc( buffSize );
            buff[0] = 0;

            int retval = WideCharToMultiByte( CP_ACP, 0, wstr.c_str(), -1, buff, buffSize, nullptr, nullptr );
            if ((retval > 0) && buff[0])
                rets = buff;
            free(buff);
        }
        return rets;
    }
};

#endif // _MMUNICODE