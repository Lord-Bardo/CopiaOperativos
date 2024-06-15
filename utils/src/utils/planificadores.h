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
}t_estado;

//hi
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
}t_registros;

typedef struct{
    int pid;
    int quantum;
    t_estado estado;
    t_registros registros;
    // FALTARÍAN AGREGAR EL SIGUIENTE CAMPO (posiblemente, no asegurado)
    //segment table
}t_pcb;

// Mati: "Me copie todas las estructuras al modulo de planificacion del kernel.
//        Creo q CPU no necesita conocer las mismas cosas del pcb que Kernel.
//        Capaz conviene tener una estructura en Kernel y otra en CPU."

#endif