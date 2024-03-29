#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "Perceptron.h"
#include "activation.h"

//
static void Perceptron_Init(Perceptron *);

static char *toString(Perceptron *This);

static bool set_prev(Perceptron *This, Perceptron **prev, unsigned int nprev);
static bool set_next(Perceptron *This, Perceptron **next, unsigned int nnext);

static Synapse *get_synapse(Perceptron *This, unsigned int id);
static bool set_synapse(Perceptron *This, unsigned int id, double weight);

static bool set_exit(Perceptron *This, double *exit);
static bool set_exitDelta(Perceptron *This, double *exitDelta);

static int forming(Perceptron *This);
static int formingDelta(Perceptron *This);
static int update(Perceptron *This);
static bool destructor(Perceptron *This);
//

/******************************************************************************/
Perceptron *New_Perceptron(unsigned int id, unsigned int nenter, Perceptron **prev, unsigned int nprev, Perceptron **next __attribute__((unused)), unsigned int nnext, FuncName funcActName)
{
	Perceptron *This = malloc(sizeof(Perceptron));
	if (!This)
		return NULL;
	Perceptron_Init(This);

	switch (funcActName)
	{
	case 0: //SIGMOID
		This->funcAct = sigmoid;
		This->funcAct_d = sigmoid_d;
		break;
	case 1: //HEAVISIDE
		This->funcAct = heaviside;
		This->funcAct_d = heaviside_d;
		break;

	default:
		This->funcAct = sigmoid;
		This->funcAct_d = sigmoid_d;
		break;
	}

	This->nenter = nenter;
	This->nprev = nprev;
	This->nnext = nnext;
	This->id = id;

	if (prev != NULL)
		This->set_prev(This, prev, nprev);
	if (next != NULL)
		This->set_next(This, next, nnext);

	return This;
}

/******************************************************************************/
static void Perceptron_Init(Perceptron *This)
{
	This->destructor = destructor;

	This->toString = toString;

	This->set_prev = set_prev;
	This->set_next = set_next;

	This->get_synapse = get_synapse;
	This->set_synapse = set_synapse;
	This->set_exit = set_exit;
	This->set_exitDelta = set_exitDelta;

	This->forming = forming;
	This->formingDelta = formingDelta;
	This->update = update;

	This->funcAct = sigmoid;
	This->funcAct_b = sigmoid_b;
	This->funcAct_d = sigmoid_d;

	This->nenter = 0;
	This->nprev = 0;
	This->nnext = 0;
	This->id = 0;

	This->prev = NULL;
	This->next = NULL;
	This->synapses = NULL;

	This->exit = NULL;
	This->exitDelta = NULL;

	This->solve = NULL;

	This->nformed = 0;
	This->nformedDelta = 0;
}

/******************************************************************************/
static bool set_prev(Perceptron *This, Perceptron **prev, unsigned int nprev)
{
	This->nprev = nprev;
	This->prev = prev;

	if (This->synapses != NULL)
	{
		free(This->synapses);
	}
	This->synapses = malloc(sizeof(Synapse *) * nprev);

	for (unsigned int n = 0; n < This->nprev; n++)
	{
		This->synapses[n] = malloc(sizeof(Synapse));
		This->synapses[n]->id = This->prev[n]->id;
	}

	return true;
}

/******************************************************************************/
static bool set_next(Perceptron *This, Perceptron **next, unsigned int nnext)
{
	This->nnext = nnext;
	This->next = next;
	return true;
}

/******************************************************************************/
static Synapse *get_synapse(Perceptron *This, unsigned int id)
{
	for (unsigned int n = 0; n < This->nprev; n++)
	{
		if (This->synapses[n]->id == id)
		{
			return This->synapses[n];
		}
	}
	return NULL;
}

/******************************************************************************/
static bool set_synapse(Perceptron *This, unsigned int id, double weight)
{
	for (unsigned int n = 0; n < This->nprev; n++)
	{
		if (This->synapses[n]->id == id)
		{
			This->synapses[n]->weight = weight;
			break;
		}
	}

	return true;
}

/******************************************************************************/
static bool set_exit(Perceptron *This, double *exit)
{
	if (This->exit != NULL)
		free(This->exit);
	This->exit = malloc(sizeof(double) * (This->nenter));

	for (unsigned int n = 0; n < This->nenter; n++)
	{
		This->exit[n] = exit[n];
	}
	return true;
}


/******************************************************************************/
static bool set_exitDelta(Perceptron *This, double *exitDelta)
{
	if (This->exitDelta != NULL)
		free(This->exitDelta);
	This->exitDelta = malloc(sizeof(double) * (This->nenter));

	for (unsigned int n = 0; n < This->nenter; n++)
	{
		This->exitDelta[n] = exitDelta[n];
	}
	return true;
}

/******************************************************************************/
static int forming(Perceptron *This)
{
	This->nformed = This->nformed + 1;
	//printf("\nN:%d\tGOAL:%d\tAdress: %p\n", This->get_nformed(This), This->nprev;
	if (This->nformed == This->nprev)
	{
		double r[This->nenter];
		double s[This->nenter];
		for (unsigned int e = 0; e < This->nenter; e++)
		{
			r[e] = -0.01;
			for (unsigned int n = 0; n < This->nprev; n++)
			{
				//printf("###---[ %d\tNprev: %d\n", n, This->nprev;
				//printf("%p", (void *)This->prev[n] );

				double weight = This->get_synapse(This, This->prev[n]->id)->weight;
				r[e] += This->prev[n]->exit[e] * weight;
				//printf("-->%lf\t*\t%lf\t=\t%lf\n", (This->prev[n]->exit[n])[e]), weight, r[e]);
			}

			s[e] = This->funcAct(r[e]);

			//printf("------------>%lf\t------------>%lf\n",r[e],s[e]);
		}

		//printf("%p : %lf\n",(void *)This,s[0]);
		This->set_exit(This, s);
		//printf("AAAAAAAAAAAAAAA\t%d\t%lf\n",This->nnext,This->exit[0]);
		//printf("----------------%d\n",This->nnext);
		for (unsigned int n = 0; n < This->nnext; n++)
			This->next[n]->forming(This->next[n]);

		This->nformed = 0;
	}

	return true;
}

/******************************************************************************/
static int formingDelta(Perceptron *This)
{

	This->nformedDelta = This->nformedDelta + 1;
	//printf("\nformingDelta -> N:%d\tGOAL:%d\n",This->nformedDelta,This->nnext);
	if (This->nformedDelta == This->nnext && This->nprev > 0)
	{

		double s[This->nenter];
		for (unsigned int e = 0; e < This->nenter; e++)
		{
			s[e] = 0.f;

			if (This->solve != NULL)
			{
				s[e] += This->funcAct_d(This->exit[e] - This->solve->exit[e]);
			}
			else
			{
				if (This->nnext != 0)
				{
					for (unsigned int n = 0; n < This->nnext; n++)
					{
						s[e] += This->next[n]->exitDelta[e];
					}

					//double exit = 0;
					//
					//printf("%d: ln(%lf)\t",This->id, This->exit[e]);
					//for (unsigned int n = 0; n < This->nnext; n++)
					//{
					//	double weight = This->get_synapse(This->next[n], This->id)->weight;
					//	exit += This->funcAct_b(This->exit[e]) / weight;
					//	printf("%lf; ",weight);
					//}
					//
					//printf("%lf / %d = %lf\n", exit, This->nnext, exit/This->nnext);
					//exit /= This->nnext;
					//
					//s[e] += This->funcAct_d(This->exit[e] - exit);
				}
			}
		}
		This->set_exitDelta(This, s);

		for (int n = This->nprev - 1; n >= 0; n--)
			This->prev[n]->formingDelta(This->prev[n]);

		This->nformedDelta = -1;
	}

	return true;
}

/******************************************************************************/
static int update(Perceptron *This)
{

	double r;
	double alpha = 0.01;
	for (unsigned int n = 0; n < This->nprev; n++)
	{
		r = 0.f;

		for (unsigned int e = 0; e < This->nenter; e++)
		{
			/* printf("-----------------------------FFFFFFFFFFFLLLLLLLAAAAAAAAAAAGGGGGG---------------\n\n");
        printf("-----------------------------EEEEEEEEENNNNNNNNNNNNNNDDDDDDDDDDDDDDDD---------------\n\n");
              return false;*/
			//printf("%lf\n",This->get_exitDelta(This)[e]);

			r += This->prev[n]->exit[e] * This->exitDelta[e];
		}

		//printf("||||||||||||||||||||||||||||||||||||--------------------->>>>>>>> %lf\n",r[n]);

		double weight = This->get_synapse(This, This->prev[n]->id)->weight;
		r = weight - (r * alpha);

		This->set_synapse(This, This->prev[n]->id, r);
	}

	return true;
}

/******************************************************************************/
static char *toString(Perceptron *This)
{

	size_t size = sizeof(char) * 100;
	char *string = (char *)malloc(size);

	snprintf(string, size, "Exit: %0.5lf\t%0.5lf\t%0.5lf\t%0.5lf\n", This->exit[0], This->exit[1], This->exit[2], This->exit[3]);
	return string;
}

/******************************************************************************/
static bool destructor(Perceptron *This)
{
	free(This->synapses);
	free(This->exit);
	free(This->exitDelta);

	return true;
}
