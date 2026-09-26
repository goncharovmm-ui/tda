#include "gpaTurboExpanderMath.h"

#include <algorithm>
#include <cmath>
#include <numbers>

namespace gpa::turboExpander
{
gpaReal smoothStep(gpaReal value)
{
    const gpaReal x = std::clamp(value, gpaReal(0.0), gpaReal(1.0));
    return x * x * (3.0 - 2.0 * x);
}

gpaReal compressorHead(gpaReal nominalHead, gpaReal speedRatio, gpaReal valveZeta)
{
    if (!std::isfinite(nominalHead) || !std::isfinite(speedRatio) || !std::isfinite(valveZeta))
        return 0.0;
    return std::max(0.0, nominalHead) * std::pow(std::max(0.0, speedRatio), 2) /
           std::max(0.02, valveZeta);
}

gpaReal compressorPressureRise(gpaReal nominalHead, gpaReal speedRatio,
                               gpaReal massFlow, gpaReal nominalMassFlow,
                               gpaReal valveZeta)
{
    if (!std::isfinite(nominalHead) || !std::isfinite(speedRatio) ||
        !std::isfinite(massFlow) || !std::isfinite(nominalMassFlow) ||
        !std::isfinite(valveZeta) || nominalMassFlow <= 0.0)
        return 0.0;

    // In the Python model the impeller and its downstream valve are separate:
    // H_impeller ~ N^2 / K_valve, while the valve contributes K·G|G| in the
    // pressure balance.  Keep the common map signature, but do not embed a
    // second flow law into the impeller head.
    (void)massFlow;
    (void)nominalMassFlow;
    return compressorHead(nominalHead, speedRatio, valveZeta);
}

gpaReal isentropicCompressorRise(gpaReal inletPressure, gpaReal outletPressure,
                                 gpaReal heatCapacity, gpaReal inletTemperature,
                                 gpaReal heatCapacityRatio)
{
    if (!std::isfinite(inletPressure) || !std::isfinite(outletPressure) ||
        !std::isfinite(heatCapacity) || !std::isfinite(inletTemperature) ||
        !std::isfinite(heatCapacityRatio) || inletPressure <= 0.0 ||
        outletPressure <= inletPressure || heatCapacity <= 0.0 ||
        inletTemperature <= 0.0 || heatCapacityRatio <= 1.0)
        return 0.0;

    const gpaReal exponent = (heatCapacityRatio - 1.0) / heatCapacityRatio;
    return heatCapacity * inletTemperature *
           (std::pow(outletPressure / inletPressure, exponent) - 1.0);
}

gpaReal valvePressureDrop(gpaReal nominalResistance, gpaReal flowArea,
                          gpaReal opening, gpaReal density,
                          gpaReal massFlow, gpaReal referenceMassFlow)
{
    if (!std::isfinite(nominalResistance) || !std::isfinite(flowArea) ||
        !std::isfinite(opening) || !std::isfinite(density) ||
        !std::isfinite(massFlow) || !std::isfinite(referenceMassFlow) ||
        nominalResistance < 0.0 || flowArea <= 0.0 || density <= 0.0 ||
        referenceMassFlow <= 0.0)
        return 0.0;

    constexpr gpaReal minimumOpening = 1.0e-8;
    constexpr gpaReal pascalPerKilopascal = 1000.0;
    const gpaReal effectiveOpening = std::clamp(opening, minimumOpening, gpaReal(1.0));
    const gpaReal localResistance = nominalResistance / effectiveOpening;
    const gpaReal velocity = massFlow / (density * flowArea);
    const gpaReal referenceVelocity = referenceMassFlow / (density * flowArea);
    return localResistance / std::numbers::sqrt2_v<gpaReal> * density * velocity *
           std::sqrt(velocity * velocity + referenceVelocity * referenceVelocity) / 2.0 /
           pascalPerKilopascal;
}

gpaReal isentropicTurbineDrop(gpaReal inletPressure, gpaReal outletPressure,
                              gpaReal heatCapacity, gpaReal temperature,
                              gpaReal heatCapacityRatio)
{
    if (!std::isfinite(inletPressure) || !std::isfinite(outletPressure) ||
        !std::isfinite(heatCapacity) || !std::isfinite(temperature) ||
        !std::isfinite(heatCapacityRatio) || inletPressure <= 0.0 ||
        heatCapacity < 0.0 || temperature < 0.0 || heatCapacityRatio <= 1.0)
        return 0.0;

    const gpaReal pressureRatio = std::clamp(outletPressure / inletPressure, 1.0e-9, 1.0);
    return heatCapacity * temperature *
           (1.0 - std::pow(pressureRatio, (heatCapacityRatio - 1.0) / heatCapacityRatio));
}

gpaReal turbineMapPressure(gpaReal stagePressure, gpaReal massFlow,
                           gpaReal nominalMassFlow, gpaReal nominalInletPressure,
                           gpaReal mapCoefficient, gpaReal speedRatio,
                           gpaReal speedFactor)
{
    if (!std::isfinite(stagePressure) || !std::isfinite(massFlow) ||
        !std::isfinite(nominalMassFlow) || !std::isfinite(nominalInletPressure) ||
        !std::isfinite(mapCoefficient) || !std::isfinite(speedRatio) ||
        !std::isfinite(speedFactor) || stagePressure <= 0.0 || nominalMassFlow <= 0.0 ||
        nominalInletPressure <= 0.0 || mapCoefficient < 0.0)
        return stagePressure;

    const gpaReal boundedSpeedRatio = std::max(0.0, speedRatio);
    const gpaReal effectiveSpeedFactor = std::max(
        1.0e-6, (1.0 - speedFactor) + speedFactor * boundedSpeedRatio * boundedSpeedRatio);
    const gpaReal reducedFlow = (massFlow / nominalMassFlow) *
                                (nominalInletPressure / stagePressure) /
                                effectiveSpeedFactor;
    // Retain a finite slope at zero flow for the pressure/flow Newton block.
    // The normalization keeps the nominal point unchanged: at reducedFlow=1
    // the shape below is exactly one.
    constexpr gpaReal flowShapeCurvature = 0.25;
    const gpaReal flowShape = reducedFlow * (1.0 + flowShapeCurvature * std::abs(reducedFlow)) /
                              (1.0 + flowShapeCurvature);
    return stagePressure * std::exp(-mapCoefficient * flowShape);
}

gpaReal signedValvePressureDrop(gpaReal referencePressure, gpaReal referenceMassFlow,
                                gpaReal massFlow, gpaReal coefficient)
{
    if (!std::isfinite(referencePressure) || !std::isfinite(referenceMassFlow) ||
        !std::isfinite(massFlow) || !std::isfinite(coefficient) ||
        referencePressure < 0.0 || referenceMassFlow <= 0.0 || coefficient < 0.0)
        return 0.0;

    const gpaReal normalizedFlow = massFlow / referenceMassFlow;
    constexpr gpaReal smoothing = 1.0e-6;
    return referencePressure * coefficient * normalizedFlow *
           std::sqrt(normalizedFlow * normalizedFlow + smoothing);
}

gpaReal lossTorque(gpaReal nominalTurbineTorque, gpaReal speedRatio,
                   gpaReal bearingLossZeta, gpaReal windageLossZeta)
{
    const gpaReal nonNegativeSpeedRatio = std::max(0.0, speedRatio);
    return std::max(0.0, nominalTurbineTorque) *
           (std::max(0.0, bearingLossZeta) * nonNegativeSpeedRatio +
            std::max(0.0, windageLossZeta) * nonNegativeSpeedRatio * nonNegativeSpeedRatio);
}
} // namespace gpa::turboExpander
