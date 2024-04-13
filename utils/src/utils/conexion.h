#ifndef CONEXION_H_
#define CONEXION_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<assert.h>
#include<commons/log.h>
#include<commons/collections/list.h>

typedef enum{
	MENSAJE,
	PAQUETE
}op_code;

typedef struct{
	int size;
	void* stream; // void* es un puntero generico (puede apuntar a cualquier tipo de dato, pero tmp sabe a q tipo de dato apunta asi que no puedo sacarle informacion de eso y por eso es importante el size)
} t_buffer;

typedef struct{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

//CLIENTE
int crear_conexion(char* ip, char* puerto);
void enviar_mensaje(char* mensaje, int socket_cliente);
t_paquete* crear_paquete(void);
void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio);
void enviar_paquete(t_paquete* paquete, int socket_cliente);
void liberar_conexion(int socket_cliente);
void eliminar_paquete(t_paquete* paquete);

// SERVIDOR
int servidor(char* puerto);
void iterator(char*);

int iniciar_servidor(int puerto, t_log* logger, char* msj_server);
int esperar_cliente(int socket_servidor, t_log* logger);
int recibir_operacion(int socket_cliente);
void* recibir_buffer(int* size, int socket_cliente);
void recibir_mensaje(int socket_cliente, t_log* logger);
t_list* recibir_paquete(int);

#endif