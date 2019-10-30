---
layout: steps
title: Test creating Newick strings
partnum: 5
subpart: 2
description: Here you will test your new makeNewick function
---
{% assign OS = site.operatingsystem %}
Add the following highlighted line to {% indexfile main.cpp %} (after `createTestTree()`):
~~~~~~
{{ "steps/step-05/src/main.cpp" | polcodesnippet:"start-end","a" }}
~~~~~~
{:.cpp}

## Installing the Boost header files
{% indexhide boost, installing headers %}
{% if OS == "linux" %}
[//]: ##################################### LINUX #######################################
Build strom using the usual `meson install` command from the `build` directory:
~~~~~
cd ~/Documents/strom/build
meson install
~~~~~
{:.bash}
This time things will not go smoothly, however. You should see errors to the effect that {% indexfile boost/format.hpp %} does not exist. The problem is that we've used a feature from a header {% indexfile format.hpp %} in the Boost C++ library but have not yet told meson where this header file can be found. In fact, we have not yet even installed the Boost C++ library, so let's do that now.

In your browser, go to the [Boost C++ home page](https://www.boost.org), find the latest release, and navigate to the Downloads page. Copy the link address for the gzipped tar file for the latest version and download it into your home directory as follows:
~~~~~
cd
curl -LO https://dl.bintray.com/boostorg/release/1.71.0/source/boost_1_71_0.tar.gz
~~~~~
{:.bash}

Here, the -L switch tells curl to follow the path indicated if the original file has moved and redirection information has been provided. The -O switch tells curl to save the file under the name specified in the last part of the URL.

Create the directory {% indexfile ~/Documents/libraries %}, then use the tar command to uncompress and untar the {% indexfile boost_1_71_0.tar.gz %} into the libraries directory as follows:
~~~~~
cd ~/Documents
mkdir libraries
cd libraries
tar zxvf ~/boost_1_71_0.tar.gz
~~~~~
{:.bash}

We now need to modify the {% indexfile meson.build %} file to tell meson where to find the boost headers:
~~~~~~
{{ "steps/step-05/src/meson.build" | polcodesnippet:"",""}}
~~~~~~
{:.meson}

I've added an `include_directories` command pointing to the boost install directory and stored this directory as the meson variable `incl_boost`. You should of course use the correct absolute path to _your_ boost installation; unless your user name is `paul`, the `include_directories` command will not work as written above! Note that I've also appended `, include_directories: incl_boost` to the end of the `executable` command.

{% elsif OS == "mac" %}
[//]: ##################################### MAC #######################################

In your browser, go to the [Boost C++ home page](https://www.boost.org), find the latest release (1.71.0 at this writing), and navigate to the Downloads page. Download the file {% indexfile boost_1_71_0.tar.gz %} or {% indexfile boost_1_71_0.zip %} and unpack in the directory of your choice. I will assume that you unpacked it at {% indexfile ~/Documents/libraries %} so that you now have a directory {% indexfile ~/Documents/libraries/boost_1_71_0 %}.

### Create a custom path (alias) for the Boost library root directory

Choose _Xcode > Preferences..._ from the main menu, then choose the "Locations" tab, then click the "Custom Paths" subtab.

Click the `+` button and add a new custom path item named `BOOST_ROOT` with display name `BOOST_ROOT`. On my computer, I entered the path `/Users/plewis/Documents/libraries/boost_1_71_0` (but you should of course modify this for your case).

Click the red button at the top left to close the preferences dialog box.

### Add Boost to header search path

Click once on the _strom_ project (with the blue project icon :blueproject:) in the Project Navigator, then select the _strom_ TARGET within the project settings shown in the central pane. You should now see _General_, _Resource Tags_, _Build Settings_, _Build Phases_, and _Build Rules_ across the top of the central pane.

Click on _Build Settings_, _All_ (not _Basic_ or _Customized_), _Levels_ (not _Combined_), and then type "Header Search Paths" into the _Search_ box to filter out all build settings except the one we are interested in at the moment.

In the "Header Search Paths" row, double-click on the column with the {% indexshow blue project icon %} :blueproject: (this is the column for the project settings, the other column labeled strom contains target-specific settings).

Click the `+` button at the bottom left corner and type `$(BOOST_ROOT)` (this will expand to the custom path you defined earlier).

{% elsif OS == "win" %}
[//]: ##################################### WINDOWS #####################################
In your browser, go to the [Boost C++ home page](https://www.boost.org), find the latest release (1.71.0 at this writing), and navigate to the Downloads page. Download the file {% indexfile boost_1_71_0.zip %} and _extract_ into the directory of your choice. I will assume that you extracted it at {% indexfile ~/Documents/libraries %}, which I will hereafter refer to as the {% indexbold libraries directory %}). You should now have a directory {% indexfile ~/Documents/libraries/boost_1_71_0 %}, which I will hereafter refer to as `BOOST_ROOT`.

### Important notes about zip files in Windows

Windows can fool you into thinking you have a folder when in reality the apparent folder is still in form of a zip file. If what looks like the {% indexfile boost_1_71_0 %} folder has a zipper on the icon, beware! The zipper means it is still a zip file. VSC19 will only be able to use files if they have been extracted from the {% indexfile boost_1_71_0.zip %} file. 

Windows is notoriously slow at extracting large zip files, so I recommend downloading the [7-zip](www.7-zip.org/) program to do the unzipping for you. Once installed, you can right-click the zip file and choose _Extract here_ from the 7-zip menu to unpack the boost libraries. It will still take awhile (because Boost is huge) but it will unpack orders of magnitude faster than using the native Windows unzipper.

Finally, deleted the {% indexfile boost_1_71_0.zip %} file (the one with the zipper on the icon) after you extract it to avoid future confusion.

### Add Boost to header search path 

Now we need to tell VSC19 about the Boost library so that we can use Boost header files and compiled libraries in our project.

Right-click the _strom_ project (note: click the project, not the solution) in the Solution Explorer pane of VSC19 and choose _Properties_ from the popup menu.
Expand _VC++ Directories_, then click on _Include Directories_, then click the down-pointing triangle, then click _&lt;Edit...&gt;_ and add `BOOST_ROOT`, which, you remember from the paragraph above, is {% indexfile ~/Documents/libraries/boost_1_71_0 %}, where the `~` is a stand-in for your home directory. The easiest way to get this right is to use the button that allows you to add a new line, then click the elipses (`...`) button on the right end of that line, then choose the `BOOST_ROOT` directory using the file chooser. When I finished this process, the line read:
~~~~~~
C:\Users\Paul Lewis\Documents\libraries\boost_1_71_0
~~~~~~
{:.bash-output}

{% endif %}
[//]: ##################################### ENDIF #####################################

## Compile and run
Go ahead and compile strom again. This time it should compile and install cleanly. You should see the following additional line appear in the output when the program is run:
~~~~~
((1:0.100,2:0.100):0.100,3:0.200)
~~~~~
{:.bash-output}

Note that 3 decimal places are used in outputting edge lengths because you supplied 3 as an argument to the `makeNewick` function.

