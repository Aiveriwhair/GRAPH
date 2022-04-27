#include <stdlib.h>
#include "pile.h"

ppile_t creer_pile ()
{
  ppile_t pile = malloc(sizeof(pile_t)); // struct ?
  pile->sommet = 0;
  return pile;
}

int detruire_pile (ppile_t p)
{
  free(p);
  return 1;
}  

int pile_vide (ppile_t p)
{
  return p->sommet == 0;
}

int pile_pleine (ppile_t p)
{
  return p->sommet == MAX_PILE_SIZE;
} 

psommet_t depiler (ppile_t p)
{
  if (pile_vide(p))
    return NULL;
  p->sommet--;
  return p->Tab[p->sommet];
}

int empiler (ppile_t p, psommet_t pn)
{
  if (p->sommet == MAX_PILE_SIZE)
    return 0;
  p->sommet++;
  p->Tab[p->sommet-1] = pn;
  return 1;
}
