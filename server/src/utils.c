#include "utils.h"

t_log* logger;

int iniciar_servidor(void)
{
	int socket_servidor;

	struct addrinfo hints, *servinfo, *p; //p?

	memset(&hints, 0, sizeof(hints)); //Setea los valores de hints a 0
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(NULL, "4444", &hints, &servinfo); //Syscall para configurar el puntero servinfo con los valores para el socket

	// Creamos el socket de escucha del servidor

	socket_servidor = socket(servinfo->ai_family,
							servinfo->ai_socktype,
							servinfo->ai_protocol);

	// Asociamos el socket a un puerto

	bind(socket_servidor, servinfo->ai_addr,servinfo->ai_addrlen); //Le decimos que se enlace al puerto del socket? address == puerto en este caso?
	
	// Escuchamos las conexiones entrantes

	listen(socket_servidor,SOMAXCONN);

	freeaddrinfo(servinfo);
	log_trace(logger, "Listo para escuchar a mi cliente");

	return socket_servidor;
}

int esperar_cliente(int socket_servidor)
{
	// Aceptamos un nuevo cliente

	int socket_cliente = accept(socket_servidor,NULL,NULL);
	log_info(logger, "Se conecto un cliente!");

	return socket_cliente; //socket_cliente representa la conexión bidireccional entre los sockets cliente y servidor
}

int recibir_operacion(int socket_cliente) //Handshake? Si el codigo enviado no es valido cierra la conexión
{
	int cod_op;
	if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0)
		return cod_op;
	else
	{
		close(socket_cliente);
		return -1;
	}
}

void* recibir_buffer(int* size, int socket_cliente)
{
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL); //Recibe el tamaño del buffer
	buffer = malloc(*size); //Reserva memoria para el buffer 
	recv(socket_cliente, buffer, *size, MSG_WAITALL); //Recive el buffer

	return buffer;
}

void recibir_mensaje(int socket_cliente){
	int size;
	char* buffer = recibir_buffer(&size, socket_cliente);
	log_info(logger, "Me llego el mensaje %s", buffer);
	free(buffer);
}

t_list* recibir_paquete(int socket_cliente)
{
	int size;
	int desplazamiento = 0;
	void * buffer;
	t_list* valores = list_create();
	int tamanio;

	buffer = recibir_buffer(&size, socket_cliente);
	while(desplazamiento < size)
	{
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento+=sizeof(int);
		char* valor = malloc(tamanio);
		//memset
		memcpy(valor, buffer+desplazamiento, tamanio); //Segmentation fault
		desplazamiento+=tamanio;
		list_add(valores, valor);
		//free(valor); Si se libera entonces la lista no muestra valores (porque se libero esa memoria)
	}
	free(buffer);
	return valores;
}
