#ifndef CPU_UTILS_
#define CPU_UTILS_

#include "cpu_gestor.h"
#include "instrucciones.h"
#include "procesos.h"

void agregar_instruccion_paquete(t_paquete *paquete, char* instruccion);
void agregar_contexto_ejecucion_a_paquete(t_paquete *paquete, t_pcb* pcb);
void agregar_uint32_a_paquete(t_paquete *paquete, uint32_t n);
void agregar_uint8_a_paquete(t_paquete *paquete, uint8_t n);
void agregar_registros_a_paquete(t_paquete *paquete, t_registros registros);
void agregar_pid_a_paquete(t_paquete *paquete, int pid);



#endif