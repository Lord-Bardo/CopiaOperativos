#ifndef KERNEL_PCB_H_
#define KERNEL_PCB_H_

#include "kernel_gestor.h"
#include "kernel_utils.h"
#include "kernel_planificadores.h"
#include "kernel_estados.h"

// PCB
t_pcb *crear_pcb(int pid, char* path);
void eliminar_pcb(t_pcb *pcb);
int generar_pid(void);
int pcb_get_pid(t_pcb *pcb);
int pcb_get_quantum_inicial(t_pcb *pcb);
void pcb_set_quantum_inicial(t_pcb *pcb, int quantum);
int pcb_get_quantum_restante(t_pcb *pcb);
void pcb_set_quantum_restante(t_pcb *pcb, int quantum);
t_nombre_estado pcb_get_estado(t_pcb *pcb);
t_registros pcb_get_registros(t_pcb *pcb);
void pcb_cambiar_estado_a(t_pcb *pcb, t_nombre_estado nuevo_estado);
uint32_t pcb_get_pc(t_pcb *pcb);
char *pcb_get_path(t_pcb *pcb);
char *pcb_get_nombre_recurso_causa_bloqueo(t_pcb *pcb);
void pcb_set_nombre_recurso_causa_bloqueo(t_pcb *pcb, char *nombre_recurso_causa_bloqueo);
t_dictionary *pcb_get_diccionario_recursos_usados(t_pcb *pcb);
bool pcb_usa_recurso(t_pcb *pcb, char *nombre_recurso);
bool pcb_esta_bloqueado_por_recurso(t_pcb *pcb);

// DICCIONARIO PROCESOS A FINALIZAR
void diccionario_procesos_a_finalizar_agregar_proceso(t_dictionary *diccionario_procesos_a_finalizar, t_pcb *pcb);
bool diccionario_procesos_a_finalizar_proceso_esta_pendiente_de_finalizar(t_dictionary *diccionario_procesos_a_finalizar, t_pcb *pcb);
void diccionario_procesos_a_finalizar_remover_proceso(t_dictionary *diccionario_procesos_a_finalizar, t_pcb *pcb);

// CONTAR QUANTUM PARAMETROS
t_contar_quantum_parametros *crear_contar_quantum_parametros(int quantum, int pid);
void eliminar_contar_quantum_parametros(t_contar_quantum_parametros *contar_quantum_parametros);
int contar_quantum_parametros_get_quantum(t_contar_quantum_parametros *contar_quantum_parametros);
void contar_quantum_parametros_set_quantum(t_contar_quantum_parametros *contar_quantum_parametros, int quantum);
int contar_quantum_parametros_get_pid(t_contar_quantum_parametros *contar_quantum_parametros);

#endif