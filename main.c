#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

#include <windows.h>
#include <tchar.h>
#include <crtdbg.h>

#include "global.h"
#include "resources/resource.h"
#include "docker/docker.h"

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK AboutDialogProcedure(HWND, UINT, WPARAM, LPARAM);

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
    HACCEL hAccel; // menu shortcuts
    docker* doc;

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
    wincl.cbWndExtra = sizeof doc; // space for the pointer to the docker
    /* Use Windows's default color as the background of the window */
    wincl.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

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

void onScroll(HWND hwnd, docker* doc, int barType, ushort scrollRequest) {
    doc->si.fMask = SIF_ALL;
    GetScrollInfo(hwnd, barType, &doc->si);
    int pos = doc->si.nPos;
    switch(scrollRequest) {
        case SB_LINEUP:     doc->si.nPos -= 1;                break;
        case SB_LINEDOWN:   doc->si.nPos += 1;                break;
        case SB_PAGEUP:     doc->si.nPos -= doc->si.nPage;    break;
        case SB_PAGEDOWN:   doc->si.nPos += doc->si.nPage;    break;
        default:
            if (barType == SB_VERT) {
                switch (scrollRequest) {
                    case SB_TOP:        doc->si.nPos = doc->si.nMin;      break;
                    case SB_BOTTOM:     doc->si.nPos = doc->si.nMax;      break;
                    case SB_THUMBTRACK: doc->si.nPos = doc->si.nTrackPos; break;
                    default:
                        break;
                }
            }
            if (barType == SB_HORZ && scrollRequest == SB_THUMBPOSITION)
                doc->si.nPos = doc->si.nTrackPos;
            break;
    }

    doc->si.fMask = SIF_POS;
    SetScrollInfo(hwnd, barType, &doc->si, TRUE);
    GetScrollInfo(hwnd, barType, &doc->si);

    if ((ARRAY_SIZETYPE)doc->si.nPos != pos) {
        // unless vm_draw is smart enough to provide only
        // the data for repaint, it's useless
    //                ScrollWindow(hwnd, 0, f.chHeight * (vm.vPos - si.nPos), NULL, NULL);
        InvalidateRect(hwnd, NULL, TRUE);
    }
}


/*  This function is called by the Windows function DispatchMessage()  */
LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    docker* doc = (docker*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    if (!doc) {
        switch (message) {
            case WM_CREATE:
                failClean(docker_init(hwnd, &doc) != SUCCESS, docker_free(&doc), "failed to init Docker")
                SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)doc);

                SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &doc->scrollLines, 0);
                doc->deltaPerLine = (doc->scrollLines != 0) * (WHEEL_DELTA / doc->scrollLines);

                doc->hInstance = ((CREATESTRUCT*)lParam)->hInstance;

                SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW | SWP_NOREDRAW);
                return 0;
            default:
                return DefWindowProc (hwnd, message, wParam, lParam);
            }
    }
    switch (message) /* handle the messages */
    {
        case WM_QUERYENDSESSION:
        case WM_CLOSE:
            if (IDYES == MessageBox(hwnd, _T("Do you want to quit?"),
                           szClassName, MB_YESNO | MB_ICONQUESTION))
                DestroyWindow(hwnd);
            return 0;
        case WM_DESTROY:
            docker_free(&doc);
            PostQuitMessage(0); /* send a WM_QUIT to the message queue */
            return 0;
        case WM_GETMINMAXINFO:
            doc->si.fMask = SIF_RANGE | SIF_PAGE;

            RECT clientRect;
            clientRect.top = clientRect.left = 0;

            GetScrollInfo(hwnd, SB_VERT, &doc->si);
            clientRect.right = docker_getMinWidth(doc) + ((int)doc->si.nPage <= doc->si.nMax - doc->si.nMin) * GetSystemMetrics(SM_CXVSCROLL);
            GetScrollInfo(hwnd, SB_HORZ, &doc->si);
            clientRect.bottom = docker_getMinHeight(doc) + ((int)doc->si.nPage <= doc->si.nMax - doc->si.nMin) * GetSystemMetrics(SM_CYVSCROLL);
            // to have not the window itself fit, but the client area
            AdjustWindowRect(&clientRect, WS_OVERLAPPEDWINDOW, TRUE);
            // the test has shown, that DefWindowProc implements similar logic
            // hence assigning "overhead" is overlooked
            ((LPMINMAXINFO)lParam)->ptMinTrackSize.x = clientRect.right - clientRect.left;
            ((LPMINMAXINFO)lParam)->ptMinTrackSize.y = clientRect.bottom - clientRect.top;
            return 0;
        case WM_SIZE:
            if (docker_isSailed(doc))
                checkRC(vm_resizeViewModel(hwnd, doc->vm, doc->dm, doc->f, LOWORD(lParam) / font_getWidth(doc->f), HIWORD(lParam) / font_getHeight(doc->f)))
            return 0;
        case WM_VSCROLL:
            onScroll(hwnd, doc, SB_VERT, LOWORD(wParam));
            return 0;
        case WM_HSCROLL:
            onScroll(hwnd, doc, SB_HORZ, LOWORD(wParam));
            return 0;
        case WM_MOUSEWHEEL:
            if (doc->deltaPerLine == 0)
                return 0;
            doc->accumDelta += (short) HIWORD (wParam);
            while(doc->accumDelta >= doc->deltaPerLine) {
                SendMessage(hwnd, WM_VSCROLL, SB_LINEUP, 0);
                doc->accumDelta -= doc->deltaPerLine;
            }
            while(doc->accumDelta <= -doc->deltaPerLine) {
                SendMessage(hwnd, WM_VSCROLL, SB_LINEDOWN, 0);
                doc->accumDelta += doc->deltaPerLine;
            }
            return 0;
        case WM_KEYDOWN:
            switch (wParam) {
                case VK_HOME:  SendMessage(hwnd, WM_VSCROLL, SB_TOP, 0);       break;
                case VK_END:   SendMessage(hwnd, WM_VSCROLL, SB_BOTTOM, 0);    break;
                case VK_PRIOR: SendMessage(hwnd, WM_VSCROLL, SB_PAGEUP, 0);    break;
                case VK_NEXT:  SendMessage(hwnd, WM_VSCROLL, SB_PAGEDOWN, 0);  break;
                case VK_UP:    SendMessage(hwnd, WM_VSCROLL, SB_LINEUP, 0);    break;
                case VK_DOWN:  SendMessage(hwnd, WM_VSCROLL, SB_LINEDOWN, 0);  break;
                case VK_LEFT:  SendMessage(hwnd, WM_HSCROLL, SB_PAGELEFT, 0);  break;
                case VK_RIGHT: SendMessage(hwnd, WM_HSCROLL, SB_PAGERIGHT, 0); break;
            }
            return 0;
        case WM_PAINT:
            doc->si.fMask = SIF_POS;
            GetScrollInfo(hwnd, SB_VERT, &doc->si);
            doc->vm->vPos = doc->si.nPos;
            GetScrollInfo(hwnd, SB_HORZ, &doc->si);
            doc->vm->hPos = doc->si.nPos;

            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            failClean(vm_drawViewModel(hdc, doc->vm, doc->dm, doc->f) != SUCCESS, EndPaint(hwnd, &ps), "error during drawing")
            EndPaint(hwnd, &ps);
            return 0;
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDM_FILE_OPEN:
                    OPENFILENAME ofn;
                    char szFile[MAX_PATH] = {0};

                    ZeroMemory(&ofn, sizeof ofn);
                    ofn.lStructSize = sizeof ofn;
                    ofn.hwndOwner = hwnd;
                    ofn.lpstrFilter = "*.txt\0\0";
                    ofn.nFilterIndex = 1;
                    ofn.nMaxFile = sizeof szFile;
                    ofn.lpstrFile = szFile;
                    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

                    if (GetOpenFileName(&ofn) == TRUE) {
                        checkRC(docker_setSail(doc, ofn.lpstrFile))
                        RECT clientRect;
                        GetClientRect(hwnd, &clientRect);
                        checkRC(vm_buildViewModel(hwnd, doc->vm, doc->dm,
                                                   (clientRect.right - clientRect.left) / font_getWidth(doc->f),
                                                   (clientRect.bottom - clientRect.top) / font_getHeight(doc->f)))
                        EnableMenuItem(GetMenu(hwnd), IDM_VIEW_WRAP, MF_BYCOMMAND | MF_ENABLED);
                    }
                    break;
                case IDM_ABOUT:
                    DialogBox(doc->hInstance, _T("AboutBox"), hwnd, AboutDialogProcedure);
                    break;
                case IDM_APP_EXIT:
                    SendMessage(hwnd, WM_CLOSE, 0, 0);
                    break;
                case IDM_VIEW_WRAP:
                    // the menu is grayed until the docker is sailed
                    CheckMenuItem(GetMenu(hwnd), IDM_VIEW_WRAP, (GetMenuState(GetMenu(hwnd), IDM_VIEW_WRAP, MF_BYCOMMAND) ^ MF_CHECKED));
                    vm_changeViewMode(doc->vm);
                    checkRC(vm_buildViewModel(hwnd, doc->vm, doc->dm, doc->vm->maxChars, doc->vm->maxLines))
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

BOOL CALLBACK AboutDialogProcedure(HWND hdlg, UINT message, WPARAM wParam, LPARAM lParam) {
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
