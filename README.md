## What is the Strom Tutorial?
The Strom Tutorial shows how to create C++ software for estimating phylogenetic trees using Bayesian MCMC. It is intended to provide a means for getting up to speed if you seriously want or need to learn how phylogenetics software works. A use case would be a graduate student interested in developing and testing a new phylogenetic method for their thesis research, or a postdoctoral researcher who obtained a Ph.D. in systematics but is beginning work in a laboratory that develops new phylogenetic methods.

## How does the Strom tutorial web site work?
The code stored in source branch of this repository is used to regenerate the web site for the tutorial. The static web site itself is stored in the master branch and is served as a GitHub Pages site. Jekyll is used to regenerate the static content. There are currently three versions of the tutorial:
- the **Linux version** (uses the meson/ninja build system and ilustrates how to write software without using an integrated development environment (IDE)
- the **Mac version** (illustrates how to develop the software using the Xcode IDE)
- the **Windows version** (illustrates how to develop the software using the Visual Studio IDE)

### Source code for each step

Note that it is possible to obtain the source code for any step in the tutorial from the source branch. Thus, if you ever want to use a particular step in the tutorial as the starting point for a project, you can do that by copying the _src_ directory associated with that step.

