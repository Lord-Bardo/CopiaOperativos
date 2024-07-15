#include "registros.h"

t_registros *crear_registros(){
    t_registros *registros = malloc(sizeof(t_registros));
    if( registros == NULL ){
        perror("Error al asignar memoria para los REGISTROS");
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

void eliminar_registros(t_registros *registros){
    if( registros != NULL){
        if( registros->AX != NULL ){
            free(registros->AX);
        }
        if( registros->BX != NULL ){
            free(registros->BX);
        }
        if( registros->CX != NULL ){
            free(registros->CX);
        }
        if( registros->DX != NULL ){
            free(registros->DX);
        }
        if( registros->EAX != NULL ){
            free(registros->EAX);
        }
        if( registros->EBX != NULL ){
            free(registros->EBX);
        }
        if( registros->ECX != NULL ){
            free(registros->ECX);
        }
        if( registros->EDX != NULL ){
            free(registros->EDX);
        }
        if( registros->SI != NULL ){
            free(registros->SI);
        }
        if( registros->DI != NULL ){
            free(registros->DI);
        }
        free(registros);
    }
}

// get
char *registros_get_registro(char *registro, int bytes_registro){
    // Devuelvo un string para que su manejo sea mas sencillo, pero no modifico el registro    
    char* registro_string;

    // Si es NULL => no se le seteo ningun valor => retorno un string vacio
    if( registro == NULL ){
        registro_string = malloc(1);
        if( registro_string == NULL ){
            perror("Error al asignar memoria para el REGISTRO STRING");
            return NULL;
        }
        registro_string[0] = '\0';
    }
    else{
        registro_string = malloc(bytes_registro + 1);
        if( registro_string == NULL ){
            perror("Error al asignar memoria para el REGISTRO STRING");
            return NULL;
        }
        memcpy(registro_string, registro, bytes_registro);
        registro_string[bytes_registro] = '\0';
    }

    return registro_string;
}

char *registros_get_registro_ax(t_registros *registros){
    return registros_get_registro(registros->AX, 1);
}

char *registros_get_registro_bx(t_registros *registros){
    return registros_get_registro(registros->BX, 1);
}

char *registros_get_registro_cx(t_registros *registros){
    return registros_get_registro(registros->CX, 1);
}

char *registros_get_registro_dx(t_registros *registros){
    return registros_get_registro(registros->DX, 1);
}

char *registros_get_registro_eax(t_registros *registros){
    return registros_get_registro(registros->EAX, 4);
}

char *registros_get_registro_ebx(t_registros *registros){
    return registros_get_registro(registros->EBX, 4);
}

char *registros_get_registro_ecx(t_registros *registros){
    return registros_get_registro(registros->ECX, 4);
}

char *registros_get_registro_edx(t_registros *registros){
    return registros_get_registro(registros->EDX, 4);
}

char *registros_get_registro_si(t_registros *registros){
    return registros_get_registro(registros->SI, 4);
}

char *registros_get_registro_di(t_registros *registros){
    return registros_get_registro(registros->DI, 4);
}

// set
void registros_set_registro(char **registro, char *valor, int bytes_registro){
    // Si el string esta vacio => se deja el registro como estaba
    if( string_is_empty(valor) ) return;
    // FORMA PISA VALOR ANTERIOR Y LO PONE EN NULL
    // if( string_is_empty(valor) ){
    //     *registro = NULL;
    //     return;
    // }
    
    // Si el string tiene contenido, pero el registro esta en NULL => se le asigna memoria
    if( *registro == NULL ){
        *registro = malloc(bytes_registro);
        if( *registro == NULL ){
            perror("Error al asignar memoria para el REGISTRO");
            return;
        }
    }

    // Se le asigna el valor al registro
    memmove(*registro, valor, bytes_registro);
}

void registros_set_registro_ax(t_registros *registros, char *valor){
    registros_set_registro(&(registros->AX), valor, 1);
}

void registros_set_registro_bx(t_registros *registros, char *valor){
    registros_set_registro(&(registros->BX), valor, 1);
}

void registros_set_registro_cx(t_registros *registros, char *valor){
    registros_set_registro(&(registros->CX), valor, 1);
}

void registros_set_registro_dx(t_registros *registros, char *valor){
    registros_set_registro(&(registros->DX), valor, 1);
}

void registros_set_registro_eax(t_registros *registros, char *valor){
    registros_set_registro(&(registros->EAX), valor, 4);
}

void registros_set_registro_ebx(t_registros *registros, char *valor){
    registros_set_registro(&(registros->EBX), valor, 4);
}

void registros_set_registro_ecx(t_registros *registros, char *valor){
    registros_set_registro(&(registros->ECX), valor, 4);
}

void registros_set_registro_edx(t_registros *registros, char *valor){
    registros_set_registro(&(registros->EDX), valor, 4);
}

void registros_set_registro_si(t_registros *registros, char *valor){
    registros_set_registro(&(registros->SI), valor, 4);
}

void registros_set_registro_di(t_registros *registros, char *valor){
    registros_set_registro(&(registros->DI), valor, 4);
}

// OTRA FORMA: ---------------------------
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

// void registros_set_registro(char **registro, char* valor, int bytes_registro){
//     // Se le asigna el valor al registro
//     memmove(*registro, valor, bytes_registro);
//     memmove(*registro + bytes_registro, "\0", 1);
// }

// PCB KERNEL ---------------------------
// t_pcb *crear_pcb(int pid, char* path){
//     t_pcb *pcb = malloc(sizeof(t_pcb));
//     if( pcb == NULL ){
//         log_error(kernel_logger, "Error al asignar memoria para el PCB");
//         return NULL;
//     }

//     pcb->pid = pid;
//     pcb->estado = NEW;
//     pcb->quantum = QUANTUM;
//     pcb->PC = 0;
//     pcb->registros = crear_registros();

//     pcb->path = string_new();
//     if( pcb->path == NULL ){
//         log_error(kernel_logger, "Error al asignar memoria para el PATH");
//         return NULL;
//     }
//     string_append(&(pcb->path), path);

//     pcb->diccionario_recursos_usados = dictionary_create(); // KEY: Nombre del recurso - Value: Cantidad de instancias usadas

//     return pcb;
// }

// void eliminar_pcb(t_pcb *pcb){
//     if( pcb != NULL ){
//         if( pcb->registros != NULL ){
//             eliminar_registros(pcb->registros);
//         }
//         if( pcb->path != NULL ){
//             free(pcb->path);
//         }

//         if( pcb->diccionario_recursos_usados != NULL ){
//             dictionary_destroy(pcb->diccionario_recursos_usados);
//         }

//         free(pcb);
//         pcb = NULL; // para realizar comprobacion de si el proceso ya fue finalizado (en kernel_entradasalida.c por ejemplo)
//     }
// }

// t_registros *pcb_get_registros(t_pcb *pcb){
//     return pcb->registros;
// }

// char *pcb_get_registro_ax(t_pcb *pcb){
//     return registros_get_registro_ax(pcb->registros);
// }

// char *pcb_get_registro_bx(t_pcb *pcb){
//     return registros_get_registro_bx(pcb->registros);
// }

// char *pcb_get_registro_cx(t_pcb *pcb){
//     return registros_get_registro_cx(pcb->registros);
// }

// char *pcb_get_registro_dx(t_pcb *pcb){
//     return registros_get_registro_dx(pcb->registros);
// }

// char *pcb_get_registro_eax(t_pcb *pcb){
//     return registros_get_registro_eax(pcb->registros);
// }

// char *pcb_get_registro_ebx(t_pcb *pcb){
//     return registros_get_registro_ebx(pcb->registros);
// }

// char *pcb_get_registro_ecx(t_pcb *pcb){
//     return registros_get_registro_ecx(pcb->registros);
// }

// char *pcb_get_registro_edx(t_pcb *pcb){
//     return registros_get_registro_edx(pcb->registros);
// }

// char *pcb_get_registro_si(t_pcb *pcb){
//     return registros_get_registro_si(pcb->registros);
// }

// char *pcb_get_registro_di(t_pcb *pcb){
//     return registros_get_registro_di(pcb->registros);
// }

// void pcb_set_registros(t_pcb *pcb, t_registros *registros){
//     pcb->registros = registros;
// }

// void pcb_set_registro_ax(t_pcb *pcb, char *valor){
//     registros_set_registro_ax(pcb->registros, valor);
// }

// void pcb_set_registro_bx(t_pcb *pcb, char *valor){
//     registros_set_registro_bx(pcb->registros, valor);
// }

// void pcb_set_registro_cx(t_pcb *pcb, char *valor){
//     registros_set_registro_cx(pcb->registros, valor);
// }

// void pcb_set_registro_dx(t_pcb *pcb, char *valor){
//     registros_set_registro_dx(pcb->registros, valor);
// }

// void pcb_set_registro_eax(t_pcb *pcb, char *valor){
//     registros_set_registro_eax(pcb->registros, valor);
// }

// void pcb_set_registro_ebx(t_pcb *pcb, char *valor){
//     registros_set_registro_ebx(pcb->registros, valor);
// }

// void pcb_set_registro_ecx(t_pcb *pcb, char *valor){
//     registros_set_registro_ecx(pcb->registros, valor);
// }

// void pcb_set_registro_edx(t_pcb *pcb, char *valor){
//     registros_set_registro_edx(pcb->registros, valor);
// }

// void pcb_set_registro_si(t_pcb *pcb, char *valor){
//     registros_set_registro_si(pcb->registros, valor);
// }

// void pcb_set_registro_di(t_pcb *pcb, char *valor){
//     registros_set_registro_di(pcb->registros, valor);
// }

// UTILS KERNEL
// void agregar_registros_a_paquete(t_paquete *paquete, t_registros *registros){
//     char *ax = registros_get_registro_ax(registros);
//     agregar_string_a_paquete(paquete, ax);
//     free(ax);

//     char *bx = registros_get_registro_bx(registros);
//     agregar_string_a_paquete(paquete, bx);
//     free(bx);

//     char *cx = registros_get_registro_cx(registros);
//     agregar_string_a_paquete(paquete, cx);
//     free(cx);

//     char *dx = registros_get_registro_dx(registros);
//     agregar_string_a_paquete(paquete, dx);
//     free(dx);

//     char *eax = registros_get_registro_eax(registros);
//     agregar_string_a_paquete(paquete, eax);
//     free(eax);

//     char *ebx = registros_get_registro_ebx(registros);
//     agregar_string_a_paquete(paquete, ebx);
//     free(ebx);

//     char *ecx = registros_get_registro_ecx(registros);
//     agregar_string_a_paquete(paquete, ecx);
//     free(ecx);

//     char *edx = registros_get_registro_edx(registros);
//     agregar_string_a_paquete(paquete, edx);
//     free(edx);

//     char *si = registros_get_registro_si(registros);
//     agregar_string_a_paquete(paquete, si);
//     free(si);

//     char *di = registros_get_registro_di(registros);
//     agregar_string_a_paquete(paquete, di);
//     free(di);
// }

// void buffer_desempaquetar_registros(t_buffer *buffer, t_registros *registros){
//     // Si buffer_desempaquetar_string lee "\0", en registros_set_registro se va a dejar al registro como estaba -> Si estaba en NULL queda en NULL y sino quedara con lo q ya tenga
//     registros_set_registro_ax(registros, buffer_desempaquetar_string(buffer));
//     registros_set_registro_bx(registros, buffer_desempaquetar_string(buffer));
//     registros_set_registro_cx(registros, buffer_desempaquetar_string(buffer));
//     registros_set_registro_dx(registros, buffer_desempaquetar_string(buffer));
//     registros_set_registro_eax(registros, buffer_desempaquetar_string(buffer));
//     registros_set_registro_ebx(registros, buffer_desempaquetar_string(buffer));
//     registros_set_registro_ecx(registros, buffer_desempaquetar_string(buffer));
//     registros_set_registro_edx(registros, buffer_desempaquetar_string(buffer));
//     registros_set_registro_si(registros, buffer_desempaquetar_string(buffer));
//     registros_set_registro_di(registros, buffer_desempaquetar_string(buffer));
// }

// void buffer_desempaquetar_contexto_ejecucion(t_buffer *buffer, t_pcb* pcb){
//     int pid_recibido;
//     buffer_desempaquetar(buffer, &pid_recibido);
//     if( pid_recibido != pcb_get_pid(pcb) ){
//         log_error(kernel_logger, "El PID recibido no se corresponde con el PID del proceso en ejecucion");
//     }
    
//     buffer_desempaquetar(buffer, &(pcb->PC));
//     buffer_desempaquetar_registros(buffer, pcb_get_registros(pcb));
// }