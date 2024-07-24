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
        log_error(kernel_logger, "Error al asignar memoria para el MUTEX del ESTADO");
        return NULL;
    }
    pthread_mutex_init(mutex, NULL);
    estado->mutex_estado = mutex;

    sem_t *sem = malloc(sizeof(sem_t));
    if( sem == NULL ){
        log_error(kernel_logger, "Error al asignar memoria para el SEMAFORO del ESTADO");
        return NULL;
    }
    sem_init(sem, 0, 0); // La lista arranca vacia
    estado->sem_estado = sem;

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
}

t_pcb *estado_desencolar_primer_pcb(t_estado *estado){
    pthread_mutex_lock(estado_get_mutex(estado));
    t_pcb * pcb = list_remove(estado_get_lista_procesos(estado), 0);
    pthread_mutex_unlock(estado_get_mutex(estado));

    return pcb;
}

t_pcb *estado_desencolar_pcb_por_pid(t_estado *estado, int pid){
    pthread_mutex_lock(estado_get_mutex(estado));
    // int index = list_get_index(estado_get_lista_procesos(estado), pcb_comparar_pid, pid);
    // t_pcb * pcb = NULL;
    // if( index != -1 ){
    //     pcb = list_remove(estado_get_lista_procesos(estado), index);
    // }
    bool pcb_comparar_pid(void *pcb){ return pcb_get_pid((t_pcb *) pcb) == pid; } // "nested function"
    t_pcb *pcb = list_remove_by_condition(estado_get_lista_procesos(estado), pcb_comparar_pid);
    pthread_mutex_unlock(estado_get_mutex(estado));

    return pcb;
}

bool estado_contiene_pcbs(t_estado *estado){
    return !list_is_empty(estado_get_lista_procesos(estado));
}

t_pcb *estado_rastrear_y_desencolar_pcb_por_pid(int pid){
    t_pcb* pcb = NULL;
    
    if( estado_contiene_pcbs(estado_new) ){
        sem_wait(estado_get_sem(estado_new)); // debe haber elementos en la lista para poder desencolar
        pcb = estado_desencolar_pcb_por_pid(estado_new, pid);
        if( pcb != NULL ){
            return pcb;
        }
        sem_post(estado_get_sem(estado_new));
    }
    if( estado_contiene_pcbs(estado_ready) ){
        sem_wait(estado_get_sem(estado_ready)); // debe haber elementos en la lista para poder desencolar
        pcb = estado_desencolar_pcb_por_pid(estado_ready, pid);
        if( pcb != NULL ){
            return pcb;
        }
        sem_post(estado_get_sem(estado_ready));
    }
    if( estado_contiene_pcbs(estado_ready_plus) ){
        sem_wait(estado_get_sem(estado_ready_plus)); // debe haber elementos en la lista para poder desencolar
        pcb = estado_desencolar_pcb_por_pid(estado_ready_plus, pid);
        if( pcb != NULL ){
            return pcb;
        }
        sem_post(estado_get_sem(estado_ready_plus));
    }
    if( estado_contiene_pcbs(estado_blocked) ){
        sem_wait(estado_get_sem(estado_blocked)); // debe haber elementos en la lista para poder desencolar
        pcb = estado_desencolar_pcb_por_pid(estado_blocked, pid);
        if( pcb != NULL ){
            return pcb;
        }
        sem_post(estado_get_sem(estado_blocked));
    }
    if( estado_contiene_pcbs(estado_exec) ){
        sem_wait(estado_get_sem(estado_exec)); // debe haber elementos en la lista para poder desencolar
        pcb = estado_desencolar_pcb_por_pid(estado_exec, pid);
        if( pcb != NULL ){
            return pcb;
        }
        sem_post(estado_get_sem(estado_exec));
    }
    
    return pcb;
}

// ---------------------------
// t_pcb *estado_desencolar_pcb_por_indice(t_estado *estado, int indice){
//     sem_wait(estado_get_sem(estado)); // debe haber elementos en la lista para poder desencolar
//     pthread_mutex_lock(estado_get_mutex(estado));
//     t_pcb * pcb = list_remove(estado_get_lista_procesos(estado), indice);
//     pthread_mutex_unlock(estado_get_mutex(estado));

//     return pcb;
// }

// bool pcb_comparar_pid(void *pcb, void *pid){
//     return pcb_get_pid((t_pcb *) pcb) == *(int *) pid;
// }

// int list_get_index(t_list *list, bool (*cutting_condition)(void *temp, void *target), void *target){
//     // Busqueda secuencial hasta encontrar el elemento buscado (target) -> si lo encuentra devuelve el indice, si no lo encuentra devuelve -1
//     for(int i = 0; i < list_size(list); i++){
//         void *temp = list_get(list, i);

//         if(cutting_condition(temp, target)){
//             return i;
//         }
//     }

//     return -1;
// }

// t_estado *estado_rastrear_pcb_por_pid(int pid){
//     if( estado_contiene_pcbs(estado_new) ){
//         int index = list_get_index(estado_get_lista_procesos(estado_new), pcb_comparar_pid, &pid);
//         if( index != -1 ){
//             return estado_new;
//         }
//     }
//     if( estado_contiene_pcbs(estado_ready) ){
//         int index = list_get_index(estado_get_lista_procesos(estado_ready), pcb_comparar_pid, &pid);
//         if( index != -1 ){
//             return estado_ready;
//         }
//     }
//     if( estado_contiene_pcbs(estado_ready_plus) ){
//         int index = list_get_index(estado_get_lista_procesos(estado_ready_plus), pcb_comparar_pid, &pid);
//         if( index != -1 ){
//             return estado_ready_plus;
//         }
//     }
//     if( estado_contiene_pcbs(estado_blocked) ){
//         int index = list_get_index(estado_get_lista_procesos(estado_blocked), pcb_comparar_pid, &pid);
//         if( index != -1 ){
//             return estado_blocked;
//         }
//     }
//     if( estado_contiene_pcbs(estado_exec) ){
//         int index = list_get_index(estado_get_lista_procesos(estado_exec), pcb_comparar_pid, &pid);
//         if( index != -1 ){
//             return estado_exec;
//         }
//     }
//     if( estado_contiene_pcbs(estado_exit) ){
//         int index = list_get_index(estado_get_lista_procesos(estado_exit), pcb_comparar_pid, &pid);
//         if( index != -1 ){
//             return estado_exit;
//         }
//     }

//     return NULL;
// }