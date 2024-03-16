#include <time.h>
#include <stdio.h>
#include <string.h>

int extraMemoryAllocated;

void *Alloc(size_t sz) {
    extraMemoryAllocated += sz;
    size_t* ret = (size_t*)malloc(sizeof(size_t) + sz);
    if (ret == NULL) {
        printf("Error: Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    *ret = sz;
    printf("Extra memory allocated, size: %ld\n", sz);
    return &ret[1];
}

void DeAlloc(void* ptr) {
    size_t* pSz = (size_t*)ptr - 1;
    extraMemoryAllocated -= *pSz;
    printf("Extra memory deallocated, size: %ld\n", *pSz);
    free((size_t*)ptr - 1);
}

size_t Size(void* ptr) {
    return *((size_t*)ptr - 1);
}

// Implement merge sort
void merge(int pData[], int l, int m, int r) {
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;

    // Create temporary arrays
    int* L = (int*)Alloc(sizeof(int) * n1);
    int* R = (int*)Alloc(sizeof(int) * n2);

    // Copy data to temporary arrays L[] and R[]
    for (i = 0; i < n1; i++)
        L[i] = pData[l + i];
    for (j = 0; j < n2; j++)
        R[j] = pData[m + 1 + j];

    // Merge the temporary arrays back into pData[l..r]
    i = 0; // Initial index of first subarray
    j = 0; // Initial index of second subarray
    k = l; // Initial index of merged subarray
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            pData[k] = L[i];
            i++;
        } else {
            pData[k] = R[j];
            j++;
        }
        k++;
    }

    // Copy the remaining elements of L[], if any
    while (i < n1) {
        pData[k] = L[i];
        i++;
        k++;
    }

    // Copy the remaining elements of R[], if any
    while (j < n2) {
        pData[k] = R[j];
        j++;
        k++;
    }

    // Deallocate temporary arrays
    DeAlloc(L);
    DeAlloc(R);
}

// Merge Sort function
void mergeSort(int pData[], int l, int r) {
    if (l < r) {
        // Same as (l+r)/2, but avoids overflow for large l and r
        int m = l + (r - l) / 2;

        // Sort first and second halves
        mergeSort(pData, l, m);
        mergeSort(pData, m + 1, r);

        // Merge the sorted halves
        merge(pData, l, m, r);
    }
}

// Parses input file to an integer array
int parseData(char* inputFileName, int** ppData) {
    FILE* inFile = fopen(inputFileName, "r");
    int dataSz = 0;
    int i, n, *data;
    *ppData = NULL;

    if (inFile) {
        fscanf(inFile, "%d\n", &dataSz);
        *ppData = (int*)Alloc(sizeof(int) * dataSz);
        if (*ppData == NULL) {
            printf("Error: Memory allocation failed\n");
            exit(EXIT_FAILURE);
        }
        for (i = 0; i < dataSz; ++i) {
            fscanf(inFile, "%d ", &n);
            data = *ppData + i;
            *data = n;
        }

        fclose(inFile);
    }

    return dataSz;
}

// Prints first and last 100 items in the data array
void printArray(int pData[], int dataSz) {
    int i, sz = dataSz - 100;
    printf("\tData:\n\t");
    for (i = 0; i < 100; ++i) {
        printf("%d ", pData[i]);
    }
    printf("\n\t");

    for (i = sz; i < dataSz; ++i) {
        printf("%d ", pData[i]);
    }
    printf("\n\n");
}

int main(void) {
    clock_t start, end;
    int i;
    double cpu_time_used;
    char* fileNames[] = { "input1.txt", "input2.txt", "input3.txt", "input4.txt" };

    for (i = 0; i < 4; ++i) {
        int* pDataSrc, * pDataCopy;
        int dataSz = parseData(fileNames[i], &pDataSrc);

        if (dataSz <= 0)
            continue;

        pDataCopy = (int*)Alloc(sizeof(int) * dataSz);

        printf("---------------------------\n");
        printf("Dataset Size : %d\n", dataSz);
        printf("---------------------------\n");

        printf("Merge Sort:\n");
        memcpy(pDataCopy, pDataSrc, dataSz * sizeof(int));
        extraMemoryAllocated = 0;
        start = clock();
        mergeSort(pDataCopy, 0, dataSz - 1);
        end = clock();
        cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
        printf("\truntime\t\t\t: %.1lf\n", cpu_time_used);
        printf("\textra memory allocated\t: %d\n", extraMemoryAllocated);
        printArray(pDataCopy, dataSz);

        DeAlloc(pDataCopy);
        DeAlloc(pDataSrc);
    }

    return 0;
}
