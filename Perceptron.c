#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "Perceptron.h"
#include "activation.h"

static void Perceptron_Init(Perceptron*);

static char* toString(Perceptron *This);

static Perceptron** get_prev(Perceptron *This);
static int set_prev(Perceptron *This,Perceptron** prev,unsigned int nprev);
static Perceptron** get_next(Perceptron *This);
static int set_next(Perceptron *This,Perceptron** next,unsigned int nnext);
static double get_weight(Perceptron *This);
static int set_weight(Perceptron *This,double weight);
static double get_exit(Perceptron *This);
static int set_exit(Perceptron *This,double exit);
static unsigned int get_nprev(Perceptron *This);
static int set_nprev(Perceptron *This,unsigned int nprev);
static unsigned int get_nnext(Perceptron *This);
static int set_nnext(Perceptron *This,unsigned int nnext);

static int forming(Perceptron *This);

/******************************************************************************/
Perceptron* New_Perceptron(Perceptron **prev,unsigned int nprev,Perceptron **next,unsigned int nnext)
{
    Perceptron *This = malloc(sizeof(Perceptron));
    if(!This) return NULL;
    Perceptron_Init(This);
    
    This->set_nprev(This,nprev);
    This->set_nnext(This,nnext);

    if(prev!=NULL)This->set_prev(This,prev,nprev);
    if(next!=NULL)This->set_prev(This,next,nnext);

    return This;
}

/******************************************************************************/
static void Perceptron_Init(Perceptron *This)
{
    This->toString=toString;

    This->get_prev=get_prev;
    This->set_prev=set_prev;
    This->get_next=get_next;
    This->set_next=set_next;
    This->get_weight=get_weight;
    This->set_weight=set_weight;
    This->get_exit=get_exit;
    This->set_exit=set_exit;
    This->get_nprev=get_nprev;
    This->set_nprev=set_nprev;
    This->get_nnext=get_nnext;
    This->set_nnext=set_nnext;

    This->forming=forming;

    This->prev=NULL;
    This->next=NULL;

    This->weight=(sqrt(-2.0*log((double)rand()/RAND_MAX)))*(cos(6.28318530718*(double)rand()/RAND_MAX));;
    This->exit=0.f;
}

/******************************************************************************/
static Perceptron** get_prev(Perceptron *This)
{
       return This->prev;
}

/******************************************************************************/
static int set_prev(Perceptron *This,Perceptron** prev,unsigned int nprev)
{
    This->set_nprev(This,nprev);
       This->prev = prev;
       return 1;
}

/******************************************************************************/
static Perceptron** get_next(Perceptron *This)
{
       return This->next;
}

/******************************************************************************/
static int set_next(Perceptron *This,Perceptron** next,unsigned int nnext)
{
    This->set_nnext(This,nnext);
       This->next = next;
       return 1;
}


/******************************************************************************/
static double get_weight(Perceptron *This)
{
       return This->weight;
}

/******************************************************************************/
static int set_weight(Perceptron *This,double weight)
{
       This->weight = weight;
       return 1;
}

/******************************************************************************/
static double get_exit(Perceptron *This)
{
       return This->exit;
}

/******************************************************************************/
static int set_exit(Perceptron *This,double exit)
{
       This->exit = exit;
       return 1;
}

/******************************************************************************/
static unsigned int get_nprev(Perceptron *This)
{
       return This->nprev;
}

/******************************************************************************/
static int set_nprev(Perceptron *This,unsigned int nprev)
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
static int set_nnext(Perceptron *This,unsigned int nnext)
{
       This->nnext = nnext;
       return 1;
}

/******************************************************************************/
static int forming(Perceptron *This)
{  
    double r=0.f;
	for(unsigned int n=0;n<This->get_nprev(This);n++){	   
	    r+=(This->get_prev(This)[n]->get_exit(This->get_prev(This)[n]) * This->get_weight(This));
        //printf("-->%lf\n",(This->get_prev(This)[n]->get_exit(This->get_prev(This)[n]) * This->get_weight(This)));
    }
	//printf("------------>%lf\n\n",r);			
	This->set_exit(This,sigmoid(r));


       return 1;
}

/******************************************************************************/
static char* toString( Perceptron *This)
{
       size_t size = sizeof(char) * 40;
       char* string = (char*)malloc(size);
       
       snprintf(string, size, "Weight: %0.5lf\nExit: %0.5lf\n",This->get_weight(This),This->get_exit(This) );
       
       return string;
}

