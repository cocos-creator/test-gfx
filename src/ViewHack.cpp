#include <Windows.h>
#include <memory>

#include "platform/win32/View-win32.h"

namespace cc {
View::View(const std::string &title, int width, int height) {}
View::~View() = default;
HWND View::getWindowHandler() { return nullptr; }
} // namespace cc

std::shared_ptr<cc::View> cc_get_application_view() {
    static std::shared_ptr<cc::View> view = std::make_shared<cc::View>("", 1, 1);
    return view;
}
