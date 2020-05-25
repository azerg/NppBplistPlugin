#pragma once

#include "PluginDefinition.h"
#include "PlistMngr.h"
#include <Windows.h>

namespace bplist
{
  BOOL InitPlugin() noexcept;
  void FreePlugin() noexcept;
  void InsertDataIntoSkilla( HWND hSkilla, const char* pData, size_t cbData ) noexcept;
  void MarkDocumentIsUnmodified( HWND hSkilla ) noexcept;

  void OnBufferActivated( SCNotification *notifyCode );
  void OnFileBeforeSave( SCNotification *notifyCode );
  void OnFileSaved( SCNotification *notifyCode );
  void OnFileClosed( SCNotification *notifyCode );

  bool IsCurrentFileIsABplistFile();
} // namespace bplist



