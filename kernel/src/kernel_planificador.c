#include "../include/kernel_planificador.h"

void inicializar_estructuras_planificador(){
    inicializar_estructuras_pid();
    // incializar_estructuras_planificador_largo_plazo();
    // incializar_estructuras_planificador_corto_plazo();

    // Semaforos
    sem_init(&sem_grado_multiprogramacion, 0, GRADO_MULTIPROGRAMACION);

    // Colas Ready
    cola_ready = queue_create();
    cola_ready_plus = queue_create();
}

void inicializar_estructuras_pid(void){
    pid_actual = 0;
    pthread_mutex_init(&mutex_pid, NULL);
    
    return;
}

// INICIAR PROCESO
// Ante la solicitud de la consola de crear un nuevo proceso el Kernel deberá informarle a la memoria
// que debe crear un proceso cuyas operaciones corresponderán al archivo de pseudocódigo pasado
// por parámetro, todos los procesos iniciarán sin espacio reservado en memoria, por lo que solamente
// tendrán una tabla de páginas vacía.
// En caso de que el grado de multiprogramación lo permita, los procesos creados podrán pasar de la
// cola de NEW a la cola de READY, caso contrario, se quedarán a la espera de que finalicen procesos
// que se encuentran en ejecución.

void iniciar_proceso(const char *path){
	sem_wait(sem_grado_multiprogramacion);
	t_pcb *pcb = crear_pcb();
	proceso_a_ready(pcb);
	pedir_a_memoria_iniciar_proceso(path);
}

void proceso_a_ready(t_pcb *pcb){
	pcb->estado = READY;
	queue_push(ready, pcb);
}

void pedir_a_memoria_iniciar_proceso(const char *path){
    // TODO
}

// FINALIZAR PROCESO

void finalizar_proceso(){
    // TODO
}