#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define DEBUG 0
#define MAX_FILE_NAME 100
#define MAX_LINE 100

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

struct List {
    int len;
    int* arr;
};

void PrintList(List lists[], int len) {
    for (int i = 0; i < len; i++)
    {
        List list = lists[i];

        for (int j = 0; j < list.len; j++)
        {
            printf("%d ", list.arr[j]);
        }
        printf("\n"); 
    }
}

void LoadTestCaseInputData(char filePath[], int lineItemCount, List reports[])
{
    FILE *testCaseFile = fopen(filePath, "r");
    if (testCaseFile == NULL) {
        printf("no such file.\n");
        return;
    }

    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    char delimiter[] = " ";
    int lineNumber = 0;

    while ((read = getline(&line, &len, testCaseFile)) != -1) {
        int report[MAX_LINE];
        int numItems = 0;
        
        char* token = strtok(line, delimiter);
        while (token != NULL) {
            int num;
            sscanf(token, "%d", &num);

            report[numItems] = num;
            token = strtok(NULL, delimiter);
            numItems++;
        }

        // Set the length this report in the resulting reports
        reports[lineNumber].len = numItems;

        // Allocate memory in the report
        reports[lineNumber].arr = (int*)malloc(numItems);

        // Use memcpy to copy report to reports[lineNumber].arr
        int n = sizeof(report) / sizeof(report[0]);
        memcpy(reports[lineNumber].arr, report, n * sizeof(report[0]));

        lineNumber++;
    }
    
    fclose(testCaseFile);
    if (line) {
        free(line);
    }
    
    if (DEBUG) {
        PrintList(reports, lineNumber);
    }
}

void PrintTestCase(char filePath[], int safeReports)
{
    printf("Test Case:\t%s\n", filePath);
    printf("Safe Reports:\t%d\n", safeReports);
}

int compare(const void* a, const void* b) {
   return (*(int*)a - *(int*)b);
}

int CountSafeReports(int lineItemCount, List reports[]) {
    return 0;
}

bool IsReportSafe(List report) {
    qsort(report.arr, report.len, sizeof(int), compare);
    return true;
}

struct TestResults{
    int safeReports;
};

struct TestInfo {
    char testCaseDataFileName[MAX_FILE_NAME];
    TestResults expectedValues;
};

void GetTestInfo(TestInfo *testInfo, char testCaseInfoFilePath[]) {
    FILE *testCaseFile = fopen(testCaseInfoFilePath, "r");
    if (testCaseFile == NULL) {
        printf("no such file.\n");
        return;
    }
    
    fscanf(testCaseFile, "%s\n", testInfo->testCaseDataFileName);
    fscanf(testCaseFile, "%d\n", &testInfo->expectedValues.safeReports);

    fclose(testCaseFile); 
}

void constructTestFilePath(char fileName[], char path[]) {
    strcat(path, testFilePrefix);
    strcat(path, fileName);  
}

void checkResults(int lineItemCount, TestInfo testInfo, TestResults expectedValues) {    
    if (testInfo.expectedValues.safeReports != expectedValues.safeReports) {
        printf("%s FAILED. Expected %d, got %d\n\n", testInfo.testCaseDataFileName, testInfo.expectedValues.safeReports, expectedValues.safeReports);
    }
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

        int lineItemCount = CountFileLines(testCaseDataFilePath);

        List reports[lineItemCount];

        LoadTestCaseInputData(testCaseDataFilePath, lineItemCount, reports);

        TestResults results;

        // Calculate results
        results.safeReports = CountSafeReports(lineItemCount, reports);

        // Debug logging for troubleshooting
        if (DEBUG) {
            PrintTestCase(testCaseDataFilePath, results.safeReports);    
            if (testInfo.expectedValues.safeReports == results.safeReports) {
                printf("%s SUCCESS. Expected %d, got %d\n\n", testInfo.testCaseDataFileName, testInfo.expectedValues.safeReports, results.safeReports);
            }
        }
        
        // Assert against expections
        checkResults(lineItemCount, testInfo, results);
        
    }
    return 0;
}