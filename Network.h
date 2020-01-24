#ifndef CGI_NETWORK_H
#define CGI_NETWORK_H

#include "activation.h"
#include "Perceptron.h"

#ifdef __cplusplus
    extern "C" {
#endif

/*  Structure représantant l'objet Network. */
typedef struct Network
    {
        int             train(struct Network*);
    } Network ;


Network* New_Network();

#ifdef __cplusplus
}
#endif

#endif
            