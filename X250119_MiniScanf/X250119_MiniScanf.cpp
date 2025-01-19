// このソースファイルには、mini_scanf 関数のテストを自動的に実施するための main 関数が含まれています。
#include "mini_scanf.h"
#include <stdio.h>
#include <iostream>
#include <stdexcept>
#include <Windows.h>

#define	FileName_TestData_floating_point_txt     "TestData_floating_point.txt"
#define	FileName_TestData_integral_txt           "TestData_integral.txt"
#define	FileName_TestData_yyyy_mm_dd_txt         "TestData_yyyy_mm_dd.txt"

template<class T>void test_mini_scanf(const char* format_for_scanf, const char* format_for_printf)
{
	T value;
	if (mini_scanf(format_for_scanf, &value) == 1) {
		printf(format_for_printf, value);
		putchar('\n');
	}
	else
		throw std::runtime_error(__FILE__ "(" _CRT_STRINGIZE(__LINE__) "): mini_scanf failed.\n");
}
void test1()
{
#pragma warning(suppress : 4996)
	if (freopen(FileName_TestData_floating_point_txt, "r", stdin)) {
		for (int i = 0; i < 9; ++i) {
			for (int j = 0; j < 4; ++j) {
				double value;
				if (mini_scanf("%lf", &value) == 1)
					printf("%g\n", value);
				else
					throw std::runtime_error(__FILE__ "(" _CRT_STRINGIZE(__LINE__) "): mini_scanf failed.\n");
			}
			putchar('\n');
		}
	}
}
void test2()
{
#pragma warning(suppress : 4996)
	if (freopen(FileName_TestData_integral_txt, "r", stdin)) {
		test_mini_scanf<         long long>("%lld", "%lld"     );
		test_mini_scanf<         long long>("%lld", "%lld"     );
		test_mini_scanf<         long long>("%lld", "%lld"     );
		test_mini_scanf<unsigned long long>("%llu", "%llu" "\n");
		test_mini_scanf<         long     >("%ld" , "%ld"      );
		test_mini_scanf<         long     >("%ld" , "%ld"      );
		test_mini_scanf<         long     >("%ld" , "%ld"      );
		test_mini_scanf<unsigned long     >("%lu" , "%lu"  "\n");
		test_mini_scanf<         int      >("%d"  , "%d"       );
		test_mini_scanf<         int      >("%d"  , "%d"       );
		test_mini_scanf<         int      >("%d"  , "%d"       );
		test_mini_scanf<unsigned          >("%u"  , "%u"   "\n");
		test_mini_scanf<         short    >("%hd" , "%d"       );
		test_mini_scanf<         short    >("%hd" , "%d"       );
		test_mini_scanf<         short    >("%hd" , "%d"       );
		test_mini_scanf<unsigned short    >("%hu" , "%u"   "\n");
		test_mini_scanf<         char     >("%hhd", "%d"       );
		test_mini_scanf<         char     >("%hhd", "%d"       );
		test_mini_scanf<         char     >("%hhd", "%d"       );
		test_mini_scanf<unsigned char     >("%hhu", "%u"   "\n");
	}
}
void test3()
{
	char buf[256];
#pragma warning(suppress : 4996)
	if (freopen(FileName_TestData_floating_point_txt, "r", stdin)) {
		for (;;) {
			switch (mini_scanf("%s", buf)) {
			case 1:
				printf("%s\n", buf);
				continue;
			case EOF:
				return;
			default:
				throw std::runtime_error(__FILE__ "(" _CRT_STRINGIZE(__LINE__) "): mini_scanf failed.\n");
			}
		}
	}
	else
		throw std::runtime_error("freopen(\"" FileName_TestData_floating_point_txt "\") failed.\n");
}
void test4()
{
#pragma warning(suppress : 4996)
	if (freopen(FileName_TestData_yyyy_mm_dd_txt, "r", stdin)) {
		unsigned short yyyy;
		unsigned char  mm  ;
		unsigned char  dd  ;
		if (mini_scanf("%hu/%hhu/%hhu", &yyyy, &mm, &dd) == 3)
			printf("%04u/%02u/%02u\n", yyyy, mm, dd);
		else
			throw std::runtime_error(__FILE__ "(" _CRT_STRINGIZE(__LINE__) "): mini_scanf failed.\n");
	}
	else
		throw std::runtime_error("freopen(\"" FileName_TestData_yyyy_mm_dd_txt "\") failed.\n");

}
int main()
{
	try {
		test1();
		test2();
		test3();
		test4();
	}
	catch (const std::exception& e) {
		OutputDebugStringA(e.what());
		OutputDebugStringA("\n");
		std::cerr << e.what() << '\n';
	}
	return 0;
}
