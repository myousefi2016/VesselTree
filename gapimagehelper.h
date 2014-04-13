#include "obraz.h"

#include "itkImage.h"
typedef itk::Image< float, 3 >                                                  ImageType;
#include "itkImageFileReader.h"
typedef itk::ImageFileReader< ImageType >                                       ReaderType;
#include "itkImageFileWriter.h"
typedef itk::ImageFileWriter< ImageType >                                       WriterType;
#include "itkDiscreteGaussianImageFilter.h"
typedef itk::DiscreteGaussianImageFilter< ImageType, ImageType>                 GaussianFilterType;
#include "itkHessianRecursiveGaussianImageFilter.h"
typedef itk::HessianRecursiveGaussianImageFilter< ImageType >                   HessianFilterType;
#include "itkHessian3DToVesselnessMeasureImageFilter.h"
typedef itk::Hessian3DToVesselnessMeasureImageFilter< float >                   VesselnessMeasureFilterType;
#include "itkImageRegionConstIterator.h"
typedef itk::ImageRegionConstIterator< ImageType >                              ConstIteratorType;
#include "itkImageRegionIterator.h"
typedef itk::ImageRegionIterator< ImageType>                                    IteratorType;
#include "itkImageRegionConstIteratorWithIndex.h"
typedef itk::ImageRegionIteratorWithIndex< ImageType>                           IteratorTypeWI;
#include "itkConnectedThresholdImageFilter.h"
typedef itk::ConnectedThresholdImageFilter< ImageType, ImageType >              ConnectedFilterType;
#include "itkMinimumMaximumImageCalculator.h"
typedef itk::MinimumMaximumImageCalculator <ImageType>                          ImageCalculatorFilterType;
#include "itkImageToVTKImageFilter.h"
typedef itk::ImageToVTKImageFilter< ImageType >                                 Connector;
#include "itkRescaleIntensityImageFilter.h"
typedef itk::RescaleIntensityImageFilter< ImageType,ImageType>                  RescaleFilterType;

//! K L A S A    I T K F U N C T I O N S
/*!
Klas� itkFunctions tworz� funkcje przetwarzania obraz�w. Zwarzywszy na fakt, �e ka�da z nich
pobiera obraz wej�ciowy jako argument funkcji, przeprowadza operacj� filtracji po czym zwraca obraz,
nale�a�oby si� zastanowi� czy istnieje potrzeba tworzenia klasy. Je�li do��czy�oby si� te funkcje jako
"samodzielne metody" to umo�liwi�oby to korzystanie z nich bez konieczno�ci inicjalizowania obiektu klasy.
Ta decyzja pozostawiona jest G��wnemu Tw�rcy oprogramowania.
----------------------------------------------------------------------------------------------------------
Dwie pierwsze funkcje (itkImageToStructure i StructureToItkImage) s�u�� do konwersji mi�dzy obrazami
w postaci struktury (obraz.cpp) i kontenerem obrazu u�ywanym przez biblioteki Insight Toolkit. Jest to konieczne,
poniewa� wszystkie metody wykorzystuj� metody w�a�nie z tego �rodowiska (ITK).
*/
class GapImageHelpers
{
private:

public:
    //! Konwersja obrazu ITK do postaci struktury zdeklarowanej w obraz.cpp
    /*!
    \param par1 wska�nik do obrazu ITK.
    \return ten sam obraz reprezentowany ju� jako struktura.
    */
    strukturaObrazu itkImageToStructure(ImageType::Pointer par1);
    //! Konwersja obrazu w postaci struktury zdeklarowanej w obraz.cpp do typu akceptowalnego przez ITK
    /*!
    \param par1 obraz w postaci struktury.
    \return ten sam obraz reprezentowany ju� jako ITK image.
    */
    ImageType::Pointer StructureToItkImage(strukturaObrazu par1);
    //! Otwarcie obrazu zapisanego w plikach .nii, .img, .hdr
    /*!
    \param par1 �cie�ka do pliku
    \return obraz w postaci struktury.
    */
    strukturaObrazu openAnalyzeImage(std::string par1);
    //! Zmiana zakresu jasno�ci reprezentuj�cych obraz ("rozci�ganie histogramu")
    /*!
    \param par1 obraz wej�ciowy w postaci struktury
    \param min warto�� minimalna jasno�ci w obrazie wyj�ciowym (domy�lnie 0)
    \param max warto�� maxymalna jasno�ci w obrazie wyj�ciowym (domy�lnie 255)
    \return obraz wyj�ciowy w postaci struktury.
    */
    strukturaObrazu rescaleIntensity( strukturaObrazu par1, float min = 0, float max = 255 );
    //! Filtr Gaussa o zadanym poziomie rozmycia
    /*!
    \param par1 obraz wej�ciowy w postaci struktury
    \param par2 warto�� wariancji
    \return obraz wyj�ciowy w postaci struktury.
    */
    strukturaObrazu gaussianFilter(strukturaObrazu par1, float par2);
    //! Filtracja Macie�� Hessego i funkcja unnaczynniania Franghiego
    /*!
      Funkcja s�u�y do wyeksponowania struktur cylindrycznych w obrazie. Warto�� rozmycia
      (par2) odpowiada za rozmiar obiekt�w kt�re zostan� wyeksponowane.
    \param par1 obraz wej�ciowy w postaci struktury
    \param par2 warto�� sigma odpowiedzialna na rozmycie
    \return obraz wyj�ciowy w postaci struktury.
    */
    strukturaObrazu hessianFilter(strukturaObrazu par1, float par2);
    //! Z�o�enie dw�ch obraz�w w jeden za zasadzie rzutowania najwi�kszych jasno�ci
    /*!
      W przyadku filtracji wieloskalowych (filtracja hessego) konieczne jest sumowanie kolejnych
      efekt�w filtracji. Funkcja ta por�wnuje warto�ci wokseli obu obraz�w o tych samych wsp�rz�dnych.
      Do obrazu wynikowego trafia wy�sza warto��. Operacja powtarzana jest dla wszystkich punkt�w obrazu.
      Obrazy musz� by� tych samych rozmiar�w.
    \param par1 obraz wej�ciowy w postaci struktury
    \param par2 obraz wej�ciowy w postaci struktury
    \param par3 warto�� sigma odpowiedzialna na rozmycie
    \return obraz wyj�ciowy w postaci struktury.
    */
    strukturaObrazu mipTwoImages(strukturaObrazu par1, strukturaObrazu par2, float par3);
    //! Rozrost obszaru od zarodka
    /*!
      Rozrost obszaru od danego punktu obrazu. Metoda iteracyjna polegaj�ca na do��czaniu do obszaru s�siednich punkt�w
      kt�rych warto�ci s� w przedziale (par2, par3).
    \param par1 obraz wej�ciowy w postaci struktury
    \param par2 pr�g dolny
    \param par3 pr�g g�rny
    \param coord wsp�rz�dne punktu startu rozrostu obszaru
    \return obraz wyj�ciowy w postaci struktury.
    */
    strukturaObrazu RegionGrowing(strukturaObrazu par1, float par2, float par3, std::vector<unsigned int> coord);
    //! Utworzenie pustego obrazu
    /*!
      Utworzenie pustego obrazu, o tej samej rozdzielczo�ci, spacingu i typie co obraz wej�ciowy.
    \param par1 obraz wej�ciowy w postaci struktury
    \return obraz wyj�ciowy w postaci struktury.
    */
    strukturaObrazu CreateEmptyStructure(strukturaObrazu par1);
    //! Wieloskalowa filtracja Hessego
    /*!
      Funkcja "hessianFilter" wykonywana wielokrotnie, a wyniki poszczeg�lnych iteracji sk�adane s� przy
      pomocy funkcji "mipTwoImages".
    \param par1 obraz wej�ciowy w postaci struktury
    \param sigmaMin warto�� minimalna rozmycia
    \param sigmaMax warto�� maksymalna rozmycia
    \param noOfScales ilo�� iteracji filtracji
    \return obraz wyj�ciowy w postaci struktury.
    */
    strukturaObrazu MultiscaleHessianAlgorithm(strukturaObrazu par1, float sigmaMin, float sigmaMax, int noOfScales);
    //! Znajdowanie zarodka, punktu startu operacji rozrostu obszaru
    /*!
      Prosta funkcja znajduj�ca najja�niejszy punkt wewn�trz obrazu
    \param par1 obraz wej�ciowy w postaci struktury
    \return wsp�rz�dne w postaci wektora.
    */
    std::vector<unsigned int> FindSeed(strukturaObrazu par1);
    //! Warto�� maksymalna jasno�ci obrazu
    /*!
    \param par1 obraz wej�ciowy w postaci struktury
    \return warto�� maksymana
    */
    double FindMaximumValue(strukturaObrazu par1);
    //! Warto�� minimalna jasno�ci obrazu
    /*!
    \param par1 obraz wej�ciowy w postaci struktury
    \return warto�� minimalna
    */
    double FindMinimumValue(strukturaObrazu par1);
    //! Zapis obrazu do pliku (.nii, .img, .hdr)
    /*!
    \param par1 obraz wej�ciowy w postaci struktury
    \param par2 �cie�ka wraz z nazw� pliku
    */
    void saveImage(strukturaObrazu par1, std::string par2);
    //! Algorytm znajdowania naczy� krwiono�nych w obrazach Time of Flight
    /*!
      Funkcja wykonuje wielokrotn� filtracj� hessego, oraz przeprowadza segmentacj� od automatycznie znalezionego
      punktu rozrostu obszaru.
    \param par1 obraz wej�ciowy w postaci struktury
    \param noOfScales ilo�� iteracji filtracji hessego
    \param thresholdPercent warto�� progu wyra�ona procentowo
    \return obraz wyj�ciowy w postaci struktury.
    */
    strukturaObrazu HVSalgorithm(strukturaObrazu par1, int noOfScales, float thresholdPercent);
};
