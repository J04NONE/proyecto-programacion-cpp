/*
 * Avance del proyecto:

 * Hemos implementado en esta etapa del desarrollo las siguientes funcionalidades:
 * - Registro de usuarios con validación de entradas.
 * - Cálculo del costo del servicio según el tiempo, tipo de ordenador y servicio.
 * - Visualización de todos los usuarios registrados.
 * - Resumen por tipo de ordenador y servicio.
 * - Resumen de recaudación por categoría.
 *
 * Aún nos falta implementar:
 * - Mostrar usuarios con tiempo mayor al promedio.
 * - Cálculo y visualización de la recaudación total.
 *
 * Integrantes del equipo:
 * Ivan Velasco
 * David Alvarez
 * Joan Murillo
 */

#include <iostream>     
#include <iomanip>      
#include <string>      
#include <fstream>     
#include <locale>       


using namespace std;


const int MAX_USUARIOS = 10;

const string NOMBRE_ARCHIVO = "usuarios.txt"; 


struct Usuario {
    string nombre; // IMPORTANTE: los nombres serán de UNA SOLA PALABRA.
    int tiempo;
    int tipoOrdenador;
    int servicio;
    float costo;
};

// --- Arreglo Global de Estructuras y Contador ---
Usuario listaUsuarios[MAX_USUARIOS];
int totalUsuarios = 0;


/**
 * Calcula el costo del servicio para un usuario. */

float calcularCostoUsuario(int tiempo, int tipoOrd, int servicio) {
    float costo = 0.0f;
    if (tipoOrd == 1) { // Bajas prestaciones
        if (servicio == 1) { // Ofimática
            if (tiempo <= 60) {
                costo = tiempo * 20.0f;
            } else {
                costo = tiempo * 10.0f;
            }
        } else if (servicio == 2) { // Navegación
            if (tiempo <= 60) {
                costo = tiempo * 30.0f;
            } else {
                costo = tiempo * 15.0f;
            }
        }
    } else if (tipoOrd == 2) { // Altas prestaciones
        if (servicio == 1) { // Ofimática
            if (tiempo <= 60) {
                costo = tiempo * 30.0f;
            } else {
                costo = tiempo * 15.0f;
            }
        } else if (servicio == 2) { // Navegación
            if (tiempo <= 60) {
                costo = tiempo * 40.0f;
            } else {
                costo = tiempo * 20.0f;
            }
        }
    }
    return costo;
}

/**
 * Carga los datos de los usuarios desde un archivo de texto.*/

void cargarUsuariosDesdeArchivoTexto(const string& nombreArchivo, Usuario usuarios[], int& numUsuarios) {
    ifstream archivo(nombreArchivo);
    numUsuarios = 0; // Reiniciar contador

    if (!archivo.is_open()) {
        cout << "Información: No se encontró el archivo '" << nombreArchivo 
             << "'. Se iniciará sin datos previos." << endl;
        return;
    }

    cout << "Cargando datos desde '" << nombreArchivo << "'..." << endl;

    // Leer mientras no se alcance el máximo de usuarios y la lectura de todos los campos sea exitosa
    // El operador >> omite automáticamente los saltos de línea entre lecturas de diferentes campos.

    while (numUsuarios < MAX_USUARIOS &&
           (archivo >> usuarios[numUsuarios].nombre) && 
           (archivo >> usuarios[numUsuarios].tiempo) &&
           (archivo >> usuarios[numUsuarios].tipoOrdenador) &&
           (archivo >> usuarios[numUsuarios].servicio) &&
           (archivo >> usuarios[numUsuarios].costo)
          ) {

        numUsuarios++; // Incrementar solo si todos los campos del usuario se leyeron correctamente
    }

    // Informar sobre el resultado de la carga

    if (archivo.eof() && !archivo.fail()) { // Se alcanzó el fin del archivo sin errores de lectura
        if (numUsuarios > 0)
         {
             cout << "Se cargaron todos los " << numUsuarios << " usuarios desde '" << nombreArchivo << "'." << endl;
        } else {
             // Esto puede significar que el archivo estaba vacío o no tenía el formato esperado desde el inicio.
             cout << "El archivo '" << nombreArchivo << "' está vacío o no contiene datos en el formato esperado." << endl;
        }
    } else if (archivo.fail() && !archivo.eof()) { // Fallo de lectura antes de llegar al final (error de formato)
        cerr << "Advertencia: Error de formato en el archivo '" << nombreArchivo 
             << "'. No todos los datos pudieron ser leídos correctamente." << endl;
        cerr << "Se cargaron " << numUsuarios << " usuarios antes del error." << endl;
        archivo.clear(); // Limpiar los flags de error del stream para poder cerrarlo, etc.
    } else if (archivo.fail() && archivo.eof()) { // Fin de archivo, pero la última lectura falló (archivo incompleto)
        cerr << "Advertencia: El archivo '" << nombreArchivo 
             << "' parece estar incompleto. Se alcanzó el fin del archivo durante la lectura de un registro." << endl;
        cerr << "Se cargaron " << numUsuarios << " usuarios completos." << endl;
         archivo.clear(); // Limpiar flags
    }

    // Comprobar si se llenó el arreglo y podrían quedar más datos en el archivo
    if (numUsuarios == MAX_USUARIOS && !(archivo.eof() || archivo.fail())) {
        string tempNombreTest; // Intentar leer un dato más para ver si hay más
        if (archivo >> tempNombreTest) { // Si se puede leer algo más
            cout << "Advertencia: Se alcanzó el límite de " << MAX_USUARIOS 
                 << " usuarios. Es posible que no todos los usuarios del archivo hayan sido cargados." << endl;
        }
    }
    
    archivo.close();
}


/**
 *  Guarda los datos de los usuarios en un archivo de texto.
 * Formato: un campo por línea. Esta función es inherentemente simple y no cambia mucho.*/

void guardarUsuariosEnArchivoTexto(const string& nombreArchivo, const Usuario usuarios[], int numUsuarios) {
    ofstream archivo(nombreArchivo); 

    if (!archivo.is_open()) {
        cerr << "Error: No se pudo abrir el archivo '" << nombreArchivo << "' para guardar los datos." << endl;
        return;
    }

    cout << "Guardando datos en '" << nombreArchivo << "'..." << endl;
    for (int i = 0; i < numUsuarios; ++i) {
        archivo << usuarios[i].nombre << endl;
        archivo << usuarios[i].tiempo << endl;
        archivo << usuarios[i].tipoOrdenador << endl;
        archivo << usuarios[i].servicio << endl;
        archivo << fixed << setprecision(2) << usuarios[i].costo << endl;
    }
    archivo.close();

    if (numUsuarios > 0) {
        cout << "Datos de " << numUsuarios << " usuarios guardados exitosamente en '" << nombreArchivo << "'." << endl;
    } else {
        cout << "No hay datos de usuarios para guardar. El archivo '" << nombreArchivo << "' ha sido creado/actualizado (posiblemente vacío)." << endl;
    }
}


/**
 * Registra un nuevo usuario en el sistema.*/

void registrarUsuario(Usuario usuarios[], int& numUsuarios) {
    if (numUsuarios < MAX_USUARIOS) {
        cout << "\n--- Registrar Nuevo Usuario ---" << endl;
        Usuario nuevoUsuario;

        cout << "Nombre del usuario (UNA SOLA PALABRA): ";
        cin >> nuevoUsuario.nombre; // Lee solo hasta el primer espacio

        cout << "Tiempo de uso (en minutos): ";
        cin >> nuevoUsuario.tiempo;
        while (cin.fail() || nuevoUsuario.tiempo <= 0) {
            cout << "Entrada inválida. Ingrese un tiempo positivo (número entero): ";
        
            cin.clear(); // Limpiar flags de error
            // Forma simple de descartar la línea incorrecta:
            char ch;
            while (cin.get(ch) && ch != '\n'); // Lee caracteres hasta encontrar un salto de línea
            cin >> nuevoUsuario.tiempo; // Intenta leer de nuevo
        }

        cout << "Tipo de ordenador (1: Bajas, 2: Altas): ";
        cin >> nuevoUsuario.tipoOrdenador;
        while (cin.fail() || (nuevoUsuario.tipoOrdenador != 1 && nuevoUsuario.tipoOrdenador != 2)) {
            cout << "Opción inválida. Ingrese 1 o 2: ";
            cin.clear();
            char ch;
            while (cin.get(ch) && ch != '\n');
            cin >> nuevoUsuario.tipoOrdenador;
        }

        cout << "Servicio ofrecido (1: Ofimática, 2: Navegación): ";
        cin >> nuevoUsuario.servicio;
        while (cin.fail() || (nuevoUsuario.servicio != 1 && nuevoUsuario.servicio != 2)) {
            cout << "Opción inválida. Ingrese 1 o 2: ";
            cin.clear();
            char ch;
            while (cin.get(ch) && ch != '\n');
            cin >> nuevoUsuario.servicio;
        }

        nuevoUsuario.costo = calcularCostoUsuario(nuevoUsuario.tiempo, nuevoUsuario.tipoOrdenador, nuevoUsuario.servicio);
        
        usuarios[numUsuarios] = nuevoUsuario;
        numUsuarios++;
        cout << "Usuario '" << nuevoUsuario.nombre << "' registrado exitosamente." << endl;
    } else {

        cout << "El cibercafé ha alcanzado su capacidad máxima. No se pueden registrar más usuarios." << endl;
    }
}

/**
 * Muestra los detalles de un usuario específico. (Demostración de paso por puntero)
 * 
 */
void mostrarDetalleUsuario(const Usuario* ptrUsuario) {
    if (ptrUsuario == nullptr) return;

    string tipoStr = (ptrUsuario->tipoOrdenador == 1) ? "Bajas Prest." : "Altas Prest.";
    string servStr = (ptrUsuario->servicio == 1) ? "Ofimática" : "Navegación";

    cout << left << setw(15) << ptrUsuario->nombre // Ancho ajustado para nombres más cortos
         << setw(10) << ptrUsuario->tiempo
         << setw(15) << tipoStr
         << setw(15) << servStr
         << setw(10) << fixed << setprecision(2) << ptrUsuario->costo << endl;
}

/**
 *  Muestra todos los usuarios registrados en formato tabular.
 * 
 */
void mostrarUsuarios(const Usuario usuarios[], int numUsuarios) {
    if (numUsuarios > 0) {
        cout << "\n--- Usuarios Registrados ---" << endl;
        cout << left << setw(15) << "Nombre" 
             << setw(10) << "Tiempo"
             << setw(15) << "Tipo Ord."
             << setw(15) << "Servicio"
             << setw(10) << "Costo ($)" << endl;
        cout << string(70, '-') << endl; // Ajustar longitud de la línea

        for (int i = 0; i < numUsuarios; ++i) {
            mostrarDetalleUsuario(&usuarios[i]);
        }
        cout << string(70, '-') << endl;
    } else {
        cout << "No hay usuarios registrados." << endl;
    }
}

/**
 * Muestra un resumen de la cantidad de usuarios por tipo de ordenador y servicio.
 */
void resumenUsuarios(const Usuario usuarios[], int numUsuarios) {
    if (numUsuarios == 0) {
        cout << "No hay usuarios registrados para generar resumen." << endl;
        return;
    }
    int bajasOfimatica = 0, bajasNavegacion = 0, altasOfimatica = 0, altasNavegacion = 0;
    for (int i = 0; i < numUsuarios; i++) {
        if (usuarios[i].tipoOrdenador == 1) {
            if (usuarios[i].servicio == 1) bajasOfimatica++; else bajasNavegacion++;
        } else {
            if (usuarios[i].servicio == 1) altasOfimatica++; else altasNavegacion++;
        }
    }
    cout << "\n--- Resumen por Servicio y Tipo de Ordenador ---" << endl;
    cout << "Bajas prestaciones - Ofimática:   " << bajasOfimatica << endl;
    cout << "Bajas prestaciones - Navegación:  " << bajasNavegacion << endl;
    cout << "Altas prestaciones - Ofimática:   " << altasOfimatica << endl;
    cout << "Altas prestaciones - Navegación:  " << altasNavegacion << endl;
}

/**
 *  Muestra un resumen de la recaudación por tipo de ordenador y servicio.
 */
void resumenRecaudacion(const Usuario usuarios[], int numUsuarios) {
    if (numUsuarios == 0) {
        cout << "No hay usuarios registrados para generar resumen de recaudación." << endl;
        return;
    }
    float recBajasOfi = 0.0f, recBajasNav = 0.0f, recAltasOfi = 0.0f, recAltasNav = 0.0f;
    for (int i = 0; i < numUsuarios; i++) {
        if (usuarios[i].tipoOrdenador == 1) {
            if (usuarios[i].servicio == 1) recBajasOfi += usuarios[i].costo; else recBajasNav += usuarios[i].costo;
        } else {
            if (usuarios[i].servicio == 1) recAltasOfi += usuarios[i].costo; else recAltasNav += usuarios[i].costo;
        }
    }
    cout << "\n--- Resumen de Recaudación por Servicio y Tipo de Ordenador ---" << endl;
    cout << fixed << setprecision(2);
    cout << "Bajas prestaciones - Ofimática:   $" << recBajasOfi << endl;
    cout << "Bajas prestaciones - Navegación:  $" << recBajasNav << endl;
    cout << "Altas prestaciones - Ofimática:   $" << recAltasOfi << endl;
    cout << "Altas prestaciones - Navegación:  $" << recAltasNav << endl;
}

/**
 * @brief Muestra los usuarios cuyo tiempo de uso es mayor al promedio.
 * 
 */
void mostrarUsuariosMayorPromedio(const Usuario usuarios[], int numUsuarios) {
    if (numUsuarios == 0) {
        cout << "No hay usuarios registrados para calcular el promedio." << endl;
        return;
    }
    long long sumaTiempos = 0;
    for (int i = 0; i < numUsuarios; i++) sumaTiempos += usuarios[i].tiempo;
    
    float promedioTiempo = 0.0;
    if (numUsuarios > 0) { // Prevenir división por cero si, por alguna razón, numUsuarios fuera 0 aquí.
        promedioTiempo = static_cast<float>(sumaTiempos) / numUsuarios;
    }


    cout << "\n--- Usuarios con tiempo mayor al promedio (" << fixed << setprecision(2) << promedioTiempo << " min) ---" << endl;
    bool encontrados = false;
    for (const Usuario* ptr = usuarios; ptr < usuarios + numUsuarios; ++ptr) {
        if (ptr->tiempo > promedioTiempo) {
            cout << ptr->nombre << " - Tiempo: " << ptr->tiempo << " minutos, Costo: $" << ptr->costo << endl;
            encontrados = true;
        }
    }
    if (!encontrados) cout << "Ningún usuario supera el tiempo promedio." << endl;
}

/**
 * Calcula y muestra la recaudación total de todos los usuarios.
 *
 */
void mostrarRecaudacionTotal(const Usuario usuarios[], int numUsuarios) {
    if (numUsuarios == 0) {
        cout << "No hay usuarios registrados para calcular la recaudación total." << endl;
        return;
    }
    float totalRecaudado = 0.0f;
    for (const Usuario* ptr = usuarios; ptr < usuarios + numUsuarios; ++ptr) {
        totalRecaudado += ptr->costo;
    }
    cout << "\n--- Recaudación Total ---" << endl;
    cout << "Total recaudado: $" << fixed << setprecision(2) << totalRecaudado << endl;
}

/**
 *  Muestra el menú principal y maneja la selección del usuario.
 * Manejo de error de 'cin' simplificado.
 */
void menu() {
    int opcion;
    do {
        cout << "\n--- Menú Principal Ciber Café ---" << endl;
        cout << "1. Registrar usuario" << endl;
        cout << "2. Mostrar todos los usuarios" << endl;
        cout << "3. Resumen de usuarios (por tipo y servicio)" << endl;
        cout << "4. Resumen de recaudación (por tipo y servicio)" << endl;
        cout << "5. Mostrar usuarios con tiempo mayor al promedio" << endl;
        cout << "6. Mostrar recaudación total" << endl;
        cout << "0. Salir y Guardar" << endl;
        cout << "Ingrese su opción: ";
        
        cin >> opcion;
        if (cin.fail()) { 
            cout << "Opción inválida. Por favor, ingrese un número." << endl;
            cin.clear(); 
            char ch;
            while (cin.get(ch) && ch != '\n'); // Descarta el resto de la línea
            opcion = -1; // Para que el bucle continúe y no ejecute un case por error
        }

        switch (opcion) {
            case 1: registrarUsuario(listaUsuarios, totalUsuarios); break;
            case 2: mostrarUsuarios(listaUsuarios, totalUsuarios); break;
            case 3: resumenUsuarios(listaUsuarios, totalUsuarios); break;
            case 4: resumenRecaudacion(listaUsuarios, totalUsuarios); break;
            case 5: mostrarUsuariosMayorPromedio(listaUsuarios, totalUsuarios); break;
            case 6: mostrarRecaudacionTotal(listaUsuarios, totalUsuarios); break;
            case 0:
                guardarUsuariosEnArchivoTexto(NOMBRE_ARCHIVO, listaUsuarios, totalUsuarios);
                cout << "Saliendo del programa..." << endl;
                break;
            default:
                if (opcion != -1) { // No mostrar si ya se mostró el error de cin.fail()
                   cout << "Opción no válida. Intente nuevamente." << endl;
                }
        }
        // cout << endl; // Opcional: Añade un salto de línea extra después de cada acción para espaciar.
    } while (opcion != 0);
}

/**
 * @brief Función principal del programa.
 */
int main() {
    setlocale(LC_ALL, "Spanish"); // Para mostrar correctamente caracteres como 'ñ' o tildes.

    cargarUsuariosDesdeArchivoTexto(NOMBRE_ARCHIVO, listaUsuarios, totalUsuarios);
    menu();

    return 0; 
}
