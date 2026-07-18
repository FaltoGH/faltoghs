#include <stdio.h>

int main(int argc, char **argv)
{
    int result = 0;
    scanf("%d", &result);
    while (1)
    {
        char c;
        scanf("%c", &c);
        while (c == ' ')
        {
            scanf("%c", &c);
        }

        if(c == ';'){
            break;
        }

        int b;
        scanf("%d", &b);
        result = result + b;
    }

    printf("%d", result);
    return 0;
}
