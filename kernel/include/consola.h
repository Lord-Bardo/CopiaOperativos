#ifndef CONSOLA_H_
#define CONSOLA_H_

#include "kernel_gestor.h"

typedef enum{
    EJECUTAR_SCRIPT,
    INCIAR_PROCESO,
    FINALIZAR_PROCESO,
    DETENER_PLANIFICACION,
    INICIAR_PLANIFICACION,
    MULTIPROGRAMACION,
    PROCESO_ESTADO
}comando;

void iniciar_consola_interactiva(void);
void leer_consola(void);

#endif