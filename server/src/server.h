#ifndef SERVER_H_
#define SERVER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include "utils.h"

void iterator(char* value);
void terminar_programa(int conexion_cliente,int conexion_servidor, t_log* logger);
#endif /* SERVER_H_ */
