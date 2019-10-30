---
layout: steps
title: Setting Up A Build System
partnum: 1
subpart: 0
description: The first step is to set up the build system for your platform.
---
{% assign OS = site.operatingsystem %}
{% if OS == "linux" %}
[//]: ################################### LINUX #########################################

The Linux version will not use an Integrated Development Environment as this version is designed to show how to use the [Meson](https://mesonbuild.com) and [Ninja](https://ninja-build.org) {% indexhide Meson build system %} {% indexhide Ninja build system %} build systems to simplify compiling and linking to create each step of the program being developed during the tutorial. Many projects use other build systems, such as cmake or Gnu autotools (the familiar configure/make combination). Here Meson takes the place of `configure` and Ninja takes the place of `make` and the Meson/Ninja combination greatly simplifies building a complex executable that requires linking several third-party libraries (Nexus Class Library, Boost Program Options, and BeagleLib).

I will assume you are using Ubuntu 16.04 LTS Linux. The instructions will probably still work if you have a different flavor of Linux, but in this case you will need to figure out how to install the needed tools for your system (e.g. for a Fedora system you would use the package manager `yum` rather than `apt` to install tools).

The first step is to ensure that your system has some critical capabilities installed.

## Test

operatingsystem is {{ site.operatingsystem }}.

## Install the Gnu C++ compiler
~~~~~
sudo apt install build-essential
~~~~~
{:.bash}
**Note:** Do not type the initial `$` symbol. That is the {% indexshow bash prompt %} character and it is included here so that you know that this code block contains commands to be issued when you are in a bash shell. If you are in a bash shell (bash is the standard command interpreter in Linux), you should see the `$` in your terminal (perhaps preceded by other information, such as your username and the current directory), but you should only type (or copy/paste) what follows the `$` character.

## Install Git for version control
~~~~~
sudo apt install git
~~~~~
{:.bash}

## Install curl for downloading files from URLs
~~~~~
sudo apt install curl
~~~~~
{:.bash}

## Install unzip for unpacking zip files
~~~~~
sudo apt install unzip
~~~~~
{:.bash}

## Install the meson build system
Because meson is a python app, we will use pip to install it. This will also ensure that we install the latest version. We first must ensure that pip itself is installed:
~~~~~
sudo apt install python3-pip
pip3 install meson
~~~~~
{:.bash}

## Install ninja
Download the latest linux binary of ninja (see [release list](https://github.com/ninja-build/ninja/releases). Here's how to do this using curl for version 1.8.2:
~~~~~
cd
curl -LO https://github.com/ninja-build/ninja/releases/download/v1.8.2/ninja-linux.zip
unzip ninja-linux.zip
rm ninja-linux.zip
sudo mv ninja /usr/local/bin
~~~~~
{:.bash}
We can't use the apt package manager to install ninja because the version of ninja installed will probably be too old to work with the version of meson that you just installed.

## Create a folder for your project
Create a folder named *strom* inside your {% indexfile ~/Documents %} directory, create a directory *src* inside {% indexfile ~/Documents/strom %}, then create the {% indexfile meson.build %} and {% indexfile main.cpp %} files inside {% indexfile ~/Documents/strom/src %}. You should also create a directory {% indexfile distr %} to serve as an install target. The commands for doing this are:
~~~~~
cd ~/Documents
mkdir strom
cd strom
mkdir distr
mkdir src
cd src
touch meson.build
touch main.cpp
~~~~~
{:.bash}
**Note:** The tilde (`~`) in the change-directory (`cd`) command is replaced by your home directory. The make-directory (`mkdir`) command creates a new directory. The nominal duty of the `touch` command is to modify the time stamp for a file, but if the file specified does not exist, it can be used to create an empty file by that name (and that is its purpose here).

## Editing files

You can use whatever editor you like to modify text files. There is a built-in text editor in Ubuntu called "Text Editor". Because I work on linux systems remotely from a Mac, I use the [BBEdit](http://www.barebones.com/products/bbedit/) editor on my Mac, editing files on the Ubuntu server remotely using the *File > Open From FTP/SFTP Server...* menu command. You are of course also free to use emacs, vim, or nano. The program nano is usually the easiest text editor to use for Linux newbies. Just be sure to use a *text editor*, not a *word processor*, so that you end up with text files and not documents filled with formatting commands that will not be understood by compilers, autotools, etc.

## Editing *meson.build*

Use your favorite text editor to add the following text to the currently empty file {% indexfile ~/Documents/strom/src/meson.build %}:
~~~~~~
{{ "steps/step-01/src/meson.build" | polcodesnippet:"start-end",""}}
~~~~~~
{:.meson}
We will add to this file as the program develops, but this much will enable us to compile the file {% indexfile main.cpp %} using the [C++11 programming language standard](https://en.wikipedia.org/wiki/C%2B%2B11).

The first line of meson.build declares the name of the project (`strom`), which is a C++ program (`cpp`), defines the version (`1.0`), and specifies a couple of default options. The `cpp_std=c++11` specifies that we want to use features of C++11. The `cpp_link_args=-static` causes the standard libraries (and libraries we add) to be directly incorporated into (statically linked to) the strom executable. Without this option, these libraries would be dynamically linked, which would work but slightly complicates running the program if some of the libraries are not installed in standard locations where they can be located by the operating system. We will thus statically link every library to our executable so that everything needed is in a single file on the disk. Documantation for options such as `cpp_std` and `cpp_link_args` can be found at the [Meson Built-in Options web page](https://mesonbuild.com/Builtin-options.html).

Finally, the `prefix=/home/plewis/Documents/strom/distr` option tells meson where we wish to install our program. Note that the prefix directory is an absolute URL on _my_ system; **you should modify this so that it specifies an actual directory on your system.**

The second line tells meson that we wish to create an executable file (a program) named `strom` by compiling `main.cpp` and that we wish to install this program into the directory specified by the prefix that we declared in the `default_options` section. The dot `.` means "current directory", but note that this is relative to the prefix, so by using `'.'` we are telling meson that we wish to install this program directly into the prefix directory, not a subdirectory within the directory defined by the prefix.

## Editing *main.cpp*

We will need at least one source code file in order to test out the build system, so add the following text to the currently empty {% indexfile ~/Documents/strom/src/main.cpp %} file:
~~~~~~
{{ "steps/step-01/src/main.cpp" | polcodesnippet:"start-end",""}}
~~~~~~
{:.cpp}

This is the C++ version of the [Hello World!](https://en.wikipedia.org/wiki/"Hello,_World!"_program) program.

## Building the "Hello World!" program

Create a directory to hold the build products:
~~~~~
cd ~/Documents/strom/src
mkdir build
~~~~~
{:.bash}
Everything in this *build* folder can later be deleted because the contents of this folder are automatically generated from your source files. The first (`cd`) command simply ensures that you are in the correct directory before you issue the `mkdir` command; you do not need to type the `cd` command if you are sure you are in that directory already.

Now run meson to create the *build.ninja* file inside the newly-created *build* directory:
~~~~~
meson ./build
~~~~~
{:.bash}
This `meson ./build` command is analogous to the `configure` command you may have used when installing packages that use Gnu autotools. The *build.ninja* file is analogous to the *Makefile* generated by the `configure` command and used by the `make` command to compile and link a program.

Finally, run ninja to do the actual building:
~~~~~
ninja -C ./build
~~~~~
{:.bash}
Ninja looks for the build.ninja file in the directory specified (the `-C` switch tells ninja to change to the build directory before doing anything), and uses the information therein to build the program.

The executable file should now be found inside the {% indexfile ~/Documents/strom/src/build %} directory. Try to run it just to make sure everything worked:
~~~~~
cd ~/Documents/strom/src/build
./strom
~~~~~
{:.bash}
You should see the output
~~~~~
Hello World!
~~~~~
{:.bash-output}

While you are still inside the build directory, try typing
~~~~~
meson install
~~~~~
{:.bash-output}
This will install the `strom` executable in the directory specified by the variable prefix in the {% indexfile meson.build %} file, which should be the directory {% indexfile ~/Documents/strom/distr %}. Because meson install calls ninja automatically, you can actually skip the ninja step in the future.

{% elsif OS == "mac" %}
[//]: ################################### MAC #########################################

I am assuming you are using MacOS 10.14.6 (Mojave) or later and Xcode 10.3; these instructions have not been tested on earlier versions.

## Installing Xcode
Install [Xcode](https://developer.apple.com/xcode/) if it is not already installed on your Mac. I will assume you are using Xcode version 10.3.

After installing {% indexshow Xcode %}, you may also want to install the command line tools. To do this, go to _Xcode > Open Developer Tool > More Developer Tools..._, login using your existing Apple ID, then choose the latest Command Line Tools download.

## Creating a command line tool Xcode project

In the _Welcome to Xcode_ window, click on _Create a new Xcode project_, or choose _File > New > Project..._ from the Xcode main menu.

Under the MacOS tab, choose the _Command Line Tool_ Application type and click the _Next_ button.

Here's how I filled out the next dialog box (but you should feel free to enter whatever you like here):

|        Description       |           Value           |
| -----------------------: | :------------------------ |
|            Product name: | strom                     |
|                    Team: | None                      |
|       Organization Name: | Paul O. Lewis             |
| Organization Identifier: | edu.uconn.phylogeny       |
|       Bundle Identifier: | edu.uconn.phylogeny.strom |
|                Language: | C++                       |

Next you will be asked to specify a directory in which to create the project. This location is entirely up to you. I chose {% indexfile /Users/plewis/Documents %}. For this choice of parent folder, Xcode created the following folder structure:
~~~~~~
Users/
    plewis/
        Documents/
            strom/
                strom.xcodeproj
                strom/
                    main.cpp
~~~~~~
{:.bash-output}

## Setting up Xcode to use the C++11 standard

We will use the [C++11](https://en.wikipedia.org/wiki/C%2B%2B11) standard for this tutorial. As of this writing, C++17 is the latest C++ _dialect_. You are welcome to use the C++17 dialect, but because this tutorial does not use any C++ features that are not in the C++11 standard, I will show how to set up Xcode and external libraries to use C++11 so that third-party libraries will link correctly to the tutorial. If you decide to go with C++17, be sure to use C++17 when compiling every library or you will have difficult-to-diagnose errors at the link step.

In the Xcode Project Navigator pane on the left, you should now see your strom project (beside a blue project icon :blueproject:), and inside that you should see a strom folder (beside a yellow folder icon :yellowfolder:), and inside that you should see a file named _main.cpp_. (If you do not see anything like this, it may be because your Project Navigator pane is not visible. You can always get it back using _View > Navigators > Show Project Navigator_ from the main menu.)

Click once on the _strom_ project (with the blue project icon :blueproject:) in the Project Navigator, then select the _strom_ TARGET within the project settings shown in the central pane. You should now see _General_, _Resource Tags_, _Build Settings_, _Build Phases_, and _Build Rules_ across the top of the central pane.

Click on _Build Settings_, _All_ (not _Basic_ or _Customized_), _Levels_ (not _Combined_), and then type "C++ Language Dialect" into the _Search_ box to filter out all build settings except the one we are interested in at the moment.

In the "C++ Language Dialect" row, double-click on the column with the {% indexshow blue project icon %} :blueproject: (this is the column for the project settings, the other column labeled strom contains target-specific settings). This should allow you to choose `C++11 [-std=c++11]` as your C++ dialect.

## Running the default "Hello, World!" version of the program

Press the run button (:arrow_forward:) in the tool bar and your minimal "Hello, World!" program should compile and run successfully.

## Setting up Xcode to save an optimized executable

Throughout most of this tutorial (and especially for this first _Hello, World!_ program), you will not need to optimize your executable for speed to run the examples, but you will want to create an optimized {% indexbold release version %} of your executable to use for real data examples, where the speed is essential. To create an optimized version, you need to select _Archive_ from the _Product_ main menu in Xcode. The optimized version will be stored in an out-of-the-way part of your hard drive where Xcode keeps its archived products (you can find where this is by going to the Xcode _Preferences..._ menu item and clicking on the _Locations_ tab). Xcode also stores this location in an environmental variable `$ARCHIVE_PRODUCTS_PATH`. It is a pain to extract your executable from this obscure location each time you need it (even though Xcode gives you a button to save it somewhere else), so let's set up a {% indexbold post-action script %} to automatically copy the release version to a directory that is on your executable path and thus can be run from anywhere.

Start by choosing the Xcode _Product_ menu item _while holding down the Option key_, then selecting the _Archive..._ menu item. (Note: if the menu item is _Archive_ and not _Archive..._, then you failed to hold down the Option key while selecting the _Product_ menu item). Another way to do this is to use the key combination _Command-Option-r_ and then click on _Archive_ in the left panel.

Expand the _Archive_ action by clicking on the triangular disclosure button on the left. This allows you to add a post-action. Add a New Run Script Action by pressing the + button at the bottom left of the post-action panel and replace _# Type a script or drag a script file from your workspace to insert its path._ with the following small script:
~~~~~~
exec > "/tmp/xcode_run_script.log" 2>&1
cp "$ARCHIVE_PRODUCTS_PATH/usr/local/bin/strom" /Users/plewis/bin
~~~~~~
{:.bash-output}
You will, of course, need to change some parts of this, because you are not me, but the script is pretty simple. The first line uses the exec command to save all output to a temporary file named {% indexfile /tmp/xcode_run_script.log %}. This allows you to see what went wrong if anything does go wrong in the copying process. The second line simply copies the executable (named {% indexfile strom %}) from the Xcode archive location ({% indexfile $ARCHIVE_PRODUCTS_PATH/usr/local/bin %}) to a directory of your choice. I have used the {% indexfile bin %} directory inside my home directory {% indexfile /Users/plewis %}, which is a directory I created for storing programs that I want to be able to run from anywhere. I made this happen by adding this directory to my `PATH` environmental variable in my {% indexfile $HOME/.bash_profile %} file:
~~~~~~
export PATH="$HOME/bin:$PATH"
~~~~~~
{:.bash-output}
Be sure to specify {% indexfile /bin/bash %} as the Shell to use for interpreting your post-action script, then press the Archive button to create the release executable. From now on, whenever I ask you to create a release version of your program, you can simply choose _Archive_ from the _Product_ menu and your post-action script will automatically copy your program to the directory you specified!

{% elsif OS == "win" %}

[//]: ################################### WINDOWS #########################################

I am assuming you are using Windows 10 (but these instructions may also work for other versions of Windows).

## Installing Visual Studio

1. Download and install the free Microsoft Visual Studio Community 2019 for Windows from the [Visual Studio web site](http://www.visualstudio.com/)
1. Under Workloads, choose _Desktop development with C++_, which will check _Visual C++ core desktop features_ and several optional components. Check also the _Clang/C2 (experimental)_ optional component
1. Click the _Individual components_ tab and, under _Code tools_, choose _Git for Windows_ and _GitHub extension for Visual Studio_. Note that the installation will take a very long time!
1. Click Install to start the installation
1. Once it is downloaded, you will need to restart Windows
1. After starting VSC19 (my abbreviation for Visual Studio Community 2019), you will be asked to select a default configuration: choose Visual C++ here (we will not be using any other development environment for this project)

## Creating a solution with one project

1. Start VSC19
1. Choose _Create a new project_ (_Console App_), and name it _strom_ in the next panel. I will refer to the folder where the strom project is saved as `PROJECT_DIR`. For me, `PROJECT_DIR` is simply the default: {% indexfile C:\Users\plewis\source\repos %}.

{% endif %}
[//]: ################################### ENDIF #########################################
