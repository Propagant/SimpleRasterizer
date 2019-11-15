#include <windows.h>
#include <iostream>
#include <ctime>

//-------------Very simple rasterizer by Matej Vanco 2019

using namespace std;

#define MPI 3.14159265358979323846

#pragma region Definitions

struct Point
{
	int x, y;
	Point(int xx, int yy)
	{
		x = xx;
		y = yy;
	}
};

struct Triangle
{
	Point p0, p1, p2;
};

struct Color
{
	int r, g, b;
	Color(int rr, int gg, int bb)
	{
		r = rr;
		g = gg;
		b = bb;
	}
};

void drawTriangle(Triangle t, Color c);
void drawPyramid(Triangle t, Triangle t2, Color c1, Color c2 = Color(50, 50, 50));
void drawCube(Triangle t, Triangle t2, Triangle t3, Triangle t4, Triangle t5, Triangle t6, Color c1, Color c2 = Color(100, 100, 100), Color c3 = Color(50, 50, 50));
void drawCircle(Point pos, int radius, Color c = Color(100, 100, 100));

#pragma endregion

HDC consoleDC;

//------Key Event method
bool GetKeyPress(KEY_EVENT_RECORD& krec)
{
	DWORD cc;
	INPUT_RECORD irec;
	HANDLE h = GetStdHandle(STD_INPUT_HANDLE);

	if (h == NULL)
		return false;

	for (; ; )
	{
		ReadConsoleInput(h, &irec, 1, &cc);
		if (irec.EventType == KEY_EVENT
			&& ((KEY_EVENT_RECORD&)irec.Event).bKeyDown)
		{
			krec = (KEY_EVENT_RECORD&)irec.Event;
			return true;
		}
	}
	return false;
}

//-------Main Function
int main()
{
	HWND consoleWindow = GetConsoleWindow();

	SetConsoleTitle("Simple Rasterizer by Matej Vanco");

	RECT r;
	GetWindowRect(consoleWindow, &r);
	MoveWindow(consoleWindow, r.left, r.top, 800, 600, TRUE);

	consoleDC = GetDC(consoleWindow);
	FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));

	int offset = 20;
	struct Point cSize { 400, 400 };

	KEY_EVENT_RECORD key;
	
	clock_t current_ticks, delta_ticks;
	clock_t fps;

	Triangle t{ Point(250,100), Point(124,350) ,Point(350,280) };
	Triangle t2{ Point(250,100), Point(50,250) ,Point(124,350) };

	Triangle t3{ Point(250,100), Point(124,350) ,Point(350,280) };
	Triangle t4{ Point(250,100), Point(50,250) ,Point(124,350) };

	Triangle t5{ Point(250,100), Point(124,350) ,Point(350,280) };
	Triangle t6{ Point(250,100), Point(50,250) ,Point(124,350) };

	Color c{ 150, 150, 150 };
	cout << "Press A & D to choose between shapes... Press Space to refresh. ";

	int shape = 0;
	while (true)
	{
		GetKeyPress(key);
		if (key.uChar.AsciiChar == 'd')
			shape++;
		else if (key.uChar.AsciiChar == 'a')
			shape--;
		else if (key.uChar.AsciiChar == ' ')
			system("cls");
		else continue;

		if (shape >= 4)
		{
			shape = 0;
			system("cls");
		}
		else if (shape < 0)
			shape = 3;

		current_ticks = clock();

		for (int y = offset; y < cSize.y; y++)
		{
			for (int x = offset; x < cSize.x; x++)
				SetPixel(consoleDC, x, y, RGB(0, 0, 0));
		}

		switch (shape)
		{
			//---Pyramide
			case 1:
				cout << "1 Pyramide ";
				 t = Triangle { Point(250,100), Point(124,350) ,Point(350,280) };
				 t2 = Triangle { t.p0, Point(50,250) , t.p1 };
				drawPyramid(t, t2, c);
				break;

			//---Cube
			case 2:
				cout << "2 Cube ";
				//RightFace
				t = Triangle{ Point(380, 100), Point(250, 150), Point(380, 300) };
				t2 = Triangle{ t.p1, Point(t.p1.x, 380), t.p2};
				//LeftFace
				t3 = Triangle{ Point(20, t.p0.y), t2.p1, t2.p0 };
				t4 = Triangle{ t3.p0, Point(20, t.p2.y), t3.p1 };
				//TopFace
				t5 = Triangle{ Point(130, 60), t3.p0, t.p1 };
				t6 = Triangle{ t5.p0, t.p1, t.p0 };
				drawCube(t, t2, t3, t4, t5, t6, c);
				break;

			//---Circle
			case 3:
				cout << "3 Circle ";
				drawCircle(Point(200,200),100);
				break;

			//---Single Triangle
			default:
				cout << "0 Triangle ";
				t = Triangle{ Point(200,80), Point(50,350) ,Point(350,350) };
				drawTriangle(t, c);
			break;
		}
	}
}

#pragma region Methods

int inArea(int x, int y, int x1, int y1, int x2, int y2)
{
	if (x <= max(x1, x2) && x >= min(x1, x2) &&
		y <= max(y1, y2) && y >= min(y1, y2))
		return (((x - x1) * (y2 - y1)) - ((y - y1) * (x2 - x1)));
	else
		return 0;
}

void drawTriangle(Triangle t, Color color)
{
	int xmin = min(t.p0.x,min(t.p1.x,t.p2.x));
	int ymin = min(t.p0.y, min(t.p1.y, t.p2.y));
	int xmax = max(t.p0.x, max(t.p1.x, t.p2.x));
	int ymax = max(t.p0.y, max(t.p1.y, t.p2.y));

	for (int y = ymin; y < ymax; y++)
	{
		for (int x = xmin; x < xmax; x++)
		{
			int a = inArea(x, y, t.p0.x, t.p0.y, t.p1.x, t.p1.y);
			int b = inArea(x, y, t.p1.x, t.p1.y, t.p2.x, t.p2.y);
			int c = inArea(x, y, t.p2.x, t.p2.y, t.p0.x, t.p0.y);

			if (a >= 0 && b >= 0 && c >= 0)
				SetPixel(consoleDC, x, y, RGB(color.r, color.g, color.b));
		}
	}
}

void drawPyramid(Triangle t, Triangle t2, Color color, Color color2)
{
	drawTriangle(t, color);
	drawTriangle(t2, color2);
}

void drawCube(Triangle t, Triangle t2, Triangle t3, Triangle t4, Triangle t5, Triangle t6, Color color, Color color2, Color color3)
{
	//---Face 1
	drawTriangle(t, color);
	drawTriangle(t2, color);
	//---Face 2
	drawTriangle(t3, color2);
	drawTriangle(t4, color2);
	//---Face 3
	drawTriangle(t5, color3);
	drawTriangle(t6, color3);
}

void drawCircle(Point pos, int radius, Color color)
{
	for (int j = 1; j <= radius; j++) //---Circle Radius
	{
		int fill = j;
		for (double i = 0.0; i < 360.0; i += 0.1) //---Angle
		{
			double angle = i * MPI / 180;

			int x = (int)(pos.x + fill * cos(angle));
			int y = (int)(pos.y + fill * sin(angle));

			SetPixel(consoleDC, x, y, RGB(j*2, j*2, j*2));
		}
	}
}

#pragma endregion