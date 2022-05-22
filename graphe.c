#define DEBUG

#ifdef DEBUG
#define DEBUG_PRINT(x) printf x
#else
#define DEBUG_PRINT(x) \
  do                   \
  {                    \
  } while (0)
#endif

/*
  Structures de type graphe
  Structures de donnees de type liste
  (Pas de contrainte sur le nombre de noeuds des  graphes)
*/

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "file.h"
#include "pile.h"
#include "graphe.h"

psommet_t chercher_sommet(pgraphe_t g, int label)
{
  psommet_t s;

  s = g;

  while ((s != NULL) && (s->label != label))
  {
    s = s->sommet_suivant;
  }
  return s;
}

parc_t existence_arc(parc_t l, psommet_t s)
{
  parc_t p = l;

  while (p != NULL)
  {
    if (p->dest == s)
      return p;
    p = p->arc_suivant;
  }
  return p;
}

void ajouter_arc(psommet_t o, psommet_t d, int distance)
{
  parc_t parc;

  parc = (parc_t)malloc(sizeof(arc_t));

  if (existence_arc(o->liste_arcs, d) != NULL)
  {
    fprintf(stderr, "ajout d'un arc deja existant\n");
    exit(-1);
  }

  parc->poids = distance;
  parc->dest = d;
  parc->arc_suivant = o->liste_arcs;
  o->liste_arcs = parc;
  return;
}

// ===================================================================

int nombre_sommets(pgraphe_t g)
{
  psommet_t p = g;
  int nb = 0;

  while (p != NULL)
  {
    nb = nb + 1;
    p = p->sommet_suivant;
  }

  return nb;
}

int nombre_arcs(pgraphe_t g)
{

  psommet_t p = g;
  int nb_arcs = 0;

  while (p != NULL)
  {
    parc_t l = p->liste_arcs;

    while (l != NULL)
    {
      nb_arcs = nb_arcs + 1;
      l = l->arc_suivant;
    }

    p = p->sommet_suivant;
  }
  return nb_arcs;
}

void init_couleur_sommet(pgraphe_t g)
{
  psommet_t p = g;

  while (p != NULL)
  {
    p->couleur = 0;        // couleur indefinie
    p = p->sommet_suivant; // passer au sommet suivant dans le graphe
  }

  return;
}

int colorier_graphe(pgraphe_t g)
{
  /*
    coloriage du graphe g

    datasets
    graphe data/gr_planning
    graphe data/gr_sched1
    graphe data/gr_sched2
  */

  psommet_t p = g;
  parc_t a;
  int couleur;
  int max_couleur = INT_MIN; // -INFINI

  int change;

  init_couleur_sommet(g);

  while (p != NULL)
  {
    couleur = 1; // 1 est la premiere couleur

    // Pour chaque sommet, on essaie de lui affecter la plus petite couleur

    // Choix de la couleur pour le sommet p

    do
    {
      a = p->liste_arcs;
      change = 0;

      while (a != NULL)
      {
        if (a->dest->couleur == couleur)
        {
          couleur = couleur + 1;
          change = 1;
        }
        a = a->arc_suivant;
      }

    } while (change == 1);

    // couleur du sommet est differente des couleurs de tous les voisins

    p->couleur = couleur;
    if (couleur > max_couleur)
      max_couleur = couleur;

    p = p->sommet_suivant;
  }

  return max_couleur;
}

void init_marqueur_sommet(pgraphe_t g)
{
  psommet_t p = g;

  while (p != NULL)
  {
    p->marqueur = 0;        // reset le marqueur
    p = p->sommet_suivant; // passer au sommet suivant dans le graphe
  }

  return;
}

int nombre_voisins(psommet_t s){
  int res = 0;
  parc_t l = s->liste_arcs;
  while (l != NULL)
  {
    res++;
    l = l->arc_suivant;
  }
  return res;
}

void afficher_graphe_largeur(pgraphe_t g, int r)
{
  printf("-- Affichage du graphe en largeur --\n");
  init_marqueur_sommet(g);
  psommet_t s = chercher_sommet(g, r);
  pfile_t file = creer_file();
  enfiler(file, s);
  s->marqueur = 1;

  while (!file_vide(file))
  {
    s = defiler(file);
    printf(">Sommet %d ", s->label);

    parc_t arc = s->liste_arcs;
    while (arc != NULL)
    {
      psommet_t voisin = arc->dest;
      if (voisin->marqueur == 0)
      {
        enfiler(file, voisin);
        voisin->marqueur = 1;
      }
      arc = arc->arc_suivant;
    }
    printf("\n");
  }
  return;
}


void explorer(pgraphe_t g, int r)
{
  psommet_t p = chercher_sommet(g, r);
  
  if (p->marqueur == 0){
  printf("Sommet %d#  ", p->label);
  p->marqueur = 1;
  }
  else {
    return;
  }

  parc_t arcs = p->liste_arcs;

  while(arcs  != NULL && arcs->dest != NULL){
    explorer(g, arcs->dest->label);
    arcs = arcs->arc_suivant;
  }
}

void afficher_graphe_profondeur(pgraphe_t g, int r)
{
  init_marqueur_sommet(g);
  explorer(g,r);
  printf("\n");

  return;
}

void algo_dijkstra(pgraphe_t g, int r)
{
  DEBUG_PRINT(("-- Algorithme de Dijkstra --\n"));
  /*
    algorithme de dijkstra
    des variables ou des chanmps doivent etre ajoutees dans les structures.
  */
  psommet_t p = g;
  psommet_t *P = malloc(sizeof(psommet_t) * nombre_sommets(g));
  int n = 0;
  int existe = 0;
  while (p != NULL)
  {
    p->distance = INT_MAX - 1;
    if (p->label == r)
    {
      p->distance = 0;
      existe = 1;
    }
    p = p->sommet_suivant;
  }
  if (!existe)
  {
    DEBUG_PRINT(("Le sommet %d n'existe pas\n", r));
    return;
  }
  p = g;
  while (p != NULL)
  {
    DEBUG_PRINT(("\tCalculating routes from : %d\n", p->label));
    P[n] = p;
    parc_t a = p->liste_arcs;
    while (a != NULL)
    {
      if (a->dest->distance > a->poids + p->distance)
      {
        DEBUG_PRINT(("\t\tAssigned %d to %d\n", a->poids + p->distance, a->dest->label));
        a->dest->distance = a->poids + p->distance;
      }
      a = a->arc_suivant;
    }
    p = p->sommet_suivant;
  }
  DEBUG_PRINT(("-- Fin de l'algorithme de Dijkstra --\n"));
  return;
}

// ======================================================================

int degre_sortant_sommet(pgraphe_t g, psommet_t s)
{
  /*
    Cette fonction retourne le nombre d'arcs sortants
    du sommet n dans le graphe g
  */
  psommet_t p = chercher_sommet(g, s->label);
  parc_t arcs = p->liste_arcs;
  int count = 0;
  while(arcs != NULL){
    arcs = arcs->arc_suivant;
    count++;
  }
  return count;
}

int degre_entrant_sommet(pgraphe_t g, psommet_t s)
{
    /*
    Cette fonction retourne le nombre d'arcs entrant
    du noeud n dans le graphe g
  */
  psommet_t p = g;
  int count = 0;
  while (p)
  {
    parc_t arcs = p->liste_arcs;
    while(arcs != 0){
      if (arcs->dest->label == s->label)
      {
        count++;
      }
      arcs = arcs->arc_suivant;
    }
    p = p->sommet_suivant;
  }
  return count;
}

int degre_maximal_graphe(pgraphe_t g)
{
  /*
    Max des degres des sommets du graphe g
  */

  return 0;
}

int degre_minimal_graphe(pgraphe_t g)
{
  /*
    Min des degres des sommets du graphe g
  */

  return 0;
}

int independant(pgraphe_t g)
{
  /* Les aretes du graphe n'ont pas de sommet en commun */

  return 0;
}

int complet(pgraphe_t g)
{
  /* Toutes les paires de sommet du graphe sont jointes par un arc */

  return 0;
}

int regulier(pgraphe_t g)
{
  /*
     graphe regulier: tous les sommets ont le meme degre
     g est le ponteur vers le premier sommet du graphe
     renvoie 1 si le graphe est régulier, 0 sinon
  */

  return 0;
}

/*
  placer les fonctions de l'examen 2017 juste apres
*/
