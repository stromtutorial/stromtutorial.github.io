---
layout: steps
title: Building trees and handling exceptions
partnum: 6
subpart: 0
description: In this step you will learn how to build a tree in memory from a Newick tree description and will create an exception class (`XStrom`) to handle unexpected run-time circumstances.
---
{{ page.description | markdownify }}

The previous section added the function `makeNewick` that saves a tree in memory to a string in newick format. This part will focus on the opposite: building a tree in memory given a newick string. After finishing this part, you will be in position to read trees from a tree file and build each one in computer memory.

Sometimes programs encounter situations that were unexpected. For example, the user supplies a data file name, but that file does not contain data, or contains data in a format that the program is not prepared to handle. In these circumstances, our program should exit with an informative message for the user so that they know what to correct before running it again. This is accomplished by throwing an {% indexbold exception %}, which is an object (an instance of the `XStrom` class){% indexhide XStrom class %}. The `main` function will catch any exception thrown at any point and display the message carried by the `XStrom` object to the user before exiting.

{% include subparts.html %}
