#ifndef UTILS_PLANIFICADORES_H_
#define UTILS_PLANIFICADORES_H_

#include <stdlib.h>
#include <stdio.h>

typedef enum{
    NEW,
    READY,
    RUNNING,
    BLOCKED,
    EXIT,
}estado;

typedef struct{
    __uint32_t PC;
    __uint8_t AX;
    __uint8_t BX;
    __uint8_t CX;
    __uint8_t DX;
    __uint32_t EAX;
    __uint32_t EBX;
    __uint32_t ECX;
    __uint32_t EDX;
    __uint32_t SI; //contiene la direccion logica de memoria de origen desde donde se va a copir un string
    __uint32_t DI;  //contiene la direccion logica de memoria de destino desde donde se va a copir un string
}t_registros;

typedef struct{
    int PID;
    int quantum;
    estado estado;
    t_registros registros;
    // FALTARÍAN AGREGAR EL SIGUIENTE CAMPO (posiblemente, no asegurado)
    //segment table
}t_pcb; 

/*
typedef struct t_pcb {
	uint32_t pid;  // También funciona como ID del socket
	process_state state;
	int aprox_burst_time;
	uint64_t last_burst_time;
	time_t last_ready_time;
	t_dictionary* local_files; // Key-value: filename - position
	t_execution_context* execution_context;
} t_pcb;
/*

#endif