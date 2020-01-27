#include <stdlib.h>
#include <stdio.h>

#include "activation.h"
#include "Network.h"
#include "Perceptron.h"

int fillInNetworkJSON(Network *network);

int main()
{   
    Network *network = New_Network("netword_template.json","RANDOM");
    
    srand(time(NULL));

    network->inputData(network);


    network->train(network);

    while (1)
    {
        network->testNetwork(network);

    }
  

#ifdef __WIN32__
        system("PAUSE");
#endif
        return 0;
}


