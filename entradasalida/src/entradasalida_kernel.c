#include "../include/entradasalida_kernel.h"

// DUDA: cuando tengo que interactuar con memoria y cuando con kernel?
void atender_entradasalida_kernel(){
    t_codigo_operacion instruccion;
    t_buffer *buffer = crear_buffer(); 

    while (1) {
        recibir_paquete(fd_kernel, &instruccion, buffer);
        int pid_proceso;
        buffer_desempaquetar(buffer, &pid_proceso);

        /* Log obligatorio: Operación: "PID: <PID> - Operacion: <OPERACION_A_REALIZAR>" */
        log_info(entradasalida_logger_min_y_obl, "Operacion: PID: '%d' - Operacion: '%d'", pid_proceso, instruccion);
    
        // TODO - Debería de modelar la notificación a KERNEL de que ya termine de usar la interfaz
        switch (instruccion){
            case COP_IO_GEN_SLEEP:
                // IO_GEN_SLEEP Int1 10
                int cant_unidades_trabajo;
                buffer_desempaquetar(buffer, &cant_unidades_trabajo);

                interfaz_generica(cant_unidades_trabajo);

                enviar_codigo_operacion(fd_kernel, IO_FIN_OPERACION);
                break; // Para salir del switch, sino intenta desempaquetar denuevo
            case COP_IO_STDIN_READ:
                // IO_STDIN_READ Int2 EAX AX
                int cant_direcciones;
                buffer_desempaquetar(buffer, &cant_direcciones);

                t_list *lista_direcciones = list_create();
                
                for (int i = 0; i < cant_direcciones; i++)
                {
                    t_direccion *direccion = malloc(sizeof(t_direccion));
                    buffer_desempaquetar(buffer, &(direccion->direccion_fisica));
                    buffer_desempaquetar(buffer, &(direccion->bytes));
                    list_add(lista_direcciones, direccion);
                }              

                interfaz_stdin(lista_direcciones);

                list_destroy_and_destroy_elements(lista_direcciones, free);

                enviar_codigo_operacion(fd_kernel, IO_FIN_OPERACION);
                break;
            case COP_IO_STDOUT_WRITE:
                // IO_STDOUT_WRITE Int3 BX EAX
                int cant_direcciones;
                buffer_desempaquetar(buffer, &cant_direcciones);

                t_list *lista_direcciones = list_create();
                
                for (int i = 0; i < cant_direcciones; i++)
                {
                    t_direccion *direccion = malloc(sizeof(t_direccion));
                    buffer_desempaquetar(buffer, &(direccion->direccion_fisica));
                    buffer_desempaquetar(buffer, &(direccion->bytes));
                    list_add(lista_direcciones, direccion);
                }              

                interfaz_stdout(lista_direcciones, cant_direcciones);
                
                list_destroy_and_destroy_elements(lista_direcciones, free);

                enviar_codigo_operacion(fd_kernel, IO_FIN_OPERACION);
                break;
            case COP_IO_FS_CREATE:
                // IO_FS_CREATE Int4 notas.txt
                char* filename = buffer_desempaquetar_string(buffer);

                interfaz_fs_create(filename);

                free(filename);
                enviar_codigo_operacion(fd_kernel, IO_FIN_OPERACION);
                break;
            case COP_IO_FS_DELETE:
                break;
            case COP_IO_FS_TRUNCATE:
                break;
            case COP_IO_FS_WRITE:
                break;
            case COP_IO_FS_READ:
                break;
            default:
                log_info(entradasalida_logger, "Instruccion desconocida");
                enviar_codigo_operacion(fd_kernel, IO_OPERACION_INVALIDA); // Preguntarle a mati que le parece            
        } 

    }
    eliminar_buffer(buffer);
}