#include "../include/kernel_planificadores.h"

void iniciar_planificacion(){
    // Estructuras
    inicializar_estructuras();

    // Planificador largo plazo
    pthread_t hilo_planificador_largo_plazo;
    pthread_create(&hilo_planificador_largo_plazo, NULL, (void*)planificador_largo_plazo, NULL);
    pthread_detach(hilo_planificador_largo_plazo);

    // Planificador corto plazo
    pthread_t hilo_planificador_corto_plazo;
    pthread_create(&hilo_planificador_corto_plazo, NULL, (void*)planificador_corto_plazo, NULL);
    pthread_detach(hilo_planificador_corto_plazo);
}

void inicializar_estructuras(){
    inicializar_estructuras_estados();
    inicializar_estructuras_pid();
    inicializar_semaforos();
}

void inicializar_estructuras_estados(){
    estado_new = crear_estado(NEW);
    estado_ready = crear_estado(READY);
    estado_ready_plus = crear_estado(READY_PLUS);
    estado_running = crear_estado(RUNNING);
    estado_blocked = crear_estado(BLOCKED);
    estado_exit = crear_estado(EXIT);
}

void inicializar_estructuras_pid(void){
    pid_actual = 0;
    pthread_mutex_init(&mutex_pid, NULL);
    
    return;
}

void inicializar_semaforos(){
    sem_init(&sem_grado_multiprogramacion, 0, GRADO_MULTIPROGRAMACION);
}

void planificador_largo_plazo(){
    // TODO
}

void planificador_corto_plazo(){
    // TODO
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
    t_pcb *pcb = crear_pcb();
    estado_encolar_pcb(estado_new, pcb);
    sem_wait(&sem_grado_multiprogramacion);
    pedir_a_memoria_iniciar_proceso(path);
    // Mati: Creo q deberiamos esperar el aviso de memoria de que ya esta creado el espacio antes de encolar el proceso,
    //       porque puede ser que se encole sin que esten las estructuras creadas
    proceso_a_ready(t_pcb *pcb);
}

void proceso_a_ready(t_pcb *pcb){
	pcb_cambiar_estado_a(pcb, READY);
	estado_encolar_pcb(estado_ready, pcb);
    sem_post(&sem_grado_multiprogramacion);
}

void pedir_a_memoria_iniciar_proceso(const char *path){
    // TODO
}

// FINALIZAR PROCESO

void finalizar_proceso(){ // TERMINAR
    


    sem_post(&sem_grado_multiprogramacion);
}