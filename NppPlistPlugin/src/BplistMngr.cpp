#include "BplistMngr.h"
#include "defines.h"
#include "PluginDefinition.h"

#include <memory>
#include <unordered_map>

typedef std::unique_ptr<plist::PlistEntry> PlistEntryPtr;
#define MAKE_PLIST_PTR std::make_unique<plist::PlistEntry>

//
// int (key) - BufferId
//
typedef std::unordered_map<int, PlistEntryPtr> PlistEntryPtrMap;

//---------------------------------------------------------------------------------

extern FuncItem funcItem[nbFunc];
extern NppData nppData;

//
// Stores list of loaded bplists. Is used to return converted xml text for exact bplist
//
std::unique_ptr<PlistEntryPtrMap> g_pLoadedBplists;

namespace plist
{

  BOOL InitPlugin()
  {
    try
    {
      g_pLoadedBplists = std::make_unique<PlistEntryPtrMap>();
    }
    catch ( std::bad_alloc& )
    {
      return FALSE;
    }

    return TRUE;
  }

  //
  // We have to force calling destructor for global objects in DLL ( free manyally on DLL_UNLOAD )
  //
  void FreePlugin() _NOEXCEPT
  {
    g_pLoadedBplists.reset();
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
  }

  void MarkDocumentIsUnmodified( HWND hSkilla )
  {
    //::SendMessage( hSkilla, SCI_SETSAVEPOINT, NULL, NULL );
  }

  ///////////////////////////////////////////////////////////////////////////////
  //
  // PLUGIN MESSAGES
  //
  //////////////////////////////////////////////////////////////////////////////

  void OnBufferActivated( SCNotification *notifyCode )
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
        return;
      }

      PlistEntryPtr plistEntry = MAKE_PLIST_PTR( std::move( rawBuff ) );

      // get converted text to display
      auto XMLVt = plistEntry->GetXML();
      InsertDataIntoSkilla( hSkilla, XMLVt.data(), XMLVt.size() );

      g_pLoadedBplists->insert( std::make_pair( notifyCode->nmhdr.idFrom, std::move( plistEntry ) ) );

      MarkDocumentIsUnmodified( hSkilla );
    }
  }

  void OnFileSaved( SCNotification *notifyCode )
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
        return;
      }

      loadedBplist->second->UpdateBPlist( std::move( rawBuff ) );

      // get converted text to display
      auto XMLVt = loadedBplist->second->GetXML();
      InsertDataIntoSkilla( hSkilla, XMLVt.data(), XMLVt.size() );
    }
  }

  void OnFileBeforeSave( SCNotification *notifyCode )
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

  void OnFileClosed( SCNotification *notifyCode )
  {
    g_pLoadedBplists->erase( notifyCode->nmhdr.idFrom );
  }

} // namespace plist