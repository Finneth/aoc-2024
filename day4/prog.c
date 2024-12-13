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

struct coordinateNode {
    int row;
    int column;
    coordinateNode *next;
};

coordinateNode *addCoordinateNode(coordinateNode *current, int row, int column) {
    coordinateNode* next = (coordinateNode*) malloc(sizeof(coordinateNode));
    if (next == NULL) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }

    next->next = NULL;
    next->row = row;
    next->column = column;

    if (current == NULL){
        return next;
    }

    current->next = next;
    return next;
}

void printCoordinateNode(coordinateNode* p){
    coordinateNode* current = p;
    printf("coordinateNode\n");
    printf("row\tcol\n");
    while(current != NULL){
        printf("%d\t%d\t\n", current->row, current->column);
        current = current->next;
    }
}

void freeCoordinateNode(coordinateNode* p){
    coordinateNode* next = p->next;

    if( next == NULL )
    {
        free(p);
        return;
    }

    freeCoordinateNode(next);
    free(p);
}

// Compiles a list of coordinates of where the X's are
coordinateNode* findFirstChars(char *strings[], int rows, char find[])
{
    coordinateNode* firstNode = NULL;
    coordinateNode* currentNode = firstNode;

    char firstChar = find[0];
    for(int i = 0; i < rows; i++) {
        char *row = strings[i];
        for(int j = 0; j < strlen(strings[i]); j++) {
            char col = row[j];
            if(col == firstChar) {
                if(firstNode == NULL) {
                    firstNode = addCoordinateNode(NULL, i, j);
                    currentNode = firstNode;
                } else {
                    currentNode = addCoordinateNode(currentNode, i, j);
                }
            }
        }    
    }

    return firstNode;
}

// generateDirections creates a coordinateNode that contains all the
// possible directions for a wordsearch.
// coordinateNode is just a pair of ints, so we can use it to store
// a "direction", i.e. A pair of numbers that represent a relative
// translation in the wordsearch. 
coordinateNode* generateDirections()
{
    coordinateNode* directions = NULL;
    coordinateNode* directionsCurr = directions;

    // Let's build up all the 8 possible directions a word can oriented in
    for(int rowOffset = -1; rowOffset <= 1; rowOffset++) {
        for(int colOffset = -1; colOffset <= 1; colOffset++) {
            if( rowOffset == 0 && colOffset == 0) {
                // This is the only non valid combination of offsets
                // so we just skip it
                continue;
            }

            if(directions == NULL) {
                directions = addCoordinateNode(directionsCurr, rowOffset, colOffset);    
                directionsCurr = directions;
            } else {
                directionsCurr = addCoordinateNode(directionsCurr, rowOffset, colOffset);
            }
        }
    }

    return directions;
}

int processWordSearch(char *strings[], int rows, int columns, char find[]){
    // This is a list of coordinates of where the X's are
    coordinateNode* leadingChars = findFirstChars(strings, rows, find);
    coordinateNode* currentLeadingChar = leadingChars;

    int wordLength = strlen(find);
    int maxDelta = wordLength - 1;

    int foundInstances = 0;

    coordinateNode* directions = generateDirections();
    
    // Iterate through the coordinates known to contain the first character of our word.
    while(currentLeadingChar != NULL){
        // Reset directions on each new leading char
        coordinateNode* currentDirection = directions;
        // Iterate through the possible directions
        while(currentDirection != NULL){
            // Add the direction to the coordinates of the leading char incrementally,
            // and check the character at the new coordinate against the appropriate
            // character of our word to find.

            for(int letterNo = 1; letterNo < wordLength; letterNo++) {
                int targetRow = currentLeadingChar->row + letterNo * currentDirection->row;
                int targetColumn = currentLeadingChar->column + letterNo * currentDirection->column;

                if(targetRow < 0 || targetColumn < 0) {
                    // This is true if we've reached the left or top edge of the word search
                    // and haven't matched our word without finding a full match.
                    // We need not continue in this direction.
                    break;
                }

                if(targetRow >= rows || targetColumn >= columns) {
                    // This is true if we've reached the left or top edge of the word search
                    // and haven't matched our word without finding a full match.
                    // We need not continue in this direction.
                    break;
                }

                if( strings[targetRow][targetColumn] != find[letterNo]) {
                    // The target character in the wordsearch does not match
                    // We need not continue searching in this direction.
                    break;
                }

                if(letterNo == wordLength - 1) {
                    // We've matched every letter!
                    foundInstances++;
                }
            }

            currentDirection = currentDirection->next;
        }
        currentLeadingChar = currentLeadingChar->next;   
    }

    return foundInstances;
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
        results.part1 = processWordSearch(wordSearch, rows, columns, find);
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