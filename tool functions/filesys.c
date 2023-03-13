
#include <stdio.h>

int main(void) {
    unsigned blockSize, blockPtSize, blockSizeByts;
    puts("block size ? KiB");
    scanf("%u", &blockSize);
    blockSizeByts = blockSize * 1024;
    puts("block pointers size ? bytes");
    scanf("%u", &blockPtSize);

    unsigned dirPt, indirPt, dbIndirPt, tpIndirPt;
    puts("i-node has ? direct pointers");
    scanf("%u", &dirPt);
    puts("i-node has ? indirect pointers");
    scanf("%u", &indirPt);
    puts("i-node has ? double indirect pointers");
    scanf("%u", &dbIndirPt);
    puts("i-node has ? triple indirect pointers");
    scanf("%u", &tpIndirPt);

    unsigned indirBlksNums = blockSizeByts / blockPtSize;
    unsigned long fstRead, scdRead, trdRead;
    unsigned long fouRead, sum;
    fstRead = dirPt * blockSize;
    scdRead = indirPt * indirBlksNums * blockSize;
    trdRead = dbIndirPt * indirBlksNums * indirBlksNums * blockSize;
    fouRead = tpIndirPt * indirBlksNums * indirBlksNums * indirBlksNums * blockSize;
    sum = fstRead + scdRead + trdRead + fouRead;

    printf("1st read sum: %lu KiB\n", fstRead);
    printf("2nd read sum: %lu + %lu = %lu KiB\n", fstRead, scdRead, fstRead + scdRead);
    printf("3rd read sum: %lu + %lu + %lu = %lu KiB\n",
            fstRead, scdRead, trdRead, fstRead + scdRead + trdRead);

    if (tpIndirPt) {
        printf("4th read sum: %lu + %lu + %lu + %lu = %lu KiB\n",
               fstRead, scdRead, trdRead, fouRead, fstRead + scdRead + trdRead + fouRead);
    }

    printf("Max file size: %lu KiB\n", sum);
}

