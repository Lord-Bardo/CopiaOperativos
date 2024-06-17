#include "../include/entradasalida_kernel.h"

// DUDA: cuando tengo que interactuar con memoria y cuando con kernel?
void atender_entradasalida_kernel(){
    while (1) {
        t_codigo_operacion instruccion;
        t_buffer *buffer = crear_buffer();

        recibir_paquete(fd_kernel, &instruccion, buffer);

        int cant_unidades_trabajo;
        buffer_desempaquetar(buffer,&cant_unidades_trabajo);
    
        switch (instruccion){
            case IO_GEN_SLEEP:
                interfaz_generica(cant_unidades_trabajo);
                break;
            default:
                log_info(entradasalida_logger, "Instruccion desconocida");
        } 

        eliminar_buffer(buffer);

    }
}