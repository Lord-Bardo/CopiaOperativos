#include "../include/entradasalida_kernel.h"

void atender_entradasalida_kernel(){
    while (1) {
        char* peticion = recibir_peticion_kernel(fd_entradasalida);
        if (peticion != NULL) {
            manejar_peticion_kernel(peticion);
        } else {
			log_info(entradasalida_logger, "Error al recibir petición del kernel");
            break;
        }
    }
    return NULL;
}

void manejar_peticion_kernel(char* peticion) {
	char operacion[32];
    int cant_unidades_trabajo;

    // Parsear la petición para extraer la operación y el parámetro
    sscanf(peticion, "%s %d", operacion, &cant_unidades_trabajo);

    if (strcmp(operacion, "IO_GEN_SLEEP") == 0) {
        interfaz_generica(cant_unidades_trabajo);
    } else {
		log_info(entradasalida_logger, "Operacion desconocida: %s", operacion);
    }

    free(peticion);  // Liberar memoria asignada para la petición
}

char* recibir_peticion_kernel(int fd_kernel) {
    int longitud_peticion;

    // Recibo la longitud del mensaje
    if (recv(fd_kernel, &longitud_peticion, sizeof(int), MSG_WAITALL) <= 0) {
        return NULL;
    }

    char* peticion = (char*)malloc(longitud_peticion + 1);
    if (peticion == NULL) {
        perror("Error al asignar memoria para la petición");
        exit(1);
    }

    if (recv(fd_kernel, peticion, longitud_peticion, MSG_WAITALL) <= 0) {
        free(peticion);
        return NULL;
    }

    peticion[longitud_peticion] = '\0';  // Asegurar que la cadena esté terminada en null
    return peticion;
}