#include<regex.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"../utilities.h"

#define DEBUG 0

#define MAX_LINE 100

int handleInstructionsPart1(StringNode *instructions) {
    char doString[] = "do()";
    char dontString[] = "don't()";
    int acc = 0;
    StringNode *current = instructions;
    int a;
    int b;

    int count = 0;

    do{
        count++;

        if(strcmp(current->string, doString) == 0){
            current = current->next;
            continue;
        }

        if(strcmp(current->string, dontString) == 0){
            current = current->next;
            continue;
        }

        int res = sscanf(current->string, "mul(%d,%d)\n", &a, &b);

        if(res == 2) {
            acc += a*b;
            current = current->next;
            continue;
        } 
        
        return 0;
        
    } while(current != NULL);

    return acc;
}

int handleInstructionsPart2(StringNode *instructions) {
    char doString[] = "do()";
    char dontString[] = "don't()";
    int acc = 0;
    StringNode *current = instructions;
    bool doAddition = true;
    int a;
    int b;

    do{
        if(strcmp(current->string, doString) == 0){
            doAddition = true;
            current = current->next;
            continue;
        }

        if(strcmp(current->string, dontString) == 0){
            doAddition = false;
            current = current->next;
            continue;
        }

        int res = sscanf(current->string, "mul(%d,%d)\n", &a, &b);
        if(res == 2) {
            if (doAddition) {

                acc += a*b;    
            }
            current = current->next;
            continue;
        } 

        return 0;
        
    } while(current != NULL);

    return acc;
}

int main()
{
    char testCaseInfoFileNames[][MAX_FILE_NAME] = {
        "testCase1.txt",
        "testCase2.txt",
        "testCase3.txt",
        "testCase5.txt",
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

        char instructionRegex[] = "(mul\\([0-9]+,[0-9]+\\)|do\\(\\)|don't\\(\\))";
        StringNode* instructions = Match(testCaseData, instructionRegex);
    
        // Calculate results
        TestResults results;
        results.part1 = handleInstructionsPart1(instructions);
        results.part2 = handleInstructionsPart2(instructions);

        // Debug logging for troubleshooting
        if (DEBUG) {
            PrintStringNode(instructions);
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

        FreeStringNode(testCaseData);
        FreeStringNode(instructions);
        
    }
    return 0;
}