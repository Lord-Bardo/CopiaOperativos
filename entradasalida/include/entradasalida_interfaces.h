#ifndef IO_INTERFACE_H
#define IO_INTERFACE_H

#include "./entradasalida_gestor.h"

void interfaz_generica(int cant_unidades_trabajo);
// void interfaz_stdin(char* registro_direccion, char* registro_tamanio);
void interfaz_stdin(t_list *lista_direcciones, int pid);
// void enviar_texto_a_memoria(char* direccion_fisica, char* texto, int tamanio);
// void interfaz_stdout(char* registro_direccion, char* registro_tamanio);
void interfaz_stdout(t_list* lista_direcciones, int cant_direcciones, int pid);
// void solicitar_datos_a_memoria(char* direccion_fisica, int tamanio);
// char* recibir_datos_de_memoria(int tamanio);
void interfaz_fs_create(char* filename, int pid);
void interfaz_fs_delete(char* filename, int pid);
void interfaz_fs_truncate(char* filename, int nuevo_tamanio, int pid);

int cantidadBloques(int tamanio_bytes);
void aumentarTamanioArchivo(t_config* metadata_file_config, int bloque_inicial, int tamanio_archivo, int cant_bloques_tam_archivo, int cant_bloques_a_aumentar);
void reducirTamanioArchivo(int bloque_inicial, int cant_bloques_a_reducir, int cant_bloques_tam_archivo);
void leerBloque(void* contenido, int byteDesde, int byteHasta);
void leerBloqueCompleto(void* contenido, int bytes_desde);
void leerArchivo(void* contenido, int tamanio_archivo, int bloque_inicial);
void moverContenidoBloques(int bloque_inicial, int bloque_inicial_aux, int tamanio_archivo, int cant_bloques_tam_archivo);
void liberarBloques(int bloque_inicial, int cant_bloques_tam_archivo);
bool hayEspacioContiguo(int* bloque_inicial, int cant_bloques_totales);
void ocuparBloques(int ocupar_desde, int cant_bloques_a_aumentar);
bool hayEspacioInmediato(int bloque_inicial, int cant_bloques_a_aumentar);

#endif