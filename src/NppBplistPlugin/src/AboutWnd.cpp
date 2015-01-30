#include "AboutWnd.h"
#include "Resource.h"
#include "defines.h"

void LaunchAboutWnd()
{
  HINSTANCE hInst = GetModuleHandle(TEXT("NppBplistPlugin.dll"));
  DialogBox(hInst, MAKEINTRESOURCE(IDD_DLG_ABOUT), NULL, (DLGPROC)&AboutWndProc);
}

LRESULT CALLBACK AboutWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message)
  {
  case WM_CLOSE:
  case WM_DESTROY:
    PostQuitMessage(0);
    break;
  default:
    return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return 0;
}