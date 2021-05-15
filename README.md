# Práctica 1:  estimación de PI mediante el método de la integración
- Aproximación del valor de PI mediante la integración de 4/(1 + *x*<sup>2</sup>) en el
intervalo [0, 1].
- Se divide el intervalo en N subintervalos de longitud 1/*N*.
- Para cada subintervalo se calcula el área del rectángulo cuya altura es el valor de
4/(1 + *x*<sup>2</sup>) en su punto medio.
- La suma de las áreas de los N rectángulos aproxima el área bajo la curva.
- A mayor N, más precisa la aproximación de PI

## Código secuencial
Código secuencial: `pi.c`
```
#include <stdio.h>
#include <math.h>

int main(int argc, char *argv[])
{
    int i, done = 0, n;
    double PI25DT = 3.141592653589793238462643;
    double pi, h, sum, x;

    while (!done)
    {
        printf("Enter the number of intervals: (0 quits) \n");
        scanf("%d",&n);
    
        if (n == 0) break;
  
        h   = 1.0 / (double) n;
        sum = 0.0;
        for (i = 1; i <= n; i++) {
            x = h * ((double)i - 0.5);
            sum += 4.0 / (1.0 + x*x);
        }
        pi = h * sum;

        printf("pi is approximately %.16f, Error is %.16f\n", pi, fabs(pi - PI25DT));
    }
}
```

## Paralelización
- Implementación SPMD
- La E/S (scanf/printf) la hace el proceso 0
- Distribuir *n* a todos los procesos (con Send/Recv)
- Reparto de la carga de trabajo en el bucle for con “paso” i+=numprocs en lugar de i++
- Recoger estimación de PI de cada proceso (con Send/Recv)

# Práctica 2:  colectivas MPI en la estimación de PI
## Paralelización realizada en la práctica 1 + mejoras práctica 2!
- Implementación SPMD
- La E/S (scanf/printf) la hace el proceso 0
- Distribuir *n* a todos los procesos (con Send/Recv) **Ahora con operación colectiva MPI!**
- Reparto de la carga de trabajo en el bucle for con “paso” i+=numprocs en lugar de i++
- Recoger estimación de PI de cada proceso (con Send/Recv) **Ahora con operación colectiva MPI!**

## Uso de colectivas MPI
- Inicialmente operaciones colectivas estándar de MPI
- Posteriormente introducción de implementación propia de colectiva (con igual cabecera que la
colectiva estándar) para la distribución de *n*, inicialmente utilizando las mismas operaciones
de Send/Recv que en la implementación sin colectivas (bucle for de Sends), implementación que
denominaremos MPI_FlattreeColectiva. No hace falta que devuelva correctamente el entero de error.
- Implementación de colectiva en árbol binomial, implementación que denominaremos
MPI_BinomialColectiva, a utilizar en la distribución de *n*.

Implementación de Bcast con árbol binomial (MPI_BinomialBcast):
- Mismos parámetros que MPI_Bcast (consultar página man de MPI_Bcast
para obtener cabecera), asumiendo por simplicidad que el root es el 0. No
hace falta que devuelva correctamente el entero de error.

- En el paso “i” los procesos con *myrank* < 2<sup>*i*−1</sup> se comunican
con el proceso *myrank* + 2<sup>*i*−1</sup>
