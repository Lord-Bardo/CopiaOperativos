#include "conexion.h"

// CONEXION -----------------------------------------------------------------------------
// Cliente
int crear_conexion(char* ip, char* puerto){
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	// Creamos el socket
	int socket_cliente = socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

	// Ahora que tenemos el socket, lo conectamos
	connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen);

	freeaddrinfo(server_info);

	return socket_cliente;
}

// Servidor
int iniciar_servidor(char* puerto){
	int socket_servidor;

	struct addrinfo hints, *servinfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(NULL, puerto, &hints, &servinfo); // el parametro NULL (junto con el fla AI_PASSIVE de hints) si no entendi mal es para poder aceptar cualquier direccion IP que tenga asociada la maquina que este corriendo el proceso

	// Creamos el socket de escucha del servidor
	socket_servidor = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

	// Asociamos el socket a un puerto
	bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen);

	// Escuchamos las conexiones entrantes
	listen(socket_servidor, SOMAXCONN); // El segundo parametro es cantidad de conexiones vivas que puede mantener, SOMAXCONN como indica el nombre, es la cantidad máxima que admite el sistema operativo

	freeaddrinfo(servinfo);

	return socket_servidor;
}

int esperar_cliente(int socket_servidor){
	// Aceptamos un nuevo cliente
	int socket_cliente;
	socket_cliente = accept(socket_servidor, NULL, NULL);

	return socket_cliente;
}

// Cliente y Servidor
void liberar_conexion(int socket){
	close(socket);
}

// Handshake
void enviar_handshake(int socket, t_codigo_operacion handshake){
	send(socket, &handshake, sizeof(t_codigo_operacion), 0);
}

t_codigo_operacion recibir_handshake(int socket){
	t_codigo_operacion handshake;
	recv(socket, &handshake, sizeof(t_codigo_operacion), MSG_WAITALL);
	return handshake;
}

// PAQUETE Y BUFFER -----------------------------------------------------------------------------
// Crear y Eliminar
t_buffer *crear_buffer(){
    t_buffer *buffer = malloc(sizeof(t_buffer));
    if( buffer == NULL ){
        perror("Error al asignar memoria al buffer");
        return NULL;
    }
    buffer->size = 0;
    buffer->stream = NULL;

    return buffer;
}

void eliminar_buffer(t_buffer *buffer){
	if( buffer != NULL ){
        free(buffer->stream);
        free(buffer);
    }
}

t_paquete *crear_paquete(t_codigo_operacion codigo){
	t_paquete *paquete = malloc(sizeof(t_paquete));
    if( paquete == NULL ){
        perror("Error al asignar memoria al paquete");
        return NULL;
    }
	paquete->codigo_operacion = codigo;
	paquete->buffer = crear_buffer();
	if( paquete->buffer == NULL ){
        free(paquete);
        return NULL;
    }

	return paquete;
}

void eliminar_paquete(t_paquete *paquete){
	if( paquete != NULL ){
        eliminar_buffer(paquete->buffer);
        free(paquete);
    }
}

// Enviar
void agregar_a_paquete(t_paquete *paquete, void *valor, int tamanio){
	// FUNCION: agrega al buffer del paquete lo recibido en "valor" que tiene un determinado "tamanio" de bytes
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio + sizeof(int)); // realloc sirve para cambiar el tamaño de un bloque de memoria previamente asignado dinámicamente. En este caso le asigno a stream el tamaño q ya tenia el buffer del paquete + el numero de bytes de valor + un int para poner el tamaño de valor

	memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(int)); // copia el tamaño de valor (tamanio) al final del stream del buffer. Esto es necesario para que, al recibir el paquete, se pueda saber cuántos bytes se deben leer para obtener el valor.
	memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), valor, tamanio); // se copia valor dsp de tamanio (q colocamos recien)

	paquete->buffer->size += tamanio + sizeof(int); // actualiza el tamaño del buffer
}

void agregar_int_a_paquete(t_paquete *paquete, int n){
    agregar_a_paquete(paquete, &n, sizeof(int));
}

void agregar_uint8_a_paquete(t_paquete *paquete, uint8_t n){
    agregar_a_paquete(paquete, &n, sizeof(uint8_t));
}

void agregar_uint32_a_paquete(t_paquete *paquete, uint32_t n){
    agregar_a_paquete(paquete, &n, sizeof(uint32_t));
}

void agregar_string_a_paquete(t_paquete *paquete, char *string){
    agregar_a_paquete(paquete, string, string_length(string)+1); // +1 para contar el '\0'
}

void *serializar_paquete(t_paquete *paquete, int bytes){
	void *magic = malloc(bytes);
	int desplazamiento = 0;

	// Formato: codigo de operacion (int) + numero de bytes del contenido (int) + contenido (paquete->buffer->size)
	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento += paquete->buffer->size;
 
	return magic; //por qué magic?????
}

int enviar_paquete(int socket, t_paquete *paquete){
	int bytes = paquete->buffer->size + 2 * sizeof(int); // tamaño del stream del buffer + un int para el codigo de operacion + un int para el tamaño del buffer
	void *a_enviar = serializar_paquete(paquete, bytes);

	int ret = send(socket, a_enviar, bytes, MSG_NOSIGNAL);

	free(a_enviar);

	return ret;
}

int enviar_codigo_operacion(int socket, t_codigo_operacion codigo_operacion){
	return send(socket, &codigo_operacion, sizeof(t_codigo_operacion), MSG_NOSIGNAL);
}

// Recibir
int recibir_codigo_operacion(int socket, t_codigo_operacion *codigo_operacion){
    // if( recv(socket, codigo_operacion, sizeof(t_codigo_operacion), MSG_WAITALL) != sizeof(t_codigo_operacion) ){
    //     perror("Error al recibir el codigo de operacion");
    //     return;
    // }
	return recv(socket, codigo_operacion, sizeof(t_codigo_operacion), MSG_WAITALL);
}

void recibir_buffer(int socket, t_buffer *buffer){
    // Recibo el tamaño
    if( recv(socket, &(buffer->size), sizeof(int), MSG_WAITALL) != sizeof(int) ){
        perror("Error al recibir el tamaño del buffer");
        return;
    }

    // Redimensiono el tamaño del buffer
    void* new_stream = realloc(buffer->stream, buffer->size); // Asigno el puntero a otra variable xq si falla el realloc perderia la referencia a buffer->stream
    if( new_stream == NULL ){
        perror("Error al redimensionar el buffer");
        return;
    }
    buffer->stream = new_stream;

    // Recibo el contenido
    if( recv(socket, buffer->stream, buffer->size, MSG_WAITALL) != buffer->size ){
        perror("Error al recibir el contenido del buffer");
        return;
    }
}

int recibir_paquete(int socket, t_codigo_operacion *codigo_operacion, t_buffer *buffer){
    int ret = recibir_codigo_operacion(socket, codigo_operacion);
	if(ret > 0){
		recibir_buffer(socket, buffer);
	}
    return ret;
}

// Desempaquetar Buffer
void buffer_actualizar_size(t_buffer *buffer, int bytes){
	// Actualizo el tamaño del buffer
	buffer->size -= bytes;
}

void buffer_actualizar_stream(t_buffer *buffer, int bytes){
	// Muevo todo el contenido del stream posterior a los bytes leidos al principio del stream
    // Por si no se entendio: stream = "4como5estas". Suponiendo que lo ultimo leido fue el 4 => me copio al principio del stream "como5estas", lo cual esta en la posicion stream + los bytes leidos (que en este caso seria el int)
    memmove(buffer->stream, buffer->stream + bytes, buffer->size); // Uso memmove porq es mas seguro q memcpy cuando hay que solapar memoria

    // Redimensiono el tamaño de memoria asignado al stream del buffer
    void *new_stream = realloc(buffer->stream, buffer->size);
    if( new_stream == NULL ){
        perror("Error al redimensionar el buffer");
        return;
    }
    buffer->stream = new_stream;
}

void buffer_actualizar(t_buffer *buffer, int bytes){
	buffer_actualizar_size(buffer, bytes);
	// Compruebo que el size sea mayor a 0 porq sino el realloc de buffer_actualizar_stream va a devolver NULL (por su implementacion, no porque haya fallado la asignacion de memoria)
	// Ademas, si size = 0 es porque ya no quedan elementos en el buffer, no hay necesidad de actualizar.
	if( buffer->size > 0 ){
		buffer_actualizar_stream(buffer, bytes);
	}
}

void buffer_desempaquetar(t_buffer *buffer, void *destino){
    // Chequeo que el buffer tenga contenido para desempaquetar
    if( buffer->stream == NULL || buffer->size == 0 ){
        perror("Error al desempaquetar el buffer - No hay contenido a desempaquetar");
        return;
    }
    
    // Desempaqueto el tamaño del siguiente contenido y lo guardo en bytes
    int bytes;
    memcpy(&bytes, buffer->stream, sizeof(int));
    buffer_actualizar(buffer, sizeof(int));
    
	// Desempaqueto el contenido y lo guardo en destino
    memcpy(destino, buffer->stream, bytes);
	buffer_actualizar(buffer, bytes);
}

// Se requiere una funcion especial para los char * xq hasta no desempaquetar la cantidad de bytes del string no se sabe cuanta memoria pedir con malloc
char *buffer_desempaquetar_string(t_buffer *buffer){
	// Chequeo que el buffer tenga contenido para desempaquetar
    if( buffer->stream == NULL || buffer->size == 0 ){
        perror("Error al desempaquetar el buffer - No hay contenido a desempaquetar");
        return NULL;
    }

    // Desempaqueto el tamaño del siguiente string y lo guardo en bytes
    int bytes;
    memcpy(&bytes, buffer->stream, sizeof(int));
    buffer_actualizar(buffer, sizeof(int));
    
	// Desempaqueto el string.
    char *string = malloc(bytes);
	if( string == NULL ){
		perror("Error al asginar memoria para el STRING");
		return NULL;
	}
	memcpy(string, buffer->stream, bytes);
	buffer_actualizar(buffer, bytes);

	return string;
}

// Otra opcion de desempaquetar que sirve para todo, pero es mas quilombo recibir -> Ejemplo de uso: int pid = *(int *)buffer_desempaquetar(buffer); || int *pid = buffer_desempaquetar(buffer);
// void *buffer_desempaquetar(t_buffer *buffer){
//     // Chequeo que el buffer tenga contenido para desempaquetar
//     if( buffer->stream == NULL || buffer->size == 0 ){
//         perror("Error al desempaquetar el buffer - No hay contenido a desempaquetar");
//         return "";
//     }
    
//     // Desempaqueto el tamaño del siguiente contenido y lo guardo en bytes
//     int bytes;
//     memcpy(&bytes, buffer->stream, sizeof(int));
//     buffer_actualizar(buffer, sizeof(int));
    
//     void *contenido = malloc(bytes);
//     if( contenido == NULL ){
//         perror("Error al asignar memoria para el contenido del buffer");
//         return NULL;
//     }
//     memcpy(contenido, buffer->stream, bytes);
//     buffer_actualizar(buffer, bytes);

//     return contenido;
// }



// void crear_buffer(t_paquete *paquete){
// 	paquete->buffer = malloc(sizeof(t_buffer));
// 	paquete->buffer->size = 0;
// 	paquete->buffer->stream = NULL;
// }

// t_paquete *crear_paquete(t_codigo_operacion codigo){
// 	t_paquete *paquete = malloc(sizeof(t_paquete));
// 	paquete->codigo_operacion = codigo;
// 	crear_buffer(paquete);
// 	return paquete;
// }

// void enviar_mensaje(char *mensaje, int socket_cliente){ // REVISAR SI ES NECESARIA
// 	t_paquete *paquete = malloc(sizeof(t_paquete));

// 	paquete->codigo_operacion = MENSAJE;
// 	paquete->buffer = malloc(sizeof(t_buffer));
// 	paquete->buffer->size = strlen(mensaje) + 1; // +1 para el \0
// 	paquete->buffer->stream = malloc(paquete->buffer->size);
// 	memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size); // memcpy(destino, fuente, numero_de_bytes_a_copiar)

// 	int bytes = paquete->buffer->size + 2 * sizeof(int); // tamaño del mensaje + un int para el codigo de operacion + un int para el tamaño del mensaje

// 	void *a_enviar = serializar_paquete(paquete, bytes); // codigo de operacion (int) + numero de bytes del contenido (int) + contenido (paquete->buffer->size)

// 	send(socket_cliente, a_enviar, bytes, 0); // Se envia el paquete al cliente. DATO: en el ultimo parametro van flags q son indicadores adicionales que controlan el comportamiento de la operación de envío (asi dijo el Dr. chatGPT); en 0 se comporta de manera estandar

// 	free(a_enviar);
// 	eliminar_paquete(paquete);
// }

// t_codigo_operacion recibir_codigo_operacion(int socket_cliente){
// 	t_codigo_operacion cod_op;
// 	if (recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0) // MSG_WAITALL asegura que recv bloquee la ejecución hasta que se hayan recibido todos los bytes / MSG_WAITALL es la bandera q hace q recv sea bloqueante y espere a recbir un valor para continuar. Si recibe un valor devuelve un numero > a 0 y lo q recibio se lo asigna a cod_op, si la conexion se corta o algo devulve un numero < a 0
// 		return cod_op;
// 	else{
// 		close(socket_cliente);
// 		return -1;
// 	}
// }

// void *recibir_buffer(int *size, int socket_cliente){
// 	void *buffer;

// 	recv(socket_cliente, size, sizeof(int), MSG_WAITALL); // recibe el tamaño del contenido y lo guarda en size
// 	buffer = malloc(*size);								  // reserva el espacio de memoria necesario
// 	recv(socket_cliente, buffer, *size, MSG_WAITALL);	  // recibe el contenido le haya llegado y lo almacena en el buffer
// 	// quien llame a la funcion deberia encargarse de liberar la memoria que usa el buffer

// 	return buffer;
// }

// void recibir_mensaje(int socket_cliente){ // REVISAR SI ES NECESARIA
// 	int size;
// 	char *buffer = recibir_buffer(&size, socket_cliente); // recibe el mensaje
// 	free(buffer);										  // libera la memoria creada dinamicamente por recibir_buffer
// }

// t_list *recibir_paquete(int socket_cliente){
// 	int size;
// 	int desplazamiento = 0;
// 	void *buffer;
// 	t_list *valores = list_create();
// 	int tamanio;

// 	buffer = recibir_buffer(&size, socket_cliente);
// 	while (desplazamiento < size){
// 		memcpy(&tamanio, buffer + desplazamiento, sizeof(int)); // se copia en tamanio el tamaño en bytes del siguiente valor a leer  
// 		desplazamiento += sizeof(int); // se desplaza hasta donde termina el int definido para guardar tamanio
// 		char *valor = malloc(tamanio); // reserva dinamicamente la memoria necesaria para guardar el valor a leer
// 		memcpy(valor, buffer + desplazamiento, tamanio); // se copia el valor del determinado "tamanio" en "valor"
// 		desplazamiento += tamanio; // se desplaza hasta donde termina "valor"
// 		list_add(valores, valor); // agrega "valor" a la lista valores
// 	}
// 	free(buffer);

// 	return valores;
// }


// FUNCIONES FABRI, FRAN Y VICKY ----------------------------------------------------------------------------
/*

void recibir_pcb(int socket_cliente){
	t_list *pcb_como_lista = recibir_paquete(socket_cliente);
	t_pcb pcb;
	
}

void agregar_instruccion_paquete(t_paquete *paquete, char* instruccion)
{
	agregar_a_paquete(paquete, instruccion, string_length(instruccion));
}
void agregar_pcb_paquete(t_paquete *paquete, t_pcb *pcb){
	agregar_a_paquete(paquete, pcb->PID, sizeof(int));
	agregar_a_paquete(paquete, pcb->quantum, sizeof(int));
	agregar_a_paquete(paquete, &(pcb->estado), sizeof(t_estado));
	agregar_a_paquete(paquete, pcb->registros.PC, 8);
	agregar_a_paquete(paquete, pcb->registros.AX, 2);
	agregar_a_paquete(paquete, pcb->registros.BX, 2);
	agregar_a_paquete(paquete, pcb->registros.CX, 2);
	agregar_a_paquete(paquete, pcb->registros.DX, 2);
	agregar_a_paquete(paquete, pcb->registros.EAX, 8);
	agregar_a_paquete(paquete, pcb->registros.EBX, 8);
	agregar_a_paquete(paquete, pcb->registros.ECX, 8);
	agregar_a_paquete(paquete, pcb->registros.EDX, 8);
	agregar_a_paquete(paquete, pcb->registros.SI, 8);
	agregar_a_paquete(paquete, pcb->registros.DI, 8);
}

*/


// creamos el paquete, agregar_instruccion_a_paquete(paquete,puntero instruccion,queremos guardarlo sin padding), serializar_paquete_completo(agrega el op_code y al buffer a un void * y lo devuelve)
/*
void* deserializar(t_paquete paquete) { // creo que la arreglé

	 if(paquete == NULL || paquete->stream == NULL || paquete->size <= 0) {
       printf("\n[ERROR] El paquete no es válido para deserializar.\n");
       return NULL; // Devuelve NULL para indicar un error
    }
	int size_mensaje; // entiendase mensaje por el void* que esta en el paquete.
	memcpy(&size_mensaje, paquete->stream, sizeof(int));
	void* mensaje = malloc(size_mensaje);
	memcpy(mensaje, paquete->stream + sizeof(int), size_mensaje);

	int nuevo_size = paquete->size - sizeof(int) - size_mensaje;
	if (nuevo_size < 0) {
		perror("\n [ERROR_MENSAJE] BUFFER con tamaño negativo");
		exit(EXIT_FAILURE);
	}

	paquete->size = nuevo_size;
	if (nuevo_size == 0) {
		free(paquete->stream);
		paquete->stream = NULL;
	} else {
		void* nuevo_stream = malloc(nuevo_size);
		memcpy(nuevo_stream, paquete->stream + sizeof(int) + size_mensaje, nuevo_size); 
		free(paquete->stream);
		paquete->stream = nuevo_stream;
	}

    return mensaje;
}*/

/*
en shared_deserializer:
// Devuelve el mensaje deserializado, y destruye el paquete
char* deserialize_message(t_package* package);
---------------------------------------------------------------
char* deserialize_message(t_package* package) {
	uint64_t offset = 0;
	char* message = package_decode_string(package->buffer, &offset);
	package_destroy(package);
	return message;
}

t_package* package_decode(void* source, uint64_t* offset) {
	bool discarded_offset = offset == NULL;
	if (discarded_offset) {
		offset = s_malloc(sizeof(uint64_t));
		*offset = 0;
	}
	t_package* package = s_malloc(sizeof(t_package));
	memcpy(&(package->size), source + *offset, sizeof(uint64_t));
	*offset += sizeof(uint64_t);
	memcpy(&(package->type), source + *offset, sizeof(int32_t));
	*offset += sizeof(int32_t);
	package->buffer = s_malloc(package->size);
	memcpy(package->buffer, source + *offset, package->size);
	*offset += package->size;
	if (discarded_offset) free(offset);
	return package;
}

char* package_decode_string(void* buffer, uint64_t* offset) {
	bool discarded_offset = offset == NULL;
	if (discarded_offset) {
		offset = s_malloc(sizeof(uint64_t));
		*offset = 0;
	}
	uint64_t *size = s_malloc(sizeof(uint64_t));
	memcpy(size, buffer + *offset, sizeof(uint64_t));
	*offset += sizeof(uint64_t);
	char *str = s_malloc(*size + 1);
	memcpy(str, buffer + *offset, *size);
	str[*size] = '\0';
	*offset += *size;
	free(size);
	if (discarded_offset) free(offset);
	return str;
}
void package_decode_buffer(void* buffer, void* dest, uint64_t* offset) {
	bool discarded_offset = offset == NULL;
	if (discarded_offset) {
		offset = s_malloc(sizeof(uint64_t));
		*offset = 0;
	}
	uint64_t *size = s_malloc(sizeof(uint64_t));
	memcpy(size, buffer + *offset, sizeof(uint64_t));
	*offset += sizeof(uint64_t);
	memcpy(dest, buffer + *offset, *size);
	*offset += *size;
	free(size);
	if (discarded_offset) free(offset);
}
*/