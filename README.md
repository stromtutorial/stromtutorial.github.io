# source
Repository containing the source code for the Strom Tutorial GitHub Pages web site.

## What is the Strom Tutorial?
The Strom Tutorial shows how to create C++ software for estimating phylogenetic trees using Bayesian MCMC. It is intended to provide a means for getting up to speed if you seriously want or need to learn how phylogenetics software works. A use case would be a graduate student interested in developing and testing a new phylogenetic method for their thesis research, or a postdoctoral researcher who obtained a Ph.D. in systematics but is beginning work in a laboratory that develops new phylogenetic methods.

## How does the Strom tutorial web site work?
The code stored in this repository is used to regenerate the web site for the tutorial. The web site is served as a GitHub Pages site, and Jekyll is used to regenerate the static content. There are currently three versions of the tutorial:
- the **Linux version** (uses the meson/ninja build system and ilustrates how to write software without using an integrated development environment (IDE)
- the **Mac version** (illustrates how to develop the software using the Xcode IDE)
- the **Windows version** (illustrates how to develop the software using the Visual Studio IDE)

The Linux version is published as https://stromtutorial.github.io/linux/, the Mac version as  https://stromtutorial.github.io/mac/, and the Windows version as https://stromtutorial.github.io/windows/.

### Source code for each step

Note that it is possible to obtain the source code for any step in the tutorial from this source repository. Thus, if you ever want to use a particular step in the tutorial as the starting point for a project, you can do that by copying the _src_ directory associated with that step. The source code blocks quoted in the tutorial are taken directly from the _src_ directory for the step. The Markdown file for each step contains Liquid tags that make use of the polsnippet and polcodesnippet filters (in the _\_plugins/filters.rb_ file), which specifies which lines from which source code file shoud be shown at that point in the tutorial. The fact that the tutorial takes its code directly from the _src_ directory means that there is never any divergence between the tutorial and code that is known to work correctly.

