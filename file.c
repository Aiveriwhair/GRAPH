#include <stdlib.h>
#include "file.h"

pfile_t creer_file ()
{
  pfile_t file = malloc(sizeof(file_t));
  file->queue = 0;
  file->tete = 0;
  file->nbElems = 0;
  return file;
}

int detruire_file (pfile_t f)
{
  free(f);
  return 1;
}


int file_vide (pfile_t f)
{
  return f->nbElems == 0;
}

int file_pleine (pfile_t f)
{
  return f->nbElems == MAX_FILE_SIZE;
}

psommet_t defiler (pfile_t f)
{
  if(file_vide(f)) {
    return NULL;
  }
  f->nbElems -= 1;
  f->tete = (f->tete + 1) % 32;
  if(f->tete == 0) return f->Tab[31];
  return f->Tab[f->tete - 1];
}

int enfiler (pfile_t f, psommet_t p)
{
  if(file_pleine(f)) {
    return 0;
  }
  f->nbElems += 1;
  f->queue = (f->queue + 1) % 32;
  if(f->queue == 0) f->Tab[31] = p;
  else f->Tab[f->queue - 1] = p;
  return 1;
}
