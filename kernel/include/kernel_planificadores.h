#ifndef KERNEL_PLANIFICADOR_H_
#define KERNEL_PLANIFICADOR_H_

#include "kernel_gestor.h"
#include "kernel_utils.h"
#include "kernel_pcb.h"
#include "kernel_estados.h"
#include "kernel_recursos.h"
#include "kernel_entradasalida.h"

// Motivos finalizacion
#define FINALIZACION_SUCCESS "SUCCESS"
#define FINALIZACION_INVALID_RESOURCE "INVALID_RESOURCE"
#define FINALIZACION_INVALID_INTERFACE "INVALID_INTERFACE"
#define FINALIZACION_OUT_OF_MEMORY "OUT_OF_MEMORY"
#define FINALIZACION_INTERRUPTED_BY_USER "INTERRUPTED_BY_USER"
#define FINALIZACION_ERROR "ERROR" // lo agregue yo para los casos q no caen en ninguno de los anteriores
#define FINALIZACION_DESCONEXION_INTERFAZ "DESCONEXION_INTERFAZ" // lo agregue yo

// INICIALIZACION PLANIFICADORES
void iniciar_planificadores(void);
void inicializar_estructuras(void);
void inicializar_estructuras_planificacion(void);
void inicializar_estructuras_estados(void);
void inicializar_estructuras_pid(void);
void inicializar_estructuras_grado_multiprogramacion(void);
void inicializar_estructuras_diccionarios(void);
void inicializar_estructuras_sockets(void);
void iniciar_planificacion(void);
void detener_planificacion(void);
void detener_planificacion_new_to_ready(void);
void detener_planificacion_ready_to_exec(void);
void detener_planificacion_blocked_to_ready_recurso(void);
void detener_planificacion_blocked_to_ready_interfaz(void);
void detener_planificacion_exec_to_exec_or_ready_or_blocked(void);
void cambiar_grado_multiprogramacion_a(int nuevo_grado_multiprogramacion);

// PLANIFICADOR CORTO PLAZO
void planificador_corto_plazo(void);
void planificador_corto_plazo_fifo(void);
t_pcb *elegir_proceso_segun_fifo(void);
void planificador_corto_plazo_rr(void);
t_pcb *elegir_proceso_segun_rr(void);
void contar_quantum(void *quantum_void);
void planificador_corto_plazo_vrr(void);
t_pcb *elegir_proceso_segun_vrr(void);
void enviar_contexto_de_ejecucion(t_pcb *pcb);
void manejar_motivo_desalojo(t_pcb *pcb, t_codigo_operacion motivo_desalojo, t_buffer *buffer, t_temporal *clock_exec, t_contar_quantum_parametros *contar_quantum_parametros);

// PLANIFICADOR LARGO PLAZO
void iniciar_proceso(char *path);
void finalizar_proceso(int pid);
void planificador_largo_plazo(void);
t_codigo_operacion pedir_a_memoria_iniciar_proceso(int pid, char *path);
void liberar_procesos_exit(void);
t_codigo_operacion pedir_a_memoria_finalizar_proceso(int pid);
void liberar_recursos_usados(t_pcb *pcb);
void liberar_recurso(t_pcb *pcb, char *nombre_recurso);
void ejecutar_instruccion_signal(t_pcb *pcb, char *nombre_recurso);
void ejecutar_instruccion_wait(t_pcb *pcb, char *nombre_recurso);

// PROCESO A ...
void proceso_a_ready(t_pcb *pcb);
void proceso_a_ready_plus(t_pcb *pcb);
void proceso_a_exec(t_pcb *pcb);
void proceso_a_blocked(t_pcb *pcb, char *motivo_bloqueo);
void proceso_a_exit(t_pcb *pcb, char *motivo_finalizacion);

#endif