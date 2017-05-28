/** 
    Beginning Game Programming, Third Edition 
    Chapter 2 
    GameLoop project 
**/  
  
#include <windows.h>  
#include <iostream> 
#include <tlhelp32.h> 
#include <time.h>  
#include <tchar.h>
#include <sstream>
using namespace std;  
  
const string APPTITLE = "TaskKiller";  
HWND window;  
HWND thishWnd;
HDC device;
int width=300,height=500,x=0,y=0;
bool gameover = false;  

DWORD GetMainThreadIdFromName(LPCSTR szName)
{
    DWORD idThread = 0;         // 进程ID
    DWORD idProcess = 0;        // 主线程ID

    // 获取进程ID
    PROCESSENTRY32 pe;      // 进程信息
    pe.dwSize = sizeof(PROCESSENTRY32);
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); // 获取系统进程列表
    if(Process32First(hSnapshot, &pe))      // 返回系统中第一个进程的信息
    {
        do
        {
            if(0 == _stricmp(pe.szExeFile, szName)) // 不区分大小写比较
            {
                idProcess = pe.th32ProcessID;
                break;
            }
        }while(Process32Next(hSnapshot, &pe));      // 下一个进程
    }
    CloseHandle(hSnapshot); // 删除快照
    if (idProcess == 0)
    {
        return 0;
    }

    // 获取进程的主线程ID
    THREADENTRY32 te;       // 线程信息
    te.dwSize = sizeof(THREADENTRY32);
    hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0); // 系统所有线程快照
    if(Thread32First(hSnapshot, &te))       // 第一个线程
    {
        do
        {
            if(idProcess == te.th32OwnerProcessID)      // 认为找到的第一个该进程的线程为主线程
            {
                idThread = te.th32ThreadID;
                break;
            }
        }while(Thread32Next(hSnapshot, &te));           // 下一个线程
    }
    CloseHandle(hSnapshot); // 删除快照
    return idThread;
}


void TaskKiller()
{
    DWORD id = GetMainThreadIdFromName("StudentMain.exe");
    if (id > 0)
    {
        bool comfirm=MessageBox(thishWnd,"Are you sure that you want to kill the process?","Comfirm",MB_YESNO|MB_ICONEXCLAMATION);
        if(comfirm==IDYES)
        {
            PostThreadMessage(id, WM_QUIT, 0, 0);
            MessageBox(NULL,"Kill Completed","Result",MB_OK|MB_ICONINFORMATION);
        }
        else
            MessageBox(NULL,"Kill Cancelled","Result",MB_OK|MB_ICONINFORMATION);
    }
    else
    {
        MessageBox(NULL,"Kill Failed","Warning",MB_OK|MB_ICONERROR);
    }
}

void int2str(const int &int_temp,string &string_temp)  
{  
        stringstream stream;  
        stream<<int_temp;  
        string_temp=stream.str();   //此处也可以用 stream>>string_temp  
}

void TextManager()
{
    string str="已注册！";
    HWND hWndParent=FindWindow(NULL,"软件注册");  
    HWND hWndText=FindWindowEx(hWndParent,NULL,NULL,"Text1");
    SendMessage(hWndText, WM_SETTEXT, NULL, LPARAM(str.c_str()));
    HWND hWndCommand=FindWindowEx(hWndParent,NULL,NULL,"Command1");
    SendMessage(hWndCommand, BM_CLICK, NULL, NULL);
    char Text[256];
    SendMessage(hWndText,WM_GETTEXT, sizeof(Text)/sizeof(char), (LPARAM)Text);
    MessageBox(hWndParent," "," ",MB_OK);
    for(int i=1;i<=0x7fffffff;i++)
    {
        int2str(i,str);
        SendMessage(hWndText, WM_SETTEXT, NULL, LPARAM(str.c_str()));
        SendMessage(hWndCommand, BM_CLICK, NULL, NULL);
    }
}

void DrawPixel(HWND &hWnd,int x,int y)
{
    RECT rt;  
    GetClientRect(hWnd, &rt); 
    PAINTSTRUCT ps;  
    HDC hdc = BeginPaint(hWnd, &ps); 
    //DrawText(hdc, text.c_str(), text.length(), &rt, DT_CENTER); 
    COLORREF c = RGB(rand()%256, rand()%256, rand()%256);  
    SetPixel(hdc, x, y, c);
    EndPaint(hWnd, &ps);
}

/** 
 ** Loads and draws a bitmap from a file and then frees the memory 
 ** 必须承认的是这个函数很慢，这个函数适合于装载完整的背景图片 
 ** 或者只需将图片绘制一次的小位图。这个函数将位图文件装入内存 
 **/  
void DrawBitmap(char *filename, int x, int y)                                         
{                                                                                     
    
    //load the bitmap image  
    HBITMAP image = (HBITMAP)LoadImage(0,filename,IMAGE_BITMAP,300,500,LR_LOADFROMFILE);  
  
    //read the bitmap's properties  
    BITMAP bm;  
    GetObject(image, sizeof(BITMAP), &bm);  
  
    //create a device context for the bitmap  
    HDC hdcImage = CreateCompatibleDC(device);  
    SelectObject(hdcImage, image);  
  
    //draw the bitmap to the window (bit block transfer)  
    BitBlt(   
        device,                  //destination device context  
        x, y,                    //x,y location on destination  
        bm.bmWidth, bm.bmHeight, //width,height of source bitmap  
        hdcImage,                  //source bitmap device context  
        0, 0,                    //start x,y on source bitmap  
        SRCCOPY);                //blit method  
  
    //delete the device context and bitmap  
    DeleteDC(hdcImage);  
    DeleteObject((HBITMAP)image);  
}  
  
/** 
 ** Startup and loading code goes here 
 **/  
bool Game_Init()  
{  
    //start up the random number generator  
    srand(time(NULL));
    return 1;  
}  
  
/** 
 ** Update function called from inside game loop 
 **/  

void Game_Run()  
{  
    if (gameover == true) return;  
  
    //get the drawing surface  
    RECT rect;  
    GetClientRect(window, &rect);  
  
    //draw bitmap at random location    
    DrawBitmap("c.bmp", x, y); 
}  
  
/** 
 ** Shutdown code 
 **/  
void Game_End()  
{  
    //free the device  
    ReleaseDC(window, device);  
}  
  
/** 
 ** Window callback function 
 **/  
LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)  
{  
    thishWnd=hWnd;
    switch (message)   
    {
        case WM_DESTROY:
            gameover = true;  
            PostQuitMessage(0);
            break;
        case WM_PAINT:
            DrawBitmap("c.bmp",x,y);
            break;
        case WM_KEYDOWN:
            switch(wParam)
            {
                case VK_LEFT:
                    x-=10;
                    break;
                case VK_RIGHT:
                    x+=10;
                    break;
                case VK_UP:
                    TaskKiller();
                    y-=10;
                    break;
                case VK_DOWN:
                    TextManager();
                    y+=10;
                    break;
                case VK_LBUTTON:
                    //DrawPixel(hWnd,LOWORD(lParam),HIWORD(lParam));
                    //TaskKiller();
                    //string text=LOWORD(lParam)+"+"+HIWORD(lParam);
                    break;
            }
            SendMessage(hWnd,WM_PAINT,0,0);
            break;
        default:
            //DefWindowProc(hWnd, message, wParam, lParam);
            break;
    }
    DefWindowProc(hWnd, message, wParam, lParam);
}  
  
/** 
 ** MyRegiserClass function sets program window properties 
 **/  
ATOM MyRegisterClass(HINSTANCE hInstance)  
{  
    //create the window class structure  
    WNDCLASSEX wc;  
    wc.cbSize = sizeof(WNDCLASSEX);   
  
    //fill the struct with info  
    wc.style         = CS_HREDRAW | CS_VREDRAW;  
    wc.lpfnWndProc   = (WNDPROC)WinProc;  
    wc.cbClsExtra     = 0;  
    wc.cbWndExtra     = 0;  
    wc.hInstance     = hInstance;  
    wc.hIcon         = LoadIcon(NULL,IDI_APPLICATION);  
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);  
    wc.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);  
    wc.lpszMenuName  = NULL;  
    wc.lpszClassName = APPTITLE.c_str();  
    wc.hIconSm       = NULL;  
  
    //set up the window with the class info  
    return RegisterClassEx(&wc);  
}  
  
/** 
 ** Helper function to create the window and refresh it 
 **/  
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)  
{  
    //create a new window  
    window = CreateWindow(  
       APPTITLE.c_str(),              //window class  
       APPTITLE.c_str(),              //title bar  
       WS_OVERLAPPEDWINDOW,   //window style  
       CW_USEDEFAULT,         //x position of window  
       CW_USEDEFAULT,         //y position of window  
       315,                   //width of the window  
       530,                   //height of the window  
       NULL,                  //parent window  
       NULL,                  //menu  
       hInstance,             //application instance  
       NULL);                 //window parameters  
  
    //was there an error creating the window?  
    if (window == 0) return 0;  
  
    //display the window  
    ShowWindow(window, 0/*nCmdShow*/);  
    UpdateWindow(window);  
  
    //get device context for drawing  
    device = GetDC(window);  
    return 1;  
}  
  
/** 
 ** Entry point function 
 **/  
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)  
{  
    MSG msg;  
  
    //create window  
    MyRegisterClass(hInstance);  
    if (!InitInstance (hInstance, nCmdShow)) return 0;  
  
    //initialize the game  
    if (!Game_Init()) return 0;  
  
    // main message loop  
    Game_Run();
    while (!gameover)  
    {
        //process Windows events  
        SetWindowPos(thishWnd,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))   
        {  
            TranslateMessage(&msg);  
            DispatchMessage(&msg);  
        }  
          
        //process game loop  
          
    }  
  
    //free game resources  
    Game_End();  
  
    return msg.wParam;  
}
