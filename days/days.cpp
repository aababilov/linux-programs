#include <cstdio>
#include <cstdlib>

int get_l(int d, int m, int y, int nd, int nm, int ny)
{
	const int mth[] = {31,29,31,
	     30,31,30,
	     31,31,30,
	     31,30,31};

	int i = 1;
	--m;
	--nm;
	while (y != ny || m != nm || d != nd) {
		printf("%d.%d.%d\n", d, m + 1, y);
		++i;
		++d;
		if (mth[m] < d || m == 1 && d == 29 &&
			y % 4 != 0) {
			d = 1;
			++m;
			if (m >= 12) {
				++y;
				m = 0;
			}
				
		}
	}
	return i;
}

int d, m, y;
int nd = 27, nm = 3, ny = 2009;
int i = 1;


int main()
{
	scanf("%d.%d.%d", &d, &m, &y);
	scanf("%d.%d.%d", &nd, &nm, &ny);
	i = get_l(d, m, y, nd, nm, ny);
	printf("%d\n", i);
	int lth[] = {23,28,33};
	for (int k = 0; k < 3; ++k) {
		int l = lth[k];
		printf("%d %d\n", i / l, i % l);
	}
}
