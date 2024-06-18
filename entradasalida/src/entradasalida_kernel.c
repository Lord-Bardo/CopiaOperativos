#include "../include/entradasalida_kernel.h"

// DUDA: cuando tengo que interactuar con memoria y cuando con kernel?
void atender_entradasalida_kernel(){
    while (1) {
        t_codigo_operacion instruccion;
        t_buffer *buffer = crear_buffer(); 

        // TODO - Pregunta a mati, si recibo mas de dos cosas cómo hago? - Algunas interfaces reciben dos, otras más de dos
        recibir_paquete(fd_kernel, &instruccion, buffer);

        int cant_unidades_trabajo;
        buffer_desempaquetar(buffer,&cant_unidades_trabajo);
    
        switch (instruccion){
        // TODO - Debería de modelar la notificación a KERNEL de que ya termine de usar la interfaz
            case IO_GEN_SLEEP:
                // IO_GEN_SLEEP Int1 10
                interfaz_generica(cant_unidades_trabajo);
                break;
            case IO_STDIN_READ:
                // IO_STDIN_READ Int2 EAX AX
                // interfaz_stdin()
            default:
                log_info(entradasalida_logger, "Instruccion desconocida");
        } 

        eliminar_buffer(buffer);

    }
}

/* En caso de que la interfaz exista y esté conectada, KERNEL deberá validar que la interfaz admite la operación solicitada, en caso de que no sea así, se deberá enviar el proceso a EXIT. */

// Preguntar sobre esto:
/* Creo que voy a tener que hacer 4 módulos separados de entradasalida como si fueran otros módulos */
/* ¿Debería hacer 4 archivos de configuración para cada módulo? */
/* ¿Debería chequear cuando leo la consola que no se pase del tamaño pedido en el registro tamaño? */
/* Toda interfaz de I/O puede conectarse y desconectarse en tiempo de ejecución. No se evaluará el caso en que una interfaz se desconecte estando ocupada por un proceso. */