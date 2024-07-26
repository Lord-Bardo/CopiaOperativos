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
    int bloque_inicial = config_get_int_value(metadata_file_config, "BLOQUE_INICIAL");
    int tamanio_archivo = config_get_int_value(metadata_file_config,"TAMANIO_ARCHIVO");
    
    // Libera los bloques del archivo a borrar y sincroniza
    liberarBloques(bloque_inicial,cantidadBloques(tamanio_archivo));

    
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
    if(tamanio_bytes ==0){
        return 1;
    }
    return (int) ceil((double)tamanio_bytes / (double)BLOCK_SIZE);
}

void reducirTamanioArchivo(int bloque_inicial, int cant_bloques_a_reducir, int cant_bloques_tam_archivo){
    int i = 1;
    while (i <= cant_bloques_a_reducir)
    {
        int bloque_a_limpiar = bloque_inicial + cant_bloques_tam_archivo - i;
        bitarray_clean_bit(bitmap, bloque_a_limpiar);
        i++;
        log_info(entradasalida_logger, "Reduciendo el tamaño del archivo, bloque limpiado: %d", bloque_a_limpiar);
    }
    msync(bitmap_data, bitmap->size, MS_SYNC);

}


bool hayEspacioInmediato(int bloque_inicial, int cant_bloques_a_aumentar){
    int cont_bloques_contiguos = 0;
    int bloque_actual = bloque_inicial;
    while(cont_bloques_contiguos<cant_bloques_a_aumentar && bloque_actual < BLOCK_COUNT){
        bool bit = bitarray_test_bit(bitmap, bloque_actual); // Si está vacio - false
        if( bit == true ){
            log_info(entradasalida_logger, "El bloque %d estaba ocupado, no hay espacio inmediato", bloque_actual);
            return false;  
        }
        
        cont_bloques_contiguos++;
        bloque_actual++;

    }
    if(bloque_actual ==BLOCK_COUNT && cont_bloques_contiguos != cant_bloques_a_aumentar){
            log_info(entradasalida_logger, "No hay mas bloques para asignar, no hay espacio inmediato");
            return false;
        }
    log_info(entradasalida_logger, "Hay espacio inmediato - Se asignan los %d bloques sin compactar", cant_bloques_a_aumentar);
    return true;

}

void ocuparBloques(int ocupar_desde, int cant_bloques_a_aumentar){
    int bloque_hasta = ocupar_desde + cant_bloques_a_aumentar - 1;

    for (int i = ocupar_desde; i <= bloque_hasta; i++)
    {
        bitarray_set_bit(bitmap,i);
        log_info(entradasalida_logger, "Asignando bloques - Bloque asignado: %d", i);
    }
    msync(bitmap_data, bitmap->size, MS_SYNC);
}

bool hayEspacioContiguo(int* bloque_inicial, int cant_bloques_totales){
    int bloque_actual = 0;
    int cont_bloques_contiguos = 0;
    //cant bloques totales 4 // i=5  //cont bloques 4
    while(bloque_actual < BLOCK_COUNT){
        bool bit = bitarray_test_bit(bitmap, bloque_actual); //
        if( bit == false ){
            cont_bloques_contiguos++;
            if (cont_bloques_contiguos == cant_bloques_totales) {
                *bloque_inicial = bloque_actual - cont_bloques_contiguos +1;
                log_info(entradasalida_logger, "Se encontro espacio contiguo en el bloque %d", *bloque_inicial);
                return true;
            }
        }
        else {
            cont_bloques_contiguos = 0;
        }
        bloque_actual++;
    }
    return false;
}

void liberarBloques(int bloque_inicial, int cant_bloques_tam_archivo){
    for (int i = bloque_inicial; i < bloque_inicial + cant_bloques_tam_archivo ; i++)
    {
        bitarray_clean_bit(bitmap,i);
    }
    log_info(entradasalida_logger, "Libere los bloques desde %d hasta %d", bloque_inicial, bloque_inicial + cant_bloques_tam_archivo-1);
    msync(bitmap_data, bitmap->size, MS_SYNC);
}

void moverContenidoBloques(int bloque_inicial, int bloque_inicial_nuevo, int tamanio_archivo){
    void* contenido = malloc(tamanio_archivo);
    if( contenido == NULL ){
        log_error(entradasalida_logger, "Error al asignar memoria para el CONTENIDO!");
    }
    FILE* archivo_bloques = abrirArchivoBloques();
    leerArchivo(archivo_bloques, contenido, tamanio_archivo, bloque_inicial);
    escribirArchivo(archivo_bloques, contenido, tamanio_archivo, bloque_inicial_nuevo);
    free(contenido);
    fclose(archivo_bloques);
}

void leerArchivo(FILE* archivo_bloques, void* contenido, int tamanio_archivo, int bloque_inicial){
    fseek(archivo_bloques, bloque_inicial, SEEK_SET);
    size_t bytes_leidos = fread(contenido, 1, tamanio_archivo, archivo_bloques);
    if (bytes_leidos != tamanio_archivo) {
        log_error(entradasalida_logger, "Hubo un error al leer los bytes del archivo de bloques");
        exit(1);
    }
}

// void leerArchivo(FILE* archivo_bloques, void* contenido, int tamanio_archivo, int bloque_inicial){
//     int cant_bloques_completos = tamanio_archivo / BLOCK_SIZE; // 150 / 64 = 2 (Redondea el int)
//     int bytes_restantes = tamanio_archivo - (cant_bloques_completos * BLOCK_SIZE); // 150 - (2 * 64) = 22 bytes
//     int bytes_desde = bloque_inicial * BLOCK_SIZE;
//     for (int i = 0; i < cant_bloques_completos; i++)
//     {
//         leerBloqueCompleto(archivo_bloques, contenido, bytes_desde);
//         bytes_desde += BLOCK_SIZE;
//     }
//     leerBloque(archivo_bloques, contenido, bytes_desde, bytes_restantes);
// }

void leerBloqueCompleto(FILE* archivo_bloques, void* contenido, int bytes_desde){
    leerBloque(archivo_bloques,contenido, bytes_desde, BLOCK_SIZE);
}

void leerBloque(FILE* archivo_bloques, void* contenido, int bytes_desde, int cantidad_bytes){
    fseek(archivo_bloques, bytes_desde, SEEK_SET);
    int bytes_leidos = fread(contenido, 1, cantidad_bytes,archivo_bloques);
    log_info(entradasalida_logger, "bytes_desde: %d", bytes_desde);
    log_info(entradasalida_logger, "bytes_hasta: %d", cantidad_bytes);
    log_info(entradasalida_logger, "bytes_leidos: %d", bytes_leidos);
    if (bytes_leidos != cantidad_bytes) {
        log_error(entradasalida_logger, "Hubo un error al leer los bytes del archivo de bloques en leer bloque");
        exit(1);
    }
}

void escribirArchivo(FILE* archivo_bloques, void* contenido, int tamanio_archivo, int bloque_inicial){
    fseek(archivo_bloques, bloque_inicial, SEEK_SET);
    size_t bytes_leidos = fwrite(contenido, 1, tamanio_archivo, archivo_bloques);
    if (bytes_leidos != tamanio_archivo) {
        log_error(entradasalida_logger, "Hubo un error al escribir los bytes en el archivo de bloques");
        exit(1);
    }
}

FILE* abrirArchivoBloques(){
    char *path_archivo_bloques = string_duplicate(PATH_BASE_DIALFS);
    string_append(&path_archivo_bloques, "/bloques.dat");

    FILE *bloques_file = fopen(path_archivo_bloques, "r+");
    if( bloques_file == NULL ){
        fclose(bloques_file);
        log_error(entradasalida_logger, "Error al abrir el archivo bloques.dat");
        exit(1);
    }
    return bloques_file;
}

bool hayEspacioSuficiente(int cant_bloques_totales){
    int i = 0;
    int cont_bloques = 0;

    while(cont_bloques < cant_bloques_totales && i < BLOCK_COUNT){
        bool bit = bitarray_test_bit(bitmap, i);
        if( bit == false ){
            cont_bloques++;
            if (cont_bloques == cant_bloques_totales) {
                log_info(entradasalida_logger, "Hay espacio suficiente para el archivo!");
                return true;
            }
        }
        i++;
    }
    return false;
}

void compactar(int *ultimo_bloque_ocupado, int pid){
    log_info(entradasalida_logger_min_y_obl, "DialFS - Inicio Compactación: PID: %d - Inicio Compactación.", pid);
    
    usleep(RETRASO_COMPACTACION * 1000);//es mas tiempo

    FILE* archivo_bloques = abrirArchivoBloques();

    void *contenido_bloques = NULL;
    int tamanio_contenido_bloques = 0;
    for(int i = 0; i < BLOCK_COUNT; i++){
        bool bit = bitarray_test_bit(bitmap, i);
        if( bit == true ){
            tamanio_contenido_bloques += BLOCK_SIZE;
            log_info(entradasalida_logger, "Tamanio_contenido_bloques: %d", tamanio_contenido_bloques);
            contenido_bloques = realloc(contenido_bloques, tamanio_contenido_bloques);
            if (contenido_bloques == NULL){
                log_error(entradasalida_logger, "Falló el realloc, es NULL");
                exit(1);
            }
            leerBloqueCompleto(archivo_bloques, contenido_bloques, i * BLOCK_SIZE);
            bitarray_clean_bit(bitmap, i);
        }
    }
    msync(bitmap_data, bitmap->size, MS_SYNC);

    *ultimo_bloque_ocupado = tamanio_contenido_bloques / BLOCK_SIZE;

    escribirArchivo(archivo_bloques, contenido_bloques, tamanio_contenido_bloques, 0);
    ocuparBloques(0, *ultimo_bloque_ocupado);

    free(contenido_bloques);
    fclose(archivo_bloques);

    log_info(entradasalida_logger_min_y_obl, "DialFS - Fin Compactación: PID: %d - Fin Compactación.", pid);
}

void guardarContenidoArchivo(void *contenido_archivo, int bloque_inicial, int tamanio_archivo){
    FILE* archivo_bloques = abrirArchivoBloques();
    leerArchivo(archivo_bloques, contenido_archivo, tamanio_archivo, bloque_inicial);
    fclose(archivo_bloques);
}

bool aumentarTamanioArchivo(t_config* metadata_file_config, int bloque_inicial, int tamanio_archivo, int cant_bloques_tam_archivo, int cant_bloques_a_aumentar, int pid){
    // tamanio_archivo = 100 // bloque inicial 1 // 100 //cant bloques nuevo tamanio 1 // cantidad bloques aumentar 6
    int bloque_inicial_nuevo;
    int bloque_final_actual = bloque_inicial + cant_bloques_tam_archivo - 1;
    log_info(entradasalida_logger,"Valor bloque final actual: %d  |  bloque inicial: %d  |  cantidad bloques tamanio archivo: %d",bloque_final_actual,bloque_inicial,cant_bloques_tam_archivo);
    // Liberar mi propio bloque
    liberarBloques(bloque_inicial, cant_bloques_tam_archivo); //check

    // Hay espacio libre a la derecha del bloque en el que está mi archivo
    // Pongo bloque_inicial + 1 debido a que quiero chequear desde los bloques despues del mio porque el mio esta en 0
    // 1 +1 //6
    if(hayEspacioInmediato(bloque_final_actual + 1, cant_bloques_a_aumentar)){
        ocuparBloques(bloque_inicial, cant_bloques_tam_archivo);
        int ocupar_desde = bloque_inicial + cant_bloques_tam_archivo;
        ocuparBloques(ocupar_desde, cant_bloques_a_aumentar);
    }
    // Hay espacio libre contiguo en algun lugar de todo mi archivo de bloques
    else if (hayEspacioContiguo(&bloque_inicial_nuevo, cant_bloques_tam_archivo + cant_bloques_a_aumentar)){
        config_set_value(metadata_file_config, "BLOQUE_INICIAL", string_itoa(bloque_inicial_nuevo));
        config_save(metadata_file_config);
        // Ocupamos los nuevos bloques contiguos
        ocuparBloques(bloque_inicial_nuevo, cant_bloques_tam_archivo + cant_bloques_a_aumentar);
        // Mover la información de los bloques del archivo a los bloques a ocupar
        moverContenidoBloques(bloque_inicial, bloque_inicial_nuevo, tamanio_archivo);
        // No liberamos los bloques anteriores despues de moverlos porque ya los liberamos al principio de la funcion 
    }
    // Hay espacio suficiente en todo mi archivo de bloques
    else if (hayEspacioSuficiente(cant_bloques_tam_archivo + cant_bloques_a_aumentar)) {
        // Nos guardamos el contenido de los bloques del archivo a truncar
        void *contenido_archivo = malloc(tamanio_archivo); //fijarse
        if( contenido_archivo == NULL ){
            log_error(entradasalida_logger, "Error al asignar memoria para el CONTENIDO_ARCHIVO!");
        }
        guardarContenidoArchivo(contenido_archivo, bloque_inicial, tamanio_archivo);
        // Compactamos los bloques para poder tener el espacio contiguo
        int ultimo_bloque_ocupado;
        compactar(&ultimo_bloque_ocupado, pid);
        // Movemos el contenido guardado al final
        config_set_value(metadata_file_config, "BLOQUE_INICIAL", string_itoa(ultimo_bloque_ocupado+1));
        config_save(metadata_file_config);
        FILE* archivo_bloques = abrirArchivoBloques();
        escribirArchivo(archivo_bloques, contenido_archivo, tamanio_archivo, ultimo_bloque_ocupado + 1);
        ocuparBloques(ultimo_bloque_ocupado + 1, cant_bloques_tam_archivo);
        free(contenido_archivo);
        fclose(archivo_bloques);
    }
    else {
        ocuparBloques(bloque_inicial, cant_bloques_tam_archivo);
        log_info(entradasalida_logger, "No hay espacio disponible para aumentar el tamanio del archivo a %d bloques", cant_bloques_tam_archivo + cant_bloques_a_aumentar);
        return false;
    }
    return true;
}

void interfaz_fs_truncate(char* filename, int nuevo_tamanio, int pid) {
    log_info(entradasalida_logger_min_y_obl, "DialFS - Truncar Archivo: PID: %d - Truncar Archivo: %s - Tamaño: %d", pid, filename, nuevo_tamanio);
    
    t_config* metadata_file_config = dictionary_get(metadata_dictionary_files,filename);
    if(metadata_file_config ==NULL){
        log_error(entradasalida_logger,"No encontro el archivo de config");
        exit(1);
    }
    int bloque_inicial = config_get_int_value(metadata_file_config, "BLOQUE_INICIAL");
    int tamanio_archivo = config_get_int_value(metadata_file_config, "TAMANIO_ARCHIVO");

    
    
    // nuevo_tamanio = 3 // archivo pepe
    //bloque inicial 1 //tamanio_Archivo=100
    int cant_bloques_tam_archivo = cantidadBloques(tamanio_archivo); // 1
    int cant_bloques_nuevo_tam = cantidadBloques(nuevo_tamanio); // 7
    
    if (cant_bloques_nuevo_tam == cant_bloques_tam_archivo){
        log_info(entradasalida_logger,"El archivo: %s ya tiene tamanio: %d",filename,tamanio_archivo);
        return;
    }
    else if (cant_bloques_tam_archivo < cant_bloques_nuevo_tam){
        // Aumentar tamaño archivo
        int cant_bloques_a_aumentar = cant_bloques_nuevo_tam - cant_bloques_tam_archivo;
        bool respuesta =aumentarTamanioArchivo(metadata_file_config, bloque_inicial, tamanio_archivo, cant_bloques_tam_archivo, cant_bloques_a_aumentar, pid);
        if(respuesta){
            config_set_value(metadata_file_config, "TAMANIO_ARCHIVO", string_itoa(nuevo_tamanio));
            config_save(metadata_file_config);
        }
    }
    else {
        //bloque inicial 1 // cant_bloques_Tam_archivo 7 // cant bloques nuevo 1/ cant bloques reducir 6
        // Reducir tamaño de archivo
        int cant_bloques_a_reducir = cant_bloques_tam_archivo - cant_bloques_nuevo_tam; // 1
        reducirTamanioArchivo(bloque_inicial, cant_bloques_a_reducir, cant_bloques_tam_archivo); // 3 // 1 // 2 
        config_set_value(metadata_file_config, "TAMANIO_ARCHIVO", string_itoa(nuevo_tamanio));
        config_save(metadata_file_config);
    }
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