#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "Network.h"

#define NN "8"
#define DEFAULTNETWORKDESC ("network/network_desc_template.txt")
#define DEFAULTNETWORKSTRUCT ("network/network_struct_template.txt")
#define DEFAULTRESULT ("result.txt")
#define NPARAMETER 7


char **parameter(int argc, char *argv[]);

int main(int argc, char *argv[])
{

    char **allParametre = parameter(argc, argv);

    Network *network = New_Network(allParametre[5], allParametre[6], allParametre[1],strtoul(allParametre[0], NULL, 10));

    bool end = false;

    srand(time(NULL));

    //network->inputData(network);
    network->inputDataCsv(network, "data/(aETb)OUnonc.csv");

    do
    {
        printf("\n1 erreur\n2 export\n3 test\n4 %s\n5 exit\n", "START train");
        int choice = 0;
        scanf("%d", &choice);
        switch (choice)
        {
        case 1:
            printf("ERR: %lf", network->err);
            break;
        case 2:
            network->exportNetwork(network, allParametre[2]);
            break;
        case 3:
            network->testNetwork(network);
            break;
        case 4:
            network->train(network, strtoul(allParametre[3], NULL, 10), strtod(allParametre[4], NULL));
            break;
        case 5:
            end = true;
            break;
        default:
            break;
        }

    } while (!end);

    network->destructor(network);
    free(network);

#ifdef __WIN32__
    system("PAUSE");
#endif
    return 0;
}

char **parameter(int argc, char *argv[])
{
    char **buffer = malloc(sizeof(char *) * NPARAMETER);
    for (int n = 0; n < NPARAMETER; n++)
    {
        buffer[n] = malloc(sizeof(char) * 128);
    }
    strcpy(buffer[0], NN);
    strcpy(buffer[1], "RANDOM");
    strcpy(buffer[2], DEFAULTRESULT);
    strcpy(buffer[3], "10000");
    strcpy(buffer[4], "0");
    strcpy(buffer[5], DEFAULTNETWORKDESC);
    strcpy(buffer[6], DEFAULTNETWORKSTRUCT);

    for (int n = 1; n < argc; n++)
    {
        if (!strcmp(argv[n], "-nenter:"))
        {
            strcpy(buffer[0], argv[++n]);
        }
        else
        {
            if (!strcmp(argv[n], "-weight:"))
            {
                strcpy(buffer[1], argv[++n]);
            }
            else
            {
                if (!strcmp(argv[n], "-dest:"))
                {
                    strcpy(buffer[2], argv[++n]);
                }
                else
                {
                    if (!strcmp(argv[n], "-iter:"))
                    {
                        strcpy(buffer[3], argv[++n]);
                    }
                    else
                    {
                        if (!strcmp(argv[n], "-err:"))
                        {
                            strcpy(buffer[4], argv[++n]);
                        }
                        else
                        {
                            if (!strcmp(argv[n], "-descFile:"))
                            {
                                strcpy(buffer[5], argv[++n]);
                            }
                            else
                            {
                                if (!strcmp(argv[n], "-structFile:"))
                                {
                                    strcpy(buffer[6], argv[++n]);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return buffer;
}
