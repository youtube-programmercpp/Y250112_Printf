#include "mini_scanf.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdexcept>
#include <Windows.h>

template<class T>static _Check_return_ T      __cdecl powt(_In_ T      _X, _In_ T      _Y);
template<       >static _Check_return_ double __cdecl powt(_In_ double _X, _In_ double _Y){return pow (_X, _Y);}
template<       >static _Check_return_ float  __cdecl powt(_In_ float  _X, _In_ float  _Y){return powf(_X, _Y);}

/// <summary>
/// 数字列を走査する
/// </summary>
/// <typeparam name="T">数値型</typeparam>
/// <param name="ch">文字</param>
/// <returns>数値</returns>
template<class T>
static std::enable_if_t<std::is_integral_v<T>, T>
_Success_(return != false)
scan_digit_sequence
( _Inout_ int& ch   
)
{
	T value = static_cast<T>(ch - '0');
	while ((ch = getchar()) != EOF && isdigit((unsigned char)ch)) {
		value *= static_cast<T>(10);
		value += static_cast<T>(ch - '0');
	}
	return value;
}

/// <summary>
/// 小数部を走査する
/// </summary>
/// <typeparam name="T">数値型</typeparam>
/// <param name="ch">文字</param>
/// <returns>小数値</returns>
template<class T>
static std::enable_if_t<std::is_floating_point_v<T>, T>
scan_fractional_part(int& ch)
{
	auto x     = static_cast<T>(10);
	auto value = (ch - '0') / x;
	while ((ch = getchar()) != EOF && isdigit((unsigned char)ch)) {
		x *= 10;
		value += (ch - '0') / x;
	}
	return value;
}

/// <summary>
/// 省略可能な指数部を走査し、仮数を掛けた値を返す
/// </summary>
/// <typeparam name="T">数値型</typeparam>
/// <param name="ch">文字</param>
/// <param name="value">仮数</param>
/// <returns>仮数×指数部</returns>
template<class T>
static std::enable_if_t<std::is_floating_point_v<T>, T>
scan_optional_exponent_part
( _Inout_ int    & ch   
, _In_    const T  value
)
{
	switch (ch) {
	case 'E':
	case 'e':
		if ((ch = getchar()) == '-') {
			if ((ch = getchar()) == EOF || !isdigit((unsigned char)ch))
				throw std::runtime_error("Eの後に指数がありませんでした。");
			else
				return value * powt<T>(10, -static_cast<T>(scan_digit_sequence<unsigned>(ch)));
		}
		else {
			if (ch == '+')
				ch = getchar();
			if (ch == EOF || !isdigit((unsigned char)ch))
				throw std::runtime_error("Eの後に指数がありませんでした。");
			else
				return value * powt<T>(10, static_cast<T>(scan_digit_sequence<unsigned>(ch)));
		}
	default:
		return value;
	}
}

/// <summary>
/// 仮数部を走査する
/// </summary>
/// <typeparam name="T">数値型</typeparam>
/// <param name="ch">文字</param>
/// <param name="value">仮数</param>
/// <returns>仮数部があれば true、無ければ false</returns>
template<class T>
static std::enable_if_t<std::is_floating_point_v<T>, bool>
_Success_(return != false)
scan_mantissa_part
( _Inout_ int& ch   
, _Out_   T  & value
)
{
	if (ch == '.') {
		if ((ch = getchar()) == EOF || !isdigit((unsigned char)ch))
			throw std::runtime_error("小数点の後に数字がありませんでした。");
		else {
			value = scan_fractional_part<T>(ch);
			return true;
		}
	}
	else if (isdigit((unsigned char)ch)) {
		value = ch - static_cast<T>('0');
		while ((ch = getchar()) != EOF && isdigit((unsigned char)ch)) {
			value *= 10;
			value += ch - '0';
		}
		if (ch == '.') {
			if ((ch = getchar()) != EOF && isdigit((unsigned char)ch))
				value += scan_fractional_part<T>(ch);
		}
		return true;
	}
	else
		return false;
}

/// <summary>
/// 符号を含まない実数を走査する
/// </summary>
/// <typeparam name="T">実数型</typeparam>
/// <param name="ch">文字</param>
/// <param name="value">実数値</param>
/// <returns>実数値があれば true、無ければ false</returns>
template<class T>
static std::enable_if_t<std::is_floating_point_v<T>, bool>
scan_without_sign
( _Inout_ int& ch   
, _Out_   T  & value
)
{
	if (scan_mantissa_part(ch, value)) {
		value = scan_optional_exponent_part(ch, value);
		return true;
	}
	else
		return false;
}

/// <summary>
/// 符号を含まない整数を走査する
/// </summary>
/// <typeparam name="T">整数型</typeparam>
/// <param name="ch">文字</param>
/// <param name="value">整数値</param>
/// <returns>整数値があれば true、無ければ false</returns>
template<class T>
static std::enable_if_t<std::is_integral_v<T>, bool>
_Success_(return != false)
scan_without_sign
( _Inout_ int& ch   
, _Out_   T  & value
)
{
	if (isdigit((unsigned char)ch)) {
		value = scan_digit_sequence<T>(ch);
		return true;
	}
	else
		return false;
}

/// <summary>
/// 数値を走査する（符号無し整数を除く）
/// </summary>
/// <typeparam name="T">数値型</typeparam>
/// <param name="ch">文字</param>
/// <param name="value">数値</param>
/// <returns>数値があれば true、無ければ false</returns>
template<class T>
static std::enable_if_t<!std::is_unsigned_v<T> && std::is_arithmetic_v<T>, bool>
scan_arithmetic
( _Inout_ int& ch   
, _Out_   T  & value
)
{
	while (isspace((unsigned char)ch))
		ch = getchar();
	switch (ch) {
	case '+':
		if (scan_without_sign<T>(ch = getchar(), value)) {
			return true;
		}
		else
			throw std::runtime_error("符号の後に数値が記述されていませんでした。");
	case '-':
		if (scan_without_sign<T>(ch = getchar(), value)) {
			value *= static_cast<T>(-1);
			return true;
		}
		else
			throw std::runtime_error("符号の後に数値が記述されていませんでした。");
	default :
		if (scan_without_sign<T>(ch, value)) {
			return true;
		}
		else
			return false;
	}
}

/// <summary>
/// 符号無し整数を走査する
/// </summary>
/// <typeparam name="T">符号無し整数型</typeparam>
/// <param name="ch">文字</param>
/// <param name="value">符号無し整数値</param>
/// <returns>符号無し整数値があれば true、無ければ false</returns>
template<class T>
static std::enable_if_t<std::is_unsigned_v<T>, bool>
_Success_(return != false)
scan_arithmetic
( _Inout_ int& ch   
, _Out_   T  & value
)
{
	while (isspace((unsigned char)ch))
		ch = getchar();
	if (isdigit((unsigned char)ch)) {
		value = scan_digit_sequence<T>(ch);
		return true;
	}
	else
		return false;
}

/// <summary>
/// 文字列を走査する
/// </summary>
/// <param name="ch">文字</param>
/// <param name="s">文字列の受取先領域</param>
/// <returns>文字列があれば true、無ければ false</returns>
static bool scan_character_sequence(int& ch, char* s)
{
	while (isspace((unsigned char)ch))
		ch = getchar();
	if (ch == EOF)
		return false;
	do {
		*s++ = ch;
		ch = getchar();
	} while (ch != EOF && !isspace((unsigned char)ch));
	*s = '\0';
	return true;
}

/// <summary>
/// 書式に従って走査する
/// </summary>
/// <param name="format">書式</param>
/// <param name="ch">文字</param>
/// <param name="args">引数リスト</param>
/// <returns>書式に対応したデータが取り込めたときは true、取り込めなかったときは false</returns>
static bool scan_format(const char*& format, int& ch, va_list& args)
{
	switch (*format) {
	case 'd': ++format; return scan_arithmetic(ch, *va_arg(args, int     *));
	case 'u': ++format; return scan_arithmetic(ch, *va_arg(args, unsigned*));
	case 'f': ++format; return scan_arithmetic(ch, *va_arg(args, float   *));
	case 'l':
		switch (*++format) {
		case 'd': ++format; return scan_arithmetic(ch, *va_arg(args,          long*));
		case 'u': ++format; return scan_arithmetic(ch, *va_arg(args, unsigned long*));
		case 'f': ++format; return scan_arithmetic(ch, *va_arg(args,        double*));
		case 'l':
			switch (*++format) {
			case 'd': ++format; return scan_arithmetic(ch, *va_arg(args,          long long*));
			case 'u': ++format; return scan_arithmetic(ch, *va_arg(args, unsigned long long*));
			default:
				throw std::runtime_error("%llに続く書式指定文字が正しくありません。");
			}
		default:
			throw std::runtime_error("%lに続く書式指定文字が正しくありません。");
		}
	case 'h':
		switch (*++format) {
		case 'd': ++format; return scan_arithmetic(ch, *va_arg(args,          short*));
		case 'u': ++format; return scan_arithmetic(ch, *va_arg(args, unsigned short*));
		case 'h':
			switch (*++format) {
			case 'd': ++format; return scan_arithmetic(ch, *va_arg(args,          char*));
			case 'u': ++format; return scan_arithmetic(ch, *va_arg(args, unsigned char*));
			default:
				throw std::runtime_error("%hhに続く書式指定文字が正しくありません。");
			}
		default:
			throw std::runtime_error("%hに続く書式指定文字が正しくありません。");
		}
	case 's': //%s : 文字列
		++format; return scan_character_sequence(ch, va_arg(args, char*));
	default:
		throw std::runtime_error("%に続く書式指定文字が正しくありません。");
	}
}

/// <summary>
/// 小型のscanf（幅の指定は不可、%S, %ls の指定不可）
/// </summary>
/// <param name="format">書式</param>
/// <param name="...">データを受け取るためのポインタ群</param>
/// <returns>取り込めたデータの個数（一つも取り込めずに入力装置でのエラーが発生したとき、EOFに達した時、EOFを返す）</returns>
extern "C" int mini_scanf(const char* format, ...)
{
	if (*format == '\0')
		return 0;
	else {
		int ch = getchar();
		va_list args;
		va_start(args, format);
		int count = 0;
		try {
			for (;;) {
				if (ch == EOF)
					break;
				else if (*format == '%') {
					if (scan_format(++format, ch, args))
						++count;
					else
						break;
				}
				else if (isspace((unsigned char)*format)) {
					while (isspace((unsigned char)ch))
						ch = getchar();
				}
				else {
					if (*format == ch) {
						++format;
						ch = getchar();
					}
					else
						break;
				}
			}
		}
		catch (const std::exception& e) {
			OutputDebugStringA(e.what());
			OutputDebugStringA("\n");
		}
		va_end(args);
		if (count == 0 && ch == EOF)
			return EOF;
		else
			return count;
	}
}
