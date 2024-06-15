#include "webinix.h"

const char *doc =
    "<html style=\"background: #654da9; color: #eee\">"
    "<head>"
    "  <script src=\"webinix.js\"></script>"
    "</head>"
    "<body>"
    "  <script>"
    "    setTimeout(async () => {"
    "      await webinix.call(\"close\");"
    "    }, 500)"
    "  </script>"
    "</body>"
    "</html>";

void close_window(webinix_event_t *e) {
	webinix_close(e->window);
}

int main() {
	size_t w = webinix_new_window();

	webinix_bind(w, "close", close_window);
	webinix_show(w, doc);
	webinix_wait();

	return 0;
}

