#pragma once

#include "resource.h"
//#include <stack>



int Round(double x)
{
	return (int)(x + 0.5);
}

void swap(int& x1, int& y1, int& x2, int& y2) {
	int tmp = x1;
	x1 = x2;
	x2 = tmp;
	tmp = y1;
	y1 = y2;
	y2 = tmp;
}

struct Vector {
	double v[2];
	Vector(double x = 0, double y = 0)
	{
		v[0] = x; v[1] = y;
	}
	double& operator[](int i) {
		return v[i];
	}
};

//***************************
//***   Functions area   ****
//***************************

void Draw4Points(HDC hdc, int xc, int yc, int x, int y, COLORREF color)
{
	SetPixel(hdc, xc + x, yc + y, color);
	SetPixel(hdc, xc + x, yc - y, color);
	SetPixel(hdc, xc - x, yc + y, color);
	SetPixel(hdc, xc - x, yc - y, color);
}

void Draw8Points(HDC hdc, int xc, int yc, int x, int y, COLORREF c) {
	// Oc_1   a,b
	SetPixel(hdc, xc + x, yc + y, c);
	// Oc_2   -a,b
	SetPixel(hdc, xc - x, yc + y, c);
	// Oc_5   -a,-b
	SetPixel(hdc, xc - x, yc - y, c);
	// Oc_6   a,-b
	SetPixel(hdc, xc + x, yc - y, c);
	// Oc_ 8  b,a
	SetPixel(hdc, xc + y, yc + x, c);
	// Oc_3   -b,a
	SetPixel(hdc, xc - y, yc + x, c);
	// Oc_4   -b,-a
	SetPixel(hdc, xc - y, yc - x, c);
	// Oc_7   b,-a
	SetPixel(hdc, xc + y, yc - x, c);
}

// --> line functions
void DrawLineMidPoint(HDC hdc, int x1, int y1, int x2, int y2, COLORREF color)
{
	int dx = abs(x2 - x1), dy = abs(y2 - y1);
	int dx2 = x2 - x1, dy2 = y2 - y1;
	int s;
	if (dx2 * dy2 > 0)s = 1;
	else s = -1;
	int d;
	if (dy <= dx)
	{
		if (x1 > x2) swap(x1, y1, x2, y2);
		d = 2 * dy - dx;
		int d1 = 2 * (dy - dx), d2 = 2 * dy;
		int x = x1, y = y1;
		SetPixel(hdc, x, y, color);
		while (x < x2)
		{
			if (d < 0) d += d2;
			else { d += d1; y += s; }
			x++;
			SetPixel(hdc, x, y, color);
		}
	}
	else
	{
		if (y1 > y2) swap(x1, y1, x2, y2);
		d = 2 * dx - dy;
		int d1 = 2 * (dx - dy), d2 = 2 * dx;
		int x = x1, y = y1;
		SetPixel(hdc, x, y, color);
		while (y < y2)
		{
			if (d < 0) d += d2;
			else { d += d1; x += s; }
			y++;
			SetPixel(hdc, x, y, color);
		}
	}
}

void SimpleDDA(HDC hdc, int xs, int ys, int xe, int ye, COLORREF color) {
	int dx = xe - xs;
	int dy = ye - ys;
	SetPixel(hdc, xs, ys, color);
	if (abs(dx) >= abs(dy)) {
		int x = xs, xinc = dx > 0 ? 1 : -1;
		double y = ys, yinc = (double)dy / dx * xinc;
		while (x != xe) {
			x += xinc;
			y += yinc;
			SetPixel(hdc, x, round(y), color);
		}
	}
	else {
		int y = ys, yinc = dy > 0 ? 1 : -1;
		double x = xs, xinc = (double)dx / dy * yinc;
		while (y != ye) {
			x += xinc;
			y += yinc;
			SetPixel(hdc, round(x), y, color);
		}
	}
}

void parametricline(HDC hdc, int x1, int y1, int x2, int y2, COLORREF c) {
	double dx = x2 - x1;
	double dy = y2 - y1;
	for (double t = 0; t < 1; t += 0.001) {
		int x = x1 + (dx * t);
		int y = y1 + (dy * t);
		SetPixel(hdc, x, y, c);
	}
}

// --> circle functions
void CircleDirect(HDC hdc, int xc, int yc, int R, COLORREF color)
{
	int x = 0,
		y = R,
		R2 = R * R;
	Draw8Points(hdc, xc, yc, x, y, color);
	while (x < y)
	{
		x++;
		y = round(sqrt((double)(R2 - x * x)));
		Draw8Points(hdc, xc, yc, x, y, color);
	}
}

void CirclePolar(HDC hdc, int xc, int yc, int R, COLORREF color)
{
	int x = R, y = 0;
	double theta = 0, dtheta = 1.0 / R;
	Draw8Points(hdc, xc, yc, x, y, color);
	while (x > y)
	{
		theta += dtheta;
		x = round(R * cos(theta));
		y = round(R * sin(theta));
		Draw8Points(hdc, xc, yc, x, y, color);
	}
}

void CircleIterativePolar(HDC hdc, int xc, int yc, int R, COLORREF color)
{
	double x = R, y = 0,
		dtheta = 1.0 / R,
		cdtheta = cos(dtheta),
		sdtheta = sin(dtheta);
	Draw8Points(hdc, xc, yc, x, y, color);
	while (x > y)
	{
		double x1 = x * cdtheta - y * sdtheta;
		y = x * sdtheta + y * cdtheta;
		x = x1;
		Draw8Points(hdc, xc, yc, round(x), round(y), color);
	}
}

void CircleBresenham(HDC hdc, int xc, int yc, int R, COLORREF color)
{
	int x = 0, y = R;
	int d = 1 - R;
	Draw8Points(hdc, xc, yc, x, y, color);
	while (x < y)
	{
		if (d < 0)
			d += 2 * x + 2;
		else
		{
			d += 2 * (x - y) + 5;
			y--;
		}
		x++;
		Draw8Points(hdc, xc, yc, x, y, color);
	}
}

void CircleFasterBresenham(HDC hdc, int xc, int yc, int R, COLORREF color)
{
	int x = 0, y = R,
		d = 1 - R,
		c1 = 3,
		c2 = 5 - 2 * R;
	Draw8Points(hdc, xc, yc, x, y, color);
	while (x < y)
	{
		if (d < 0)
		{
			d += c1;
			c2 += 2;
		}
		else
		{
			d += c2;
			c2 += 4;
			y--;
		}
		c1 += 2;
		x++;
		Draw8Points(hdc, xc, yc, x, y, color);
	}
}


// --> Ellipse functions
void DirectEllipse(HDC hdc, int xc, int yc, int A, int B, COLORREF color)
{
	int x = 0;
	double y = B;
	Draw4Points(hdc, xc, yc, 0, B, color);
	while (x * B * B < y * A * A)
	{
		x++;
		y = B * sqrt(1.0 - (double)x * x / (A * A));
		Draw4Points(hdc, xc, yc, x, Round(y), color);
	}
	int y1 = 0;
	double x1 = A;
	Draw4Points(hdc, xc, yc, A, 0, color);
	while (x1 * B * B > y1 * A * A)
	{
		y1++;
		x1 = A * sqrt(1.0 - (double)y1 * y1 / (B * B));
		Draw4Points(hdc, xc, yc, Round(x1), y1, color);
	}
}

void EllipseWithMidpoint(HDC hdc, int xc, int yc, int A, int B, COLORREF color)
{
	int x = 0;
	int y = B;
	int d1 = (B * B) - (A * A * B) + (0.25 * A * A);
	int dx = 2 * B * B * x;
	int dy = 2 * A * A * y;
	Draw4Points(hdc, xc, yc, 0, B, color);
	while (dx < dy) {
		Draw4Points(hdc, xc, yc, x, y, color);
		if (d1 < 0) {
			x++;
			dx = dx + (2 * B * B);
			d1 = d1 + dx + (B * B);
		}
		else {
			x++;
			y--;
			dx = dx + (2 * B * B);
			dy = dy - (2 * A * A);
			d1 = d1 + dx - dy + (B * B);
		}
	}
	int d2 = ((B * B) * ((x + 0.5) * (x + 0.5))) + ((A * A) * ((y - 1) * (y - 1))) - (A * A * B * B);
	while (y >= 0) {
		Draw4Points(hdc, xc, yc, x, Round(y), color);
		if (d2 > 0)
		{
			y--;
			dy = dy - (2 * A * A);
			d2 = d2 + (A * A) - dy;
		}
		else
		{
			y--;
			x++;
			dx = dx + (2 * B * B);
			dy = dy - (2 * A * A);
			d2 = d2 + dx - dy + (A * A);
		}
	}
}

void EllipseWithPolar(HDC hdc, int xc, int yc, int A, int B, COLORREF color)
{
	int x = 0;
	int y = B;
	double theta = 0.0;
	double deltaTheta = A;
	Draw4Points(hdc, xc, yc, 0, B, color);
	while (theta <= deltaTheta) {
		theta += 1 / deltaTheta;
		x = Round(A * cos(theta));
		y = Round(B * sin(theta));
		Draw4Points(hdc, xc, yc, x, y, color);
	}
}

// --> Cardinal functions
void DrawHermiteCurve(HDC hdc, Vector& p1, Vector& T1, Vector& p2, Vector& T2, COLORREF c)
{
	double a0 = p1[0], a1 = T1[0],
		a2 = -3 * p1[0] - 2 * T1[0] + 3 * p2[0] - T2[0],
		a3 = 2 * p1[0] + T1[0] - 2 * p2[0] + T2[0];
	double b0 = p1[1], b1 = T1[1],
		b2 = -3 * p1[1] - 2 * T1[1] + 3 * p2[1] - T2[1],
		b3 = 2 * p1[1] + T1[1] - 2 * p2[1] + T2[1];
	for (double t = 0; t <= 1; t += 0.001)
	{
		double t2 = t * t, t3 = t2 * t;
		double x = a0 + a1 * t + a2 * t2 + a3 * t3;
		double y = b0 + b1 * t + b2 * t2 + b3 * t3;;
		SetPixel(hdc, Round(x), Round(y), c);
	}
}

void CardinalSplineCurve(HDC hdc, Vector p1[], int n, double c, COLORREF color)
{
	c = c / 2;
	Vector* T = new Vector[n + 1];
	for (int i = 1; i < n; i++)
	{

		T[i][0] = c * (p1[i + 1][0] - p1[i - 1][0]);
		T[i][1] = c * (p1[i + 1][1] - p1[i - 1][1]);
	}
	T[0][0] = c * (p1[1][0] - p1[0][0]);
	T[0][1] = c * (p1[1][1] - p1[0][1]);

	T[n][0] = c * (p1[n][0] - p1[n - 1][0]);
	T[n][1] = c * (p1[n][1] - p1[n - 1][1]);
	for (int i = 0; i < n; i++)
	{
		DrawHermiteCurve(hdc, p1[i], T[i], p1[i + 1], T[i + 1], color);
	}
}


// --> Filling functions

// square filling hermitCurve
void Square(HDC hdc, Vector p1, int dis, COLORREF c)
{
	Vector p2;
	p2 = Vector(p1[0] + dis, p1[1] + dis);
	for (int i = 0; i < dis * 2; i++)
	{
		SetPixel(hdc, Round(p2[0]), Round(p2[1] - i), c);
		SetPixel(hdc, Round(p2[0] - dis * 2), Round(p2[1] - i), c);
		SetPixel(hdc, Round(p2[0] - i), Round(p2[1]), c);
		SetPixel(hdc, Round(p2[0] - i), Round(p2[1] - dis * 2), c);
	}
}

void DrawHermiteCurvev2(HDC hdc, Vector& p1, Vector& T1, Vector& p2, Vector& T2, COLORREF c, int lm, int st)
{
	double a0 = p1[0],
		a1 = T1[0],
		a2 = -3 * p1[0] - 2 * T1[0] + 3 * p2[0] - T2[0],
		a3 = 2 * p1[0] + T1[0] - 2 * p2[0] + T2[0];

	double b0 = p1[1],
		b1 = T1[1],
		b2 = -3 * p1[1] - 2 * T1[1] + 3 * p2[1] - T2[1],
		b3 = 2 * p1[1] + T1[1] - 2 * p2[1] + T2[1];

	for (double t = 0; t <= 1; t += 0.001)
	{
		double t2 = t * t,
			t3 = t2 * t,
			x = a0 + a1 * t + a2 * t2 + a3 * t3,
			y = b0 + b1 * t + b2 * t2 + b3 * t3;
		if (x<lm && x > st)
			SetPixel(hdc, Round(x), Round(y), c);
	}
}

// filling circle with circles
void DrawQuarter(HDC hdc, int xc, int yc, int R, int xf, int yf, COLORREF c) {
	int x1 = R, y1 = 0;
	double theta = 0, dtheta = 1.0 / R;
	while (x1 > y1) {
		theta += dtheta;
		x1 = Round(R * cos(theta));
		y1 = Round(R * sin(theta));
		if (xc<xf && yc>yf) {
			SetPixel(hdc, xc + x1, yc - y1, c);
			SetPixel(hdc, xc + y1, yc - x1, c);
		}
		else if (xc > xf && yc > yf) {
			SetPixel(hdc, xc - x1, yc - y1, c);
			SetPixel(hdc, xc - y1, yc - x1, c);
		}
		else if (xc > xf && yc < yf) {
			SetPixel(hdc, xc - x1, yc + y1, c);
			SetPixel(hdc, xc - y1, yc + x1, c);
		}
		else if (xc<xf && yf>yc) {
			SetPixel(hdc, xc + x1, yc + y1, c);
			SetPixel(hdc, xc + y1, yc + x1, c);
		}
	}
}

void fillWithCircles(HDC hdc, int xc, int yc, int x1, int y1, int xf, int yf, COLORREF co) {
	int dx = x1 - xc;
	int dy = y1 - yc;
	for (double t = 0; t < 1; t += 0.01) {
		int x = xc + (dx * t);
		int y = yc + (dy * t);
		int r = sqrt((x1 - x) * (x1 - x) + (y1 - y) * (y1 - y));
		DrawQuarter(hdc, xc, yc, r, xf, yf, co);

	}
}

// filling circle with lines
void draw8_Points(HDC hdc, int xc, int yc, int x, int y, int n, COLORREF cl)
{
	SetPixel(hdc, xc + x, yc + y, cl);
	SetPixel(hdc, xc - x, yc + y, cl);
	SetPixel(hdc, xc - x, yc - y, cl);
	SetPixel(hdc, xc + x, yc - y, cl);
	SetPixel(hdc, xc + y, yc + x, cl);
	SetPixel(hdc, xc - y, yc + x, cl);
	SetPixel(hdc, xc - y, yc - x, cl);
	SetPixel(hdc, xc + y, yc - x, cl);
	switch (n)
	{
	case 1: //first quarter
	{
		parametricline(hdc, xc, yc, xc + x, yc - y, cl);
		parametricline(hdc, xc, yc, xc + y, yc - x, cl);
		break;
	}
	case 2: // second quarter
	{
		parametricline(hdc, xc, yc, xc + x, yc + y, cl);
		parametricline(hdc, xc, yc, xc + y, yc + x, cl);
		break;
	}
	case 3: // third quarter
	{
		parametricline(hdc, xc, yc, xc - x, yc + y, cl);
		parametricline(hdc, xc, yc, xc - y, yc + x, cl);
		break;
	}
	case 4: // fourth quarter
	{
		parametricline(hdc, xc, yc, xc - x, yc - y, cl);
		parametricline(hdc, xc, yc, xc - y, yc - x, cl);
		break;
	}
	}
}

void MidpointCircle(HDC hdc, int xc, int yc, int r, int n, COLORREF cl)
{
	int x = 0;
	int y = r;
	double d = 1 - r;
	draw8_Points(hdc, xc, yc, x, y, n, cl);
	while (x < y)
	{
		if (d <= 0)
		{
			d = d + 2 * x + 3;
			x++;
		}
		else
		{
			d = d + 2 * (x - y) + 5;
			x++;
			y--;
		}
		draw8_Points(hdc, xc, yc, x, y, n, cl);
	}
}


// filling for rectangle
void DrawHermiteCurve(HDC hdc, Vector& p1, Vector& T1, Vector& p2, Vector& T2, COLORREF color, int var1, int var2)
{
	double a0 = p1[0], a1 = T1[0],
		a2 = -3 * p1[0] - 2 * T1[0] + 3 * p2[0] - T2[0],
		a3 = 2 * p1[0] + T1[0] - 2 * p2[0] + T2[0];
	double b0 = p1[1], b1 = T1[1],
		b2 = -3 * p1[1] - 2 * T1[1] + 3 * p2[1] - T2[1],
		b3 = 2 * p1[1] + T1[1] - 2 * p2[1] + T2[1];
	for (double t = 0; t <= 1; t += 0.001)
	{
		double t2 = t * t, t3 = t2 * t;
		double x = a0 + a1 * t + a2 * t2 + a3 * t3;
		double y = b0 + b1 * t + b2 * t2 + b3 * t3;

		// m = max(var)
		// don't touch: var1 < y < var2
		if (y > var1 && y < var2)
			SetPixel(hdc, round(x), round(y), color);
	}
}

void DrawBezierCurve(HDC hdc, Vector& P0, Vector& P1, Vector P2, Vector P3, COLORREF color, double var1, double var2)
{
	Vector T0(3 * (P1.v[0] - P0.v[0]), 3 * (P1.v[1] - P0.v[1]));
	Vector T1(3 * (P3.v[0] - P2.v[0]), 3 * (P3.v[1] - P2.v[1]));
	DrawHermiteCurve(hdc, P0, T0, P3, T1, color, var1, var2);
}

void fillingwithBezier(HDC hdc, Vector& P0, Vector& P1, Vector P2, Vector P3, COLORREF color, double var1, double var2)
{
	int distance = abs(var2 - var1);

	for (int i = 0; i <= distance; i++)
	{
		P0[1]--;
		P1[1]--;
		P2[1]--;
		P3[1]--;
		DrawBezierCurve(hdc, P0, P1, P2, P3, color, var1, var2);
	}

	for (int i = 0; i <= distance * 2; i++)
	{
		P0[1]++;
		P1[1]++;
		P2[1]++;
		P3[1]++;

		DrawBezierCurve(hdc, P0, P1, P2, P3, color, var1, var2);
	}
}

//// CLIIPPING
void Line_Clipping(HDC hdc, int x_1, int y_1, int x_2, int y_2, int X, int Y, int R, COLORREF c)
{
	double dx = x_2 - x_1;
	double dy = y_2 - y_1;
	for (double t = 0; t < 1; t += 0.01)
	{
		int x = x_1 + (t * dx);
		int y = y_1 + (t * dy);
		int RE = (int)sqrt(pow(abs(X - x), 2.0) + pow(abs(Y - y), 2.0));
		if (RE < R)
		{
			SetPixel(hdc, x, y, c);
		}
	}
}

void Point_Clipping(HDC hdc, int X1, int Y1, int X, int Y, int R, COLORREF c)
{
	int RE = (int)sqrt(pow(abs(X1 - X), 2.0) + pow(abs(Y1 - Y), 2.0));
	if (RE < R)
	{
		SetPixel(hdc, X1, Y1, c);
	}
}

void PointClipping(HDC hdc, int x, int y, int xleft, int ytop, int xright, int ybottom)
{
	if (x >= xleft && x <= xright && y >= ytop && y <= ybottom)
		SetPixel(hdc, x, y, RGB(0, 0, 0));
}

union OutCode
{
	unsigned All : 4;
	struct
	{
		unsigned left : 1, top : 1, right : 1, bottom : 1;
	};
};

OutCode GetOutCode(double x, double y, int xleft, int ytop, int xright, int ybottom)
{
	OutCode out;
	out.All = 0;
	if (x < xleft)
		out.left = 1;
	else if (x > xright)
		out.right = 1;
	if (y < ytop)
		out.top = 1;
	else if (y > ybottom)
		out.bottom = 1;
	return out;
}

void VIntersect(double xs, double ys, double xe, double ye, int x, double* xi, double* yi)
{
	*xi = x;
	*yi = ys + (x - xs) * (ye - ys) / (xe - xs);
}

void HIntersect(double xs, double ys, double xe, double ye, int y, double* xi, double* yi)
{
	*yi = y;
	*xi = xs + (y - ys) * (xe - xs) / (ye - ys);
}

void CohenSuth(HDC hdc, int xs, int ys, int xe, int ye, int xleft, int ytop, int xright, int ybottom)
{
	double x1 = xs, y1 = ys, x2 = xe, y2 = ye;
	OutCode out1 = GetOutCode(x1, y1, xleft, ytop, xright, ybottom);
	OutCode out2 = GetOutCode(x2, y2, xleft, ytop, xright, ybottom);
	while ((out1.All || out2.All) && !(out1.All & out2.All))
	{
		double xi, yi;
		if (out1.All)
		{
			if (out1.left)
				VIntersect(x1, y1, x2, y2, xleft, &xi, &yi);
			else if (out1.top)
				HIntersect(x1, y1, x2, y2, ytop, &xi, &yi);
			else if (out1.right)
				VIntersect(x1, y1, x2, y2, xright, &xi, &yi);
			else
				HIntersect(x1, y1, x2, y2, ybottom, &xi, &yi);
			x1 = xi;
			y1 = yi;
			out1 = GetOutCode(x1, y1, xleft, ytop, xright, ybottom);
		}
		else
		{
			if (out2.left)
				VIntersect(x1, y1, x2, y2, xleft, &xi, &yi);
			else if (out2.top)
				HIntersect(x1, y1, x2, y2, ytop, &xi, &yi);
			else if (out2.right)
				VIntersect(x1, y1, x2, y2, xright, &xi, &yi);
			else
				HIntersect(x1, y1, x2, y2, ybottom, &xi, &yi);
			x2 = xi;
			y2 = yi;
			out2 = GetOutCode(x2, y2, xleft, ytop, xright, ybottom);
		}
	}
	if (!out1.All && !out2.All)
	{
		MoveToEx(hdc, Round(x1), Round(y1), NULL);
		LineTo(hdc, Round(x2), Round(y2));
	}
}


