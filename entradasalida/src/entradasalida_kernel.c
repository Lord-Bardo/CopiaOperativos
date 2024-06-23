#include "../include/entradasalida_kernel.h"

// DUDA: cuando tengo que interactuar con memoria y cuando con kernel?
void atender_entradasalida_kernel(){
    t_codigo_operacion instruccion;
    t_buffer *buffer = crear_buffer(); 

    while (1) {

        recibir_paquete(fd_kernel, &instruccion, buffer);
    
        // TODO - Debería de modelar la notificación a KERNEL de que ya termine de usar la interfaz
        switch (instruccion){
            case IO_GEN_SLEEP:
                // IO_GEN_SLEEP Int1 10
                int cant_unidades_trabajo;
                buffer_desempaquetar(buffer,&cant_unidades_trabajo);

                interfaz_generica(cant_unidades_trabajo);

                break; // Para salir del switch, sino intenta desempaquetar denuevo
            case IO_STDIN_READ:
                // IO_STDIN_READ Int2 EAX AX
                char* registro_tamanio;
                buffer_desempaquetar(buffer, &registro_tamanio);

                char* registro_direccion = malloc(atoi(registro_tamanio));
                buffer_desempaquetar(buffer, registro_direccion);

                interfaz_stdin(registro_direccion, registro_tamanio);

                free(registro_direccion);
                break;
            case IO_STDOUT_WRITE:
                // IO_STDOUT_WRITE Int3 BX EAX
                char* registro_taman; // Le tuve que cambiar el nombre pq me tira que lo estoy redifiniendo por el case de arriba, ver de unificar esto en una funcion
                buffer_desempaquetar(buffer, &registro_taman);

                char* registro_direc = malloc(atoi(registro_taman));
                buffer_desempaquetar(buffer, registro_direc);

                interfaz_stdout(registro_direc , registro_taman);

                free(registro_direc);
                break;
            default:
                log_info(entradasalida_logger, "Instruccion desconocida");
        } 


    }
    eliminar_buffer(buffer);
}