#include "../include/consola.h"

void iniciar_consola_interactiva(){
    char *leido;

	leido = readline("> ");
	while( leido[0] != '\0' ){
		if( !validar_comando(leido) ){
			log_error(kernel_logger, "Comando invalido!");
		}
		else{
			atender_comando(leido);
		}

		leido = readline("> ");
	}
	free(leido);
}

bool validar_comando(char* leido){
	bool resultado_validacion;
	char** comando = string_split(leido, " ");

	if( strcmp(comando[0], "EJECUTAR_SCRIPT") == 0 ){
		resultado_validacion = true;
	}
	else if( strcmp(comando[0], "INICIAR_PROCESO") == 0 ){
		resultado_validacion = true;
	}
	// ...

	string_array_destroy(comando);
	return resultado_validacion;
}

void atender_comando(char* comando){

}