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