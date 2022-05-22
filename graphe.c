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
int nombre_aretes_chemin(chemin_t c){
  parc_t p = c.liste_arcs;
  int n = 0;
  while (p != NULL)
  {
    n++;
    p = p->arc_suivant;
  }
  return n;
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

void init_marqueur_arc(pgraphe_t g)
{
  //DEBUG_PRINT(("init_marqueur_arc\n"));
  psommet_t p = g;

  while (p != NULL)
  {
    //DEBUG_PRINT(("Marking %d\n", p->label));
    parc_t a = p->liste_arcs;
    while (a != NULL){
      //DEBUG_PRINT(("\tMarking %d\n", a->dest->label));
      a->marqueur = 0;
      a = a->arc_suivant; 
    }
    p = p->sommet_suivant; 
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
    p->distance = -1;
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
      if (a->dest->distance != 1 || a->dest->distance > a->poids + p->distance)
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
  int degmax = 0;
  psommet_t p = g;
  while(p != NULL){
    int degTemp = degre_entrant_sommet(g, p) + degre_sortant_sommet(g, p);
    if(degTemp > degmax)
      degmax = degTemp;
    p = p->sommet_suivant;
  }

  return degmax;
}

int degre_minimal_graphe(pgraphe_t g)
{
  int degmin = degre_maximal_graphe(g);
  psommet_t p = g;
  while(p != NULL){
    int degTemp = degre_entrant_sommet(g, p) + degre_sortant_sommet(g, p);
    if(degTemp < degmin)
      degmin = degTemp;
    p = p->sommet_suivant;
  }
  return degmin;
}

int independant(pgraphe_t g)
{
  /* Les aretes du graphe n'ont pas de sommet en commun */
  return degre_maximal_graphe(g) <= 1 ? 1 : 0;  
}

int complet(pgraphe_t g)
{
  /* Toutes les paires de sommet du graphe sont jointes par un arc */
  int nbSommet = nombre_sommets(g);
  psommet_t p = g;
  while (p != NULL) {
    if (degre_sortant_sommet(g, p) != nbSommet - 1){
      return 0;
    }
    p = p->sommet_suivant; 
  }
  return 1;
}

int regulier(pgraphe_t g)
{
  /*
     graphe regulier: tous les sommets ont le meme degre
     g est le ponteur vers le premier sommet du graphe
     renvoie 1 si le graphe est régulier, 0 sinon
  */
  if (degre_minimal_graphe(g) == degre_maximal_graphe(g))
    return 1;
  return 0;
}

int elementaire(pgraphe_t g, chemin_t c){
  /*
    1 -> chemin élémentaire :  ne passant pas 2 fois par le meme noeud
    0 -> sinon 
  */
  psommet_t p = g;
  init_marqueur_sommet(p);
  c.sommet_depart->marqueur = 1;

  parc_t arcs = c.liste_arcs;
  while (arcs != NULL)
  {
    if (arcs->dest->marqueur == 1)
    {
      return 0;
    }
    arcs->dest->marqueur = 1;
    arcs = arcs->arc_suivant;
  }
  return 1;
}
int simple(pgraphe_t g, chemin_t c){
  parc_t p = c.liste_arcs;
  while (p != NULL)
  {
    parc_t temp = g->liste_arcs;
    while (temp != NULL)
    {
      if (temp == p)
      {
        return 0;
      }
      
      temp = temp->arc_suivant;
    }
    
    p = p->arc_suivant;
  }
  return 1;
}
int eulerien(pgraphe_t g, chemin_t c){
  init_marqueur_arc(g);
  int arcRencontres = 0;

  parc_t arcs = c.liste_arcs;

  while (arcs != NULL) {
    if (arcs->marqueur == 0)
    {
      arcRencontres++;
      arcs->marqueur = 1;
    }
    arcs = arcs->arc_suivant;
  }
  return arcRencontres == nombre_arcs(g) ? 1 : 0;
}

int hamiltonien(pgraphe_t g, chemin_t c){
  init_marqueur_sommet(g);
  int noeudRencontres = 1;
  c.sommet_depart->marqueur = 1;
  psommet_t p = g;
  parc_t arcs = c.liste_arcs;
  while (arcs != NULL) {
    if (arcs->dest->marqueur == 0)
    {
      noeudRencontres++;
      arcs->dest->marqueur = 1;
    }
    arcs = arcs->arc_suivant;
  }
  return noeudRencontres == nombre_sommets(g) ? 1 : 0;
}

int graphe_eulerien_rec(psommet_t s, pchemin_t c, int n) {
  /*
    Cette fonction crée tous les chemins possibles
    entre le sommet s et tous les autres sommets du graphe
  */
  for(int i = 0; i < n; i++) {
    DEBUG_PRINT(("\t"));
  }
  DEBUG_PRINT(("graphe_eulerien_rec(%d)\n", s->label));
  if (eulerien(s, *c)) {
    return 1;
  }
  for (int i = 0; i < n; i++)
  {
    DEBUG_PRINT(("\t"));
  }
  DEBUG_PRINT(("Chemin pas eulerien\n"));
  parc_t arcs = s->liste_arcs;
  while (arcs != NULL) {
    if(arcs->dest != s) {
      parc_t temp = c->liste_arcs;
      if(temp == NULL) {
        parc_t arc_temp = malloc(sizeof(parc_t));
        arc_temp->dest = arcs->dest;
        arc_temp->arc_suivant = NULL;
        c->liste_arcs = arc_temp;
      } else {
        while (temp->arc_suivant != NULL)
        {
          temp = temp->arc_suivant;
        }
        parc_t arc_temp = malloc(sizeof(parc_t));
        arc_temp->dest = arcs->dest;
        arc_temp->arc_suivant = NULL;
        temp->arc_suivant = arcs;
      }
      for (int i = 0; i < n; i++)
      {
        DEBUG_PRINT(("\t"));
      }
      DEBUG_PRINT(("Added arc from %d to %d to chemin\n", s->label, arcs->dest->label));
      if(simple(s, *c)) {
        for (int i = 0; i < n; i++)
        {
          DEBUG_PRINT(("\t"));
        }
        DEBUG_PRINT(("Chemin is simple\n"));
        if(graphe_eulerien_rec(arcs->dest, c, n+1)) {
          return 1;
        }
      } else {
        for (int i = 0; i < n; i++)
        {
          DEBUG_PRINT(("\t"));
        }
        DEBUG_PRINT(("Chemin is not simple\n"));
      }
      if(temp == NULL) {
        c->liste_arcs = NULL;
      } else {
        temp->arc_suivant = NULL;
      }
      //temp->arc_suivant = NULL;
    }
    arcs = arcs->arc_suivant;
  }
  return 0;
}

int graphe_eulerien(pgraphe_t g){
  psommet_t p = g;
  while (p != NULL) {
    chemin_t* c = malloc(sizeof(chemin_t));
    c->sommet_depart = p;
    parc_t arc = p->liste_arcs;
    while (arc != NULL) {
      simple(p, *c);
      if(graphe_eulerien_rec(p, c, 0)) {
        return 1;
      }
      arc = arc->arc_suivant;
    }
    p = p->sommet_suivant;
  }
  return 0;
}
int graphe_hamiltonien(pgraphe_t g);


int distance(pgraphe_t g, int x, int y){
  /*
    Renvoie la distance entre les sommets x et y dadans le graphe g
  */
  psommet_t px = chercher_sommet(g, x);
  if(px == NULL) {
    DEBUG_PRINT(("Le sommet %d n'existe pas\n", x));
    return -1;
  }
  psommet_t py = chercher_sommet(g, y);
  if (py == NULL) {
    DEBUG_PRINT(("Le sommet %d n'existe pas\n", y));
    return -1;
  }
  algo_dijkstra(g, x);
  return py->distance;
}


int excentricite(pgraphe_t g, int n){
  /*
    Renvoie la distance maximale entre n et tous les autres sommets du graphe
  */
  int max = 0;
  algo_dijkstra(g, n);
  psommet_t p = g;
  while (p != NULL) {
    if (p->distance > max) {
      max = p->distance;
    }
    p = p->sommet_suivant;
  }
  return max;
}

int diametre(pgraphe_t g) {
  /*
    Renvoie la distance maximale entre deux sommets du graphe
  */
  int max = 0;
  psommet_t p = g;
  while (p != NULL) {
    int temp = excentricite(g, p->label);
    printf("Excentricite de %d : %d\n", p->label, temp);
    if (temp > max) {
      max = temp;
    }
    p = p->sommet_suivant;
  }
  return max;
}