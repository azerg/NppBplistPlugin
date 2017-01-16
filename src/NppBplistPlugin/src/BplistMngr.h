#pragma once

#include "PluginDefinition.h"
#include "PlistMngr.h"
#include <Windows.h>

namespace bplist
{
  struct LoadedFileInfo
  {
    bool isBplist = false;
  };


  BOOL InitPlugin() _NOEXCEPT;
  void FreePlugin() _NOEXCEPT;
  CharVt ReadFromSkintilla( HWND& hwndSkillaOut );
  void InsertDataIntoSkilla( HWND hSkilla, const char* pData, size_t cbData ) _NOEXCEPT;
  void MarkDocumentIsUnmodified( HWND hSkilla ) _NOEXCEPT;

  void OnBufferActivated( SCNotification *notifyCode );
  void OnFileBeforeSave( SCNotification *notifyCode );
  void OnFileSaved( SCNotification *notifyCode );
  void OnFileClosed( SCNotification *notifyCode );
  LoadedFileInfo On

  //---------------------------------------------------------------------------------------------
  //
  // Few useful templates
  //
  template <typename T>
  bool IsValidBplist( T pFileBuff )
  {
    return ( pFileBuff.size() > sizeof( defs::g_szHeader ) ) &&
      std::equal( std::begin( defs::g_szHeader ), std::end( defs::g_szHeader ), pFileBuff.begin() );
  }
} // namespace bplist



