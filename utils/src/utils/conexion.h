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
    // KERNEL - MEMORIA
    SOLICITUD_INICIAR_PROCESO,
    CONFIRMACION_PROCESO_INICIADO, // este mensaje para qué lo necesita kernel? con qué info debería enviarse?
    ERROR_CREACION_PROCESO, 
    SOLICITUD_FINALIZAR_PROCESO,
    // KERNEL - CPU DISPATCH
    CONTEXTO_DE_EJECUCION,
    // Motivos desalojo
    FINALIZACION,
    SYSCALL,
    INTERRUPCION,
    OUT_OF_MEMORY,
    // KERNEL - ENTRADASALIDA (manejo de interfaces)
    IO_GEN_SLEEP,
    IO_STDIN_READ,
    IO_STDOUT_WRITE,
    IO_FS_CREATE,
    IO_FS_DELETE,
    IO_FS_TRUNCATE,
    IO_FS_WRITE,
    IO_FS_READ,
    // CPU
    MENSAJE,
    PAQUETE,
    FETCH,
    INSTRUCCION,
    DATO, 
    PCB,//usado para mov_in y mov_out
    // ...
    //ENTRADASALIDA - MEMORIA
    SOLICITUD_ESCRITURA,
    SOLICITUD_LECTURA,
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


// CONEXION
// Cliente
int crear_conexion(char* ip, char* puerto);
// Servidor
int iniciar_servidor(char* puerto);
int esperar_cliente(int socket_servidor);
// Cliente y Servidor
void liberar_conexion(int socket);
// Handshake
void enviar_handshake(int socket, t_handshake handshake);
t_handshake recibir_handshake(int socket);

// PAQUETE Y BUFFER
// Crear y Eliminar
t_buffer *crear_buffer(void);
void eliminar_buffer(t_buffer *buffer);
t_paquete* crear_paquete(t_codigo_operacion codigo);
void eliminar_paquete(t_paquete *paquete);
// Enviar
void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio);
void *serializar_paquete(t_paquete *paquete, int bytes);
void enviar_paquete(int socket, t_paquete* paquete);
void enviar_codigo_operacion(int socket, t_codigo_operacion codigo_operacion);
// Recibir
void recibir_codigo_operacion(int socket, t_codigo_operacion *codigo_operacion);
void recibir_buffer(int socket, t_buffer *buffer);
void recibir_paquete(int socket, t_codigo_operacion *codigo_operacion, t_buffer *buffer);
// Desempaquetar Buffer
void buffer_actualizar_size(t_buffer *buffer, int bytes);
void buffer_actualizar_stream(t_buffer *buffer, int bytes);
void buffer_actualizar(t_buffer *buffer, int bytes);
void buffer_desempaquetar(t_buffer *buffer, void *destino);
char *buffer_desempaquetar_string(t_buffer *buffer);

#endif