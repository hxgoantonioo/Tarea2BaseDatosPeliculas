#include "tdas/extra.h"
#include "tdas/list.h"
#include "tdas/map.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
  char id[100];
  char title[100];
  List *genres;
  char director[300];
  float rating;
  int year;
} Film;

// Menú principal
void mostrarMenuPrincipal() {
  limpiarPantalla();
  puts("========================================");
  puts("     Base de Datos de Películas");
  puts("========================================");

  puts("1) Cargar Películas");
  puts("2) Buscar por id");
  puts("3) Buscar por género");
  puts("4) Buscar por director");
  puts("5) Buscar por década");
  puts("6) Buscar por rango de calificaciones");
  puts("7) Buscar por década y género");
  puts("8) Salir");
}

/**
 * Compara dos claves de tipo string para determinar si son iguales.
 * Esta función se utiliza para inicializar mapas con claves de tipo string.
 *
 * @param key1 Primer puntero a la clave string.
 * @param key2 Segundo puntero a la clave string.
 * @return Retorna 1 si las claves son iguales, 0 de lo contrario.
 */
int is_equal_str(void *key1, void *key2) {
  return strcmp((char *)key1, (char *)key2) == 0;
}

/**
 * Compara dos claves de tipo entero para determinar si son iguales.
 * Esta función se utiliza para inicializar mapas con claves de tipo entero.
 *
 * @param key1 Primer puntero a la clave entera.
 * @param key2 Segundo puntero a la clave entera.
 * @return Retorna 1 si las claves son iguales, 0 de lo contrario.
 */
int is_equal_int(void *key1, void *key2) {
  return *(int *)key1 == *(int *)key2; // Compara valores enteros directamente
}


/*Funcion que toma un string, los divide en substrings basados en el delimitador  y los guarda en una lista */
List* strings_list(const char *str, const char *delim){
  List *lista = list_create(); //lista que almacena las subscadenas
  char *token = strtok((char *)str, delim);

  while(token != NULL){
    while(*token == ' '){
      token++;
    }
    //Eliminar espacios en blanco
    char *end = token + strlen(token) - 1;
    while(*end == ' ' && end > token){
      *end = '\0';
      end--;
    }
    char *new_token = strdup(token); //copia el token a un nuevo string
    list_pushBack(lista, new_token); //agrega el nuevo string a la lista
    token = strtok(NULL, delim);
  }
  return lista;
}


/**
 * Carga películas desde un archivo CSV y las almacena en un mapa por ID.
 */
void cargar_peliculas(Map *pelis_byid) {
  // Intenta abrir el archivo CSV que contiene datos de películas
  FILE *archivo = fopen("data/Top1500.csv", "r");
  if (archivo == NULL) {
    perror(
        "Error al abrir el archivo"); // Informa si el archivo no puede abrirse
    return;
  }

  char **campos;
  // Leer y parsear una línea del archivo CSV. La función devuelve un array de
  // strings, donde cada elemento representa un campo de la línea CSV procesada.
  campos = leer_linea_csv(archivo, ','); // Lee los encabezados del CSV

  // Lee cada línea del archivo CSV hasta el final
  while ((campos = leer_linea_csv(archivo, ',')) != NULL) {
    // Crea una nueva estructura Film y almacena los datos de cada película
    Film *peli = (Film *)malloc(sizeof(Film));
    strcpy(peli->id, campos[1]);        // Asigna ID
    strcpy(peli->title, campos[5]);     // Asigna título
    strcpy(peli->director, campos[14]); // Asigna director
    peli->genres = list_create();       // Inicializa la lista de géneros
    peli->year = atoi(campos[10]);      // Asigna año, convirtiendo de cadena a entero
    peli->rating = atof(campos[8]);     // Asigna rating, convirtiendo de cadena a real

    // Inserta la película en el mapa usando el ID como clave
    map_insert(pelis_byid, peli->id, peli);
  }
  fclose(archivo); // Cierra el archivo después de leer todas las líneas


  // Itera sobre el mapa para mostrar las películas cargadas
  MapPair *pair = map_first(pelis_byid);
  while (pair != NULL) {
    Film *peli = pair->value;
    printf("ID: %s, Título: %s, Director: %s, Año: %d\n", peli->id, peli->title, peli->director, peli->year);
    pair = map_next(pelis_byid); // Avanza al siguiente par en el mapa
  }
}

/**
 * Busca y muestra la información de una película por su ID en un mapa.
 */
void buscar_por_id(Map *pelis_byid) {
  if (map_first(pelis_byid) == NULL)
  {
    printf("No hay peliculas registradas.\n");
    return;
  }
  char id[10]; // Buffer para almacenar el ID de la película

  // Solicita al usuario el ID de la película
  printf("Ingrese el id de la película: ");
  scanf("%s", id); // Lee el ID del teclado

  // Busca el par clave-valor en el mapa usando el ID proporcionado
  MapPair *pair = map_search(pelis_byid, id);

  // Si se encontró el par clave-valor, se extrae y muestra la información de la
  // película
  if (pair != NULL) {
    Film *peli = pair->value; // Obtiene el puntero a la estructura de la película
    // Muestra el título y el año de la película
    printf("Título: %s, Año: %d\n", peli->title, peli->year);
  } else {
    // Si no se encuentra la película, informa al usuario
    printf("La película con id %s no existe\n", id);
  }
}

void buscar_genero(Map *pelis_byid){
}

/* 
* Esta funcion buscar por nombre o apellido a los directores de las peliculas registradas en el mapa
* @param pelis_byid: mapa que contiene los datos de las peliculas registrados
* @param dir: nombre o apellido a buscar
*/
void buscar_director(Map *pelis_byid){
  if (map_first(pelis_byid) == NULL)
  {
    printf("No hay peliculas registradas.\n");
    return;
  }
  char dir[100];
  printf("Ingrese el nombre del director: ");
  scanf("%s", dir);
  MapPair *pair = map_first(pelis_byid);
  printf("\nResultado de busqueda:\n");
  while(pair != NULL){
    Film *peli = pair->value;
    /*sin un strdip se modificaria el char original, imprimiendo en "cargar peliculas" el primer token.*/
    char *directores = strdup(peli->director); // Copiar la cadena original para no  modificarla con el token
    //Tokeniza la cadena de nombres y apellidos.
    char *token = strtok(directores, ", ");
    while(token != NULL){
      if(strcmp(token, dir) == 0){
        printf("Título: %s, Año: %d, Director: %s\n\n", peli->title, peli->year, peli->director);
      }
      token = strtok(NULL, ", "); 
    }
    free(directores); //liberar la memoria de la copia
    pair = map_next(pelis_byid);
      
  }
}

//Esta funcion recibe un mapa de peliculas y imprime todas las peliculas que hayan salido en una decada dada por el usuario
void buscar_por_decada(Map *pelis_byid)
{
  if (map_first(pelis_byid) == NULL)
  {
    printf("No hay peliculas registradas.\n");
    return;
  }
  //Variable de tipo entero que almacenara la decad
  int decada;
  //Se lee la decada ingresada por el usuario
  printf("Ingresar decada buscada: ");
  scanf("%d", &decada);
  //Se crea un puntero de tipo MapPair que almacenara el primer par del mapa
  MapPair *pair = map_first(pelis_byid);
  //Se inicia un contador con las peliculas que pertenecen a la decada ingresada
  int contadorPeliculas = 0;
  //Se inicia un iterador para recorrer el mapa
  while (pair != NULL)
    {
      //Se crea un puntero de tipo Film que almacenara el valor del par actual
      Film *peli = pair->value;
      // Si el año de la pelicula es mayor o igual de la decada ingresada por el usuario y menor o igual a la decada mas 9 (que representa el ultimo año de la decada) pues se imprimiran los datos de la pelicula y aumentaran el contador de peliculas
      if(peli->year >= decada && peli->year <= decada + 9)
      {
        contadorPeliculas++;
        printf("Titulo: %s, Año: %d\n", peli->title, peli->year);
      }
      //Se avanza al siguiente par del mapa
      pair = map_next(pelis_byid);
    }
  //Si no se encuentran peliculas en la decada ingresada por el usuario, se imprime un mensaje correspondiente para dar a entender al usuario que no se encontro ninguna pelicula
  if (contadorPeliculas == 0)
  {
    printf("No se encontraron peliculas en esa decada\n");
  }
}

// Esta funcion recibe un mapa de peliculas y imprime todas las peliculas que hay en un rango predeterminado de calificaciones
void buscar_por_rango_calificaciones(Map *pelis_byid)
{
  if (map_first(pelis_byid) == NULL)
  {
    printf("No hay peliculas registradas.\n");
    return;
  }
  // Se crea tres variables, una de tipo int para guardar la opcion que el usuario desee, y dos de tipo float que seran usadas para almacenar el minimo y el maximo de la opcion ingresada por el usuario, asi tambien, se imprimen las opciones disponibles y se lee la opcion ingresada por el usuario con el scanf
  int opcion;
  float min, max;
  printf("\nLista de rangos de calificaciones \n");
  printf("1) 9.0 - 9.4\n");
  printf("2) 8.5 - 8.9\n");
  printf("3) 8.0 - 8.4\n");
  printf("4) 7.5 - 7.9\n");
  printf("5) 7.0 - 7.4\n");
  printf("6) 6.5 - 6.9\n");
  printf("7) 6.0 - 6.4\n");
  printf("8) 5.5 - 5.9\n");
  printf("9) 5.0 - 5.4\n");
  printf("Ingrese una opcion: ");
  scanf(" %d", &opcion);
  // Si por algun error, la opcion ingresada no esta dentro de la lista, se muestra un mensaje de opcion invalida y finaliza la funcion
  if (opcion > 9 || opcion < 1)
  {
    printf("Opcion invalida\n");
    return;
  }
  // Se crea un switch para evaluar la opcion ingresada por el usuario, y asi determinar el valor de las variables minimo y maximo
  switch (opcion)
    {
      case 1:
      min = 9.0;
      max = 9.4;
      break;
      case 2:
      min = 8.5;
      max = 8.9;
      break;
      case 3:
      min = 8.0;
      max = 8.4;
      break;
      case 4:
      min = 7.5;
      max = 7.9;
      break;
      case 5:
      min = 7.0;
      max = 7.4;
      break;
      case 6:
      min = 6.5;
      max = 6.9;
      break;
      case 7:
      min = 6.0;
      max = 6.4;
      break;
      case 8:
      min = 5.5;
      max = 5.9;
      break;
      case 9:
      min = 5.0;
      max = 5.4;
      break;
    }
  // Se crea un puntero de tipo MapPair que almacenara el primer par del mapa
  MapPair *pair = map_first(pelis_byid);
  // Se inicia un iterador para recorrer el mapa
  while (pair != NULL)
    {
      // Se crea un puntero de tipo Film que almacenara el valor del par actual
      Film *peli = pair->value;
      // Si la calificacion de la pelicula del par donde se ubica el puntero, esta dentro del rango de calificaciones ingresado por el usuario, se imprimira la pelicula, con su año y rating correspondiente
      if (peli->rating >= min && peli->rating <= max)
      {
        printf("Titulo: %s, Año: %d, Rating: %.1f\n", peli->title, peli->year, peli->rating);
      }
      // Se avanza al siguiente par del mapa
      pair = map_next(pelis_byid);
    }
}

void buscar_por_decada_genero(Map *pelis_byid)
{
  if (map_first(pelis_byid) == NULL)
  {
    printf("No hay peliculas registradas.\n");
  }
}

int main() {
  char opcion; // Variable para almacenar una opción ingresada por el usuario
               // (sin uso en este fragmento)

  // Crea un mapa para almacenar películas, utilizando una función de
  // comparación que trabaja con claves de tipo string.
  Map *pelis_byid = map_create(is_equal_str);

  // Recuerda usar un mapa por criterio de búsqueda

  do {
    mostrarMenuPrincipal();
    printf("Ingrese su opción: ");
    scanf(" %c", &opcion);

    switch (opcion) {
    case '1':
      cargar_peliculas(pelis_byid);
      break;
    case '2':
      buscar_por_id(pelis_byid);
      break;
    case '3':
      buscar_genero(pelis_byid);
      break;
    case '4':
      buscar_director(pelis_byid);
      break;
    case '5':
      buscar_por_decada(pelis_byid);
      break;
    case '6':
      buscar_por_rango_calificaciones(pelis_byid);
      break;
    case '7':
      buscar_por_decada_genero(pelis_byid);
      break;
    }
    presioneTeclaParaContinuar();

  } while (opcion != '8');

  return 0;
}