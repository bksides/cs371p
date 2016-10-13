#include "Allocator.h"

int main()
{
	Allocator<int, 100> a;
	a.allocate(30);
}