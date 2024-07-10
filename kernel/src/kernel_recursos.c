#include "../include/kernel_recursos.h"

// RECURSO --------------------------------------------------

t_recurso *crear_recurso(int instancias){
    t_recurso *recurso = malloc(sizeof(t_recurso));
    if( recurso == NULL ){
        log_error(kernel_logger, "Error al asignar memoria para el RECURSO");
        return NULL;
    }

    recurso->instancias = instancias;
    recurso->estado_recurso = crear_estado(RECURSO);

    return recurso;
}

void eliminar_recurso(void *recurso_void){ // lo recibo como void * porq necesito esta firma para la funcion eliminar_diccionario_recursos
    t_recurso *recurso = (t_recurso *)recurso_void;
    
    if( recurso != NULL ){
        if( recurso->estado_recurso != NULL ){
            eliminar_estado(recurso->estado_recurso);
        }

        free(recurso);
    }
}

t_estado *recurso_get_estado_recurso(t_recurso *recurso){
    return recurso->estado_recurso;
}

int recurso_get_instancias(t_recurso *recurso){
    return recurso->instancias;
}

void recurso_wait(t_recurso *recurso){
    recurso->instancias--; // necesito un mutex aca para la instancia? -> si lo llego a necesitar agregalo al struct de t_recurso
}

void recurso_signal(t_recurso *recurso){
    recurso->instancias++; // necesito un mutex aca para la instancia? -> si lo llego a necesitar agregalo al struct de t_recurso
}

bool recurso_debe_desbloquear_proceso(t_recurso *recurso){
    return estado_contiene_pcbs(recurso_get_estado_recurso(recurso)) && recurso_get_instancias(recurso) == 0;
}

bool recurso_debe_bloquear_proceso(t_recurso *recurso){
    return recurso_get_instancias(recurso) < 0;
} 

t_pcb *recurso_desencolar_primer_proceso(t_recurso *recurso){
    return estado_desencolar_primer_pcb(recurso_get_estado_recurso(recurso));
}

void recurso_encolar_proceso(t_recurso *recurso, t_pcb *pcb){
    estado_encolar_pcb(recurso_get_estado_recurso(recurso), pcb);
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