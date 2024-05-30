#ifndef KERNEL_PCB_H_
#define KERNEL_PCB_H_

#include "kernel_gestor.h"
#include "kernel_planificador.h"

// ESTRUCTURAS
typedef struct{
    int PID;
    int quantum;
    estado estado;
    t_registros registros;
} t_pcb;

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
    __uint32_t DI; //contiene la direccion logica de memoria de destino desde donde se va a copir un string
} t_registros;

// FUNCIONES
t_pcb *crear_pcb(void);
void eliminar_pcb(t_pcb *pcb);
int generar_pid(void);

#endif