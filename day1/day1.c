#include<stdio.h>
#include<stdlib.h>
#include<string.h>


#define DEBUG 0
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
    {
        if (c == '\n') // Increment count if this character is newline
        {
            count = count + 1;
        }
    }

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

void PrintTestCase(char filePath[], int lineItemCount, int listA[], int listB[], int distance, int similarity)
{
    printf("Test Case:\t%s\n", filePath);
    printf("listA\tlistB\n");
    for (int i = 0; i < lineItemCount; i++) {
        printf("%d\t%d\n", listA[i], listB[i]);
    }
    printf("Distance:\t%d\n", distance);
    printf("Similarity:\t%d\n", similarity);
}

int compare(const void* a, const void* b) {
   return (*(int*)a - *(int*)b);
}

int CalculateDistance(int lineItemCount, int listA[], int listB[]) {
    qsort(listA, lineItemCount, sizeof(int), compare);
    qsort(listB, lineItemCount, sizeof(int), compare);

    int distance = 0;

    for (int i = 0; i < lineItemCount; i++) {
        distance += abs(listA[i] - listB[i]);
    }

    return distance;
}

int CountInstances(int lineItemCount, int arr[], int value) {
    int instances = 0;

    for (int i = 0; i < lineItemCount; i++) {
        if (arr[i] == value)
        {
            instances++;
        }
    }

    return instances;
}

int CalculateSimilarity(int lineItemCount, int listA[], int listB[]) {
    qsort(listA, lineItemCount, sizeof(int), compare);
    qsort(listB, lineItemCount, sizeof(int), compare);

    int similarity = 0;

    for (int i = 0; i < lineItemCount; i++) {
        similarity += listA[i] * CountInstances(lineItemCount, listB, listA[i]);
    }

    return similarity;
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
        int expectedDistance;
        int expectedSimilarity;


        FILE *testCaseFile = fopen(testCaseInfoFilePath, "r");
        if (testCaseFile == NULL) {
            printf("no such file.\n");
            return 0;
        }

        fscanf(testCaseFile, "%s\n", dataFileName);
        fscanf(testCaseFile, "%d\n", &expectedDistance);
        fscanf(testCaseFile, "%d\n", &expectedSimilarity);

        fclose(testCaseFile);

        char testCaseDataFilePath[MAX_FILE_NAME] = "";
        strcat(testCaseDataFilePath, testFilePrefix);
        strcat(testCaseDataFilePath, dataFileName);

        int lineItemCount = CountFileLines(testCaseDataFilePath);

        int listA[lineItemCount];
        int listB[lineItemCount];

        LoadTestCaseInputData(testCaseDataFilePath, lineItemCount, listA, listB);

        int distance = CalculateDistance(lineItemCount, listA, listB);
        int similarity = CalculateSimilarity(lineItemCount, listA, listB);

        if (DEBUG) {
            PrintTestCase(testCaseDataFilePath, lineItemCount, listA, listB, distance, similarity);    
            if (expectedDistance == distance) {
                printf("%s SUCCESS. Expected %d, got %d\n\n", dataFileName, expectedDistance, distance);
            }
            if (expectedSimilarity == similarity) {
                printf("%s SUCCESS. Expected %d, got %d\n\n", dataFileName, expectedSimilarity, similarity);
            }
        }
        
        if (expectedDistance != distance) {
            printf("%s FAILED. Expected %d, got %d\n\n", dataFileName, expectedDistance, distance);
        }

        if (expectedSimilarity != similarity) {
            printf("%s FAILED. Expected %d, got %d\n\n", dataFileName, expectedSimilarity, similarity);
        }
        
    }
    return 0;
}