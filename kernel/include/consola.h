#ifndef CONSOLA_H_
#define CONSOLA_H_

#include "kernel_gestor.h"
#include "kernel_utils.h"
#include "kernel_planificadores.h"
#include "kernel_estados.h"

typedef enum{
    EJECUTAR_SCRIPT,
    INICIAR_PROCESO,
    FINALIZAR_PROCESO,
    DETENER_PLANIFICACION,
    INICIAR_PLANIFICACION,
    MULTIPROGRAMACION,
    PROCESO_ESTADO
} comando;

void iniciar_consola_interactiva(void);
void atender_comando(char *leido);
void ejecutar_script(char* path);
void proceso_estado(void);

#endif