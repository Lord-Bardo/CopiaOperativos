#include "../include/kernel_pcb.h"

t_pcb *crear_pcb(int pid, char* path){
    t_pcb *pcb = malloc(sizeof(t_pcb));
    if( pcb == NULL ){
        log_error(kernel_logger, "Error al asignar memoria para el PCB");
        return NULL;
    }

    pcb->pid = pid;
    pcb->estado = NEW;
    pcb->quantum = QUANTUM;
    pcb->PC = 0;
    pcb->registros = crear_registros();

    pcb->path = string_new();
    if( pcb->path == NULL ){
        log_error(kernel_logger, "Error al asignar memoria para el PATH");
        return NULL;
    }
    string_append(&(pcb->path), path);

    pcb->diccionario_recursos_usados = dictionary_create(); // KEY: Nombre del recurso - Value: Cantidad de instancias usadas

    return pcb;
}

void eliminar_pcb(t_pcb *pcb){ // agregar diccionario
    if( pcb != NULL ){
        if( pcb->registros != NULL ){
            eliminar_registros(pcb->registros);
        }
        if( pcb->path != NULL ){
            free(pcb->path);
        }

        if( pcb->diccionario_recursos_usados != NULL ){
            dictionary_destroy(pcb->diccionario_recursos_usados);
        }

        free(pcb);
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

int pcb_get_quantum(t_pcb *pcb){
    return pcb->quantum;
}

t_nombre_estado pcb_get_estado(t_pcb *pcb){
    return pcb->estado;
}

void pcb_cambiar_estado_a(t_pcb *pcb, t_nombre_estado nuevo_estado){
    t_nombre_estado estado_anterior = pcb_get_estado(pcb);
    pcb->estado = nuevo_estado;
    log_cambio_estado(pcb, estado_anterior, nuevo_estado);
}

uint32_t pcb_get_pc(t_pcb *pcb){
    return pcb->PC;
}

t_registros *pcb_get_registros(t_pcb *pcb){
    return pcb->registros;
}

char *pcb_get_registro_ax(t_pcb *pcb){
    return registros_get_registro_ax(pcb->registros);
}

char *pcb_get_registro_bx(t_pcb *pcb){
    return registros_get_registro_bx(pcb->registros);
}

char *pcb_get_registro_cx(t_pcb *pcb){
    return registros_get_registro_cx(pcb->registros);
}

char *pcb_get_registro_dx(t_pcb *pcb){
    return registros_get_registro_dx(pcb->registros);
}

char *pcb_get_registro_eax(t_pcb *pcb){
    return registros_get_registro_eax(pcb->registros);
}

char *pcb_get_registro_ebx(t_pcb *pcb){
    return registros_get_registro_ebx(pcb->registros);
}

char *pcb_get_registro_ecx(t_pcb *pcb){
    return registros_get_registro_ecx(pcb->registros);
}

char *pcb_get_registro_edx(t_pcb *pcb){
    return registros_get_registro_edx(pcb->registros);
}

char *pcb_get_registro_si(t_pcb *pcb){
    return registros_get_registro_si(pcb->registros);
}

char *pcb_get_registro_di(t_pcb *pcb){
    return registros_get_registro_di(pcb->registros);
}

void pcb_set_registros(t_pcb *pcb, t_registros *registros){
    pcb->registros = registros;
}

void pcb_set_registro_ax(t_pcb *pcb, char *valor){
    registros_set_registro_ax(pcb->registros, valor);
}

void pcb_set_registro_bx(t_pcb *pcb, char *valor){
    registros_set_registro_bx(pcb->registros, valor);
}

void pcb_set_registro_cx(t_pcb *pcb, char *valor){
    registros_set_registro_cx(pcb->registros, valor);
}

void pcb_set_registro_dx(t_pcb *pcb, char *valor){
    registros_set_registro_dx(pcb->registros, valor);
}

void pcb_set_registro_eax(t_pcb *pcb, char *valor){
    registros_set_registro_eax(pcb->registros, valor);
}

void pcb_set_registro_ebx(t_pcb *pcb, char *valor){
    registros_set_registro_ebx(pcb->registros, valor);
}

void pcb_set_registro_ecx(t_pcb *pcb, char *valor){
    registros_set_registro_ecx(pcb->registros, valor);
}

void pcb_set_registro_edx(t_pcb *pcb, char *valor){
    registros_set_registro_edx(pcb->registros, valor);
}

void pcb_set_registro_si(t_pcb *pcb, char *valor){
    registros_set_registro_si(pcb->registros, valor);
}

void pcb_set_registro_di(t_pcb *pcb, char *valor){
    registros_set_registro_di(pcb->registros, valor);
}

char* pcb_get_path(t_pcb *pcb){
    return pcb->path;
}

t_dictionary *pcb_get_diccionario_recursos_usados(t_pcb *pcb){
    return pcb->diccionario_recursos_usados;
}

bool pcb_usa_recurso(t_pcb *pcb, char *nombre_recurso){
    return dictionary_has_key(pcb_get_diccionario_recursos_usados(pcb), nombre_recurso);
}