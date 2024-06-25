#include "../include/kernel_recursos.h"

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