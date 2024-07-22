#include "../include/kernel_pcb.h"

// PCB --------------------------------------------------
t_pcb *crear_pcb(int pid, char* path){
    t_pcb *pcb = malloc(sizeof(t_pcb));
    if( pcb == NULL ){
        log_error(kernel_logger, "Error al asignar memoria para el PCB");
        return NULL;
    }

    pcb->pid = pid;
    pcb->quantum_inicial = QUANTUM;
    pcb->quantum_restante = QUANTUM;
    pcb->estado = NEW;
    pcb->PC = 0;
    pcb->registros.AX = 0;
    pcb->registros.BX = 0;
    pcb->registros.CX = 0;
    pcb->registros.DX = 0;
    pcb->registros.EAX = 0;
    pcb->registros.EBX = 0;
    pcb->registros.ECX = 0;
    pcb->registros.EDX = 0;
    pcb->registros.SI = 0;
    pcb->registros.DI = 0;

    pcb->path = string_new();
    if( pcb->path == NULL ){
        log_error(kernel_logger, "Error al asignar memoria para el PATH");
        return NULL;
    }
    string_append(&(pcb->path), path);

    pcb->nombre_recurso_causa_bloqueo = string_new();
    if( pcb->nombre_recurso_causa_bloqueo == NULL ){
        log_error(kernel_logger, "Error al asignar memoria para el NOMBRE RECURSO CAUSA BLOQUEO");
        return NULL;
    }

    pcb->diccionario_recursos_usados = dictionary_create(); // KEY: Nombre del recurso - Value: Cantidad de instancias usadas

    return pcb;
}

void eliminar_pcb(t_pcb *pcb){
    if( pcb != NULL ){
        if( pcb->path != NULL ){
            free(pcb->path);
        }

        if( pcb->nombre_recurso_causa_bloqueo != NULL ){
            free(pcb->nombre_recurso_causa_bloqueo);
        }

        if( pcb->diccionario_recursos_usados != NULL ){
            dictionary_destroy(pcb->diccionario_recursos_usados);
        }

        free(pcb);
        pcb = NULL; // para realizar comprobacion de si el proceso ya fue finalizado (en kernel_entradasalida.c por ejemplo)
    }
}

int generar_pid(){
	pthread_mutex_lock(&mutex_pid);
	int pid_proceso = pid_actual;
	pid_actual++;
	pthread_mutex_unlock(&mutex_pid);

	return pid_proceso;
}

int pcb_get_pid(t_pcb *pcb){
    return pcb->pid;
}

int pcb_get_quantum_inicial(t_pcb *pcb){
    return pcb->quantum_inicial;
}

void pcb_set_quantum_inicial(t_pcb *pcb, int quantum){
    pcb->quantum_inicial = quantum;
}

int pcb_get_quantum_restante(t_pcb *pcb){
    return pcb->quantum_restante;
}

void pcb_set_quantum_restante(t_pcb *pcb, int quantum){
    pcb->quantum_restante = quantum;
}

t_nombre_estado pcb_get_estado(t_pcb *pcb){
    return pcb->estado;
}

t_registros pcb_get_registros(t_pcb *pcb){
    return pcb->registros;
}

void pcb_cambiar_estado_a(t_pcb *pcb, t_nombre_estado nuevo_estado){
    t_nombre_estado estado_anterior = pcb_get_estado(pcb);
    pcb->estado = nuevo_estado;
    log_cambio_estado(pcb, estado_anterior, nuevo_estado);
}

uint32_t pcb_get_pc(t_pcb *pcb){
    return pcb->PC;
}

char *pcb_get_path(t_pcb *pcb){
    return pcb->path;
}

char *pcb_get_nombre_recurso_causa_bloqueo(t_pcb *pcb){
    return pcb->nombre_recurso_causa_bloqueo;
}

void pcb_set_nombre_recurso_causa_bloqueo(t_pcb *pcb, char *nombre_recurso_causa_bloqueo){
    // Libero la memoria previamente asignada, si existe
    if (pcb->nombre_recurso_causa_bloqueo != NULL) {
        free(pcb->nombre_recurso_causa_bloqueo);
    }

    // Le asigno una nueva cadena al campo
    pcb->nombre_recurso_causa_bloqueo = string_duplicate(nombre_recurso_causa_bloqueo);
}

t_dictionary *pcb_get_diccionario_recursos_usados(t_pcb *pcb){
    return pcb->diccionario_recursos_usados;
}

bool pcb_usa_recurso(t_pcb *pcb, char *nombre_recurso){
    return dictionary_has_key(pcb_get_diccionario_recursos_usados(pcb), nombre_recurso);
}

bool pcb_esta_bloqueado_por_recurso(t_pcb *pcb){
    return !string_is_empty(pcb_get_nombre_recurso_causa_bloqueo(pcb));
}

// PROCESOS A FINALIZAR --------------------------------------------------
// Implementacion con diccionario
void diccionario_procesos_a_finalizar_agregar_proceso(t_dictionary *diccionario_procesos_a_finalizar, t_pcb *pcb){
    dictionary_put(diccionario_procesos_a_finalizar, string_itoa(pcb_get_pid(pcb)), NULL);
}

bool diccionario_procesos_a_finalizar_proceso_esta_pendiente_de_finalizar(t_dictionary *diccionario_procesos_a_finalizar, t_pcb *pcb){
    return dictionary_has_key(diccionario_procesos_a_finalizar, string_itoa(pcb_get_pid(pcb)));
}

void diccionario_procesos_a_finalizar_remover_proceso(t_dictionary *diccionario_procesos_a_finalizar, t_pcb *pcb){
    dictionary_remove(diccionario_procesos_a_finalizar, string_itoa(pcb_get_pid(pcb)));
}

// Implementacion con lista (muy enquilombada)
// void *lista_procesos_a_finalizar_buscar_pid(t_list *lista_procesos_a_finalizar, t_pcb *pcb){
//     int pid_a_encontrar = pcb_get_pid(pcb);
//     bool comparar_pid(void *pid){ return pid_a_encontrar == *(int *)pid; } // "nested function"
//     return list_find(lista_procesos_a_finalizar, comparar_pid);
// }

// bool diccionario_procesos_a_finalizar_proceso_esta_pendiente_de_finalizar(t_list *lista_procesos_a_finalizar, t_pcb *pcb){
//     return lista_procesos_a_finalizar_buscar_pid(lista_procesos_a_finalizar, pcb) != NULL;
// }

// void diccionario_procesos_a_finalizar_remover_proceso(t_list *lista_procesos_a_finalizar, t_pcb *pcb){
//     list_remove_element(lista_procesos_a_finalizar, lista_procesos_a_finalizar_buscar_pid(lista_procesos_a_finalizar, pcb));
// }

// CONTAR QUANTUM PARAMETROS --------------------------------------------------
t_contar_quantum_parametros *crear_contar_quantum_parametros(int quantum, int pid){
    t_contar_quantum_parametros *contar_quantum_parametros = malloc(sizeof(t_contar_quantum_parametros));
    if( contar_quantum_parametros == NULL ){
        log_error(kernel_logger, "Error al asignar memoria para la estructura CONTAR_QUANTUM");
        return NULL;
    }

    contar_quantum_parametros->quantum = quantum;
    contar_quantum_parametros->pid = pid;

    return contar_quantum_parametros;
}

void eliminar_contar_quantum_parametros(t_contar_quantum_parametros *contar_quantum_parametros){
    if( contar_quantum_parametros != NULL ){
        free(contar_quantum_parametros);
    }
}

int contar_quantum_parametros_get_quantum(t_contar_quantum_parametros *contar_quantum_parametros){
    return contar_quantum_parametros->quantum;
}

void contar_quantum_parametros_set_quantum(t_contar_quantum_parametros *contar_quantum_parametros, int quantum){
    contar_quantum_parametros->quantum = quantum;
}

int contar_quantum_parametros_get_pid(t_contar_quantum_parametros *contar_quantum_parametros){
    return contar_quantum_parametros->pid;
}