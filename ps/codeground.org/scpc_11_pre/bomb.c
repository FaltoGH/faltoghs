#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#define min(x, y) (((x) > (y)) ? (y) : (x))

long long int Answer;
int main(void)
{
	int T, test_case;
	// freopen("input.txt", "r", stdin);
	setbuf(stdout, NULL);
	scanf("%d", &T);
	for (test_case = 0; test_case < T; test_case++)
	{
		Answer = 0;
		int N, L;
		scanf("%d%d", &N, &L);

		int x = 0;

		while (N--)
		{
			int y;
			scanf("%d", &y);
			if (x == 0)
			{
				Answer += y;
			}
			else
			{
				Answer += min(x + y, 2 * L - x - y);
			}
			x = y;
		}

		Answer += min(x, L - x);

		printf("Case #%d\n", test_case + 1);
		printf("%lld\n", Answer);
	}
	return 0;
}
