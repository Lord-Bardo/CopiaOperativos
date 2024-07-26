#include "../include/entradasalida_interfaces.h"

// El valor de la cant_unidades_trabajo viene de kernel y se multiplica por el valor que tiene el config de tiempo_unidad_trabajo
void interfaz_generica(int cant_unidades_trabajo) {
    int tiempo_total = (cant_unidades_trabajo * TIEMPO_UNIDAD_TRABAJO) * 1000;
    log_info(entradasalida_logger, "%d", tiempo_total);

    // Simular el tiempo de espera
    log_info(entradasalida_logger, "E/S: haciendo un sleep");
    usleep(tiempo_total);
    log_info(entradasalida_logger, "E/S: deje de hacer un sleep");

}

// SOPORTE: lista de direcciones fisicas - tamaño total (preguntar)
void interfaz_stdin(t_list *lista_direcciones, int pid){
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
        agregar_int_a_paquete(paquete, pid);

        t_direccion* t_direccion = list_get(lista_direcciones, indice);
        int df_a_enviar  = t_direccion->direccion_fisica;
        int bytes_a_enviar = t_direccion->bytes;

        agregar_int_a_paquete(paquete, df_a_enviar);
        agregar_int_a_paquete(paquete, bytes_a_enviar);

        // Calculo el tamaño del texto a enviar
        int bytes_restantes = bytesTexto - bytesLeidos;
        log_info(entradasalida_logger, "FALTAN ESCRIBIR %d BYTES!", bytes_restantes);
        if (bytes_restantes < bytes_a_enviar) {
            bytes_a_enviar = bytes_restantes;
        }
        log_info(entradasalida_logger, "VOY A ENVIAR %d BYTES!", bytes_a_enviar);

        // Considerando el caso donde bytesLeidos es 10 y bytesTexto es 16:
        // bytesTexto - bytesLeidos = 6 bytes restantes.
        // Si el próximo bytes_a_enviar es 8, se ajusta a 6 para no exceder los bytes restantes
        /* REVISAR ESTO !!!!!!!!!!!!!!!!!!!! */

        // Creo el texto a enviar
        void *textoCortado = malloc(bytes_a_enviar);
        memmove(textoCortado, texto + bytesLeidos, bytes_a_enviar);
        //textoCortado[bytes_a_enviar] = '\0';

        char *cadena_a_enviar = malloc(bytes_a_enviar+1);
        memcpy(cadena_a_enviar, textoCortado, bytes_a_enviar);
        cadena_a_enviar[bytes_a_enviar] = '\0';
        log_info(entradasalida_logger, "VOY A ENVIAR LA CADENA: %s", cadena_a_enviar);
        free(cadena_a_enviar);
        
        agregar_a_paquete(paquete, textoCortado, bytes_a_enviar);
        //agregar_string_a_paquete(paquete, textoCortado);

        enviar_paquete(fd_memoria, paquete);

        eliminar_paquete(paquete);

        t_codigo_operacion op_code;
        recibir_codigo_operacion(fd_memoria, &op_code);
        if(op_code!= CONFIRMACION_ESCRITURA){
            log_error(entradasalida_logger,"NO SE RECIBE CONFIRMACION ESCRITURA");
            break;
        }
        
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

	free(texto);
}

void interfaz_stdout(t_list* lista_direcciones, int cant_direcciones, int pid){
    char* texto = string_new();
    for (int i = 0; i < cant_direcciones; i++)
    {
        t_paquete* paquete = crear_paquete(SOLICITUD_LECTURA);
        agregar_int_a_paquete(paquete, pid);
        
        t_direccion* t_direccion = list_get(lista_direcciones, i);
        int df_a_enviar  = t_direccion->direccion_fisica;
        int bytes_a_enviar = t_direccion->bytes;

        agregar_int_a_paquete(paquete, df_a_enviar);
        agregar_int_a_paquete(paquete, bytes_a_enviar);

        enviar_paquete(fd_memoria, paquete);
        eliminar_paquete(paquete);

        /* DEFINIR COMO ME VA A TRAER LOS DATOS QUE LE MANDO ASÍ LOS PUEDO GUARDAR EN EL TEXTO */
        /* LECTURA_RESPUESTA DEBERIA SER CONFIRMACIÓN_LECTURA */
        t_codigo_operacion op_code;
        t_buffer *buffer = crear_buffer(); 
        recibir_paquete(fd_memoria, &op_code, buffer);
        if( op_code != DATO){
            log_error(entradasalida_logger,"NO SE RECIBE DATO");
            break;
        }
        char* textoTemporal = buffer_desempaquetar_string(buffer);
        string_append(&texto, textoTemporal);
        free(textoTemporal);
    }

    if (texto != NULL) {
        log_info(entradasalida_logger, "TEXTO LEIDO: %s", texto); // Preguntar si puede ser así o tiene que ser un printf u otra cosa
        free(texto);
    } else {
        log_error(entradasalida_logger, "Error al recibir datos de memoria.\n");
    }
    
}

void interfaz_fs_create(char* filename, int pid) {
    // Creo el archivo de metadata
    char *path_archivo_metadata = string_duplicate(PATH_BASE_DIALFS);
    string_append(&path_archivo_metadata, "/");
    string_append(&path_archivo_metadata, filename);

    FILE* metadata_file = fopen(path_archivo_metadata, "r");
    if(metadata_file == NULL){
        FILE* metadata_file = fopen(path_archivo_metadata, "w");
        if (metadata_file == NULL) {
            log_error(entradasalida_logger, "Error al crear el archivo de metadata");
            return;
        }

        t_config* metadata_file_config = config_create(path_archivo_metadata);
        if (metadata_file_config == NULL) {
            log_error(entradasalida_logger, "Error al crear el config de metadata");
            fclose(metadata_file);
            return;
        }
        
        // Busca un bloque libre en el bitmap
        int block_index = -1;
        for (int i = 0; i < BLOCK_COUNT; i++) {
            if (!bitarray_test_bit(bitmap, i)) {
                block_index = i;
                bitarray_set_bit(bitmap, i);
                break;
            }
        }

        if (block_index == -1) {
            printf("No hay bloques libres disponibles.\n");
            return;
        }

        msync(bitmap_data, bitmap->size, MS_SYNC);

        // fprintf(metadata_file_config, "BLOQUE_INICIAL=%d \n TAMANIO_ARCHIVO=0 \n", block_index);
        config_set_value(metadata_file_config, "BLOQUE_INICIAL", string_itoa(block_index));
        config_set_value(metadata_file_config, "TAMANIO_ARCHIVO", "0");
        
        config_save(metadata_file_config); // Ver de manejar error

        dictionary_put(metadata_dictionary_files, filename, metadata_file_config);
        fclose(metadata_file);
        log_info(entradasalida_logger_min_y_obl, "DialFS - Crear Archivo: PID: %d - Crear Archivo: %s", pid, filename);
    }
    else {
        log_info(entradasalida_logger, "El archivo ya existe, no lo creo");
        fclose(metadata_file);
        return;
    }
}

void interfaz_fs_delete(char* filename, int pid) {
 
    t_config* metadata_file_config = dictionary_remove(metadata_dictionary_files, filename);

    if (metadata_file_config == NULL){
        log_info(entradasalida_logger, "El archivo no existe, no lo borro");
        return;
    }

    // Lee el bloque inicial del archivo
    int block_index = config_get_int_value(metadata_file_config, "BLOQUE_INICIAL");

    // Libera el bloque en el bitarray
    bitarray_clean_bit(bitmap, block_index);

    // Sincronizar los cambios en el bitarray con el archivo bitmap
    msync(bitmap_data, bitmap->size, MS_SYNC);
    
    char *path_archivo_metadata = string_duplicate(metadata_file_config->path);

    // Elimina el archivo de metadata
    if (remove(path_archivo_metadata) != 0) {
        log_error(entradasalida_logger, "Error al eliminar el archivo de metadata");
        exit(1);
    }

    log_info(entradasalida_logger_min_y_obl, "DialFS - Eliminar Archivo: PID: %d - Eliminar Archivo: %s", pid, filename);

    config_destroy(metadata_file_config);    
}

int cantidadBloques(int tamanio_bytes){
    return (int) ceil((double)tamanio_bytes / (double)BLOCK_SIZE);
}

// 3 // 1 // 2
void reducirTamanioArchivo(int bloque_inicial, int cant_bloques_a_reducir, int cant_bloques_tam_archivo){
    int i = 0;
    while (i < cant_bloques_a_reducir)
    {
        // 5 
        int bloque_a_limpiar = bloque_inicial + cant_bloques_tam_archivo - i;
        bitarray_clean_bit(bitmap, bloque_a_limpiar);
        i++;
        log_info(entradasalida_logger, "Reduciendo el tamaño del archivo, bloque limpiado: %d", bloque_a_limpiar);
    }
    msync(bitmap_data, bitmap->size, MS_SYNC);

}

bool hayEspacioInmediato(int bloque_inicial, int cant_bloques_a_aumentar){
    while(bloque_inicial < bloque_inicial + cant_bloques_a_aumentar){
        bool bit = bitarray_test_bit(bitmap, bloque_inicial); // Si está vacio - false
        if( bit == true ){
            log_info(entradasalida_logger, "El bloque %d estaba ocupado, no hay espacio inmediato", bloque_inicial);
            return false;  
        }
        bloque_inicial++;
    }
    log_info(entradasalida_logger, "Hay espacio inmediato - Se asignan los %d bloques sin compactar", cant_bloques_a_aumentar);
    return true;
}

void ocuparBloques(int ocupar_desde, int cant_bloques_a_aumentar){
    int bloque_hasta = ocupar_desde + cant_bloques_a_aumentar;

    for (int i = ocupar_desde; i < bloque_hasta; i++)
    {
        bitarray_set_bit(bitmap,i);
        log_info(entradasalida_logger, "Asignando bloques - Bloque asignado: %d", i);
    }
    msync(bitmap_data, bitmap->size, MS_SYNC);
}

bool hayEspacioContiguo(int* bloque_inicial, int cant_bloques_totales){
    int i = 0;
    int cont_bloques_contiguos = 0;

    while(cont_bloques_contiguos < cant_bloques_totales && i < BLOCK_COUNT){
        bool bit = bitarray_test_bit(bitmap, i);
        if( bit == false ){
            cont_bloques_contiguos++;
            if (cont_bloques_contiguos == cant_bloques_totales) {
                *bloque_inicial = i;
                log_info(entradasalida_logger, "Se encontro espacio contiguo en el bloque %d", i);
                return true;
            }
        }
        else {
            cont_bloques_contiguos = 0;
        }
        i++;
    }
    return false;
}

void liberarBloques(int bloque_inicial, int cant_bloques_tam_archivo){
    for (int i = bloque_inicial; i < bloque_inicial + cant_bloques_tam_archivo ; i++)
    {
        bitarray_clean_bit(bitmap,i);
    }
    log_info(entradasalida_logger, "Libere los bloques desde %d hasta %d", bloque_inicial, bloque_inicial + cant_bloques_tam_archivo);
    msync(bitmap_data, bitmap->size, MS_SYNC);
}

void moverContenidoBloques(int bloque_inicial, int bloque_inicial_aux, int tamanio_archivo, int cant_bloques_tam_archivo){
    void* contenido = malloc(tamanio_archivo);
    leerArchivo(contenido, tamanio_archivo, bloque_inicial);
}

void leerArchivo(void* contenido, int tamanio_archivo, int bloque_inicial){
    int cant_bloques_completos = tamanio_archivo / BLOCK_SIZE; // 150 / 64 = 2 (Redondea el int)
    int bytes_restantes = tamanio_archivo - (cant_bloques_completos * BLOCK_SIZE); // 150 - (2 * 64) = 22 bytes
    int bytes_desde = bloque_inicial * BLOCK_SIZE;
    for (int i = 0; i < cant_bloques_completos; i++)
    {
        leerBloqueCompleto(contenido, bytes_desde);
        bytes_desde += BLOCK_SIZE;
    }
    leerBloque(contenido, bytes_desde, bytes_restantes);
}

void leerBloqueCompleto(void* contenido, int bytes_desde){
    leerBloque(contenido, bytes_desde, BLOCK_SIZE);
}

void leerBloque(void* contenido, int byteDesde, int byteHasta){

}

void aumentarTamanioArchivo(t_config* metadata_file_config, int bloque_inicial, int tamanio_archivo, int cant_bloques_tam_archivo, int cant_bloques_a_aumentar){
    int bloque_inicial_aux;

    // Liberar mi propio bloque
    bitarray_clean_bit(bitmap,bloque_inicial);

    // Hay espacio libre a la derecha del bloque en el que está mi archivo
    // Pongo bloque_inicial + 1 debido a que quiero chequear desde los bloques despues del mio porque el mio esta en 0
    if(hayEspacioInmediato(bloque_inicial + 1, cant_bloques_a_aumentar)){
        bitarray_set_bit(bitmap,bloque_inicial);
        int ocupar_desde = bloque_inicial + cant_bloques_tam_archivo;
        ocuparBloques(ocupar_desde, cant_bloques_a_aumentar);
    }
    // Hay espacio libre contiguo en algun lugar de todo mi archivo de bloques
    else if (hayEspacioContiguo(&bloque_inicial_aux, cant_bloques_tam_archivo + cant_bloques_a_aumentar)){
        config_set_value(metadata_file_config, "BLOQUE_INICIAL", string_itoa(bloque_inicial_aux));
        int ocupar_desde = bloque_inicial_aux + cant_bloques_tam_archivo;
        ocuparBloques(ocupar_desde, cant_bloques_a_aumentar);

        // Mover la información de los bloques del archivo a los bloques a ocupar
        moverContenidoBloques(bloque_inicial, bloque_inicial_aux, tamanio_archivo, cant_bloques_tam_archivo);
        // Despues de mover los bloques habría que liberar los bloques anteriores que ocupaba el archivo
        liberarBloques(bloque_inicial, cant_bloques_tam_archivo);
    }
    // Hay espacio suficiente en todo mi archivo de bloques
    else if (hayEspacioSuficiente()) {

        // Compactar
    }
    else {
        bitarray_set_bit(bitmap,bloque_inicial);
        log_info(entradasalida_logger, "No hay espacio disponible para aumentar el tamanio del archivo a %d bloques", cant_bloques_tam_archivo + cant_bloques_a_aumentar);
    }
}

void hayEspacioSuficiente(){
    
}

void interfaz_fs_truncate(char* filename, int nuevo_tamanio, int pid) {
    log_info(entradasalida_logger_min_y_obl, "DialFS - Truncar Archivo: PID: %d - Truncar Archivo: %s - Tamaño: %d", pid, filename, nuevo_tamanio);
    
    t_config* metadata_file_config = dictionary_get(metadata_dictionary_files,filename);
    int bloque_inicial = config_get_int_value(metadata_file_config, "BLOQUE_INICIAL");
    int tamanio_archivo = config_get_int_value(metadata_file_config, "TAMANIO_ARCHIVO");

    config_set_value(metadata_file_config, "TAMANIO_ARCHIVO", string_itoa(nuevo_tamanio));
    
    // tamanio_archivo = 100 // nuevo_tamanio = 60 // // bloque_inicial = 3 // 
    int cant_bloques_tam_archivo = cantidadBloques(tamanio_archivo); // 2
    int cant_bloques_nuevo_tam = cantidadBloques(nuevo_tamanio); // 1

    if (cant_bloques_nuevo_tam == cant_bloques_tam_archivo){
        return;
    }
    else if (cant_bloques_tam_archivo < cant_bloques_nuevo_tam){
        // Aumentar tamaño archivo
        int cant_bloques_a_aumentar = cant_bloques_nuevo_tam - cant_bloques_tam_archivo;
        aumentarTamanioArchivo(metadata_file_config, bloque_inicial, tamanio_archivo, cant_bloques_tam_archivo, cant_bloques_a_aumentar);
    }
    else {
        // Reducir tamaño de archivo
        int cant_bloques_a_reducir = cant_bloques_tam_archivo - cant_bloques_nuevo_tam; // 1
        reducirTamanioArchivo(bloque_inicial, cant_bloques_a_reducir, cant_bloques_tam_archivo); // 3 // 1 // 2 
    }

    // int block_index, current_size;
    // // fscanf(metadata_file_config, "BLOQUE_INICIAL=%d\n TAMANIO_ARCHIVO=%d\n", &block_index, &current_size);

    // int current_blocks = (current_size + BLOCK_SIZE - 1) / BLOCK_SIZE; // Número actual de bloques
    // int new_blocks = (nuevo_tamaño + BLOCK_SIZE - 1) / BLOCK_SIZE; // Nuevos bloques necesarios

    // if (new_blocks == current_blocks) {
    //     // No se requiere cambio de tamaño
    //     fclose(metadata_file_config);
    //     return;
    // } else if (new_blocks < current_blocks) {
    //     // Reducción del tamaño del archivo
    //     for (int i = current_blocks - 1; i >= new_blocks; i--) {
    //         bitarray_clean_bit(bitmap, block_index + i);
    //     }

    //     // Sincronizar cambios en el bitarray con el archivo bitmap
    //     msync(bitmap_data, bitarray->size, MS_SYNC);
    // } else {
    //     // Ampliación del tamaño del archivo
    //     int additional_blocks_needed = new_blocks - current_blocks;
    //     int can_expand = 1;

    //     // Verificar si hay bloques contiguos disponibles
    //     for (int i = 0; i < additional_blocks_needed; i++) {
    //         if (bitarray_test_bit(bitmap, block_index + current_blocks + i)) {
    //             can_expand = 0;
    //             break;
    //         }
    //     }

    //     if (!can_expand) {
    //         // Realizar compactación
    //         compactar_fs();
    //         // Recalcular el bloque inicial ya que pudo haber cambiado después de la compactación
    //         block_index = recalcular_bloque_inicial(filename);
    //     }

    //     // Asignar nuevos bloques después de la compactación o si ya estaban contiguos
    //     for (int i = 0; i < additional_blocks_needed; i++) {
    //         bitarray_set_bit(bitmap, block_index + current_blocks + i);
    //     }

    //     // Sincronizar cambios en el bitarray con el archivo bitmap
    //     msync(bitmap_data, bitmap->size, MS_SYNC);
    // }

    // // Actualizar el tamaño del archivo en el archivo de metadata
    // fseek(metadata_file_config, 0, SEEK_SET);
    // fprintf(metadata_file_config, "BLOQUE_INICIAL=%d\nTAMANIO_ARCHIVO=%d\n", block_index, bitmap);
    // fclose(metadata_file_config);
}

/* void IO_FS_WRITE(char* filename, int offset, char* data, int size) {
    // Abre el archivo de metadata
    FILE* metadata_file_config = fopen(filename, "r");
    if (metadata_file_config == NULL) {
        perror("Error al abrir el archivo de metadata");
        return;
    }

    // Lee el bloque inicial y el tamaño actual del archivo
    int block_index, current_size;
    fscanf(metadata_file_config, "BLOQUE_INICIAL=%d\nTAMANIO_ARCHIVO=%d\n", &block_index, &current_size);
    fclose(metadata_file_config);

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
        metadata_file_config = fopen(filename, "r+");
        if (metadata_file_config == NULL) {
            perror("Error al abrir el archivo de metadata");
            return;
        }
        fseek(metadata_file_config, 0, SEEK_SET);
        fprintf(metadata_file_config, "BLOQUE_INICIAL=%d\nTAMANIO_ARCHIVO=%d\n", block_index, offset + size);
        fclose(metadata_file_config);
    }
}
 */

/* void IO_FS_READ(char* filename, int offset, char* buffer, int size) {
    // Abre el archivo de metadata
    FILE* metadata_file_config = fopen(filename, "r");
    if (metadata_file_config == NULL) {
        perror("Error al abrir el archivo de metadata");
        return;
    }

    // Lee el bloque inicial y el tamaño actual del archivo
    int block_index, current_size;
    fscanf(metadata_file_config, "BLOQUE_INICIAL=%d\nTAMANIO_ARCHIVO=%d\n", &block_index, &current_size);
    fclose(metadata_file_config);

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