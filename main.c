#include <stdlib.h>
#include <stdio.h>

#include "Perceptron.h"

int main()
{
    srand(time(NULL));
    Perceptron *entree[4][2];
    for(int n=0;n<4;n++){
        entree[n][0] = New_Perceptron(NULL,0,NULL,0);
        entree[n][1] = New_Perceptron(NULL,0,NULL,0);
    }
    printf("\n\n");
    Perceptron *perceptron[4];
    for(int n=0;n<4;n++)perceptron[n] = New_Perceptron(entree[n],(unsigned int)2,NULL,0);
    

    for(int n=0;n<4;n++){
        entree[n][0]->set_next(entree[n][0],(Perceptron**){&perceptron[n]},1);
        entree[n][1]->set_next(entree[n][1],&perceptron[n],1);
    }   

    //printf("%p\t,%p\n",(void*)entree,(void*)perceptron->get_prev(perceptron));
    //printf("%p\t,%p\n",(void*)entree[0]->next[0],(void*)perceptron);

    double solve[4];
    for(int n=0;n<4;n++){
        for(int i=0;i<2;i++){
            double temp;
            printf("%dx%d: ",n,i);
            scanf("%lf",&temp);
            entree[n][i]->set_exit(entree[n][i],temp);
        }
        printf("y%d: ",n);
        scanf("%lf",&solve[n]);
    }

    
    for(int i=0;i<400;i++){
        double som=0;
        for(int n=0;n<4;n++){
            perceptron[n]->forming(perceptron[n]);
            printf("\n%s%lf\n",perceptron[n]->toString(perceptron[n]),solve[n]);
            som+=perceptron[n]->get_exit(perceptron[n]);
        }
        for(int n=0;n<4;n++){
            double alpha = perceptron[n]->get_exit(perceptron[n]) - solve[n];
            perceptron[n]->set_weight(perceptron[n], (perceptron[n]->get_weight(perceptron[n]) - alpha ) );
        }
        printf("-------------------------------------------->>>>>>>>>>>>>>>>%lf\n",(solve[0]+solve[1]+solve[2]+solve[3] - som));
        
    }
    

#ifdef __WIN32__
        system("PAUSE");
#endif
        return 0;
}

