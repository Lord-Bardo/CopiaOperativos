#include "memoria.h"

int main(int argc, char* argv[]) {
	// char *puerto;
	// char *tamanioPag;
	// char *tamanioMem;
    // char *path;
    // char *retardoResp;

	t_log* logger;
	logger = iniciar_logger();
    // t_config* config;

    // puerto = config_get_string_value(config, "PUERTO_ESCUCHA");
	// tamanioMem = config_get_string_value(config, "TAM_MEMORIA");
	// tamanioPag = config_get_string_value(config, "TAM_PAGINA");
    // path = config_get_string_value(config, "PATH_INSTRUCCIONES");
    // retardoResp =config_get_string_value(config, "RETARDO_RESPUESTA");

	servidor("4444", logger);

    return 0;
}

t_log *iniciar_logger(void){
	t_log *nuevo_logger;
	nuevo_logger = log_create("cpu.log", "CPU", 1, LOG_LEVEL_INFO);
	if (nuevo_logger == NULL){
		printf("No se pudo crear el logger.");
		exit(1);
	}

	return nuevo_logger;
}

t_config* iniciar_config(void){
	t_config *nuevo_config;
	nuevo_config = config_create("cpu.config");
	if (nuevo_config == NULL){
		printf("No se pudo crear el config.");
		exit(2);
	}

	return nuevo_config;
}

void leer_consola(t_log *logger){
	char *leido;

	// Leo la primer linea
	leido = readline("> ");

	// El resto, las voy leyendo y logueando hasta recibir un string vacío
	while (leido[0] != '\0'){
		log_info(logger, "%s", leido);
		leido = readline("> ");
	}

	// Libero las lineas
	free(leido);
}

void paquete(int conexion){
	char *leido;
	t_paquete *paquete;

    // Creo el paquete
	paquete = crear_paquete();

	// Leo y agrego las lineas al paquete
	leido = readline("> ");
	while (leido[0] != '\0'){
		agregar_a_paquete(paquete, leido, strlen(leido) + 1);
		leido = readline("> ");
	}

	// Envio el paquete
	enviar_paquete(paquete, conexion);

	// Libero las lineas y el paquete
	free(leido);
	eliminar_paquete(paquete);
}

void terminar_programa(int conexion, t_log* logger, t_config* config){
    if( logger != NULL ){
        log_destroy(logger);
    }
    
    if (config != NULL){
		config_destroy(config);
	}

    liberar_conexion(conexion);
}