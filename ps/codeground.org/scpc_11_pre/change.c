#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
int Answer;
int main(void)
{
	int T, test_case;
	// freopen("input.txt", "r", stdin);
	setbuf(stdout, NULL);
	scanf("%d", &T);
	for (test_case = 0; test_case < T; test_case++)
	{
		int N;
		scanf("%d", &N);
		int a500 = 0;
		int a1000 = 0;
		Answer = 0;
		bool scanonly = false;
		while (N--)
		{
			int money;
			scanf("%d", &money);
			if (scanonly)
			{
				continue;
			}

			if (money == 500)
			{
				a500++;
				Answer++;
			}
			else if (money == 1000)
			{
				if (a500 > 0)
				{
					a500--;
					a1000++;
					Answer++;
				}
				else
				{
					scanonly = true;
				}
			}
			else if (money == 5000)
			{
				if (a1000 >= 4 && a500 >= 1)
				{
					a1000 -= 4;
					a500 -= 1;
					Answer++;
				}
				else if (a1000 >= 3 && a500 >= 3)
				{
					a1000 -= 3;
					a500 -= 3;
					Answer++;
				}
				else if (a1000 >= 2 && a500 >= 5)
				{
					a1000 -= 2;
					a500 -= 5;
					Answer++;
				}
				else if (a1000 >= 1 && a500 >= 7)
				{
					a1000 -= 1;
					a500 -= 7;
					Answer++;
				}
				else if (a500 >= 9)
				{
					a500 -= 9;
					Answer++;
				}
				else
				{
					scanonly = true;
				}
			}
			else
			{
				assert(false);
			}
		}
		printf("Case #%d\n", test_case + 1);
		printf("%d\n", Answer);
	}
	return 0;
}
