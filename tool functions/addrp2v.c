#include <stdio.h>
#include <math.h>

int main(void) {
    puts("Page size(base10): ..KB");
    int pageSize, offsetBitsNo;
    scanf("%d", &pageSize);
    pageSize *= 1024;

    offsetBitsNo = (int)log2(pageSize);

    puts("Physical address(base10):");
    long pAddr;
    scanf("%ld", &pAddr);

    long frameNo, offset, pageNo;
    frameNo = (long)(pAddr / pageSize);
    offset = pAddr % pageSize;

    printf("Page: ? --> Frame: %ld\n", frameNo);
    scanf("%ld", &pageNo);

    long pageBits = pageNo << offsetBitsNo;
    long vAddr = pageBits ^ offset;

    printf("Virtual address: %ld\n", vAddr);
}

