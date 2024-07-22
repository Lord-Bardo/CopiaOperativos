#ifndef MMU_H_
#define MMU_H_

#include "cpu_gestor.h"






int min(int a, int b);
int obtener_pagina(int dl);
int obtener_offset(int dl, int pagina);
int obtener_frame(int pagina);
int obtener_bytes(int offset,int tamanio_total);
void mmu(t_direccion *direcciones, int dl, int tamanio_total);
int obtener_pagina(int dl);

void escribir_un_byte(int df,void *regd);
void escribir_un_frame(int df, int bytes, void *valor);
void mmu_escribir(int dl,int bytes, void *valor);

void mmu_leer(int dl, int bytes, void * valor);
void leer_un_frame(int df, int bytes, void * dato);
void leer_un_byte(int df,void *valor);
#endif
