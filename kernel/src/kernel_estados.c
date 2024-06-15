#include "../include/kernel_estados.h"

t_estado *crear_estado(t_nombre_estado nombre_estado){
    t_estado *estado = malloc(sizeof(t_estado));
    if( estado == NULL ){
        log_error(kernel_logger, "Error al asignar memoria para el ESTADO");
        return NULL;
    }

    estado->nombre_estado = nombre_estado;
    estado->lista_procesos = list_create();

    pthread_mutex_t *mutex = malloc(sizeof(pthread_mutex_t));
    if( mutex == NULL ){
        log_error(kernel_logger, "Error al asignar memoria para el MUTEX del ESTAD");
        return NULL;
    }
    pthread_mutex_init(mutex, NULL);

    sem_t *sem = malloc(sizeof(sem_t));
    if( sem == NULL ){
        log_error(kernel_logger, "Error al asignar memoria para el SEMAFORO del ESTAD");
        return NULL;
    }
    sem_init(sem, 0, 0); // La lista arranca vacia

    return estado;
}

void eliminar_estado(t_estado *estado){
    if( estado != NULL ){
        if( list_is_empty(estado->lista_procesos) ){
            list_destroy(estado->lista_procesos);
        }
        else{
            list_destroy_and_destroy_elements(estado->lista_procesos, pcb_destroyer);
        } 
        free(estado->lista_procesos);

        pthread_mutex_destroy(estado->mutex_estado);
        free(estado->mutex_estado);

        sem_destroy(estado->sem_estado);
        free(estado->sem_estado);

        free(estado);
    }
}

void pcb_destroyer(void *pcb){
    t_pcb *temp_pcb = (t_pcb *)pcb;
    eliminar_pcb(temp_pcb);
}

t_nombre_estado estado_get_nombre_estado(t_estado *estado){
    return estado->nombre_estado;
}

t_list *estado_get_lista_procesos(t_estado *estado){
    return estado->lista_procesos;
}

pthread_mutex_t *estado_get_mutex(t_estado *estado){
    return estado->mutex_estado;
}

sem_t *estado_get_sem(t_estado *estado){
    return estado->sem_estado;
}

void estado_encolar_pcb(t_estado *estado, t_pcb *pcb){
    pthread_mutex_lock(estado_get_mutex(estado));
    list_add(estado_get_lista_procesos(estado), pcb);
    pthread_mutex_unlock(estado_get_mutex(estado));
    sem_post(estado_get_sem(estado));
}

t_pcb *estado_desencolar_primer_pcb(t_estado *estado){
    sem_wait(estado_get_sem(estado)); // debe haber elementos en la lista para poder desencolar
    pthread_mutex_lock(estado_get_mutex(estado));
    t_pcb * pcb = list_remove(estado_get_lista_procesos(estado), 0);
    pthread_mutex_unlock(estado_get_mutex(estado));

    return pcb;
}