---
layout: steps
title: Saving trees
partnum: 5
subpart: 0
description: In this step you will learn how to save a tree in memory in the form of a string known as a Newick tree description. You will also begin using some functionality from the Boost C++ library.
---
![Newick's restaurant near Dover, New Hampshire]({{ "/assets/img/newicks.jpg" | absolute_url }}){: .right-image}
[Newick's Lobster House](http://www.newicks.com) {% indexhide Newick's Restaurant %} was the site of an historic 1986 meeting 
at which a standard was devised for storing descriptions of phylogenetic trees as strings. These newick tree descriptions (also called the {% indexshow New Hampshire format %}) became part of the {% indexshow NEXUS file format %} (described in [Maddison, Swofford, and Maddison, 1997](http://dx.doi.org/10.1093/sysbio/46.4.590)), which is a standard file format for storing systematic data that is still widely used despite the advent of [NexML](http://www.nexml.org).

{{ page.description | markdownify }}

{% include subparts.html %}

{% comment %} 
(see [Joe Felsenstein’s account](http://evolution.genetics.washington.edu/phylip/newicktree.html)) 
{% endcomment %} 
