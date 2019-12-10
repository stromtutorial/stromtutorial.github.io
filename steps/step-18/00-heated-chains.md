---
layout: steps
title: Heated Chains
partnum: 18
subpart: 0
description: Implement Metropolis-coupled MCMC in order to improve mixing. 
---
{% comment %}
{{ page.description | markdownify }}
{% endcomment %}

In this part of the tutorial you will enable your program to run multiple Markov chains simultaneously. One of these (the cold chain) explores the posterior distribution while the other (heated) chains explore "melted-down" versions of the posterior in which the peaks are not as high and the valleys not as low being traversed by the cold chain. The melting is done by raising the the posterior kernel (the kernel is the unnormalized posterior density) by a power between 0 and 1. Values close to 1 melt the posterior only slightly, while a power equal to 0 would turn the posterior kernel into a constant, which would result in a perfectly flat landscape. Thus, 0 would not be a good power to use because a chain exploring a perfectly flat landscape would simply wander off and never come back. We would like the heated chains to hover around the same peaks as the cold chain, but because the heated landscapes are weathered and more gentle, these heated chains can easily wander across the valleys between peaks and thus do not get stuck on one peak like the cold chain.

After each chain (cold chain and each heated chain) takes an ordinary step, two chains are chosen at random to attempt a swap. For the swap to be successful, both chains must be able to move to the point occupied by the other chain. One of the chains will be moving uphill, a proposal that is automatically accepted, but the other chain must also be able to make the corresponding downhill move. Because both chains are normally close to a peak, it is often nearly as easy for the cold chain to jump directly to a different peak (occupied by its swap partner) as to take a step close to where it is currently located. As a result, the heated chains serve as scouts for the cold chain, proposing new places for the cold chain to go that may be quite distant from the places where its own proposals suggest. This serves to enhance mixing for posterior distributions that are multimodal or otherwise topographically complex.

We will also use this opportunity to distinguish the burn-in period from the sampling period. During the burn-in (the length of which is specified by the user), autotuning is done and no samples are saved to the parameter or tree files. After the burn-in period, autotuning is turned off and sampling is done at regular intervals. Autotuning is particularly important when heated chains are involved because each chain is exploring a different landscape and one set of tuning parameters will likely not work very well for both the cold chain and the most heated chain.

{% include subparts.html %}

