#ifndef _MMUNICODE
#define _MMUNICODE

#include <cstdint>
#include <string>
#include <tuple>
#include <vector>


// Unicode conversion routines. Original based on 
//  https://android.googlesource.com/platform/external/id3lib/+/master/unicode.org/ConvertUTF.c
// 
// This version has been heavily modified to be C++20 compliant, uses unicode string classes.
// no lookup tables. It is a straight forward breaking into into the control points

namespace Unicode
{
    static constexpr uint32_t UNI_REPLACEMENT_CHAR = 0x0000FFFD; // use this character for all invalid sequences.

    // Given the position in the sv, determine the number of characters the sequences consumes from sv.
    // If the sequence is valid, return the sequence as UTF32
    // return format is bool valid, number of chars needed, and decoded utf32 character

    static inline std::tuple<bool, std::size_t, char32_t> utf8_to_utf32_char(std::string_view sv, std::size_t start)
    {
        if (start >= sv.size()) return {false, 0, 0};

        char32_t lead = static_cast<unsigned char>(sv[start]);
        std::size_t remaining = sv.size() - start;

        if (lead <= 0x7F) {
            // 1-byte ASCII
            return {true, 1, lead};
        } else if (lead >= 0xC2 && lead <= 0xDF) {
            // 2-byte sequence
            if (remaining < 2) return {false, 2, 0};
            char32_t b1 = static_cast<unsigned char>(sv[start + 1]);
            if (b1 < 0x80 || b1 > 0xBF) return {false, 2, UNI_REPLACEMENT_CHAR};
            char32_t cp = ((lead & 0x1F) << 6) | (b1 & 0x3F);
            return {true, 2, cp};
        } else if (lead == 0xE0) {
            // 3-byte sequence, avoid overlong
            if (remaining < 3) return {false, 3, 0};
            char32_t b1 = static_cast<unsigned char>(sv[start + 1]);
            char32_t b2 = static_cast<unsigned char>(sv[start + 2]);
            if (b1 < 0xA0 || b1 > 0xBF || b2 < 0x80 || b2 > 0xBF) return {false, 3, UNI_REPLACEMENT_CHAR};
            char32_t cp = ((lead & 0x0F) << 12) | ((b1 & 0x3F) << 6) | (b2 & 0x3F);
            return {true, 3, cp};
        } else if (lead >= 0xE1 && lead <= 0xEC) {
            if (remaining < 3) return {false, 3, 0};
            char32_t b1 = static_cast<unsigned char>(sv[start + 1]);
            char32_t b2 = static_cast<unsigned char>(sv[start + 2]);
            if (b1 < 0x80 || b1 > 0xBF || b2 < 0x80 || b2 > 0xBF) return {false, 3, UNI_REPLACEMENT_CHAR};
            char32_t cp = ((lead & 0x0F) << 12) | ((b1 & 0x3F) << 6) | (b2 & 0x3F);
            return {true, 3, cp};
        } else if (lead == 0xED) {
            // Avoid UTF-16 surrogates
            if (remaining < 3) return {false, 3, 0};
            char32_t b1 = static_cast<unsigned char>(sv[start + 1]);
            char32_t b2 = static_cast<unsigned char>(sv[start + 2]);
            if (b1 < 0x80 || b1 > 0x9F || b2 < 0x80 || b2 > 0xBF) return {false, 3, UNI_REPLACEMENT_CHAR};
            char32_t cp = ((lead & 0x0F) << 12) | ((b1 & 0x3F) << 6) | (b2 & 0x3F);
            return {true, 3, cp};
        } else if (lead >= 0xEE && lead <= 0xEF) {
            if (remaining < 3) return {false, 3, 0};
            char32_t b1 = static_cast<unsigned char>(sv[start + 1]);
            char32_t b2 = static_cast<unsigned char>(sv[start + 2]);
            if (b1 < 0x80 || b1 > 0xBF || b2 < 0x80 || b2 > 0xBF) return {false, 3, UNI_REPLACEMENT_CHAR};
            char32_t cp = ((lead & 0x0F) << 12) | ((b1 & 0x3F) << 6) | (b2 & 0x3F);
            return {true, 3, cp};
        } else if (lead == 0xF0) {
            // 4-byte sequence, avoid overlong
            if (remaining < 4) return {false, 3, 0};
            char32_t b1 = static_cast<unsigned char>(sv[start + 1]);
            char32_t b2 = static_cast<unsigned char>(sv[start + 2]);
            char32_t b3 = static_cast<unsigned char>(sv[start + 3]);
            if (b1 < 0x90 || b1 > 0xBF || b2 < 0x80 || b2 > 0xBF || b3 < 0x80 || b3 > 0xBF) return {false, 4, UNI_REPLACEMENT_CHAR};
            char32_t cp = ((lead & 0x07) << 18) | ((b1 & 0x3F) << 12) | ((b2 & 0x3F) << 6) | (b3 & 0x3F);
            return {true, 4, cp};
        } else if (lead >= 0xF1 && lead <= 0xF3) {
            if (remaining < 4) return {false, 3, 0};
            char32_t b1 = static_cast<unsigned char>(sv[start + 1]);
            char32_t b2 = static_cast<unsigned char>(sv[start + 2]);
            char32_t b3 = static_cast<unsigned char>(sv[start + 3]);
            if (b1 < 0x80 || b1 > 0xBF || b2 < 0x80 || b2 > 0xBF || b3 < 0x80 || b3 > 0xBF) return {false, 4, UNI_REPLACEMENT_CHAR};
            char32_t cp = ((lead & 0x07) << 18) | ((b1 & 0x3F) << 12) | ((b2 & 0x3F) << 6) | (b3 & 0x3F);
            return {true, 4, cp};
        } else if (lead == 0xF4) {
            // Max code point U+10FFFF
            if (remaining < 4) return {false, 3, 0};
            char32_t b1 = static_cast<unsigned char>(sv[start + 1]);
            char32_t b2 = static_cast<unsigned char>(sv[start + 2]);
            char32_t b3 = static_cast<unsigned char>(sv[start + 3]);
            if (b1 < 0x80 || b1 > 0x8F || b2 < 0x80 || b2 > 0xBF || b3 < 0x80 || b3 > 0xBF) return {false, 4, UNI_REPLACEMENT_CHAR};
            char32_t cp = ((lead & 0x07) << 18) | ((b1 & 0x3F) << 12) | ((b2 & 0x3F) << 6) | (b3 & 0x3F);
            return {true, 4, cp};
        }

        // Invalid leading byte
        return {false, 1, UNI_REPLACEMENT_CHAR};
    }

    static inline std::tuple<bool, std::size_t, char32_t> utf16_to_utf32_char(std::u16string_view sv, std::size_t index)
    {
        if (index >= sv.size()) return {false, 1, 0};

        char32_t first = (char32_t) sv[index];

        // Check if it's a surrogate
        if (first >= 0xD800 && first <= 0xDBFF) {
            // High surrogate: must be followed by a low surrogate
            if (index + 1 >= sv.size()) return {false, 2, 0};
            char32_t second = (char32_t) sv[index + 1];
            if (second < 0xDC00 || second > 0xDFFF) return {false, 2, UNI_REPLACEMENT_CHAR};

            // Decode surrogate pair
            char32_t high = first - 0xD800;
            char32_t low = second - 0xDC00;
            char32_t codepoint = (high << 10) + low + 0x10000;
            return {true, 2, codepoint};
        } else if (first >= 0xDC00 && first <= 0xDFFF) {
            // Isolated low surrogate — invalid
            return {false, 2, UNI_REPLACEMENT_CHAR};
        } else {
            // BMP character — valid single unit
            return {true, 1, static_cast<char32_t>(first)};
        }
    }

    // convert UTF32 char to UTF16, appending to UTF16 string
    static inline void utf32_to_utf16_char(char32_t cp, std::u16string &utf16 )
    {
        if (cp > 0x10FFFF || (cp >= 0xD800 && cp <= 0xDFFF)) {
            // Invalid code point: out of range or surrogate
            utf16 += static_cast<char16_t>(UNI_REPLACEMENT_CHAR);
        }
        else if (cp <= 0xFFFF) {
            // BMP character: single UTF-16 unit
            utf16 += static_cast<char16_t>(cp);
        } else {
            // Supplementary character: encode as surrogate pair
            cp -= 0x10000;
            char16_t high = static_cast<char16_t>((cp >> 10) + 0xD800);
            char16_t low  = static_cast<char16_t>((cp & 0x3FF) + 0xDC00);
            utf16 += high;
            utf16 += low;
        }
    }

    // given the UTF32 character, convert to UTF8 and add to utf8 string
    static inline void utf32_to_utf8_char(char32_t cp, std::string & utf8)
    {
        // Validate code point
        if (cp > 0x10FFFF || (cp >= 0xD800 && cp <= 0xDFFF))
        {
            cp = UNI_REPLACEMENT_CHAR;
        }

        if (cp <= 0x7F) {
            // 1-byte UTF-8
            utf8 += static_cast<char>(cp);
        } else if (cp <= 0x7FF) {
            // 2-byte UTF-8
            utf8 += static_cast<char>(0xC0 | ((cp >> 6) & 0x1F));
            utf8 += static_cast<char>(0x80 | (cp & 0x3F));
        } else if (cp <= 0xFFFF) {
            // 3-byte UTF-8
            utf8 += static_cast<char>(0xE0 | ((cp >> 12) & 0x0F));
            utf8 += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
            utf8 += static_cast<char>(0x80 | (cp & 0x3F));
        } else {
            // 4-byte UTF-8
            utf8 += static_cast<char>(0xF0 | ((cp >> 18) & 0x07));
            utf8 += static_cast<char>(0x80 | ((cp >> 12) & 0x3F));
            utf8 += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
            utf8 += static_cast<char>(0x80 | (cp & 0x3F));
        }
    }


    static inline std::u16string utf32_to_utf16_string(std::u32string_view str )
    {
        std::u16string utf16;
        utf16.reserve(str.length()*2); // should prevent regrowth

        for (std::size_t i = 0; i < str.size(); i++)
        {
            utf32_to_utf16_char( str[i], utf16 );
        }
        return utf16;
    }

    static inline std::string utf32_to_utf8_string(std::u32string_view str )
    {
        std::string utf8;
        utf8.reserve(str.length()*4); // should prevent regrowth

        for (std::size_t i = 0; i < str.size(); i++)
        {
            utf32_to_utf8_char( str[i], utf8 );
        }
        return utf8;
    }

    static inline std::u32string utf16_to_utf32_string(std::u16string_view str )
    {
        std::u32string utf32;
        utf32.reserve(str.length()); // should prevent regrowth

        for (std::size_t i = 0; i < str.size();)
        {
            auto [valid, length, u32ch] = utf16_to_utf32_char( str, i );
            i += length;
            if (u32ch)  // not null char
                utf32 += u32ch;
            else break;  // all done
        }
        return utf32;
    }

    static inline std::u32string utf8_to_utf32_string(std::string_view str )
    {
        std::u32string utf32;
        utf32.reserve(str.length()); // should prevent regrowth

        for (std::size_t i = 0; i < str.size();)
        {
            auto [valid, length, u32ch] = utf8_to_utf32_char( str, i );
            i += length;
            if (u32ch)  // not null char
                utf32 += u32ch;
            else break;  // all done
        }
        return utf32;
    }

    static inline std::u16string utf8_to_utf16_string(std::string_view str)
    {
        return utf32_to_utf16_string(utf8_to_utf32_string(str));
    }

    static inline std::string utf16_to_utf8_string(std::u16string_view str)
    {
        return utf32_to_utf8_string(utf16_to_utf32_string(str));
    }

#if 0
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
#endif
    // input string is assumed to be in system ansi code page. We are returning UTF16 data.
    static inline std::u16string ansi_to_utf16_string(std::string const & str)
    {
        std::u16string rets;

        if (!str.empty())
        {
            int size_needed = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, nullptr, 0); // compute size need to convert data
            if (size_needed > 0)
            {
                int buffsize = size_needed + 16; // add some extra space
                // Use temporary buffer
                std::vector<char16_t> buffer(buffsize, 0); // zero filled entire buffer

                int retval = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, (wchar_t *)buffer.data(), buffsize); // convert using ANSI system code page
                if ((retval > 0) && buffer[0])
                    rets = buffer.data();
            }
        }
        return rets;
    }

    // convert UTF16 data to ansi current code page. Ignoring data loss from characters that cannot be translated
    // return ANSI string
    static inline std::string utf16_to_ansi_string(std::u16string const& wstr)
    {
        std::string rets;
        if (!wstr.empty())
        {
            // Determine required buffer size
            int size_needed = WideCharToMultiByte(CP_ACP, 0, (wchar_t *)wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
            if (size_needed > 0)
            {
                int buffsize = size_needed + 16; // add some extra space
                // Use temporary buffer
                std::vector<char> buffer(buffsize, 0); // zero filled entire buffer

                // Perform conversion
                int converted = WideCharToMultiByte(CP_ACP, 0, (wchar_t *)wstr.c_str(), -1, buffer.data(), buffsize, nullptr, nullptr);

                if ((converted > 0) && buffer[0]) // have return data and its not empty
                    rets = buffer.data();
            }
        }
        return rets;
    }

    // conversion between utf16 and wstring. same size on win
    static inline std::u16string wstring_to_utf16(std::wstring_view str)
    {
        return std::u16string(str.begin(), str.end());
    }

    // used to convert command line args and filenames to utf8
    static inline std::string wstring_to_utf8(std::wstring_view str)
    {
        return utf16_to_utf8_string(wstring_to_utf16(str));
    }

    // conversion between utf16 and wstring. same size on win
    static inline std::wstring utf16_to_wstring(std::u16string_view str)
    {
        return std::wstring(str.begin(), str.end());
    }
    // used to convert interal messages to wstring for display
    static inline std::wstring utf8_to_wstring(std::string_view str)
    {
        return utf16_to_wstring(utf8_to_utf16_string(str));
    }

    static inline std::string ansi_to_utf8_string(std::string const & str)
    {
        return utf16_to_utf8_string(ansi_to_utf16_string(str));
    }

    static inline std::string utf8_to_ansi_string(std::string_view str)
    {
        return utf16_to_ansi_string(utf8_to_utf16_string(str));
    }

    // just for testing, not needed in our utility.
    static inline std::u32string ansi_to_utf32_string(std::string const & str)
    {
        return utf16_to_utf32_string(ansi_to_utf16_string(str));
    }
    static inline std::string utf32_to_ansi_string(std::u32string_view str)
    {
        return utf16_to_ansi_string(utf32_to_utf16_string(str));
    }


};

#endif // _MMUNICODE