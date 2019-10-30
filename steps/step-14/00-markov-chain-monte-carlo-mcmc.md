---
layout: steps
title: Markov Chain Monte Carlo (MCMC)
partnum: 14
subpart: 0
description: Create a class whose objects carry out Markov chain Monte Carlo (MCMC) simulation. 
---
{% comment %}
{{ page.description | markdownify }}
{% endcomment %}

The next step is to create a `Chain` class that carries out a Markov chain Monte Carlo (MCMC) simulation for the purpose of sampling from a Bayesian posterior distribution. Our initial effort will update only the gamma shape parameter used to model among-site rate heterogeneity. Later we will add the ability to update other substitution model parameters (e.g. nucleotide state frequencies and GTR exchangeabilities) as well as the tree topology and edge lengths.

{% include subparts.html %}
