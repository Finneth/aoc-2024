#include<regex.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"../utilities.h"

#define DEBUG 0
#define MAX_LINE 100

void printStrings(char *strings[], int n)
{
    for (int i=0; i<n; i++) {
        printf("String %d: %s\n", i, strings[i]);
    }
}

int processWordSearch(char *strings[], int n, char find[])
{
    return 0;
}

int main()
{
    char testCaseInfoFileNames[][MAX_FILE_NAME] = {
        "testCase1.txt",
    };

    for (size_t i = 0; i < sizeof(testCaseInfoFileNames) / sizeof(testCaseInfoFileNames[0]); i++)
    {
        // Get basic test case info
        char testCaseInfoFilePath[MAX_FILE_NAME] = "";
        constructTestFilePath(testCaseInfoFileNames[i], testCaseInfoFilePath);

        struct TestInfo testInfo;
        GetTestInfo(&testInfo, testCaseInfoFilePath);

        // Load test case data
        char testCaseDataFilePath[MAX_FILE_NAME] = "";
        constructTestFilePath(testInfo.testCaseDataFileName, testCaseDataFilePath);

        StringNode* testCaseData = LoadTestCaseInputData(testCaseDataFilePath);

        int columns = testCaseData->len;
        int rows = StringNodeLen(testCaseData);

        char *wordSearch[rows];

        StringNode *current = testCaseData;

        for(int i = 0; i < rows; i++) {
            wordSearch[i] = current->string;
            current = current->next;
        }

        // Calculate results

        char find[] = "XMAS";
        TestResults results;
        results.part1 = processWordSearch(wordSearch, rows, find);
        results.part2 = 0;

        // Debug logging for troubleshooting
        if (DEBUG) {
            printStrings(wordSearch, rows);
            PrintTestCase(testCaseDataFilePath, results.part1, results.part2);    
            if (testInfo.expectedValues.part1 == results.part1) {
                printf("%s SUCCESS. part1: Expected %ld, got %ld\n\n", testInfo.testCaseDataFileName, testInfo.expectedValues.part1, results.part1);
            }
            if (testInfo.expectedValues.part2 == results.part2) {
                printf("%s SUCCESS. part2: Expected %ld, got %ld\n\n", testInfo.testCaseDataFileName, testInfo.expectedValues.part2, results.part2);
            }
        }
        
        // Assert against expections
        checkResults(testInfo, results);
    }
    return 0;
}