
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#define max(x, y) (((x) < (y)) ? (y) : (x))
#define min(x, y) (((x) > (y)) ? (y) : (x))
#define abs(x) (((x) < 0) ? (-(x)) : (x))
#define N_MAX 200000
#define DEBUG 0

void freope(){
//freopen("input.txt", "r", stdin);
}

typedef struct _colored
{
	bool red;
	int coord;
} colored;

typedef colored sortType;

int compare(sortType *x, sortType *y)
{
	if ((x->coord) < (y->coord))
	{
		return -1;
	}
	if ((x->coord) > (y->coord))
	{
		return 1;
	}
	return 0;
}

void swap(sortType *x, sortType *y)
{
	sortType tmp = *x;
	*x = *y;
	*y = tmp;
}

sortType __aux[N_MAX];

void mergeSort(sortType *arr, int begin, int end)
{
	if (begin + 1 >= end)
		return;
	if (begin < 0)
		return;

	if (begin + 2 == end)
	{
		if (compare(arr + begin, arr + begin + 1) > 0)
		{
			swap(arr + begin, arr + begin + 1);
		}
		return;
	}

	int mid = (begin + end) >> 1;
	mergeSort(arr, begin, mid);
	mergeSort(arr, mid, end);

	int i;
	int j1 = begin;
	int j2 = mid;
	for (i = begin; i < end; i++)
	{
		if (j1 >= mid)
		{
			__aux[i] = arr[j2];
			j2++;
		}
		else if (j2 >= end)
		{
			__aux[i] = arr[j1];
			j1++;
		}
		else if (compare(arr + j1, arr + j2) <= 0)
		{
			__aux[i] = arr[j1];
			j1++;
		}
		else
		{
			__aux[i] = arr[j2];
			j2++;
		}
	}

	for (i = begin; i < end; i++)
	{
		arr[i] = __aux[i];
	}
}

colored houses[N_MAX];
colored shops[N_MAX];
int permutation[N_MAX];

long long evalPermutation(int index)
{
	int hc = houses[index].coord;
	int sc = shops[permutation[index]].coord;
	long long int d = ((long long int)hc) - sc;
	d = abs(d);
	return d;
}

void rotatePermutation(int begin, int end)
{
	int e = permutation[end - 1];
	int i;
	for (i = end - 1; i > begin; i--)
	{
		permutation[i] = permutation[i - 1];
	}
	permutation[begin] = e;
}

long long int solveAfterSort(int N, int nRedHouse, int nBlueHouse, int nBlueShop, int nRedShop)
{
	int i;

	int firstBlueHouse = 0;
	while (houses[firstBlueHouse].red)
	{
		firstBlueHouse++;
	}

	if (DEBUG)
	{
		printf("firstBlueHouse: %d\n", firstBlueHouse);
	}

	houses[firstBlueHouse].red = true;

	int blueShopPointer = 0;
	int redShopPointer = 0;

	for (i = 0; i < N; i++)
	{
		if (houses[i].red)
		{
			while (!(shops[redShopPointer].red))
			{
				redShopPointer++;
			}
			permutation[i] = redShopPointer;
			redShopPointer++;
		}
		else
		{
			while (shops[blueShopPointer].red)
			{
				blueShopPointer++;
			}
			permutation[i] = blueShopPointer;
			blueShopPointer++;
		}
	}

	long long int answer = 0;

	for (i = 0; i < N; i++)
	{
		answer += evalPermutation(i);
	}

	if (DEBUG)
	{
		printf("initial permutation: ");
		for (i = 0; i < N; i++)
		{
			printf("%2d ", permutation[i]);
		}
		puts("");
		printf("initial answer: %lld\n", answer);
	}

	int blueHousePointer = firstBlueHouse;
	long long finalAnswer = answer;

	while (1)
	{
		int nextBlueHouse = blueHousePointer + 1;

		while (1)
		{
			if (nextBlueHouse < N)
			{
				if (houses[nextBlueHouse].red)
				{
					nextBlueHouse++;
				}
				else
				{
					break;
				}
			}
			else
			{
				nextBlueHouse = -9;
				break;
			}
		}

		// nextBlueHouse is -9 or valid index.

		if (nextBlueHouse == -9)
		{
			break;
		}

		for (i = blueHousePointer; i <= nextBlueHouse; i++)
		{
			answer -= evalPermutation(i);
		}

		rotatePermutation(blueHousePointer, nextBlueHouse + 1);

		for (i = blueHousePointer; i <= nextBlueHouse; i++)
		{
			answer += evalPermutation(i);
		}

		blueHousePointer = nextBlueHouse;
		finalAnswer = min(finalAnswer, answer);

		if (DEBUG)
		{
			printf("current permutation: ");
			for (i = 0; i < N; i++)
			{
				printf("%2d ", permutation[i]);
			}
			puts("");
		}
	}

	return finalAnswer;
}

long long int solve()
{
	int N, R, i;

	if (DEBUG)
	{
		printf("begin input\n");
	}

	scanf("%d%d", &N, &R);
	int nBlueHouse = N - R;
	int nBlueShop = N - R - 1;
	int nRedShop = R + 1;

	for (i = 0; i < nBlueHouse; i++)
	{
		scanf("%d", &(houses[i].coord));
		houses[i].red = false;
	}

	for (i = 0; i < R; i++)
	{
		scanf("%d", &(houses[nBlueHouse + i].coord));
		houses[nBlueHouse + i].red = true;
	}

	for (i = 0; i < nBlueShop; i++)
	{
		scanf("%d", &(shops[i].coord));
		shops[i].red = false;
	}

	for (i = 0; i < nRedShop; i++)
	{
		scanf("%d", &(shops[nBlueShop + i].coord));
		shops[nBlueShop + i].red = true;
	}

	if (DEBUG)
	{
		printf("end input\n");
	}

	if (DEBUG)
	{
		puts("begin sort");
	}

	mergeSort(houses, 0, N);
	mergeSort(shops, 0, N);

	if (DEBUG)
	{
		puts("end sort");

		for (i = 0; i < N; i++)
		{
			printf("%s%d ", ((houses[i].red) ? ("R") : ("B")), houses[i].coord);
		}
		puts("");
		for (i = 0; i < N; i++)
		{
			printf("%s%d ", ((shops[i].red) ? ("R") : ("B")), shops[i].coord);
		}
		puts("");
	}

	return solveAfterSort(N, R, nBlueHouse, nBlueShop, nRedShop);
}

int main(void)
{
	int T, test_case;
	if (DEBUG)
	{
		printf("-----DEBUG-----\n");
	}
	freope();
	setbuf(stdout, NULL);
	scanf("%d", &T);
	for (test_case = 0; test_case < T; test_case++)
	{
		printf("Case #%d\n", test_case + 1);
		long long int Answer = solve();
		printf("%lld\n", Answer);
	}
	return 0;
}
