#ifndef CPU_GESTOR_H_
#define CPU_GESTOR_H_

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <readline/readline.h>
#include <time.h>

#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/collections/list.h>


#include <utils/conexion.h>

typedef struct {
	__uint32_t pc;
    __uint8_t ax;
    __uint8_t bx;
    __uint8_t cx;
    __uint8_t dx;
    __uint32_t eax;
    __uint32_t ebx;
    __uint32_t ecx;
    __uint32_t edx;
    __uint32_t si; //contiene la direccion logica de memoria de origen desde donde se va a copir un string
    __uint32_t di; //contiene la direccion logica de memoria de destino desde donde se va a copir un string
}t_registros;

typedef struct {
	int pid;
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
    IO_STDIN_READ, 
    IO_STDOUT_WRITE
} t_instr_code;

typedef struct {
	t_instr_code instr_code;
	char* argumentos[5];
}t_instruccion;


// VARIABLES GLOBALES
extern t_log* cpu_logger;
extern t_config* cpu_config;
extern t_pcb pcb;
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