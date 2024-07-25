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
void mmu_escribir_copy_string(int dl,int tamanio,void * cadena);
void mmu_leer_copy_string(int dl,int tamanio,void * cadena);
void escribir_un_byte(int df,void *regd);
void escribir_un_frame(int df, int bytes, void *valor);
void mmu_escribir(int dl,int bytes, void *valor);

void mmu_leer(int dl, int bytes, void * valor);
void leer_un_frame(int df, int bytes, void * dato);
void leer_un_byte(int df,void *valor);

void reemplazar_entrada(t_entrada_tlb *nuevo);
void reemplazar_fifo(t_entrada_tlb * nuevo);
void reemplazar_lru(t_entrada_tlb * nuevo);
void *comparar_tiempo(void *entrada1,void * entrada2);
void agregar_entrada_tlb(int pid,int pagina, int frame);
int *min_puntero(int a, int b);
int obtener_cantidad_dirs(int dl,int tamanio);
void mmu_agregar_dirs_tamanio_paquete(int dl,int tamanio,t_paquete *paquete);
#endif
