---
layout: steps
title: Create the main function
partnum: 2
subpart: 3
description: Define the `main` function that serves as the entry point for the program in the source code file *main.cpp*
---
{% assign OS = site.operatingsystem %}
Every C++ program must define an entry point in the form of a `main` function {% indexhide main function %}.

{% if OS == "linux" %}
[//]: ################################### LINUX #########################################
Edit the file {% indexfile main.cpp %} in the {% indexfile src %} directory and replace the text that is there with the following:

{% elsif OS == "mac" %}
[//]: #################################### MAC ##########################################
Edit the existing file {% indexfile main.cpp %}, replacing its default contents with the following, then save the file.

{% elsif OS == "win" %}
[//]: ################################## WINDOWS ########################################
Delete the existing file {% indexfile strom.cpp %} (right-click its name in the Solution Explorer, then choose _Remove_). Create a new file {% indexfile main.cpp %} (right-click _Source Files_ folder, then choose _Add > New Item..._, click _C++ File (.cpp)_) and add the following code to it, saving the file when done.

{% endif %}
[//]: ################################# ENDIF ######################################
~~~~~~
{{ "steps/step-02/src/main.cpp" | polcodesnippet:"start-end","" }}
~~~~~~
{:.cpp}

## Explanation of *main.cpp*
Recall that the data member `_smallest_edge_length` in the class `Node` was declared to be static, so we must initialize it in a source code file, and `main.cpp` is our only source code file, hence the line that sets `_smallest_edge_length` {% indexhide smallest edge length %} to the very tiny value `1.0e-12`.

The `main` function firsts announces that the program is starting to run (by displaying `Starting...` to standard output), then creates a `Tree` object having nodes made of `Node` objects, then announces that the program is finished running just before returning 0 (the exit code 0 tells the operating system that the program ended normally).





