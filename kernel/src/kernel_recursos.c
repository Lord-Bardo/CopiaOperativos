#include "../include/kernel_recursos.h"

// RECURSO --------------------------------------------------

t_recurso *crear_recurso(int instancias){
    t_recurso *recurso = malloc(sizeof(t_recurso));
    if( recurso == NULL ){
        log_error(kernel_logger, "Error al asignar memoria para el RECURSO");
        return NULL;
    }

    recurso->instancias = instancias;
    recurso->lista_procesos_bloqueados = list_create();

    pthread_mutex_t *mutex_lpb = malloc(sizeof(pthread_mutex_t));
    if( mutex_lpb == NULL ){
        log_error(kernel_logger, "Error al asignar memoria para el MUTEX del RECURSO");
        return NULL;
    }
    pthread_mutex_init(mutex_lpb, NULL);
    recurso->mutex_lista_procesos_bloqueados = mutex_lpb;

    sem_t *sem_lpb = malloc(sizeof(sem_t));
    if( sem_lpb == NULL ){
        log_error(kernel_logger, "Error al asignar memoria para el SEMAFORO del RECURSO");
        return NULL;
    }
    sem_init(sem_lpb, 0, 0); // La lista arranca vacia
    recurso->sem_lista_procesos_bloqueados = sem_lpb;

    pthread_mutex_t *mutex_r = malloc(sizeof(pthread_mutex_t));
    if( mutex_r == NULL ){
        log_error(kernel_logger, "Error al asignar memoria para el MUTEX del RECURSO");
        return NULL;
    }
    pthread_mutex_init(mutex_r, NULL);
    recurso->mutex_recurso = mutex_r;

    return recurso;
}

void eliminar_recurso(void *recurso_void){ // lo recibo como void * porq necesito esta firma para la funcion eliminar_diccionario_recursos
    t_recurso *recurso = (t_recurso *)recurso_void;
    
    if( recurso != NULL ){
        if( list_is_empty(recurso->lista_procesos_bloqueados) ){
            list_destroy(recurso->lista_procesos_bloqueados);
        }
        else{
            list_destroy_and_destroy_elements(recurso->lista_procesos_bloqueados, pcb_destroyer);
        }

        pthread_mutex_destroy(recurso->mutex_lista_procesos_bloqueados);
        free(recurso->mutex_lista_procesos_bloqueados);

        sem_destroy(recurso->sem_lista_procesos_bloqueados);
        free(recurso->sem_lista_procesos_bloqueados);

        pthread_mutex_destroy(recurso->mutex_recurso);
        free(recurso->mutex_recurso);

        free(recurso);
    }
}

int recurso_get_instancias(t_recurso *recurso){
    return recurso->instancias;
}

t_list *recurso_get_lista_procesos_bloqueados(t_recurso *recurso){
    return recurso->lista_procesos_bloqueados;
}

pthread_mutex_t *recurso_get_mutex_lista_procesos_bloqueados(t_recurso *recurso){
    return recurso->mutex_lista_procesos_bloqueados;
}

sem_t *estado_get_sem_lista_procesos_bloqueados(t_recurso *recurso){
    return recurso->sem_lista_procesos_bloqueados;
}

pthread_mutex_t *recurso_get_mutex_recurso(t_recurso *recurso){
    return recurso->mutex_recurso;
}

void recurso_wait(t_recurso *recurso){
    recurso->instancias--;
}

void recurso_signal(t_recurso *recurso){
    recurso->instancias++;
}

bool recurso_debe_desbloquear_proceso(t_recurso *recurso){
    return !list_is_empty(recurso_get_lista_procesos_bloqueados(recurso)) && recurso_get_instancias(recurso) == 0; // si la segunda se cumple, la primera tmb deberia si o si, pero bueno por las dudas la agregue
}

bool recurso_debe_bloquear_proceso(t_recurso *recurso){
    return recurso_get_instancias(recurso) < 0;
} 

t_pcb *recurso_desencolar_primer_proceso(t_recurso *recurso){
    sem_wait(estado_get_sem_lista_procesos_bloqueados(recurso)); // debe haber elementos en la lista para poder desencolar
    pthread_mutex_lock(recurso_get_mutex_lista_procesos_bloqueados(recurso));
    t_pcb * pcb = list_remove(recurso_get_lista_procesos_bloqueados(recurso), 0);
    pthread_mutex_unlock(recurso_get_mutex_lista_procesos_bloqueados(recurso));

    return pcb;
}

void recurso_encolar_proceso(t_recurso *recurso, t_pcb *pcb){
    pthread_mutex_lock(recurso_get_mutex_lista_procesos_bloqueados(recurso));
    list_add(recurso_get_lista_procesos_bloqueados(recurso), pcb);
    pthread_mutex_unlock(recurso_get_mutex_lista_procesos_bloqueados(recurso));
    sem_post(estado_get_sem_lista_procesos_bloqueados(recurso));
}

t_pcb *recurso_desencolar_proceso_por_pid(t_recurso *recurso, int pid){
    sem_wait(estado_get_sem_lista_procesos_bloqueados(recurso)); // debe haber elementos en la lista para poder desencolar
    pthread_mutex_lock(recurso_get_mutex_lista_procesos_bloqueados(recurso));
    bool pcb_comparar_pid(void *pcb){ return pcb_get_pid((t_pcb *) pcb) == pid; } // "nested function"
    t_pcb *pcb = list_remove_by_condition(recurso_get_lista_procesos_bloqueados(recurso), pcb_comparar_pid);
    pthread_mutex_unlock(recurso_get_mutex_lista_procesos_bloqueados(recurso));

    return pcb;
}

// DICCIONARIO RECURSOS --------------------------------------------------

t_dictionary *crear_diccionario_recursos(char** recursos, char** instancias_recursos){
    t_dictionary *diccionario_recursos = dictionary_create();
    for(int i = 0; recursos[i] != NULL && instancias_recursos[i] != NULL; i++){
        dictionary_put(diccionario_recursos, recursos[i], (void *) crear_recurso(atoi(instancias_recursos[i])));
    }

    return diccionario_recursos;
}

void eliminar_diccionario_recursos(t_dictionary *diccionario_recursos){
    dictionary_destroy_and_destroy_elements(diccionario_recursos, eliminar_recurso);
}

t_recurso *diccionario_recursos_get_recurso(t_dictionary *diccionario_recursos, char *nombre_recurso){
    return dictionary_get(diccionario_recursos, nombre_recurso);
}

bool diccionario_recursos_existe_recurso(t_dictionary *diccionario_recursos, char *nombre_recurso){
    return dictionary_has_key(diccionario_recursos, nombre_recurso);
}