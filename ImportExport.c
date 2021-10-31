#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <json-c/json.h>

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

    char bufferTrash[256];
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
int fillInNetworkJSON(Network *This, char *fileName)
{

    FILE *fp;
    char buffer[2048];
    struct json_object *parsed_json;
    struct json_object *input;
    struct json_object *output;
    struct json_object *solve;
    struct json_object *nPerceptron;
    struct json_object *perceptron;
    struct json_object **perceptronExit;

    fp = fopen(fileName, "r");
    fread(buffer, 2048, 1, fp);
    fclose(fp);

    parsed_json = json_tokener_parse(buffer);

    json_object_object_get_ex(parsed_json, "input", &input);
    json_object_object_get_ex(parsed_json, "output", &output);
    json_object_object_get_ex(parsed_json, "solve", &solve);
    json_object_object_get_ex(parsed_json, "nPerceptron", &nPerceptron);
    json_object_object_get_ex(parsed_json, "perceptron", &perceptron);

    This->ninput = json_object_array_length(input);
    This->noutput = json_object_array_length(output);
    This->nsolve = json_object_array_length(solve);
    This->nperceptron = json_object_get_int(nPerceptron);

    This->nperceptronExit = malloc(sizeof(int) * This->nperceptron);

    This->input = malloc(sizeof(int) * This->ninput);
    This->output = malloc(sizeof(int) * This->noutput);
    This->solve = malloc(sizeof(int) * This->nsolve);
    This->perceptronExit = malloc(sizeof(int *) * This->nperceptron);

    perceptronExit = malloc(sizeof(perceptron) * 50);

    for (int n = 0; n < This->ninput; n++)
    {
        This->input[n] = json_object_get_int(json_object_array_get_idx(input, n));
        printf("input %d -> %d\n", n, This->input[n]);
    }

    for (int n = 0; n < This->noutput; n++)
    {
        This->output[n] = json_object_get_int(json_object_array_get_idx(output, n));
        printf("output %d -> %d\n", n, This->output[n]);
    }

    for (int n = 0; n < This->nsolve; n++)
    {
        This->solve[n] = json_object_get_int(json_object_array_get_idx(solve, n));
        printf("solve %d -> %d\n", n, This->solve[n]);
    }

    for (int n = 0; n < This->nperceptron; n++)
    {

        char buffer[128];
        snprintf(buffer, sizeof(char) * 128, "%d", n);

        This->perceptronExit[n] = malloc(sizeof(int) * 50);

        json_object_object_get_ex(perceptron, buffer, &perceptronExit[n]);

        This->nperceptronExit[n] = (int)json_object_array_length(perceptronExit[n]);

        for (int i = 0; i < This->nperceptronExit[n]; i++)
        {
            This->perceptronExit[n][i] = json_object_get_int(json_object_array_get_idx(perceptronExit[n], i));
            printf("Neural %d / Exit %d -> %d\n", n, i, This->perceptronExit[n][i]);
        }
    }

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

/******************************************************************************/
int initValueJson(Network *This, char *fileName)
{
    FILE *fp;
    char buffer[1024];
    struct json_object *parsed_json;

    struct json_object *neural;
    struct json_object **perceptronWeight;

    fp = fopen(fileName, "r");
    fread(buffer, 1024, 1, fp);
    fclose(fp);

    parsed_json = json_tokener_parse(buffer);

    json_object_object_get_ex(parsed_json, "neural", &neural);

    perceptronWeight = malloc(sizeof(neural) * 10);

    for (int n = 0; n < This->nperceptron; n++)
    {

        char buffer[128];
        snprintf(buffer, sizeof(char) * 128, "%d", n);

        json_object_object_get_ex(neural, buffer, &perceptronWeight[n]);

        double tempPV;
        for (unsigned int i = 0; i < This->perceptron[n]->nprev; i++)
        {
            printf("N: %d\tWeight: %d\t%lf\n", n, i, json_object_get_double(json_object_array_get_idx(perceptronWeight[n], i)));
            tempPV = json_object_get_double(json_object_array_get_idx(perceptronWeight[n], i));
            This->perceptron[n]->set_synapse(This->perceptron[n], This->perceptron[n]->prev[i]->id, tempPV);
        }
    }
    return 0;
}