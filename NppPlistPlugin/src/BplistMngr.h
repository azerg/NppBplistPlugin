#pragma once

#include "PluginDefinition.h"
#include "PlistMngr.h"
#include <Windows.h>

namespace plist
{

  BOOL InitPlugin();
  void FreePlugin();
  CharVt ReadFromSkintilla( HWND& hwndSkillaOut );
  void InsertDataIntoSkilla( HWND hSkilla, const char* pData, size_t cbData );
  void MarkDocumentIsUnmodified( HWND hSkilla );

  void OnBufferActivated( SCNotification *notifyCode );
  void OnFileBeforeSave( SCNotification *notifyCode );
  void OnFileSaved( SCNotification *notifyCode );
  void OnFileClosed( SCNotification *notifyCode );

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
} // namespace plist



