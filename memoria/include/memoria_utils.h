#ifndef MEMORIA_UTILS_H_
#define MEMORIA_UTILS_H_

#include "memoria_gestor.h" // no está tomando este include porque t_pcb_persona me marca que "no está definido" pero si lo está en el gestor. No sé como arreglarlo.

t_pcb_memoria* inicializar_proceso();
int agregar_proceso(t_pcb_memoria proceso);
int eliminar_proceso(int index);
int encontrar_proceso(int pid);
bool instruccion_valida(char* instruccion);
void liberar_pcb_memoria(t_pcb_memoria* proceso);

// MANEJO DE BUFFER.
void buffer_desempaquetar_proceso(t_buffer *buffer, t_pcb_memoria* proceso);

#endif