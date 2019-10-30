---
layout: steps
title: Adding a pseudorandom number generator
partnum: 13
subpart: 0
description: The next step is to create a class that can generate pseudorandom numbers. 
---
{{ page.description | markdownify }}

A "lot" is an object used to determine games of chance (as in "casting lots"). As we are creating an object used to determine chance events, `Lot` is an appropriate name for our new class. `Lot` will be a thin wrapper around the pseudorandom number generators provided by the Boost C++ library. Pseudorandom numbers are essential for computer simulation, including Markov chain Monte Carlo simulation, which is where we are ultimately headed with this tutorial.

{% include subparts.html %}
