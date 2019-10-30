---
layout: steps
title: The XStrom exception class
partnum: 6
subpart: 2
description: In this section you will create a class to handle exceptions
---
Unexpected things happen while a program is running. Users may not use the program in the way intended by the developer, and assumptions made by the developer that are violated by the user should result in an informative message rather than a non-informative crash. The way this is handled in C++ is through exceptions. An exception object is "thrown" when the program realizes that an assumption has been violated, and the object is "caught" at another place in the program where it can be reported to the user. Look for the `try` block inside the `buildFromNewick` function. If anything bad happens inside the `try` block, an exception is thrown and the exception object is caught in the `catch` block near the bottom of the `buildFromNewick` function. Note what happens when the exception is caught: the `clear` function is called to ensure that the `Tree` object is not left in a partially-built state, then the exception is thrown again. We will experiment with catching the second throw in the main function and reporting the problem there, but for now we need to create the `XStrom` exception class.

Create a new class named `XStrom` in a file named {% indexfile xstrom.hpp %} and fill the file with the following code:
~~~~~~
{{ "steps/step-06/src/xstrom.hpp" | polcodesnippet:"start-end","" }}
~~~~~~
{:.cpp}


You should have already done this, but check now that you have added the following line to the top of {% indexfile tree_manip.hpp %} so that the compiler will know about our new `XStrom` class:
~~~~~~
#include "xstrom.hpp"
~~~~~~
{:.cpp}
