# ANSI format stream manipulators

Header only file for ANSI format stream manipulators.

## Usage

There are some default provided manipulators: ``format::bold(bool)``, ``format::italic(bool)`` and ``format::underline(bool)`` will apply their corresponding format to the stream.

Additional SGR formats can be specified by using ``Format<int, int>(bool)`` where the first int is the [ANSI SGR code](https://en.wikipedia.org/wiki/ANSI_escape_code) to turn the feature on, the second the one to turn it off. 

``format::fg(int)`` and ``format::bg(int)`` will apply text and background color respectively. -1 is used for terminal default color.

Right now, functors will only work on one stream at a time (preferably ``std::cout``).

### Example

```c++
#include "inc/ansi_format.hpp"
#include <iostream>

typedef format::Format<5, 25> blink;

int main() {
	
    std::cout << format::bold(true) << "this is bold. ";
    std::cout << blink(true) << "This is also bold and might be  blinking if your terminal supports it. ";
    std::cout << format::bold(false) << blink(false) << format::italic(true) << "This is italic. ";
    std::cout << format::clear << "Everything is back to normal." << std::endl;
    
    std::cout << format::fg(1) << "Colored text, ";
    std::cout << format::bg(2) << "with colored background. ";
    std::cout << format::clear << "Normal text again." << std::endl;
    
}
```
