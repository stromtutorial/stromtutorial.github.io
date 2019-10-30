---
layout: steps
title: Test Node and Tree
partnum: 2
subpart: 4
description: Test the new `Node` and `Tree` classes.
---
{% assign OS = site.operatingsystem %}
It is now time to test what we have done thus far.

## Compiling and running strom

{% if OS == "linux" %}
[//]: ##################################### LINUX #######################################

To build your program, type the following commands at the linux prompt (the first line is of course not necessary if you are already in the {% indexfile ~/Documents/strom/build %} folder). You do not need to issue the `meson ./build` command any more: meson is all set up and running `meson install` is all that is needed to build `strom` and copy the executable to the `distr` directory.
~~~~~
cd ~/Documents/strom/build
meson install
~~~~~
{:.bash}
You should now find an executable file named {% indexfile strom %} inside the directory {% indexfile ~/Documents/strom/distr %}. To run the program, type:
~~~~~
cd ~/Documents/strom/distr
./strom
~~~~~
{:.bash}

{% elsif OS == "mac" %}
[//]: ##################################### MAC #######################################

Click the large right-pointing triangular button in Xcode to start running the program (or choose *Product > Run* from the main menu). The expected output should be shown in the output pane in the Debug Area. (If you don’t see this, try choosing _View > Debug Area > Show Debug Area_ from the main Xcode menu.)

You may wish to save your built executable file so that you can run it outside the Xcode IDE. Choose _Product > Archive_ from the main menu, then click on _Distribute..._ and choose "Save Built Products", specifying any directory you choose (e.g. your Desktop). The executable file will be in the folder {% indexfile /usr/local/bin %} inside the archive directory that was saved.

You can run your newly created program in a terminal window, for example the window that appears when you start the Terminal app from your _Applications > Utilities_ folder. You may wish to invest in the [PathFinder app](https://cocoatech.com/#/), which makes it easy to open a terminal window anywhere by Control-clicking any folder. While PathFinder is not free, you will probably find that it is well worth the investment (for example, after you have spent many hours trying to navigate to a particular folder using `cd`). You may also wish to use the free terminal app [iTerm2](https://www.iterm2.com) rather than the default Terminal app.

{% elsif OS == "win" %}
[//]: ##################################### WINDOWS #####################################

In VSC19, click on the _Local Windows Debugger_ button on the toolbar. VSC19 should rebuild your program and open it in a new console window. If there are errors reported, you will need to carefully compare the contents of the files we have created with the tutorial versions to find what is different. Note that you will not see the output that you expect in the output area of the IDE. This IDE pane shows only output related to building the executable, not the output of the program itself!

{% comment %}
The problem is that the terminal window in which the program runs appears (perhaps hidden behind VSC19) and then disappears when the program finishes. We can stop it from disappearing by placing a breakpoint on the last line of code executed. To set the breakpoint, get main.cpp visible in the main VSC19 editor window and single-click in the left margin on the line containing the final right curly bracket (i.e. the last line of the file). You should see a red filled circle appear. Now click the Local Windows Debugger button on the toolbar again. The appearance of a yellow arrow inside the red filled breakpoint circle indicates that the program has been paused on this line of code. You should now be able to locate the terminal window containing the output of the program (perhaps behind the VSC19 window). When you are finished, you can finish running the program by clicking on the button with a red square in the toolbar (the tooltip text that appears when you hover over this button says Stop Debugging (Shift + F5).

## Making the DLLs available at run time
If you chose option 2 when setting up Boost and BeagleLib, then you were told to wait until the Debug directory was created before copying the DLL files. You should now be able to do that because running your program for the first time should have created the Debug directory inside your strom solution directory. The relevant pages are Setting up the Boost C++ library and Setting up BeagleLib. Revisit these two pages and follow the instructions for option 2 in the section entitled "Making the DLLs available at run time" inside the Windows instructions.
{% endcomment %}

{% endif %}
[//]: ##################################### ENDIF #######################################

## Output
The expected output of the program thus far is shown below:
~~~~~
Starting...
Constructing a Tree

Finished!
Destroying a Tree
~~~~~
{:.bash-output}
The `Starting...` and `Finished!` lines are created by the `main` function, while the other strings are output in the `Tree` constructor and destructor. Note that there is no output from the `Node` constructor or destructor. That is because, so far, no `Node` objects have been created: we have created a `Tree` with no nodes, not even a root node!

{% if OS == "win" %}
[//]: ##################################### WINDOWS #####################################

## Creating a release version

The program you just executed was the {% indexbold debug version %} of strom. The debug version runs slowly because it is not optimized. Not optimizing the program allows the IDE (VSC19) to let you run the program one line at a time and see the values of variables as the program executes. This is helpful when debugging, but ultimately we will want to create a {% indexbold release version %} that is fully optimized and as fast as possible.

To create a release version, simply change _Debug_ to _Release_ in the Solution Configurations dropdown of VSC19, then choose _Build Solution_ from the _Build_ menu (or press _Control-Shift-b_).

To run the release version outside of VSC19, open Windows Explorer, hold down the shift key while right-clicking the _Release_ folder inside `PROJECT_DIR`, then choose _Open PowerShell window here_ from the popup menu that appears. Type `.\strom` in the console window that appears. You should see the expected output when the program runs.

{% endif %}
[//]: ##################################### ENDIF #######################################
