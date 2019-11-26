#include <stdlib.h>
#include <stdio.h>
 
#include "Rectangle.h"

/*Rectangle; ./a.out  100 30*/


int main(int argc,const char* argv[])
{
    Rectangle *rectangle;
    
    if(argc > 2)rectangle = New_Rectangle(atoi(argv[1]),atoi(argv[2]));
    else rectangle = New_Rectangle(10,20);

	Rectangle *rectangle2 = New_Rectangle(20,20);

    printf("\nlongueur: %0.2f\nlargeur: %0.2f",rectangle->get_longueur(rectangle),rectangle->get_largeur(rectangle));
    printf("\nperimetre: %0.2f",rectangle->perimetre(rectangle));
    printf("\nsurface: %0.2f",rectangle->aire(rectangle));
    if(rectangle->isCarre(rectangle)) printf("\nc'est un carré\n");
    else printf("\nce n'est pas un carré\n");
    printf("%s\n",rectangle->afficherRectangle(rectangle));

	printf("\nlongueur: %0.2f\nlargeur: %0.2f",rectangle2->get_longueur(rectangle2),rectangle2->get_largeur(rectangle2));
    printf("\nperimetre: %0.2f",rectangle2->perimetre(rectangle2));
    printf("\nsurface: %0.2f",rectangle2->aire(rectangle2));
    if(rectangle2->isCarre(rectangle2)) printf("\nc'est un carré\n");
    else printf("\nce n'est pas un carré\n");
    printf("%s\n",rectangle2->afficherRectangle(rectangle2));



#ifdef __WIN32__
        system("PAUSE");
#endif
        return 0;
}

