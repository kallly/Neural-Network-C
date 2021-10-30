#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "Perceptron.h"
#include "activation.h"

//
static void Perceptron_Init(Perceptron *);

static char *toString(Perceptron *This);

static Perceptron **get_prev(Perceptron *This);
static int set_prev(Perceptron *This, Perceptron **prev, unsigned int nprev);
static Perceptron **get_next(Perceptron *This);
static int set_next(Perceptron *This, Perceptron **next, unsigned int nnext);

static unsigned int get_nenter(Perceptron *This);
static int set_nenter(Perceptron *This, unsigned int nenter);
static unsigned int get_nprev(Perceptron *This);
static int set_nprev(Perceptron *This, unsigned int nprev);
static unsigned int get_nnext(Perceptron *This);
static int set_nnext(Perceptron *This, unsigned int nnext);

static Synapse *get_synapse(Perceptron *This, unsigned int id);
static int set_synapse(Perceptron *This, unsigned int id, double weight);
static double *get_exit(Perceptron *This);
static int set_exit(Perceptron *This, double *exit);
static double *get_exitDelta(Perceptron *This);
static int set_exitDelta(Perceptron *This, double *exitDelta);

static int forming(Perceptron *This);
static int formingDelta(Perceptron *This);
static int update(Perceptron *This);

static unsigned int get_nformed(Perceptron *This);
static int set_nformed(Perceptron *This, unsigned int nformed);
static unsigned int get_nformedDelta(Perceptron *This);
static int set_nformedDelta(Perceptron *This, unsigned int nformedDelta);

static int destructor(Perceptron *This);
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

	This->set_nenter(This, nenter);
	This->set_nprev(This, nprev);
	This->set_nnext(This, nnext);
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

	This->get_prev = get_prev;
	This->set_prev = set_prev;
	This->get_next = get_next;
	This->set_next = set_next;

	This->get_nenter = get_nenter;
	This->set_nenter = set_nenter;
	This->get_nprev = get_nprev;
	This->set_nprev = set_nprev;
	This->get_nnext = get_nnext;
	This->set_nnext = set_nnext;

	This->get_synapse = get_synapse;
	This->set_synapse = set_synapse;
	This->get_exit = get_exit;
	This->set_exit = set_exit;
	This->get_exitDelta = get_exitDelta;
	This->set_exitDelta = set_exitDelta;

	This->forming = forming;
	This->formingDelta = formingDelta;
	This->update = update;

	This->get_nformed = get_nformed;
	This->set_nformed = set_nformed;

	This->get_nformedDelta = get_nformedDelta;
	This->set_nformedDelta = set_nformedDelta;

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
static unsigned int get_nenter(Perceptron *This)
{
	return This->nenter;
}

/******************************************************************************/
static int set_nenter(Perceptron *This, unsigned int nenter)
{
	This->nenter = nenter;
	return 1;
}

/******************************************************************************/
static Perceptron **get_prev(Perceptron *This)
{
	return This->prev;
}

/******************************************************************************/
static int set_prev(Perceptron *This, Perceptron **prev, unsigned int nprev)
{
	This->set_nprev(This, nprev);
	This->prev = prev;

	if (This->synapses != NULL)
	{
		free(This->synapses);
	}
	This->synapses = malloc(sizeof(Synapse *) * nprev);

	for (unsigned int n = 0; n < This->get_nprev(This); n++)
	{
		This->synapses[n] = malloc(sizeof(Synapse));
		This->synapses[n]->id = This->prev[n]->id;
	}

	return 1;
}

/******************************************************************************/
static Perceptron **get_next(Perceptron *This)
{
	return This->next;
}

/******************************************************************************/
static int set_next(Perceptron *This, Perceptron **next, unsigned int nnext)
{
	This->set_nnext(This, nnext);
	This->next = next;
	return 1;
}

/******************************************************************************/
static unsigned int get_nprev(Perceptron *This)
{
	return This->nprev;
}

/******************************************************************************/
static int set_nprev(Perceptron *This, unsigned int nprev)
{
	This->nprev = nprev;
	return 1;
}

/******************************************************************************/
static unsigned int get_nnext(Perceptron *This)
{
	return This->nnext;
}

/******************************************************************************/
static int set_nnext(Perceptron *This, unsigned int nnext)
{
	This->nnext = nnext;
	return 1;
}

/******************************************************************************/
static Synapse *get_synapse(Perceptron *This, unsigned int id)
{
	for (unsigned int n = 0; n < This->get_nprev(This); n++)
	{
		if (This->synapses[n]->id == id)
		{
			return This->synapses[n];
		}
	}
	return NULL;
}

/******************************************************************************/
static int set_synapse(Perceptron *This, unsigned int id, double weight)
{
	for (unsigned int n = 0; n < This->get_nprev(This); n++)
	{
		if (This->synapses[n]->id == id)
		{
			This->synapses[n]->weight = weight;
			break;
		}
	}

	return 1;
}

/******************************************************************************/
static double *get_exit(Perceptron *This)
{
	return This->exit;
}

/******************************************************************************/
static int set_exit(Perceptron *This, double *exit)
{
	if (This->exit != NULL)
		free(This->exit);
	This->exit = malloc(sizeof(double) * (This->get_nenter(This)));

	for (unsigned int n = 0; n < This->get_nenter(This); n++)
	{
		This->exit[n] = exit[n];
	}
	return 1;
}

/******************************************************************************/
static double *get_exitDelta(Perceptron *This)
{
	return This->exitDelta;
}

/******************************************************************************/
static int set_exitDelta(Perceptron *This, double *exitDelta)
{
	if (This->exitDelta != NULL)
		free(This->exitDelta);
	This->exitDelta = malloc(sizeof(double) * (This->get_nenter(This)));

	for (unsigned int n = 0; n < This->get_nenter(This); n++)
	{
		This->exitDelta[n] = exitDelta[n];
	}
	return 1;
}

/******************************************************************************/
static unsigned int get_nformed(Perceptron *This)
{
	return This->nformed;
}

/******************************************************************************/
static int set_nformed(Perceptron *This, unsigned int nformed)
{
	This->nformed = nformed;
	return 1;
}

/******************************************************************************/
static unsigned int get_nformedDelta(Perceptron *This)
{
	return This->nformedDelta;
}

/******************************************************************************/
static int set_nformedDelta(Perceptron *This, unsigned int nformedDelta)
{
	This->nformedDelta = nformedDelta;
	return 1;
}

/******************************************************************************/
static int forming(Perceptron *This)
{
	This->set_nformed(This, This->get_nformed(This) + 1);
	//printf("\nN:%d\tGOAL:%d\tAdress: %p\n", This->get_nformed(This), This->get_nprev(This), (void *)This);
	if (This->get_nformed(This) == (This->get_nprev(This)))
	{
		double r[This->get_nenter(This)];
		double s[This->get_nenter(This)];
		for (unsigned int e = 0; e < This->get_nenter(This); e++)
		{
			r[e] = -0.01;
			for (unsigned int n = 0; n < This->get_nprev(This); n++)
			{
				//printf("###---[ %d\tNprev: %d\n", n, This->get_nprev(This));
				//printf("%p", (void *)This->get_prev(This)[n] );

				double weight = This->get_synapse(This, This->get_prev(This)[n]->id)->weight;
				r[e] += (This->get_prev(This)[n]->get_exit(This->get_prev(This)[n])[e] * weight);
				//printf("-->%lf\t*\t%lf\t=\t%lf\n", (This->get_prev(This)[n]->get_exit(This->get_prev(This)[n])[e]), weight, r[e]);
			}

			s[e] = This->funcAct(r[e]);

			//printf("------------>%lf\t------------>%lf\n",r[e],s[e]);
		}

		//printf("%p : %lf\n",(void *)This,s[0]);
		This->set_exit(This, s);
		//printf("AAAAAAAAAAAAAAA\t%d\t%lf\n",This->get_nnext(This),This->get_exit(This)[0]);
		//printf("----------------%d\n",This->get_nnext(This));
		for (unsigned int n = 0; n < This->get_nnext(This); n++)
			This->get_next(This)[n]->forming(This->get_next(This)[n]);

		This->set_nformed(This, 0);
	}

	return 1;
}

/******************************************************************************/
static int formingDelta(Perceptron *This)
{

	This->set_nformedDelta(This, This->get_nformedDelta(This) + 1);
	//printf("\nformingDelta -> N:%d\tGOAL:%d\n",This->get_nformedDelta(This),This->get_nnext(This));
	if (This->get_nformedDelta(This) == This->get_nnext(This) && This->get_nprev(This) > 0)
	{

		double s[This->get_nenter(This)];
		for (unsigned int e = 0; e < This->get_nenter(This); e++)
		{
			s[e] = 0.f;

			if (This->solve != NULL)
			{
				s[e] += This->funcAct_d(This->get_exit(This)[e] - This->solve->get_exit(This->solve)[e]);
			}
			else
			{
				if (This->get_nnext(This) != 0)
				{
					for (unsigned int n = 0; n < This->get_nnext(This); n++)
					{
						s[e] += This->get_next(This)[n]->get_exitDelta(This->get_next(This)[n])[e];
					}

					//double exit = 0;
					//
					//printf("%d: ln(%lf)\t",This->id, This->get_exit(This)[e]);
					//for (unsigned int n = 0; n < This->get_nnext(This); n++)
					//{
					//	double weight = This->get_synapse(This->get_next(This)[n], This->id)->weight;
					//	exit += This->funcAct_b(This->get_exit(This)[e]) / weight;
					//	printf("%lf; ",weight);
					//}
					//
					//printf("%lf / %d = %lf\n", exit, This->get_nnext(This), exit/This->get_nnext(This));
					//exit /= This->get_nnext(This);
					//
					//s[e] += This->funcAct_d(This->get_exit(This)[e] - exit);
				}
			}
		}
		This->set_exitDelta(This, s);

		for (int n = This->get_nprev(This) - 1; n >= 0; n--)
			This->get_prev(This)[n]->formingDelta(This->get_prev(This)[n]);
		;
		This->set_nformedDelta(This, -1);
	}

	return 1;
}

/******************************************************************************/
static int update(Perceptron *This)
{

	double r;
	double alpha = 0.01;
	for (unsigned int n = 0; n < This->get_nprev(This); n++)
	{
		r = 0.f;

		for (unsigned int e = 0; e < This->get_nenter(This); e++)
		{
			/* printf("-----------------------------FFFFFFFFFFFLLLLLLLAAAAAAAAAAAGGGGGG---------------\n\n");
        printf("-----------------------------EEEEEEEEENNNNNNNNNNNNNNDDDDDDDDDDDDDDDD---------------\n\n");
              return 0;*/
			//printf("%lf\n",This->get_exitDelta(This)[e]);

			r += (This->get_prev(This)[n]->get_exit(This->get_prev(This)[n])[e] * (This->get_exitDelta(This)[e]));
		}

		//printf("||||||||||||||||||||||||||||||||||||--------------------->>>>>>>> %lf\n",r[n]);

		double weight = This->get_synapse(This, This->get_prev(This)[n]->id)->weight;
		r = weight - (r * alpha);

		This->set_synapse(This, This->get_prev(This)[n]->id, r);
	}

	return 1;
}

/******************************************************************************/
static char *toString(Perceptron *This)
{

	size_t size = sizeof(char) * 100;
	char *string = (char *)malloc(size);

	snprintf(string, size, "Exit: %0.5lf\t%0.5lf\t%0.5lf\t%0.5lf\n", This->get_exit(This)[0], This->get_exit(This)[1], This->get_exit(This)[2], This->get_exit(This)[3]);
	return string;
}

/******************************************************************************/
static int destructor(Perceptron *This)
{
	free(This->synapses);
	free(This->exit);
	free(This->exitDelta);

	return 1;
}
