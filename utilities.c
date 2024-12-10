#include<regex.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"utilities.h"

void GetTestInfo(TestInfo *testInfo, char testCaseInfoFilePath[])
{
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

void FreeStringNode(StringNode* p){
    StringNode* next = p->next;

    if( next == NULL )
    {
        return;
    }

    FreeStringNode(next);
    free(p);
}

void PrintStringNode(StringNode* p){
    StringNode* current = p;
    int item = 1;

    printf("StringNode\n");
    while(current != NULL){
        printf("%d\t%s\n", item, current->string);
        current = current->next;
        item++;
    }
}

StringNode * AddStringNode(StringNode *current, char string[]) {
    struct StringNode* next = (StringNode*) malloc(sizeof(*next) + sizeof(char) * (strlen(string) + 1));
    if (next == NULL) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }

    next->len = strlen(string) + 1;
    next->next = NULL;
    strcpy(next->string, string);

    if (current == NULL){
        return next;
    }

    current->next = next;
    return next;
}

// Match returns a StringNode, representing the matches of each string in
// the supplied StringNode, when the regex string supplied is applied to it.
StringNode *Match(StringNode * stringNode, char regexString[])
{
    StringNode *readCurrent = stringNode;
    StringNode *first = NULL;
    StringNode *writeCurrent = NULL;

    int count = 0;
    do{
        size_t maxGroups = 30;
        size_t maxMatches = 10000;

        regex_t regexCompiled;
        regmatch_t groupArray[maxGroups];
        unsigned int m;
        char * cursor;

        int err = regcomp(&regexCompiled, regexString, REG_EXTENDED);

        if (err)
        {
            printf("Could not compile regular expression. %d\n", err);
            return NULL;
        };

        m = 0;
        cursor = readCurrent->string;
        for (m = 0; m < maxMatches; m ++)
        {
            if (regexec(&regexCompiled, cursor, maxGroups, groupArray, 0)) {
                break;  // No more matches
            }

            unsigned int start = 1;
            unsigned int g = start;
            unsigned int offset = 0;
            for (g = start; g < maxGroups; g++)
            {
                if (groupArray[g].rm_so == (size_t)-1) {
                    break;  // No more groups    
                }

                if (g == start) {
                    offset = groupArray[g].rm_eo;    
                }      

                char cursorCopy[strlen(cursor) + 1];
                strcpy(cursorCopy, cursor);
                cursorCopy[groupArray[g].rm_eo] = 0;

                writeCurrent = AddStringNode(writeCurrent, cursorCopy + groupArray[g].rm_so);

                if (first == NULL) {
                    first = writeCurrent;
                }
            }
            cursor += offset;
        } 
        
        readCurrent = readCurrent->next;
        regfree(&regexCompiled); 
    } while(readCurrent != NULL);
    
    return first;
}

const char testFilePrefix[] = "test/";

// LoadTestCaseInputData Loads the data in filePath into a 
// StringNode linked list, with each line occupying a separate node
StringNode *LoadTestCaseInputData(char filePath[])
{
    FILE *testCaseFile = fopen(filePath, "r");
    if (testCaseFile == NULL) {
        printf("no such file.\n");
        return NULL;
    }

    char * line = NULL;
    size_t len = 0;

    size_t size = getline(&line, &len, testCaseFile);

    StringNode * first = AddStringNode(NULL, line);
    StringNode * current = first;

    while(getline(&line, &len, testCaseFile) != -1) {
        current = AddStringNode(current, line);
    } 

    if (line) {
        free(line);
    }
    fclose(testCaseFile);

    

    return first;
}

void constructTestFilePath(char fileName[], char path[])
{
    strcat(path, testFilePrefix);
    strcat(path, fileName);  
}

void checkResults(TestInfo testInfo, TestResults expectedValues)
{    
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

int StringNodeLen(StringNode *s)
{
    StringNode *current = s;
    int count = 0;

    while(current != NULL) {
        count++;
        current = current->next;
    }

    return count;
}
