/*!
 *  \file      gpaTurboExpanderInfo.h
 *  \brief     Метакласс описания АВО для библиотеки элементов
 *  \author    Фролов Алексей
 *  \date      2024
 */
#ifndef GPA_SIMPLE_INFO_H
#define GPA_SIMPLE_INFO_H

#include "gpaInclude.h"
#include "gpaDeviceInfo.h"
#include "gpaTurboExpander.h"

/*!
 * \class gpaTurboExpanderInfo
 * \brief Метакласс описания АВО для библиотеки элементов
 */
class gpaTurboExpanderInfo : public gpaDeviceInfo
{
public:
    /*!
     * Конструктор по умолчанию
     */
    gpaTurboExpanderInfo();
    /*!
     * Деструктор по умолчанию
     */
    virtual ~gpaTurboExpanderInfo() override = default;
    /*!
     * Получить количество материальных контуров элемента
     * \return количество материальных контуров элемента
     */
    virtual gpaUInt getNumMixCircuits() const override;
    /*!
     * Получить минимальное количество материальных портов элемента в заданном контуре
     * \param index индекс контура
     * \return минимальное количество материальных портов элемента в заданном контуре
     */
    virtual gpaUInt getMinNumMixPorts(gpaUInt index) const override;
    /*!
     * Получить максимальное количество материальных портов элемента в заданном контуре
     * \param index индекс контура
     * \return максимальное количество материальных портов элемента в заданном контуре
     */
    virtual gpaUInt getMaxNumMixPorts(gpaUInt index) const override;
    /*!
     * Получить название материального контура элемента
     * \param index индекс контура
     * \return название материального контура элемента
     */
    virtual const gpaString *getMixCircuitName(gpaUInt index) const override;
    /*!
     * Получить название материального порта элемента в заданном контуре
     * \param circIdx индекс контура
     * \param portIdx индекс порта в контуре
     * \return название материального порта элемента в заданном контуре
     */
    virtual const gpaString *getMixPortName(gpaUInt circIdx, gpaUInt portIdx) const override;
    /*!
     * Создать аппарат
     * \param [in] interfaces интерфейсы интегратора
     * \param [in] pairDevice ссылка на соответствующий аппарат в интеграторе
     * \param model описание параметров аппарата и его подмоделей
     * \return ссылка на аппарат
     */
    virtual gpaElement *createDevice(const gpaCoreInterfaces *interfaces, gpaCoreReference pairDevice,
                                     const gpaExtObjectModel *model) const override;
};

#endif
