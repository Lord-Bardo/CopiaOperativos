#include "instrucciones.h"

struct {
    int opcode;
    t_list* argumentos;
}t_instruccion;

typedef enum op_code {
    SET,
    SUM, 
    SUB,
    JNZ,
    IO_GEN_SLEEP,
}op_code;