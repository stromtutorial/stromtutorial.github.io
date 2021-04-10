---
layout: steps
title: Conditioning on variability
partnum: 21
subpart: 0
description: Condition on variability for discrete morphological and SNP data
---
{% comment %}
{{ page.description | markdownify }}
{% endcomment %}

When data sets include only variable characters, it is important to condition the probability of the data given the model (the likelihood) on the fact that only variable characters are present. Felsenstein (1992) conditioned on variability when analyzing restrition site data because restriction site presence/absence data sets contain variable characters only. Lewis (2001) pointed out that the same issue arises if discrete morphological data sets were used for estimating phylogeny using likelihood-based models because morphological characters are only recorded if they are variable across the taxa of interest. Lewis (2001) also demonstrated by simulation that edge length estimates tend to be extreme (either zero or very large) if the model is not informed that only variable characters are included.

{% include subparts.html %}

## Literature Cited

Felsenstein, J. 1992. Phylogenies from restriction sites: a maximum-likelihood approach. Evolution 46:159-173.
[DOI:10.1111/j.1558-5646.1992.tb01991.x](https://doi.org/10.1111/j.1558-5646.1992.tb01991.x)

Lewis, P. O. 2001. A likelihood approach to estimating phylogeny from discrete morphological character data. Systematic Biology 50:913-925.
[DOI:10.1080/106351501753462876](https://doi.org/10.1080/106351501753462876) 
