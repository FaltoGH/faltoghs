
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#define min(x, y) (((x) > (y)) ? (y) : (x))
#define PRIME (1000000007)

long long int Answer;
int main(void)
{
	long long int threeToThePowerOf[100001];
	threeToThePowerOf[0] = 1;
	int i;
	for(i=1;i<100001;i++){
		threeToThePowerOf[i]=(threeToThePowerOf[i-1]*3)%PRIME;
	}

	int T, test_case;

	setbuf(stdout, NULL);
	scanf("%d", &T);
	for (test_case = 0; test_case < T; test_case++)
	{
		Answer = 1;
		char N[100002];
		scanf("%s", &N);
		size_t len=strlen(N);
		
		for(i=len-1;i>=0;i--){
			int seat = len - i - 1;

			if(N[i]=='0'){
				Answer = Answer + 0 + 0;
			}
			else if(N[i]=='1'){
				Answer = threeToThePowerOf[seat] +  Answer + 0;
			}
			else if(N[i]=='2'){
				Answer = threeToThePowerOf[seat] + threeToThePowerOf[seat] + Answer;
			}
			else if(N[i]>='3' && N[i]<='9'){
				Answer = threeToThePowerOf[seat] + threeToThePowerOf[seat] + threeToThePowerOf[seat];
			}
			else{
				assert(false);
			}

			Answer = Answer % PRIME;
		}

		Answer--;//MUST exclude 0

		if(Answer == -1){
			Answer += PRIME;
		}

		printf("Case #%d\n", test_case + 1);
		printf("%lld\n", Answer);
	}
	return 0;
}
