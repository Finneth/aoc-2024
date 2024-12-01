#include<stdio.h>
#define MAX_FILE_NAME 100

int CountFileLines(char filePath[])
{
    FILE *fp;
    int count = 0; // Line counter (result)
    char filename[MAX_FILE_NAME];
    char c; // To store a character read from file

    // Open the file
    fp = fopen(filePath, "r");

    // Check if file exists
    if (fp == NULL)
    {
        printf("Could not open file %s", filename);
        return 0;
    }

    // Extract characters from file and store in character c
    for (c = getc(fp); c != EOF; c = getc(fp))
        if (c == '\n') // Increment count if this character is newline
            count = count + 1;

    // Close the file
    fclose(fp);
    return count;
}

void ReadTestCase(char filePath[], int lineItemCount, int listA[], int listB[])
{
    FILE *ptr = fopen(filePath, "r");
    if (ptr == NULL) {
        printf("no such file.\n");
        return;
    }

    for (int i = 0; i < lineItemCount; i++) {
        fscanf(ptr, "%d   %d\n", &listA[i], &listB[i]);
    }

    fclose(ptr);
}

void PrintTestCase(char filePath[], int lineItemCount, int listA[], int listB[], int result)
{
    printf("Test Case:\t%s\n", filePath);
    printf("listA\tlistB\n");
    for (int i = 0; i < lineItemCount; i++) {
        printf("%d\t%d\n", listA[i], listB[i]);
    }
    printf("Result:\t%d\n", result);
}

int CalculateResult(int lineItemCount, int listA[], int listB[]) {
    return 0;
}


int main()
{
    char testCaseFilefilePath[] = "test/testCase1_in.txt";
    int lineItemCount = CountFileLines(testCaseFilefilePath);

    int listA[lineItemCount];
    int listB[lineItemCount];
    
    ReadTestCase(testCaseFilefilePath, lineItemCount, listA, listB);
    int result = CalculateResult(lineItemCount, listA, listB);

    PrintTestCase(testCaseFilefilePath, lineItemCount, listA, listB, result);
}