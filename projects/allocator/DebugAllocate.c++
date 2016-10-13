#include "Allocator.h"

int main()
{
	Allocator<int, 1000> a;
	a.allocate(30);
}