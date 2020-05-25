Notepad++ bplist plugin 
- [![GitHub release](https://img.shields.io/github/release/azerg/NppBplistPlugin.svg?style=flat)](https://github.com/azerg/NppBplistPlugin/releases)
- [![Build status](https://ci.appveyor.com/api/projects/status/ox8kagplw7gvg092?svg=true)](https://ci.appveyor.com/project/azerg/nppbplistplugin)
- [![License](http://img.shields.io/badge/license-NewBSD-brightgreen.svg?style=flat-squar)](http://opensource.org/licenses/BSD-3-Clause)

==============
>Check out plugin in Notepad++ plugin manager

NotepadBplistPlugin supports viewing\editing binary plist files. As long as ordinary plist files comes in XML format, this plugin dont supports them. It loads only binary plist files ( bplist ).

#Installation
- Use Notepad++ plugin manager to manage binary plist plugin

#How it works
Just open bplist file with notepad++. You are free to edit is as usual as long as you will do it in a valid XML way. You could type an invalid XML code - in this case when you will try to save bplist, plugin will be not able to conver invalid XML to bplist and will restore original bplist data ( before your modifications )


#Dependencies
- [libimobiledevice]  - for libplist library
- [libxml2] - xml reader\writer

[libimobiledevice]:http://www.libimobiledevice.org/
[libxml2]:http://xmlsoft.org/
