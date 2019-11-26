#ifndef CGI_PERCEPTRON_H
#define CGI_PERCEPTRON_H

#include <time.h>

#ifdef __cplusplus
    extern "C" {
#endif

/*  Structure représantant l'objet Perceptron. */
typedef struct Perceptron
    {
        char*                   (*toString)(struct Perceptron*);

        struct Perceptron**		(*get_prev)(struct Perceptron*);
        int			            (*set_prev)(struct Perceptron*,struct Perceptron**,unsigned int);
        struct Perceptron**		(*get_next)(struct Perceptron*);
        int			            (*set_next)(struct Perceptron*,struct Perceptron**,unsigned int);
        double		            (*get_weight)(struct Perceptron*);
        int			            (*set_weight)(struct Perceptron*,double);
        double		            (*get_exit)(struct Perceptron*);
        int			            (*set_exit)(struct Perceptron*,double);
        unsigned int		    (*get_nprev)(struct Perceptron*);
        int			            (*set_nprev)(struct Perceptron*,unsigned int);
        unsigned int		    (*get_nnext)(struct Perceptron*);
        int			            (*set_nnext)(struct Perceptron*,unsigned int);
        
        int                     (*forming)(struct Perceptron*);

        struct Perceptron **prev;
        struct Perceptron **next;
        unsigned int nprev;
        unsigned int nnext;
        
        double weight,exit;
    } Perceptron ;


Perceptron* New_Perceptron(Perceptron **prev,unsigned int nprev,Perceptron **next,unsigned int nnext);

#ifdef __cplusplus
}
#endif

#endif
            