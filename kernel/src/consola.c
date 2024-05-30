#include "../include/consola.h"

void iniciar_consola_interactiva(){
	char *leido;
	leido = readline("> ");

	while (leido[0] != '\0'){
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
			exit(EXIT_FAILURE);
		}
	}
	else if( strcmp(comando[0], "INICIAR_PROCESO") == 0 ){
		// INICIAR_PROCESO [PATH]
		if( comando[1] ){
			iniciar_proceso(comando[1]);
		}
		else{
			log_error(kernel_logger, "Falta el PATH para iniciar un proceso");
			exit(EXIT_FAILURE);
		}
	}
	else if (strcmp(comando[0], "FINALIZAR_PROCESO") == 0){
		// cargar_string_al_buffer(buffer, comando[1]) // [PID]
	}
	else if (strcmp(comando[0], "INICIAR_PLANIFICACION") == 0){
		// ...
	}
	else if (strcmp(comando[0], "DETENER_PLANIFICACION") == 0){
		// ...
	}
	else if (strcmp(comando[0], "MULTIPROGRAMACION") == 0){
		// cargar_string_al_buffer(buffer, comando[1]) // [VALOR]
	}
	else if (strcmp(comando[0], "PROCESO_ESTADO") == 0){
		// ...
	}
	else{
		log_error(kernel_logger, "Comando invalido!");
		exit(EXIT_FAILURE);
	}

	string_array_destroy(comando);
}

// EJECUTAR SCRIPT
void ejecutar_script(char *path)
{
    FILE *archivo = fopen(path, "r");
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
    free(linea);
    fclose(archivo);
}