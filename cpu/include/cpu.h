#ifndef CPU_H_
#define CPU_H_

#include "cpu_gestor.h"
#include "inicializar_cpu.h"
#include "cpu_kernel_dispatch.h"
#include "cpu_kernel_interrupt.h"
#include "cpu_memoria.h"
#include "cpu_utils.h"

// VARIABLES GLOBALES
t_log* cpu_logger;
t_config* cpu_config;

char* IP_MEMORIA;
char* PUERTO_MEMORIA;
char* PUERTO_ESCUCHA_DISPATCH;
char* PUERTO_ESCUCHA_INTERRUPT;
int CANTIDAD_ENTRADAS_TLB;
char* ALGORITMO_TLB;

int fd_cpu_dispatch;
int fd_cpu_interrupt;
int fd_kernel_dispatch;
int fd_kernel_interrupt;
int fd_memoria;
t_pcb *pcb;

// FUNCIONES
void leer_consola(t_log*);
void paquete(int);
void terminar_programa();
void iniciar_ciclo_instruccion(t_pcb);
<<<<<<< HEAD

#endif

=======
>>>>>>> 36fe3b811fb22fc0fd4980813174401221e1127b

#endif