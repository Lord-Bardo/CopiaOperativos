#include "../include/cpu.h"

int main(int argc, char *argv[]) {
	// Inicializar estructuras de CPU (loggers y config)
	inicializar_cpu();
	//t_paquete *auxiliar = crear_paquete(PAQUETE);
	/* 
    }; */
	//agregar_pcb_paquete(auxiliar,&(ejemplo_pcb)); //es serializarlo

	//iniciar_ciclo_instruccion(ejemplo_pcb);
	// Conexion con MEMORIA
	fd_memoria = crear_conexion(IP_MEMORIA, PUERTO_MEMORIA);
	log_info(cpu_logger, "Conexion con MEMORIA establecida!");

	// Iniciar servidor DISPATCH de CPU
	fd_cpu_dispatch = iniciar_servidor(PUERTO_ESCUCHA_DISPATCH);
	log_info(cpu_logger, "Servidor CPU DISPATCH iniciado!");

	// Iniciar servidor INTERRUPT de CPU
	fd_cpu_interrupt = iniciar_servidor(PUERTO_ESCUCHA_INTERRUPT);
	log_info(cpu_logger, "Servidor CPU INTERRUPT iniciado!");

	// Esperar conexion del KERNEL
	fd_kernel_dispatch = esperar_cliente(fd_cpu_dispatch);
	log_info(cpu_logger, "Se conecto el cliente KERNEL al servidor CPU DISPATCH!");

	// Esperar conexion del KERNEL
	fd_kernel_interrupt = esperar_cliente(fd_cpu_interrupt);
	log_info(cpu_logger, "Se conecto el cliente KERNEL al servidor CPU INTERRUPT!");

	// Atender los mensajes de KERNEL - DISPATCH
	// declaro el hilo
	pthread_t hilo_kernel_dispatch;
	// creo el hilo. Primer parametro: donde guarda el hilo. Segundo: NULL para q se seteen los valores por defecto. Tercero: funcion a ejecutar en el hilo. Cuarto: si la funcion recibe parametros van aca
	pthread_create(&hilo_kernel_dispatch, NULL, (void*)atender_cpu_kernel_dispatch, NULL); // tengo duda con el tercer parametro, xq chatgpt recomienda usar un wrapper de la funcion y q tenga la firma q pide pthread_create q es void* (*)(void*) lo cual describe un puntero a una función que toma un puntero void como argumento y devuelve un puntero void. O la otra es modificar directo la firma de la funcion.

	// Atender los mensajes de KERNEL - INTERRUPT
	pthread_t hilo_kernel_interrupt;
	pthread_create(&hilo_kernel_interrupt,NULL, (void*)atender_cpu_kernel_interrupt, NULL);

	// Atender los mensajes de MEMORIA
	pthread_t hilo_memoria;
	pthread_create(&hilo_memoria, NULL, (void*)atender_cpu_memoria, NULL); //no hacen falta hilos

	// Esperar a que los hilos finalicen su ejecucion
	pthread_join(hilo_kernel_dispatch, NULL); // en el segundo parametro se guarda el resultado de la funcion q se ejecuto en el hilo, si le pongo NULL basicamente es q no me interesa el resultado, solo me importa esperar a q termine
	pthread_join(hilo_kernel_interrupt, NULL);
	pthread_join(hilo_memoria, NULL);


	// Finalizar CPU (liberar memoria usada)
	terminar_programa();

	return 0;
}

void leer_consola(t_log *logger){ //lo comenté porque ya fue definido en memoria.c
	char *leido;

	// Leo la primer linea
	leido = readline("> ");

	// El resto, las voy leyendo y logueando hasta recibir un string vacío
	while (leido[0] != '\0')
	{
		log_info(logger, "%s", leido);
		leido = readline("> ");
	}

	// Libero las lineas
	free(leido);
}

void paquete(int conexion) //esta definido en memoria.h, en cpu.h y kernel.h
{
	char *leido;
	t_paquete *paquete;

	// Creo el paquete
	paquete = crear_paquete(PAQUETE);

	// Leo y agrego las lineas al paquete
	leido = readline("> ");
	while (leido[0] != '\0')
	{
		agregar_a_paquete(paquete, leido, strlen(leido) + 1);
		leido = readline("> ");
	}

	// Envio el paquete
	enviar_paquete(paquete, conexion);

	// Libero las lineas y el paquete
	free(leido);
	eliminar_paquete(paquete);
}

void terminar_programa(){
	if (cpu_logger != NULL){
		log_destroy(cpu_logger);
	}

	if (cpu_config != NULL){
		config_destroy(cpu_config);
	}

	liberar_conexion(fd_cpu_dispatch);
	liberar_conexion(fd_cpu_interrupt);
	liberar_conexion(fd_kernel_dispatch);
	liberar_conexion(fd_kernel_interrupt);
	liberar_conexion(fd_memoria);
}

/* 
El IC es igual a op_cod + n, siendo n parametros o registros 
(por ejemplo: SET AX 3, los parametros son AX y 3, y SET es el op_cod).
Los parametros lo guardamos en una lista.
*/

void iniciar_ciclo_instruccion(t_pcb pcb){ //hay que meter manejo de interrupciones y logs y semaforos y de todo lapu
	 //buffer y buffer_size
	while(1){ //habria que mandar el pi tambien, x que van a haber varios procesos en memoria
		t_paquete *paquete_direccion_instruccion = crear_paquete(FETCH); //creamos el paquete para mandarle la dire de instruccion que queremos a memoria
		agregar_a_paquete(paquete_direccion_instruccion, pcb->registros.PC, sizeof(__uint32_t));
		serializar_paquete_direccion(paquete_direccion_instruccion, (sizeof(int) + (paquete_direccion_instruccion->buffer->size)));
		enviar_paquete(paquete_direccion_instruccion, fd_memoria); //solicitmos la instruccion a memoria, vamos a tener que mandar tambien el id de proceso para que lo pueda buscaar
		atender_cpu_memoria();//acá se ejecuta la instrucción
		//aca tiene que bloquearse x que tiene que esperar que se ejecuta le instruccion
		}
	//aca iria el camino feliz cuando termina el procesdo bien y le devuelve el nuevo pcb a kerrnel
	
	//agregar_a_paquete deberia meter en un buffer lo que queremos mandar, deberiamos hacerlo especifico para cada cosa.EJ: agregar_instruccion_a_paquete
	//serializar_paquete tiene que recibir el buffer y nos devuelve el void* que vamos a mandar


	pcb->estado= 4; //4 es exit, esto para mi va en el planificador /
	t_paquete *paquete_pcb_actualizado = crear_paquete(PAQUETE);
	agregar_a_paquete(paquete_pcb_actualizado, pcb, sizeof(t_pcb)); //tenemos que poner todo lo del nuevo pcb aca, x ahi un cargar pcb en paquete, tambien llamado serializarPCB
	serializar_paquete(paquete_pcb_actualizado, (sizeof(enum)+ (paquete_pcb_actualizado->buffer->size)));
	enviar_paquete(paquete_pcb_actualizado, fd_kernel_dispatch);
	
	
	//liberar memoria hacer logs
}

/*void ejecutar_instruccion(t_instruccion* instruccion) // recibe el PCB serializado.
{ 
	//aca hay qye hacer cosas
	switch (instruccion.op_cod) { //no mandas pc, vamos con el id del proceso y la instruccion que queremos leer, como reconocer los registros a los que acceder
		case 0: //SET
		 if (parametros_validos_SET(instruccion.argumentos)) // validamos que los parametros sean correctos y, si lo son, ejecutamos la instruccion.
			ejecutar_SET(instruccion); //podemos usar diccionario para reconocer registros
		 else 
			printf("ERROR, parametros incorrecto");
		 break;
		case 1: //SUM
		if (parametros_validos_SUM(instruccion.argumentos))
		   ejecutar_SUM(instruccion);
		else
			printf("ERROR, parametros incorrecto");
		 break;

		case 2: //SUB
		if (parametros_validos_SUB(instruccion.argumentos)){
			*destino -= *origen;
			}
			else
			no_ejecutar();
			break;

		case 3: //JNZ
	 		if (parametros_validos_JNZ(instruccion.argumentos)){
			//PC = parametros;
			}
			else
			no_ejecutar();
			break;

		case 4: //IO_GEN_SLEEP
		 break;
		default: //tendria que haber un log aca de error, y una interrupcion
		 break;
	}
}
*/
