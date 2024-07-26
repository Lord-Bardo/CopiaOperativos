#include "../include/inicializar_interfaces.h"

void inicializar_interfaz(char* nombre_interfaz, char* archivo_configuracion){
    iniciar_logger(nombre_interfaz);
    iniciar_config(nombre_interfaz, archivo_configuracion);

    if (strcmp(TIPO_INTERFAZ, "DIALFS") == 0) {
        inicializar_fs();
        // dialfs_inicializado = true;
    }

    /* Chequear si hubo previamente creación de archivos */
    /* dirent.h -- Revisar libreria */
    /* Quedarme con los archivos metadata - me sirven para la compactación */
    /* T bit array para el bitmap , debe coincidir con el tamaño del archivo */
    /* Investigar sobre la función mmap */
    /* cuando inicio el bitarray tengo que hacer un ftrunc para darle al archivo el espacio que necesita */
}

/* El de soporte me dijo que conviene tener un solo logger pero algo me dijo de los logs distintos - ver grabacion */
/* Capaz también me conviene poner un parametro como en el config */
void iniciar_logger(char* nombre_interfaz){

   /*  if (strcmp(nombre_interfaz, "generica") == 0) {
        generica_logger = log_create("generica.log", "INTERFAZ GENERICA", 1, LOG_LEVEL_INFO);
        // Ver de validar en cada uno que si no se pudo crear el log que tire un error
    } else if (strcmp(nombre_interfaz, "stdin") == 0) {
        stdin_logger = log_create("stdin.log", "INTERFAZ STDIN", 1, LOG_LEVEL_INFO);
    } else if (strcmp(nombre_interfaz, "stdout") == 0) {
        stdout_logger = log_create("stdout.log", "INTERFAZ STDOUT", 1, LOG_LEVEL_INFO);
    } else if (strcmp(nombre_interfaz, "dialfs") == 0) {
        dialfs_logger = log_create("dialfs.log", "INTERFAZ DIALFS", 1, LOG_LEVEL_INFO);
    } else {
        perror("Tipo de interfaz desconocida: %s\n", nombre_interfaz);
        exit(1);
    } */

    entradasalida_logger = log_create("entradasalida_logger.log", "ENTRADASALIDA", 1, LOG_LEVEL_INFO);
	if(entradasalida_logger == NULL){
		perror("No se pudo crear el logger.");
		exit(1);
	}

    entradasalida_logger_min_y_obl = log_create("entradasalida_min_y_obl.log", "ENTRADASALIDA", 1, LOG_LEVEL_INFO);
	if(entradasalida_logger_min_y_obl == NULL){
		perror("No se pudo crear el logger.");
		exit(1);
	}
}

void iniciar_config(char* nombre_interfaz, char* archivo_configuracion){
    char config_path[256]; // Le pongo ese tamaño para asegurarme que es lo suficientemente grande como para guardar la ruta del archivo
    sprintf(config_path, "/home/utnso/tp-2024-1c-GSN/entradasalida/%s", archivo_configuracion);
    entradasalida_config = config_create(config_path);
	if(entradasalida_config == NULL){
		perror("No se pudo crear el config.");
        perror(config_path);
		exit(2); // El exit(2) se utiliza para indicar que el programa terminó debido a un error relacionado con la creación del archivo de configuración (ver de ponerlo en los otros módulos también)
	}

    TIPO_INTERFAZ = config_get_string_value(entradasalida_config, "TIPO_INTERFAZ");
	TIEMPO_UNIDAD_TRABAJO = config_get_int_value(entradasalida_config, "TIEMPO_UNIDAD_TRABAJO");
	IP_KERNEL = config_get_string_value(entradasalida_config, "IP_KERNEL");
    PUERTO_KERNEL = config_get_string_value(entradasalida_config, "PUERTO_KERNEL");
    IP_MEMORIA = config_get_string_value(entradasalida_config, "IP_MEMORIA");
    PUERTO_MEMORIA = config_get_string_value(entradasalida_config, "PUERTO_MEMORIA");
    PATH_BASE_DIALFS = config_get_string_value(entradasalida_config, "PATH_BASE_DIALFS");
    BLOCK_SIZE = config_get_int_value(entradasalida_config, "BLOCK_SIZE");
    BLOCK_COUNT = config_get_int_value(entradasalida_config, "BLOCK_COUNT");
    RETRASO_COMPACTACION = config_get_int_value(entradasalida_config, "RETRASO_COMPACTACION");
}

// Logs obligatorios
/* Todos - Operación: "PID: <PID> - Operacion: <OPERACION_A_REALIZAR>"
DialFS - Crear Archivo: "PID: <PID> - Crear Archivo: <NOMBRE_ARCHIVO>"
DialFS - Eliminar Archivo: "PID: <PID> - Eliminar Archivo: <NOMBRE_ARCHIVO>"
DialFS - Truncar Archivo: "PID: <PID> - Truncar Archivo: <NOMBRE_ARCHIVO> - Tamaño: <TAMAÑO>"
DialFS - Leer Archivo: "PID: <PID> - Leer Archivo: <NOMBRE_ARCHIVO> - Tamaño a Leer: <TAMAÑO> - Puntero Archivo: <PUNTERO_ARCHIVO>"
DialFS - Escribir Archivo: "PID: <PID> - Escribir Archivo: <NOMBRE_ARCHIVO> - Tamaño a Escribir: <TAMAÑO> - Puntero Archivo: <PUNTERO_ARCHIVO>"
DialFS - Inicio Compactación: "PID: <PID> - Inicio Compactación."
DialFS - Fin Compactación: "PID: <PID> - Fin Compactación."
 */

void inicializar_fs(){
    metadata_dictionary_files = dictionary_create();
    crear_directorio_fs();
    crear_archivo_bloques();
    crear_bitmap();
    log_info(entradasalida_logger, "Filesystem inicializado correctamente");
}

void crear_directorio_fs(){
    // Creo la carpeta/directorio donde se van a almacenar los archivos del FS
    if( mkdir(PATH_BASE_DIALFS, 0777) == -1 ){
        // Si mkdir falla, verifico si el error es porque el directorio ya existe o si es porque fallo la creacion
        if( errno == EEXIST ){
            log_info(entradasalida_logger, "El directorio '%s' ya existe, salteo la creacion", PATH_BASE_DIALFS);
        }
        else{
            log_error(entradasalida_logger, "Fallo la creacion del directorio '%s'", PATH_BASE_DIALFS);
            return;
        }
    }
    else{
        log_info(entradasalida_logger, "El directorio '%s' fue creado correctamente!", PATH_BASE_DIALFS);
    }
}

void crear_archivo_bloques(){
    // Le asigno el path al archivo de bloques
    char *path_archivo_bloques = string_duplicate(PATH_BASE_DIALFS);
    string_append(&path_archivo_bloques, "/bloques.dat");

    // Abro/creo el archivo
    FILE *bloques_file = fopen(path_archivo_bloques, "r");
    if( bloques_file == NULL ){
        bloques_file = fopen(path_archivo_bloques, "wb");
        if( bloques_file == NULL ){
            perror("Error al crear bloques.dat");
            // fclose(bloques_file); - creo que si es null no lo abrio
            exit(1);
        }
        // Con ftruncate establezco el tamaño del archivo recién creado a BLOCK_SIZE * BLOCK_COUNT
        // fileno obtiene el file descriptor del archivo que necesita ftruncate
        ftruncate(fileno(bloques_file), BLOCK_SIZE * BLOCK_COUNT);
        fclose(bloques_file);
    } else {
        // Si el archivo ya existia lo cierro
        fclose(bloques_file);
    }
}

// MATI: Crearia una estructura para el bitmap y mandaria la creacion del bitmap a una funcion aparte. Idem bloque de datos

void crear_bitmap(){
    // Le asigno el path al archivo de bloques
    char *path_bitmap = string_duplicate(PATH_BASE_DIALFS);
    string_append(&path_bitmap, "/bitmap.dat");
    
    int bitmap_fd = open(path_bitmap, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if( bitmap_fd == -1 ){
        log_info(entradasalida_logger, "Error al abrir bitmap.dat");
        exit(1);
    }

    // fstat se usa para obtener información sobre el archivo de bitmap
    struct stat st;
    if( fstat(bitmap_fd, &st) == -1 ){
        log_info(entradasalida_logger, "Error al obtener información del bitmap");
        exit(1);
    }

    // (BLOCK_COUNT / 8) + (BLOCK_COUNT % 8 != 0) convierte el número total de bloques en el número necesario de bytes, 
    // teniendo en cuenta que pueden ser necesarios bits adicionales si el número de bloques no es un múltiplo exacto de 8 (por eso el + si no da un multiplo exactao)
    size_t bitmap_size = (BLOCK_COUNT / 8) + (BLOCK_COUNT % 8 != 0);

    // Si el tamaño del archivo es 0, se inicializa el tamaño del archivo a bitmap_size usando ftruncate
    if( st.st_size == 0 ){
        if( ftruncate(bitmap_fd, bitmap_size) == -1 ){
            log_info(entradasalida_logger, "Error al inicializar el tamaño del bitmap");
            exit(1);
        }
    }

    // Mapear el archivo del bitmap a memoria
    //   - NULL: El sistema decide la dirección de inicio del mapeo
    //   - bitmap_size: El tamaño del mapeo
    //   - PROT_READ | PROT_WRITE: Permisos de lectura y escritura
    //   - MAP_SHARED: Los cambios en el mapeo se reflejan en el archivo 
    //   - bitmap_fd: Descriptor de archivo del bitmap
    //   - 0: Desplazamiento desde el inicio del archivo
    /* mmap se usa para mapear el archivo en memoria, lo que permite un acceso más eficiente a los datos del bitmap. Necesitas un file descriptor para esta operación, y open es la forma estándar de obtenerlo. */ 
    bitmap_data = mmap(NULL, bitmap_size, PROT_READ | PROT_WRITE, MAP_SHARED, bitmap_fd, 0);
    if( bitmap_data == MAP_FAILED ){
        log_info(entradasalida_logger, "Error al mapear el archivo del bitmap");
        close(bitmap_fd);
        exit(1);
    }

    // Crear el bitarray
    // se usa para crear el bitarray utilizando los datos mapeados en memoria. MSB_FIRST indica que el bit más significativo es el primero
    bitmap = bitarray_create_with_mode(bitmap_data, bitmap_size, MSB_FIRST);
    if( bitmap == NULL ){
        log_info(entradasalida_logger, "Error al crear el bitarray");
        // munmap se usa para liberar la región de memoria mapeada
        //  - bitmap_data es el puntero a la región de memoria mapeada.
        //  - bitmap_size es el tamaño de la región de memoria a desmapear.
        munmap(bitmap_data, bitmap_size);

        // close se usa para cerrar el descriptor de archivo bitmap_fd una vez que ya no se necesita
        close(bitmap_fd);
        exit(1);
    }

    int i = 0;
    bool bit = false;
    while( bit == false && i < BLOCK_COUNT ){
        bit = bitarray_test_bit(bitmap, i);
        if( bit == false ){
            bitarray_clean_bit(bitmap, i);
        }
        i++;
    }

    // msync se usa para asegurarse de que los cambios realizados en la región de memoria mapeada se escriban en el archivo en disco
        //  - bitmap_data es el puntero a la región de memoria mapeada
        //  - bitmap_size es el tamaño de la región de memoria a sincronizar /* LE PUSE BITARRAY SIZE AHORA */
        //  - MS_SYNC es una bandera que indica que la función debe bloquearse hasta que los cambios estén escritos en el disco
    msync(bitmap_data, bitmap->size, MS_SYNC);
    close(bitmap_fd);
}