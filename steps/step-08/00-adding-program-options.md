---
layout: steps
title: Adding program options
partnum: 8
subpart: 0
description: In this part, you will use the Boost Program Options library to add the ability to specify options on the command line so that information such as tree and data file names need not be hard coded.
---
{{ page.description | markdownify }}

In the previous step, we hard-coded the name of the tree file into the main function. This forces our users to always name their data file {% indexfile test.tre %} or go to some trouble (editing {% indexfile main.cpp %} and recompiling the program) in order to change the file names. Software run from the command line normally requires some input from the user, and such user input is normally accommodated by command line options (or switches if they are boolean true/false options). We will use the [Boost program_options library](https://www.boost.org/doc/libs/1_71_0/doc/html/program_options.html) {% indexhide program_options %} to manage these options for our project. In this part you will also create a `Strom` class of which one instance will be created. This `Strom` object will have a `run` method that is called once program options are processed.

{% include subparts.html %}
