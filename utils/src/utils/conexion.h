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
#include "planificadores.h"
typedef enum{
    // KERNEL - MEMORIA
    SOLICITUD_INICIAR_PROCESO,
    // CPU
    MENSAJE,
    PAQUETE,
    FETCH,
    INSTRUCCION,
    DATO, //usado para mov_in y mov_out
    // ...
} t_codigo_operacion;

typedef enum{
    HANDSHAKE_KERNEL,
    HANDSHAKE_CPU_INTERRUPT,
    HANDSHAKE_CPU_DISPATCH,
    HANDSHAKE_MEMORIA,
    HANDSHAKE_ENTRADASALIDA,
    HANDSHAKE_OK,
    HANDSHAKE_ERROR
} t_handshake;

typedef struct{
	int size;
	void* stream; // void* es un puntero generico (puede apuntar a cualquier tipo de dato, pero tmp sabe a q tipo de dato apunta asi que no puedo sacarle informacion de eso y por eso es importante el size)
} t_buffer;

typedef struct{
	t_codigo_operacion codigo_operacion;
	t_buffer* buffer;
} t_paquete;


//CLIENTE
int crear_conexion(char* ip, char* puerto);
void enviar_mensaje(char* mensaje, int socket_cliente);
t_paquete* crear_paquete(t_codigo_operacion codigo);
void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio);
void enviar_paquete(t_paquete* paquete, int socket_cliente);
void liberar_conexion(int socket_cliente);
void *serializar_paquete(t_paquete *paquete, int bytes);
void eliminar_paquete(t_paquete* paquete);

// SERVIDOR
int servidor(char* puerto);
void iterator(char* value);

int iniciar_servidor(char* puerto);
int esperar_cliente(int socket_servidor);
int recibir_operacion(int socket_cliente);
void* recibir_buffer(int* size, int socket_cliente);
void recibir_mensaje(int socket_cliente);
t_list* recibir_paquete(int);

#endif