Notepad++ plist plugin
==============

Adds support of binary plist files ( bplist ) to Notepad++.

Based on libimobiledevice.

HOW IT WORKS:
When bplist file is opened with np++, file's contents is automatically converted to friendly xml view. When file is saved - plugin converts modified file as bplist and saved data, then converts bplist again.
From the user-side conversion is invisible - all works on background in memory.