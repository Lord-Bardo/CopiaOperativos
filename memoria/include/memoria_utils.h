#ifndef MEMORIA_UTILS_H_
#define MEMORIA_UTILS_H_

#include "memoria_gestor.h" // no está tomando este include porque t_pcb_persona me marca que "no está definido" pero si lo está en el gestor. No sé como arreglarlo.

// PROCESOS.
t_pcb_memoria* inicializar_proceso();
int agregar_proceso(t_pcb_memoria proceso);
int eliminar_proceso(int index);
int encontrar_proceso(int pid);
int sizeof_proceso(int index);
void liberar_proceso(t_pcb_memoria* proceso);
void asignar_size_proceso(int index, int size);
void aumentar_proceso(int index, int size);
void reducir_proceso(int index, int size);

// AUXILIARES.
bool instruccion_valida(char* instruccion);
int frame_libre();

// MANEJO DE BUFFERS.
void buffer_desempaquetar_proceso(t_buffer *buffer, t_pcb_memoria* proceso);

#endif