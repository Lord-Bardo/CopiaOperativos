#ifndef KERNEL_ENTRADASALIDA_H_
#define KERNEL_ENTRADASALIDA_H_

#include "kernel_gestor.h"
#include "kernel_estados.h"
#include "kernel_planificadores.h"

void atender_kernel_interfaz(void *nombre_interfaz);
// INTERFAZ
t_interfaz *crear_interfaz(int fd_interfaz, t_tipo_interfaz tipo_interfaz);
void eliminar_interfaz(void *interfaz_void);
t_tipo_interfaz interfaz_get_tipo_interfaz(t_interfaz *interfaz);
int interfaz_get_fd(t_interfaz *interfaz);
t_list *interfaz_get_lista_solicitudes_io(t_interfaz *interfaz);
pthread_mutex_t *interfaz_get_mutex_lista_solicitudes_io(t_interfaz *interfaz);
sem_t *interfaz_get_sem_lista_solicitudes_io(t_interfaz *interfaz);
sem_t *interfaz_get_sem_control_uso_interfaz(t_interfaz *interfaz);
t_solicitud_io *interfaz_desencolar_primera_solicitud_io(t_interfaz *interfaz);
void interfaz_encolar_solicitud_io(t_interfaz *interfaz, t_solicitud_io *solicitud_io);
// DICCIONARIO INTERFACES
t_interfaz *diccionario_interfaces_get_interfaz(t_dictionary *diccionario_interfaces, char *nombre_interfaz);
bool diccionario_interfaces_existe_interfaz(t_dictionary *diccionario_interfaces, char *nombre_interfaz);
void diccionario_interfaces_eliminar_interfaz(t_dictionary *diccionario_interfaces, char *nombre_interfaz);
// SOLICITUD IO
t_solicitud_io *crear_solicitud_io(t_pcb *pcb, t_paquete *paquete_solicitud_io);
void eliminar_solicitud_io(t_solicitud_io *solicitud_io);
t_pcb *solicitud_io_get_pcb(t_solicitud_io *solicitud_io);
t_paquete *solicitud_io_get_paquete_solicitud_io(t_solicitud_io *solicitud_io);

#endif