#ifndef CPU_KERNEL_DISPATCH_H_
#define CPU_KERNEL_DISPATCH_H_

#include "cpu_gestor.h"
#include "instrucciones.h"


void atender_cpu_kernel_dispatch(void);
void iniciar_ciclo_instruccion(t_pcb);
void buffer_desempaquetar_registros(t_buffer *buffer, t_registros *registros);
int pcb_get_pid(t_pcb *pcb);
void buffer_desempaquetar_pcb(t_buffer *buffer, t_pcb* pcb);
void agregar_contexto_ejecucion_a_paquete(t_paquete *paquete, t_pcb* pcb);
void agregar_registros_a_paquete(t_paquete *paquete, t_registros registros);

void agregar_uint32_a_paquete(t_paquete *paquete, uint32_t n);
void agregar_uint8_a_paquete(t_paquete *paquete, uint8_t n);
void agregar_pid_a_paquete(t_paquete *paquete, int pid);
int pcb_get_pid(t_pcb *pcb);
t_registros pcb_get_registros(t_pcb *pcb);



#endif