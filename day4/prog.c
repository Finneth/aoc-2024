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

// Compiles a list of coordinates of where the X's are
intPairNode* findFirstChars(char *strings[], int rows, char find)
{
    intPairNode* firstNode = NULL;
    intPairNode* currentNode = firstNode;

    char firstChar = find;
    for(int i = 0; i < rows; i++) {
        char *row = strings[i];
        for(int j = 0; j < strlen(strings[i]); j++) {
            char col = row[j];
            if(col == firstChar) {
                if(firstNode == NULL) {
                    firstNode = addIntPairNode(NULL, i, j);
                    currentNode = firstNode;
                } else {
                    currentNode = addIntPairNode(currentNode, i, j);
                }
            }
        }    
    }

    return firstNode;
}

// generateRadialDirections creates a intPairNode that contains all the
// possible directions for a wordsearch.
// intPairNode is just a pair of ints, so we can use it to store
// a "direction", i.e. A pair of numbers that represent a relative
// translation in the wordsearch. 
intPairNode* generateRadialDirections()
{
    intPairNode* directions = NULL;
    intPairNode* directionsCurr = directions;

    // Let's build up all the 8 possible directions a word can oriented in
    for(int rowOffset = -1; rowOffset <= 1; rowOffset++) {
        for(int colOffset = -1; colOffset <= 1; colOffset++) {
            if( rowOffset == 0 && colOffset == 0) {
                // This is the only non valid combination of offsets
                // so we just skip it
                continue;
            }

            if(directions == NULL) {
                directions = addIntPairNode(directionsCurr, rowOffset, colOffset);    
                directionsCurr = directions;
            } else {
                directionsCurr = addIntPairNode(directionsCurr, rowOffset, colOffset);
            }
        }
    }

    return directions;
}

intPairNode* generateDiagonalDirections()
{
    intPairNode* directions = NULL;
    intPairNode* directionsCurr = directions;

    // Let's build up all the 8 possible directions a word can oriented in
    for(int rowOffset = -1; rowOffset <= 1; rowOffset++) {
        for(int colOffset = -1; colOffset <= 1; colOffset++) {
            if( rowOffset == 0 || colOffset == 0) {
                // Diagonals are typified by an offset in both axes.
                // so exclude any offset pairs where one of the offsets
                // is zero. Also, 0,0 is invalid so we're happy to exclude that.
                continue;
            }

            if(directions == NULL) {
                directions = addIntPairNode(directionsCurr, rowOffset, colOffset);    
                directionsCurr = directions;
            } else {
                directionsCurr = addIntPairNode(directionsCurr, rowOffset, colOffset);
            }
        }
    }

    return directions;
}

struct wordVectorNode {
    intPair coords;
    intPair direction;
    int length;
    wordVectorNode *next;
};

wordVectorNode *addWordVectorNode(wordVectorNode *current, int row, int column, int x, int y, int len) {
    wordVectorNode* next = (wordVectorNode*) malloc(sizeof(wordVectorNode));
    if (next == NULL) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }

    next->next = NULL;
    next->coords.row = row;
    next->coords.column = column;
    next->direction.row = x;
    next->direction.column = y;
    next->length = len;

    if (current == NULL){
        return next;
    }

    current->next = next;
    return next;
}

void printWordVectorNode(wordVectorNode* p){
    wordVectorNode* current = p;
    printf("wordVectorNode\n");
    printf("row\tcol\tx\ty\tlen\n");
    while(current != NULL){
        printf("%d\t%d\t%d\t%d\t%d\t\n", current->coords.row, current->coords.column, current->direction.row, current->direction.column, current->length);
        current = current->next;
    }
}

int countWordVectorNode(wordVectorNode* p){
    int count = 0;
    wordVectorNode* current = p;
    while(current != NULL){
        count++;
        current = current->next;
    }

    return count;
}

void freeWordVectorNode(wordVectorNode* p){
    wordVectorNode* next = p->next;

    if( next == NULL )
    {
        free(p);
        return;
    }

    freeWordVectorNode(next);
    free(p);
}

wordVectorNode *findWords(intPairNode* directions, char *strings[], int rows, int columns, char find[]){
    // This is a list of coordinates of where the X's are
    intPairNode* leadingChars = findFirstChars(strings, rows, find[0]);
    intPairNode* currentLeadingChar = leadingChars;

    int wordLength = strlen(find);
    int maxDelta = wordLength - 1;

    wordVectorNode *foundInstances = NULL;
    wordVectorNode *foundInstancesCurr = foundInstances;
    
    // Iterate through the coordinates known to contain the first character of our word.
    while(currentLeadingChar != NULL){
        // Reset directions on each new leading char
        intPairNode* currentDirection = directions;
        // Iterate through the possible directions
        while(currentDirection != NULL){
            // Add the direction to the coordinates of the leading char incrementally,
            // and check the character at the new coordinate against the appropriate
            // character of our word to find.

            for(int letterNo = 1; letterNo < wordLength; letterNo++) {
                int targetRow = currentLeadingChar->coords.row + letterNo * currentDirection->coords.row;
                int targetColumn = currentLeadingChar->coords.column + letterNo * currentDirection->coords.column;

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

                    if(foundInstances == NULL){
                        foundInstances = addWordVectorNode(foundInstances, currentLeadingChar->coords.row, currentLeadingChar->coords.column, currentDirection->coords.row, currentDirection->coords.column, wordLength);
                        foundInstancesCurr = foundInstances;
                    } else {
                        foundInstancesCurr = addWordVectorNode(foundInstancesCurr, currentLeadingChar->coords.row, currentLeadingChar->coords.column, currentDirection->coords.row, currentDirection->coords.column, wordLength);
                    }
                }
            }

            currentDirection = currentDirection->next;
        }
        currentLeadingChar = currentLeadingChar->next;   
    }

    return foundInstances;
}


// findComplementaryDiagonals takes a wordVectorNode, and returns
// a new wordVectorNode that represents the complementary diagnols to
// that wordVector. It does not iterate to the next node.
wordVectorNode *findComplementaryDiagonals(wordVectorNode *wordVector) {
    if (wordVector->direction.row == 0 || wordVector->direction.column == 0) {
        printf("Direction %d, %d is not a diagonal. Abort.", wordVector->direction.row, wordVector->direction.column);
        return NULL;
    }

    int lastCharRow = wordVector->coords.row + (wordVector->length - 1) * wordVector->direction.row;
    int lastCharColumn = wordVector->coords.column + (wordVector->length - 1) * wordVector->direction.column;


    // Our first complementary diagonal starts from the opposite row,
    // but the same column, and has the same horizontal direction,
    // but the opposite vertical
    wordVectorNode * comp = addWordVectorNode(NULL, lastCharRow, wordVector->coords.column, -wordVector->direction.row, wordVector->direction.column, wordVector->length);

    // Our second starts from the same row, but the opposite column,
    // and has the same vertical direction, but the opposite horizontal.
    addWordVectorNode(comp, wordVector->coords.row, lastCharColumn, wordVector->direction.row, -wordVector->direction.column, wordVector->length);

    return comp;
}

void getWord(wordVectorNode *wordVector, char *wordSearch[], char *diagWord) {
    for( int i = 0; i < wordVector->length; i++) {
        int targetRow = wordVector->coords.row + i * wordVector->direction.row;
        int targetColumn = wordVector->coords.column + i * wordVector->direction.column;
        char fromGrid = wordSearch[targetRow][targetColumn];

        diagWord[i] = wordSearch[targetRow][targetColumn];
    }

    diagWord[wordVector->length] = '\0';
}

wordVectorNode *findCrossWords(wordVectorNode *words, char *wordSearch[], int rows, int columns, char find[]){
    wordVectorNode *foundInstances = NULL;
    wordVectorNode *foundInstancesCurr = foundInstances;
    wordVectorNode *wordsIter = words;

    while (wordsIter != NULL) {
        // Check if either of the diagonals matches our word.
        wordVectorNode *diags = findComplementaryDiagonals(wordsIter);

        char diagWord[diags->length+1];
        getWord(diags, wordSearch, diagWord);

        bool match = false;

        if( strcmp(diagWord, find) == 0 ) {
            // Got a match on the first complementary diagonal
            match = true;
        } else {
            getWord(diags->next, wordSearch, diagWord);
            if(strcmp(diagWord, find) == 0) {
                // Got a match on the second diagonal
                match = true;
            }
        }

        if(match) {
            if( foundInstances == NULL ) {
                foundInstances = addWordVectorNode(foundInstances, wordsIter->coords.row, wordsIter->coords.column, wordsIter->direction.row, wordsIter->direction.column, wordsIter->length);
                foundInstancesCurr = foundInstances;
            } else {
                foundInstancesCurr = addWordVectorNode(foundInstancesCurr, wordsIter->coords.row, wordsIter->coords.column, wordsIter->direction.row, wordsIter->direction.column, wordsIter->length);
            }
        }

        wordsIter = wordsIter->next;
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

        char find1[] = "XMAS";
        intPairNode* radialDirections = generateRadialDirections();
        wordVectorNode *radialWords = findWords(radialDirections, wordSearch, rows, columns, find1);

        char find2[] = "MAS";
        intPairNode* diagonalDirections = generateDiagonalDirections();
        wordVectorNode *diagonalWords = findWords(diagonalDirections, wordSearch, rows, columns, find2);
        wordVectorNode *crossWords = findCrossWords(diagonalWords, wordSearch, rows, columns, find2);

        TestResults results;
        results.part1 = countWordVectorNode(radialWords);

        // findCrossWords finds words that have a crossed word.
        // This means we end up double counting the crossword grids,
        // so just half the result here.
        results.part2 = countWordVectorNode(crossWords) / 2;

        // Debug logging for troubleshooting
        if (DEBUG) {
            printWordVectorNode(crossWords);
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