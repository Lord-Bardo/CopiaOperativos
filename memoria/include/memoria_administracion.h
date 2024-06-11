#ifndef MEMORIA_ADMINISTRACION_H_
#define MEMORIA_ADMINISTRACION_H_

//copie los include de memoria.h pero no se cuáles va a necesitar en realidad
#include "memoria_gestor.h"
#include "inicializar_memoria.h"
#include "memoria_entradasalida.h"
#include "memoria_kernel.h"
#include "memoria_cpu.h"
#include "instrucciones.c" // no sé si está bien esto

//VARIABLES GLOBALES
void* espacio_usuario; // supongo que con un malloc se puede inicializar y supongo que es donde estarán los frames con las páginas.
u_int32_t tabla_de_pags[]; // a cada proceso se le asignará una tabla en donde estarán todas sus páginas (supongo la cantidad de pags se lee del archivo de pseudocodigo). Se sabe que cada pag ocupa 32 bytes por eso usamos un array de u_int32_t.
t_instruccion proceso[]; // se almacenan, por cada proceso, sus instrucciones y, supongo, se los guarda en memoria (¿Las páginas deberían ser un array de instrucciones?)

//FUNCIONES


#endif