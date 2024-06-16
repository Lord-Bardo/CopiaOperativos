#ifndef ENTRADASALIDA_KERNEL_H_
#define ENTRADASALIDA_KERNEL_H_

#include "entradasalida_gestor.h"

void atender_entradasalida_kernel(void);
void manejar_peticion_kernel(char* peticion);
char* recibir_peticion_kernel(int fd_kernel);

#endif