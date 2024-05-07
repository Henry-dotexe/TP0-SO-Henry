#include "client.h"
int main(void)
{
	/*---------------------------------------------------PARTE 2-------------------------------------------------------------*/

	int socket_server;
	char* ip;
	char* puerto;
	int valor; //clave
	int opc; //Opcion de menu

	t_log* logger;
	t_config* config;

	/* ---------------- LOGGING ---------------- */

	logger = iniciar_logger();

	/* ---------------- ARCHIVOS DE CONFIGURACION ---------------- */

	config = iniciar_config();
	if(config == NULL){
		log_info(logger,"No se pudo crear el config!");
		return 0;
	}

	/*---------------------------------------------------PARTE 3-------------------------------------------------------------*/

	// ADVERTENCIA: Antes de continuar, tenemos que asegurarnos que el servidor esté corriendo para poder conectarnos a él

	// Creamos una conexión hacia el servidor

	socket_server = crear_conexion("127.0.0.1","4444");
	
	printf("\tMenu de TP0\n\n[0]Prueba de Logger\n[1]Obtener valores config\n[2]Leer por consola y loggear\n[3]Handshake (Requiere valores de config)\n[4]Enviar paquete\n[5]Salir\n\n");

	printf("Por favor introduzca una opcion [ ]\b\b");
	scanf("%d",&opc);

	while(opc!=5){
		switch(opc){
			case 0://Prueba de logger
				log_info(logger, "Hola! Soy un log");
				break;
			case 1: //Obtener valores de config
				// Usando el config creado previamente, leemos los valores del config y los dejamos en las variables 'ip', 'puerto' y 'valor'
				ip = config_get_string_value(config, "IP");
				puerto = config_get_string_value(config, "PUERTO");
				valor = config_get_int_value(config, "CLAVE");
				// Loggeamos el valor de config
				log_info(logger, "La clave es %d",valor);
				break;
			case 2: //Leer por consola y loggear
				leer_consola(logger);
				break;
			case 3: //Handshake
				if(handshake(socket_server,valor) == 1){
					log_info(logger, "El servidor me reconoce como cliente!");
				}else{
					log_info(logger,"El servidor no me reconoce! >:(");
				}
				break;
			case 4: //Envio de paquete
				paquete(socket_server); //Registra las lineas ingresadas, arma y envia paquete
				break;
			case 5: //Liberar punteros y salir
				terminar_programa(socket_server, logger, config);
				printf("Cerrando programa...");
				break;
		}
		printf("\n\t~Menu de TP0~\n\n[0] Prueba de Logger\n[1] Obtener valores config\n[2] Leer por consola y loggear\n[3] Handshake (Requiere valores de config)\n[4] Enviar paquete\n[5] Salir\n\n");
		printf("Por favor introduzca una opcion [ ]\b\b");
		scanf("%d",&opc);
	}

	/*---------------------------------------------------PARTE 5-------------------------------------------------------------*/
	// Proximamente
	return 0;
}

t_log* iniciar_logger(void)
{
	t_log* nuevo_logger;

	nuevo_logger = log_create("../tp0.log", "H-Logger", true, LOG_LEVEL_INFO);

	return nuevo_logger;
}

t_config* iniciar_config(void)
{
	t_config* nuevo_config;

	nuevo_config = config_create("../cliente.config");

	return nuevo_config;
}

void leer_consola(t_log* logger)
{
	char* leido;

	// La primera te la dejo de yapa
	leido = readline("> ");

	// El resto, las vamos leyendo y logueando hasta recibir un string vacío
	while(strcmp(leido,"")!=0){
		log_info(logger,leido);
		leido = readline("> ");
	}
	
	// ¡No te olvides de liberar las lineas antes de regresar!
	free(leido); //Se liberaría? Pero no apunta a memoria dinámica
}

int handshake(int socket_server,int valor){
	int respuesta;
	int bytes_enviados = send(socket_server,&valor,sizeof(valor),0); //Mando clave

	recv(socket_server,&respuesta,sizeof(respuesta),MSG_WAITALL); //Recibo respuesta

	return respuesta;
}

void paquete(int conexion)
{
	// Ahora toca lo divertido!
	char* leido;
	t_paquete* paquete = crear_paquete(); //Reserva memoria para el paquete, asigna codigo PAQUETE y crea el buffer
	
	// Leemos y esta vez agregamos las lineas al paquete
	printf("Introduce las lineas que se van a enviar\n");
	leido = readline("> ");
	
	while(strcmp(leido,"")!=0){

		agregar_a_paquete(paquete,leido,strlen(leido));
		//agregar_a_paquete recalcula el size y agrega la linea al stream
		leido = readline("> ");
	}

	printf("Lineas empaquetadas\nEnviando paquete...\n");
	enviar_paquete(paquete,conexion);
	printf("Paquete enviado!\n");
	// ¡No te olvides de liberar las líneas y el paquete antes de regresar!
	
	//free(leido); No se libera la linea porque no usa memoria dinamica
	eliminar_paquete(paquete);
}

void terminar_programa(int conexion, t_log* logger, t_config* config)
{
	/* Y por ultimo, hay que liberar lo que utilizamos (conexion, log y config) 
	  con las funciones de las commons y del TP mencionadas en el enunciado */
	  log_destroy(logger);
	  config_destroy(config);
	  liberar_conexion(conexion);
}
