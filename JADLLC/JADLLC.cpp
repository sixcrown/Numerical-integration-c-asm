#include "stdafx.h"
struct IUnknown;
_declspec(dllexport)void Trapezoid(float* a, float* b, float* c, int max,float step)
{
	for (int i = 0; i < max; i++) {
		c[i] = ((a[i] + b[i])/(float)2) * step;
	}
}