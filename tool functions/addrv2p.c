#include <stdio.h>
#include <math.h>

int main(void) {
    puts("Page size(base10): ..KB");
    int pageSize, offsetBitsNo;
    scanf("%d", &pageSize);
    pageSize *= 1024;

    offsetBitsNo = (int)log2(pageSize);

    while (1) {
        puts("Virtual address(base10):");
        long vAddr;
        scanf("%ld", &vAddr);
        long pageNo, frameNo;
        pageNo = vAddr >> offsetBitsNo;
        printf("Page: %ld --> Frame: ?\n", pageNo);
        scanf("%ld", &frameNo);
        long mask, offset, pAddr;
        mask = (long)(pow(2, offsetBitsNo) - 1);
        offset = vAddr & mask;
        pAddr = frameNo * pageSize + offset;
        printf("Physical address: %ld\n", pAddr);
    }
}

