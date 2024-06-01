#include "conexion.h"

// CLIENTE

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

void enviar_handshake(int socket, t_handshake handshake){
	send(socket, &handshake, sizeof(t_handshake), 0);
}

t_handshake recibir_handshake(int socket){
	t_handshake handshake;
	recv(socket, &handshake, sizeof(t_handshake), MSG_WAITALL);
	return handshake;
}

void enviar_mensaje(char *mensaje, int socket_cliente){
	t_paquete *paquete = malloc(sizeof(t_paquete));

	paquete->codigo_operacion = MENSAJE;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = strlen(mensaje) + 1; // +1 para el \0
	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size); // memcpy(destino, fuente, numero_de_bytes_a_copiar)

	int bytes = paquete->buffer->size + 2 * sizeof(int); // tamaño del mensaje + un int para el codigo de operacion + un int para el tamaño del mensaje

	void *a_enviar = serializar_paquete(paquete, bytes); // codigo de operacion (int) + numero de bytes del contenido (int) + contenido (paquete->buffer->size)

	send(socket_cliente, a_enviar, bytes, 0); // Se envia el paquete al cliente. DATO: en el ultimo parametro van flags q son indicadores adicionales que controlan el comportamiento de la operación de envío (asi dijo el Dr. chatGPT); en 0 se comporta de manera estandar

	free(a_enviar);
	eliminar_paquete(paquete);
}

void crear_buffer(t_paquete *paquete){
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = 0;
	paquete->buffer->stream = NULL;
}

t_paquete *crear_paquete(void){
	t_paquete *paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = PAQUETE;
	crear_buffer(paquete);
	return paquete;
}

void agregar_a_paquete(t_paquete *paquete, void *valor, int tamanio) // agrega al buffer del paquete lo recibido en "valor" que tiene un determinado "tamanio" de bytes
{
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio + sizeof(int)); // realloc sirve para cambiar el tamaño de un bloque de memoria previamente asignado dinámicamente. En este caso le asigno a stream el tamaño q ya tenia el buffer del paquete + el numero de bytes de valor + un int para poner el tamaño de valor

	memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(int)); // copia el tamaño de valor (tamanio) al final del stream del buffer. Esto es necesario para que, al recibir el paquete, se pueda saber cuántos bytes se deben leer para obtener el valor.
	memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), valor, tamanio); // se copia valor dsp de tamanio (q colocamos recion)

	paquete->buffer->size += tamanio + sizeof(int); // actualiza el tamaño del buffer
}

void *serializar_paquete(t_paquete *paquete, int bytes){
	void *magic = malloc(bytes);
	int desplazamiento = 0;

	// Formato (no se si esto sera el protocolo de comunicacion): codigo de operacion (int) + numero de bytes del contenido (int) + contenido (paquete->buffer->size)
	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento += paquete->buffer->size;
 
	return magic; //por qué magic?????
}
// creamos el paquete, agregar_instruccion_a_paquete(paquete,puntero instruccion,queremos guardarlo sin padding), serializar_paquete_completo(agrega el op_code y al buffer a un void * y lo devuelve)

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
}

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



void enviar_paquete(t_paquete *paquete, int socket_cliente){
	int bytes = paquete->buffer->size + 2 * sizeof(int); // tamaño del stream del buffer + un int para el codigo de operacion + un int para el tamaño del buffer
	void *a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
}

void eliminar_paquete(t_paquete *paquete){
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

void liberar_conexion(int socket_cliente){
	close(socket_cliente);
}

// SERVIDOR
t_log* logger;

int servidor(char *puerto)
{
	logger = log_create("log.log", "Servidor", 1, LOG_LEVEL_DEBUG);

	int server_fd = iniciar_servidor(puerto);
	log_info(logger, "Servidor listo para recibir al cliente");
	int cliente_fd = esperar_cliente(server_fd);

	t_list *lista;
	while (1)
	{
		int cod_op = recibir_operacion(cliente_fd); // pese a q esto es un while(1) NO es un bucle que esta todo el tiempo usando el recurso del procesador, porq recibir_operacion usa recv que es una sys bloqueante y ahi corta la ejecucion hasta recibir algo
		switch (cod_op)
		{
		case MENSAJE:
			recibir_mensaje(cliente_fd);
			break;
		case PAQUETE:
			lista = recibir_paquete(cliente_fd);
			log_info(logger, "Me llegaron los siguientes valores:\n");
			list_iterate(lista, (void *)iterator);
			break;
		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
			return EXIT_FAILURE;
		default:
			log_warning(logger, "Operacion desconocida. No quieras meter la pata");
			break;
		}
	}
	return EXIT_SUCCESS;
}

void iterator(char *value)
{
	log_info(logger, "%s", value);
}

int iniciar_servidor(char* puerto){
	int socket_servidor;

	struct addrinfo hints, *servinfo, *p; // nose para que esta *p la verdad pero estaba en el TP0 :)

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

int recibir_operacion(int socket_cliente){
	int cod_op;
	if (recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0) // MSG_WAITALL asegura que recv bloquee la ejecución hasta que se hayan recibido todos los bytes / MSG_WAITALL es la bandera q hace q recv sea bloqueante y espere a recbir un valor para continuar. Si recibe un valor devuelve un numero > a 0 y lo q recibio se lo asigna a cod_op, si la conexion se corta o algo devulve un numero < a 0
		return cod_op;
	else
	{
		close(socket_cliente);
		return -1;
	}
}

void *recibir_buffer(int *size, int socket_cliente)
{
	void *buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL); // recibe el tamaño del contenido y lo guarda en size
	buffer = malloc(*size);								  // reserva el espacio de memoria necesario
	recv(socket_cliente, buffer, *size, MSG_WAITALL);	  // recibe el contenido le haya llegado y lo almacena en el buffer
	// quien llame a la funcion deberia encargarse de liberar la memoria que usa el buffer

	return buffer;
}
//hacer un recibir_instruccion
void recibir_mensaje(int socket_cliente)
{
	int size;
	char *buffer = recibir_buffer(&size, socket_cliente); // recibe el mensaje
	log_info(logger, "Me llego el mensaje %s", buffer);	  // el mensaje del log capaz q se puede cambiar
	free(buffer);										  // libera la memoria creada dinamicamente por recibir_buffer
}

t_list *recibir_paquete(int socket_cliente)
{
	int size;
	int desplazamiento = 0;
	void *buffer;
	t_list *valores = list_create();
	int tamanio;

	buffer = recibir_buffer(&size, socket_cliente);
	while (desplazamiento < size)
	{
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int)); // se copia en tamanio el tamaño en bytes del siguiente valor a leer  
		desplazamiento += sizeof(int); // se desplaza hasta donde termina el int definido para guardar tamanio
		char *valor = malloc(tamanio); // reserva dinamicamente la memoria necesaria para guardar el valor a leer
		memcpy(valor, buffer + desplazamiento, tamanio); // se copia el valor del determinado "tamanio" en "valor"
		desplazamiento += tamanio; // se desplaza hasta donde termina "valor"
		list_add(valores, valor); // agrega "valor" a la lista valores
	}
	free(buffer);

	return valores;
}

/*
void serialize(MyStruct *input, void **output, size_t *size) {
    // Calcular el tamaño necesario
    *size = sizeof(input->num) + sizeof(input->value) + sizeof(input->inner.id) + sizeof(input->inner.name);
    
    // Asignar memoria para el stream
    *output = malloc(*size);
    if (*output == NULL) {
        perror("Error allocating memory");
        exit(EXIT_FAILURE);
    }

    // Copiar los datos al stream
    void *ptr = *output;
    memcpy(ptr, &(input->num), sizeof(input->num));
    ptr += sizeof(input->num);
    memcpy(ptr, &(input->value), sizeof(input->value));
    ptr += sizeof(input->value);
    memcpy(ptr, &(input->inner.id), sizeof(input->inner.id));
    ptr += sizeof(input->inner.id);
    memcpy(ptr, &(input->inner.name), sizeof(input->inner.name));
}

int main() {
    MyStruct s = {42, 3.14, {1, "example"}};
    void *stream;
    size_t size;

    serialize(&s, &stream, &size);

    // Aquí puedes usar el stream como desees
    printf("Serialized data size: %zu bytes\n", size);

    // No olvides liberar la memoria cuando termines
    free(stream);

    return 0;
}


//shared-package
void package_close(t_package* package) {
	uint64_t package_size = sizeof(uint64_t) + sizeof(int32_t) + package->size;
	void* stream = s_malloc(package_size);
	uint64_t offset = 0;
	memcpy(stream + offset, &(package->size), sizeof(uint64_t)); //operación esté destinada a serializar el dato size en un flujo de datos (stream) para su posterior transmisión o almacenamiento.
	offset += sizeof(uint64_t);
	memcpy(stream + offset, &(package->type), sizeof(int32_t));
	offset += sizeof(int32_t);
	memcpy(stream + offset, package->buffer, package->size);
	package->type = SERIALIZED;
	package->size = package_size;
	free(package->buffer);
	package->buffer = stream;
}






*/