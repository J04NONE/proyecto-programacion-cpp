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
#include <locale>

using namespace std;

// --- Constantes ---
const int MAX_USUARIOS = 10; // Dejamos limitado a un número pequeño para simplificar

// --- Arreglos  ---
string nombres[MAX_USUARIOS];
int tiempos[MAX_USUARIOS];
int tiposOrdenador[MAX_USUARIOS]; // 1 = bajas, 2 = altas
int servicios[MAX_USUARIOS];     // 1 = ofimática, 2 = navegación
float costos[MAX_USUARIOS];
int totalUsuarios = 0;

/**
 * Calcula el costo del servicio para un usuario 
 */
float calcularCostoUsuario(int tiempo, int tipoOrd, int servicio) {
    float costo = 0.0f;

    if (tipoOrd == 1) { // Bajas prestaciones
        if (servicio == 1) { // Ofimática
            if (tiempo <= 60) costo = tiempo * 20.0f;
            else costo = tiempo * 10.0f;
        } else if (servicio == 2) { // Navegación
            if (tiempo <= 60) costo = tiempo * 30.0f;
            else costo = tiempo * 15.0f;
        }
    } else if (tipoOrd == 2) { // Altas prestaciones
        if (servicio == 1) { // Ofimática
            if (tiempo <= 60) costo = tiempo * 30.0f;
            else costo = tiempo * 15.0f;
        } else if (servicio == 2) { // Navegación
            if (tiempo <= 60) costo = tiempo * 40.0f;
            else costo = tiempo * 20.0f;
        }
    }

    return costo;
}

/**
 * Registrar un nuevo usuario.
 */
void registrarUsuario() {
    if (totalUsuarios < MAX_USUARIOS) {
        cout << "\n--- Registrar Nuevo Usuario ---" << endl;
        cout << "Nombre del usuario: ";
        cin >> nombres[totalUsuarios];

        cout << "Tiempo de uso (en minutos): ";
        cin >> tiempos[totalUsuarios];

        cout << "Tipo de ordenador (1: Bajas, 2: Altas): ";
        cin >> tiposOrdenador[totalUsuarios];
        
        while (tiposOrdenador[totalUsuarios] != 1 && tiposOrdenador[totalUsuarios] != 2) {
            cout << "Opción inválida. Ingrese 1 o 2: ";
            cin >> tiposOrdenador[totalUsuarios];
        }

        cout << "Servicio ofrecido (1: Ofimática, 2: Navegación): ";
        cin >> servicios[totalUsuarios];
        
        while (servicios[totalUsuarios] != 1 && servicios[totalUsuarios] != 2) {
            cout << "Opción inválida. Ingrese 1 o 2: ";
            cin >> servicios[totalUsuarios];
        }

        costos[totalUsuarios] = calcularCostoUsuario(tiempos[totalUsuarios], tiposOrdenador[totalUsuarios], servicios[totalUsuarios]);
        
        totalUsuarios++;
        cout << "Usuario registrado exitosamente." << endl;
    } 
    else {
        cout << "El cibercafé ha alcanzado su capacidad máxima." << endl;
    }
}

/**
 * Mostrar todos los usuarios registrados.
 */
void mostrarUsuarios() {
    
    if (totalUsuarios > 0) {
        cout << "\n--- Usuarios Registrados ---" << endl;
        cout << left << setw(15) << "Nombre"
             << setw(10) << "Tiempo"
             << setw(15) << "Tipo Ord."
             << setw(15) << "Servicio"
             << setw(10) << "Costo ($)" << endl;
        cout << string(65, '-') << endl;

        for (int i = 0; i < totalUsuarios; ++i) {
            string tipoStr = (tiposOrdenador[i] == 1) ? "Bajas" : "Altas";
            string servStr = (servicios[i] == 1) ? "Ofimática" : "Navegación";
            cout << left << setw(15) << nombres[i]
                 << setw(10) << tiempos[i]
                 << setw(20) << tipoStr
                 << setw(25) << servStr
                 << setw(10) << fixed << setprecision(2) << costos[i] << endl;
        }
        cout << string(65, '-') << endl;
    } else {
        cout << "No hay usuarios registrados." << endl;
    }
}

void resumenUsuarios() {
    if (totalUsuarios == 0) {
        cout << "No hay usuarios registrados para generar resumen." << endl;
        return;
    }

    int bajasOfimatica = 0, bajasNavegacion = 0, altasOfimatica = 0, altasNavegacion = 0;

    for (int i = 0; i < totalUsuarios; i++) {
        if (tiposOrdenador[i] == 1) {
            if (servicios[i] == 1) bajasOfimatica++;
            else bajasNavegacion++;
        } else {
            if (servicios[i] == 1) altasOfimatica++;
            else altasNavegacion++;
        }
    }

    cout << "\n--- Resumen por Servicio y Tipo ---" << endl;
    cout << "Bajas prestaciones - Ofimática:   " << bajasOfimatica << endl;
    cout << "Bajas prestaciones - Navegación:  " << bajasNavegacion << endl;
    cout << "Altas prestaciones - Ofimática:   " << altasOfimatica << endl;
    cout << "Altas prestaciones - Navegación:  " << altasNavegacion << endl;
}

void resumenRecaudacion() {
    if (totalUsuarios == 0) {
        cout << "No hay usuarios registrados para generar resumen de recaudación." << endl;
        return;
    }

    float recaudacionBajasOfimatica = 0.0f;
    float recaudacionBajasNavegacion = 0.0f;
    float recaudacionAltasOfimatica = 0.0f;
    float recaudacionAltasNavegacion = 0.0f;

    for (int i = 0; i < totalUsuarios; i++) {
        if (tiposOrdenador[i] == 1) {
            if (servicios[i] == 1) recaudacionBajasOfimatica += costos[i];
            else recaudacionBajasNavegacion += costos[i];
        } else {
            if (servicios[i] == 1) recaudacionAltasOfimatica += costos[i];
            else recaudacionAltasOfimatica += costos[i];
        }
    }

    cout << "\n--- Resumen de Recaudación por Servicio y Tipo ---" << endl;
    cout << fixed << setprecision(2);
    cout << "Bajas prestaciones - Ofimática:   $" << recaudacionBajasOfimatica << endl;
    cout << "Bajas prestaciones - Navegación:  $" << recaudacionBajasNavegacion << endl;
    cout << "Altas prestaciones - Ofimática:   $" << recaudacionAltasOfimatica << endl;
    cout << "Altas prestaciones - Navegación:  $" << recaudacionAltasNavegacion << endl;
}

/**
 * Mostrar el menú principal y manejar la selección del usuario.
 */
void menu() {
    int opcion;

    do {
        cout << "\n--- Menú Principal ---" << endl;
        cout << "1. Registrar usuario" << endl;
        cout << "2. Mostrar usuarios registrados" << endl;
        cout << "3. Resumen de usuarios por servicio y tipo" << endl;
        cout << "4. Resumen de recaudación por servicio y tipo de PC" << endl;
        cout << "0. Salir" << endl;
        cout << "Ingrese su opción: ";
        cin >> opcion;

        switch (opcion) {
            case 1:
                registrarUsuario();
                break;
            case 2:
                mostrarUsuarios();
                break;
            case 3:
                resumenUsuarios();
                break;
            case 4:
                resumenRecaudacion();
                break;
            
                case 0:
                cout << "Saliendo del programa..." << endl;
                break;
            default:
                cout << "Opción inválida. Intente nuevamente." << endl;
        }
    } while (opcion != 0);
}

int main() {
    setlocale(LC_ALL, "Spanish");
    menu();
    return 0;
}