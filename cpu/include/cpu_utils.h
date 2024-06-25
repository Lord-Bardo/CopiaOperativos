#ifndef CPU_UTILS_
#define CPU_UTILS_

#include "cpu_gestor.h"
#include "instrucciones.h"

void agregar_instruccion_paquete(t_paquete *paquete, char* instruccion);
void agregar_pcb_paquete(t_paquete *paquete, t_pcb *pcb);
//char* recibir_instruccion(int socket_cliente);

void recibir_instruccion(t_instruccion * instruccion);
void buffer_desempaquetar_instruccion(t_buffer *buffer, t_instruccion * instruccion);
void buffer_desempaquetar_argumentos(t_buffer *buffer, t_instruccion * instruccion);


void liberar_instruccion(t_instruccion *instruccion);

void enviar_fetch_memoria(int pid, u_int32_t pc);
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