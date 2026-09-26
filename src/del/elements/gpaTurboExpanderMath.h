/**
 * @file gpaTurboExpanderMath.h
 * @brief Численные зависимости упрощённой модели турбодетандера.
 */
#ifndef GPA_TURBOEXPANDER_MATH_H
#define GPA_TURBOEXPANDER_MATH_H

#include "gpaInclude.h"

namespace gpa::turboExpander
{
/** @brief Нормированная S-образная характеристика: 0 <= x <= 1. */
[[nodiscard]] gpaReal smoothStep(gpaReal value);

/**
 * @brief Напор компрессора по закону подобия.
 * @param nominalHead Номинальный напор, кПа.
 * @param speedRatio Отношение текущей частоты к номинальной.
 * @param valveZeta Относительный коэффициент сопротивления клапана.
 */
[[nodiscard]] gpaReal compressorHead(gpaReal nominalHead, gpaReal speedRatio, gpaReal valveZeta);

/**
 * @brief Напор компрессора с плавной зависимостью от частоты и расхода.
 *
 * Характеристика проходит через номинальную точку и сохраняет ненулевую
 * производную по расходу в начале координат. Это важно для P-P решателя:
 * при нулевой частоте и нулевом расходе Якобиан не должен терять строку
 * давления.
 */
[[nodiscard]] gpaReal compressorPressureRise(gpaReal nominalHead, gpaReal speedRatio,
                                              gpaReal massFlow, gpaReal nominalMassFlow,
                                              gpaReal valveZeta);

/** @brief Изоэнтропический прирост энтальпии компрессора, Дж/кг. */
[[nodiscard]] gpaReal isentropicCompressorRise(gpaReal inletPressure, gpaReal outletPressure,
                                                gpaReal heatCapacity, gpaReal inletTemperature,
                                                gpaReal heatCapacityRatio);

/**
 * @brief Перепад давления на клапане по гидравлическому закону FHE, кПа.
 *
 * @f$\zeta=\zeta_\mathrm{nom}/u@f$, @f$v=G/(\rho A)@f$ и
 * @f$\Delta p=\zeta\rho v\sqrt{v^2+v_\mathrm{ref}^2}/(2\sqrt2)@f$.
 * Opening is an effective opening in [0; 1]; its tiny numerical floor is
 * used only in the P-P residual so a closed valve keeps a finite Jacobian.
 */
[[nodiscard]] gpaReal valvePressureDrop(gpaReal nominalResistance, gpaReal flowArea,
                                         gpaReal opening, gpaReal density,
                                         gpaReal massFlow, gpaReal referenceMassFlow);

/** @brief Изоэнтропический перепад энтальпии детандера, Дж/кг. */
[[nodiscard]] gpaReal isentropicTurbineDrop(gpaReal inletPressure, gpaReal outletPressure,
                                            gpaReal heatCapacity, gpaReal temperature,
                                            gpaReal heatCapacityRatio);

/**
 * @brief Давление после карты детандера из Python-модели.
 *
 * Давление вычисляется по приведённому расходу, давлению на входе ступени
 * и поправке на частоту ротора.
 */
[[nodiscard]] gpaReal turbineMapPressure(gpaReal stagePressure, gpaReal massFlow,
                                          gpaReal nominalMassFlow, gpaReal nominalInletPressure,
                                          gpaReal mapCoefficient, gpaReal speedRatio,
                                          gpaReal speedFactor);

/**
 * @brief Знаковая квадратичная характеристика местного сопротивления клапана.
 *
 * Давление считается положительным в направлении положительного расхода.
 * Поэтому при смене направления расхода величина сопротивления меняет знак:
 * @f$\Delta p = K\,G|G|@f$. Небольшая сглаживающая добавка сохраняет
 * конечную производную в нуле для нелинейного решателя.
 */
[[nodiscard]] gpaReal signedValvePressureDrop(gpaReal referencePressure, gpaReal referenceMassFlow,
                                              gpaReal massFlow, gpaReal coefficient);

/** @brief Момент механических потерь подшипников и вентиляции, Н·м. */
[[nodiscard]] gpaReal lossTorque(gpaReal nominalTurbineTorque, gpaReal speedRatio,
                                 gpaReal bearingLossZeta, gpaReal windageLossZeta);
} // namespace gpa::turboExpander

#endif // GPA_TURBOEXPANDER_MATH_H
