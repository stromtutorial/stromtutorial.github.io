---
layout: steps
title: The ExchangeabilityUpdater Class
partnum: 16
subpart: 3
description: Create the derived class ExchangeabilityUpdater from the base class DirichletUpdater. 
---
{% comment %}
{{ page.description | markdownify }}
{% endcomment %}

Creating an `ExchangeabilityUpdater` is as easy as creating the `StateFreqUpdater` (in fact, the two classes are almost identical). 

Create a new header file named {% indexfile exchangeability_updater.hpp %} and fill it with the following.
~~~~~~
{{ "steps/step-16/src/exchangeability_updater.hpp" | polcodesnippet:"start-end_class_declaration,end","" }}
~~~~~~
{:.cpp}

## Constructor and destructor

The constructor first calls the `clear` function defined by the `DirichletUpdater` base class. It then sets its `_name` to “Exchangeabilities” and its `_qmatrix` data member to the shared pointer `qmatrix` supplied as an argument. This points to the `QMatrix` object that manages the exchangeabilities that this class will update. The destructor is merely a placeholder, as usual.
~~~~~~
{{ "steps/step-16/src/exchangeability_updater.hpp" | polcodesnippet:"begin_constructor-end_destructor","" }}
~~~~~~
{:.cpp}

## The pullFromModel and pushToModel member functions

The `pullFromModel` function first obtains a shared pointer to the exchangeabilities vector of the `QMatrix` object pointed to by `_qmatrix`. It then copies those exchangeabilities into the `_curr_point` vector. The `pushToModel` function copies the values from `_curr_point` to the model via the `setExchangeabilities` member function of `QMatrix`, which handles recalculation of the instantaneous rate matrix and the corresponding eigen decomposition using the new exchangeabilities.
~~~~~~
{{ "steps/step-16/src/exchangeability_updater.hpp" | polcodesnippet:"begin_pullFromModel-end_pushToModel","" }}
~~~~~~
{:.cpp}
