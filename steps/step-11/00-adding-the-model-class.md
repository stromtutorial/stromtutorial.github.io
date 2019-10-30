---
layout: steps
title: Adding the Model class
partnum: 11
subpart: 0
description: In this part we add a Model class to handle the tedium associated with informing BeagleLib of the details of the substitution model. The Eigen library is used to manage calculation of transition probability matrices.
---
{{ page.description | markdownify }}

The only substitution model we've implemented thus far has been the JC69 model. The only parameters of this model are edge lengths, and thus it does not model even the most basic aspects of molecular evolution, such as transition/transversion bias and unequal base frequencies. Furthermore, the JC69 model we used assumed that substitution rates were equal across sites and thus did not accommodate among-site rate heterogeneity. This section introduces the standard GTR+I+G model, which can handle among-site rate heterogeneity, unequal nucleotide frequencies, and unequal exchangeabilities (including transition/transversion bias) as well as a codon model that takes into account unequal codon frequencies and the nonsysnonymous/synonymous rate ratio. While our program can read in protein and standard discrete morphological data, this tutorial does not explicitly detail how to calculate likelihoods under those models; however, implementing GTR+I+G and a codon model illustrates enough of the complexity that you should be able to implement other models on your own. 

{% include subparts.html %}
