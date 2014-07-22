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
#include "defines.h"
#include "PlistMngr.h"
#include <memory>
#include <algorithm>
#include <unordered_map>

typedef std::unique_ptr<plist::PlistEntry> PlistEntryPtr;
#define MAKE_PLIST_PTR std::make_unique<plist::PlistEntry>

// int (key) - BufferId 
typedef std::unordered_map<int, PlistEntryPtr> PlistEntryPtrMap;

extern FuncItem funcItem[nbFunc];
extern NppData nppData;
std::unique_ptr<PlistEntryPtrMap> g_pLoadedBplists;


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  reasonForCall, 
                       LPVOID lpReserved )
{
    switch (reasonForCall)
    {
      case DLL_PROCESS_ATTACH:
        pluginInit(hModule);
        g_pLoadedBplists = std::make_unique<PlistEntryPtrMap>();
        break;

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

extern "C" __declspec(dllexport) const TCHAR * getName()
{
	return NPP_PLUGIN_NAME;
}

extern "C" __declspec(dllexport) FuncItem * getFuncsArray(int *nbF)
{
	*nbF = nbFunc;
	return funcItem;
}

bool IsValidBplist( CharVt& pFileBuff )
{
  if ( ( pFileBuff.size() > sizeof( defs::g_szHeader ) ) &&
    std::equal( std::begin( defs::g_szHeader ), std::end( defs::g_szHeader ), 
    pFileBuff.begin() )
    )
  {
    return true;
  }
  
  return false;
}

CharVt ReadFromSkintilla( HWND& hwndSkillaOut )
{
  int which = -1;
  ::SendMessage( nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&which );
  if ( which == -1 )
    return CharVt();
  HWND hCurrentEditView = (which == 0) ? nppData._scintillaMainHandle : nppData._scintillaSecondHandle;

  int cbText = ::SendMessage( hCurrentEditView, SCI_GETLENGTH, NULL, NULL );

  CharVt rawBuff( cbText + 1 );
  ::SendMessage( hCurrentEditView, SCI_GETTEXT, rawBuff.size(), (LPARAM)rawBuff.data() );

  // dummy trick to read cbText data from Skintlla:
  // We ARE NOT ABLE to read exact N bytes because of tail 0
  rawBuff.resize( cbText );

  hwndSkillaOut = hCurrentEditView;

  return std::move( rawBuff );
}

void InsertDataIntoSkilla( HWND hSkilla, const char* pData, size_t cbData )
{
  ::SendMessage( hSkilla, SCI_CLEARALL, NULL, NULL );
  ::SendMessage( hSkilla, SCI_ADDTEXT, cbData, (LPARAM)pData );
};

void MarkDocumentIsUnmodified( HWND hSkilla )
{
  ::SendMessage( hSkilla, SCI_SETSAVEPOINT, NULL, NULL );
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
        auto loadedBplist = g_pLoadedBplists->find( notifyCode->nmhdr.idFrom );

        if ( loadedBplist == g_pLoadedBplists->end() )
        {
          // Get the current scintilla

          HWND hSkilla;
          CharVt rawBuff = ReadFromSkintilla( hSkilla );

          // not a bplist file
          if ( !IsValidBplist( rawBuff ) )
          {
            break;
          }

          PlistEntryPtr plistEntry = MAKE_PLIST_PTR( std::move( rawBuff ) );

          // get converted text to display
          auto XMLVt = plistEntry->GetXML();
          InsertDataIntoSkilla( hSkilla, XMLVt.data(), XMLVt.size() );

          g_pLoadedBplists->insert( std::make_pair( notifyCode->nmhdr.idFrom, std::move( plistEntry ) ) );

          MarkDocumentIsUnmodified( hSkilla );
        }
      }
        break;
      case NPPN_FILEBEFORESAVE:
      {
        auto loadedBplist = g_pLoadedBplists->find( notifyCode->nmhdr.idFrom );

        if ( loadedBplist != g_pLoadedBplists->end() )
        {
          // Read all content -> convert it back to Bplist and save
          HWND hSkilla;
          CharVt xmlPlistStr = ReadFromSkintilla( hSkilla );

          loadedBplist->second->UpdateXML( std::move( xmlPlistStr ) );

          auto binPlist = loadedBplist->second->GetBinPlist();
          InsertDataIntoSkilla( hSkilla, binPlist.data(), binPlist.size() );
        }
      }
        break;
      case NPPN_FILESAVED:
      {
        auto loadedBplist = g_pLoadedBplists->find( notifyCode->nmhdr.idFrom );

        if ( loadedBplist != g_pLoadedBplists->end() )
        {
          // Get the current scintilla

          HWND hSkilla;
          CharVt rawBuff = ReadFromSkintilla( hSkilla );

          // not a bplist file
          if ( !IsValidBplist( rawBuff ) )
          {
            break;
          }

          loadedBplist->second->UpdateBPlist( std::move( rawBuff ) );

          // get converted text to display
          auto XMLVt = loadedBplist->second->GetXML();
          InsertDataIntoSkilla( hSkilla, XMLVt.data(), XMLVt.size() );
        }
      }
        break;
      case NPPN_FILECLOSED:
      {
        g_pLoadedBplists->erase( notifyCode->nmhdr.idFrom );
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
