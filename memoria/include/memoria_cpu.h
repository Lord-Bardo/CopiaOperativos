#ifndef MEMORIA_CPU_H_
#define MEMORIA_CPU_H_

#include "memoria_utils.h"

void atender_memoria_cpu(void);
void obtener_instruccion(int pid, int pc, char* instruccion);
void obtener_frame(int pid, int num_pag, int* num_frame);

#endif