#include "../include/consola.h"
#include <commons/collections> // Para traerme la función de queue

void iniciar_consola_interactiva()
{
	char *leido;
	leido = readline("> ");

	while (leido[0] != '\0')
	{
		atender_comando(leido);

		leido = readline("> ");
	}

	free(leido);
}

void atender_comando(char *leido)
{
	char **comando = string_split(leido, " ");

	// Creo que en cada if se deberian ir cargando los parametros del comando al buffer e ir iniciando los procesos, creando hilos, etc (lo que en realidad se hace en kernel.c)
	// (No termino de entender por qué es necesario cargarlo al buffer)
	if (strcmp(comando[0], "EJECUTAR_SCRIPT") == 0)
	{
		// EJECUTAR_SCRIPT [PATH]
		ejecutar_script(comando[1]);
	}
	else if (strcmp(comando[0], "INICIAR_PROCESO") == 0)
	{
		// INICIAR_PROCESO [PATH]
		iniciar_proceso();
	}
	else if (strcmp(comando[0], "FINALIZAR_PROCESO") == 0)
	{
		// cargar_string_al_buffer(buffer, comando[1]) // [PID]
	}
	else if (strcmp(comando[0], "DETENER_PLANIFICACION") == 0)
	{
		// ...
	}
	else if (strcmp(comando[0], "INICIAR_PLANIFICACION") == 0)
	{
		// ...
	}
	else if (strcmp(comando[0], "MULTIPROGRAMACION") == 0)
	{
		// cargar_string_al_buffer(buffer, comando[1]) // [VALOR]
	}
	else if (strcmp(comando[0], "PROCESO_ESTADO") == 0)
	{
		// ...
	}
	else
	{
		log_error(kernel_logger, "Comando invalido!");
		exit(EXIT_FAILURE);
	}

	string_array_destroy(comando);
}

void ejecutar_script(char *path)
{
}
// PROCESO---------------------------------------------
int pid = 0;
t_queue *ready = queue_create();

void iniciar_proceso()
{
	t_pcb *pcb = crear_pcb();

	if (queue_size(ready) < GRADO_MULTIPROGRAMACION) // Lo cambié por una función de la commons
	{ // ready podria ser una cola de pcb's
		queue_push(ready, pcb)
		proceso_a_ready(); // Esta función que hace?
	}
}

pthread_t mutex_pid;
t_pcb *crear_pcb()
{
	t_pcb *pcb = malloc(sizeof(t_pcb));
	pthread_create(&mutex_pid, NULL, (void *)generar_pid, NULL);
	pthread_join(mutex_pid, pcb->PID);
	pcb->estado = NEW;
	pcb->quantum = QUANTUM;
	pcb->registros = crear_registros();

	return pcb;
}

int generar_pid()
{
	int pid_proceso;
	pthread_mutex_lock();
	pid_proceso = pid;
	pid++;
	pthread_mutex_unlock();

	return pid_proceso;
}