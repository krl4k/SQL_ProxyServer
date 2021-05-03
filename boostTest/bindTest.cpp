#include <iostream>
#include <functional>

template <class Operation>
void display( const int a[], size_t n, Operation operation )
{
	for ( size_t i = 0; i < n; i++ )
	{
		std::cout << operation( a[i] ) << ' ';
	}
	std::cout << std::endl;
}

int main()
{
	const size_t N = 10;
	int a[N] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

	auto square = bind( std::multiplies<int>(), std::placeholders::_1, std::placeholders::_1 );

	display( a, N, square );
	std::cout << std::endl;
}   