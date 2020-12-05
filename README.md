# JSON

Esta librería te permitirá manipular archivos y objetos en formato JSON en C++. Es ligera y fácil de usar e instalar: todo el código está contenido en un único archivo de cabecera [`JSON.hpp`](https://github.com/joaquinrmi/JSON/blob/master/include/dnc/JSON.hpp).

## ¿Qué puede hacer?

* Manipular objetos de cinco tipos distintos: `bool`, `number`, `string`, `array` y `object`; además del tipo especial `null`.
* Recorrer `array` y `object` usando iteradores.
* Convertir objetos `JSON` en `std::string` de C++ y guardarlos en archivos.
* Crear objetos `JSON` a partir de cadenas y archivos de texto.

## Instalación

Solo debes colocar el archivo [`JSON.hpp`](https://github.com/joaquinrmi/JSON/blob/master/include/dnc/JSON.hpp) en tu proyecto y comenzar a usarlo.

## Formas de uso

Para hacer uso de la librería, primero deberás importarla.
```cpp
#include "dnc/JSON.hpp"

using JSON = dnc::JSON;
```

### Creando objetos

```cpp
//creando un objeto vacío, igual a 'null'
JSON empty;

//creando un objeto de valor booleano
JSON bool_object = true;

//creando un objeto de valor numérico
JSON number = 3.14159265359;

//creando una cadena de texto
JSON str = "Hello World!";

//creando un arreglo vacío
JSON arr = JSON::Array();

//creando un objeto vacío
JSON obj = JSON::Object();

//creando un arreglo no vacío
JSON arr = JSON::Array({
   true, 25, false, "mouse"
});

//creando un objeto no vacío
JSON obj({
   {"empty", JSON()},
   {"age", 3.14159265359},
   {"name", "Juan"},
   {"friends", JSON::Array({
      "Pepe", "Claudia", "Teresa"
   })}
});
```

### Obtener valor de `JSON` en tipos primitivos de C++

Se puede acceder a los valores de los tipos `bool`, `number` y `string` de `JSON` como tipos de C++ con los métodos `JSON::get()`.
```cpp
//obtener un valor booleano
JSON json = true;
bool value = json.get<bool>();

//obtener un valor numérico
JSON json = 3.1415;
double number = json.get<double>();

//obtener una cadena de texto
JSON json = "Hello World!";
string str = json.get<string>();
```

En el caso de las cadenas de texto, el valor devuelto por `JSON::get<string>()` es diferente al devuelto por `JSON::toString()`.
```cpp
JSON str = "Hello World!";

cout << str.get<string>() << endl;
cout << str.toString() << endl;

//>> Hello World!
//>> "Hello World!"
```

El método `JSON::get()` devuelve el valor del objeto por referencia, por lo que puede usarse para editar el contenido más facilmente.
```cpp
JSON number = 456;
number.get<double>() += 1;

JSON str = "Hello World";
str.get<string>() += "!";
```

Para los casos de `array` y `object`, la función `JSON::get()` está sobrecargada para aceptar un índice `uint32_t` o una clave `string`, respectivamente.
```cpp
JSON arr = JSON::Array({ 25, 26, 27 });
cout << arr.get<double>(1) << endl;
//>> 26

JSON obj({
   {"name", "Pedro"}
});
cout << obj.get<string>("name") << endl;
//>> Pedro
```

### Los tipos `array` y `object`

Tanto `array` como `object` pueden almacenar objetos en su interior. Ambos respetan el orden con el que fueron insertados los elementos (los primeros elementos insertados se ubican al inicio del contenedor) y ambos poseen operaciones para acceder a los elementos que contienen, añadir nuevos y eliminar los existentes. En ambos casos, los elementos que se almacenan son objetos de tipo `JSON`, por lo que al acceder a uno de ellos se deberá tener eso en cuenta.

### Creando objetos desde texto

```cpp
//se inicializa 'person' como 'null'
JSON person;

//sobrescribimos el objeto deserializando el texto
person.parse("{\"name\": \"Gabriela\", \"age\": 31}");

//ahora podemos acceder a las propiedades 'name' y 'age' de 'person'
cout << person["name"].toString() << endl;
cout << person["age"].toString() << endl;
//>> "Gabriela"
//>> 31
```

Lo mismo es posible desde archivos de texto.
```cpp
person.parseFromFile("person.json");
```

### Recorrer arreglos y objetos

Un arreglo puede recorrerse utilizando índices o iteradores.
```cpp
JSON myArray = JSON::Array(/* ... */);

//con índices
for(uint32_t i = 0; i < myArray.size(); ++i)
{
   cout << myArray[i].toString() << endl;
}

//con iteradores
for(auto it = myArray.begin(); it != myArray.end(); ++it)
{
   cout << it->toString() << endl;
}

//o más compacto
for(auto& element : myArray)
{
   cout << element.toString() << endl;
}
```

Un objeto se puede recorrer con iteradores.
```cpp
JSON myObject({/* ... */});

for(auto it = myObject.begin(); it != myObject.end(); ++it)
{
   //it.key() devuelve la clave del valor actual del iterador.
   cout << it.key() << ": " << it->toString() << endl;
}
```

### Exportar objeto `JSON` a un archivo de texto

```cpp
JSON person({/* ... */});

if(person.saveToFile("person.json"))
{
   cout << "¡Archivo guardado exitosamente!" << endl;
}
else
{
   cout << "No se ha podido guardar el archivo." << endl;
}
```

## Características técnicas

* Versión actual: **1.0**.
* El *parser* soporta cadenas de texto en formato `UTF-8`, lo cual también es compatible con formato ASCII.
* Los objetos de tipo `number` están representados internamente con una variable `double`, por lo que se deberá tener eso en cuenta al intentar almacenar números demasiado grandes.
* Las cadenas de texto se almacenan en formato `UTF-8`, utilizando cada caracter de la cadena como un *byte* de la codificación.
* De la misma forma, los archivos generados por la función `JSON::saveToFile()` también están codificados en `UTF-8`.