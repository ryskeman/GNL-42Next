*Este proyecto ha sido creado como parte del currículo de 42 por fernfern.*

## Descripción
`get_next_line` es un proyecto del cursus de 42 cuyo objetivo es implementar una función en C capaz de leer un archivo o la entrada estándar (`stdin`) línea a línea mediante sucesivas llamadas.

La función extrae y retorna la siguiente línea disponible terminada en `\n` (excepto si se alcanza el final del archivo sin dicho carácter) y devuelve `NULL` cuando la lectura finaliza o si ocurre algún error de descriptor o reserva de memoria. Para mantener el seguimiento entre llamadas sucesivas sin variables globales, se utiliza memoria estática.

## Instrucciones

### Requisitos previos
- Compilador de C (`gcc` o `clang`).
- Entorno POSIX (Linux o macOS).

### Compilación
El proyecto no requiere Makefile. Se compila directamente junto con el archivo que contenga la función `main` y especificando el tamaño del búfer de lectura con la flag `-D BUFFER_SIZE=n`:

**Parte obligatoria:**
```bash
cc -Wall -Werror -Wextra -D BUFFER_SIZE=42 get_next_line.c get_next_line_utils.c main.c -o gnl
```

**Parte bonus (soporte multidescriptor):**
```bash
cc -Wall -Werror -Wextra -D BUFFER_SIZE=42 get_next_line_bonus.c get_next_line_utils_bonus.c main.c -o gnl_bonus
```

Si no se proporciona la bandera -D BUFFER_SIZE, se aplicará el valor por defecto definido en el header (42).

### Ejemplo de uso

```C
#include "get_next_line.h"
#include <fcntl.h>
#include <stdio.h>

int	main(void)
{
	int		fd;
	char	*line;

	fd = open("test.txt", O_RDONLY);
	if (fd < 0)
		return (1);
	line = get_next_line(fd);
	while (line)
	{
		printf("%s", line);
		free(line);
		line = get_next_line(fd);
	}
	close(fd);
	return (0);
}
```

## Explicación y Justificación del Algoritmo

**Problema de complejidad $O(N^2)$ en aproximaciones clásicas**

La implementación típica de `get_next_line` concatena en cada ciclo de read el búfer temporal con el acumulador estático mediante strjoin. Para líneas largas con un `BUFFER_SIZE` reducido (por ejemplo, 20.000 caracteres con `BUFFER_SIZE=10`), esto obliga a recalcular longitudes y realocar memoria miles de veces, disparando la complejidad a $O(N^2)$ y provocando timeouts en testers automáticos.

**Solución adoptada:** Búfer de crecimiento geométrico amortizado $O(N)$ Para resolver este cuello de botella y garantizar un consumo mínimo de operaciones:

1. Chequeo previo: Antes de llamar a `read()`, se comprueba si el residuo estático ya contiene un `\n`. De ser así, se evita interactuar con el sistema de archivos.

2. Crecimiento geométrico de capacidad: Durante el proceso de lectura, el búfer acumulador no se incrementa de $B$ en $B$ bytes. En su lugar, duplica su capacidad disponible cuando el espacio se agota. Esto reduce el número de llamadas a `malloc()` de miles a un orden logarítmico ($O(\log N)$).

3. Búsqueda focalizada del salto de línea: El chequeo de `\n` se realiza exclusivamente sobre los bytes recién leídos `(tmp)`, eliminando inspecciones completas y redundantes del acumulador en cada ciclo.

4. Extracción y limpieza atómica: Una vez localizado el `\n` o detectado `EOF (read = 0)`, la subcadena de la línea se extrae con `ft_substr`, el remanente se guarda en la variable estática y la memoria no utilizada se libera. Si cualquier asignación intermedia falla, todos los punteros asociados se liberan y se setean a `NULL`, garantizando cero fugas de memoria (leaks) y previniendo dobles liberaciones (double free).

## Recursos
### Referencias técnicas

**Manuales POSIX:** read(2), malloc(3), free(3).

**Estándar ISO C:** Manejo de punteros, memoria dinámica en Heap y variables con especificador de almacenamiento static (segmento .bss / .data).

**Framework de tests:** Francinette y tests de integración con inyección de fallos de memoria (fsoares).

### Declaración de uso de IA

Se utilizó asistencia de Inteligencia Artificial (IA) en las siguientes fases del desarrollo:

**Análisis de profiling y diagnóstico de fallos:** Localización de cuellos de botella temporales que provocaban Timeout en `giant_line.txt` bajo simulación estricta de fallos de memoria.

**Detección de dobles liberaciones (double free):** Depuración del flujo de errores para asegurar la desvinculación segura de punteros intermedios durante abortos en cascada de malloc.

**Refactorización de funciones:** Ajuste estructural para cumplir estrictamente con los límites de 25 líneas por función de la Norminette sin fragmentar la lógica de gestión de errores.