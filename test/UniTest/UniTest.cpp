// UniTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <Windows.h>
#include <string>
#include <iostream>
#include "../../mmunicode.h"


// testing of unicode conversions
// all valid unicode characters from U+0001 to U+10FFFF are testing to/from UTF8/UTF16/UTF32
// Windows current code page ANSI to/from UTF32/UTF16/UTF8

void TestUni8_32()
{
    int firstByte = 0;
    int secondByte = 0;
    int thirdByte = 0;
    int fourthByte = 0;

    std::string utf8;
    std::u32string utf32;  // utf32 version
    std::string utf8back;  // utf8 converted back from utf32

    // test all single byte characters 0x01 to 0x7f
    utf8.clear();
    for (firstByte = 0; firstByte < 0x7f; firstByte++)
    {
        utf8 += (char)(unsigned char)(firstByte + 1);
    }

    utf32 = Unicode::utf8_to_utf32_string(utf8);
    utf8back = Unicode::utf32_to_utf8_string(utf32);

    if (utf8 == utf8back)
        std::cout << "UTF-8 <-> UTF-32 conversion successful!" << std::endl;
    else
        std::cout << "UTF-8 <-> UTF-32 conversion failed!" << std::endl;

    // test all double byte characters 0xC2 0x80 to 0xDF 0xBF

    utf8.clear();
    for (firstByte = 0xC2; firstByte <= 0xDF; firstByte++)
    {
        for (secondByte = 0x80; secondByte <= 0xBF; secondByte++)
        {
            utf8 += (char)(unsigned char)(firstByte);
            utf8 += (char)(unsigned char)(secondByte);
        }
    }
    utf32 = Unicode::utf8_to_utf32_string(utf8);
    utf8back = Unicode::utf32_to_utf8_string(utf32);
    if (utf8 == utf8)
        std::cout << "UTF-8 <-> UTF-32 conversion successful!" << std::endl;
    else
        std::cout << "UTF-8 <-> UTF-32 conversion failed!" << std::endl;

    // 3 byte sequencs have multiple ranges.
    // 3 byte lead E0 to E0
    utf8.clear();
    firstByte = 0xE0;
    for (firstByte = 0xe0; firstByte <= 0xe0; firstByte++)
    {
        for (secondByte = 0xA0; secondByte <= 0xBF; secondByte++)
        {
            for (thirdByte = 0x80; thirdByte <= 0xBF; thirdByte++)
            {
                utf8 += (char)(unsigned char)(firstByte);
                utf8 += (char)(unsigned char)(secondByte);
                utf8 += (char)(unsigned char)(thirdByte);
            }
        }
    }

    utf32    = Unicode::utf8_to_utf32_string(utf8);
    utf8back = Unicode::utf32_to_utf8_string(utf32);
    if (utf8 == utf8back)
        std::cout << "UTF-8 <-> UTF-32 conversion successful!" << std::endl;
    else
        std::cout << "UTF-8 <-> UTF-32 conversion failed!" << std::endl;

    // 3 byte lead E1 to EC
    utf8.clear();
    for (firstByte = 0xe1; firstByte <= 0xec; firstByte++)
    {
        for (secondByte = 0x80; secondByte <= 0xBF; secondByte++)
        {
            for (thirdByte = 0x80; thirdByte <= 0xBF; thirdByte++)
            {
                utf8 += (char)(unsigned char)(firstByte);
                utf8 += (char)(unsigned char)(secondByte);
                utf8 += (char)(unsigned char)(thirdByte);
            }
        }
    }

    utf32    = Unicode::utf8_to_utf32_string(utf8);
    utf8back = Unicode::utf32_to_utf8_string(utf32);
    if (utf8 == utf8back)
        std::cout << "UTF-8 <-> UTF-32 conversion successful!" << std::endl;
    else
        std::cout << "UTF-8 <-> UTF-32 conversion failed!" << std::endl;

    // 3 byte lead ED to ED
    utf8.clear();
    for (firstByte = 0xed; firstByte <= 0xed; firstByte++)
    {
        for (secondByte = 0x80; secondByte <= 0x9f; secondByte++)
        {
            for (thirdByte = 0x80; thirdByte <= 0xBF; thirdByte++)
            {
                utf8 += (char)(unsigned char)(firstByte);
                utf8 += (char)(unsigned char)(secondByte);
                utf8 += (char)(unsigned char)(thirdByte);
            }
        }
    }

    utf32    = Unicode::utf8_to_utf32_string(utf8);
    utf8back = Unicode::utf32_to_utf8_string(utf32);
    if (utf8 == utf8back)
        std::cout << "UTF-8 <-> UTF-32 conversion successful!" << std::endl;
    else
        std::cout << "UTF-8 <-> UTF-32 conversion failed!" << std::endl;


    // 3 byte lead EE to EF
    utf8.clear();
    for (firstByte = 0xee; firstByte <= 0xef; firstByte++)
    {
        for (secondByte = 0x80; secondByte <= 0xbf; secondByte++)
        {
            for (thirdByte = 0x80; thirdByte <= 0xBF; thirdByte++)
            {
                utf8 += (char)(unsigned char)(firstByte);
                utf8 += (char)(unsigned char)(secondByte);
                utf8 += (char)(unsigned char)(thirdByte);
            }
        }
    }

    utf32    = Unicode::utf8_to_utf32_string(utf8);
    utf8back = Unicode::utf32_to_utf8_string(utf32);
    if (utf8 == utf8back)
        std::cout << "UTF-8 <-> UTF-32 conversion successful!" << std::endl;
    else
        std::cout << "UTF-8 <-> UTF-32 conversion failed!" << std::endl;

    // 4 byte sequences
    // 4 byte lead F0 to F0
    utf8.clear();
    for (firstByte = 0xf0; firstByte <= 0xf0; firstByte++)
    {
        for (secondByte = 0x90; secondByte <= 0xbf; secondByte++)
        {
            for (thirdByte = 0x80; thirdByte <= 0xBF; thirdByte++)
            {
                for (fourthByte = 0x80; thirdByte <= 0xBF; thirdByte++)
                {
                    utf8 += (char)(unsigned char)(firstByte);
                    utf8 += (char)(unsigned char)(secondByte);
                    utf8 += (char)(unsigned char)(thirdByte);
                    utf8 += (char)(unsigned char)(fourthByte);
                }
            }
        }
    }

    utf32    = Unicode::utf8_to_utf32_string(utf8);
    utf8back = Unicode::utf32_to_utf8_string(utf32);
    if (utf8 == utf8back)
        std::cout << "UTF-8 <-> UTF-32 conversion successful!" << std::endl;
    else
        std::cout << "UTF-8 <-> UTF-32 conversion failed!" << std::endl;


    // 4 byte lead F1 to F3
    utf8.clear();
    for (firstByte = 0xf1; firstByte <= 0xf3; firstByte++)
    {
        for (secondByte = 0x80; secondByte <= 0xbf; secondByte++)
        {
            for (thirdByte = 0x80; thirdByte <= 0xBF; thirdByte++)
            {
                for (fourthByte = 0x80; thirdByte <= 0xBF; thirdByte++)
                {
                    utf8 += (char)(unsigned char)(firstByte);
                    utf8 += (char)(unsigned char)(secondByte);
                    utf8 += (char)(unsigned char)(thirdByte);
                    utf8 += (char)(unsigned char)(fourthByte);
                }
            }
        }
    }

    utf32    = Unicode::utf8_to_utf32_string(utf8);
    utf8back = Unicode::utf32_to_utf8_string(utf32);
    if (utf8 == utf8back)
        std::cout << "UTF-8 <-> UTF-32 conversion successful!" << std::endl;
    else
        std::cout << "UTF-8 <-> UTF-32 conversion failed!" << std::endl;

    // 4 byte lead F4 to F4
    utf8.clear();
    for (firstByte = 0xf4; firstByte <= 0xf4; firstByte++)
    {
        for (secondByte = 0x80; secondByte <= 0x8f; secondByte++)
        {
            for (thirdByte = 0x80; thirdByte <= 0xBF; thirdByte++)
            {
                for (fourthByte = 0x80; thirdByte <= 0xBF; thirdByte++)
                {
                    utf8 += (char)(unsigned char)(firstByte);
                    utf8 += (char)(unsigned char)(secondByte);
                    utf8 += (char)(unsigned char)(thirdByte);
                    utf8 += (char)(unsigned char)(fourthByte);
                }
            }
        }
    }

    utf32    = Unicode::utf8_to_utf32_string(utf8);
    utf8back = Unicode::utf32_to_utf8_string(utf32);
    if (utf8 == utf8back)
        std::cout << "UTF-8 <-> UTF-32 conversion successful!" << std::endl;
    else
        std::cout << "UTF-8 <-> UTF-32 conversion failed!" << std::endl;
}


void TestUni8_16()
{
    int firstByte = 0;
    int secondByte = 0;
    int thirdByte = 0;
    int fourthByte = 0;

    std::string utf8;
    std::u16string utf16;  // utf16 version
    std::string utf8back;  // utf8 converted back from utf16

    // test all single byte characters 0x01 to 0x7f
    utf8.clear();
    for (firstByte = 0; firstByte < 0x7f; firstByte++)
    {
        utf8 += (char)(unsigned char)(firstByte + 1);
    }

    utf16 = Unicode::utf8_to_utf16_string(utf8);
    utf8back = Unicode::utf16_to_utf8_string(utf16);

    if (utf8 == utf8back)
        std::cout << "UTF-8 <-> UTF-16 conversion successful!" << std::endl;
    else
        std::cout << "UTF-8 <-> UTF-16 conversion failed!" << std::endl;

    // test all double byte characters 0xC2 0x80 to 0xDF 0xBF

    utf8.clear();
    for (firstByte = 0xC2; firstByte <= 0xDF; firstByte++)
    {
        for (secondByte = 0x80; secondByte <= 0xBF; secondByte++)
        {
            utf8 += (char)(unsigned char)(firstByte);
            utf8 += (char)(unsigned char)(secondByte);
        }
    }
    utf16 = Unicode::utf8_to_utf16_string(utf8);
    utf8back = Unicode::utf16_to_utf8_string(utf16);
    if (utf8 == utf8)
        std::cout << "UTF-8 <-> UTF-16 conversion successful!" << std::endl;
    else
        std::cout << "UTF-8 <-> UTF-16 conversion failed!" << std::endl;

    // 3 byte sequencs have multiple ranges.
    // 3 byte lead E0 to E0
    utf8.clear();
    firstByte = 0xE0;
    for (firstByte = 0xe0; firstByte <= 0xe0; firstByte++)
    {
        for (secondByte = 0xA0; secondByte <= 0xBF; secondByte++)
        {
            for (thirdByte = 0x80; thirdByte <= 0xBF; thirdByte++)
            {
                utf8 += (char)(unsigned char)(firstByte);
                utf8 += (char)(unsigned char)(secondByte);
                utf8 += (char)(unsigned char)(thirdByte);
            }
        }
    }

    utf16    = Unicode::utf8_to_utf16_string(utf8);
    utf8back = Unicode::utf16_to_utf8_string(utf16);
    if (utf8 == utf8back)
        std::cout << "UTF-8 <-> UTF-16 conversion successful!" << std::endl;
    else
        std::cout << "UTF-8 <-> UTF-16 conversion failed!" << std::endl;

    // 3 byte lead E1 to EC
    utf8.clear();
    for (firstByte = 0xe1; firstByte <= 0xec; firstByte++)
    {
        for (secondByte = 0x80; secondByte <= 0xBF; secondByte++)
        {
            for (thirdByte = 0x80; thirdByte <= 0xBF; thirdByte++)
            {
                utf8 += (char)(unsigned char)(firstByte);
                utf8 += (char)(unsigned char)(secondByte);
                utf8 += (char)(unsigned char)(thirdByte);
            }
        }
    }

    utf16    = Unicode::utf8_to_utf16_string(utf8);
    utf8back = Unicode::utf16_to_utf8_string(utf16);
    if (utf8 == utf8back)
        std::cout << "UTF-8 <-> UTF-16 conversion successful!" << std::endl;
    else
        std::cout << "UTF-8 <-> UTF-16 conversion failed!" << std::endl;

    // 3 byte lead ED to ED
    utf8.clear();
    for (firstByte = 0xed; firstByte <= 0xed; firstByte++)
    {
        for (secondByte = 0x80; secondByte <= 0x9f; secondByte++)
        {
            for (thirdByte = 0x80; thirdByte <= 0xBF; thirdByte++)
            {
                utf8 += (char)(unsigned char)(firstByte);
                utf8 += (char)(unsigned char)(secondByte);
                utf8 += (char)(unsigned char)(thirdByte);
            }
        }
    }

    utf16    = Unicode::utf8_to_utf16_string(utf8);
    utf8back = Unicode::utf16_to_utf8_string(utf16);
    if (utf8 == utf8back)
        std::cout << "UTF-8 <-> UTF-16 conversion successful!" << std::endl;
    else
        std::cout << "UTF-8 <-> UTF-16 conversion failed!" << std::endl;


    // 3 byte lead EE to EF
    utf8.clear();
    for (firstByte = 0xee; firstByte <= 0xef; firstByte++)
    {
        for (secondByte = 0x80; secondByte <= 0xbf; secondByte++)
        {
            for (thirdByte = 0x80; thirdByte <= 0xBF; thirdByte++)
            {
                utf8 += (char)(unsigned char)(firstByte);
                utf8 += (char)(unsigned char)(secondByte);
                utf8 += (char)(unsigned char)(thirdByte);
            }
        }
    }

    utf16    = Unicode::utf8_to_utf16_string(utf8);
    utf8back = Unicode::utf16_to_utf8_string(utf16);
    if (utf8 == utf8back)
        std::cout << "UTF-8 <-> UTF-16 conversion successful!" << std::endl;
    else
        std::cout << "UTF-8 <-> UTF-16 conversion failed!" << std::endl;

    // 4 byte sequences
    // 4 byte lead F0 to F0
    utf8.clear();
    for (firstByte = 0xf0; firstByte <= 0xf0; firstByte++)
    {
        for (secondByte = 0x90; secondByte <= 0xbf; secondByte++)
        {
            for (thirdByte = 0x80; thirdByte <= 0xBF; thirdByte++)
            {
                for (fourthByte = 0x80; thirdByte <= 0xBF; thirdByte++)
                {
                    utf8 += (char)(unsigned char)(firstByte);
                    utf8 += (char)(unsigned char)(secondByte);
                    utf8 += (char)(unsigned char)(thirdByte);
                    utf8 += (char)(unsigned char)(fourthByte);
                }
            }
        }
    }

    utf16    = Unicode::utf8_to_utf16_string(utf8);
    utf8back = Unicode::utf16_to_utf8_string(utf16);
    if (utf8 == utf8back)
        std::cout << "UTF-8 <-> UTF-16 conversion successful!" << std::endl;
    else
        std::cout << "UTF-8 <-> UTF-16 conversion failed!" << std::endl;


    // 4 byte lead F1 to F3
    utf8.clear();
    for (firstByte = 0xf1; firstByte <= 0xf3; firstByte++)
    {
        for (secondByte = 0x80; secondByte <= 0xbf; secondByte++)
        {
            for (thirdByte = 0x80; thirdByte <= 0xBF; thirdByte++)
            {
                for (fourthByte = 0x80; thirdByte <= 0xBF; thirdByte++)
                {
                    utf8 += (char)(unsigned char)(firstByte);
                    utf8 += (char)(unsigned char)(secondByte);
                    utf8 += (char)(unsigned char)(thirdByte);
                    utf8 += (char)(unsigned char)(fourthByte);
                }
            }
        }
    }

    utf16    = Unicode::utf8_to_utf16_string(utf8);
    utf8back = Unicode::utf16_to_utf8_string(utf16);
    if (utf8 == utf8back)
        std::cout << "UTF-8 <-> UTF-16 conversion successful!" << std::endl;
    else
        std::cout << "UTF-8 <-> UTF-16 conversion failed!" << std::endl;

    // 4 byte lead F4 to F4
    utf8.clear();
    for (firstByte = 0xf4; firstByte <= 0xf4; firstByte++)
    {
        for (secondByte = 0x80; secondByte <= 0x8f; secondByte++)
        {
            for (thirdByte = 0x80; thirdByte <= 0xBF; thirdByte++)
            {
                for (fourthByte = 0x80; thirdByte <= 0xBF; thirdByte++)
                {
                    utf8 += (char)(unsigned char)(firstByte);
                    utf8 += (char)(unsigned char)(secondByte);
                    utf8 += (char)(unsigned char)(thirdByte);
                    utf8 += (char)(unsigned char)(fourthByte);
                }
            }
        }
    }

    utf16    = Unicode::utf8_to_utf16_string(utf8);
    utf8back = Unicode::utf16_to_utf8_string(utf16);
    if (utf8 == utf8back)
        std::cout << "UTF-8 <-> UTF-16 conversion successful!" << std::endl;
    else
        std::cout << "UTF-8 <-> UTF-16 conversion failed!" << std::endl;
}

void TestUni32_8()
{
    int firstChar = 0;
    std::u32string utf32;
    std::string utf8;      // utf8 version
    std::u32string utf32back;  // utf32 converted back from utf8

    // test all single byte characters 0x01 to 0x7f
    utf32.clear();
    for (firstChar = 0; firstChar < 0x7f; firstChar++)
    {
        utf32 += (char32_t)(firstChar + 1);
    }
    utf8      = Unicode::utf32_to_utf8_string(utf32);
    utf32back = Unicode::utf8_to_utf32_string(utf8);
    if (utf32 == utf32back)
        std::cout << "UTF-32 <-> UTF-8 conversion successful!" << std::endl;
    else
        std::cout << "UTF-32 <-> UTF-8 conversion failed!" << std::endl;

    // test all double byte characters 0x80 to 0x7ff
    utf32.clear();
    for (firstChar = 0x80; firstChar <= 0x7ff; firstChar++)
    {
        utf32 += (char32_t)(firstChar);
    }
    utf8      = Unicode::utf32_to_utf8_string(utf32);
    utf32back = Unicode::utf8_to_utf32_string(utf8);
    if (utf32 == utf32back)
        std::cout << "UTF-32 <-> UTF-8 conversion successful!" << std::endl;
    else
        std::cout << "UTF-32 <-> UTF-8 conversion failed!" << std::endl;

    // test all triple byte characters 0x800 to 0xffff
    utf32.clear();
    for (firstChar = 0x800; firstChar <= 0xffff; firstChar++)
    {
        // skip surrogates
        if ((firstChar >= 0xd800) && (firstChar <= 0xdfff))
            continue;
        utf32 += (char32_t)(firstChar);
    }
    utf8      = Unicode::utf32_to_utf8_string(utf32);
    utf32back = Unicode::utf8_to_utf32_string(utf8);
    if (utf32 == utf32back)
        std::cout << "UTF-32 <-> UTF-8 conversion successful!" << std::endl;
    else
        std::cout << "UTF-32 <-> UTF-8 conversion failed!" << std::endl;

    // test all four byte characters 0x10000 to 0x10ffff
    utf32.clear();
    for (firstChar = 0x10000; firstChar <= 0x10ffff; firstChar++)
    {
        utf32 += (char32_t)(firstChar);
    }
    utf8      = Unicode::utf32_to_utf8_string(utf32);
    utf32back = Unicode::utf8_to_utf32_string(utf8);
    if (utf32 == utf32back)
        std::cout << "UTF-32 <-> UTF-8 conversion successful!" << std::endl;
    else
        std::cout << "UTF-32 <-> UTF-8 conversion failed!" << std::endl;
}

void TestUni32_16()
{
    int firstChar = 0;
    std::u32string utf32;
    std::u16string utf16;      // utf16 version
    std::u32string utf32back;  // utf32 converted back from utf16

    // test all single byte characters 0x01 to 0x7f
    utf32.clear();
    for (firstChar = 0; firstChar < 0x7f; firstChar++)
    {
        utf32 += (char32_t)(firstChar + 1);
    }
    utf16     = Unicode::utf32_to_utf16_string(utf32);
    utf32back = Unicode::utf16_to_utf32_string(utf16);
    if (utf32 == utf32back)
        std::cout << "UTF-32 <-> UTF-16 conversion successful!" << std::endl;
    else
        std::cout << "UTF-32 <-> UTF-16 conversion failed!" << std::endl;

    // test all double byte characters 0x80 to 0x7ff
    utf32.clear();
    for (firstChar = 0x80; firstChar <= 0x7ff; firstChar++)
    {
        utf32 += (char32_t)(firstChar);
    }
    utf16     = Unicode::utf32_to_utf16_string(utf32);
    utf32back = Unicode::utf16_to_utf32_string(utf16);
    if (utf32 == utf32back)
        std::cout << "UTF-32 <-> UTF-16 conversion successful!" << std::endl;
    else
        std::cout << "UTF-32 <-> UTF-16 conversion failed!" << std::endl;

    // test all triple byte characters 0x800 to 0xffff
    utf32.clear();
    for (firstChar = 0x800; firstChar <= 0xffff; firstChar++)
    {
        // skip surrogates
        if ((firstChar >= 0xd800) && (firstChar <= 0xdfff))
            continue;
        utf32 += (char32_t)(firstChar);
    }
    utf16     = Unicode::utf32_to_utf16_string(utf32);
    utf32back = Unicode::utf16_to_utf32_string(utf16);
    if (utf32 == utf32back)
        std::cout << "UTF-32 <-> UTF-16 conversion successful!" << std::endl;
    else
        std::cout << "UTF-32 <-> UTF-16 conversion failed!" << std::endl;

    // test all four byte characters 0x10000 to 0x10ffff
    utf32.clear();
    for (firstChar = 0x10000; firstChar <= 0x10ffff; firstChar++)
    {
        utf32 += (char32_t)(firstChar);
    }
    utf16     = Unicode::utf32_to_utf16_string(utf32);
    utf32back = Unicode::utf16_to_utf32_string(utf16);
    if (utf32 == utf32back)
        std::cout << "UTF-32 <-> UTF-16 conversion successful!" << std::endl;
    else
        std::cout << "UTF-32 <-> UTF-16 conversion failed!" << std::endl;
}

void TestUni16_8()
{
    int firstChar = 0;
    int secondChar = 0;

    std::u16string utf16;
    std::string utf8;          // utf8 version
    std::u16string utf16back;  // utf16 converted back from utf8

    // test all single byte characters 0x01 to 0x7f
    utf16.clear();
    for (firstChar = 0; firstChar < 0x7f; firstChar++)
    {
        utf16 += (char16_t)(firstChar + 1);
    }
    utf8      = Unicode::utf16_to_utf8_string(utf16);
    utf16back = Unicode::utf8_to_utf16_string(utf8);
    if (utf16 == utf16back)
        std::cout << "UTF-16 <-> UTF-8 conversion successful!" << std::endl;
    else
        std::cout << "UTF-16 <-> UTF-8 conversion failed!" << std::endl;

    // test all double byte characters 0x80 to 0x7ff
    utf16.clear();
    for (firstChar = 0x80; firstChar <= 0x7ff; firstChar++)
    {
        utf16 += (char16_t)(firstChar);
    }
    utf8      = Unicode::utf16_to_utf8_string(utf16);
    utf16back = Unicode::utf8_to_utf16_string(utf8);
    if (utf16 == utf16back)
        std::cout << "UTF-16 <-> UTF-8 conversion successful!" << std::endl;
    else
        std::cout << "UTF-16 <-> UTF-8 conversion failed!" << std::endl;

    // test all triple byte characters 0x800 to 0xffff
    utf16.clear();
    for (firstChar = 0x800; firstChar <= 0xffff; firstChar++)
    {
        // skip surrogates
        if ((firstChar >= 0xd800) && (firstChar <= 0xdfff))
            continue;
        utf16 += (char16_t)(firstChar);
    }
    utf8      = Unicode::utf16_to_utf8_string(utf16);
    utf16back = Unicode::utf8_to_utf16_string(utf8);
    if (utf16 == utf16back)
        std::cout << "UTF-16 <-> UTF-8 conversion successful!" << std::endl;
    else
        std::cout << "UTF-16 <-> UTF-8 conversion failed!" << std::endl;

    // test all four byte characters 0x10000 to 0x10ffff
    utf16.clear();
    for (firstChar = 0xd800; firstChar <= 0xdbff; firstChar++)
    {
        for (secondChar = 0xdc00; secondChar <= 0xdfff; secondChar++)
        {
            utf16 += (char16_t)(firstChar);
            utf16 += (char16_t)(secondChar);
        }
    }
    utf8      = Unicode::utf16_to_utf8_string(utf16);
    utf16back = Unicode::utf8_to_utf16_string(utf8);
    if (utf16 == utf16back)
        std::cout << "UTF-16 <-> UTF-8 conversion successful!" << std::endl;
    else
        std::cout << "UTF-16 <-> UTF-8 conversion failed!" << std::endl;
}


void TestUni16_32()
{
    int firstChar = 0;
    int secondChar = 0;

    std::u16string utf16;
    std::u32string utf32;      // utf32 version
    std::u16string utf16back;  // utf16 converted back from utf32

    // test all single byte characters 0x01 to 0x7f
    utf16.clear();
    for (firstChar = 0; firstChar < 0x7f; firstChar++)
    {
        utf16 += (char16_t)(firstChar + 1);
    }
    utf32     = Unicode::utf16_to_utf32_string(utf16);
    utf16back = Unicode::utf32_to_utf16_string(utf32);
    if (utf16 == utf16back)
        std::cout << "UTF-16 <-> UTF-32 conversion successful!" << std::endl;
    else
        std::cout << "UTF-16 <-> UTF-32 conversion failed!" << std::endl;

    // test all double byte characters 0x80 to 0x7ff
    utf16.clear();
    for (firstChar = 0x80; firstChar <= 0x7ff; firstChar++)
    {
        utf16 += (char16_t)(firstChar);
    }
    utf32      = Unicode::utf16_to_utf32_string(utf16);
    utf16back = Unicode::utf32_to_utf16_string(utf32);
    if (utf16 == utf16back)
        std::cout << "UTF-16 <-> UTF-32 conversion successful!" << std::endl;
    else
        std::cout << "UTF-16 <-> UTF-32 conversion failed!" << std::endl;

    // test all triple byte characters 0x800 to 0xffff
    utf16.clear();
    for (firstChar = 0x800; firstChar <= 0xffff; firstChar++)
    {
        // skip surrogates
        if ((firstChar >= 0xd800) && (firstChar <= 0xdfff))
            continue;
        utf16 += (char16_t)(firstChar);
    }
    utf32     = Unicode::utf16_to_utf32_string(utf16);
    utf16back = Unicode::utf32_to_utf16_string(utf32);
    if (utf16 == utf16back)
        std::cout << "UTF-16 <-> UTF-32 conversion successful!" << std::endl;
    else
        std::cout << "UTF-16 <-> UTF-32 conversion failed!" << std::endl;

    // test all four byte characters 0x10000 to 0x10ffff
    utf16.clear();
    for (firstChar = 0xd800; firstChar <= 0xdbff; firstChar++)
    {
        for (secondChar = 0xdc00; secondChar <= 0xdfff; secondChar++)
        {
            utf16 += (char16_t)(firstChar);
            utf16 += (char16_t)(secondChar);
        }
    }
    utf32     = Unicode::utf16_to_utf32_string(utf16);
    utf16back = Unicode::utf32_to_utf16_string(utf32);
    if (utf16 == utf16back)
        std::cout << "UTF-16 <-> UTF-32 conversion successful!" << std::endl;
    else
        std::cout << "UTF-16 <-> UTF-32 conversion failed!" << std::endl;
}

void TestUniANSI_8()
{
    int firstByte = 0;

    std::string utf8;      // convert to utf8
    std::string ansi;      // start with ansi version
    std::string ansiback;  // ansi converted back from utf8

    // test all single byte characters 0x01 to 07f
    ansi.clear();
    for (firstByte = 1; firstByte <= 0x7f; firstByte++)
    {
        ansi += (char)(unsigned char)(firstByte);
    }

    utf8     = Unicode::ansi_to_utf8_string(ansi);
    ansiback = Unicode::utf8_to_ansi_string(utf8);

    if (ansi == ansiback)
        std::cout << "ANSI <-> UTF-8 conversion successful!" << std::endl;
    else
        std::cout << "ANSI <-> UTF-8 conversion failed!" << std::endl;

    // test all single byte characters 0x80 to 0xff
    ansi.clear();
    for (firstByte = 0x80; firstByte <= 0xff; firstByte++)
    {
        ansi += (char)(unsigned char)(firstByte);
    }

    utf8     = Unicode::ansi_to_utf8_string(ansi);
    ansiback = Unicode::utf8_to_ansi_string(utf8);

    if (ansi == ansiback)
        std::cout << "ANSI <-> UTF-8 conversion successful!" << std::endl;
    else
        std::cout << "ANSI <-> UTF-8 conversion failed!" << std::endl;

}

void TestUniANSI_16()
{
    int firstByte = 0;

    std::u16string utf16;      // convert to utf16
    std::string ansi;      // start with ansi version
    std::string ansiback;  // ansi converted back from utf16

    // test all single byte characters 0x01 to 07f
    ansi.clear();
    for (firstByte = 1; firstByte <= 0x7f; firstByte++)
    {
        ansi += (char)(unsigned char)(firstByte);
    }

    utf16    = Unicode::ansi_to_utf16_string(ansi);
    ansiback = Unicode::utf16_to_ansi_string(utf16);

    if (ansi == ansiback)
        std::cout << "ANSI <-> UTF-16 conversion successful!" << std::endl;
    else
        std::cout << "ANSI <-> UTF-16 conversion failed!" << std::endl;

    // test all single byte characters 0x160 to 0xff
    ansi.clear();
    for (firstByte = 0x80; firstByte <= 0xff; firstByte++)
    {
        ansi += (char)(unsigned char)(firstByte);
    }

    utf16    = Unicode::ansi_to_utf16_string(ansi);
    ansiback = Unicode::utf16_to_ansi_string(utf16);

    if (ansi == ansiback)
        std::cout << "ANSI <-> UTF-16 conversion successful!" << std::endl;
    else
        std::cout << "ANSI <-> UTF-16 conversion failed!" << std::endl;

}

void TestUniANSI_32()
{
    int firstByte = 0;

    std::u32string utf32;      // convert to utf32
    std::string ansi;      // start with ansi version
    std::string ansiback;  // ansi converted back from utf32

    // test all single byte characters 0x01 to 07f
    ansi.clear();
    for (firstByte = 1; firstByte <= 0x7f; firstByte++)
    {
        ansi += (char)(unsigned char)(firstByte);
    }

    utf32    = Unicode::ansi_to_utf32_string(ansi);
    ansiback = Unicode::utf32_to_ansi_string(utf32);

    if (ansi == ansiback)
        std::cout << "ANSI <-> UTF-32 conversion successful!" << std::endl;
    else
        std::cout << "ANSI <-> UTF-32 conversion failed!" << std::endl;

    // test all single byte characters 0x320 to 0xff
    ansi.clear();
    for (firstByte = 0x80; firstByte <= 0xff; firstByte++)
    {
        ansi += (char)(unsigned char)(firstByte);
    }

    utf32    = Unicode::ansi_to_utf32_string(ansi);
    ansiback = Unicode::utf32_to_ansi_string(utf32);

    if (ansi == ansiback)
        std::cout << "ANSI <-> UTF-32 conversion successful!" << std::endl;
    else
        std::cout << "ANSI <-> UTF-32 conversion failed!" << std::endl;
}


void TestUnicode()
{
    TestUni8_32();
    TestUni8_16();
    TestUni32_8();
    TestUni32_16();
    TestUni16_32();
    TestUni16_8();

    TestUniANSI_8();  // we can only start with ansi and go to UTF reliability
    TestUniANSI_16();  // we can only start with ansi and go to UTF reliability
    TestUniANSI_32();  // we can only start with ansi and go to UTF reliability

}

int main()
{
    TestUnicode();
}

