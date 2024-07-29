#ifndef CPU_UTILS_
#define CPU_UTILS_

#include "cpu_gestor.h"
#include "instrucciones.h"

void agregar_instruccion_paquete(t_paquete *paquete, char* instruccion);
void agregar_pcb_paquete(t_paquete *paquete, t_pcb *pcb);
//char* recibir_instruccion(int socket_cliente);

void recibir_instruccion(t_instruccion * instruccion);
void buffer_desempaquetar_instruccion(t_buffer *buffer, t_instruccion * instruccion);

void pedir_frame_memoria(int pagina);
void recibir_frame(int *frame);

void enviar_fetch_memoria(int pid, u_int32_t pc);
void buffer_desempaquetar_registros(t_buffer *buffer, t_registros *registros);
int pcb_get_pid(t_pcb *pcb);
void buffer_desempaquetar_pcb(t_buffer *buffer, t_pcb* pcb);
void agregar_contexto_ejecucion_a_paquete(t_paquete *paquete, t_pcb* pcb);
void agregar_registros_a_paquete(t_paquete *paquete, t_registros registros);
void enviar_pcb_kernel(t_codigo_operacion motivo_desalojo);
void agregar_pid_a_paquete(t_paquete *paquete, int pid);
int pcb_get_pid(t_pcb *pcb);
t_registros pcb_get_registros(t_pcb *pcb);

// void insertar_ordenado_lista_interrupciones(t_interrupt interrupcion_nueva);
// void quitar_lista_interrupciones(int pid_sacar);
// int buscar_lista_interrupciones(int pid_buscar);
// int obtener_motivo_lista(int pid_buscar);
void convertir_string_a_instruccion(char* cadena,t_instruccion * instruccion);
void log_n_parametros(int n, t_instruccion *instr);
void check_interrupt();
#endif