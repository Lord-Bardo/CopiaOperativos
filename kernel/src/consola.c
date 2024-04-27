#include "../include/consola.h"

void iniciar_consola_interactiva(){
    char *leido;
	leido = readline("> ");

	while( leido[0] != '\0' ){

		if(!validar_comando(leido))
			log_error(kernel_logger, "Comando invalido!");
		else
			atender_comando(leido);

		leido = readline("> ");
	}
	
	free(leido);
}

bool validar_comando(char* leido){
	bool resultado_validacion = false;
	char** comando = string_split(leido, " ");

	if( strcmp(comando[0], "EJECUTAR_SCRIPT") == 0 ){
		resultado_validacion = true;
	}
	else if( strcmp(comando[0], "INICIAR_PROCESO") == 0 ){
		resultado_validacion = true;
	}
	else if( strcmp(comando[0], "FINALIZAR_PROCESO") == 0 ){
		resultado_validacion = true;
	}
	else if( strcmp(comando[0], "DETENER_PLANIFICACION") == 0 ){
		resultado_validacion = true;
	}
	else if( strcmp(comando[0], "INICIAR_PLANIFICACION") == 0 ){
		resultado_validacion = true;
	}
	else if( strcmp(comando[0], "MULTIPROGRAMACION") == 0 ){
		resultado_validacion = true;
	}
	else if( strcmp(comando[0], "PROCESO_ESTADO") == 0 ){
		resultado_validacion = true;
	}

	string_array_destroy(comando);
	return resultado_validacion;

	// IDEA POSIBLE QUE SE ME OCURRIÓ
	/* const CANTIDAD_DE_COMANDOS = 7;
	const char *comandos_posibles[CANTIDAD_DE_COMANDOS] = {
		"EJECUTAR_SCRIPT",
		"INICIAR_PROCESO",
		"FINALIZAR_PROCESO",
		"DETENER_PLANIFICACION",
		"INICIAR_PLANIFICACION",
		"MULTIPROGRAMACION",
		"PROCESO_ESTADO"
	};

	// Se verifica si un comando está en el array de comandos posibles
	for (int i = 0; i < CANTIDAD_DE_COMANDOS; i++) {
		if (strcmp(comando[0], comandos_posibles[i]) == 0) { // El comando está en la lista de comandos posibles
			resultado_validacion = true;
			break; 
		}
	} */
}

/* No me gusta que se haga denuevo la validación de ifs, pero por el momento lo dejo así */
void atender_comando(char* comando){
	// t_buffer* buffer = crear_buffer();

	// Creo que en cada if se deberian ir cargando los parametros del comando al buffer e ir iniciando los procesos, creando hilos, etc (lo que en realidad se hace en kernel.c)
	// (No termino de entender por qué es necesario cargarlo al buffer)
	if( strcmp(comando[0], "EJECUTAR_SCRIPT") == 0 ){
		// cargar_string_al_buffer(buffer, comando[1]) // [PATH]
	}
	else if( strcmp(comando[0], "INICIAR_PROCESO") == 0 ){
		// cargar_string_al_buffer(buffer, comando[1]) // [PATH]
	}
	else if( strcmp(comando[0], "FINALIZAR_PROCESO") == 0 ){
		// cargar_string_al_buffer(buffer, comando[1]) // [PID]
	}
	else if( strcmp(comando[0], "DETENER_PLANIFICACION") == 0 ){
		// ...
	}
	else if( strcmp(comando[0], "INICIAR_PLANIFICACION") == 0 ){
		// ...
	}
	else if( strcmp(comando[0], "MULTIPROGRAMACION") == 0 ){
		// cargar_string_al_buffer(buffer, comando[1]) // [VALOR]
	}
	else if( strcmp(comando[0], "PROCESO_ESTADO") == 0 ){
		// ...
	}
	else {
		log_error(kernel_logger, "Comando invalido!"); // No deberia pasar nunca si pasó bien el anterior filtro, pero lo pongo por las dudas
		exit(EXIT_FAILURE) // En el video pone esto, pero creo que seria lo mismo que poner exit(1)
	}

	string_array_destroy(comando);
}