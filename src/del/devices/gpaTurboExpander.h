/*!
 *  \file      gpaTurboExpander.h
 *  \brief     Класс, описывающий рекуператор
 *  \author
 *  \date      2025
 */
#ifndef GPA_TURBOEXPANDER_H
#define GPA_TURBOEXPANDER_H

#include "gpaInclude.h"
#include "gpaExportSystem.h"
#include "gpaExpander.h"

/*!
 * \class gpaTurboExpander
 * \brief Класс, реализующий рекуператор
 */
class gpaTurboExpander : public gpaExportSystem
{
public:
    /*!
     * Конструктор с параметрами
     * \param interfaces интерфейсы интегратора
     * \param importRef ссылка на парный аппарат в интеграторе
     */
    gpaTurboExpander(const gpaCoreInterfaces *interfaces, gpaCoreReference importRef);
    /*!
     * Конструктор копирования
     * \param recuperator ссылка на рекуператор
     */
    gpaTurboExpander(const gpaTurboExpander &recuperator) = delete;
    /*!
     * Деструктор по умолчанию
     */
    virtual ~gpaTurboExpander() override = default;
    /*!
     * Функция копирования рекуператора
     * \return скопированный рекуператор
     */
    virtual gpaTurboExpander *copy() const override { return NULL; }
    /*!
     * Получить имя типа элемента
     * \return имя типа элемента
     */
    virtual const gpaString &getTypeName() const override { return getDeviceTypeName(); }
    /*!
     * Получить описание аппарата
     * \return описание аппарата
     */
    virtual const gpaString &getDescription() const override { return getDeviceDescription(); }
    /*!
     * Функция получения количества материальных контуров элемента
     * \return количество материальных контуров элемента
     */
    virtual gpaUInt getNumMixCircuits() const override { return getDeviceNumMixCircuits(); }
    /*!
     * Функция получения минимального количества портов в материальном контуре элемента
     * \param index индекс контура
     * \return минимальное количество портов
     */
    virtual gpaUInt getMinNumMixPorts(gpaUInt index) const override { return getDeviceMinNumMixPorts(index); }
    /*!
     * Функция получения максимального количества портов в материальном контуре элемента
     * \param index индекс контура
     * \return максимальное количество портов
     */
    virtual gpaUInt getMaxNumMixPorts(gpaUInt index) const override { return getDeviceMaxNumMixPorts(index); }
    /*!
     * Инициализировать аппарат
     * \return код операции
     */
    virtual gpaResult initialize() override;
    /*!
     * Рассчитать значения сенсоров
     * \param [out] sensors вектор сенсоров
     * \return код операции
     */
    virtual gpaResult calcSensors(gpaVector &sensors) const override;

protected:
    /*!
     * Функция получения вектора с описаниями параметров аппарата
     * \return вектор с описаниями параметров аппарата
     */
    virtual const gpaParameterInfoVector *getParamsInfo() const override { return getParametersInfo(); }
    /*!
     * Функция получения вектора с описаниями сенсоров модели аппарата
     * \return вектор с описаниями сенсоров модели аппарата
     */
    virtual const gpaSensorInfoVector *getSensesInfo() const override { return getSensorsInfo(); }
    /*!
     * Функция проверки значений параметров модели аппарата
     * \param values вектор параметров модели аппарата
     * \return код операции
     */
    virtual gpaResult checkParameterValues(const gpaParameterValueVector& parameters) const override;
    /*!
     * Функция назначения параметров модели аппарата
     * \param values вектор параметров модели аппарата
     * \param checkNeeded флаг о необходимости проверить параметры
     * \return код операции
     */
    virtual gpaResult setParameterValues(const gpaParameterValueVector& parameters, bool checkNeeded = true) override;

private:
    gpaExpander m_expander;
    void setNames();
    /*!
     * Соединить аппараты и потоки в аппарате
     */
    gpaResult buildDevice();

public:
public:
    /*!
     * Получить название типа аппарата
     * \return название типа аппарата
     */
    static const gpaString &getDeviceTypeName() { return m_typeName; }
    /*!
     * Получить описание аппарата
     * \return описание аппарата
     */
    static const gpaString &getDeviceDescription() { return m_description; }
    /*!
     * Получить количество материальных контуров элемента
     * \return количество материальных контуров элемента
     */
    static gpaUInt getDeviceNumMixCircuits();
    /*!
     * Получить минимальное количество подключенных материальных потоков
     * \param index индекс материального контура
     * \return минимальное количество подключенных материальных потоков
     */
    static gpaUInt getDeviceMinNumMixPorts(gpaUInt index);
    /*!
     * Получить максимальное количество подключенных материальных потоков
     * \param index индекс материального контура
     * \return максимальное количество подключенных материальных потоков
     */
    static gpaUInt getDeviceMaxNumMixPorts(gpaUInt index);
    /*!
     * Получить вектор описаний параметров аппарата
     * \return вектор описаний параметров аппарата
     */
    static const gpaParameterInfoVector *getParametersInfo() { return &m_parameters; }
    /*!
     * Получить вектор описаний сенсоров модели
     * \return вектор описаний сенсоров модели
     */
    static const gpaSensorInfoVector *getSensorsInfo() { return &m_sensors; }
    /*!
     * Функция получения названия материального контура элемента
     * \param index индекс контура
     * \return название материального контура элемента
     */
    virtual const gpaString *getMixCircuitName(gpaUInt index) const override { return getDeviceMixCircuitName(index); }
    /*!
     * Функция получения названия порта в материальном контуре элемента
     * \param circIdx индекс контура
     * \param portIdx индекс порта
     * \return название порта
     */
    virtual const gpaString *getMixPortName(gpaUInt circIdx, gpaUInt portIdx) const override { return getDeviceMixPortName(circIdx, portIdx); }
    /*!
     * Получить название материального порта элемента
     * \param index индекс контура
     * \return название материального порта элемента
     */
    static const gpaString *getDeviceMixCircuitName(gpaUInt index);

    /*!
     * Получить название материального порта элемента
     * \param index индекс контура
     * \return название материального порта элемента
     */
    static const gpaString *getDeviceMixPortName(gpaUInt circIdx, gpaUInt portIdx);

private:
    //! Название аппарата
    static const gpaString m_typeName;
    //! Описание аппарата
    static const gpaString m_description;
    //! Вектор описаний параметров аппарата
    static const gpaParameterInfoVector m_parameters;
    //! Вектор описаний сенсоров модели
    static const gpaSensorInfoVector m_sensors;
    //! Массив названий портов элемента
    static const gpaStringVector m_portNames;
    //! Массив названий контуров элемента
    static const gpaStringVector m_circuitNames;
};

#endif
