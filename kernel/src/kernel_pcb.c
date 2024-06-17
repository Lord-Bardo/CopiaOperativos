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

    return pcb;
}

void eliminar_pcb(t_pcb *pcb){
    if( pcb != NULL ){
        if( pcb->path != NULL ){
            free(pcb->path);
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

t_registros *crear_registros(){
    t_registros *registros = malloc(sizeof(t_registros));
    if( registros == NULL ){
        log_error(kernel_logger, "Error al asignar memoria para los REGISTROS");
        return NULL;
    }
    
    registros->AX = NULL;
    registros->BX = NULL;
    registros->CX = NULL;
    registros->DX = NULL;
    registros->EAX = NULL;
    registros->EBX = NULL;
    registros->ECX = NULL;
    registros->EDX = NULL;
    registros->SI = NULL;
    registros->DI = NULL;

    return registros;
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

uint32_t pcb_get_pc(t_pcb *pcb){
    return pcb->PC;
}

t_registros *pcb_get_registros(t_pcb *pcb){
    return pcb->registros;
}

// FORMA 1: ---------------------------
char *get_registro(char *registro, int bytes_registro){
    // Devuelvo un string para que su manejo sea mas sencillo    
    char* registro_string;

    // Si es NULL => no se le seteo ningun valor => retorno un string vacio
    if( registro == NULL ){
        registro_string = malloc(1);
        if( registro_string == NULL ){
            log_error(kernel_logger, "Error al asignar memoria para el REGISTRO STRING");
            return NULL;
        }
        registro_string[0] = '\0';
    }
    else{
        registro_string = malloc(bytes_registro + 1);
        if( registro_string == NULL ){
            log_error(kernel_logger, "Error al asignar memoria para el REGISTRO STRING");
            return NULL;
        }
        memcpy(registro_string, registro, bytes_registro);
        registro_string[bytes_registro] = '\0';
    }

    return registro_string;
}

char *pcb_get_registro_ax(t_pcb *pcb){
    return get_registro(pcb->registros->AX, 1);
}

char *pcb_get_registro_eax(t_pcb *pcb){
    return get_registro(pcb->registros->EAX, 4);
}

void set_registro(char **registro, char* valor, int bytes_registro){
    // Si el string esta vacio => se deja el registro como NULL
    if( string_is_empty(valor) ) return;
    
    // Si el string tiene contenido, pero el registro esta en NULL => se le asigna memoria
    if( *registro == NULL ){
        *registro = malloc(bytes_registro * sizeof(char));
        if( *registro == NULL ){
            log_error(kernel_logger, "Error al asignar memoria para el REGISTRO");
            return;
        }
    }

    // Se le asigna el valor al registro
    memmove(*registro, valor, bytes_registro);
}

// FORMA 2: ---------------------------
// t_registros *crear_registros(){
//     t_registros *registros = malloc(sizeof(t_registros));
//     if( registros == NULL ){
//         log_error(kernel_logger, "Error al asignar memoria para los REGISTROS");
//         return NULL;
//     }
    
//     registros->AX = string_new();
//     registros->BX = string_new();
//     registros->CX = string_new();
//     registros->DX = string_new();
//     registros->EAX = string_new();
//     registros->EBX = string_new();
//     registros->ECX = string_new();
//     registros->EDX = string_new();
//     registros->SI = string_new();
//     registros->DI = string_new();

//     return registros;
// }

// char *pcb_get_registro_ax(t_pcb *pcb){
//     return pcb->registros->AX;
// }

// char *pcb_get_registro_eax(t_pcb *pcb){
//     return pcb->registros->EAX;
// }

// void set_registro(char **registro, char* valor, int bytes_registro){
//     // Se le asigna el valor al registro
//     memmove(*registro, valor, bytes_registro);
//     memmove(*registro + bytes_registro, "\0", 1);
// }

void pcb_set_registro_ax(t_pcb *pcb, char *valor){
    set_registro(&(pcb->registros->AX), valor, 1);
}

void pcb_set_registro_eax(t_pcb *pcb, char *valor){
    set_registro(&(pcb->registros->EAX), valor, 4);
}

char* pcb_get_path(t_pcb *pcb){
    return pcb->path;
}

void pcb_cambiar_estado_a(t_pcb *pcb, t_nombre_estado nuevo_estado){
    t_nombre_estado estado_anterior = pcb_get_estado(pcb);
    pcb->estado = nuevo_estado;
    log_cambio_estado(pcb, estado_anterior, nuevo_estado);
}