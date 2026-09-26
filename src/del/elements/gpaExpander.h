/*!
 *  \file      gpaExpander.h
 *  \brief     Класс для описания однослойного объемного элемента
 *  \author    Мельников Роман
 *  \date      2025
 */
#ifndef GPA_EXPANDER_H
#define GPA_EXPANDER_H

#include "gpaHeatLayer.h"
#include "gpaInclude.h"
#include "gpaMedium.h"
#include "gpaVolumeElement.h"

/*!
 * \class gpaExpander
 * \brief Класс, реализующий однослойный объемный элемента
 */
class gpaExpander : public gpaElement {
public:
  /*!
   * Конструктор по умолчанию
   * \param link ссылки на родительские контейнеры
   */
  gpaExpander(const gpaParentLink *link = NULL);
  /*!
   * Конструктор копирования
   * \param volume объемный элемент для копирования
   */
  gpaExpander(const gpaExpander &volume);
  /*!
   * Деструктор по умолчанию
   */
  virtual ~gpaExpander() override;
  /*!
   * Функция копирования объемного элемента
   * \return скопированный объемный элемент
   */
  virtual gpaExpander *copy() const override;
  bool setRatedParameters(gpaReal omega, gpaReal G_KS, gpaReal G_TDS);
  bool setPolytropicCurve(std::vector<double> relative_MFR,
                             std::vector<double> polytropic_eta);
  bool setAdiabaticCurve(std::vector<double> relative_MFR,
                            std::vector<double> adiabatic_eta);
  bool setMOI(gpaReal moment);
  bool setInitialRPM(gpaReal omega);
  gpaReal getRPM() const;
  gpaReal getPower() const;
  /*!
   * Получить название типа элемента
   * \return название типа элемента
   */
  virtual const gpaString &getTypeName() const override {
    return getDeviceTypeName();
  }
  /*!
   * Получить описание элемента
   * \return описание элемента
   */
  virtual const gpaString &getDescription() const override {
    return getDeviceDescription();
  }
  /*!
   * Функция получения минимального количества портов в материальном контуре
   * элемента
   * \param index индекс контура
   * \return минимальное количество портов
   */
  virtual gpaUInt getMinNumMixPorts(gpaUInt index) const override;
  /*!
   * Функция получения максимального количества портов в материальном контуре
   * элемента
   * \param index индекс контура
   * \return максимальное количество портов
   */
  virtual gpaUInt getMaxNumMixPorts(gpaUInt index) const override;
  /*!
   * Получить значение размерности
   * \return значение размерности
   */
  virtual gpaUInt getNumVariables() const override;
  /*!
   * Заполнить названия неизвестных
   * \param [out] varNames названия неизвестных
   */
  virtual void fillVarNames(gpaStringVector &varNames) const override;
  /*!
   * Инициализировать камеру и ее параметры для расчета
   * \return код операции
   */
  virtual gpaResult initialize() override;
  /*!
   * Заполнить матрицу масс камеры
   * \param refRow первая строка подматрицы
   * \param refCol первый столбец подматрицы
   * \return код ошибки
   */
  virtual gpaResult fillMassMatrix(gpaUInt refRow,
                                   gpaUInt refCol) const override;
  gpaResult calcKSProcess(std::pair<gpaReal, gpaReal> &res, gpaReal dp);
  gpaResult calcTDSProcess(std::pair<gpaReal, gpaReal> &res, gpaReal dp);
  /*!
   * Инициализировать решение заданным вектором
   * \param [out] variables вектор неизвестных
   * \return код ошибки
   */
  virtual gpaResult initVariables(gpaVector &variables) const override;
  /*!
   * Рассчитать значения функций правых частей
   * \param [in] states вектор состояний
   * \param [in] variables вектор неизвестных
   * \param [out] values вектор значений функций правых частей
   * \return код операции
   */
  virtual gpaResult calcFuncValues(const gpaConstVector &states,
                                   const gpaConstVector &variables,
                                   gpaVector &values,
                                   const gpaSetOfEquations &eqIds) override;
  /*!
   * Получить невязку уравнения для удельной мольной энтальпии для заданного
   * порта материального потока
   * \param circIdx индекс материального контура
   * \param portIdx индекс порта материального потока
   * \param enthalpy текущее значение энтальпии (относительно которого требуется
   * невязка)
   * \param [out] funcValue значение невязки уравнения для удельной мольной
   * энтальпии для заданного порта материального потока
   * \param код операции
   */
  virtual gpaResult calcEnthalpyFuncValueAt(gpaUInt circIdx, gpaUInt portIdx,
                                            gpaReal enthalpy,
                                            gpaReal &funcValue) const override;
  /*!
   * Получить невязки уравнений для мольных долей компонентов смеси для
   * заданного порта материального потока
   * \param circIdx индекс материального контура
   * \param portIdx индекс порта материального потока
   * \param fractions текущие значения мольных долей компонентов (относительно
   * которых требуется невязка)
   * \param [out] funcValues вектор невязок уравнений для мольных долей
   * компонентов смеси для заданного порта материального потока
   * \param код операции
   */
  virtual gpaResult
  calcFractionsFuncValuesAt(gpaUInt circIdx, gpaUInt portIdx,
                            const gpaConstVector &fractions,
                            gpaVector &funcValues) const override;

  virtual gpaUInt getNumMixCircuits() const override { return 2; }

private:
  // Properties
  gpaMedium *m_compressorMedium;
  gpaMedium *m_expanderMedium;
  gpaReal m_I;                   // [Момент инерции вала турбины, кг * м^2]
  gpaReal m_w_nom;               // [Номинальная скорость вращения вала, об/мин]
  gpaReal m_G_KS_nom;            // [Номинальный расход, кг/с]
  gpaReal m_G_TDS_nom;           // [Номинальный расход, кг/с]
  std::vector<gpaReal> m_Gr_TDS; // [Относительный массовый расход детандера]
  std::vector<gpaReal>
      m_adiabatic_eta_TDS;      // [Адиабатный КПД детандерной части, д/ед.]
  std::vector<gpaReal> m_Gr_KS; // [Относительный массовый расход компрессора]
  std::vector<gpaReal>
      m_polytropic_eta_KS; // [Политропный КПД компрессорной части, д/ед.]
  gpaReal m_w_init;        // [начальная скорость вращения турбины, об/с]
  gpaReal m_h_KS;          // [Номинальный расход, кг/с]
  gpaReal m_h_TDS;         // [Номинальный расход, кг/с]
  gpaReal m_w;
  gpaReal m_power;

public:
  /*!
   * Получить название типа элемента
   * \return название типа элемента
   */
  static const gpaString &getDeviceTypeName() { return m_typeName; }
  /*!
   * Получить описание элемента
   * \return описание элемента
   */
  static const gpaString &getDeviceDescription() { return m_description; }

  static gpaUInt getDeviceNumMixCircuits() { return 2; }

private:
  //! Название элемента
  static const gpaString m_typeName;
  //! Описание элемента
  static const gpaString m_description;
};

#endif
