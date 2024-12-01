#include<stdio.h>
#include<string.h>
#define MAX_FILE_NAME 100


const char testFilePrefix[] = "test/";

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

void LoadTestCaseInputData(char filePath[], int lineItemCount, int listA[], int listB[])
{
    FILE *testCaseFile = fopen(filePath, "r");
    if (testCaseFile == NULL) {
        printf("no such file.\n");
        return;
    }

    for (int i = 0; i < lineItemCount; i++) {
        fscanf(testCaseFile, "%d   %d\n", &listA[i], &listB[i]);
    }

    fclose(testCaseFile);
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
    char testCaseInfoFileNames[][MAX_FILE_NAME] = {
        "testCase1.txt",
        "testCase2.txt"
    };

    for (size_t i = 0; i < sizeof(testCaseInfoFileNames) / sizeof(testCaseInfoFileNames[0]); i++)
    {
        char testCaseInfoFilePath[MAX_FILE_NAME] = "";
        strcat(testCaseInfoFilePath, testFilePrefix);
        strcat(testCaseInfoFilePath, testCaseInfoFileNames[i]);

        char dataFileName[MAX_FILE_NAME];
        int expectedResult;


        FILE *testCaseFile = fopen(testCaseInfoFilePath, "r");
        if (testCaseFile == NULL) {
            printf("no such file.\n");
            return 0;
        }

        fscanf(testCaseFile, "%s\n", dataFileName);
        fscanf(testCaseFile, "%d\n", &expectedResult);

        fclose(testCaseFile);

        char testCaseDataFilePath[MAX_FILE_NAME] = "";
        strcat(testCaseDataFilePath, testFilePrefix);
        strcat(testCaseDataFilePath, dataFileName);

        int lineItemCount = CountFileLines(testCaseDataFilePath);

        int listA[lineItemCount];
        int listB[lineItemCount];

        LoadTestCaseInputData(testCaseDataFilePath, lineItemCount, listA, listB);
        int result = CalculateResult(lineItemCount, listA, listB);

        PrintTestCase(testCaseDataFilePath, lineItemCount, listA, listB, result);
    }
    return 0;
}