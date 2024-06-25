#include "../include/entradasalida_interfaces.h"

// El valor de la cant_unidades_trabajo viene de kernel y se multiplica por el valor que tiene el config de tiempo_unidad_trabajo
void interfaz_generica(int cant_unidades_trabajo) {
    int tiempo_total = (cant_unidades_trabajo * TIEMPO_UNIDAD_TRABAJO) / 1000;
    log_info(entradasalida_logger, "%d", tiempo_total);

    // Simular el tiempo de espera
    log_info(entradasalida_logger, "E/S: haciendo un sleep");
    sleep(tiempo_total);
    log_info(entradasalida_logger, "E/S: deje de hacer un sleep");
}

// SOPORTE: lista de direcciones fisicas - tamaño total (preguntar)
void interfaz_stdin(char* registro_direccion, char* registro_tamanio){
    char *texto;
	texto = readline("> ");

    if (strlen(texto) > atoi(registro_tamanio)) {
        texto[atoi(registro_tamanio)] = '\0';
    }

    enviar_texto_a_memoria(registro_direccion, texto, atoi(registro_tamanio));

	free(texto);
}

void enviar_texto_a_memoria(char* direccion_fisica, char* texto, int tamanio) {
    t_paquete* paquete = crear_paquete(PAQUETE); // Creo que se usa ese codigo de operacion (PREGUNTAR A MATI)
    agregar_a_paquete(paquete, direccion_fisica, sizeof(char*));
    agregar_a_paquete(paquete, texto, tamanio);

    enviar_paquete(fd_memoria, paquete);

    eliminar_paquete(paquete);
}

void interfaz_stdout(char* registro_direccion, char* registro_tamanio) {
    solicitar_datos_a_memoria(registro_direccion, atoi(registro_tamanio));

    char* texto = recibir_datos_de_memoria(atoi(registro_tamanio));
    
    if (texto != NULL) {
        log_info(entradasalida_logger, "TEXTO LEIDO: %s", texto);
        free(texto);
    } else {
        log_error(entradasalida_logger, "Error al recibir datos de memoria.\n");
    }
}

void solicitar_datos_a_memoria(char* direccion_fisica, int tamanio){
    t_paquete* paquete = crear_paquete(PAQUETE);

    agregar_a_paquete(paquete, direccion_fisica, sizeof(char*));
    agregar_a_paquete(paquete, tamanio, sizeof(int));

    enviar_paquete(fd_memoria, paquete);

    eliminar_paquete(paquete);
}

char* recibir_datos_de_memoria(int tamanio) {
    t_buffer* buffer = crear_buffer();
    recibir_paquete(fd_memoria, PAQUETE, buffer);

    char* texto = malloc(tamanio + 1);
    if (texto != NULL) {
        buffer_desempaquetar(buffer, texto);
        texto[tamanio] = '\0';
    }

    eliminar_buffer(buffer);

    return texto;
}
