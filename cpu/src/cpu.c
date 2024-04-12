#include "cpu.h"

int main(int argc, char* argv[]) {
    t_config* config;
    char* ip;
    
    // Inicio el config
    config = iniciar_config();

    // Obtengo el valor del campo IP del archivo cpu.config
    ip = config_get_string_value(config, "IP");

    // Muestro en pantalla el valor de IP
    printf("%s", ip);

    terminar_programa(config);

    return 0;
}

t_config* iniciar_config(void) {
	t_config *nuevo_config;
	nuevo_config = config_create("cpu.config");
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