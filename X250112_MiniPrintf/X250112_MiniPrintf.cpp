#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
void mini_printf(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	for (const char* p = format;*p; ++p) {
		if (*p == '%') {
			char buf[128];
			switch (*++p) {
			case 'd': //%d : int 型の値を１０進数で出力する
				_itoa_s(va_arg(args, int), buf, sizeof buf, 10);
				fputs(buf, stdout);
				continue;
			case 'f': //%f : double 型の値を出力する
				_gcvt_s(buf, sizeof buf, va_arg(args, double), 16);
				fputs(buf, stdout);
				continue;
			case 's': //%s : 文字列を出力する
				fputs(va_arg(args, const char*), stdout);
				continue;
			default:
				fprintf(stderr, "書式指定エラー : '%s'\n", p);
				exit(EXIT_FAILURE);
			}
		}
		else
			putchar(*p);
	}
	va_end(args);
}
#include <math.h>
int main()
{
	const float  pi_float  = 4.f * atanf(1.f);
	const double pi_double = 4.  * atan (1. );
	mini_printf("整数  : %d\n",  123 );
	mini_printf("文字列: %s\n", "ABC");
	mini_printf("実数  : 単精度 %f\n"
	            "        倍精度 %f\n"
	, pi_float
	, pi_double
	);
	return 0;
}
