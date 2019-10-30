---
layout: steps
title: Adding Helper Functions to Data and Model
partnum: 15
subpart: 2
description: Integrate the OutputManager class into the Data and Model classes
---
{% comment %}
{{ page.description | markdownify }}
{% endcomment %}

The `OutputManager` class depends on (1) the `Data::createTaxaBlock` and `Data::createTranslateStatement` functions to create a taxa block and a translate statement, respectively, in its `openTreeFile` function; the `Model::paramNamesAsString` and `Model::paramValuesAsString` functions to obtain current model parameter names in its `openParamFile` function and values in its `outputParameters` function; and the `TreeManip::makeNewick` function to provide the newick tree description that it saves to the tree file in its `outputTree` function.

The last of these (`TreeManip::makeNewick`) already exists, and the following two sections add the other helper functions to the `Data` and `Model` classes.

## Adding createTaxaBlock and createTranslateStatement to the Data class

Add the bold, blue lines to the `Data` class declaration in the file `data.hpp`.
~~~~~~
{{ "steps/step-15/src/data.hpp" | polcodesnippet:"begin_data-end_data","a,b" }}
~~~~~~
{:.cpp}

Now add the 2 function bodies below the class declaration but before the namespace-closing right curly bracket.
~~~~~~
{{ "steps/step-15/src/data.hpp" | polcodesnippet:"begin_createTaxaBlock-end_createTranslateStatement","" }}
~~~~~~
{:.cpp}

## Adding paramNamesAsString and paramValuesAsString to the model class

Add the bold, blue lines to the `Model` class declaration in the file {% indexfile model.hpp %}.
~~~~~~
{{ "steps/step-15/src/model.hpp" | polcodesnippet:"begin_model-end_model","a,b" }}
~~~~~~
{:.cpp}

Add the function body for `paramNamesAsString` below the class declaration but before the namespace-closing right curly bracket. This creates a row of parameter names (headers) that serve as a key to the values in the columns below. The index of the subset to which each parameter belongs is incorporated into the name, with subset 0 being the first subset (even if there is only one subset).
~~~~~~
{{ "steps/step-15/src/model.hpp" | polcodesnippet:"begin_paramNamesAsString-end_paramNamesAsString","" }}
~~~~~~
{:.cpp}

Add the function body for `paramValuesAsString` below the class declaration but before the namespace-closing right curly bracket. This function is nearly identical to `paramNamesAsString` except that it outputs current parameter values rather than parameter names.
~~~~~~
{{ "steps/step-15/src/model.hpp" | polcodesnippet:"begin_paramValuesAsString-end_paramValuesAsString","" }}
~~~~~~
{:.cpp}

