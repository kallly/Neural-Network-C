#ifndef CGI_PERCEPTRON_H
#define CGI_PERCEPTRON_H

#include <time.h>
#include <stdbool.h> 

#ifdef __cplusplus
    extern "C" {
#endif
typedef enum FuncName{
    SIGMOID,
    HEAVISIDE
}FuncName;

typedef struct Synapse
{
    unsigned int id;
    double weight;
}Synapse;

/*  Structure représantant l'objet Perceptron. */
typedef struct Perceptron
{
    int                     (*destructor)(struct Perceptron*);

    char*                   (*toString)(struct Perceptron*);

    struct Perceptron**	    (*get_prev)(struct Perceptron*);
    int			            (*set_prev)(struct Perceptron*,struct Perceptron**,unsigned int);
    struct Perceptron** 	(*get_next)(struct Perceptron*);
    int			            (*set_next)(struct Perceptron*,struct Perceptron**,unsigned int);
    
    unsigned int		    (*get_nenter)(struct Perceptron*);
    int			            (*set_nenter)(struct Perceptron*,unsigned int);
    unsigned int		    (*get_nprev)(struct Perceptron*);
    int			            (*set_nprev)(struct Perceptron*,unsigned int);
    unsigned int		    (*get_nnext)(struct Perceptron*);
    int			            (*set_nnext)(struct Perceptron*,unsigned int);

    Synapse*		        (*get_synapse)(struct Perceptron*,unsigned int);
    int			            (*set_synapse)(struct Perceptron*,unsigned int,double);
    double*		            (*get_exit)(struct Perceptron*);
    int			            (*set_exit)(struct Perceptron*,double*);
    double*		            (*get_exitDelta)(struct Perceptron*);
    int			            (*set_exitDelta)(struct Perceptron*,double*);
        

    int                     (*forming)(struct Perceptron*);
    int                     (*formingDelta)(struct Perceptron*);
    int                     (*update)(struct Perceptron*);
    
    unsigned int		    (*get_nformed)(struct Perceptron*);
    int			            (*set_nformed)(struct Perceptron*,unsigned int);
    unsigned int            (*get_nformedDelta)(struct Perceptron*);
    int                     (*set_nformedDelta)(struct Perceptron*,unsigned int);
    
    double                  (*funcAct)(double);
    double                  (*funcAct_b)(double);
    double                  (*funcAct_d)(double);
    
    struct Perceptron **prev, **next, *solve;
    Synapse **synapses;

    unsigned int nenter,nprev,nnext,id;
    
    double *exit,*exitDelta;

    unsigned int nformed,nformedDelta;

}Perceptron;

Perceptron* New_Perceptron(unsigned int id,unsigned int nenter,Perceptron **prev,unsigned int nprev,Perceptron **next,unsigned int nnext,FuncName funcActName);

#ifdef __cplusplus
}
#endif

#endif
            