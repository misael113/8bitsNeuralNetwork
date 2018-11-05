#include <Wire.h>
#include <LSM6.h>
#include <Zumo32U4.h>

Zumo32U4Motors motores;

float punto_control_arreglo[5];                       //con este arreglo guardamos las primeras 5 lecturas del sensor en el metodo setup();
 float punto_control;                                 // calculamos el promedio de las primeras 5 lecturas para definir nuestro punto de control
int  lambda_1, lambda0, lambda1,lambda2;           //umbrales para medir el error


//Los arreglos utilizados como los pesos, deben de ser de tipo entero.

int w[]= {0,0};             //Los pesos de la red, los inicializamos con 100
int w1[]= {0,0};                //con este arreglo actualizamos los pesos de la red

float B,C;                                            //varables para las B-Spline
                       

LSM6 sensor;        //Declaramos el MiniMu-9 V5 con el nombre "sensor"
char report[80];
int vel;     //Es la velocidad que reciben los motores

void setup()
{
  Serial.begin(9600);
  Wire.begin();
 
//La siguiente condicion nos sirve para verificar que el sensor funciona correctamente
  if (!sensor.init())
  {
   
    Serial.println("Ha fallado la conexiòn con el sensor!");
  
  }
  sensor.enableDefault();    //El sensor inicia con su configuracion por defecto, pero podemos cambiar registros si se necesita en el programa
  
  
  
    
  for(int i=0; i <5; i++){
  
  sensor.read();        //iniciamos la lectura de los datos
  float lectura_inicial = (int)sensor.g.z;

   punto_control_arreglo[i]=lectura_inicial;
   

  }
  
   
//Calculamos el promedio de las primeras 5 lecturas, lo guardamos en punto_control; que es nuestro setpoint
punto_control=(const int) ((punto_control_arreglo[0] + punto_control_arreglo[1] + punto_control_arreglo[2] + punto_control_arreglo[3]+punto_control_arreglo[4]+punto_control_arreglo[5])/6);


delay (3000);
}

void loop()
{
  
//Serial.println(punto_control,DEC);
 
  
  
  sensor.read();                //iniciamos las lecturas del sensor
  

 
  /** Con las siguientes lineas de codigo queremos controlar los motores
  del zumo usando los valores entregados por el sensor.
  Asi el zumo puede mantener una direccion sin importar cambios en el ambiente**/




float lectura_sensor = (int)sensor.g.z;                   //Leemos los datos del sensor del eje "z" y los asignamos a la variable lectura_sensor
//Serial.println(lectura_sensor,DEC);


float error = ((punto_control)  - (lectura_sensor)) ;     //El error es la diferencia entre el setpoint y el valor que se recibe del sensor
//Serial.println(error,DEC);


float err = error/1000;           //Escalamiento del error
//Serial.println(err,DEC);
int a,b,c,d,e;
 
//Serial.println(err,DEC);   //sirve para ver el valor del error antes de todo el procesamiento

// el error ingresa a las funciones de activacion B-Spline

//las lambdas son los umbrales donde necesarios para detectar los movimientos y nos sirven para construir las B-Spline
lambda_1 =-30;
lambda0 = -4;
lambda1= 4;
lambda2 = 30;


//Si el error se encuentra en el umbral entonces se activa la funcion
if (err >= lambda_1 && err < lambda0){
  b=1;
} else{
  b=0;
}

if (err >= lambda0 && err < lambda1){
  c=1;
}else{
  c=0;
} 
if (err >= lambda1 && err <= lambda2){
  d=1;
}else{
  d=0;
}


//Obtenemos un vector de 2 componentes, son dos splines de segundo orden

B = b + ((lambda1 - err)/(lambda1 - lambda0))*c;
C = ((err - lambda0)/(lambda1 - lambda0))*c + d;

//las siguientes lineas nos permiten analizar el comportamiento de las funciones base
/**
snprintf(report, sizeof(report), "B: %6d   C:%6d",
    B,C);
    
  Serial.println(report);
**/



//Serial.println(C);

//calcular la norma del vector que devuelven las B-Spline
float Y= (B*B) + (C*C) ;
float norma = (sqrt(Y)*1000);
//Serial.println(norma,DEC);



//dividir el error entre la norma
float division1= (error) / (norma);
//Serial.println((division1)/2,DEC);


// el siguiente producto recibe la norma, el vector de la b-splines y un delta
float delta = 0.5;
/** Cuando delta tiene un valor de 1, salida de la red neuronal que puede regresar a 0, es decir, tenemos inestabilidad
 *  Cuando delta tiene un valor de 1.5, tambien tenemos inestabilidad en la red neuronal
**/


//int producto = division1*delta;

//producto1 es un vector con 2 componentes

float producto1b= division1*B*delta;     //imprime 0 sin movimiento. Con movimiento imprime valores negativos
float producto1c= division1*C*delta;

//Serial.println(producto1c,DEC);



//el siguiente vector tiene 4 componentes y los 4 se inicializan con 100

//proceso de aprendizaje de la red neuronal

/**Debido a que en la memoria solo se guardan numeros enteros, los valores pequeños que corresponden al procesamiento
 * de los movimientos suaves, se guardan como 0, esto implica que perdemos sensibilidad
**/
w1[0]=w[0] + producto1b;
w1[1]=w[1] + producto1c;

//Las siguientes instrucciones son para analizar el comportamiento de la memoria
/**
snprintf(report, sizeof(report), "w1[0]: %6d   w1[1]:%6d",
    w1[0],w1[1]);
    
  Serial.println(report);
//Serial.println(w1[1]);
**/

//el siguiente paso es calcular el producto punto
float   producto_punto =   ((B)*(w1[0])) + ((C)*(w1[1])) ;

//con las siguientes condiciones acotamos el valor final devuelto por la red
//los limites en las acotaciones dependen de la velocidad inicial de los motores

if(producto_punto>100){
  producto_punto =100;
}
if (producto_punto < -100){
  producto_punto =-100;
}
Serial.println(producto_punto,DEC);

//actualizacion de los pesos  
w[0]=w1[0];
w[1]=w1[1];



  

  vel = 100 + (producto_punto);
  motores.setLeftSpeed((int) vel );

  vel = 100 - (producto_punto);
  motores.setRightSpeed ((int) vel);
 
  

}
