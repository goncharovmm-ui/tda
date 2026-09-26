#ifndef GPA_TURBOEXPANDER_V2_ELEMENT_H
#define GPA_TURBOEXPANDER_V2_ELEMENT_H

#include <algorithm>

#include "gpaFlowElement.h"

class gpaTurboExpanderV2Element : public gpaFlowElement
{
public:
    gpaTurboExpanderV2Element(const gpaParentLink* link = nullptr);
    gpaTurboExpanderV2Element(const gpaTurboExpanderV2Element&) = delete;
    ~gpaTurboExpanderV2Element() override = default;

    gpaTurboExpanderV2Element* copy() const override { return nullptr; }
    const gpaString& getTypeName() const override;
    const gpaString& getDescription() const override;
    gpaUInt getNumMixCircuits() const override { return 2; }
    gpaUInt getMinNumMixPorts(gpaUInt index) const override { return index < 2 ? 2 : 0; }
    gpaUInt getMaxNumMixPorts(gpaUInt index) const override { return index < 2 ? 2 : 0; }
    gpaUInt getNumVariables() const override { return 3; }
    void fillVarNames(gpaStringVector& names) const override;
    void fillEquationNames(gpaStringVector& names) const override;
    gpaResult initialize() override;
    gpaResult initVariables(gpaVector& variables) const override;
    gpaResult fillMassMatrix(gpaUInt row, gpaUInt column) const override;
    gpaResult calcFuncValues(const gpaConstVector&, const gpaConstVector& variables,
                             gpaVector& values, const gpaSetOfEquations&) override;
    gpaResult calcEnthalpyFuncValueAt(gpaUInt circuit, gpaUInt port, gpaReal enthalpy,
                                      gpaReal& value) const override;
    gpaResult calcFractionsFuncValuesAt(gpaUInt circuit, gpaUInt port,
                                        const gpaConstVector& fractions, gpaVector& values) const override;

    void setShaftInertia(gpaReal value) { m_shaftInertia = value; }
    void setNominalSpeed(gpaReal value) { m_nominalSpeed = value; }
    void setInitialSpeed(gpaReal value) { m_initialSpeed = value; }
    void setNominalCompressorMassFlow(gpaReal value) { m_nominalCompressorMassFlow = value; }
    void setNominalTurbineMassFlow(gpaReal value) { m_nominalTurbineMassFlow = value; }
    void setNominalCompressorHead(gpaReal value) { m_nominalCompressorHead = value; }
    void setNominalTurbinePressureDrop(gpaReal value) { m_nominalTurbinePressureDrop = value; }
    void setCompressorHeadCoefficient(gpaReal value) { m_compressorHeadCoefficient = value; }
    void setCompressorResistanceZeta(gpaReal value) { m_compressorResistanceZeta = value; }
    void setCompressorValveZeta(gpaReal value) { m_compressorValveZeta = value; }
    void setCompressorValveFlowArea(gpaReal value) { m_compressorValveFlowArea = value; }
    void setCompressorValveTimeConstant(gpaReal value) { m_compressorValveTimeConstant = value; }
    void setTurbineResistanceZeta(gpaReal value) { m_turbineResistanceZeta = value; }
    void setTurbineValveZeta(gpaReal value) { m_turbineValveZeta = value; }
    void setTurbineValveFlowArea(gpaReal value) { m_turbineValveFlowArea = value; }
    void setTurbineValveTimeConstant(gpaReal value) { m_turbineValveTimeConstant = value; }
    void setTurbineBackPressure(gpaReal value) { m_turbineBackPressure = value; }
    void setTurbineTemperature(gpaReal value) { m_turbineTemperature = value; }
    void setTurbineHeatCapacity(gpaReal value) { m_turbineHeatCapacity = value; }
    void setTurbineHeatCapacityRatio(gpaReal value) { m_turbineHeatCapacityRatio = value; }
    void setTurbineEfficiency(gpaReal value) { m_turbineEfficiency = value; }
    void setCompressorHeatCapacity(gpaReal value) { m_compressorHeatCapacity = value; }
    void setCompressorHeatCapacityRatio(gpaReal value) { m_compressorHeatCapacityRatio = value; }
    void setCompressorEfficiency(gpaReal value) { m_compressorEfficiency = value; }
    void setCompressorEnthalpyChange(gpaReal value) { m_compressorEnthalpyChange = value; }
    void setTurbineEnthalpyChange(gpaReal value) { m_turbineEnthalpyChange = value; }
    void setTurbineSpeedFactor(gpaReal value) { m_turbineSpeedFactor = value; }
    void setNominalShaftTorque(gpaReal value) { m_nominalShaftTorque = value; }
    void setBearingLossZeta(gpaReal value) { m_bearingLossZeta = value; }
    void setWindageLossZeta(gpaReal value) { m_windageLossZeta = value; }
    void setValveOpening(gpaReal value) { m_valveOpening = value; }
    void setCompressorValveOpening(gpaReal value) { m_compressorValveOpening = value; }

    [[nodiscard]] gpaReal getSpeed() const { return m_currentSpeed; }
    [[nodiscard]] gpaReal getCompressorPower() const { return m_compressorPower; }
    [[nodiscard]] gpaReal getTurbinePower() const { return m_turbinePower; }
    [[nodiscard]] gpaReal getLossTorque() const { return m_lossTorque; }
    [[nodiscard]] gpaReal getTurbineMassFlow() const { return m_turbineMassFlow; }
    [[nodiscard]] gpaReal getTurbineStageInletPressure() const { return m_turbineStageInletPressure; }
    [[nodiscard]] gpaReal getCompressorMassFlow() const { return m_compressorMassFlow; }
    [[nodiscard]] gpaReal getCompressorDischargePressure() const { return m_compressorDischargePressure; }
    [[nodiscard]] gpaReal getCompressorOutletPressure() const { return m_compressorOutletPressure; }
    [[nodiscard]] gpaReal getTurbineTorque() const { return m_turbineTorque; }
    [[nodiscard]] gpaReal getCompressorTorque() const { return m_compressorTorque; }
    [[nodiscard]] gpaReal getShaftAcceleration() const { return m_shaftAcceleration; }
    [[nodiscard]] gpaReal getCompressorEnthalpyChange() const { return m_compressorEnthalpyChange; }
    [[nodiscard]] gpaReal getTurbineEnthalpyChange() const { return m_turbineEnthalpyChange; }
    [[nodiscard]] gpaReal getCompressorFormulaEnthalpyChange() const { return m_compressorFormulaEnthalpyChange; }
    [[nodiscard]] gpaReal getTurbineFormulaEnthalpyChange() const { return m_turbineFormulaEnthalpyChange; }
    [[nodiscard]] gpaReal getCompressorFormulaPower() const { return m_compressorFormulaPower; }
    [[nodiscard]] gpaReal getTurbineFormulaPower() const { return m_turbineFormulaPower; }
    [[nodiscard]] gpaReal getCompressorNominalPower() const
    {
        return std::max(0.0, m_nominalCompressorMassFlow) *
            std::max(0.0, m_compressorEnthalpyChange) * 1000.0;
    }
    [[nodiscard]] gpaReal getTurbineNominalPower() const
    {
        return std::max(0.0, m_nominalTurbineMassFlow) *
            std::max(0.0, -m_turbineEnthalpyChange) * 1000.0;
    }
    [[nodiscard]] gpaReal getCompressorInletEnthalpy() const
    {
        return m_compressorInlet && m_compressorInlet->getStreamMedium()
            ? m_compressorInlet->getStreamMedium()->getEnthalpy() : 0.0;
    }
    [[nodiscard]] gpaReal getCompressorOutletEnthalpy() const
    {
        return m_compressorOutlet && m_compressorOutlet->getStreamMedium()
            ? m_compressorOutlet->getStreamMedium()->getEnthalpy() : 0.0;
    }
    [[nodiscard]] gpaReal getTurbineInletEnthalpy() const
    {
        return m_turbineInlet && m_turbineInlet->getStreamMedium()
            ? m_turbineInlet->getStreamMedium()->getEnthalpy() : 0.0;
    }
    [[nodiscard]] gpaReal getTurbineOutletEnthalpy() const
    {
        return m_turbineOutlet && m_turbineOutlet->getStreamMedium()
            ? m_turbineOutlet->getStreamMedium()->getEnthalpy() : 0.0;
    }
    [[nodiscard]] gpaReal getTurbineValveOpening() const { return m_currentTurbineValveOpening; }
    [[nodiscard]] gpaReal getCompressorValveOpening() const { return m_currentCompressorValveOpening; }

private:
    gpaMixPort* m_compressorInlet{nullptr};
    gpaMixPort* m_compressorOutlet{nullptr};
    gpaMixPort* m_turbineInlet{nullptr};
    gpaMixPort* m_turbineOutlet{nullptr};
    gpaReal m_shaftInertia{300.0};
    gpaReal m_nominalSpeed{10560.0};
    gpaReal m_initialSpeed{0.0};
    gpaReal m_nominalCompressorMassFlow{137.26};
    gpaReal m_nominalTurbineMassFlow{95.85};
    gpaReal m_nominalCompressorHead{741.0};
    gpaReal m_nominalTurbinePressureDrop{5046.0};
    gpaReal m_compressorHeadCoefficient{1.0};
    gpaReal m_compressorResistanceZeta{0.0};
    gpaReal m_compressorValveZeta{1.0};
    gpaReal m_compressorValveFlowArea{0.027};
    gpaReal m_compressorValveTimeConstant{2.0};
    gpaReal m_turbineResistanceZeta{0.0};
    gpaReal m_turbineValveZeta{1.0};
    gpaReal m_turbineValveFlowArea{0.010};
    gpaReal m_turbineValveTimeConstant{2.0};
    gpaReal m_turbineBackPressure{2944.0};
    gpaReal m_turbineTemperature{263.91};
    gpaReal m_turbineHeatCapacity{2.041};
    gpaReal m_turbineHeatCapacityRatio{1.70};
    gpaReal m_turbineEfficiency{0.866};
    gpaReal m_compressorHeatCapacity{2.041};
    gpaReal m_compressorHeatCapacityRatio{1.376};
    gpaReal m_compressorEfficiency{0.808};
    gpaReal m_turbineSpeedFactor{0.10};
    gpaReal m_nominalShaftTorque{5800.0};
    gpaReal m_bearingLossZeta{0.004};
    gpaReal m_windageLossZeta{0.0064};
    gpaReal m_valveOpening{0.0};
    gpaReal m_compressorValveOpening{1.0};
    gpaReal m_currentTurbineValveOpening{0.0};
    gpaReal m_currentCompressorValveOpening{1.0};
    gpaReal m_currentSpeed{0.0};
    gpaReal m_compressorPower{0.0};
    gpaReal m_turbinePower{0.0};
    gpaReal m_lossTorque{0.0};
    gpaReal m_turbineMassFlow{0.0};
    gpaReal m_turbineStageInletPressure{0.0};
    gpaReal m_compressorMassFlow{0.0};
    gpaReal m_compressorDischargePressure{0.0};
    gpaReal m_compressorOutletPressure{0.0};
    gpaReal m_turbineTorque{0.0};
    gpaReal m_compressorTorque{0.0};
    gpaReal m_shaftAcceleration{0.0};
    gpaReal m_compressorEnthalpyChange{0.0};
    gpaReal m_turbineEnthalpyChange{0.0};
    gpaReal m_compressorFormulaEnthalpyChange{0.0};
    gpaReal m_turbineFormulaEnthalpyChange{0.0};
    gpaReal m_compressorFormulaPower{0.0};
    gpaReal m_turbineFormulaPower{0.0};
};

#endif // GPA_TURBOEXPANDER_V2_ELEMENT_H
