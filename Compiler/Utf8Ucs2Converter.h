#pragma once
/**
* Utf8Ucs2Converter
*
* This code was written, while I answered my own question on:
* http://stackoverflow.com/questions/37947717/convert-utf-8-to-from-ucs-2-in-c98
*
* Please contribute on:
* https://github.com/RoelofBerg/Utf8Ucs2Converter
*
* This was written for C++98 compilers. If your compiler supports C++11 you can better use
* codecvt_utf8, as shown in the readme-file on above's github project.
*
* License:
* Just do what you want with this code and please contribute if you find some improvement.
*
* Roelof Berg, 2016 June 22d
*/

namespace utf {
    bool utf8CharToUcs2Char(const char* utf8Tok, wchar_t* ucs2Char, uint32_t* utf8TokLen);
    void ucs2CharToUtf8Char(const wchar_t ucs2Char, char* utf8Tok);
    std::wstring utf8ToUcs2(const std::string& utf8Str);
    std::string ucs2ToUtf8(const std::wstring& ucs2Str);
} // namespace utf
