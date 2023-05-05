#include "webinix.hpp"
#include <iostream>

int main() {
    webinix::window my_window;
    my_window.show("<html>Hello World!</html>");
    webinix::wait();
    return 0;
}

#ifdef _WIN32
    int WINAPI wWinMain(
        _In_ HINSTANCE hInstance,
        _In_opt_ HINSTANCE hPrevInstance,
        _In_ LPWSTR lpCmdLine,
        _In_ int nShowCmd
    ) {
        return main();
    }
#endif
