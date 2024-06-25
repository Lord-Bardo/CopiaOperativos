#ifndef KERNEL_PLANIFICADOR_H_
#define KERNEL_PLANIFICADOR_H_

#include "kernel_gestor.h"
#include "kernel_utils.h"
#include "kernel_pcb.h"
#include "kernel_estados.h"
#include "kernel_recursos.h"

// FUNCIONES
void iniciar_planificadores(void);
void inicializar_estructuras(void);
void inicializar_estructuras_estados(void);
void inicializar_estructuras_pid(void);
void inicializar_semaforos(void);
void inicializar_diccionario_recursos(void);
void iniciar_planificacion(void);
void detener_planificacion(void);
void cambiar_grado_multiprogramacion_a(int nuevo_grado_multiprogramacion);
void planificador_largo_plazo(void);
void liberar_procesos_exit(void);
void planificador_corto_plazo(void);
void planificador_corto_plazo_fifo(void);
void planificador_corto_plazo_rr(void);
void planificador_corto_plazo_vrr(void);
t_pcb *elegir_proceso_segun_fifo(void);
t_pcb *elegir_proceso_segun_rr(void);
t_pcb *elegir_proceso_segun_vrr(void);
void proceso_a_exec(t_pcb *pcb);
void enviar_contexto_de_ejecucion(t_pcb *pcb);
void recibir_contexto_de_ejecucion_actualizado();
void iniciar_proceso(char *path);
void proceso_a_ready(t_pcb *pcb);
void proceso_a_exit(t_pcb *pcb);
void pedir_a_memoria_iniciar_proceso(int pid, char *path);
void pedir_a_memoria_finalizar_proceso(int pid);
t_codigo_operacion recibir_confirmacion_memoria_proceso_iniciado(void);
void finalizar_proceso(int pid);

#endif