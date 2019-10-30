---
layout: steps
title: The OutputManager Class
partnum: 15
subpart: 1
description: Create the OutputManager class
---
{% comment %}
{{ page.description | markdownify }}
{% endcomment %}

The `OutputManager` will handle opening and closing tree and parameter files, saving sampled trees to the tree file, saving sampled parameter values to the parameter file, and displaying output to the console.

Some changes/additions will need to be made to the `Data` and `Model` classes, but that will be postponed until after we’ve created the `OutputManager` class.

Create a new file {% indexfile output_manager.hpp %} and replace the default contents with the following class declaration.
~~~~~~
{{ "steps/step-15/src/output_manager.hpp" | polcodesnippet:"start-end_class_declaration,end","" }}
~~~~~~
{:.cpp}

## Constructor and destructor

The constructor stores default names for the tree and parameter files, and the destructor is empty. (The output lines are provided but can remain commented out unless you need them at a later time.)
~~~~~~
{{ "steps/step-15/src/output_manager.hpp" | polcodesnippet:"begin_constructor-end_destructor","" }}
~~~~~~
{:.cpp}

## The openTreeFile and closeTreeFile member functions

These functions are responsible for opening and closing the file used to store trees sampled during the MCMC simulation. Note that some of the work in the `openTreeFile` function is done by the `Data` class: because the `Data` object knows the taxon names, it can easily write out the taxa block for us, as well as the translate command in the trees block.
~~~~~~
{{ "steps/step-15/src/output_manager.hpp" | polcodesnippet:"begin_openTreeFile-end_closeTreeFile","" }}
~~~~~~
{:.cpp}

## The openParameterFile and closeParameterFile member functions

These functions are responsible for opening and closing the file used to store model parameters sampled during the MCMC simulation. The `openParameterFile` function asks the supplied `Model` object to provide a list of parameter names that it then uses for column headers when creating a new parameter file.
~~~~~~
{{ "steps/step-15/src/output_manager.hpp" | polcodesnippet:"begin_openParameterFile-end_closeParameterFile","" }}
~~~~~~
{:.cpp}

## The outputConsole, outputTree, and outputParameters member functions

These three functions are called to output messages to the user on the console, output sampled trees to the tree file, and output sampled parameters to the parameter file, respectively. The `outputConsole` function currently just outputs the supplied message to the terminal (console), but could easily be modified to output the same message to both the console and to a file (we are not going to write that code in this tutorial, however). The `outputTree` function asks the supplied `TreeManip` object to provide the tree description that it then stores in the tree file. The `outputParameters` function asks the supplied `Model` object to provide the current values of all model parameters that it then stores in the parameters file.
~~~~~~
{{ "steps/step-15/src/output_manager.hpp" | polcodesnippet:"begin_outputConsole-end_outputParameters","" }}
~~~~~~
{:.cpp}
