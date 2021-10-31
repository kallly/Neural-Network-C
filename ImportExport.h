#include "Network.h"

int scanLine(FILE*, int*);
int fillInNetwork(Network*, char*, char*);
int fillInNetworkJSON(Network*, char*);
int initValueJson(Network*, char*);
int inputDataCsv(Network*, char*);
int exportNetwork(Network*, char*);