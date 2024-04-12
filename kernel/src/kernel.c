#include "kernel.h"

int main(int argc, char* argv[]) {
    decir_hola("Kernel");

    char *puerto_escucha;
    char *ip_memoria;
    char *puerto_memoria;
    char *ip_cpu;
    char *puerto_cpu_dispatch;
    char *puerto_cpu_interrupt;
    char *algoritmo_planificacion;
    char *quantum;
    char *recursos;
    char *instancias_recursos;
    char *grado_multiprogramacion;

    t_config *config;

    //  ARCHIVOS DE CONFIGURACION

    puerto_escucha = config_get_string_value(config, "PUERTO_ESCUCHA");
    ip_memoria = config_get_string_value(config, "IP_MEMORIA");
    puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
    ip_cpu = config_get_string_value(config, "IP_CPU");
    puerto_cpu_dispatch = config_get_string_value(config, "PUERTO_CPU_DISPATCH");
    puerto_cpu_interrupt = config_get_string_value(config, "PUERTO_CPU_INTERRUPT");
    algoritmo_planificacion = config_get_string_value(config, "ALGORITMO_PLANIFICACION");
    quantum = config_get_string_value(config, "QUANTUM");
    recursos = config_get_string_value(config, "RECURSOS");
    instancias_recursos = config_get_string_value(config, "INSTANCIAS_RECURSOS");
    grado_multiprogramacion = config_get_string_value(config, "GRADO_MULTIPROGRAMACION");

        #include "cpu.h"

    // Muestro en pantalla el valor de QUANTUM
    printf("%s", quantum);

    terminar_programa(config);

    return 0;
}

t_config *iniciar_config(void)
{
	t_config *nuevo_config;
	nuevo_config = config_create("kernel.config"); 
	if (nuevo_config == NULL){
		printf("No se pudo crear el config.");
		exit(2);
	}

	return nuevo_config;
}

void terminar_programa(t_config *config)
{
	if (config != NULL){
		config_destroy(config);
	}
}
    return 0;
}

t_config *iniciar_config(void)
{
	t_config *nuevo_config;
	nuevo_config = config_create("kernel.config"); 
	if (nuevo_config == NULL){
		printf("No se pudo crear el config.");
		exit(2);
	}

	return nuevo_config;
}

void terminar_programa(t_config *config)
{
	if (config != NULL){
		config_destroy(config);
	}
}