#include "memoria.h"

int main(int argc, char* argv[]) {

	char *puerto;
	char *tamanioPag;
	char *tamanioMem;
    char *path;
    char *retardoResp;

	t_config *config;

    puerto = config_get_string_value(config, "PUERTO_ESCUCHA");
	tamanioMem = config_get_string_value(config, "TAM_MEMORIA");
	tamanioPag = config_get_string_value(config, "TAM_PAGINA");
    path = config_get_string_value(config, "PATH_INSTRUCCIONES");
    retardoResp =config_get_string_value(config, "RETARDO_RESPUESTA");


    decir_hola("Memoria");
    return 0;
}
t_config* iniciar_config(void) {
	t_config *nuevo_config;
	nuevo_config = config_create("memoria.config");
	if (nuevo_config == NULL){
		printf("No se pudo crear el config.");
		exit(2);
	}

	return nuevo_config;
}

void terminar_programa(t_config* config){
    if (config != NULL){
		config_destroy(config);
	}
}