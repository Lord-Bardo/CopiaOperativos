#include "../include/inicializar_memoria.h"

void inicializar_memoria(char* archivo_configuracion)
{
    iniciar_logger();
    iniciar_config(archivo_configuracion);
	iniciar_variables();
}

void iniciar_logger(void)
{
	memoria_logger = log_create("memoria.log", "MEMORIA", 1, LOG_LEVEL_INFO);
	if(memoria_logger == NULL){
		printf("No se pudo crear el logger.");
		exit(1);
	}

	memoria_logger_min_y_obl = log_create("memoria_min_y_obl.log", "ENTRADASALIDA", 1, LOG_LEVEL_INFO);
	if(memoria_logger_min_y_obl == NULL){
		perror("No se pudo crear el logger.");
		exit(1);
	}
}

void iniciar_config(char* archivo_configuracion)
{
 	char* config_path = string_duplicate("../");
    string_append(&config_path,archivo_configuracion);

	memoria_config = config_create(config_path);
	if(memoria_config == NULL){
		perror("No se pudo crear el config.");
		perror(config_path);
		exit(2);
	}

    PUERTO_ESCUCHA = config_get_string_value(memoria_config, "PUERTO_ESCUCHA");
	TAM_MEMORIA = config_get_int_value(memoria_config, "TAM_MEMORIA");
	TAM_PAGINA = config_get_int_value(memoria_config, "TAM_PAGINA");
    PATH_INSTRUCCIONES = config_get_string_value(memoria_config, "PATH_INSTRUCCIONES");
    RETARDO_RESPUESTA =config_get_int_value(memoria_config, "RETARDO_RESPUESTA");
}

void iniciar_variables(void)
{
	espacio_usuario = malloc(TAM_MEMORIA);
    memset(espacio_usuario, 0, TAM_MEMORIA);
	pthread_mutex_init(&mutex_espacio_usuario, NULL);
	char *puntero_a_bits = malloc((TAM_MEMORIA/TAM_PAGINA)/8);

    frames_libres = bitarray_create_with_mode(puntero_a_bits, (TAM_MEMORIA/TAM_PAGINA)/8, MSB_FIRST);
	for(int i = 0; i < (TAM_MEMORIA/TAM_PAGINA); i++){ 
		bitarray_clean_bit(frames_libres, i);
	}
	procesos = list_create(); //array es dinámico porque la info viene en el momento
	pthread_mutex_init(&mutex_procesos, NULL);
	pthread_mutex_init(&mutex_bitmap, NULL);
}