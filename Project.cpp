#include <iostream>

#include "framework.h"
#include "Project.h"			// to be changed
#include <math.h>
#include <cmath>
#include <vector>
#include <Windows.h>
#include <stack>
#include <cstdlib>
#include <iostream>
#include <tchar.h>
#include <list>
//#include <bits/stdc++.h>

#define MAX_LOADSTRING 100
#define MAXENTRIES 600

using namespace std;



// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

//// flood fill
struct Vertex
{
    int x, y;
    Vertex(int x, int y) :x(x), y(y)
    {
    }
};


void FloodFill(HDC hdc, int x, int y, COLORREF Cb, COLORREF Cf)
{
    COLORREF C = GetPixel(hdc, x, y);
    if (C == Cb || C == Cf)return;
    SetPixel(hdc, x, y, Cf);
    FloodFill(hdc, x + 1, y, Cb, Cf);
    FloodFill(hdc, x - 1, y, Cb, Cf);
    FloodFill(hdc, x, y + 1, Cb, Cf);
    FloodFill(hdc, x, y - 1, Cb, Cf);
}

void NRFloodFill(HDC hdc, int x, int y, COLORREF Cb, COLORREF Cf)
{
    stack<Vertex> S;
    S.push(Vertex(x, y));
    while (!S.empty())
    {
        Vertex v = S.top();
        S.pop();
        COLORREF c = GetPixel(hdc, v.x, v.y);
        if (c == Cb || c == Cf)continue;
        SetPixel(hdc, v.x, v.y, Cf);
        S.push(Vertex(v.x + 1, v.y));
        S.push(Vertex(v.x - 1, v.y));
        S.push(Vertex(v.x, v.y + 1));
        S.push(Vertex(v.x, v.y - 1));
    }
}





//the convex filling
struct Entry
{
    int xmin, xmax;
};

void InitEntries(Entry table[])
{
    for (int i = 0; i < MAXENTRIES; i++)
    {
        table[i].xmin = INT_MAX;
        table[i].xmax = -INT_MAX;
    }
}

void ScanEdge(POINT v1, POINT v2, Entry table[])
{
    if (v1.y == v2.y)return;
    if (v1.y > v2.y)swap(v1, v2);
    double minv = (double)(v2.x - v1.x) / (v2.y - v1.y);
    double x = v1.x;
    int y = v1.y;
    while (y < v2.y)
    {
        if (x < table[y].xmin)table[y].xmin = (int)ceil(x);
        if (x > table[y].xmax)table[y].xmax = (int)floor(x);
        y++;
        x += minv;
    }
}

void DrawSanLines(HDC hdc, Entry table[], COLORREF color)
{
    for (int y = 0; y < MAXENTRIES; y++)
        if (table[y].xmin < table[y].xmax)
            for (int x = table[y].xmin; x <= table[y].xmax; x++)
                SetPixel(hdc, x, y, color);
}

void ConvexFill(HDC hdc, POINT p[], int n, COLORREF color)
{
    Entry* table = new Entry[MAXENTRIES];
    InitEntries(table);
    POINT v1 = p[n - 1];
    for (int i = 0; i < n; i++)
    {
        POINT v2 = p[i];
        ScanEdge(v1, v2, table);
        v1 = p[i];
    }
    DrawSanLines(hdc, table, color);
    delete table;
}

//the non convex filling
struct EdgeRec
{
    double x;
    double minv;
    int ymax;
    bool operator<(EdgeRec r)
    {
        return x < r.x;
    }
};

typedef list<EdgeRec> EdgeList;

EdgeRec InitEdgeRec(POINT& v1, POINT& v2)
{
    if (v1.y > v2.y)swap(v1, v2);
    EdgeRec rec;
    rec.x = v1.x;
    rec.ymax = v2.y;
    rec.minv = (double)(v2.x - v1.x) / (v2.y - v1.y);
    return rec;
}

void InitEdgeTable(POINT* polygon, int n, EdgeList table[])
{
    POINT v1 = polygon[n - 1];
    for (int i = 0; i < n; i++)
    {
        POINT v2 = polygon[i];
        if (v1.y == v2.y)
        {
            v1 = v2;
            continue;
        }
        EdgeRec rec = InitEdgeRec(v1, v2);
        table[v1.y].push_back(rec);
        v1 = polygon[i];
    }
}

void non_convexFill(HDC hdc, POINT* polygon, int n, COLORREF c)
{
    EdgeList* table = new EdgeList[MAXENTRIES];
    InitEdgeTable(polygon, n, table);
    int y = 0;
    while (y < MAXENTRIES && table[y].size() == 0)y++;
    if (y == MAXENTRIES)return;
    EdgeList ActiveList = table[y];
    while (ActiveList.size() > 0)
    {
        ActiveList.sort();
        for (EdgeList::iterator it = ActiveList.begin(); it != ActiveList.end(); it++)
        {
            int x1 = (int)ceil(it->x);
            it++;
            int x2 = (int)floor(it->x);
            for (int x = x1; x <= x2; x++)SetPixel(hdc, x, y, c);
        }
        y++;
        EdgeList::iterator it = ActiveList.begin();
        while (it != ActiveList.end())
            if (y == it->ymax) it = ActiveList.erase(it);
            else it++;
        for (EdgeList::iterator it = ActiveList.begin(); it != ActiveList.end(); it++)
            it->x += it->minv;
        ActiveList.insert(ActiveList.end(), table[y].begin(), table[y].end());
    }
    delete[] table;
}


// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_PROJECT, szWindowClass, MAX_LOADSTRING);		// to be changed
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PROJECT));

    // Main message loop:
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDC_PROJECT));		// to be changed
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_PROJECT);					// to be changed
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    
    // Change the cursor shape
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    // Change the background colour
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    return RegisterClassExW(&wcex);
}


BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Store instance handle in our global variable

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}



// default colour is black
static COLORREF colour = RGB(0, 0, 0);
static COLORREF fill_colour = RGB(255, 255, 0);

static string parentChoice = "", choice = "", addi = "";
static int drawing_count = 0, lm, st, index = 0, point = 0;
static int D, S, A, B, A1, B1, R;
static bool flag = true;

// fill circle with lines qaurter argument
static int quarter = 1;

// for convex and non convex
POINT points[5];

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc = GetDC(hWnd);
    static Vector p[6];
    static Vector p2[4];
    static Vector hermit[4];
    static Vector clicks[100];
    static int dis = 0, V1 = 0, V2 = 0;
    
    switch (message)
    {
    
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {

            // Colouring
        case ID_COLOUR_RED:
            colour = RGB(255, 0, 0);
            break;

        case ID_COLOUR_BLUE:
            colour = RGB(0, 0, 255);
            break;

        case ID_COLOUR_GREEN:
            colour = RGB(0, 255, 0);
            break;

        case ID_COLOUR_BLACK:
            colour = RGB(0, 0, 0);
            break;

        case ID_COLOUR_PURPLE:
            colour = RGB(159, 43, 104);
            break;

            // Filling Colour
        case ID_FILLINGCOLOUR_RED:
            fill_colour = RGB(255, 0, 0);
            break;

        case ID_FILLINGCOLOUR_BLUE:
            fill_colour = RGB(0, 0, 255);
            break;

        case ID_FILLINGCOLOUR_GREEN:
            fill_colour = RGB(0, 255, 0);
            break;

        case ID_FILLINGCOLOUR_BLACK:
            fill_colour = RGB(0, 0, 0);
            break;

        case ID_FILLINGCOLOUR_PURPLE:
            fill_colour = RGB(159, 43, 104);
            break;
        case ID_FILLINGCOLOUR_YELLOW:
            fill_colour = RGB(255, 255, 0);
            break;
            // Line Drawing cases
        case ID_LINE_DDA:
            parentChoice = "LINE";
            choice = "ID_LINE_DDA";
            break;

        case ID_LINE_MIDPOINT:
            parentChoice = "LINE";
            choice = "ID_LINE_MIDPOINT";
            break;

        case ID_LINE_PARAMETRIC:
            parentChoice = "LINE";
            choice = "ID_LINE_PARAMETRIC";
            break;

            // ---|> Circle drawing cases
        case ID_CIRCLE_DIRECT:
            parentChoice = "CIRCLE";
            choice = "ID_CIRCLE_DIRECT";
            break;

        case ID_CIRCLE_POLAR:
            parentChoice = "CIRCLE";
            choice = "ID_CIRCLE_POLAR";
            break;

        case ID_CIRCLE_ITERATIVEPOLAR:
            parentChoice = "CIRCLE";
            choice = "ID_CIRCLE_ITERATIVEPOLAR";
            break;

        case ID_CIRCLE_MIDPOINT:
            parentChoice = "CIRCLE";
            choice = "ID_CIRCLE_MIDPOINT";
            break;

        case ID_CIRCLE_MODIFIEDMIDPOINT:
            parentChoice = "CIRCLE";
            choice = "ID_CIRCLE_MODIFIEDMIDPOINT";
            break;

            // ---|> Ellipse Drawing
        case ID_ELLIPSE_DIRECT:
            parentChoice = "ELLIPSE";
            choice = "ID_ELLIPSE_DIRECT";
            break;

        case ID_ELLIPSE_POLAR:
            parentChoice = "ELLIPSE";
            choice = "ID_ELLIPSE_POLAR";
            break;

        case ID_ELLIPSE_MIDPOINT:
            parentChoice = "ELLIPSE";
            choice = "ID_ELLIPSE_MIDPOINT";
            break;

        case ID_CARDINALSPLINECURVE:
            parentChoice = "CARDI";
            break;

            // ---|> Filling cases
            
            //fill with lines quarter cases
        case ID_LINES_Q1:
            parentChoice = "FILLING";
            choice = "ID_CIRCLEWITH_LINES";
            quarter = 1;
            break;
        
        case ID_LINES_Q2:
            parentChoice = "FILLING";
            choice = "ID_CIRCLEWITH_LINES";
            quarter = 2;
            break;
        
        case ID_LINES_Q3:
            parentChoice = "FILLING";
            choice = "ID_CIRCLEWITH_LINES";
            quarter = 3;
            break;
        
        case ID_LINES_Q4:
            parentChoice = "FILLING";
            choice = "ID_CIRCLEWITH_LINES";
            quarter = 4;
            break;

        case ID_CIRCLEWITH_CIRCLES:
            parentChoice = "FILLING";
            choice = "ID_CIRCLEWITH_CIRCLES";
            break;

        case ID_SQUAREWITH_HERMITCURVE:
            parentChoice = "FILLING";
            choice = "ID_SQUAREWITH_HERMITCURVE";
            break;

        case ID_RECTANGLEWITH_BEZIERCURVEHORIZONTAL:
            parentChoice = "FILLING";
            choice = "ID_RECTANGLEWITH_BEZIERCURVEHORIZONTAL";
            break;

        case ID_FILLING_CONVEX:
            parentChoice = "CONVEX";
            choice = "ID_FILLING_CONVEX";
            break;

        case ID_FILLING_NONCONVEX:
            parentChoice = "CONVEX";
            choice = "ID_FILLING_NONCONVEX";
            break;

        case ID_FILLING_RECURSIVE:
            parentChoice = "FILLING";
            choice = "ID_FILLING_RECURSIVE";
            break;

        case ID_FILLING_NONRECURSIVE:
            parentChoice = "FILLING";
            choice = "ID_FILLING_NONRECURSIVE";
            break;

            // ---|> clipping cases
        case ID_CIRCLECLIPPINGWINDOW_POINT:
            parentChoice = "CLIPPING";
            choice = "ID_CIRCLECLIPPINGWINDOW_POINT";
            break;

        case ID_CIRCLECLIPPINGWINDOW_LINE:
            parentChoice = "CLIPPING";
            choice = "ID_CIRCLECLIPPINGWINDOW_LINE";
            break;

        case ID_SQUARECLIPPINGWINDOW_POINT:
            parentChoice = "CLIPPING";
            choice = "ID_SQUARECLIPPINGWINDOW_POINT";
            break;

        case ID_SQUARECLIPPINGWINDOW_LINE:
            parentChoice = "CLIPPING";
            choice = "ID_SQUARECLIPPINGWINDOW_LINE";
            break;
 
        case ID_RECTANGLECLIPPINGWINDOW_POINT:
            parentChoice = "CLIPPING";
            choice = "ID_RECTANGLECLIPPINGWINDOW_POINT";
            break;

        case ID_RECTANGLECLIPPINGWINDOW_LINE:
            parentChoice = "CLIPPING";
            choice = "ID_RECTANGLECLIPPINGWINDOW_LINE";
            break;
        
        case ID_RECTANGLECLIPPINGWINDOW_POLYGON:
            parentChoice = "CLIPPING";
            choice = "ID_RECTANGLECLIPPINGWINDOW_POLYGON";
            break;
 

// ---|> Clear Case
        case IDM_CLEAR:
            InvalidateRect(hWnd, nullptr, true);
            drawing_count = 0;
            index = 0;
            point = 0;
            break;

// ---|> Save Case
        case ID_SAVE:
            break;

        case ID_ADDITIONAL_WHITE:
            addi = "white";
            break;

        case ID_MOUSE_SHAPE:
            addi = "mouse";
            break;

            // ---|> About Explaining
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            cout << wmId;
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    
    break;

    case WM_LBUTTONDOWN:
        if (parentChoice == "LINE") {
            p[drawing_count] = Vector(LOWORD(lParam), HIWORD(lParam));
            drawing_count++;

            if (drawing_count == 2) {
                double centre_x = p[0][0], centre_y = p[0][1]
                    , x = p[1][0], y = p[1][1];

                if (choice == "ID_LINE_DDA")
                    SimpleDDA(hdc, centre_x, centre_y, x, y, colour);
                
                else if (choice == "ID_LINE_MIDPOINT") 
                    DrawLineMidPoint(hdc, centre_x, centre_y, x, y, colour);
                
                else if (choice == "ID_LINE_PARAMETRIC")
                    parametricline(hdc, centre_x, centre_y, x, y, colour);
                
                drawing_count = 0;
                ReleaseDC(hWnd, hdc);
            }
        }
        
        else if (parentChoice == "CIRCLE") {
            
            p[drawing_count] = Vector(LOWORD(lParam), HIWORD(lParam));
            drawing_count++;

            if (drawing_count == 2) {
                double centre_x = p[0][0], centre_y = p[0][1]
                    , x = p[1][0], y = p[1][1];
                
                double r = sqrt((x - centre_x) * (x - centre_x) 
                    + (y - centre_y) * (y - centre_y));
                
                if (choice == "ID_CIRCLE_DIRECT")
                    CircleDirect(hdc, centre_x, centre_y, r, colour);
                
                else if (choice == "ID_CIRCLE_POLAR")
                    CirclePolar(hdc, centre_x, centre_y, r, colour);
                
                else if(choice == "ID_CIRCLE_ITERATIVEPOLAR")
                    CircleIterativePolar(hdc, centre_x, centre_y, r, colour);
                
                else if (choice == "ID_CIRCLE_MIDPOINT")
                    CircleBresenham(hdc, centre_x, centre_y, r, colour);
                
                else if (choice == "ID_CIRCLE_MODIFIEDMIDPOINT")
                    CircleFasterBresenham(hdc, centre_x, centre_y, r, colour);
                
                drawing_count = 0;
                ReleaseDC(hWnd, hdc);
            }
        }
        
        else if (parentChoice == "ELLIPSE") {
            p[drawing_count] = Vector(LOWORD(lParam), HIWORD(lParam));
            drawing_count++;

            if (drawing_count == 3) {
                double centre_x = p[0][0], centre_y = p[0][1],
                    x = p[1][0], y = p[1][1],
                    x2 = p[2][0], y2 = p[2][1];

                V1 = sqrt((x - centre_x) * (x - centre_x) + (y - centre_y) * (y - centre_y));
                
                V2 = sqrt((x2 - centre_x) * (x2 - centre_x) + (y2 - centre_y) * (y2 - centre_y));


                if (choice == "ID_ELLIPSE_DIRECT")
                    DirectEllipse(hdc, centre_x, centre_y, V1, V2, colour);

                else if (choice == "ID_ELLIPSE_POLAR")
                    EllipseWithPolar(hdc, centre_x, centre_y, V1, V2, colour);

                else if (choice == "ID_ELLIPSE_MIDPOINT")
                    EllipseWithMidpoint(hdc, centre_x, centre_y, V1, V2, colour);
                
                drawing_count = 0;
                ReleaseDC(hWnd, hdc);
            }
        }
        
        else if (parentChoice == "CARDI") {
            clicks[index] = Vector(LOWORD(lParam), HIWORD(lParam));
            index++;
        }
        
        else if (parentChoice == "FILLING") {
            if (choice == "ID_CIRCLEWITH_LINES") {
                p[drawing_count] = Vector(LOWORD(lParam), HIWORD(lParam));
                drawing_count++;
                
                if (drawing_count == 2) {
                    double centre_x = p[0][0], centre_y = p[0][1],
                        x = p[1][0], y = p[1][1];

                    double r = sqrt((x - centre_x) * (x - centre_x) + (y - centre_y) * (y - centre_y));
                    MidpointCircle(hdc, centre_x, centre_y, r, quarter, colour);

                    drawing_count = 0;
                    ReleaseDC(hWnd, hdc);
                }
            }

            else if(choice == "ID_CIRCLEWITH_CIRCLES"){
                p[drawing_count] = Vector(LOWORD(lParam), HIWORD(lParam));
                drawing_count++;
                if (drawing_count == 3) {
                    double centre_x = p[0][0], centre_y = p[0][1],
                        x = p[1][0], y = p[1][1],
                        x2 = p[2][0], y2 = p[2][1];

                    double r = sqrt((x - centre_x) * (x - centre_x) + (y - centre_y) * (y - centre_y));

                    CirclePolar(hdc, centre_x, centre_y, r, colour);

                    if (pow(x2 - centre_x, 2) + pow(y2 - centre_y, 2) < pow(r, 2)) {
                        fillWithCircles(hdc, centre_x, centre_y, x, y, x2, y2,fill_colour);
                    }

                    drawing_count = 0;
                    ReleaseDC(hWnd, hdc);
                }

            }

            else if(choice == "ID_SQUAREWITH_HERMITCURVE"){
                p[drawing_count] = Vector(LOWORD(lParam), HIWORD(lParam));
                drawing_count++;

                if (drawing_count == 2)
                {
                    dis = p[0][0] - p[1][0];
                    if (dis < 0)
                    {
                        dis = dis * -1;
                    }
                    Square(hdc, p[0], dis, colour);
                }
                else if (drawing_count == 4)
                {
                    lm = dis + p[0][0];
                    st = p[0][0] - dis;
                    p[1][0] = p[0][0] - dis;
                    p[1][1] = p[0][1] - dis;
                    p[0][0] = p[0][0] - dis;
                    p[0][1] = p[0][1] + dis;

                    p2[2][0] = p[2][0];
                    p2[2][1] = p[2][1];

                    p2[3][0] = p[3][0];
                    p2[3][1] = p[3][1];

                    for (int i = 0; i < dis * 2; i++)
                    {
                        Vector T1(3 * (p[2][0] - p[0][0]), 3 * (p[2][1] - p[0][1]));
                        Vector T2(3 * (p[1][0] - p[3][0]), 3 * (p[1][1] - p[3][1]));
                        hdc = GetDC(hWnd);
                        DrawHermiteCurvev2(hdc, p[0], T1, p[1], T2, fill_colour, lm, st);
                        ReleaseDC(hWnd, hdc);
                        p[0][0]++;
                        p[1][0]++;
                        p[2][0]++;
                        p[3][0]++;
                    }

                    // talk to ammar about the delay and explain using different colours
                    for (int i = 0; i < dis * 2; i++)
                    {
                        Vector T1(3 * (p2[2][0] - p[0][0]), 3 * (p2[2][1] - p[0][1]));
                        Vector T2(3 * (p[1][0] - p2[3][0]), 3 * (p[1][1] - p2[3][1]));
                        hdc = GetDC(hWnd);
                        DrawHermiteCurvev2(hdc, p[0], T1, p[1], T2, fill_colour, lm, st);
                        ReleaseDC(hWnd, hdc);
                        p[0][0]--;
                        p[1][0]--;
                        p2[2][0]--;
                        p2[3][0]--;
                    }
                    drawing_count = 0;
                    ReleaseDC(hWnd, hdc);
                }
            }

            else if(choice == "ID_RECTANGLEWITH_BEZIERCURVEHORIZONTAL"){
                if (drawing_count < 3)
                    p[drawing_count] = Vector(LOWORD(lParam), HIWORD(lParam));
                else
                    hermit[drawing_count - 2] = Vector(LOWORD(lParam), HIWORD(lParam));
                
                drawing_count++;
                
                if (drawing_count == 3) {
                    double x1 = p[0][0], y1 = p[0][1],
                        x2 = p[1][0], y2 = y1,
                        x3 = x2, y3 = p[2][1],
                        x4 = x1, y4 = y3;
                    //p[5] = Vector(y1, y4);
                    hermit[0] = Vector(x4, y4);
                    hermit[3] = Vector(x3, y3);
                        
                    // line 1 
                    parametricline(hdc, x1, y1, x2, y2, colour);
                    // line 2 
                    parametricline(hdc, x2, y2, x3, y3, colour);
                    // line 3 
                    parametricline(hdc, x3, y3, x4, y4, colour);
                    // line 4 
                    parametricline(hdc, x4, y4, x1, y1, colour);

                    ReleaseDC(hWnd, hdc);
                }
                
                else if (drawing_count == 5) {
                    // y1, y4
                    int v1 = p[0][1],
                        v2 = p[2][1];

                    fillingwithBezier(hdc, hermit[0], hermit[1], hermit[2], hermit[3], fill_colour, v1, v2);
                    drawing_count = 0;
                    ReleaseDC(hWnd, hdc);
                }
            }

            else if(choice == "ID_FILLING_RECURSIVE"){
                double x = LOWORD(lParam),
                y = HIWORD(lParam);
                FloodFill(hdc, x, y, colour, fill_colour);
                ReleaseDC(hWnd, hdc);
            }

            else if(choice == "ID_FILLING_NONRECURSIVE"){
                double x = LOWORD(lParam),
                    y = HIWORD(lParam);
                NRFloodFill(hdc, x, y, colour, fill_colour);
                ReleaseDC(hWnd, hdc);
            }
        }
        
        else if (parentChoice == "CONVEX") {
            
            points[drawing_count].x = LOWORD(lParam);
            points[drawing_count].y = HIWORD(lParam);
            drawing_count++;
            
            if (drawing_count == 5) {
                if (choice == "ID_FILLING_CONVEX") {
                    ConvexFill(hdc, points, 5, fill_colour);
                    drawing_count = 0;
                    ReleaseDC(hWnd, hdc);
                }

                else if (choice == "ID_FILLING_NONCONVEX") {
                    non_convexFill(hdc, points, 5, fill_colour);
                    drawing_count = 0;
                    ReleaseDC(hWnd, hdc);
                }
            }
        }
        
        else if (parentChoice == "CLIPPING") {

            if (choice == "ID_CIRCLECLIPPINGWINDOW_POINT") {
                p[point] = Vector(LOWORD(lParam), HIWORD(lParam));
                point++;
                int X_left = p[0][0], Y_top = p[0][1],
                    X_right = p[1][0], Y_bottom = p[1][1];
                X_right = X_left + (Y_bottom - Y_top);

                if (point == 2) {
                    Ellipse(hdc, X_left, Y_top, X_right, Y_bottom);
                    D = (int)sqrt(pow(abs(X_left - X_right), 2.0) + pow(abs(Y_top - Y_bottom), 2.0));
                    S = D / sqrt(2);
                    A = X_left + (S / 2);
                    B = Y_top + (S / 2);
                    B1 = B - (S / 2);
                    A1 = A;
                    R = (int)sqrt(pow(abs(A - A1), 2.0) + pow(abs(B - B1), 2.0));
                    
                    ReleaseDC(hWnd, hdc);
                }
                if (point == 3) {
                    int x3 = p[2][0], y3 = p[2][1];
                    Point_Clipping(hdc, x3, y3, A, B, R, colour);
                    point = 2;
                    ReleaseDC(hWnd, hdc);
                }
            }

            else if (choice == "ID_CIRCLECLIPPINGWINDOW_LINE") {
                p[point] = Vector(LOWORD(lParam), HIWORD(lParam));
                point++;
                int X_left = p[0][0], Y_top = p[0][1],
                    X_right = p[1][0], Y_bottom = p[1][1];
                    X_right = X_left + (Y_bottom - Y_top);

                if (point == 2 && flag) {
                    Ellipse(hdc, X_left, Y_top, X_right, Y_bottom);
                    D = (int)sqrt(pow(abs(X_left - X_right), 2.0) + pow(abs(Y_top - Y_bottom), 2.0));
                    S = D / sqrt(2);
                    A = X_left + (S / 2);
                    B = Y_top + (S / 2);
                    B1 = B - (S / 2);
                    A1 = A;
                    R = (int)sqrt(pow(abs(A - A1), 2.0) + pow(abs(B - B1), 2.0));

                    ReleaseDC(hWnd, hdc);
                }
                if (point == 4) {
                    flag = false;
                    int x3 = p[2][0], y3 = p[2][1];
                    int x4 = p[3][0], y4 = p[3][1];
                    Line_Clipping(hdc, x3, y3,x4, y4, A, B, R, colour);
                    point = 2;
                    ReleaseDC(hWnd, hdc);
                }
            }

            else if (choice == "ID_SQUARECLIPPINGWINDOW_POINT") {
                p[point] = Vector(LOWORD(lParam), HIWORD(lParam));
                point++;
                int X_left = p[0][0], Y_top = p[0][1],
                    X_right = p[1][0], Y_bottom = p[1][1];
                X_right = X_left + (Y_bottom - Y_top);

                if (point == 2 && flag) {
                    Rectangle(hdc, X_left, Y_top, X_right, Y_bottom);
                    ReleaseDC(hWnd, hdc);
                }
                if (point == 3) {
                    flag = false;
                    int x3 = p[2][0], y3 = p[2][1];
                    PointClipping(hdc, x3, y3, X_left, Y_top, X_right, Y_bottom);
                    point = 2;
                    ReleaseDC(hWnd, hdc);
                }
            }
            
            else if (choice == "ID_SQUARECLIPPINGWINDOW_LINE") {
                p[point] = Vector(LOWORD(lParam), HIWORD(lParam));
                point++;
                int X_left = p[0][0], Y_top = p[0][1],
                    X_right = p[1][0], Y_bottom = p[1][1];
                X_right = X_left + (Y_bottom - Y_top);

                if (point == 2 && flag) {
                    Rectangle(hdc, X_left, Y_top, X_right, Y_bottom);
                    ReleaseDC(hWnd, hdc);
                }
                if (point == 4) {
                    flag = false;
                    int x3 = p[2][0], y3 = p[2][1];
                    int x4 = p[3][0], y4 = p[3][1];
                    CohenSuth(hdc, x3, y3, x4, y4, X_left, Y_top, X_right, Y_bottom);
                    point = 2;
                    ReleaseDC(hWnd, hdc);
                }
            }

            else if (choice == "ID_RECTANGLECLIPPINGWINDOW_POINT") {
                p[point] = Vector(LOWORD(lParam), HIWORD(lParam));
                point++;
                int X_left = p[0][0], Y_top = p[0][1],
                    X_right = p[1][0], Y_bottom = p[1][1];
                //X_right = X_left + (Y_bottom - Y_top);

                if (point == 2 && flag) {
                    Rectangle(hdc, X_left, Y_top, X_right, Y_bottom);
                    ReleaseDC(hWnd, hdc);
                }
                if (point == 3) {
                    flag = false;
                    int x3 = p[2][0], y3 = p[2][1];
                    PointClipping(hdc, x3, y3, X_left, Y_top, X_right, Y_bottom);
                    point = 2;
                    ReleaseDC(hWnd, hdc);
                }
            }
            
            else if (choice == "ID_RECTANGLECLIPPINGWINDOW_LINE") {
                p[point] = Vector(LOWORD(lParam), HIWORD(lParam));
                point++;
                int X_left = p[0][0], Y_top = p[0][1],
                    X_right = p[1][0], Y_bottom = p[1][1];
                //X_right = X_left + (Y_bottom - Y_top);

                if (point == 2 && flag) {
                    Rectangle(hdc, X_left, Y_top, X_right, Y_bottom);
                    ReleaseDC(hWnd, hdc);
                }
                if (point == 4) {
                    flag = false;
                    int x3 = p[2][0], y3 = p[2][1];
                    int x4 = p[3][0], y4 = p[3][1];
                    CohenSuth(hdc, x3, y3, x4, y4, X_left, Y_top, X_right, Y_bottom);
                    point = 2;
                    ReleaseDC(hWnd, hdc);
                }
            }
            
            else if (choice == "ID_RECTANGLECLIPPINGWINDOW_POLYGON") {}

        }        
        
        break;
    
    case WM_RBUTTONDOWN:
        clicks[index] = Vector(LOWORD(lParam), HIWORD(lParam));
        CardinalSplineCurve(hdc, clicks, index, 0.5, colour);
        clicks[0] = Vector(LOWORD(lParam), HIWORD(lParam));
        index = 1;
        ReleaseDC(hWnd, hdc);
        break;

    case WM_PAINT:
    {
        // comment the if_condition to need no an option
        if (parentChoice == "white") {
            PAINTSTRUCT ps;
            hdc = BeginPaint(hWnd, &ps);
            // All painting occurs here, between BeginPaint and EndPaint.
            FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
            EndPaint(hWnd, &ps);
        }
    }
    break;

    case WM_SETCURSOR:
        if (addi == "mouse") {
            SetCursor(LoadCursor(nullptr, IDC_CROSS));
            SetWindowLongPtr(hWnd, DWLP_MSGRESULT, TRUE);
        }
        break;
    
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}



// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
