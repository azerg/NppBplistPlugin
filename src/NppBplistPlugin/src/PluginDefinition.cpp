//this file is part of notepad++
//Copyright (C)2003 Don HO <donho@altern.org>
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#include "PluginDefinition.h"
#include "menuCmdID.h"
#include "BplistMngr.h"
#include "AboutWnd.h"

#include <Windows.h>
#include <stdexcept>
#include <memory>

//
// The plugin data that Notepad++ needs
//
FuncItem funcItem[nbFunc];

//
// The data of Notepad++ that you can use in your plugin commands
//
NppData nppData;

//
// Initialize your plugin data here
// It will be called while plugin loading
BOOL pluginInit( HANDLE hModule ) noexcept
{
  return bplist::InitPlugin();
}

//
// Here you can do the clean up, save the parameters (if any) for the next session
//
void pluginCleanUp() noexcept
{
  bplist::FreePlugin();
}

//
// Initialization of your plugin commands
// You should fill your plugins commands here
void commandMenuInit() noexcept
{

    //--------------------------------------------//
    //-- STEP 3. CUSTOMIZE YOUR PLUGIN COMMANDS --//
    //--------------------------------------------//
    // with function :
    // setCommand(int index,                      // zero based number to indicate the order of command
    //            TCHAR *commandName,             // the command name that you want to see in plugin menu
    //            PFUNCPLUGINCMD functionPointer, // the symbol of function (function pointer) associated with this command. The body should be defined below. See Step 4.
    //            ShortcutKey *shortcut,          // optional. Define a shortcut to trigger this command
    //            bool check0nInit                // optional. Make this menu item be checked visually
    //            );
    //setCommand(0, TEXT("Hello Notepad++"), hello, NULL, false);
    //setCommand(1, TEXT("About"), helloDlg, NULL, false);

    setCommand( 0, TEXT("Is currently opened file a bplist file?"), IsItABplistFileHandler, NULL, false );
    setCommand( 1, TEXT("-"), nullptr, NULL, false);
    setCommand( 2, TEXT("About"), helloDlg, NULL, false);
}

//
// Here you can do the clean up (especially for the shortcut)
//
void commandMenuCleanUp()
{
  // Don't forget to deallocate your shortcut here
}


//
// This function help you to initialize your plugin commands
//
bool setCommand( size_t index, const TCHAR *cmdName, PFUNCPLUGINCMD pFunc, ShortcutKey *sk, bool check0nInit ) noexcept
{
    if (index >= nbFunc)
        return false;

    if (!pFunc)
        return false;

    lstrcpy(funcItem[index]._itemName, cmdName);
    funcItem[index]._pFunc = pFunc;
    funcItem[index]._init2Check = check0nInit;
    funcItem[index]._pShKey = sk;

    return true;
}

//----------------------------------------------//
//-- STEP 4. DEFINE YOUR ASSOCIATED FUNCTIONS --//
//----------------------------------------------//
void helloDlg() noexcept
{
  ::MessageBox(NULL
  , TEXT("Just open bplist file with Notepad++ and use it as ordinary text file.")
  , TEXT("Notepad++ plist plugin")
  , MB_OK);
}

void IsItABplistFileHandler() noexcept
{
  std::wstring msg =
    bplist::IsCurrentFileIsABplistFile() ? L"Bplist file!" : L"NOT a Bplist file!";

  ::MessageBoxW(NULL
    , msg.c_str()
    , L"Notepad++ plist plugin"
    , MB_OK);
}