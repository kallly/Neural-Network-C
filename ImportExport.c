#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <json-c/json.h>

#include "ImportExport.h"

#define NN 8

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
    fgets(bufferTrash,1024,fp);

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
        do{
            if (buffer[i] == (char)';')
                count++;
        }while(buffer[i++]!='/');

        This->nperceptronExit[n] = count;
        This->perceptronExit[n] = malloc(sizeof(int) * count);
    }

    rewind(fpStruct);

    for (int n = 0; n < This->nperceptron - This->noutput - This->nsolve; n++)
    {
        
        scanLine(fpStruct,This->perceptronExit[n]);
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
int exportNetwork(Network *This, char *networkName, char *fileName)
{

    if (!strcmp(networkName, fileName))
    {
        FILE *source = NULL;
        FILE *temp = NULL;
        char ch;
        source = fopen(networkName, "r");
        temp = fopen("temp", "w+");

        for (int n = 0; (ch = getc(source)) != EOF; n++)
        {

            putc(ch, temp);
        }

        fclose(source);
        fclose(temp);
        strcpy(networkName, "temp");
    }

    FILE *file = NULL;
    size_t size = (strlen(networkName) + strlen(fileName) + 2) * sizeof(char);
    char *buffer = malloc(size);
    snprintf(buffer, size, "%s_%s", networkName, fileName);

    file = fopen(fileName, "w+");

    if (file != NULL)
    {
        FILE *source;
        char ch;
        source = fopen(networkName, "r");

        int compte;
        for (int n = 0; (ch = getc(source)) != EOF; n++)
        {
            if (ch == '}')
            {
                compte = n;
            }
        }
        rewind(source);
        for (int n = 0; (ch = getc(source)) != EOF; n++)
        {
            if (n != compte)
            {
                putc(ch, file);
            }
        }
        fclose(source);

        char *neural;
        neural = malloc(sizeof(char) * 772 * This->nperceptron);

        for (int n = 0; n < This->nperceptron; n++)
        {
            char *temp;
            temp = malloc(sizeof(char) * 516);

            char *weights;
            weights = malloc(sizeof(char) * 256);
            unsigned int i = 0;

            if (This->perceptron[n]->nprev > 0)
            {
                for (; i < This->perceptron[n]->nprev - 1; i++)
                {
                    char *weight;
                    weight = malloc(sizeof(char) * 32);
                    snprintf(weight, sizeof(char) * 30, "%lf", This->perceptron[n]->get_synapse(This->perceptron[n],This->perceptron[n]->prev[i]->id)->weight);
                    strcat(weights, weight);
                    strcat(weights, ",");
                    free(weight);
                }

                char *weight;
                weight = malloc(sizeof(char) * 32);
                snprintf(weight, sizeof(char) * 30, "%lf", This->perceptron[n]->get_synapse(This->perceptron[n],This->perceptron[n]->prev[i]->id)->weight);
                strcat(weights, weight);
                free(weight);
            }

            snprintf(temp, sizeof(char) * 516, "\t\t\"%d\":[%s]", n, weights);
            free(weights);
            strcat(neural, temp);
            if (n < This->nperceptron - 1)
            {
                strcat(neural, ",\n");
            }
            free(temp);
        }

        char *buffer;
        buffer = malloc(sizeof(char) * 1032 * This->nperceptron);
        snprintf(buffer, sizeof(char) * 1032 * This->nperceptron, ",\n\t\"neural\":{\n%s\n\t}\n}", neural);

        fputs(buffer, file);
        fclose(file);
    }
    else
    {
        return 0;
    }

    return 1;
}

/******************************************************************************/
int inputDataCsv(Network *This, char *fileName)
{

    FILE *file = NULL;

    file = fopen(fileName, "r");

    double tempSolve[NN];
    double temp[This->ninput][NN];

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
        for (int n = 0; n < NN; n++)
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
            This->perceptron[n]->set_synapse(This->perceptron[n],This->perceptron[n]->prev[i]->id, tempPV);
        }
    }
    return 0;
}