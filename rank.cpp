#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <iostream>
#include <algorithm>
#include <set>
#include <map>
#include <vector>
#include <string>
#include <queue>
using namespace std;
typedef long long LL;
#define For(i,a,b) for (int i = (a); i <= (b); i++)
#define Cor(i,a,b) for (int i = (a); i >= (b); i--)
#define rep(i,a) for (int i = 0; i < a; i++)
#define Fill(a,b) memset(a,b,sizeof(a))

struct Card
{
	int color, number;
	Card(int c, int n): color(c), number(n) {}
	Card() {}
};

class Rank
{
	private:
		map<int, int> OT;
		int flushes[8000], unique5[8000];
		const int prime[15] = {0, 0, 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41};
		int hashCard(int num, int col)
		{
			int x = prime[num];
			x += num << 8;
			x += 1 << (12 + col);
			x += 1 << (num - 2 + 16);
			return x;
		} 
	public:
		void init()
		{
			FILE *f = fopen("flushes.out", "r");
			int a, b;
			while (fscanf(f, "%d %d", &a, &b) != EOF)
				flushes[b] = a;
			fclose(f);
			f = fopen("unique5.out", "r");
			while (fscanf(f, "%d %d", &a, &b) != EOF)
				unique5[b] = a;
			fclose(f);
			f = fopen("othertype.out", "r");
			while (fscanf(f, "%d %d", &a, &b) != EOF)
				OT[b] = a;
			fclose(f);
		}
		int rank(Card *C)
		{
			int c[6];
			rep(i, 5)
				c[i] = hashCard(C[i].number, C[i].color);
			int r = c[0];
			For(i, 1, 4)
				r &= c[i];
			r &= 0xF000;
			int q = 0;
			For (i, 0, 4)
				q |= c[i];
			q >>= 16;
			if (r != 0)
			{
				return flushes[q];
			}
			else
			{
				int num = __builtin_popcount(q);
				if (num == 5) 
				{
					return unique5[q];
				}
				else
				{
					int x = 1;
					For(i, 0, 4)
						x *= (c[i] & 0xff);
					return OT[x];
				}
			}
		}
};
int main()
{
	Rank r;
	r.init();
	Card c[6];
	For(i, 0, 4)
		scanf("%d%d", &c[i].number, &c[i].color);
	cout << r.rank(c) << endl;
	return 0;
}
