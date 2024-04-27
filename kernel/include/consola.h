#ifndef CONSOLA_H_
#define CONSOLA_H_

#include "kernel_gestor.h"

typedef enum{
    EJECUTAR_SCRIPT,
    INICIAR_PROCESO,
    FINALIZAR_PROCESO,
    DETENER_PLANIFICACION,
    INICIAR_PLANIFICACION,
    MULTIPROGRAMACION,
    PROCESO_ESTADO
}comando;

void iniciar_consola_interactiva(void);
int validar_comando(char* leido);
void atender_instruccion(char* comando);

#endif