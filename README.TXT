COMP2006 coursework
===================

This readme includes instructions for various different platforms since you may want to use different platforms.
Please note that we can support you on Windows, e.g. helping out if there is some configuration issue, but you should only use a different platform if you think you can do so without needing our support for it!
i.e. the different options are provided for your convenience, at your own risk.

You can demo the coursework using any platform you wish - just make sure it works when you demo it.
It should be possible to take your files to any other platform and they just run.
If you want to submit to the hall of fame, please recompile the code on the lab PCs at that time, and provide me an exe for windows.

IMPORTANT: If you change platforms or versions of Visual Studio after you have added files, you will need to remember to add them to the project / makefiles that you are using.


Windows:
========

Open the CourseworkBase.sln file in this directory using Visual Studio 2022.

When you build, the output executables will be placed in the Debug and Release directories in this folder.
When you run the program from within Visual Studio, it will look for things like the fonts and images in the src directory.
You MUST have the SDL dll files in your Debug and Release directories if you want to be able to run it. Copy everything from the SDL/SDL3_dlls_x86 directory into your Debug and Release directories and it will find the SDL dlls that it needs to run.
These dll files are already included in the project - you do not need to install SDL separately.


Mac or Linux
============

A CMakeLists.txt is included in this directory. Please refer to README_MAC_LINUX.TXT for instructions on how to build the project using CMake.
