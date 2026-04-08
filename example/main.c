#include "slog.h"

#include <stdio.h>

int main()
{
    const double golden = 1.6180;
    const double euler = 2.7183;
    slog("pi %f\n", golden);
    puts("mid");
    slog("e  %f\n", euler);
    flushlog();
}
