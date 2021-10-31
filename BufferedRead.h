#ifndef BUFFEREDREAD_H_INCLUDED
#define BUFFEREDREAD_H_INCLUDED

typedef unsigned int uint;
typedef unsigned short ushort;

typedef struct tagDataModel {
    char** lines;
    uint numLines;
    uint maxLen;
} DataModel;


void readFile(char* filename, ushort buffSize, DataModel* dm);

void freeDM(DataModel* dm);

#endif // BUFFEREDREAD_H_INCLUDED
