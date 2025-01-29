/********************
 LIBRERIAS
 ********************/

//Inclusión de librerías necesarias
#include <Servo.h>
#include "IRremote.h"


/****************************
 VARIABLES
 ****************************/

//Definición de variables para el control de los motores
#define ENA 6
#define ENB 3 
#define IN1 7 
#define IN2 5
#define IN3 4
#define IN4 2

//Definición de variables para el control del sensor Ultrasónico
long  cm, duration;
#define ECO 10
#define TRIG 8
#define receiver 11 

//Definición de variables para el control del servo
Servo myservo;
int angulo = 90, Boton;

/*-----( Declare objects )-----*/
IRrecv irrecv(receiver);           // create instance of 'irrecv'
decode_results results;            // create instance of 'decode_results'
 
void setup()
{
//Inicialización de todos los parámetros del sistema (esto solo se ejecuta una vez al inicio)

     //Defino dónde está conectado el servo
     myservo.attach(9);  

     //Definición conexiones para el de los control motores
     pinMode(ENA, OUTPUT);
     pinMode(ENB, OUTPUT);
     pinMode(IN1, OUTPUT);
     pinMode(IN2, OUTPUT);
     pinMode(IN3, OUTPUT);
     pinMode(IN4, OUTPUT);

     // Enable Motor A, Motor B: Constant Speed
     digitalWrite(ENA, HIGH);
     digitalWrite(ENB, HIGH);


     // Serial communication
     Serial.begin(9600);
     
     // Start the receiver
     irrecv.enableIRIn(); 
}
 
void loop()
{
//Rutina que se ejecuta en bucle contínuamente

  //¿Manual o automático?
  while(Boton != 5 && Boton != 6) //El programa se quedará dentro de este bucle while hasta que se defina un modo, consultará cada 100ms si se ha presionado un botón
  {
     delay(100);
     if (irrecv.decode(&results)) 
      {
            translateIR(); 
            irrecv.resume(); 
      } 
  }

  //Si hemos presionado automático, el sistema empieza a funcionar de forma autónoma	
  if(Boton == 5)
  {
	 //Definición de variables para el funcionamiento de esta rutina
         cm = Distancia();

	 //¿Estoy más cerca de 35 cm?**
         if(cm <= 35) //SÍ, pues compruebo los sensores para decidir hacia dónde giro
         { 
         long verif_dist_derecha, verif_dist_izquierda;    //Defino unas variables para guardar las distancias de los objetos a izq. y dcha.
             MotorAB_Brake(1000);                          //Paro el coche 
             for (angulo = 0; angulo <= 180; angulo++) {   //Muevo el servo para enfocar el sensor a la izq.
                   myservo.write(angulo);             
                   delay(15);                       
             }
             verif_dist_izquierda = Distancia();	   //Leo distancia y lo guardo en la variable izq.
             for (angulo = 180; angulo >= 0; angulo--) {   //Muevo el servo para enfocar el sensor a la dcha.
                  myservo.write(angulo);              
                  delay(15);                       
             }
             verif_dist_derecha = Distancia();		   //Leo distancia y lo guardo en la variable dcha.
             myservo.write(90);                            //Recoloco sensor mirando al frente
             delay(200);

             //¿Dónde tengo más espacio?
             if(verif_dist_derecha > verif_dist_izquierda) //Como tengo más espacio a la derecha, giro el coche hacia la derecha
             {
                   digitalWrite(IN1, LOW); // Girar derecha
                   digitalWrite(IN2, HIGH);
                   digitalWrite(IN3, HIGH);
                   digitalWrite(IN4, LOW);
                   delay(250);
             } 
             else                                          //Como tengo más espacio a la izquierda, giro el coche hacia la izquierda
             {
                   digitalWrite(IN1, HIGH); // Girar izquierda
                   digitalWrite(IN2, LOW);
                   digitalWrite(IN3, LOW);
                   digitalWrite(IN4, HIGH);
                   delay(250);
             }
         }
         else                                              //** Como la distancia es mayor a 35 cm puedo continuar adelante
         {
            digitalWrite(IN1, HIGH);  // Adelante
            digitalWrite(IN2, LOW);
            digitalWrite(IN3, HIGH);
            digitalWrite(IN4, LOW);
         }
         delay(100);
  }

  //Si hemos presionado modo manual, iremos leyendo las instrucciones enviadas por el mando IR para ver qué tenemos que hacer
  else if(Boton == 6)
  {
      //Mientras no apriete el botón de pausa, el sistema funcionará en modo manual
      while(Boton != 7)
      {
	    //Leo el valor enviado por el mando cada 50 ms
            if (irrecv.decode(&results)) 
            {
                  translateIR(); 
                  irrecv.resume();
                  delay(50); 
            } 
		
               // Según el botón presionado realizaré una acción
               switch(Boton)
               {
                    case 1:
                        digitalWrite(IN1, HIGH);  // Adelante
                        digitalWrite(IN2, LOW);
                        digitalWrite(IN3, HIGH);
                        digitalWrite(IN4, LOW);
                    break;
                    case 2:
                        digitalWrite(IN1, HIGH); // Girar izquierda
                         digitalWrite(IN2, LOW);
                         digitalWrite(IN3, LOW);
                         digitalWrite(IN4, HIGH);
                    break;   
                    case 3:
                        digitalWrite(IN1, LOW); // Girar derecha
                         digitalWrite(IN2, HIGH);
                         digitalWrite(IN3, HIGH);
                         digitalWrite(IN4, LOW);
                    break;
                    case 4:
                        digitalWrite(IN1, HIGH);  // Adelante
                        digitalWrite(IN2, HIGH);
                        digitalWrite(IN3, HIGH);
                        digitalWrite(IN4, HIGH);
                    break;
                    default:
                        digitalWrite(IN1, HIGH);  // Adelante
                        digitalWrite(IN2, HIGH);
                        digitalWrite(IN3, HIGH);
                        digitalWrite(IN4, HIGH);           
               }
      }
  }
}

long Distancia()
{
//Esta rutina, cuando es llamada, devuelve la distancia en función del tiempo en que tardan en rebotar las ondas
     pinMode(TRIG, OUTPUT);
     digitalWrite(TRIG, LOW);
     delayMicroseconds(2);
     digitalWrite(TRIG, HIGH);
     delayMicroseconds(5);
     digitalWrite(TRIG, LOW);
     pinMode(ECO, INPUT);
     duration = pulseIn(ECO, HIGH);
     return duration / 29 / 2;
}
 
void MotorAB_Brake(int milliseconds)
{
//Esta rutina para el coche un tiempo determinado, según se indique cuando ésta es llamada
     digitalWrite(IN1, HIGH);
     digitalWrite(IN2, HIGH);
     digitalWrite(IN3, HIGH);
     digitalWrite(IN4, HIGH);
     if(milliseconds > 0)
     delay(milliseconds); 
}

void translateIR() // takes action based on IR code received

// describing KEYES Remote IR codes 

{

  switch(results.value)

  {
  //Definición de los códigos IR 
  case 0xFF629D: Boton = 1; break; // ADELANTE
  case 0xFF22DD: Boton = 2; break; // IZQ
  case 0xFFC23D: Boton = 3; break; // DER
  case 0xFFA857: Boton = 4; break; // ABAJO
  case 0xFF6897: Boton = 5; break; // MODO 1
  case 0xFF9867: Boton = 6; break; // MODO 2
  case 0xFF52AD: Boton = 7; break; // # PAUSA;  

  default: 
    Serial.println(" other button   ");

  }// End Case

  delay(500); // Do not get immediate repeat


} //END translateIR
