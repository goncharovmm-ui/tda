#include "gpaTurboExpanderV2Element.h"

#include "gpaTurboExpanderMath.h"

#include <algorithm>
#include <cmath>
#include <numbers>

namespace
{
constexpr gpaUInt compressorCircuit = 0;
constexpr gpaUInt turbineCircuit = 1;
constexpr gpaUInt inletPort = 0;
constexpr gpaUInt outletPort = 1;
constexpr gpaReal kilopascalToPascal = 1000.0;
constexpr gpaReal universalGasConstant = 8.314462618;
constexpr gpaReal flowRegularization = 1.0e-6;

enum class turboEquations : gpaUInt
{
    rotorDynamics,
    turbineValveDynamics,
    compressorValveDynamics,
    compressorPressureBalance,
    compressorFlowBalance,
    turbinePressureBalance,
    turbineFlowBalance,
    count
};

enum class turboVariables : gpaUInt
{
    rotorSpeed,
    turbineValveOpening,
    compressorValveOpening,
    count
};

const gpaString typeName = "turbo-expander-v2";
const gpaString description = "Турбодетандер с компрессором на общем валу";

gpaReal getSpecificHeatCapacity(const gpaMixPort* port, gpaReal fallback)
{
    const gpaMedium* medium = port ? port->getStreamMedium() : nullptr;
    if (!medium)
        return fallback;
    const gpaReal molarHeatCapacity = medium->getHeatCapacity();
    const gpaReal molarMass = medium->getMolarMass();
    const gpaReal specificHeatCapacity = molarHeatCapacity / molarMass;
    return std::isfinite(specificHeatCapacity) && specificHeatCapacity > 0.0
        ? specificHeatCapacity : fallback;
}

gpaReal getHeatCapacityRatio(const gpaMixPort* port, gpaReal fallback)
{
    const gpaMedium* medium = port ? port->getStreamMedium() : nullptr;
    if (!medium)
        return fallback;
    const gpaReal molarHeatCapacity = medium->getHeatCapacity();
    const gpaReal molarMass = medium->getMolarMass();
    const gpaReal specificGasConstant = universalGasConstant / molarMass;
    const gpaReal specificHeatCapacity = molarHeatCapacity / molarMass;
    const gpaReal specificHeatCapacityAtConstantVolume =
        specificHeatCapacity - specificGasConstant;
    const gpaReal ratio = specificHeatCapacity / specificHeatCapacityAtConstantVolume;
    return std::isfinite(ratio) && ratio > 1.0 ? ratio : fallback;
}
}

gpaTurboExpanderV2Element::gpaTurboExpanderV2Element(const gpaParentLink* link) : gpaFlowElement(link)
{
    // gpaFlowElement/gpaCircuitElement already creates material circuit 0.
    // Only the second (turbine) circuit has to be added here.
    setMixCircuit(new gpaElementMixCircuit(this, turbineCircuit));
}

const gpaString& gpaTurboExpanderV2Element::getTypeName() const { return typeName; }
const gpaString& gpaTurboExpanderV2Element::getDescription() const { return description; }

void gpaTurboExpanderV2Element::fillVarNames(gpaStringVector& names) const
{
    if (names.size() < static_cast<gpaUInt>(turboVariables::count))
        return;
    names[static_cast<gpaUInt>(turboVariables::rotorSpeed)] = "rotor-speed";
    names[static_cast<gpaUInt>(turboVariables::turbineValveOpening)] = "turbine-valve-opening";
    names[static_cast<gpaUInt>(turboVariables::compressorValveOpening)] = "compressor-valve-opening";
}

void gpaTurboExpanderV2Element::fillEquationNames(gpaStringVector& names) const
{
    if (names.size() < static_cast<gpaUInt>(turboEquations::count))
        return;
    names[static_cast<gpaUInt>(turboEquations::rotorDynamics)] = "rotor-dynamics";
    names[static_cast<gpaUInt>(turboEquations::turbineValveDynamics)] = "turbine-valve-dynamics";
    names[static_cast<gpaUInt>(turboEquations::compressorValveDynamics)] = "compressor-valve-dynamics";
    names[static_cast<gpaUInt>(turboEquations::compressorPressureBalance)] = "compressor-pressure-balance";
    names[static_cast<gpaUInt>(turboEquations::compressorFlowBalance)] = "compressor-flow-balance";
    names[static_cast<gpaUInt>(turboEquations::turbinePressureBalance)] = "turbine-pressure-balance";
    names[static_cast<gpaUInt>(turboEquations::turbineFlowBalance)] = "turbine-flow-balance";
}

gpaResult gpaTurboExpanderV2Element::initialize()
{
    m_compressorInlet = getMixPort(compressorCircuit, inletPort);
    m_compressorOutlet = getMixPort(compressorCircuit, outletPort);
    m_turbineInlet = getMixPort(turbineCircuit, inletPort);
    m_turbineOutlet = getMixPort(turbineCircuit, outletPort);
    if (!m_compressorInlet || !m_compressorOutlet || !m_turbineInlet || !m_turbineOutlet ||
        !m_compressorInlet->getStreamMedium() || !m_turbineInlet->getStreamMedium() ||
        m_shaftInertia <= 0.0 || m_nominalSpeed <= 0.0 ||
        m_nominalCompressorMassFlow <= 0.0 || m_nominalTurbineMassFlow <= 0.0 ||
        m_nominalTurbinePressureDrop < 0.0 || m_turbineHeatCapacityRatio <= 1.0 ||
        m_turbineValveFlowArea <= 0.0 || m_compressorValveFlowArea <= 0.0 ||
        m_turbineValveTimeConstant <= 0.0 || m_compressorValveTimeConstant <= 0.0)
        return GPA_ERROR_WRONG_ARGS;
    return GPA_RESULT_OK;
}

gpaResult gpaTurboExpanderV2Element::initVariables(gpaVector& variables) const
{
    if (variables.size() < static_cast<gpaUInt>(turboVariables::count))
        return GPA_ERROR_WRONG_ARGS;
    variables[static_cast<gpaUInt>(turboVariables::rotorSpeed)] = m_initialSpeed;
    variables[static_cast<gpaUInt>(turboVariables::turbineValveOpening)] =
        std::clamp(m_valveOpening, 0.0, 1.0);
    variables[static_cast<gpaUInt>(turboVariables::compressorValveOpening)] =
        std::clamp(m_compressorValveOpening, 0.0, 1.0);
    return GPA_RESULT_OK;
}

gpaResult gpaTurboExpanderV2Element::fillMassMatrix(gpaUInt row, gpaUInt column) const
{
    for (gpaUInt index = 0; index < static_cast<gpaUInt>(turboVariables::count); ++index)
    {
        const gpaResult result = setMassMatrCoef(row + index, column + index, 1.0);
        if (result != GPA_RESULT_OK)
            return result;
    }
    return GPA_RESULT_OK;
}

gpaResult gpaTurboExpanderV2Element::calcFuncValues(const gpaConstVector&, const gpaConstVector& variables,
                                                      gpaVector& values, const gpaSetOfEquations&)
{
    if (variables.size() < static_cast<gpaUInt>(turboVariables::count) ||
        values.size() < static_cast<gpaUInt>(turboEquations::count))
        return GPA_ERROR_WRONG_ARGS;
    m_currentSpeed = std::max(0.0, variables[static_cast<gpaUInt>(turboVariables::rotorSpeed)]);
    const gpaReal speedRatio = m_currentSpeed / m_nominalSpeed;
    const gpaReal rawTurbineValveOpening = variables[static_cast<gpaUInt>(turboVariables::turbineValveOpening)];
    const gpaReal rawCompressorValveOpening = variables[static_cast<gpaUInt>(turboVariables::compressorValveOpening)];
    m_currentTurbineValveOpening = std::clamp(rawTurbineValveOpening, 0.0, 1.0);
    m_currentCompressorValveOpening = std::clamp(rawCompressorValveOpening, 0.0, 1.0);
    const gpaReal nominalTurbineInletPressure = m_turbineBackPressure + m_nominalTurbinePressureDrop;
    // At an element port the signed incoming rates have opposite signs at
    // inlet/outlet.  The through-flow therefore has the same convention as
    // the existing gpaExpander: (G_out - G_in) / 2.
    m_compressorMassFlow = 0.5 *
        (m_compressorOutlet->getIncomingMassRate() - m_compressorInlet->getIncomingMassRate());
    m_turbineMassFlow = 0.5 *
        (m_turbineOutlet->getIncomingMassRate() - m_turbineInlet->getIncomingMassRate());

    // Each material circuit has one pressure equation and one signed
    // flow-balance equation.  The internal compressor valve is part of the
    // pressure equation, so the model exposes both wheel discharge pressure
    // and pressure after this valve without introducing a fifth external port.
    const gpaReal compressorPressureRise = gpa::turboExpander::compressorPressureRise(
        m_nominalCompressorHead, speedRatio, m_compressorMassFlow,
        m_nominalCompressorMassFlow, m_compressorHeadCoefficient);
    const gpaReal compressorInternalDrop = gpa::turboExpander::signedValvePressureDrop(
        m_nominalCompressorHead, m_nominalCompressorMassFlow, m_compressorMassFlow,
        std::max(0.0, m_compressorResistanceZeta));
    const gpaReal compressorValveDrop = gpa::turboExpander::valvePressureDrop(
        m_compressorValveZeta, m_compressorValveFlowArea, m_currentCompressorValveOpening,
        std::max(flowRegularization, m_compressorInlet->getStreamDensity()),
        m_compressorMassFlow, m_nominalCompressorMassFlow);
    m_compressorDischargePressure = m_compressorInlet->getStreamPressure() + compressorPressureRise;
    m_compressorOutletPressure = m_compressorDischargePressure - compressorInternalDrop - compressorValveDrop;

    const gpaReal boundedTurbineFlow = std::max(flowRegularization, m_nominalTurbineMassFlow);
    const gpaReal turbineValveDrop = gpa::turboExpander::valvePressureDrop(
        m_turbineValveZeta, m_turbineValveFlowArea, m_currentTurbineValveOpening,
        std::max(flowRegularization, m_turbineInlet->getStreamDensity()),
        m_turbineMassFlow, boundedTurbineFlow);
    const gpaReal turbineInternalDrop = gpa::turboExpander::signedValvePressureDrop(
        m_nominalTurbinePressureDrop, boundedTurbineFlow, m_turbineMassFlow,
        std::max(0.0, m_turbineResistanceZeta));

    // The inlet port is the pressure upstream of the valve. The valve loss is
    // subtracted before evaluating the turbine map, leaving one continuous
    // pressure relation for both closed and open positions.
    m_turbineStageInletPressure = std::max(
        flowRegularization, m_turbineInlet->getStreamPressure() - turbineValveDrop);

    const gpaReal turbineMapCoefficient = nominalTurbineInletPressure > 0.0
        ? -std::log(std::clamp(m_turbineBackPressure / nominalTurbineInletPressure, 1.0e-9, 1.0))
        : 0.0;
    const gpaReal turbineMapPressure = gpa::turboExpander::turbineMapPressure(
        m_turbineStageInletPressure, m_turbineMassFlow, boundedTurbineFlow,
        nominalTurbineInletPressure, turbineMapCoefficient, speedRatio,
        m_turbineSpeedFactor);
    m_turbineFormulaHeatCapacity = getSpecificHeatCapacity(
        m_turbineInlet, m_turbineHeatCapacity);
    m_turbineFormulaHeatCapacityRatio = getHeatCapacityRatio(
        m_turbineInlet, m_turbineHeatCapacityRatio);
    m_compressorFormulaHeatCapacity = getSpecificHeatCapacity(
        m_compressorInlet, m_compressorHeatCapacity);
    m_compressorFormulaHeatCapacityRatio = getHeatCapacityRatio(
        m_compressorInlet, m_compressorHeatCapacityRatio);
    const gpaReal turbineFormulaTemperature = m_turbineInlet->getStreamTemperature() > 0.0
        ? m_turbineInlet->getStreamTemperature() : m_turbineTemperature;
    const gpaReal turbineIsentropicEnthalpyDrop = gpa::turboExpander::isentropicTurbineDrop(
        m_turbineStageInletPressure, m_turbineOutlet->getStreamPressure(),
        m_turbineFormulaHeatCapacity * kilopascalToPascal, turbineFormulaTemperature,
        m_turbineFormulaHeatCapacityRatio);
    const gpaReal turbineEnthalpyDrop = turbineIsentropicEnthalpyDrop *
        std::clamp(m_turbineEfficiency, 0.0, 1.0);
    const gpaReal compressorIsentropicEnthalpyRise = gpa::turboExpander::isentropicCompressorRise(
        m_compressorInlet->getStreamPressure(), m_compressorDischargePressure,
        m_compressorFormulaHeatCapacity * kilopascalToPascal,
        m_compressorInlet->getStreamTemperature(), m_compressorFormulaHeatCapacityRatio);
    const gpaReal compressorEnthalpyRise = compressorIsentropicEnthalpyRise /
        std::max(1.0e-6, m_compressorEfficiency);
    const gpaReal positiveCompressorMassFlow = std::max(0.0, m_compressorMassFlow);
    const gpaReal positiveTurbineMassFlow = std::max(0.0, m_turbineMassFlow);
    // The P-P model transports the prescribed, passport enthalpy changes.
    // The correlations remain diagnostics: they are useful for calibration,
    // but inserting them into the stream equations makes the PH flash stiff
    // when pressure and flow are solved simultaneously.
    m_compressorFormulaEnthalpyChange = compressorEnthalpyRise / kilopascalToPascal;
    m_turbineFormulaEnthalpyChange = -turbineEnthalpyDrop / kilopascalToPascal;
    m_compressorPower = positiveCompressorMassFlow *
        std::max(0.0, m_compressorEnthalpyChange) * kilopascalToPascal;
    m_turbinePower = positiveTurbineMassFlow *
        std::max(0.0, -m_turbineEnthalpyChange) * kilopascalToPascal;
    // These diagnostic powers use the same solved mass flow as the working
    // model, but the pressure-ratio correlations for Delta h.  They make the
    // calibration error visible without feeding it back into PH flashes or
    // into the shaft dynamics.
    m_compressorFormulaPower = positiveCompressorMassFlow *
        std::max(0.0, m_compressorFormulaEnthalpyChange) * kilopascalToPascal;
    m_turbineFormulaPower = positiveTurbineMassFlow *
        std::max(0.0, -m_turbineFormulaEnthalpyChange) * kilopascalToPascal;
    // The shaft load must follow the actual gas work, not an independent N^2
    // curve. At standstill P / omega is singular; using the nominal angular
    // speed as a lower bound preserves a finite starting torque. Above the
    // nominal speed this is the exact relation M = P / omega.
    const gpaReal nominalAngularSpeed = m_nominalSpeed * 2.0 * std::numbers::pi / 60.0;
    const gpaReal currentAngularSpeed = m_currentSpeed * 2.0 * std::numbers::pi / 60.0;
    const gpaReal torqueAngularSpeed = std::max(nominalAngularSpeed, currentAngularSpeed);
    m_turbineTorque = m_turbinePower / torqueAngularSpeed;
    m_compressorTorque = m_compressorPower / torqueAngularSpeed;
    m_lossTorque = gpa::turboExpander::lossTorque(m_nominalShaftTorque, speedRatio,
                                                  m_bearingLossZeta, m_windageLossZeta);
    m_shaftAcceleration = (m_turbineTorque - m_compressorTorque - m_lossTorque) /
        m_shaftInertia * 60.0 / (2.0 * std::numbers::pi);
    values[static_cast<gpaUInt>(turboEquations::rotorDynamics)] = m_shaftAcceleration;
    values[static_cast<gpaUInt>(turboEquations::turbineValveDynamics)] =
        (std::clamp(m_valveOpening, 0.0, 1.0) - rawTurbineValveOpening) / m_turbineValveTimeConstant;
    values[static_cast<gpaUInt>(turboEquations::compressorValveDynamics)] =
        (std::clamp(m_compressorValveOpening, 0.0, 1.0) - rawCompressorValveOpening) /
        m_compressorValveTimeConstant;
    values[static_cast<gpaUInt>(turboEquations::compressorPressureBalance)] =
        m_compressorOutlet->getStreamPressure() - m_compressorOutletPressure;
    values[static_cast<gpaUInt>(turboEquations::compressorFlowBalance)] =
        m_compressorInlet->getIncomingMassRate() + m_compressorOutlet->getIncomingMassRate();
    values[static_cast<gpaUInt>(turboEquations::turbinePressureBalance)] =
        turbineMapPressure - turbineInternalDrop - m_turbineOutlet->getStreamPressure();
    values[static_cast<gpaUInt>(turboEquations::turbineFlowBalance)] =
        m_turbineInlet->getIncomingMassRate() + m_turbineOutlet->getIncomingMassRate();

    return GPA_RESULT_OK;
}

gpaResult gpaTurboExpanderV2Element::calcEnthalpyFuncValueAt(gpaUInt circuit, gpaUInt port,
                                                               gpaReal enthalpy, gpaReal& value) const
{
    const gpaMixPort* opposite = getMixPort(circuit, 1 - port);
    if (!opposite || !opposite->getStreamMedium() || circuit > turbineCircuit)
        return GPA_ERROR_WRONG_ARGS;
    const gpaReal passportEnthalpyChange = circuit == compressorCircuit
        ? m_compressorEnthalpyChange : m_turbineEnthalpyChange;
    // This follows the established P-P element convention in this library:
    // the configured passport value is used directly in the stream residual.
    // It remains bounded at a closed valve and avoids a PH flash excursion.
    value = enthalpy - opposite->getStreamMedium()->getEnthalpy() - passportEnthalpyChange;
    return GPA_RESULT_OK;
}

gpaResult gpaTurboExpanderV2Element::calcFractionsFuncValuesAt(gpaUInt circuit, gpaUInt port,
                                                                 const gpaConstVector& fractions, gpaVector& values) const
{
    const gpaMixPort* opposite = getMixPort(circuit, 1 - port);
    if (!opposite || circuit > turbineCircuit || fractions.size() < values.size())
        return GPA_ERROR_WRONG_ARGS;
    const gpaConstVector source = opposite->getStreamMedium()->getFractions();
    if (source.size() < values.size())
        return GPA_ERROR_WRONG_ARGS;
    for (gpaUInt i = 0; i < values.size(); ++i)
        values[i] = fractions[i] - source[i];
    return GPA_RESULT_OK;
}
