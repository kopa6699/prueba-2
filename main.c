#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "http_client.h"

int main() {
  // Pedir al usuario que ingrese el tipo de empresa
  char type_of_company[100];
  printf("Ingrese el tipo de empresa: ");
  fgets(type_of_company, sizeof(type_of_company), stdin);
  type_of_company[strlen(type_of_company) - 1] = '\0';

  // Conectarse con el servidor de Bard
  struct http_client *client = http_client_new();
  http_client_set_url(client, "https://api.bard.ai/v1/generate");
  http_client_set_header(client, "Authorization", "Bearer Ap1");
  http_client_set_body(client, "prompt=Escribe la visión, misión, valores y políticas para una empresa de tipo %s", type_of_company);
  int status_code = http_client_send(client);
  if (status_code != 200) {
    fprintf(stderr, "Error al conectarse con el servidor de Bard: %d\n", status_code);
    exit(1);
  }

  // Obtener la respuesta del servidor
  char *response = http_client_get_response(client);
  struct json_object *json = json_object_from_string(response);
  struct json_object *outputs = json_object_get_array_item(json, 0);

  // Imprimir la respuesta al usuario
  for (int i = 0; i < json_object_array_length(outputs); i++) {
    struct json_object *output = json_object_array_get_item(outputs, i);
    struct json_object *choices = json_object_get_array_item(output, 0);
    struct json_object *choice = json_object_array_get_item(choices, 0);

    switch (i) {
      case 0:
        printf("Visión: %s\n", json_object_get_string(choice));
        break;
      case 1:
        printf("Misión: %s\n", json_object_get_string(choice));
        break;
      case 2:
        printf("Valores:\n");
        for (int j = 0; j < json_object_array_length(json_object_get_array_item(output, 1)); j++) {
          struct json_object *value = json_object_array_get_item(json_object_get_array_item(output, 1), j);
          printf("  * %s\n", json_object_get_string(value));
        }
        break;
      case 3:
        printf("Políticas:\n");
        for (int j = 0; j < json_object_array_length(json_object_get_array_item(output, 2)); j++) {
          struct json_object *policy = json_object_array_get_item(json_object_get_array_item(output, 2), j);
          printf("  * %s\n", json_object_get_string(policy));
        }
        break;
    }
  }

  // Liberar recursos
  http_client_free(client);
  free(response);
  json_object_put(json);

  return 0;
}

