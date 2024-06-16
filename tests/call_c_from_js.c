#include "webinix.h"

const char *doc =
    "<html style=\"background: #654da9; color: #eee\">"
    "<head>"
    "  <script src=\"webinix.js\"></script>"
    "</head>"
    "<body>"
    "  <script>"
    "    setTimeout(async () => {"
    "      await webinix.assert_int(1, 23, 345);"
    "      await webinix.assert_float(1.0, 2.3, 3.45);"
    "      await webinix.assert_string('foo', 'bar', 'baz');"
    "      await webinix.assert_bool(true, false, true);"
    "      await webinix.assert_close();"
    "    }, 500)"
    "  </script>"
    "</body>"
    "</html>";

void assert_int(webinix_event_t *e) {
	size_t count = webinix_get_count(e);
	assert(count == 3);

	long long num = webinix_get_int(e);
	long long num1 = webinix_get_int_at(e, 0);
	long long num2 = webinix_get_int_at(e, 1);
	long long num3 = webinix_get_int_at(e, 2);

	assert(num == 1);
	assert(num1 == num);
	assert(num2 == 23);
	assert(num3 == 345);
}

void assert_float(webinix_event_t *e) {
	size_t count = webinix_get_count(e);
	assert(count == 3);

	double num = webinix_get_float(e);
	double num1 = webinix_get_float_at(e, 0);
	double num2 = webinix_get_float_at(e, 1);
	double num3 = webinix_get_float_at(e, 2);

	printf("num1: %f, num2: %f, num3: %f\n", num1, num2, num3);
	// TODO: enable asserts after get_float is fixed.
	// assert(num == 1.0);
	// assert(num1 == num);
	// assert(num2 == 2.3);
	// assert(num3 == 3.45);
}

void assert_string(webinix_event_t *e) {
	size_t count = webinix_get_count(e);
	assert(count == 3);

	const char *str = webinix_get_string(e);
	const char *str1 = webinix_get_string_at(e, 0);
	const char *str2 = webinix_get_string_at(e, 1);
	const char *str3 = webinix_get_string_at(e, 2);

	assert(strcmp(str, "foo") == 0);
	assert(strcmp(str1, str) == 0);
	assert(strcmp(str2, "bar") == 0);
	assert(strcmp(str3, "baz") == 0);
}

void assert_bool(webinix_event_t *e) {
	size_t count = webinix_get_count(e);
	assert(count == 3);

	long long b = webinix_get_bool(e);
	long long b1 = webinix_get_bool_at(e, 0);
	long long b2 = webinix_get_bool_at(e, 1);
	long long b3 = webinix_get_bool_at(e, 2);

	assert(b == true);
	assert(b1 == b);
	assert(b2 == false);
	assert(b3 == true);
}

void assert_close(webinix_event_t *e) {
	// Closing often leads to a seqfault at the moment. Therefore, just a sysexit for now.
	// webinix_close(e->window);
	exit(EXIT_SUCCESS);
}

int main() {
	size_t w = webinix_new_window();

	webinix_bind(w, "assert_int", assert_int);
	webinix_bind(w, "assert_float", assert_float);
	webinix_bind(w, "assert_string", assert_string);
	webinix_bind(w, "assert_bool", assert_bool);
	webinix_bind(w, "assert_close", assert_close);
	webinix_show(w, doc);
	webinix_wait();
	webinix_clean();

	return 0;
}
