#ifndef UTILS_INSTRUCCIONES_H_
#define UTILS_INSTRUCCIONES_H_


#include "cpu_gestor.h"

typedef struct {
    int ins_code;
    t_list* argumentos;
}t_instruccion;

typedef enum{
    SET,
    SUM, 
    SUB,
    JNZ,
    IO_GEN_SLEEP,
}ins_code;

#endif