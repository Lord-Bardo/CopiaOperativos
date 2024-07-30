#ifndef CPU_GESTOR_H_
#define CPU_GESTOR_H_

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <readline/readline.h>
#include <time.h>
#include <math.h>

#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/collections/list.h>


#include <utils/conexion.h>




// typedef struct nodo {
//     t_interrupt interrupcion;
//     struct nodo* siguiente;
// } nodo_t;

typedef struct{
	int pid;
	int pagina;
	int frame;
	int tiempo;
}t_entrada_tlb;

typedef struct{
	int reemplazar_fifo;
	int tiempo_actual;
	t_list *entradas;
}t_tlb;

typedef struct {
	uint8_t ax;
	uint8_t bx;
	uint8_t cx;
	uint8_t dx;
	uint32_t eax;
	uint32_t ebx;
	uint32_t ecx;
	uint32_t edx;
	uint32_t di;
	uint32_t si;
}t_registros;

typedef struct {
	int pid;
	uint32_t pc;
	t_registros registros;
}t_pcb;

typedef enum{
	SET,
	SUM,
    SUB,
    JNZ,
    IO_GEN_SLEEP,
    MOV_IN, 
    MOV_OUT, 
    RESIZE, 
    COPY_STRING, 
	WAIT,
	SIGNAL,
    IO_STDIN_READ, 
    IO_STDOUT_WRITE,
	IO_FS_CREATE,
	IO_FS_DELETE,
	IO_FS_TRUNCATE,
	IO_FS_WRITE,
	IO_FS_READ,
	EXIT,
	NOCODE
} t_instr_code;

typedef struct {
	t_instr_code instr_code;
	t_list *argumentos;
}t_instruccion;


// VARIABLES GLOBALES
extern t_log* cpu_logger;
extern t_log* cpu_logger_obligatorio;
extern t_config* cpu_config;
extern t_pcb pcb;
extern int salir_ciclo_instruccion;
extern int tamanio_pagina;
extern t_codigo_operacion motivo_desalojo;
//extern nodo_t *lista_interrupciones;
extern t_dictionary *diccionario_interrupciones;
extern t_tlb tlb;
extern pthread_mutex_t mutex_diccionario;
//HABIRA QUE PONER EL PCB ACA CREO
extern char* IP_MEMORIA;
extern char* PUERTO_MEMORIA;
extern char* PUERTO_ESCUCHA_DISPATCH;
extern char* PUERTO_ESCUCHA_INTERRUPT;
extern int CANTIDAD_ENTRADAS_TLB;
extern char* ALGORITMO_TLB;

extern int fd_cpu_dispatch;
extern int fd_cpu_interrupt;
extern int fd_kernel_dispatch;
extern int fd_kernel_interrupt;
extern int fd_memoria;


// VARIABLES GLOBALES

 //el que vamos a editar
#endif