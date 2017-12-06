# ANSI format stream manipulators

Header only file for ANSI format stream manipulators.

## Usage

There are some default provided manipulators: ``format::bold(bool)``, ``format::italic(bool)`` and ``format::underline(bool)`` will apply their corresponding format to the stream.

Additional SGR formats can be specified by using ``Format<int, int>(bool)`` where the first int is the [ANSI SGR code](https://en.wikipedia.org/wiki/ANSI_escape_code) to turn the feature on, the second the one to turn it off. 

``format::fg(uint8_t)`` and ``format::bg(uint8_t)`` will apply text and background [xterm 8-bit color](https://upload.wikimedia.org/wikipedia/commons/1/15/Xterm_256color_chart.svg) respectively.  ``format::fg(uint8_t r, uint8_t g, uint8_t b)`` and its ``format::bg`` equivalent will apply true color. ``format::fg_default`` and ``format::bg_default`` is used for terminal default color.

### Example

```c++
#include "inc/ansi_format.hpp"
#include <iostream>

typedef format::Format<5, 25> blink;

int main() {
	
    std::cout << format::bold(true) << "this is bold. ";
    std::cout << blink(true) << "This is also bold and might be blinking if your terminal supports it. ";
    std::cout << format::bold(false) << blink(false) << format::italic(true) << "This is italic. ";
    std::cout << format::clear << "Everything is back to normal." << std::endl;
    
    std::cout << format::fg(1) << "Colored text, ";
    std::cout << format::bg(2) << "with colored background. ";
    std::cout << format::clear << "Normal text again." << std::endl;
    
}
```
