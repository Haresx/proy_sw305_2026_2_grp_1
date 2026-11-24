#ifndef UI_H
#define UI_H

// Pequeña capa de interfaz "modo inmediato" sobre raylib: tema, tipografia,
// botones y campos de texto. Cada widget se dibuja y se procesa en la misma llamada.

#include "raylib.h"
#include <string>
#include <vector>

namespace ui {

// ---- Paleta -----------------------------------------------------------------
inline const Color FONDO         = {243, 245, 248, 255};
inline const Color SUPERFICIE    = {255, 255, 255, 255};
inline const Color LINEA         = {221, 227, 234, 255};
inline const Color TEXTO         = { 30,  42,  54, 255};
inline const Color TEXTO_SUAVE   = {107, 122, 138, 255};
inline const Color ACENTO        = { 36,  71, 199, 255};   // cobalto
inline const Color ACENTO_HOVER  = { 28,  56, 164, 255};
inline const Color ACENTO_TENUE  = {226, 233, 252, 255};
inline const Color AMBAR         = {242, 169,   0, 255};   // comparando
inline const Color ROJO          = {229,  72,  77, 255};   // pivote
inline const Color VERDE         = { 43, 166, 117, 255};   // ya en su lugar
inline const Color BARRA         = {159, 179, 207, 255};
inline const Color BARRA_TENUE   = {212, 220, 232, 255};
inline const Color BARRA_MEZCLADA= {118, 144, 188, 255};
inline const Color AUX_IZQ       = {143, 176, 240, 255};
inline const Color AUX_DER       = {226, 170, 128, 255};

// ---- Tipografia -------------------------------------------------------------
void iniciarFuentes();
void cerrarFuentes();
float anchoTexto(const std::string& t, int tam, bool negrita = false);
void texto(const std::string& t, float x, float y, int tam, Color c, bool negrita = false);
void textoDer(const std::string& t, float xDer, float y, int tam, Color c, bool negrita = false);
void textoCentro(const std::string& t, float xCentro, float y, int tam, Color c, bool negrita = false);
// Texto con salto de linea automatico dentro de 'r'. Devuelve la altura usada.
float parrafo(const std::string& t, Rectangle r, int tam, Color c, bool negrita = false);
// Recorta con "..." para que quepa en 'ancho'.
std::string ajustar(const std::string& t, float ancho, int tam, bool negrita = false);

// ---- Formas -----------------------------------------------------------------
void rectRedondo(Rectangle r, float radio, Color c);
void tarjeta(Rectangle r, float radio, Color relleno, Color borde);
void chevronIzq(float cx, float cy, float tam, Color c);

// ---- Entrada ----------------------------------------------------------------
void inicioFrame();   // al comenzar cada frame
void finFrame();      // antes de EndDrawing: ajusta el cursor
bool sobre(Rectangle r);
bool clic(Rectangle r);   // el boton izquierdo se acaba de presionar dentro de r
void manoActiva();        // pide el cursor de mano en este frame (elementos clicables propios)

enum class Boton { Primario, Secundario, Texto };
// Devuelve true en el frame en que se hace clic (solo si 'activo').
bool boton(Rectangle r, const std::string& etiqueta, Boton estilo = Boton::Secundario,
           bool activo = true);

// Campo de texto de una linea. 'enfocado' lo decide quien lo llama (devuelve true si se hizo clic en el).
bool campo(Rectangle r, const std::string& etiqueta, const std::string& valor,
           bool enfocado, const std::string& pista);
// Aplica el teclado a 's' (letras, backspace con repeticion). Solo digitos si 'soloDigitos'.
void editar(std::string& s, size_t maxLen, bool soloDigitos);

// ---- Utilidades -------------------------------------------------------------
// Cuadro oscuro junto al cursor. La primera linea va en negrita.
void tooltip(const std::vector<std::string>& lineas);
// 1274 -> "1 274"
std::string miles(long long n);
// Barra vertical con la parte de arriba redondeada. 'yBase' es la linea de base.
void barra(int x, float yBase, int w, float h, Color c);
// Barra "fantasma": solo contorno (elemento que ya se copio al arreglo auxiliar).
void barraFantasma(int x, float yBase, int w, float h, Color c);

}  // namespace ui

#endif
