/*
 * VesselTree - MRI image segmentation and characterization
 *
 * Copyright 2014  Piotr M. Szczypiński <piotr.szczypinski@p.lodz.pl>
 *                 Adam Sankowski <sankowski@gmail.com>
 *                 Grzegorz Dwojakowski <grzegorz.dwojakowski@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <vector>

//! Typ wyliczeniowy
/*! Typ danych reprezentuj�cych obraz */
enum type
{
    BOOL1, UCHAR8, INT16, FLOAT32, DOUBLE64
};
//! S T R U K T U R A    O B R A Z U
/*!
Struktura zawieraj�ca niezb�dne informacje o obrazie.
*/
struct strukturaObrazu
{
    std::vector<unsigned int> size; /*!< vektor - rozmiar obrazu w danym kierunku  */
    std::vector<double> spacing;    /*!< vektor - spacing w danym kierunku   */
    std::vector<double> origin;     /*!< vektor - origin w danym kierunku   */
    type fileType;                  /*!< typ obrazu wybrany z typu wyliczeniowego enum   */
    std::string shortNote;          /*!< opcjonalna informacja o obrazie   */
    void *imageData;                /*!< wska�nik do danych   */
};

//! K L A S A    O B R A Z
/*!
Klasa Obraz zawiera podstawowe funkcje dzia�aj�ce na strukturze obrazu. Wszystkie z nich napisane zosta�y w taki spos�b
aby u�ytkownik nie musia� ko�ysta� z zewn�trznych bibliotek do przetwarzania obraz�w.
*/
class Obraz : strukturaObrazu
{
public:
    //! Informacja o ilo�ci wymiar�w obrazu
    /*!
    \return liczba wymiar�w.
    */
    int dimensionality();
    //! Typ obrazu
    /*!
    \return typ obrazu w postaci pozycji z enum type
    */
    type getType();
    //! Informacje o obrazie
    /*!
    \return informacja tekstowa o obrazie
    */
    std::string getShortNote();
    //! Rozmiar obrazu
    /*!
    \return rozmiar obrazu we wszystkich dost�pnych kierunkach
    */
    std::vector<unsigned int> getSize();
    //! Spacing obrazu
    /*!
    \return spacing obrazu we wszystkich dost�pnych kierunkach
    */
    std::vector<double> getSpacing();
    //! Pocz�tek obrazu
    /*!
    \return origin obrazu we wszystkich dost�pnych kierunkach
    */
    std::vector<double> getOrigin();
    //! Pobierz warto�� voksela
    /*!
    \param coord wsp�rz�dne woksela
    \return warto�� danego woksela
    */
    double getValue(std::vector<int> coord);
    //! Pobierz warto�� voksela (tylko obraz 3d)
    /*!
    \param x wsp�rz�dna 0
    \param y wsp�rz�dna 1
    \param z wsp�rz�dna 2
    \return warto�� danego woksela
    */
    double getValue(int x, int y, int z);
    //! Pobierz wska�nik do linni
    /*!
    \param coord wsp�rz�dne woksela
    \param direction kierunek linni
    \return wska�nik do pocz�tku lini
    */
    void* getLinePointer(std::vector<int> coord, int direction);
    //! Pobierz wska�nik do slica
    /*!
    \param coord wsp�rz�dne woksela
    \param direction kierunek slica
    \return wska�nik do pocz�tku slica
    */
    void* getSlice(std::vector<int> coord, int direction);
    //! Ustaw warto�� voksela
    /*!
    \param coord wsp�rz�dne woksela
    \param value rz�dana warto��
    */
    void setValue(std::vector<int> coord, double value);
    //! Ustaw warto�� voksela (tylko obraz 3d)
    /*!
    \param x wsp�rz�dna 0
    \param y wsp�rz�dna 1
    \param z wsp�rz�dna 2
    \param value rz�dana warto��
    */
    void setValue(int x, int y, int z, double value);
    //! Wype�nij struktur� danymi
    /*!
    \param image struktura obrazu
    */
    void fillStructure(strukturaObrazu image);
    //! Zwr�� struktur� obrqazu
    /*!
    \return struktura obrazu
    */
    strukturaObrazu returnStruct();
};
