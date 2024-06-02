#include <utils/instrucciones.h>

#include <commons/collections/list.h>

struct {
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