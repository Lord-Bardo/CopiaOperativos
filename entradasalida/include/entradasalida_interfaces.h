#ifndef IO_INTERFACE_H
#define IO_INTERFACE_H

#include <pthread.h>

typedef enum {
    GENERICA,
    STDIN,
    STDOUT,
    DIALFS
} TipoInterfaz;

void interfaz_generica(int cant_unidades_trabajo);

#endif