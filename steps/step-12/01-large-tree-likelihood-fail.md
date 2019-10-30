---
layout: steps
title: Large Tree Likelihood Fail
partnum: 12
subpart: 1
description: An illustration of underflow in calculation the likelihood of a large tree.
---
{% assign OS = site.operatingsystem %}

Before we add rescaling, let me prove to you that our code as it now stands will not work on a large tree. Download the files [rbcl738.nex]({{ "/assets/data/rbcl738.nex" | absolute_url }}) and [rbcl738nj.tre]({{ "/assets/data/rbcl738nj.tre" | absolute_url }}) and move them to the same directory where you have {% indexfile rbcL.nex %}.

Modify your {% indexfile strom.conf %} file to look like this:
~~~~~~
{{ "steps/step-12/test/strom.conf" | polsnippet:"1-6,8","" }}
~~~~~~
{:.bash-output}

{% if OS == "linux" %}
[//]: ################################### LINUX #########################################
Modify your meson.build script so that it copies the two new files to your install directory:
~~~~~~
{{ "steps/step-12/src/meson.build" | polcodesnippet:"","a,b" }}
~~~~~~
{:.meson}

Run the program using the above {% indexfile strom.conf %} file. Before doing this, you will need to run `meson install` in order to get the new `strom.conf`, data file, and tree file copied to your install directory.

{% elsif OS == "mac" %}
[//]: #################################### MAC ##########################################

{% elsif OS == "win" %}
[//]: ################################## WINDOWS ########################################

{% endif %}
[//]: ################################# ENDIF ######################################

You should find that the log-likelihood computed is shown as `-inf` (negative infinity), which means the likelihood (on the linear scale) is 0.0, which in turn means that underflow has occurred in at least some of the site likelihood calculations. The next page will show you how to set up BeagleLib to rescale the likelihood so that underflow does not happen.


