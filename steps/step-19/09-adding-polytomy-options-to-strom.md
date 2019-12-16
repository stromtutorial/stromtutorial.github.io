---
layout: steps
title: Modifying Strom
partnum: 19
subpart: 9
description: Add polytomy-related command-line options to Strom.
---
{% comment %}
{{ page.description | markdownify }}
{% endcomment %}

## Modifying Strom

Add data members `_allow_polytomies`, `_topo_prior_C`, `_resolution_class_prior` to the `Strom` class as shown below to allow the user to (1) choose whether to allow polytomies, (2) set the C parameter of the topology prior, and (3) choose whether the topology prior represents a resolution class or standard polytomy prior.
~~~~~~
{{ "steps/step-19/src/strom.hpp" | polcodesnippet:"start-end_class_declaration","a-b" }}
~~~~~~
{:.cpp}

Initialize the 3 new data members in the `Strom::clear` function
~~~~~~
{{ "steps/step-19/src/strom.hpp" | polcodesnippet:"begin_clear-end_clear","c-d" }}
~~~~~~
{:.cpp}

Provide settings for the new data members to allow the user to modify them in `processCommandLineOptions`.
~~~~~~
{{ "steps/step-19/src/strom.hpp" | polcodesnippet:"begin_processCommandLineOptions-end_add_options","e-f" }}
~~~~~~
{:.cpp}

Modify the `sample` function to output the resolution class of sampled trees in the parameters file.
~~~~~~
{{ "steps/step-19/src/strom.hpp" | polcodesnippet:"begin_sample-end_sample","g,h" }}
~~~~~~
{:.cpp}

Modify the `initChains` function to notify chains of the new settings via their `Model`.
~~~~~~
{{ "steps/step-19/src/strom.hpp" | polcodesnippet:"begin_initChains-end_initChains","i" }}
~~~~~~
{:.cpp}


