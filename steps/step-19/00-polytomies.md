---
layout: steps
title: Allowing Polytomies
partnum: 19
subpart: 0
description: Implement reversible-jump MCMC to allow polytomies in trees. 
---
{% comment %}
{{ page.description | markdownify }}
{% endcomment %}

Forcing a Bayesian MCMC analysis to consider only binary (i.e. fully-resolved) tree topologies can result in the artefact known as the Star Tree Paradox (see Lewis, Holder, and Holsinger, 2005, and papers citing it) in which one of the possible resolutions of a polytomous node will end up with the highest posterior support, and the posterior probability associated with that arbitrary resolution may be quite high (or quite low, but we tend to notice only the high ones). It is as if the topological uncertainty resulting from the polytomy is manifested in a random degree of support for a few arbitrary resolutions rather than an even spread of posterior support over all possible resolutions.

The solution implemented here is the one proposed by Lewis, Holder, and Holsinger (2005). This uses Green's (1995) reversible-jump MCMC approach to move between trees in different resolution classes, where resolution class 1 is the star tree (only 1 internal node) and the highest resolution class has n-2 internal nodes (if unrooted) or n-1 internal nodes (if rooted). 

The benefit to implementing a polytomy-friendly rjMCMC sampler is that a tree with a polytomy is inferred to be a tree with a polytomy, not a fully-resolved tree with incorrect posterior support. You may feel that polytomies do not really exist in nature. Tree graphs are models that capture important features of the underlying history of the data being analyzed. If two sequential speciation events occurred so close together in time that no nucleotide substitutions were fixed in any gene analyzed, then it could be argued that a polytomy is a better representation of reality than an arbitrary resolution with a tiny internal edge length. At the very least, it offers a very straighforward example of how to implement reversible-jump MCMC, which is a very important tool in the arsenal of any Bayesian phylogenetic modeler.

{% include subparts.html %}

## Literature Cited

Green, Peter J. 1995. Reversible jump Markov chain Monte Carlo computation and Bayesian model determination. Biometrika 82(4):711-732. [JSTOR](https://www.jstor.org/stable/2337340)

Lewis, P. O., Holder, M. T., and Holsinger, K. E. 2005. Polytomies and bayesian phylogenetic inference. Systematic Biology 54(2):241–253. [https://doi.org/10.1080/10635150590924208](https://doi.org/10.1080/10635150590924208)