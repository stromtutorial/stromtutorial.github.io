--- 
layout: steps 
title: Modify Strom 
partnum: 15 
subpart: 3
description: Integrate the OutputManager class into the Strom class. 
---
{% comment %} 
{{ page.description | markdownify }} 
{% endcomment %}

## Changes to Strom

All that is needed now is to create an `OutputManager` object and call its functions at the appropriate places. The parts that need to be added (or modified) in the `Strom` class declaration in the file {% indexfile strom.hpp %} are marked with bold, blue text below.
~~~~~~
{{ "steps/step-15/src/strom.hpp" | polcodesnippet:"start-end_class_declaration","a,b,c" }}
~~~~~~
{:.cpp}

Add the indicated line to the `clear` function body to initialize the `_output_manager` data member.
~~~~~~
{{ "steps/step-15/src/strom.hpp" | polcodesnippet:"begin_clear-end_clear","d" }}
~~~~~~
{:.cpp}

You’ll note that I’ve added a private member function named `sample`. This function is not essential, but serves to make the `run` function more tidy by gathering all code lines related to saving a tree and parameter sample together so that they can be called by adding a single line to the run function whereever sampling needs to occur. Here is the body of the `sample` function.
~~~~~~
{{ "steps/step-15/src/strom.hpp" | polcodesnippet:"begin_sample-end_sample","" }}
~~~~~~
{:.cpp}

Changes to the `run` function involve creating an `OutputManager` object and opening/closing tree and sample files. You will also note that some of the blue, bold text reflects sections of code that have been commented out because their functionality has been replaced by the new `OutputManager` object. The two calls to the `sample` function cause output to be sent to the console and/or sample and tree files at the very start (iteration 0) and at each `samplefreq` (or `printfreq`) iteration thereafter. 
~~~~~~
{{ "steps/step-15/src/strom.hpp" | polcodesnippet:"begin_run-end_run","f-ff,g-gg,h-hh,i,j,k-kk,l-ll,m-mm,n" }}
~~~~~~
{:.cpp}
