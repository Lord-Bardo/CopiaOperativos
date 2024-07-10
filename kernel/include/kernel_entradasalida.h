#ifndef KERNEL_ENTRADASALIDA_H_
#define KERNEL_ENTRADASALIDA_H_

#include "kernel_gestor.h"
#include "kernel_estados.h"

void atender_kernel_interfaz(void *nombre_interfaz);
t_interfaz *crear_interfaz(int fd_interfaz);
void eliminar_interfaz(t_interfaz *interfaz);
int interfaz_get_fd(t_interfaz *interfaz);
t_estado *interfaz_get_estado_interfaz(t_interfaz *interfaz);
t_pcb *interfaz_desencolar_primer_proceso(t_interfaz *interfaz);

t_interfaz *diccionario_interfaces_get_interfaz(t_dictionary *diccionario_interfaces, char *nombre_interfaz);
bool diccionario_interfaces_existe_interfaz(t_dictionary *diccionario_interfaces, char *nombre_interfaz);

#endif