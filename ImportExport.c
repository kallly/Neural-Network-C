#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "ImportExport.h"

/******************************************************************************/
int scanLine(FILE *fp, int *data)
{
    int i = 0;
    char trash = '/';
    do
    {
        fscanf(fp, "%d", &data[i++]);
        //printf("%d\n",data[i-1]);

        fscanf(fp, "%c", &trash);
    } while (trash == ';');

    char bufferTrash[1024];
    fgets(bufferTrash, 1024, fp);

    return 0;
}

/******************************************************************************/
int fillInNetwork(Network *This, char *descFileName, char *structFileName)
{
    FILE *fpDesc;
    fpDesc = fopen(descFileName, "r");

    if (fpDesc == NULL)
    {
        printf("Open file error\n");
        exit(1);
    }

    char bufferTrash[256];

    fscanf(fpDesc, "%d", &This->ninput);
    fgets(bufferTrash, 256, fpDesc);
    fscanf(fpDesc, "%d", &This->noutput);
    fgets(bufferTrash, 256, fpDesc);
    fscanf(fpDesc, "%d", &This->nsolve);
    fgets(bufferTrash, 256, fpDesc);
    fscanf(fpDesc, "%d", &This->nperceptron);
    fgets(bufferTrash, 256, fpDesc);

    This->input = malloc(sizeof(int) * This->ninput);
    This->output = malloc(sizeof(int) * This->noutput);
    This->solve = malloc(sizeof(int) * This->nsolve);
    This->perceptronExit = malloc(sizeof(int *) * This->nperceptron);
    This->nperceptronExit = malloc(sizeof(int) * This->nperceptron);

    scanLine(fpDesc, This->input);
    scanLine(fpDesc, This->output);
    scanLine(fpDesc, This->solve);

    fclose(fpDesc);

    FILE *fpStruct;
    fpStruct = fopen(structFileName, "r");

    if (fpDesc == NULL)
    {
        printf("Open file error\n");
        exit(1);
    }

    for (int n = 0; n < This->nperceptron - This->noutput - This->nsolve; n++)
    {
        char buffer[1024];
        fgets(buffer, 1024, fpStruct);

        int count = 1;
        int i = 0;
        do
        {
            if (buffer[i] == (char)';')
                count++;
        } while (buffer[i++] != '/');

        This->nperceptronExit[n] = count;
        This->perceptronExit[n] = malloc(sizeof(int) * count);
    }

    rewind(fpStruct);

    for (int n = 0; n < This->nperceptron - This->noutput - This->nsolve; n++)
    {

        scanLine(fpStruct, This->perceptronExit[n]);
        for (int i = 0; i < This->nperceptronExit[n]; i++)
        {
            printf("Neural %d / Exit %d -> %d\n", n, i, This->perceptronExit[n][i]);
        }
    }
    printf("\n");

    fclose(fpStruct);

    return 0;
}


/******************************************************************************/
int exportNetwork(Network *This, char *fileName)
{
    FILE *fpResult = NULL;
    fpResult = fopen(fileName, "w+");
    if (fpResult == NULL)
    {
        return 0;
    }

    for (int i = 0; i < This->nperceptron; i++)
    {
        char *weights;
        weights = malloc(sizeof(char) * 1024);

        if (This->perceptron[i]->nprev>0)
        {
            for (unsigned int j = 0; j < This->perceptron[i]->nprev - 1; j++)
            {
                if (j > 7)
                    exit(0);
                char *weight;
                weight = malloc(sizeof(char) * 30);

                snprintf(weight, 30, "%lf", This->perceptron[i]->synapses[j]->weight);
                strcat(weights, weight);
                strcat(weights, ";");

                free(weight);
            }

            if (This->perceptron[i]->synapses != NULL)
            {
                char *weight;
                weight = malloc(sizeof(char) * 30);
                snprintf(weight, sizeof(char) * 30, "%lf", This->perceptron[i]->synapses[This->perceptron[i]->nprev - 1]->weight);

                strcat(weights, weight);
                free(weight);
            }
        }

        fprintf(fpResult, "%s//perceptron %d\n", weights, i);

        free(weights);
    }

    fclose(fpResult);

    return 1;
}

/******************************************************************************/
int inputDataCsv(Network *This, char *fileName)
{

    FILE *file = NULL;

    file = fopen(fileName, "r");

    double tempSolve[This->nenter];
    double temp[This->ninput][This->nenter];

    if (file == NULL)
    {
        return 0;
    }
    else
    {
        char chaine = 'a';
        while (chaine != '\n')
        {
            chaine = fgetc(file);
            //printf("%c", chaine);
        }
        for (int n = 0; n < This->nenter; n++)
        {
            for (int i = 0; i < This->ninput; i++)
            {
                fscanf(file, "%lf", &temp[i][n]);
                //printf("%lf\t",temp[i][n]);
                fgetc(file);
            }
            fscanf(file, "%lf", &tempSolve[n]);
            //printf("%lf\n",tempSolve[n]);
            fgetc(file);
        }
    }

    This->perceptron[This->solve[0]]->set_exit(This->perceptron[This->solve[0]], tempSolve);

    for (int n = 0; n < This->ninput; n++)
    {
        This->perceptron[n]->set_exit(This->perceptron[n], temp[n]);
    }

    return 1;
}