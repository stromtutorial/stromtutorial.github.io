---
layout: home
title: Strom 3
subtitle: A tutorial for building Bayesian phylogenetic software using C++
---
<!-- <div class="row" style="display: flex; align-items: center;">
<div class="col-sm-10 col-xs-8">
  <h1 style="font-size: 2.5rem"><b>Strom 3</b></h1>
  <h3>A tutorial for building Bayesian phylogenetic software using C++</h3>
</div>
</div> -->

This tutorial teaches you how to create C++ software that performs Bayesian phylogenetic analyses. It was written primarily to help students in my laboratory to develop software that they can later modify for their own purposes, but I hope it is more broadly useful.

This is the third version of the tutorial. It differs from the [second version](https://phylogeny.uconn.edu/tutorial-v2/) in the order in which features are added, in adding invariable sites and codon models, and in allowing data partitioning. 

Note: (Many more) bugs have been fixed! If you are in the middle of the tutorial now, I would advise that you replace your code for the step you are on with the source code files for that step from the repository. The easy way to do this is to visit the [GitHub repository](https://github.com/stromtutorial/stromtutorial.github.io/tree/source, ensure you are on the `source` branch (not the `master` branch), navigate to your current step in the `steps` directory, and find the source code files for that step in the `src` directory under that step. See the Version History section of the source branch [README](https://github.com/stromtutorial/stromtutorial.github.io/tree/source) (specifically version 3.2) to see my notes on this update.
{:.poltodo}

#### Begin Tutorial

Three versions of the tutorial are provided. The Mac and Windows versions use graphical IDEs (Integrated Development Environments). The Mac version uses the Xcode IDE, while the Windows version uses the Visual Studio IDE. The Linux version shows you how to build software on an Ubuntu system without using a graphical IDE using the meson/ninja approach. Get started using your platform of choice below:

* Mac version: [Step-by-step instructions]({{ site.baseurl }}/mac/steps/)
* Windows version: [Step-by-step instructions]({{ site.baseurl }}/win/steps/)
* Linux version: [Step-by-step instructions]({{ site.baseurl }}/linux/steps/)

#### Author

The tutorial was written by [Paul O. Lewis](http://phylogeny.uconn.edu). Please write to me at [paul.lewis@uconn.edu](mailto:paul.lewis@uconn.edu) if you find errors.

#### Funding
This tutorial was developed as a broader impact project associated with National Science Foundation grant DEB-1354146 (Estimating the Bayesian phylogenetic information content of systematic data, PI Paul O. Lewis).

#### Acknowledgements

If there is anything in this tutorial that is well-designed, it is due to the patience and generosity of the many great teachers, mentors, and colleagues who have helped me over the years. The person who taught me the most about phylogenetics and, particularly, how to _program_ phylogenetics is David Swofford, who sets a very high standard for code quality that improves the ability of all of us who try to attain it.

#### Licence
The software that you will create falls under the permissive open-source [MIT License](license.html).

