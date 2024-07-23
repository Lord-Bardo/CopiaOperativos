#ifndef MEMORIA_UTILS_H_
#define MEMORIA_UTILS_H_

#include "memoria_gestor.h" // no está tomando este include porque t_pcb_persona me marca que "no está definido" pero si lo está en el gestor. No sé como arreglarlo.

// PROCESOS.
t_pcb_memoria* inicializar_proceso();
void destruir_proceso(void* proceso_void);
void asignar_size_proceso(t_pcb_memoria* proceso, int size);
void aumentar_proceso(t_pcb_memoria* proceso, int size);
void reducir_proceso(t_pcb_memoria* proceso, int size);
void obtener_frame(int pag, int* frame);


// AUXILIARES.
bool instruccion_valida(char* instruccion);
int obtener_primer_frame_libre();
void escribir(int direc_fisica, int bytes, void* dato);
void leer(int direc_fisica_read, int bytes_read, void* dato);
bool comparar_pid_kernel(void* pid);
bool comparar_pid_cpu(void* pid);
bool comparar_pid_es(void* pid);
t_pagina *crear_pagina();
int asignar_frame_libre();

// MANEJO DE BUFFERS.
void buffer_desempaquetar_proceso(t_buffer *buffer, t_pcb_memoria* proceso);

#endif