// contributed by stloeffler,
// see https://github.com/jlaurens/synctex/issues/100

#include <assert.h>
#include <stdio.h>
#include <limits.h>

#include <synctex_parser_utils.h>

void test(char * src, int expected) {
	int result = synctex_parse_int(src, NULL);
	if (result == expected) {
		printf("  %s = %i\n", src, expected);
	}
	else {
		printf("X %s = %i != %i\n", src, result, expected);
	}
}

int main(int argc, char ** argv) {
	assert(INT_MAX >= 2147483647);

	synctex_parse_int_policy(synctex_parse_int_policy_raw2);
	test("0", 0);
	test("01", 1);
	test("123", 123);
	test("+123", 123);
	test("   123", 123);
	test("12AB", 12);
	test("999999999", 999999999);
	test("2147483639", 2147483639);
	test("2147483646", 2147483646);
	test("2147483647", 2147483647);
	test("2147483648", 2147483647);
	test("2147483649", 2147483647);
	test("2147483650", 2147483647);
	test("9999999999", 2147483647);
	test("99999999999", 2147483647);
	test("-0", 0);
	test("-01", -1);
	test("-123", -123);
	test("   -123", -123);
	test("-12AB", -12);
	test("-999999999", -999999999);
	test("-2147483639", -2147483639);
	test("-2147483647", -2147483647);
	test("-2147483648", -2147483648);
	test("-2147483649", -2147483648);
	test("-2147483650", -2147483648);
	test("-9999999999", -2147483648);
	test("-99999999999", -2147483648);

	return 0;
}
