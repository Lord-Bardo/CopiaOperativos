#include "../include/consola.h"

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

// HABRIA QUE HACER MÁS VALIDACIONES
void atender_comando(char *leido)
{
	char **comando = string_split(leido, " ");

	// Creo que en cada if se deberian ir cargando los parametros del comando al buffer e ir iniciando los procesos, creando hilos, etc (lo que en realidad se hace en kernel.c)
	if (strcmp(comando[0], "EJECUTAR_SCRIPT") == 0)
	{
		// EJECUTAR_SCRIPT [PATH]
		if (comando[1]) {
			ejecutar_script(comando[1]);
		} else {
			log_error(kernel_logger, "Falta el PATH del script");
			exit(EXIT_FAILURE);
		}
	}
	else if (strcmp(comando[0], "INICIAR_PROCESO") == 0)
	{
		// INICIAR_PROCESO [PATH]
		if (comando[1]) {
			iniciar_proceso(comando[1]);
		} else {
			log_error(kernel_logger, "Falta el PATH para iniciar un proceso");
			exit(EXIT_FAILURE);
		}
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
    FILE *archivo = fopen(path, "r");
    if (archivo == NULL) {
        log_error(kernel_logger, "Error al abrir el archivo de script");
        return;
    }

    // Leer el archivo línea por línea
    char *linea = NULL;
    size_t longitud = 0;
    ssize_t leido;

    // getline es una función que lee una línea completa de un archivo y la almacena en un buffer, gestiona la asignación de memoria automaticamente
    // getline(&linea, &longitud, archivo)
	// &linea: puntero al buffer donde se almacenará la línea leída
	// &longitud: puntero a una variable que contiene el tamaño del buffer
	// archivo: puntero al archivo desde el cual se está leyendo.

	while ((leido = getline(&linea, &longitud, archivo)) != -1) {
        // Remover el salto de línea al final
        if (linea[leido - 1] == '\n') {
            linea[leido - 1] = '\0';
        }

        // Ejecutar el comando
        atender_comando(linea);
    }

    // Liberar la memoria y cerrar el archivo
    free(linea);
    fclose(archivo);
}

/* void iniciar_proceso(const char *path){
	t_pcb *pcb = crear_pcb(path);

    pthread_mutex_lock(&mutex_pid);
    if (queue_size(ready) < GRADO_MULTIPROGRAMACION) {
        queue_push(ready, pcb);
        proceso_a_ready(pcb);  // Esta función debería cambiar el estado del proceso a READY y manejar cualquier lógica adicional
    } else {
	   pthread_mutex_unlock(&mutex_pid);
	   free(pcb)
       // Tirar error o bloquear el proceso (?
    }
    pthread_mutex_unlock(&mutex_pid);
} */

/* t_pcb *crear_pcb(const char *path) {
    t_pcb *pcb = malloc(sizeof(t_pcb));
    if (pcb == NULL) {
        printf("Error al asignar memoria para el PCB"); // Deberia ser un error, pero no se si logear todo al kernel sea una buena idea
        return NULL;
    }

    pcb->PID = generar_pid();
    pcb->estado = NEW;
    pcb->quantum = QUANTUM;
    pcb->registros = crear_registros();

    // Leer las instrucciones desde el archivo y asignarlas al PCB si es necesario
    // Aquí podrías añadir más lógica para leer el archivo y asociar las instrucciones al PCB

    return pcb;
} */

void proceso_a_ready(t_pcb *pcb){
	pcb->estado = READY;
    // Agregar más logica
}

// PROCESO---------------------------------------------
int pid = 0;
// No sé si está bien que la cola de READY este acá, ver donde inicializarla
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

pthread_t mutex_pid; // VER COMO SE DECLARA EL SEMAFORO!!!
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
	// int pid_proceso; // creo que es medio al pedo declararla acá
	pthread_mutex_lock(&mutex_pid);
	int pid_proceso = pid;
	pid++;
	pthread_mutex_unlock(&mutex_pid);
	return pid_proceso;
}