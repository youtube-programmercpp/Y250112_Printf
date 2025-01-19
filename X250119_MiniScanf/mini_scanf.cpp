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
/// ������𑖍�����
/// </summary>
/// <typeparam name="T">���l�^</typeparam>
/// <param name="ch">����</param>
/// <returns>���l</returns>
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
/// �������𑖍�����
/// </summary>
/// <typeparam name="T">���l�^</typeparam>
/// <param name="ch">����</param>
/// <returns>�����l</returns>
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
/// �ȗ��\�Ȏw�����𑖍����A�������|�����l��Ԃ�
/// </summary>
/// <typeparam name="T">���l�^</typeparam>
/// <param name="ch">����</param>
/// <param name="value">����</param>
/// <returns>�����~�w����</returns>
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
				throw std::runtime_error("E�̌�Ɏw��������܂���ł����B");
			else
				return value * powt<T>(10, -static_cast<T>(scan_digit_sequence<unsigned>(ch)));
		}
		else {
			if (ch == '+')
				ch = getchar();
			if (ch == EOF || !isdigit((unsigned char)ch))
				throw std::runtime_error("E�̌�Ɏw��������܂���ł����B");
			else
				return value * powt<T>(10, static_cast<T>(scan_digit_sequence<unsigned>(ch)));
		}
	default:
		return value;
	}
}

/// <summary>
/// �������𑖍�����
/// </summary>
/// <typeparam name="T">���l�^</typeparam>
/// <param name="ch">����</param>
/// <param name="value">����</param>
/// <returns>������������� true�A������� false</returns>
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
			throw std::runtime_error("�����_�̌�ɐ���������܂���ł����B");
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
/// �������܂܂Ȃ������𑖍�����
/// </summary>
/// <typeparam name="T">�����^</typeparam>
/// <param name="ch">����</param>
/// <param name="value">�����l</param>
/// <returns>�����l������� true�A������� false</returns>
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
/// �������܂܂Ȃ������𑖍�����
/// </summary>
/// <typeparam name="T">�����^</typeparam>
/// <param name="ch">����</param>
/// <param name="value">�����l</param>
/// <returns>�����l������� true�A������� false</returns>
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
/// ���l�𑖍�����i�������������������j
/// </summary>
/// <typeparam name="T">���l�^</typeparam>
/// <param name="ch">����</param>
/// <param name="value">���l</param>
/// <returns>���l������� true�A������� false</returns>
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
			throw std::runtime_error("�����̌�ɐ��l���L�q����Ă��܂���ł����B");
	case '-':
		if (scan_without_sign<T>(ch = getchar(), value)) {
			value *= static_cast<T>(-1);
			return true;
		}
		else
			throw std::runtime_error("�����̌�ɐ��l���L�q����Ă��܂���ł����B");
	default :
		if (scan_without_sign<T>(ch, value)) {
			return true;
		}
		else
			return false;
	}
}

/// <summary>
/// �������������𑖍�����
/// </summary>
/// <typeparam name="T">�������������^</typeparam>
/// <param name="ch">����</param>
/// <param name="value">�������������l</param>
/// <returns>�������������l������� true�A������� false</returns>
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
/// ������𑖍�����
/// </summary>
/// <param name="ch">����</param>
/// <param name="s">������̎���̈�</param>
/// <returns>�����񂪂���� true�A������� false</returns>
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
/// �����ɏ]���đ�������
/// </summary>
/// <param name="format">����</param>
/// <param name="ch">����</param>
/// <param name="args">�������X�g</param>
/// <returns>�����ɑΉ������f�[�^����荞�߂��Ƃ��� true�A��荞�߂Ȃ������Ƃ��� false</returns>
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
				throw std::runtime_error("%ll�ɑ��������w�蕶��������������܂���B");
			}
		default:
			throw std::runtime_error("%l�ɑ��������w�蕶��������������܂���B");
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
				throw std::runtime_error("%hh�ɑ��������w�蕶��������������܂���B");
			}
		default:
			throw std::runtime_error("%h�ɑ��������w�蕶��������������܂���B");
		}
	case 's': //%s : ������
		++format; return scan_character_sequence(ch, va_arg(args, char*));
	default:
		throw std::runtime_error("%�ɑ��������w�蕶��������������܂���B");
	}
}

/// <summary>
/// ���^��scanf�i���̎w��͕s�A%S, %ls �̎w��s�j
/// </summary>
/// <param name="format">����</param>
/// <param name="...">�f�[�^���󂯎�邽�߂̃|�C���^�Q</param>
/// <returns>��荞�߂��f�[�^�̌��i�����荞�߂��ɓ��͑��u�ł̃G���[�����������Ƃ��AEOF�ɒB�������AEOF��Ԃ��j</returns>
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
