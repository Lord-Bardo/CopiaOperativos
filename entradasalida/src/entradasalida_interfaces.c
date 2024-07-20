#include "../include/entradasalida_interfaces.h"

// El valor de la cant_unidades_trabajo viene de kernel y se multiplica por el valor que tiene el config de tiempo_unidad_trabajo
void interfaz_generica(int cant_unidades_trabajo) {
    int tiempo_total = (cant_unidades_trabajo * TIEMPO_UNIDAD_TRABAJO) / 1000;
    //log_info(entradasalida_logger, "%d", tiempo_total);

    // Simular el tiempo de espera
    log_info(entradasalida_logger, "E/S: haciendo un sleep");
    sleep(tiempo_total);
    log_info(entradasalida_logger, "E/S: deje de hacer un sleep");

    /* Agregar log obligatorio: Operación: "PID: <PID> - Operacion: <OPERACION_A_REALIZAR>" */
}

// SOPORTE: lista de direcciones fisicas - tamaño total (preguntar)
void interfaz_stdin(t_list *lista_direcciones){
    char *texto;
	texto = readline("Ingrese el texto a escribir en MEMORIA: ");

    if(texto == NULL) {
        log_error(entradasalida_logger,"Error al leer el texto");
        exit(1);
    }

    int bytesTexto = strlen(texto);
    int bytesLeidos = 0;
    int indice = 0;

    while(bytesLeidos < bytesTexto){
        t_paquete* paquete = crear_paquete(SOLICITUD_ESCRITURA); 

        t_direccion* t_direccion = list_get(lista_direcciones, indice);
        int df_a_enviar  = t_direccion->direccion_fisica;
        int bytes_a_enviar = t_direccion->bytes;

        agregar_a_paquete(paquete, df_a_enviar, sizeof(int));
        agregar_a_paquete(paquete, bytes_a_enviar, sizeof(int));

        // Calculo el tamaño del texto a enviar
        int bytes_restantes = bytesTexto - bytesLeidos;
        if (bytes_restantes < bytes_a_enviar) {
            bytes_a_enviar = bytes_restantes;
        }

        // Considerando el caso donde bytesLeidos es 10 y bytesTexto es 16:
        // bytesTexto - bytesLeidos = 6 bytes restantes.
        // Si el próximo bytes_a_enviar es 8, se ajusta a 6 para no exceder los bytes restantes

        // Creo el texto a enviar
        char *textoCortado = malloc(bytes_a_enviar + 1);
        memmove(textoCortado, texto + bytesLeidos, bytes_a_enviar);
        textoCortado[bytes_a_enviar] = '\0';

        agregar_string_a_paquete(paquete, textoCortado);

        enviar_paquete(fd_memoria, paquete);

        eliminar_paquete(paquete);
        
        /* DEFINIR CUAL VA A SER EL COD OP*/ /* VER SI ESTÁ BIEN COLOCADO ACÁ */ /* VALIDACIONES DE ERRORES */
        recibir_codigo_operacion(fd_memoria, CONFIRMACION_ESCRITURA);

	    free(textoCortado);
        
        bytesLeidos += bytes_a_enviar;
        indice++;
    }
       // Hola cómo andas/0 --- 16 BYTES
                // 2 bytes - 11
                // 8 bytes - 31
                // 6 bytes - 8

                // Ho
                // la cómo 
                // andas/0

	free(texto); // Es necesario?
}

void interfaz_stdout(t_list* lista_direcciones, int cant_direcciones){
    char* texto;
    for (int i = 0; i < cant_direcciones; i++)
    {
        t_paquete* paquete = crear_paquete(SOLICITUD_LECTURA);
        
        t_direccion* t_direccion = list_get(lista_direcciones, i);
        int df_a_enviar  = t_direccion->direccion_fisica;
        int bytes_a_enviar = t_direccion->bytes;

        agregar_a_paquete(paquete, df_a_enviar, sizeof(int));
        agregar_a_paquete(paquete, bytes_a_enviar, sizeof(int));

        enviar_paquete(fd_memoria, paquete);
        eliminar_paquete(paquete);

        /* DEFINIR COMO ME VA A TRAER LOS DATOS QUE LE MANDO ASÍ LOS PUEDO GUARDAR EN EL TEXTO */
        /* LECTURA_RESPUESTA DEBERIA SER CONFIRMACIÓN_LECTURA */
        t_buffer *buffer = crear_buffer(); 
        recibir_paquete(fd_memoria,LECTURA_RESPUESTA, buffer);
        char* textoTemporal = buffer_desempaquetar_string(buffer);
        strcat(texto,textoTemporal);
    }


    /* HACER UN STRCAT PARA UNIR LAS CADENAS */
    if (texto != NULL) {
        log_info(entradasalida_logger, "TEXTO LEIDO: %s", texto); // Preguntar si puede ser así o tiene que ser un printf u otra cosa
        free(texto);
    } else {
        log_error(entradasalida_logger, "Error al recibir datos de memoria.\n");
    }
    
}

// void interfaz_stdout(char* registro_direccion, char* registro_tamanio) {
//     solicitar_datos_a_memoria(registro_direccion, atoi(registro_tamanio));

//     char* texto = recibir_datos_de_memoria(atoi(registro_tamanio));
    
//     if (texto != NULL) {
//         log_info(entradasalida_logger, "TEXTO LEIDO: %s", texto);
//         /* Agregar log obligatorio: Operación: "PID: <PID> - Operacion: <OPERACION_A_REALIZAR>" */
//         free(texto);
//     } else {
//         log_error(entradasalida_logger, "Error al recibir datos de memoria.\n");
//     }
// }

/* VER DE USAR ESTA FUNCION ASÍ LA UNIFICO PARA STDIN Y ST */
// void solicitar_datos_a_memoria(char* direccion_fisica, int tamanio){
//     t_paquete* paquete = crear_paquete(SOLICITUD_LECTURA);

//     agregar_a_paquete(paquete, direccion_fisica, sizeof(char*));
//     agregar_a_paquete(paquete, tamanio, sizeof(int));

//     enviar_paquete(fd_memoria, paquete);

//     eliminar_paquete(paquete);
// }

// char* recibir_datos_de_memoria(int tamanio) {
//     t_buffer* buffer = crear_buffer();
//     t_codigo_operacion cod_operacion;
//     recibir_paquete(fd_memoria, &cod_operacion, buffer);

//     /* Poner logica para leer cod operacion */
//     char* texto = malloc(tamanio + 1);
//     if (texto != NULL) {
//         buffer_desempaquetar(buffer, texto);
//         /* Poner desempaquetar string */
//         texto[tamanio] = '\0';
//     }

//     eliminar_buffer(buffer);

//     return texto;
// }

void interfaz_fs_create(char* filename) {
    // Busca un bloque libre en el bitmap
    int block_index = -1;
    for (int i = 0; i < BLOCK_COUNT; i++) {
        if (!bitarray_test_bit(bitarray, i)) {
            block_index = i;
            bitarray_set_bit(bitarray, i);
            break;
        }
    }

    if (block_index == -1) {
        printf("No hay bloques libres disponibles.\n");
        /* Ver si tendria que fijarme si hay lugar pero no puedo guardarlo por fragmentacion externa (en ese caso deberia hacer compactacion) */
        return;
    }

    // Creo el archivo de metadata
    FILE* metadata_file = fopen(filename, "w");
    if (metadata_file == NULL) {
        perror("Error al crear el archivo de metadata");
        return;
    }
    fprintf(metadata_file, "BLOQUE_INICIAL=%d\nTAMANIO_ARCHIVO=0\n", block_index);
    fclose(metadata_file);
}

void interfaz_fs_delete(char* filename) {
    // Abro el archivo de metadata
    FILE* metadata_file = fopen(filename, "r");
    if (metadata_file == NULL) {
        perror("Error al abrir el archivo de metadata");
        return;
    }

    // Lee el bloque inicial del archivo
    int block_index;
    fscanf(metadata_file, "BLOQUE_INICIAL=%d\n", &block_index);
    fclose(metadata_file);

    // Libera el bloque en el bitmap
    bitarray_clean_bit(bitarray, block_index);

    // Elimina el archivo de metadata
    if (remove(filename) != 0) {
        perror("Error al eliminar el archivo de metadata");
    }
}

/* void IO_FS_TRUNCATE(char* filename, int new_size) {
    // Abre el archivo de metadata
    FILE* metadata_file = fopen(filename, "r+");
    if (metadata_file == NULL) {
        perror("Error al abrir el archivo de metadata");
        return;
    }

    // Lee el bloque inicial y el tamaño actual del archivo
    int block_index, current_size;
    fscanf(metadata_file, "BLOQUE_INICIAL=%d\nTAMANIO_ARCHIVO=%d\n", &block_index, &current_size);

    // Actualiza el tamaño del archivo en el archivo de metadata
    fseek(metadata_file, 0, SEEK_SET);
    fprintf(metadata_file, "BLOQUE_INICIAL=%d\nTAMANIO_ARCHIVO=%d\n", block_index, new_size);
    fclose(metadata_file);

    // Maneja la lógica de asignación y liberación de bloques según el nuevo tamaño
    // Aquí podrías necesitar agregar lógica para asignar o liberar bloques adicionales
} */

/* void IO_FS_WRITE(char* filename, int offset, char* data, int size) {
    // Abre el archivo de metadata
    FILE* metadata_file = fopen(filename, "r");
    if (metadata_file == NULL) {
        perror("Error al abrir el archivo de metadata");
        return;
    }

    // Lee el bloque inicial y el tamaño actual del archivo
    int block_index, current_size;
    fscanf(metadata_file, "BLOQUE_INICIAL=%d\nTAMANIO_ARCHIVO=%d\n", &block_index, &current_size);
    fclose(metadata_file);

    // Abre el archivo de bloques
    FILE* bloques_file = fopen(BLOQUES_PATH, "rb+");
    if (bloques_file == NULL) {
        perror("Error al abrir bloques.dat");
        return;
    }

    // Escribe los datos en el archivo de bloques en la posición adecuada
    fseek(bloques_file, block_index * BLOCK_SIZE + offset, SEEK_SET);
    fwrite(data, 1, size, bloques_file);
    fclose(bloques_file);

    // Actualiza el tamaño del archivo en el archivo de metadata si es necesario
    if (offset + size > current_size) {
        metadata_file = fopen(filename, "r+");
        if (metadata_file == NULL) {
            perror("Error al abrir el archivo de metadata");
            return;
        }
        fseek(metadata_file, 0, SEEK_SET);
        fprintf(metadata_file, "BLOQUE_INICIAL=%d\nTAMANIO_ARCHIVO=%d\n", block_index, offset + size);
        fclose(metadata_file);
    }
}
 */

/* void IO_FS_READ(char* filename, int offset, char* buffer, int size) {
    // Abre el archivo de metadata
    FILE* metadata_file = fopen(filename, "r");
    if (metadata_file == NULL) {
        perror("Error al abrir el archivo de metadata");
        return;
    }

    // Lee el bloque inicial y el tamaño actual del archivo
    int block_index, current_size;
    fscanf(metadata_file, "BLOQUE_INICIAL=%d\nTAMANIO_ARCHIVO=%d\n", &block_index, &current_size);
    fclose(metadata_file);

    // Asegúrate de no leer más allá del final del archivo
    if (offset + size > current_size) {
        size = current_size - offset;
    }

    // Abre el archivo de bloques
    FILE* bloques_file = fopen(BLOQUES_PATH, "rb");
    if (bloques_file == NULL) {
        perror("Error al abrir bloques.dat");
        return;
    }

    // Lee los datos del archivo de bloques en la posición adecuada
    fseek(bloques_file, block_index * BLOCK_SIZE + offset, SEEK_SET);
    fread(buffer, 1, size, bloques_file);
    fclose(bloques_file);
}
 */


/* int asignar_bloque() {
    for (int i = 0; i < BLOCK_COUNT; i++) {
        if (!bitarray_test_bit(bitarray, i)) {
            bitarray_set_bit(bitarray, i);
            return i;
        }
    }
    return -1; // No hay bloques disponibles
}

void liberar_bloque(int block_index) {
    if (block_index >= 0 && block_index < BLOCK_COUNT) {
        bitarray_clean_bit(bitarray, block_index);
    }
} */