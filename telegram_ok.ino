
#include <WiFiClientSecure.h>
#include <ESP8266WiFi.h>


// Initialize Wifi connection to the router
const char *ssid = "xxxxx";   // cannot be longer than 32 characters!
const char *pass = "yyyyyyyyy";   // your network key
int status = WL_IDLE_STATUS;

// Datos del Bot de Telegram

String BOTtoken = "botxxxxxxxx:yyyyyyyyyyyyyyyyyyyyyy";
String Chat_id = "2xxxxxxx"; // Lo escribimos aqui el Chat_id, pero podriamos obtenerlo del ultimo Msg
String Texto_enviar = "";
String Texto_recibido = "";
String Update_id = "";
String anterior_upd = "";
String Nueva_upd = "";
String Respuesta = "";
 
 // Variables del codigo de tiempo
int Inicio;
int Termino;
int Intervalo = 15000;
unsigned long elapsed = 0;
unsigned long previous;
boolean respondio = false;


WiFiClientSecure client; // inicio del cliente seguro
IPAddress server(149,154,167,200); // IP de api.telegram.org telegram


void setup() {

  Serial.begin(115200);
    // check for the presence of the shield:
  WiFi.begin(ssid, pass); // Conexion a Wifi
 /// }
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  // Comprobamos la conexion a Telegram
  if (client.connect(server, 443)) { 
     Serial.println(".... connected to Telegram");
  }
  // y enviamos el texto de inicio
  Enviar_texto(" Inicio del Sistema .......");

  // Comprobamos el ultimo mensaje
  Ultimo_msg();
  previous = millis();

}



void loop() {
  elapse();
  Leer_msg(); // leemos el ultimo mensaje

  // Comprobamos que haya pasado xx seg desde la ultima vez
  if (elapsed > 500) {
    
    anterior_upd = Update_id; // Guardamos la anterior Update , Nota: Deberiamos usar un fichero o Db para hacerlo mejor
    Ultimo_msg (); // comprobamos el ultimo mensaje
    delay(1000); // Esperamos a recibir los datos
    Leer_msg(); // Leemos los datos
    busca_upd_id(Respuesta); // buscamos la Update_id y la guardamos
    busca_texto(Respuesta); // Buscamos el Texto del mensaje 
    // Si ha cambiado la Update_id seguimos con el codigo
    if (anterior_upd != Nueva_upd) {
        //Serial.println("Es diferente Update");
        Responder_mensaje(Texto_recibido);
     } else {
       } // No hacemos nada si es el mismo Upd_id     
   }
} // Fin Loop


// Orden para buscar el texto del mensaje
void busca_texto( String Rsp ) {
  Texto_recibido = "";
  int  start = Rsp.indexOf("text") +7 ; // Buscamos el indice ( numero ) de la palabra "text" y le añadimos 7
  int  fin = Rsp.indexOf("}}]}") -1; // Buscamos el indice del texto }}]} y le restamos uno
  Texto_recibido = (Rsp.substring(start,fin)); // Guardamos el resultado en la variable
}

//Orden para buscar la Update_id
void busca_upd_id( String Rsp ) {
  anterior_upd = Update_id; // Guardamos la anterior Update_id para comprobar
  int  start = Rsp.indexOf("update_id") +11 ; // Buscamos el indice del texto y le añadimos 11
  int  fin = Rsp.indexOf("message") -2; // Buscamos el indice del texto y le restamos 2 
  Update_id =Rsp.substring(start,fin); // Guardamos la Update_id 
  Nueva_upd = Rsp.substring(start,fin); // Volvemos a guardar la Update_id pero en la variable de nueva
}

// Orden para pedir el ultimo mensaje, vemos que se usa el Offset=-1&limit=1 para mostrar solo el ultimo
void Ultimo_msg () {
  if (client.connect(server, 443)) { 
   // client.println("GET /bot259047685:AAH7Y8DV3PZn6H52uhmrsvc_q3yOuyqqH6g/getUpdates?offset=-1&limit=1");
    client.println("GET /"+ BOTtoken + "/getUpdates?offset=-1&limit=1");
  }
  previous = millis(); // Guardamos los milisegundos para comprobar que haya pasado X tiempo entre lecturas
}

//Leemos el mensaje completo y lo añadimos a una variable caracter por caracter
void Leer_msg () {
  Respuesta= ""; // Vaciamos la variable
  while (client.available()) { // Mientras no lo lea todo seguira leyendo
    char inChar = client.read(); // Lee el caracter
    Respuesta += inChar; // Añadimos caracter a caracter el mensaje
  }  
}

//Orden para comprobar el tiempo entre lecturas
void elapse(){
  elapsed = millis() - previous;
}


//Orden para enviar cualquier texto a Telegram
void Enviar_texto( String Texto_enviar ) {
    if (client.connect(server, 443)) { 
       client.println("GET /"+ BOTtoken + "/sendMessage?chat_id=" + Chat_id + "&text=" + Texto_enviar +"");
    }
}

//Aqui añadiremos las ordenes de respuesta del arduino
void Responder_mensaje ( String mensaje ) {
   
  
   if (mensaje == "Hola") {
    Enviar_texto("Hola!");
    respondio = true;
   }


if (respondio == true) { // mostramos el texto que se ha entendio 
  Serial.println("El Texto : " + mensaje + " Lo he entendio perfectamente");
}else {
  Serial.println("El Texto : " + mensaje + " No Lo he entendio");
}
respondio = false ; // Dejamos en falso que entendio el mensaje 
}

////////// Fin del codigo



