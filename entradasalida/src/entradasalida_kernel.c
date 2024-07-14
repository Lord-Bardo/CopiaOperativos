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
                buffer_desempaquetar(buffer, &cant_unidades_trabajo);

                interfaz_generica(cant_unidades_trabajo);

                enviar_codigo_operacion(fd_kernel, IO_FIN_OPERACION);
                break; // Para salir del switch, sino intenta desempaquetar denuevo
            case IO_STDIN_READ:
                // IO_STDIN_READ Int2 EAX AX
                /* PREGUNTARLE A FABRI COMO QUEDAN LOS REGISTROS AL FINAL */
                /* VER DE HACER AMBOS REGISTROS GLOBALES - raro porque capaz no los uso si uso la IO genérica */
                char* registro_tamanio = buffer_desempaquetar_string(buffer);
                char* registro_direccion = malloc(atoi(buffer_desempaquetar_string(buffer)));

                interfaz_stdin(registro_direccion, registro_tamanio);

                free(registro_tamanio);
                free(registro_direccion);

                enviar_codigo_operacion(fd_kernel, IO_FIN_OPERACION);
                break;
            case IO_STDOUT_WRITE:
                // IO_STDOUT_WRITE Int3 BX EAX
                char* registro_taman = buffer_desempaquetar_string(buffer);
                char* registro_direc = malloc(atoi(buffer_desempaquetar_string(buffer)));

                interfaz_stdout(registro_direc , registro_taman);

                free(registro_tamanio);
                free(registro_direccion);

                enviar_codigo_operacion(fd_kernel, IO_FIN_OPERACION);
                break;
            case IO_FS_CREATE:
                break;
            case IO_FS_DELETE:
                break;
            case IO_FS_TRUNCATE:
                break;
            case IO_FS_WRITE:
                break;
            case IO_FS_READ:
                break;
            default:
                log_info(entradasalida_logger, "Instruccion desconocida");
                enviar_codigo_operacion(fd_kernel, IO_OPERACION_INVALIDA); // Preguntarle a mati que le parece
        } 

    }
    eliminar_buffer(buffer);
}