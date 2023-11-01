# Simulación del tráfico de un cruce mediante programación concurrente en el entorno Windows

![cruce](https://github.com/rmelgo/SSOO-II-Programacion-multiproceso-cruce-semaforos/assets/145989723/f9c4ab82-5e85-4717-b436-1fd395f90f66)

# - Introducción

Proyecto realizado en la asignatura de ***SSOO II*** del grado de Ingenieria Informática de la Universidad de Salamanca. El enunciado del proyecto se encuentra en el siguiente enlace:  

http://avellano.usal.es/~labssoo/pract309.htm
  
El principal objetivo de este proyecto es la realización de un programa en C que simule el tráfico de coches y peatones en un cruce donde cada peatón y cada coche será simulado por un proceso.
De esta manera, el objetivo es establecer una correcta sincronización y comunicación entre los distintos procesos para que se cumplan las normas de tráfico en el cruce.
Para ello, se utilizarán distintos mecanismos IPC de comunicación entre procesos como los semáforos, los buzones o la memoria compartida.

# - Comentarios sobre el entorno de ejecución

Para ejecutar este programa, se requerirá de una distribución del Sistema Operativo **Windows**.    

Para poder compilar correctamente el programa, se deberá tener instalada una versión del compilador gcc o similar.  

Se recomienda utilizar alguna herramienta como ***Dev-C++*** o ***Visual Studio*** para la correcta compilación del programa, ya que estas herramientas cuentas con el compilador gcc. Es importante utilizar un compilador de 32 bits, por que en caso contrario el programa no podrá ser ejecutado correctamente. 

Para ejecutar el programa es imprescindible tener una ventana de terminal tenga un tamaño de 80x25 caracteres. En caso contrario, debe cambiarse en el menú de propiedades de la ventana.

# - Parámetros de ejecución

Para ejecutar el programa es necesario proporcionar 2 argumentos. 

El primer argumento se trata del **número máximo de procesos** (peatones y coches) que pueden existir de manera simultanea en el cruce. Este primer argumento puede tomar valores del ***3 al 49***, en caso contrario la práctica no funcionará.

El segundo argumento hace referencia a la **velocidad** con la que se desplazaran los peatones y los coches. Este argumento podrá tomar valores del ***0 al 40*** de forma que la velocidad del programa será mas rápida cuanto mas pequeño sea el número introducido.  
Si el segundo argumento es 0, el programa se ejecutará a ***máxima velocidad***, lo que implicará un alto consumo de CPU.

Si los parámetros introducidos no respetan las reglas anteriores, el programa lo detectará, informará al usuario y acabará.

# Comentarios sobre el material adjuntado

El proyecto cuenta con los siguientes ficheros:

- Un fichero llamado ***cruce2.cpp*** que contendrá el código necesario para poner en marcha la infraestructura del cruce, leer y interpretar los argumentos introducidos y crear los peatones y coches correspondientes.
- Un fichero llamado ***cruce2.dll*** el cual se trata de una biblioteca de enlazado dinámico que contendrá una serie de funciones para gestionar la creación y finalización del cruce así como la gestión del valor de los semáforos, creación y movimiento de peatones y coches.
- Un fichero llamado ***cruce2.h*** que contiene las cabeceras de las funciones definidas en la biblioteca *libcruce.a* así como una serie de macros que utiliza la biblioteca.

Adicionalmente se incluirá un fichero ***cruce2.exe*** con el programa ya compilado, por si existen problemas a la hora de generar el ejecutable del programa.

# - Funciones de la biblioteca de enlazado dinámico ***cruce2.dll***

La biblioteca estática de funciones ***cruce2.cpp*** cuenta con las siguientes funciones:

- **int CRUCE_gestor_inicio(void)**: Esta función deberá ser ejecutada por el proceso principal al principio del código.

- **int CRUCE_inicio(int ret, int maxProcs)**: Esta función deberá ser ejecutada por el proceso principal despues de crear los mecanismos IPC correspondientes y antes de haber creado algún peatón o coche. Esta función recibe los siguientes parámetros:
  
  - **ret**: Número entero que hace referencia a la velocidad de representación de los peatones y coches en el cruce (segundo argumento pasado en la línea de órdenes).
  - **maxProcs**: Número entero que hace referencia al número máximo de procesos (peatones y coches) que pueden existir de manera simultanea en el cruce (primer argumento pasado en la línea de órdenes).

- **int CRUCE_pon_semAforo(int sem, int color)**: Esta función pone el semáforo pasado en el primer parámetro al color indicado en el segundo parámetro. Esta función recibe los siguientes parámetros:

  - **sem**: Número entero que hace referencia a un semáforo del cruce. Este parámetro puede tomar los valores *SEM_P1*, *SEM_P2*, *SEM_C1*, *SEM_C2* dependiendo del semáforo al que se desea cambiar el color.
  - **color**: Número entero que hace referencia al color al que se desea cambiar el semáforo pasado como primer parámetro. Este parámetro puede tomar los valores *ROJO*, *AMARILLO* (solamente para los semáforos de los coches) y *VERDE*.
 
Nota: *SEM_P1*, *SEM_P2*, *SEM_C1*, *SEM_C2*, *ROJO*, *AMARILLO* y *VERDE* son macros definidas en ***cruce.h***.

- **int CRUCE_nuevo_proceso(void)**: Esta función crea un nuevo proceso que puede ser un coche o un peatón. Esta función devuelve *COCHE* en el caso de que el proceso creado se trate de un coche y *PEAToN* en el caso de que el proceso creado se trate de un peatón.

Nota: *COCHE* y *PEAToN* son macros definidas en ***cruce2.h***.

- **struct posiciOn CRUCE_inicio_coche(void)**: Cada vez que se crea un nuevo proceso de tipo *COCHE*, debe llamar a esta función. Esta función devuelve las coordenadas de la posición siguiente que ocupará el coche a través de un tipo de datos llamado *posiciOn*.

Nota: *posiciOn* es un tipo de datos definido en ***cruce2.h*** y cuenta con dos enteros que hacen referencia a la coordenada x y a la coordenada y.

- **struct posiciOn CRUCE_nuevo_inicio_peatOn(void)**: Cada vez que se crea un nuevo proceso de tipo *PEAToN*, debe llamar a esta función. Esta función devuelve las coordenadas de la posición siguiente que ocupará el peatón a través de un tipo de datos llamado *posiciOn*.

- **struct posiciOn CRUCE_avanzar_coche(struct posiciOn sgte)**: Esta función debe ser llamada por cada coche después de haber ejecutado ***CRUCE_inicio_coche***. Esta función recibe como parámetro la posición a la que desea ir el coche y devuelve la nueva posición siguiente que ocupará el coche.

- **struct posiciOn CRUCE_avanzar_peatOn(struct posiciOn sgte)**: Esta función debe ser llamada por cada peatón después de haber ejecutado ***CRUCE_inicio_peatOn_ext***. Esta función recibe como parámetro la posición a la que desea ir el peatón y devuelve la nueva posición siguiente que ocupará el peatón.

- **int pausa_coche(void)**: Esta función debe ser llamada por cada coche cada vez que se haya producido un avance mediante la función ***CRUCE_avanzar_coche***.

- **int pausa(void)**: Esta función debe ser llamada por cada peatón cada vez que se haya producido un avance mediante la función ***CRUCE_avanzar_peatOn***.

- **int CRUCE_fin_coche(void)**: Cuando un coche tenga en la posición siguiente un valor negativo para la coordenada y, deberá llamar a esta función para finalizar el proceso coche de manera correcta.

- **int CRUCE_fin_peatOn(void)**: Cuando un peatón tenga en la posición siguiente un valor negativo para la coordenada y, deberá llamar a esta función para finalizar el proceso peatón de manera correcta.

- **int CRUCE_fin(void)**: Cuando la ejecución del cruce haya finalizado, el proceso principal debe ejecutar esta función. Esta función se encargará de finalizar la representación del cruce y limpiar todos los procesos y mecanismos IPC que estuvieran en uso.

- **void pon_error(char *mensaje)**: Esta función coloca el mensaje pasado como parámetro en un recuadro y espera a que el usuario pulse *Intro* para seguir con la ejecución del programa.

# - Ciclo semafórico

Como se puede observar en la siguiente imagen, el cruce cuenta con 4 semáforos, 2 para coches y 2 para peatones.

![esquemacruce](https://github.com/rmelgo/SSOO-II-Programacion-multiproceso-cruce-semaforos/assets/145989723/75191847-4f28-4036-8468-459646b98f73)

De esta manera, el ciclo semafórico del cruce estará compuesto por 3 fases:

- **Fase 1**: El semáforo *C1* y *P2* se encontrarán en verde mientras que el resto de semáforos se encontrarán en rojo. La duración de esta fase será de *6 pausas* (una pausa equivale a una llamada a la función *pausa*).
- **Fase 2**: El semáforo *C2* se encontrará en verde mientras que el resto de semáforos se encontrarán en rojo. La duración de esta fase será de *8 pausas*.
- **Fase 3**: El semáforo *P1* se encontrará en verde mientras que el resto de semáforos se encontrarán en rojo. La duración de esta fase será de *12 pausas*.

Para cambiar de una fase a otra, existirá una pequeña fase de transición donde el color de los semáforos de los coches se encontrará en amarillo. Esta fase de transición durará en principio 2 pausas aunque esta fase puede extenderse hasta que los cruces se encuentren libres para así evitar posibles colisiones entre peatones y coches. 
  
# - Pasos necesarios para ejecutar el programa

**Paso 1: Compilar el programa**  

El entorno de desarrollo que se utilice se encargará de compilar el programa utilizando los comandos adecuados.

**Paso 2: Ejecutar el programa**  

Para ello, se debe introducir el siguiente comando:    

```cruce2.exe <número-máximo-procesos> <velocidad>```

Tras ejecutar este comando, el programa se habra ejecutado correctamente, siempre y cuando se hayan introducido los argumentos correspondientes.

**Finalización del programa**

Para finalizar la ejecución del programa simplemente bastara con pulsar las teclas *CTRL+C* desde el terminal. De esta manera, el programa automaticamente recibirá una señal de terminación por lo que realizará las tareas necsarias para finalizar el cruce y liberar todos los recursos utilizados.

# - Ejemplo de ejecución

## Ejecución a velocidad media (10) y máximo de 4 procesos

En la siguiente imagen, se muestra un ejemplo del uso y funcionamiento del programa con un límite de 4 procesos y una velocidad de 10:    

![Ejemplo ejecucion 1](https://github.com/rmelgo/SSOO-II-Programacion-multiproceso-cruce-semaforos_Windows/assets/145989723/b0e5ffc9-83c0-47f5-b3bb-049f28dd45ae)

## Ejecución a velocidad máxima y máximo de 49 procesos

En la siguiente imagen, se muestra un ejemplo del uso y funcionamiento del programa con un límite de 49 procesos y una velocidad de 0:    

![Ejemplo ejecucion 2](https://github.com/rmelgo/SSOO-II-Programacion-multiproceso-cruce-semaforos_Windows/assets/145989723/af5850d5-b379-45b4-8319-7817548a84f0)

# - Participantes

<table>
  <td align="center"><a href="https://github.com/rmelgo"><img src="https://avatars.githubusercontent.com/u/145989723?s=400&u=e5c06adba3f3c418207178abc845d398b3d5f77f&v=4" width="100px;" alt=""/><br /><sub><b>Raúl Melgosa</b></sub></a><br/> 
  <td align="center"><img src="https://avatars.githubusercontent.com/u/84237179?v=4" width="100px;" alt=""/><br /><sub><b>Juan Carlos Velasco</b></sub></a><br/> 
</table>
