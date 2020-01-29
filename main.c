#include <stdlib.h>
#include <stdio.h>

#include "Network.h"

int fillInNetworkJSON(Network *network);

int main()
{   
    Network *network = New_Network("netword_template2.json","RANDOM");
    
    srand(time(NULL));

    //network->inputData(network);
    network->inputDataCsv(network,"data/(aETb)OUnonc.csv");
    network->train(network);
    network->exportNetwork(network,"result.json");
            
    for (;;)
    {
        printf("\n1 train\n2 export\n3 test\n4 exit\n");
        int choice=0;
        scanf("%d",&choice);
        switch (choice)
        {
        case 1:
            network->train(network);
            break;
        case 2:
            network->exportNetwork(network,"result.json");
            break;
        case 3:
            network->testNetwork(network);
            break;
        case 4:
            return 0;
            break;
        default:
            break;
        } 
    }
  

#ifdef __WIN32__
        system("PAUSE");
#endif
        return 0;
}


