#include<regex.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define DEBUG 0
#define MAX_FILE_NAME 100
#define MAX_LINE 100

const char testFilePrefix[] = "test/";

struct Pair {
    int a;
    int b;
};

struct PairNode {
    struct Pair pair;
    PairNode *next;
};

void PrintPairNode(PairNode* p){
    PairNode* current = p;
    int item = 1;

    printf("PairNode\n item\tfirst\t second\n");
    while(current != NULL){
        printf("%d\t%d\t%d\n", item, current->pair.a, current->pair.b);
        current = current->next;
        item++;
    }
}

PairNode * AddNode(PairNode *current, int a, int b) {
    PairNode* next = (PairNode*) malloc(sizeof(PairNode));

    next->pair.a = a;
    next->pair.b = b;
    next->next = NULL;

    if (current == NULL){
        return next;
    }

    current->next = next;
    return next;
}

PairNode *LoadTestCaseInputData(char filePath[])
{
    FILE *testCaseFile = fopen(filePath, "r");
    if (testCaseFile == NULL) {
        printf("no such file.\n");
        return NULL;
    }

    char * line = NULL;
    size_t len = 0;

    int lineNumber = 0;

    PairNode * first;
    PairNode * current;

    bool firstRun = true;

    while(getline(&line, &len, testCaseFile) != -1){
        char regexString[] = "mul\\(([0-9]+),([0-9]+)\\)";
        size_t maxGroups = 3;
        size_t maxMatches = 10000;

        regex_t regexCompiled;
        regmatch_t groupArray[maxGroups];
        unsigned int m;
        char * cursor;

        if (regcomp(&regexCompiled, regexString, REG_EXTENDED))
        {
            printf("Could not compile regular expression.\n");
            return NULL;
        };

        m = 0;
        cursor = line;
        for (m = 0; m < maxMatches; m ++)
        {
            if (regexec(&regexCompiled, cursor, maxGroups, groupArray, 0)){
                break;  // No more matches
            }

            // Copy the line, but put a string terminator in at the match end index
            // for match 1, which corresponds to the first match.
            char cursorCopy1[strlen(cursor) + 1];
            strcpy(cursorCopy1, cursor);
            cursorCopy1[groupArray[1].rm_eo] = 0;
            
            // Copy the line, but put a string terminator in at the match end index
            // for match 2, which corresponds to the second match.
            char cursorCopy2[strlen(cursor) + 1];
            strcpy(cursorCopy2, cursor);
            cursorCopy2[groupArray[2].rm_eo] = 0;

            // Convert the values to ints
            int a = atoi(cursorCopy1 + groupArray[1].rm_so);
            int b = atoi(cursorCopy2 + groupArray[2].rm_so);

            if(firstRun) {
                first = AddNode(NULL, a, b);
                current = first;
                firstRun = false;
            } else {
                current = AddNode(current, a, b);
            }
            
            cursor += groupArray[0].rm_eo;
        }

        regfree(&regexCompiled); 
    }

    if (line) {
        free(line);
    }
    fclose(testCaseFile);

    return first;
}

struct TestResults{
    long part1;
    long part2;
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
    fscanf(testCaseFile, "%ld\n", &testInfo->expectedValues.part1);
    fscanf(testCaseFile, "%ld\n", &testInfo->expectedValues.part2);

    fclose(testCaseFile); 
}

void constructTestFilePath(char fileName[], char path[]) {
    strcat(path, testFilePrefix);
    strcat(path, fileName);  
}

void checkResults(TestInfo testInfo, TestResults expectedValues) {    
    if (testInfo.expectedValues.part1 != expectedValues.part1) {
        printf("%s FAILED. part1: Expected %ld, got %ld\n\n", testInfo.testCaseDataFileName, testInfo.expectedValues.part1, expectedValues.part1);
    }

    if (testInfo.expectedValues.part2 != expectedValues.part2) {
        printf("%s FAILED. part2: Expected %ld, got %ld\n\n", testInfo.testCaseDataFileName, testInfo.expectedValues.part2, expectedValues.part2);
    }
}

void PrintTestCase(char filePath[], int part1, int part2)
{
    printf("Test Case:\t%s\n", filePath);
    printf("Part 1:\t%d\n", part1);
    printf("Part 2:\t%d\n", part2);
}

long CalcTotal(PairNode* p){
    PairNode* current = p;
    long acc = 0;

    while(current != NULL){
        acc += current->pair.a * current->pair.b;
        current = current->next;
    }
    return acc;
}

void FreePairNode(PairNode* p){
    PairNode* next = p->next;

    if( next == NULL )
    {
        return;
    }

    FreePairNode(next);
    free(p);
}

int main()
{
    char testCaseInfoFileNames[][MAX_FILE_NAME] = {
        "testCase1.txt",
        "testCase2.txt",
        "testCase3.txt",
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

        PairNode* first = LoadTestCaseInputData(testCaseDataFilePath);

        if (DEBUG) {
            PrintPairNode(first);
        }


        TestResults results;

        // Calculate results
        results.part1 = CalcTotal(first);
        results.part2 = 0;

        // Debug logging for troubleshooting
        if (DEBUG) {
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

        FreePairNode(first);
        
    }
    return 0;
}