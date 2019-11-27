#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "Perceptron.h"
#include "activation.h"

//
       static void Perceptron_Init(Perceptron*);
       
       static char* toString(Perceptron *This);
       
       static Perceptron** get_prev(Perceptron *This);
       static int set_prev(Perceptron *This,Perceptron** prev,unsigned int nprev);
       static Perceptron** get_next(Perceptron *This);
       static int set_next(Perceptron *This,Perceptron** next,unsigned int nnext);
       
       static unsigned int get_nenter(Perceptron *This);
       static int set_nenter(Perceptron *This,unsigned int nenter);
       static unsigned int get_nprev(Perceptron *This);
       static int set_nprev(Perceptron *This,unsigned int nprev);
       static unsigned int get_nnext(Perceptron *This);
       static int set_nnext(Perceptron *This,unsigned int nnext);
       
       static double* get_weights(Perceptron *This);
       static int set_weights(Perceptron *This,...);
       static double* get_exit(Perceptron *This);
       static int set_exit(Perceptron *This,double *exit);
       
       static int forming(Perceptron *This);
//


/******************************************************************************/
Perceptron* New_Perceptron(unsigned int nenter,Perceptron **prev,unsigned int nprev,Perceptron **next,unsigned int nnext)
{
    Perceptron *This = malloc(sizeof(Perceptron));
    if(!This) return NULL;
    Perceptron_Init(This);
    
    This->set_nenter(This,nenter);
    This->set_nprev(This,nprev);
    This->set_nnext(This,nnext);

    if(nprev!=0)This->set_prev(This,prev,nprev);
    if(nnext!=0)This->set_next(This,next,nnext);

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
    
    This->get_nenter=get_nenter;
    This->set_nenter=set_nenter;
    This->get_nprev=get_nprev;
    This->set_nprev=set_nprev;
    This->get_nnext=get_nnext;
    This->set_nnext=set_nnext;

    This->get_weights=get_weights;
    This->set_weights=set_weights;
    This->get_exit=get_exit;
    This->set_exit=set_exit;
    

    This->forming=forming;

    This->nenter=0;
    This->nprev=0;
    This->nnext=0;

    This->prev=NULL;
    This->next=NULL;

    This->weights=NULL;
    This->exit=NULL;
}

/******************************************************************************/
static unsigned int get_nenter(Perceptron *This)
{
          return This->nenter;
}

/******************************************************************************/
static int set_nenter(Perceptron *This,unsigned int nenter)
{
       This->nenter = nenter;
       return 1;
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
static double* get_weights(Perceptron *This)
{
       return This->weights;
}

/******************************************************************************/
static int set_weights(Perceptron *This,...)
{
       if(This->weights!=NULL)free(This->weights);
       This->weights = malloc( sizeof(double) * (This->get_nprev(This)));

       va_list ap;
       va_start(ap,This);

       for(unsigned int n=0;n< This->get_nprev(This);n++){
       This->weights[n] = va_arg(ap,double);
       }

       va_end(ap);

       return 1;
}

/******************************************************************************/
static double* get_exit(Perceptron *This)
{
       return This->exit;
}

/******************************************************************************/
static int set_exit(Perceptron *This,double *exit)
{
       if(This->exit!=NULL)free(This->exit);
       This->exit = malloc( sizeof(double) * (This->get_nenter(This)));

       for(unsigned int n=0;n< This->get_nenter(This);n++){
       This->exit[n] = exit[n];
       }
       return 1;
}

/******************************************************************************/
static int forming(Perceptron *This)
{  
    double r[This->get_nenter(This)];
    double s[This->get_nenter(This)];
    for(unsigned int e=0;e<This->get_nenter(This);e++){
       r[e]=-1;
	for(unsigned int n=0;n<This->get_nprev(This);n++){	   
	    r[e]+=(This->get_prev(This)[n]->get_exit(This->get_prev(This)[n])[e]
              * (This->get_weights(This)[n]) );
        printf("-->%lf\t*\t%lf\n",(This->get_prev(This)[n]->get_exit(This->get_prev(This)[n])[e]), (This->get_weights(This)[n]) );
       }
	      
       s[e]=sigmoid(r[e]); 
       printf("------------>%lf\t------------>%lf\n",r[e],s[e]);
    }
    This->set_exit(This,s);
       
    return 1;
}

/******************************************************************************/
static char* toString( Perceptron *This)
{
       size_t size = sizeof(char) * 100;
       char* string = (char*)malloc(size);
       
       snprintf(string, size, "Exit: %0.5lf\t%0.5lf\t%0.5lf\t%0.5lf\n"
                                                 ,This->get_exit(This)[0]
                                                 ,This->get_exit(This)[1]
                                                 ,This->get_exit(This)[2]
                                                 ,This->get_exit(This)[3] 
                                                 );
       
       return string;
}

