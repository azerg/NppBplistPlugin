#include "BplistMngr.h"
#include "defines.h"
#include "PluginDefinition.h"

#include <memory>
#include <unordered_map>
#include <assert.h>
#include <stdexcept>
#include <optional>

typedef std::unique_ptr<bplist::PlistEntry> PlistEntryPtr;
#define MAKE_PLIST_PTR std::make_unique<bplist::PlistEntry>

//
// int (key) - BufferId
//
typedef std::unordered_map<LRESULT, PlistEntryPtr> PlistEntryPtrMap;

//---------------------------------------------------------------------------------

extern FuncItem funcItem[nbFunc];
extern NppData nppData;

//
// Stores list of loaded bplists. Is used to return converted xml text for exact bplist
//
std::unique_ptr<PlistEntryPtrMap> g_pLoadedBplists;

namespace bplist
{
  namespace
  {
    std::optional<int> GetCurrentEditTextLength() noexcept
    {
      int which = -1;
      ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&which);
      if (which == -1)
        return std::nullopt;
      HWND hCurrentEditView = (which == 0) ? nppData._scintillaMainHandle : nppData._scintillaSecondHandle;

      return (int)::SendMessage(hCurrentEditView, SCI_GETLENGTH, NULL, NULL);
    }

    CharVt ReadFromSkintilla(HWND& hwndSkillaOut, unsigned bytesToRead)
    {
      int which = -1;
      ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&which);
      if (which == -1)
        return CharVt();
      HWND hCurrentEditView = (which == 0) ? nppData._scintillaMainHandle : nppData._scintillaSecondHandle;

      CharVt rawBuff(bytesToRead + 1);
      ::SendMessage(hCurrentEditView, SCI_GETTEXT, rawBuff.size(), (LPARAM)rawBuff.data());

      // dummy trick to read cbText data from Skintlla:
      // We ARE NOT ABLE to read exact N bytes because of tail 0
      rawBuff.resize(bytesToRead);

      hwndSkillaOut = hCurrentEditView;

      return rawBuff;
    }

    CharVt ReadFromSkintilla(HWND& hwndSkillaOut)
    {
      int which = -1;
      ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&which);
      if (which == -1)
        return {};
      HWND hCurrentEditView = (which == 0) ? nppData._scintillaMainHandle : nppData._scintillaSecondHandle;

      auto cbText = GetCurrentEditTextLength();
      if (!cbText)
        return {};

      return ReadFromSkintilla(hwndSkillaOut, *cbText);
    }

    bool IsValidBplistBuffer()
    {
      auto totalTextSize = GetCurrentEditTextLength();
      if (!totalTextSize)
        return false;

      if (totalTextSize <= sizeof(defs::g_szHeader))
        return false; // buffer is too small

      HWND hSkilla;
      auto rawBuff = ReadFromSkintilla(hSkilla, sizeof(defs::g_szHeader));
      return std::equal(std::cbegin(defs::g_szHeader), std::cend(defs::g_szHeader), std::cbegin(rawBuff));
    }
  }


  BOOL InitPlugin() noexcept
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
  // We have to force calling destructor for global objects in DLL ( free manually on DLL_UNLOAD )
  //
  void FreePlugin() noexcept
  {
    g_pLoadedBplists.reset();
  }

  void InsertDataIntoSkilla( HWND hSkilla, const char* pData, size_t cbData ) noexcept
  {
    ::SendMessage( hSkilla, SCI_CLEARALL, NULL, NULL );
    ::SendMessage( hSkilla, SCI_ADDTEXT, cbData, (LPARAM)pData );
  }

  void MarkDocumentIsUnmodified( HWND hSkilla ) noexcept
  {
    ::SendMessage( hSkilla, SCI_SETSAVEPOINT, NULL, NULL );
  }

  ///////////////////////////////////////////////////////////////////////////////
  //
  // PLUGIN MESSAGES
  //
  //////////////////////////////////////////////////////////////////////////////

  void OnBufferActivated( SCNotification *notifyCode )
  {
    int which = -1;
    ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&which);
    if (which == -1)
      return;
    HWND hCurrentEditView = (which == 0) ? nppData._scintillaMainHandle : nppData._scintillaSecondHandle;

    int cbText = (int)::SendMessage(hCurrentEditView, SCI_GETLENGTH, NULL, NULL);

    auto loadedBplist = g_pLoadedBplists->find( notifyCode->nmhdr.idFrom );
    if ( loadedBplist == g_pLoadedBplists->end() )
    {
      // check if active windows has valid bplist buffer
      if (!IsValidBplistBuffer())
        return;

      // Get the current scintilla
      HWND hSkilla;
      CharVt rawBuff = ReadFromSkintilla( hSkilla );

      PlistEntryPtr plistEntry = MAKE_PLIST_PTR();

      // get converted text to display
      auto XMLVt = plistEntry->GetXML( std::move( rawBuff ) );
      InsertDataIntoSkilla( hSkilla, XMLVt.data(), XMLVt.size() );

      g_pLoadedBplists->insert( std::make_pair( notifyCode->nmhdr.idFrom, std::move( plistEntry ) ) );

      MarkDocumentIsUnmodified( hSkilla );
    }
  }

  //
  // Move bplist back to Notepad++ window in order to save buffer automatically.
  // If, after user's changes, we are not able to convert xml to bplist,
  // we should return original bplist buffer and notify user that conversion failed.
  //
  void OnFileBeforeSave( SCNotification *notifyCode )
  {
    auto loadedBplist = g_pLoadedBplists->find( notifyCode->nmhdr.idFrom );

    HWND hSkilla;

    try
    {
      if ( loadedBplist != g_pLoadedBplists->end() )
      {
        // Read all content -> convert it back to Bplist and save
        CharVt xmlPlistStr = ReadFromSkintilla( hSkilla );

        auto binPlist = loadedBplist->second->GetBinPlist( std::move(xmlPlistStr) );
        InsertDataIntoSkilla( hSkilla, binPlist.data(), binPlist.size() );
      }
    }
    catch ( std::runtime_error& )
    {
      // Update Notepad++ window with previous-saved (good & valid) bplist buffer
      assert( loadedBplist->second->GetContentType() != ContentType::xml );

      auto binPlist = loadedBplist->second->GetBinPlist();
      InsertDataIntoSkilla( hSkilla, binPlist.data(), binPlist.size() );

      // as long as we've restored xml to original state - lets mark it as "unchanged"
      MarkDocumentIsUnmodified( hSkilla );
      
      throw;
    }
  }

  //
  // Raw bplist data was saved, so now lets return friendly xml plist data to notepad++
  //
  void OnFileSaved( SCNotification *notifyCode )
  {
    auto loadedBplist = g_pLoadedBplists->find( notifyCode->nmhdr.idFrom );

    if ( loadedBplist != g_pLoadedBplists->end() &&
         loadedBplist->second->GetContentType() != ContentType::corrupted ) // check that file was saved properly
    {
      // check if active windows has valid bplist buffer
      if (!IsValidBplistBuffer())
        return;

      HWND hSkilla;
      CharVt rawBuff = ReadFromSkintilla( hSkilla );

      // get converted text to display
      auto XMLVt = loadedBplist->second->GetXML( std::move( rawBuff ) );
      InsertDataIntoSkilla( hSkilla, XMLVt.data(), XMLVt.size() );

      MarkDocumentIsUnmodified(hSkilla);
    }
  }

  void OnFileClosed( SCNotification *notifyCode )
  {
    g_pLoadedBplists->erase( notifyCode->nmhdr.idFrom );
  }

  bool IsCurrentFileIsABplistFile()
  {
    auto bufferId = ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTBUFFERID, 0, 0);
    auto loadedBplist = g_pLoadedBplists->find(bufferId);
    return !(loadedBplist == g_pLoadedBplists->end());
  }

} // namespace bplist