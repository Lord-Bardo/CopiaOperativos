#include "../include/kernel_planificadores.h"

void iniciar_planificadores(){
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

    iniciar_planificacion();
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
    estado_exec = crear_estado(EXEC);
    estado_blocked = crear_estado(BLOCKED);
    estado_exit = crear_estado(EXIT);
}

void inicializar_estructuras_pid(void){
    pid_actual = 0;
    pthread_mutex_init(&mutex_pid, NULL);
}

void inicializar_semaforos(){
    sem_init(&sem_grado_multiprogramacion, 0, GRADO_MULTIPROGRAMACION);
    sem_init(&sem_dispatch, 0, 1);
}

// // Mati: Para las siguientes 3 funciones: entiendo que la asignacion es atomica y no requiere uso de semaforo
void iniciar_planificacion(){
    estado_planificacion = ACTIVA;
}

void detener_planificacion(){
    estado_planificacion = PAUSADA;
}

void cambiar_grado_multiprogramacion_a(int nuevo_grado_multiprogramacion){
    GRADO_MULTIPROGRAMACION = nuevo_grado_multiprogramacion;
}

void planificador_largo_plazo(){
    // Manejar NEW -> READY
    // while(1){
    //     sem_wait(&sem_grado_multiprogramacion);
    //     t_pcb *pcb = estado_desencolar_primer_pcb(estado_new);
    //     estado_encolar_pcb(estado_ready, pcb);
    // }

    // Manejar ESTADO -> EXIT

}

// void planificador_corto_plazo(){
//     while(1){
//         t_pcb *pcb = elegir_proceso();
//         proceso_a_exec(pcb);
//         pthread_mutex_lock(&mutex_dispatch);
//         // Mandarselo a CPU
//         // Esperar respuesta
//     }
// }

// t_pcb *elegir_proceso(){ // TERMINAR
//     t_pcb *pcb;
    
//     if( strcmp(ALGORITMO_PLANIFICACION, "FIFO") == 0 ){
//         pcb = elegir_proceso_segun_fifo();
//     }
//     else if( strcmp(ALGORITMO_PLANIFICACION, "RR") == 0 ){
//         TODO
//     }
//     else if( strcmp(ALGORITMO_PLANIFICACION, "VRR") == 0 ){
//         TODO
//         Tener en cuenta READY_PLUS
//     }
//     else{
//         log_error(kernel_logger, "Algoritmo de planificacion invalido!");
//     }

//     return pcb;
// }

void planificador_corto_plazo(){    
    if( strcmp(ALGORITMO_PLANIFICACION, "FIFO") == 0 ){
        planificador_corto_plazo_fifo();
    }
    else if( strcmp(ALGORITMO_PLANIFICACION, "RR") == 0 ){
        // TODO
    }
    else if( strcmp(ALGORITMO_PLANIFICACION, "VRR") == 0 ){
        // TODO
        // Tener en cuenta READY_PLUS
    }
    else{
        log_error(kernel_logger, "Algoritmo de planificacion invalido!");
    }
}

void planificador_corto_plazo_fifo(){
    while( estado_planificacion ){
        // sem_wait(&sem_dispatch); // Espera a que no hay ningun proceso ejecutando.
        t_pcb *pcb = elegir_proceso_segun_fifo();
        proceso_a_exec(pcb);
        enviar_contexto_de_ejecucion();
        recibir_contexto_de_ejecucion_actualizado(); // Si es bloqueante no deberia ser necesario usar el semaforo
    }
}

t_pcb *elegir_proceso_segun_fifo(){
    return estado_desencolar_primer_pcb(estado_ready);
}

void proceso_a_exec(pcb){
    pcb_cambiar_estado_a(pcb, EXEC);
    estado_encolar_pcb(estado_exec, pcb);
}

void enviar_contexto_de_ejecucion(){
    // Manda a CPU el contexto de la ejecucion por el Dispatch
}

void recibir_contexto_de_ejecucion_actualizado(){
    // Espera por el Dispatch la llegada del contexto actualizado tras la ejecucion del proceso
    // Junto con el contexto debe llegar el motivo por el cual finalizo la ejecucion (motivo de desalojo)
    // En cualquier caso se lo debe desencolar de EXEC
    // Si puede seguir ejecutando se lo encola en READY
    // Si no se bloqueo entonces se lo encola en BLOCKED
}

// INICIAR PROCESO
// Ante la solicitud de la consola de crear un nuevo proceso el Kernel deberá informarle a la memoria
// que debe crear un proceso cuyas operaciones corresponderán al archivo de pseudocódigo pasado
// por parámetro, todos los procesos iniciarán sin espacio reservado en memoria, por lo que solamente
// tendrán una tabla de páginas vacía.
// En caso de que el grado de multiprogramación lo permita, los procesos creados podrán pasar de la
// cola de NEW a la cola de READY, caso contrario, se quedarán a la espera de que finalicen procesos
// que se encuentran en ejecución.

// Crea el pcb, lo encola en new y le pide a memoria q cree las estructuras
void iniciar_proceso(const char *path){ // REVISAR
    t_pcb *pcb = crear_pcb();
    pedir_a_memoria_iniciar_proceso(pcb_get_pid(pcb), path); // Mati: calculo que memoria tendra una tabla con PIDs y sus path asociados 
    estado_encolar_pcb(estado_new, pcb);
    //sem_wait(&sem_grado_multiprogramacion);
    // Mati: Creo q deberiamos esperar el aviso de memoria de que ya esta creado el espacio antes de encolar el proceso,
    //       porque puede ser que se encole sin que esten las estructuras creadas
    // Mati: Podria meterse la espera dentro de pedir_a_memoria_iniciar_proceso
    // Mati en respuesta a Mati: Por lo q me parecio entender en los issues, no deberia nunca surgir un problema de falta de memoria.
    //                           Por lo q no seria necesario esperar para pedirle a memoria q cree las estructuras.
    log_creacion_proceso();
}

void proceso_a_ready(t_pcb *pcb){ // REVISAR
	pcb_cambiar_estado_a(pcb, READY);
	estado_encolar_pcb(estado_ready, pcb);
    sem_post(&sem_grado_multiprogramacion);
    log_ingreso_ready();
}

// void iniciar_proceso(const char *path){
//     t_pcb *pcb = crear_pcb();
//     estado_encolar_pcb(estado_new, pcb);
//     // Mati: Puede q si convenga tener el path dentro del pcb, asi el tema de pedirle a memoria que cree las estructuras lo hace el planificador cuando lo desencola
// }

void pedir_a_memoria_iniciar_proceso(const char *path){
    // TODO
    t_paquete *paquete = crear_paquete();
}

// FINALIZAR PROCESO

void finalizar_proceso(){ // TERMINAR
    


    sem_post(&sem_grado_multiprogramacion);
}