#ifndef KERNEL_RECURSOS_H_
#define KERNEL_RECURSOS_H_

#include "kernel_gestor.h"
#include "kernel_estados.h"

// FUNCIONES
t_recurso *crear_recurso(int instancias);
void eliminar_recurso(void *recurso);
t_dictionary *crear_diccionario_recursos(char** recursos, char** instancias_recursos);
void eliminar_diccionario_recursos(t_dictionary *diccionario_recursos);

#endif