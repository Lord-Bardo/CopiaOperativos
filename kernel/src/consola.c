#include "../include/consola.h"

void iniciar_consola_interactiva(){
	char *leido;
	leido = readline("> ");

	while( leido[0] != '\0' ){
		atender_comando(leido);

		leido = readline("> ");
	}

	free(leido);
}

// HABRIA QUE HACER MÁS VALIDACIONES
void atender_comando(char *leido){
	char **comando = string_split(leido, " ");

	// Creo que en cada if se deberian ir cargando los parametros del comando al buffer e ir iniciando los procesos, creando hilos, etc (lo que en realidad se hace en kernel.c)
	if( strcmp(comando[0], "EJECUTAR_SCRIPT") == 0 ){
		// EJECUTAR_SCRIPT [PATH]
		if( comando[1] ){
			ejecutar_script(comando[1]);
		}
		else{
			log_error(kernel_logger, "Falta el PATH del script");
		}
	}
	else if( strcmp(comando[0], "INICIAR_PROCESO") == 0 ){
		// INICIAR_PROCESO [PATH]
		if( comando[1] ){
			iniciar_proceso(comando[1]);
		}
		else{
			log_error(kernel_logger, "Falta el PATH para iniciar un proceso");
		}
	}
	else if( strcmp(comando[0], "FINALIZAR_PROCESO") == 0 ){
		// FINALIZAR_PROCESO [PID]
		if( comando[1] ){
			finalizar_proceso(atoi(comando[1]));
		}
		else{
			log_error(kernel_logger, "Falta el PID para finalizar un proceso");
		}
	}
	else if( strcmp(comando[0], "INICIAR_PLANIFICACION") == 0 ){
		// INICIAR_PLANIFICACION
		iniciar_planificacion();
	}
	else if( strcmp(comando[0], "DETENER_PLANIFICACION") == 0){
		// DETENER_PLANIFICACION
		detener_planificacion();
	}
	else if( strcmp(comando[0], "MULTIPROGRAMACION") == 0 ){
		// MULTIPROGRAMACION [VALOR]
		if( comando[1] ){
			cambiar_grado_multiprogramacion_a(atoi(comando[1]));
		}
		else{
			log_error(kernel_logger, "Falta el VALOR del grado de multiprogramacion");
		}
	}
	else if (strcmp(comando[0], "PROCESO_ESTADO") == 0){
		// PROCESO_ESTADO
		proceso_estado();
	}
	else{
		log_error(kernel_logger, "Comando invalido!");
	}

	string_array_destroy(comando);
}

// EJECUTAR SCRIPT
void ejecutar_script(char *path){
    char *path_base = string_duplicate("../../../c-comenta-pruebas");
	string_append(&path_base, path);
	FILE *archivo = fopen(path_base, "r");
    if (archivo == NULL) {
        log_error(kernel_logger, "Error al abrir el archivo de script");
        return;
    }

    // Leer el archivo línea por línea
    char *linea = NULL;
    size_t longitud = 0;
    ssize_t leido;

    // getline es una función que lee una línea completa de un archivo y la almacena en un buffer, gestiona la asignación de memoria automaticamente
    // getline(&linea, &longitud, archivo)
	// &linea: puntero al buffer donde se almacenará la línea leída
	// &longitud: puntero a una variable que contiene el tamaño del buffer
	// archivo: puntero al archivo desde el cual se está leyendo.

	while ((leido = getline(&linea, &longitud, archivo)) != -1) {
        // Remover el salto de línea al final
        if (linea[leido - 1] == '\n') {
            linea[leido - 1] = '\0';
        }

        // Ejecutar el comando
        atender_comando(linea);
    }

    // Liberar la memoria y cerrar el archivo
    free(path_base);
	free(linea);
    fclose(archivo);
}

void proceso_estado(){
	// NEW
	char *lista_new = lista_pids_string(estado_new);
	printf("NEW: %s\n", lista_new);
	free(lista_new);

	// READY
	char *lista_ready = lista_pids_string(estado_ready);
	printf("READY: %s\n", lista_ready);
	free(lista_ready);

	// READY_PLUS
	char *lista_ready_plus = lista_pids_string(estado_ready_plus);
	printf("READY PLUS: %s\n", lista_ready_plus);
	free(lista_ready_plus);

	// EXEC
	char *lista_exec = lista_pids_string(estado_exec);
	printf("EXEC: %s\n", lista_exec);
	free(lista_exec);

	// BLOCKED
	char *lista_blocked = lista_pids_string(estado_blocked);
	printf("BLOCKED: %s\n", lista_blocked);
	free(lista_blocked);

	// EXIT
	char *lista_exit = lista_pids_string(estado_exit);
	printf("EXIT: %s\n", lista_exit);
	free(lista_exit);
}