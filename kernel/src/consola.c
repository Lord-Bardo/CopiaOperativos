#include "../include/consola.h"

void iniciar_consola_interactiva(){
    char *leido;

    // Leer la primer linea
	leido = readline("> ");
    if(validar_comando() == 0){

    }

}

void leer_consola(){
	char *leido;

	// Leer la primer linea
	leido = readline("> ");

	// Leer el resto hasta recibir un string vacío
	while (leido[0] != '\0'){
		log_info(kernel_logger, "%s", leido);
		leido = readline("> ");
	}

	// Libero las lineas
	free(leido);
}

int validar_comando(){
    
}