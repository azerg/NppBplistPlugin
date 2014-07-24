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
#include "BplistMngr.h"


extern FuncItem funcItem[nbFunc];
extern NppData nppData;


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  reasonForCall, 
                       LPVOID lpReserved ) _NOEXCEPT
{
    switch (reasonForCall)
    {
      case DLL_PROCESS_ATTACH:
        return pluginInit(hModule);

      case DLL_PROCESS_DETACH:
        pluginCleanUp();
        break;
    }

    return TRUE;
}


extern "C" __declspec(dllexport) void setInfo(NppData notpadPlusData)
{
	nppData = notpadPlusData;
	commandMenuInit();
}

extern "C" __declspec(dllexport) const TCHAR * getName() _NOEXCEPT
{
	return NPP_PLUGIN_NAME;
}

extern "C" __declspec(dllexport) FuncItem * getFuncsArray( int *nbF ) _NOEXCEPT
{
	*nbF = nbFunc;
	return funcItem;
}

extern "C" __declspec(dllexport) void beNotified(SCNotification *notifyCode)
{
  try
  {
    switch ( notifyCode->nmhdr.code )
    {

      case NPPN_SHUTDOWN:
      {
        commandMenuCleanUp();
      }
        break;
      case NPPN_BUFFERACTIVATED:
      {
        plist::OnBufferActivated( notifyCode );
      }
        break;
      case NPPN_FILEBEFORESAVE:
      {
        plist::OnFileBeforeSave(notifyCode);
      }
        break;
      case NPPN_FILESAVED:
      {
        plist::OnFileSaved( notifyCode );
      }
        break;
      case NPPN_FILECLOSED:
      {
        plist::OnFileClosed( notifyCode );
      }
        break;
      default:
        return;
    }
  }
  catch ( std::bad_alloc& )
  {
    // ToDo: log err
  }
  catch ( std::runtime_error& err )
  {
    ::MessageBoxA( NULL, err.what(), "Notepad++ plist plugin error", MB_ICONERROR );
  }
}


// Here you can process the Npp Messages 
// I will make the messages accessible little by little, according to the need of plugin development.
// Please let me know if you need to access to some messages :
// http://sourceforge.net/forum/forum.php?forum_id=482781
//
extern "C" __declspec(dllexport) LRESULT messageProc(UINT Message, WPARAM wParam, LPARAM lParam)
{/*
	if (Message == WM_MOVE)
	{
		::MessageBox(NULL, "move", "", MB_OK);
	}
*/
	return TRUE;
}

#ifdef UNICODE
extern "C" __declspec(dllexport) BOOL isUnicode()
{
    return TRUE;
}
#endif //UNICODE
