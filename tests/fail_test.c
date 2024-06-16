#include "webinix.h"

const char *doc =
    "<html style=\"background: #654da9; color: #eee\">"
    "<head>"
    "  <script src=\"webinix.js\"></script>"
    "</head>"
    "<body>"
    "  <script>"
    "    setTimeout(async () => {"
    "      await webinix.call(\"fail\");"
    "    }, 500)"
    "  </script>"
    "</body>"
    "</html>";

void fail(webinix_event_t *e) {
	// This test should help to ensure that test failures can be detected from CI.
	exit(EXIT_FAILURE);
}

int main() {
	size_t w = webinix_new_window();

	webinix_bind(w, "fail", fail);
	webinix_show(w, doc);
	webinix_wait();

	return 0;
}
