#ifndef MEMORIA_CPU_H_
#define MEMORIA_CPU_H_

#include "memoria_utils.h"

void atender_memoria_cpu(void);
void obtener_instruccion(int pid, int pc, char* instruccion);
void obtener_frame(int pid, int pag, int* frame);
void escribir(int frame, int offset, void* dato, int bytes);

#endif