// File: utilities.h

#ifndef UTILITIES_H
#define UTILITIES_H

#define MAX_FILE_NAME 100

// Structs declarations
struct StringNode {
    int len;
    StringNode * next;
    char string[];
};

struct TestResults{
    long part1;
    long part2;
};

struct TestInfo {
    char testCaseDataFileName[MAX_FILE_NAME];
    TestResults expectedValues;
};

// Function declarations
void GetTestInfo(TestInfo *testInfo, char testCaseInfoFilePath[]);
void FreeStringNode(StringNode* p);
void PrintStringNode(StringNode* p);
StringNode * AddStringNode(StringNode *current, char string[]);
StringNode *Match(StringNode * stringNode, char regexString[]);
StringNode *LoadTestCaseInputData(char filePath[]);
void constructTestFilePath(char fileName[], char path[]);
void checkResults(TestInfo testInfo, TestResults expectedValues);
void PrintTestCase(char filePath[], int part1, int part2);

#endif