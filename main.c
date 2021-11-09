#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

#include <tchar.h>
#include <windows.h>
#include "commdlg.h"


#include <stdlib.h>
#include <stdio.h>
#include <crtdbg.h>
#include <string.h>


#include "global.h"
#include "resources/resource.h"

#include "BufferedRead.h"
#include "dataModel/dataModel.h"
#include "viewModel/viewModel.h"
#include "font/font.h"

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK HelpDialogProcedure(HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
TCHAR szClassName[] = _T("Reader");


int WINAPI WinMain (HINSTANCE hThisInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpszArgument,
                     int nCmdShow)
{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */
    HACCEL hAccel;

    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_WARN, _CRTDBG_MODE_STDOUT);
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ERROR, _CRTDBG_MODE_STDOUT);
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_MODE_STDOUT);


    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS | CS_OWNDC;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon(hThisInstance, MAKEINTRESOURCE(IDI_ICON));
    wincl.hIconSm = LoadIcon(hThisInstance, MAKEINTRESOURCE(IDI_ICON_16));
    wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
    wincl.lpszMenuName = szClassName;
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default color as the background of the window */
    wincl.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;
    debug("before minmax\n");
    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
           0,                   /* Extended possibilities for variation */
           szClassName,         /* Classname */
           szClassName,         /* Title Text */
           WS_OVERLAPPEDWINDOW, /* default window */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           544,                 /* The programs width */
           375,                 /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           lpszArgument         /* No Window Creation data */
    );
    /* Make the window visible on the screen */
    ShowWindow (hwnd, nCmdShow);

    hAccel = LoadAccelerators(hThisInstance, szClassName);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0)) {
        if (!TranslateAccelerator(hwnd, hAccel, &messages)) {
            /* Translate virtual-key messages into character messages */
            TranslateMessage(&messages);
            /* Send message to WindowProcedure */
            DispatchMessage(&messages);
        }
    }

    _CrtDumpMemoryLeaks();
    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}

/*  This function is called by the Windows function DispatchMessage()  */
LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    static dataModel dm;
    static viewModel vm;
    static font f;
    ulong scrollLines;
    static int deltaPerLine, accumDelta;
    static HINSTANCE hInstance;

    switch (message) /* handle the messages */
    {
        case WM_QUERYENDSESSION:
        case WM_CLOSE:
            if (IDYES == MessageBox(hwnd, _T("Do you want to quit?"),
                           szClassName, MB_YESNO | MB_ICONQUESTION))
                DestroyWindow(hwnd);
            return 0;
        case WM_DESTROY:
            font_Free(&f);
            vm_free(&vm);
            dm_free(&dm);
            PostQuitMessage(0); /* send a WM_QUIT to the message queue */
            return 0;
        case WM_CREATE:
            hInstance = ((CREATESTRUCT*)lParam)->hInstance;
            font_SetFont(hwnd, &f);
            readFile((char*)((CREATESTRUCT*)lParam)->lpCreateParams, &dm);
            vm_init(&vm);
        // fall through
        case WM_SETTINGCHANGE:
            SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &scrollLines, 0);
            deltaPerLine = (scrollLines != 0) * (WHEEL_DELTA / scrollLines);
            return 0;
        case WM_GETMINMAXINFO:
            debug("minmax\n");
            SCROLLINFO si;
            si.cbSize = sizeof si;
            si.fMask = SIF_RANGE | SIF_PAGE;

            RECT clientRect;
            clientRect.top = clientRect.left = 0;

            GetScrollInfo(hwnd, SB_VERT, &si);
            clientRect.right = f.chWidth * MIN_CHARS + ((int)si.nPage <= si.nMax - si.nMin) * GetSystemMetrics(SM_CXVSCROLL);
            GetScrollInfo(hwnd, SB_HORZ, &si);
            clientRect.bottom = f.chHeight * MIN_LINES + ((int)si.nPage <= si.nMax - si.nMin) * GetSystemMetrics(SM_CYVSCROLL);
            // to have not the window itself fit, but the client area
            AdjustWindowRect(&clientRect, WS_OVERLAPPEDWINDOW, TRUE);
            // the test has shown, that DefWindowProc implements similar logic
            // hence assigning "overhead" is overlooked

            // font is static, hence its fields are inited with 0, therefore no error
            ((LPMINMAXINFO)lParam)->ptMinTrackSize.x = clientRect.right - clientRect.left;
            ((LPMINMAXINFO)lParam)->ptMinTrackSize.y = clientRect.bottom - clientRect.top;
            return 0;
        case WM_SIZE:
            debug("size (h %ld, w %ld)\n", HIWORD(lParam), LOWORD(lParam));
            vm_resizeViewModel(hwnd, &vm, &dm, LOWORD(lParam) / f.chWidth, HIWORD(lParam) / f.chHeight);
            return 0;
        case WM_VSCROLL:
            si.cbSize = sizeof si;
            si.fMask = SIF_ALL;
            GetScrollInfo(hwnd, SB_VERT, &si);

            vm.vPos = si.nPos;

            switch (LOWORD(wParam)) {
                case SB_TOP:
                    si.nPos = si.nMin;
                    break;
                case SB_BOTTOM:
                    si.nPos = si.nMax;
                    break;
                case SB_LINEUP:
                    si.nPos -= 1;
                    break;
                case SB_LINEDOWN:
                    si.nPos += 1;
                    break;
                case SB_PAGEUP:
                    si.nPos -= si.nPage;
                    break;
                case SB_PAGEDOWN:
                    si.nPos += si.nPage;
                    break;
                case SB_THUMBPOSITION:
                    si.nPos = si.nTrackPos;
                    break;
                default:
                    break;
            }
            si.fMask = SIF_POS;
            SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
            GetScrollInfo(hwnd, SB_VERT, &si);

            if (si.nPos != vm.vPos) {
//                 unless vm_draw is smart enough to provide only
//                 the data for repaint, it's useless
//                ScrollWindow(hwnd, 0, f.chHeight * (vm.vPos - si.nPos), NULL, NULL);
                InvalidateRect(hwnd, NULL, TRUE);
//                UpdateWindow(hwnd);
            }
            return 0;
        case WM_HSCROLL:
            si.cbSize = sizeof si;
            si.fMask = SIF_ALL;
            GetScrollInfo(hwnd, SB_HORZ, &si);

            vm.hPos = si.nPos;

            switch (LOWORD(wParam)) {
                case SB_LINELEFT:
                    si.nPos -= 1;
                    break;
                case SB_LINERIGHT:
                    si.nPos += 1;
                    break;
                case SB_PAGELEFT:
                    si.nPos -= si.nPage;
                    break;
                case SB_PAGERIGHT:
                    si.nPos += si.nPage;
                    break;
                case SB_THUMBPOSITION:
                    si.nPos = si.nTrackPos;
                    break;
                default:
                    break;
            }
            si.fMask = SIF_POS;
            SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);
            GetScrollInfo(hwnd, SB_HORZ, &si);

            if (si.nPos != vm.hPos) {
//                ScrollWindow(hwnd, f.chWidth * (vm.hPos - si.nPos), 0, NULL, NULL);
//                vm.hPos = si.nPos;
                InvalidateRect(hwnd, NULL, TRUE);
//                UpdateWindow(hwnd);
            }
            return 0;
        case WM_MOUSEWHEEL:
            if (deltaPerLine == 0)
                return 0;
            accumDelta += (short) HIWORD (wParam);
            while(accumDelta >= deltaPerLine) {
                SendMessage(hwnd, WM_VSCROLL, SB_LINEUP, 0);
                accumDelta -= deltaPerLine;
            }
            while(accumDelta <= -deltaPerLine) {
                SendMessage(hwnd, WM_VSCROLL, SB_LINEDOWN, 0);
                accumDelta += deltaPerLine;
            }
            return 0;
        case WM_KEYDOWN:
            switch (wParam) {
                case VK_HOME:
                    SendMessage(hwnd, WM_VSCROLL, SB_TOP, 0);
                    break;
                case VK_END:
                    SendMessage(hwnd, WM_VSCROLL, SB_BOTTOM, 0);
                    break;
                case VK_PRIOR:
                    SendMessage(hwnd, WM_VSCROLL, SB_PAGEUP, 0);
                    break;
                case VK_NEXT:
                    SendMessage(hwnd, WM_VSCROLL, SB_PAGEDOWN, 0);
                    break;
                case VK_UP:
                    SendMessage(hwnd, WM_VSCROLL, SB_LINEUP, 0);
                    break;
                case VK_DOWN:
                    SendMessage(hwnd, WM_VSCROLL, SB_LINEDOWN, 0);
                    break;
                case VK_LEFT:
                    SendMessage(hwnd, WM_HSCROLL, SB_PAGELEFT, 0);
                    break;
                case VK_RIGHT:
                    SendMessage(hwnd, WM_HSCROLL, SB_PAGERIGHT, 0);
                    break;
            }
            return 0;
        case WM_PAINT:
            debug("paint\n");
            PAINTSTRUCT ps;
            hdc = BeginPaint(hwnd, &ps);
            // FIXME in case of not SUCCESS, checkRC will terminate the func flow,
            //  hence EndPaint won't be called
            // ^ the whole reason of leaving Paint here, not in vm_drawViewModel,
            //  was in maintaining the flow, but it's discarded here as well - bad
//            vm_drawViewModel(hdc, &vm, &dm, &f);
            si.cbSize = sizeof si;
            si.fMask = SIF_POS;
            GetScrollInfo(hwnd, SB_VERT, &si);
            vm.vPos = si.nPos;
            GetScrollInfo(hwnd, SB_HORZ, &si);
            vm.hPos = si.nPos;

            failClean(vm_drawViewModel(hdc, &vm, &dm, &f) != SUCCESS, EndPaint(hwnd, &ps), "error during drawing")
            EndPaint(hwnd, &ps);
            return 0;
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDM_FILE_OPEN:
                    OPENFILENAME ofn;       // common dialog box structure
                    char szFile[MAX_PATH];       // buffer for file name

                    // Initialize OPENFILENAME
                    ZeroMemory(&ofn, sizeof(ofn));
                    ofn.lStructSize = sizeof(ofn);
                    ofn.hwndOwner = hwnd;
                    ofn.lpstrFile = szFile;
                    // Set lpstrFile[0] to '\0' so that GetOpenFileName does not
                    // use the contents of szFile to initialize itself.
                    ofn.lpstrFile[0] = '\0';
                    ofn.nMaxFile = sizeof(szFile);
                    ofn.lpstrFilter = "*.txt\0";
                    ofn.nFilterIndex = 1;
                    ofn.lpstrFileTitle = NULL;
                    ofn.nMaxFileTitle = 0;
                    ofn.lpstrInitialDir = NULL;
                    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

                    if (GetOpenFileName(&ofn) == TRUE) {
                        readFile(ofn.lpstrFile, &dm);
                        vm_resizeViewModel(hwnd, &vm, &dm, vm.maxChars, vm.maxLines);
                    }
                    break;
                case IDM_ABOUT:
                    DialogBox(hInstance, _T("AboutBox"), hwnd, HelpDialogProcedure);
                    break;
                case IDM_APP_EXIT:
                    SendMessage(hwnd, WM_CLOSE, 0, 0);
                    break;
                case IDM_VIEW_WRAP:
                    CheckMenuItem(GetMenu(hwnd), IDM_VIEW_WRAP, MF_BYCOMMAND | (GetMenuState(GetMenu(hwnd), IDM_VIEW_WRAP, MF_BYCOMMAND) ^ MF_CHECKED));
                    vm_changeViewMode(&vm);
                    vm_resizeViewModel(hwnd, &vm, &dm, vm.maxChars, vm.maxLines);
                    break;
                default:
                    break;
            }
            return 0;
        default: /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }
    return 0;
}

BOOL CALLBACK HelpDialogProcedure(HWND hdlg, UINT message, WPARAM wParam, LPARAM lParam) {
    debug("DIAU'G\n");
    switch (message) {
        case WM_INITDIALOG:
            return TRUE;
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDOK:
                case IDCANCEL:
                    EndDialog(hdlg, 0);
                    return TRUE;
            }
            break;
    }
    return FALSE;
}
