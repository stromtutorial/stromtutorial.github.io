---
layout: steps
title: Modifying Model and OutputManager
partnum: 19
subpart: 8
description: Update the Model and OutputManager classes to accommodate polytomy analyses.
---
{% comment %}
{{ page.description | markdownify }}
{% endcomment %}

## Update the Model class

Add the 4 new function prototypes and 3 new data members highlighted below to the Model class declaration.
~~~~~~
{{ "steps/step-19/src/model.hpp" | polcodesnippet:"start-end_class_declaration","a-b,c-d" }}
~~~~~~
{:.cpp}

Initialize the 2 new data members (`_resolution_class_prior` and `_topo_prior_C`) in the `clear` function.
~~~~~~
{{ "steps/step-19/src/model.hpp" | polcodesnippet:"begin_clear-end_clear","e-f" }}
~~~~~~
{:.cpp}

Add the body of the `setTopologyPriorOptions` function to {% indexfile model.hpp %} somehwere before the bracket that closes the `strom` namespace. This function provides a way to inform the `Model` object of all 3 parameters of the topology prior.
~~~~~~
{{ "steps/step-19/src/model.hpp" | polcodesnippet:"begin_setTopologyPriorOptions-end_setTopologyPriorOptions","" }}
~~~~~~
{:.cpp}

Finally, add bodies of the 3 accessor functions `isAllowPolytomies`, `isResolutionClassTopologyPrior`, and `getTopologyPriorC`.
~~~~~~
{{ "steps/step-19/src/model.hpp" | polcodesnippet:"begin_isAllowPolytomies-end_getTopologyPriorC","" }}
~~~~~~
{:.cpp}

## Update the OutputManager class

The only tweak we need to make to OutputManager is to add the resolution class of the current tree to the quantities output to the parameter file.

In the class declaration in the file {% indexfile output_manager.hpp %}, add `unsigned m` to the parameter list of the `outputParameters` function.
~~~~~~
{{ "steps/step-19/src/output_manager.hpp" | polcodesnippet:"start-end_class_declaration","a" }}
~~~~~~
{:.cpp}

In the function `openParameterFile`, replace the line highlighted below. The difference is that a column heading `"m"` has been added for the column showing the resolution class.
~~~~~~
{{ "steps/step-19/src/output_manager.hpp" | polcodesnippet:"begin_openParameterFile-end_openParameterFile","b" }}
~~~~~~
{:.cpp}

Finally, in the `outputParameters` function, replace the lines highlighted below. The difference is that a parameter `m` has been added to the function and the value of `m` is now output in the new resolution class column.
~~~~~~
{{ "steps/step-19/src/output_manager.hpp" | polcodesnippet:"!begin_outputParameters-end_outputParameters","begin_outputParameters,c" }}
~~~~~~
{:.cpp}

