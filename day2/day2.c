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
        reports[lineNumber].arr = (int*)malloc(sizeof(report));

        // Copy report to reports[lineNumber].arr
        memcpy(reports[lineNumber].arr, report, sizeof(report));

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

void PrintTestCase(char filePath[], int safeReports, int safeReportsDampened)
{
    printf("Test Case:\t%s\n", filePath);
    printf("Safe Reports:\t%d\n", safeReports);
    printf("Safe Reports Dampened:\t%d\n", safeReportsDampened);
}

const int safeChangeLowerBound = 1;
const int safeChangeUpperBound = 3;

bool withinTolerance(int val1, int val2) {
    int delta = abs(val1 - val2);
    return delta >= safeChangeLowerBound && delta <= safeChangeUpperBound;
}

bool IsReportSafe(List report) {
    if( report.len < 2 ) {
        return true;
    }

    if( report.len >= 2 && !withinTolerance(report.arr[0], report.arr[1])) {
        return false;
    }

    // Decide if this sequence is increasing or decreasing
    bool isIncreasing = report.arr[0] < report.arr[1];

    for(int i = 2; i < report.len; i++)
    {
        int last = report.arr[i-1];
        int current = report.arr[i];

        if (isIncreasing != last < current) {
            // This report is not safe because this pair is not going in the same
            // direction as the initial pair.
            return false;
        }

        if (!withinTolerance(last, current)) {
            // This report is not safe because the difference in this pair
            // is outside of the tolerance range.
            return false;
        }
    }

    return true;
}

bool IsReportSafeWithDampener(List report) {
    // Check if the report is safe without the damening effect
    if(IsReportSafe(report)){
        return true;
    }

    // Create a new report with one value missing, and test that instead.
    for( int exc = 0; exc < report.len; exc++ )
    {
        List reducedReport;
        reducedReport.len = report.len-1;

        // Allocate memory in the reduced report (1 entry less than report)
        reducedReport.arr = (int*)malloc(sizeof(report)-sizeof(int));

        // Populate the reducedReport
        int idx = 0;
        for( int j = 0; j < report.len; j++ )
        {
            if( j != exc ) {
                // Add the jth value from the report to the next slot
                // in the reduced report idx, as this is not the excluded
                // index.
                reducedReport.arr[idx] = report.arr[j];
                // Keep track of which index of the reduced array we are inserting our value into
                idx++;
            }
            
        }

        if(IsReportSafe(reducedReport)){
            // We found a reduction of the report which is true,
            // no need to carry on.
            return true;
        }
    }

    // No safe reduction could be found
    return false;
}

int CountSafeReports(int lineItemCount, List reports[]) {
    int safeCount = 0;
    for (int i = 0; i < lineItemCount; i++ )
    {
        List report = reports[i];

        if(IsReportSafe(report)) {
            safeCount++;
        }
    }

    return safeCount;
}

int CountSafeReportsDampened(int lineItemCount, List reports[]) {
    int safeCount = 0;
    for (int i = 0; i < lineItemCount; i++ )
    {
        List report = reports[i];

        if(IsReportSafeWithDampener(report)) {
            safeCount++;
        }
    }

    return safeCount;
}

struct TestResults{
    int safeReports;
    int safeReportsDampened;
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
    fscanf(testCaseFile, "%d\n", &testInfo->expectedValues.safeReportsDampened);

    fclose(testCaseFile); 
}

void constructTestFilePath(char fileName[], char path[]) {
    strcat(path, testFilePrefix);
    strcat(path, fileName);  
}

void checkResults(int lineItemCount, TestInfo testInfo, TestResults expectedValues) {    
    if (testInfo.expectedValues.safeReports != expectedValues.safeReports) {
        printf("%s FAILED. safeReports: Expected %d, got %d\n\n", testInfo.testCaseDataFileName, testInfo.expectedValues.safeReports, expectedValues.safeReports);
    }

    if (testInfo.expectedValues.safeReportsDampened != expectedValues.safeReportsDampened) {
        printf("%s FAILED. safeReportsDampened: Expected %d, got %d\n\n", testInfo.testCaseDataFileName, testInfo.expectedValues.safeReportsDampened, expectedValues.safeReportsDampened);
    }
}

int main()
{
    char testCaseInfoFileNames[][MAX_FILE_NAME] = {
        "testCase1.txt",
        "testCase2.txt",
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
        results.safeReportsDampened = CountSafeReportsDampened(lineItemCount, reports);

        // Debug logging for troubleshooting
        if (DEBUG) {
            PrintTestCase(testCaseDataFilePath, results.safeReports, results.safeReportsDampened);    
            if (testInfo.expectedValues.safeReports == results.safeReports) {
                printf("%s SUCCESS. safeReports: Expected %d, got %d\n\n", testInfo.testCaseDataFileName, testInfo.expectedValues.safeReports, results.safeReports);
            }
            if (testInfo.expectedValues.safeReportsDampened == results.safeReportsDampened) {
                printf("%s SUCCESS. safeReportsDampened: Expected %d, got %d\n\n", testInfo.testCaseDataFileName, testInfo.expectedValues.safeReportsDampened, results.safeReportsDampened);
            }
        }
        
        // Assert against expections
        checkResults(lineItemCount, testInfo, results);
        
    }
    return 0;
}