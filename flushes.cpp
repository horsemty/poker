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

const int prime[15] = {0, 0, 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41};
void hashCard(int num, int col)
{
	int x = prime[num];
	x += num << 8;
	x += 1 << (12 + col);
	x += 1 << (num - 2 + 16);
} 
struct Card5
{
	int a[6];
	bool operator <(const Card5 &b) const
	{
		int pd1 = 1;
		for (int i = 0; i < 4; i++)
			if (a[i + 1] != a[i] - 1 && !(a[i + 1] == 14 && a[i] == 2))
			{
				pd1 = 0;
				break;
			}
		int pd2 = 1;
		for (int i = 0; i < 4; i++)
			if (b.a[i + 1] != b.a[i] - 1 && !(b.a[i + 1] == 14 && b.a[i] == 2))
			{
				pd2 = 0;
				break;
			}
		if (pd1 & pd2)
		{
			return a[0] > b.a[0];
		}
		if (pd1 + pd2 == 1)
		{
			return pd1;
		}
		for (int i = 0; i < 5; i++)
			if (a[i] != b.a[i])
				return a[i] > b.a[i];
	}
};
Card5 C[10000], tmpC;
int tot = 0;
void dfs(int n, int num)
{
	if (n > 4)
	{
		C[tot++] = tmpC;
		return;
	}
	for (int i = 2; i < num; i++)
	{
		tmpC.a[n] = i;
		dfs(n + 1, i);
	}
}
void getFlushes()
{
	dfs(0, 15);
	For(i, 0, 4)
		C[tot].a[i] = 5 - i;
	C[tot].a[4] = 14;
	tot++;
	sort(C, C + tot);
	/*
	For(i, 0, tot - 1)
	{
		For(j, 0, 4)
			cout << C[i].a[j] << ' ';
		puts("");
	}
	*/
	int n = 1599;
	for (int i = 0; i < tot; i++)
	{
		int x = 0;
		for (int j = 0; j < 5; j++)
			x += 1 << (C[i].a[j] - 2);
		if (x == 4111 && i != 9)
			continue;
		if (i < 10)
			printf("%d %d\n", n + 1, x);
		else
			printf("%d %d\n", n + 1 + 4576, x);
		n++;
	}
}
void getType()
{
	//4-type
	int tot = 0;
	for (int i = 14; i >= 2; i--)
		for (int j = 14; j >= 2; j--)
		{
			if (i == j)
				continue;
			int x = 1;
			rep(k, 4)
				x *= prime[i];
			x *= prime[j];
			printf("%d %d\n", tot + 11, x);
			tot++;
		}
	//3+2
	tot = 0;
	for (int i = 14; i >= 2; i--)
		for (int j = 14; j >= 2; j--)
		{
			if (i == j)
				continue;
			int x = 1;
			rep(k, 3)
				x *= prime[i];
			x *= prime[j] * prime[j];
			printf("%d %d\n", tot + 167, x);
			tot++;
		}
	//3+1+1
	tot = 0;
	for (int i = 14; i >= 2; i--)
		for (int j = 14; j >= 2; j--)
		{
			if (i == j)
				continue;
			for (int k = j - 1; k >= 2; k--)
			{
				if (k == i)
					continue;
				int x = 1;
				rep(l, 3)
					x *= prime[i];
				x *= prime[j] * prime[k];
				printf("%d %d\n", tot + 1610, x);
				tot++;
			}
		}
	//2+2+1
	tot = 0;
	for (int i = 14; i >= 2; i--)
		for (int j = i - 1; j >= 2; j--)
		{
			for (int k = 14; k >= 2; k--)
			{
				if (k == i || k == j)
					continue;
				int x = 1;
				x *= prime[i] * prime[i];
				x *= prime[j] * prime[j];
				x *= prime[k];
				printf("%d %d\n", tot + 2468, x);
				tot++;
			}
		}
	//2+1+1+1
	tot = 0;
	for (int i = 14; i >= 2; i--)
		for (int j = 14; j >= 2; j--)
		{
			if (j == i)
				continue;
			for (int k = j - 1; k >= 2; k--)
			{
				if (k == i)
					continue;
				for (int l = k - 1; l >= 2; l--)
				{
					if (l == i)
						continue;
					int x;
					x = prime[i] * prime[i] * prime[j] * prime[k] * prime[l];
					printf("%d %d\n", tot + 3326, x);
					tot++;
				}
			}
		}

}
int main()
{
	freopen("othertype.out", "w", stdout);
	//getFlushes();
	getType();
	return 0;
}
