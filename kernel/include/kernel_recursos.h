#ifndef KERNEL_RECURSOS_H_
#define KERNEL_RECURSOS_H_

#include "kernel_gestor.h"
#include "kernel_estados.h"

// FUNCIONES
// Recurso
t_recurso *crear_recurso(int instancias);
void eliminar_recurso(void *recurso);
t_estado *recurso_get_estado_recurso(t_recurso *recurso);
int recurso_get_instancias(t_recurso *recurso);
void recurso_wait(t_recurso *recurso);
void recurso_signal(t_recurso *recurso);
bool recurso_debe_desbloquear_proceso(t_recurso *recurso);
t_pcb *recurso_desencolar_primer_proceso(t_recurso *recurso);

// Diccionario Recursos
t_dictionary *crear_diccionario_recursos(char** recursos, char** instancias_recursos);
void eliminar_diccionario_recursos(t_dictionary *diccionario_recursos);
t_recurso *diccionario_recursos_get_recurso(t_dictionary *diccionario_recursos, char *nombre_recurso);
bool diccionario_recursos_existe_recurso(t_dictionary *diccionario_recursos, char *nombre_recurso);

#endif