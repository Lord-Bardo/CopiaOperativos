#include "entradasalida.h"

int main(int argc, char* argv[]) {
    decir_hola("una Interfaz de Entrada/Salida");

    char *tipo_interfaz;
	char *tiempo_unidad_trabajo;
	char *ip_kernel;
    char *puerto_kernel;
    char *ip_memoria;
    char *puerto_memoria;
    char *path_base_dialfs;
    char *block_size;
    char *block_count;

    t_config *config;

    config = iniciar_config();

    tipo_interfaz = config_get_string_value(config, "TIPO_INTERFAZ");
	tiempo_unidad_trabajo = config_get_string_value(config, "TIEMPO_UNIDAD_TRABAJO");
	ip_kernel = config_get_string_value(config, "IP_KERNEL");
    puerto_kernel = config_get_string_value(config, "PUERTO_KERNEL");
    ip_memoria = config_get_string_value(config, "IP_MEMORIA");
    puerto_memoria = config_get_string_value (config, "PUERTO_MEMORIA");
    path_base_dialfs = config_get_string_value (config, "PATH_BASE_DIALFS");
    block_size = config_get_string_value (config, "BLOCK_SIZE");
    block_count = config_get_string_value (config, "BLOCK_COUNT");

    t_config* iniciar_config(void);
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
