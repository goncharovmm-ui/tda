#include "gpaTurboExpanderV2.h"

#include <cmath>

namespace { enum parameter : gpaUInt { shaftInertia, nominalSpeed, initialSpeed, nominalCompressorMassFlow,
 nominalTurbineMassFlow, nominalCompressorHead, nominalTurbinePressureDrop, compressorHeadCoefficient,
 compressorResistanceZeta, compressorValveZeta, compressorValveFlowArea, compressorValveTimeConstant,
 turbineResistanceZeta, turbineValveZeta, turbineValveFlowArea, turbineValveTimeConstant,
 turbineBackPressure, turbineTemperature, turbineHeatCapacity,
 turbineHeatCapacityRatio, turbineEfficiency, compressorHeatCapacity, compressorHeatCapacityRatio,
 compressorEfficiency, compressorEnthalpyChange, turbineEnthalpyChange, turbineSpeedFactor, nominalShaftTorque, bearingLossZeta, windageLossZeta,
 turbineValveOpening, compressorValveOpening, COUNT }; }
const gpaString gpaTurboExpanderV2::m_typeName = "turbo-expander-v2";
const gpaString gpaTurboExpanderV2::m_description = "Турбодетандер с компрессором на общем валу";
const gpaStringVector gpaTurboExpanderV2::m_circuitNames = {"Компрессор", "Детандер"};
const gpaStringVector gpaTurboExpanderV2::m_portNames = {"Вход компрессора", "Выход компрессора", "Вход детандера", "Выход детандера"};
const gpaParameterInfoVector gpaTurboExpanderV2::m_parameters = {
 realParam("shaft-inertia", "Момент инерции ротора", GPA_DIMLESS_UNIT, false, 300),
 realParam("nominal-speed", "Номинальная частота вращения, об/мин", GPA_DIMLESS_UNIT, false, 10560),
 realParam("initial-speed", "Начальная частота вращения, об/мин", GPA_DIMLESS_UNIT, false, 0),
 realParam("nominal-compressor-mass-flow", "Номинальный расход компрессора", GPA_MASS_RATE_UNIT, false, 137.26),
 realParam("nominal-turbine-mass-flow", "Номинальный расход детандера", GPA_MASS_RATE_UNIT, false, 95.85),
 realParam("nominal-compressor-head", "Номинальный напор компрессора", GPA_PRES_DIFFER_UNIT, false, 741),
 realParam("nominal-turbine-pressure-drop", "Номинальный перепад детандера", GPA_PRES_DIFFER_UNIT, false, 5046),
 realParam("compressor-head-coefficient", "Коэффициент напора компрессора", GPA_DIMLESS_UNIT, false, 1),
 realParam("compressor-resistance-zeta", "Сопротивление проточной части компрессора", GPA_DIMLESS_UNIT, false, 0),
 realParam("compressor-valve-zeta", "Номинальное местное сопротивление клапана компрессора", GPA_DIMLESS_UNIT, false, 1),
 realParam("compressor-valve-flow-area", "Номинальная площадь прохода клапана компрессора", GPA_AREA_UNIT, false, 0.027),
 realParam("compressor-valve-time-constant", "Постоянная времени привода клапана компрессора, с", GPA_TIME_UNIT, false, 2),
 realParam("turbine-resistance-zeta", "Сопротивление проточной части детандера", GPA_DIMLESS_UNIT, false, 0),
 realParam("turbine-valve-zeta", "Номинальное местное сопротивление клапана детандера", GPA_DIMLESS_UNIT, false, 1),
 realParam("turbine-valve-flow-area", "Номинальная площадь прохода клапана детандера", GPA_AREA_UNIT, false, 0.010),
 realParam("turbine-valve-time-constant", "Постоянная времени привода клапана детандера, с", GPA_TIME_UNIT, false, 2),
 realParam("turbine-back-pressure", "Номинальное давление за детандером для характеристики, кПа", GPA_PRESSURE_UNIT, false, 2944),
 realParam("turbine-temperature", "Температура газа детандера, K", GPA_TEMPERATURE_UNIT, false, 263.91),
 realParam("turbine-heat-capacity", "Удельная теплоёмкость газа детандера, кДж/(кг K)", GPA_MASS_HEAT_CAPACITY_UNIT, false, 2.041),
 realParam("turbine-heat-capacity-ratio", "Показатель адиабаты детандера", GPA_DIMLESS_UNIT, false, 1.70),
 realParam("turbine-efficiency", "Адиабатический КПД детандера", GPA_DIMLESS_UNIT, false, 0.866),
 realParam("compressor-heat-capacity", "Удельная теплоёмкость газа компрессора, кДж/(кг K)", GPA_MASS_HEAT_CAPACITY_UNIT, false, 2.041),
 realParam("compressor-heat-capacity-ratio", "Показатель адиабаты компрессора", GPA_DIMLESS_UNIT, false, 1.376),
 realParam("compressor-efficiency", "Адиабатический КПД компрессора", GPA_DIMLESS_UNIT, false, 0.808),
 realParam("compressor-enthalpy-change", "Номинальный прирост энтальпии компрессора, кДж/кг", GPA_MASS_ENERGY_UNIT, false, 46.495702),
 realParam("turbine-enthalpy-change", "Номинальное изменение энтальпии детандера, кДж/кг", GPA_MASS_ENERGY_UNIT, false, -67.282212),
 realParam("turbine-speed-factor", "Поправка карты детандера по частоте", GPA_DIMLESS_UNIT, false, 0.10),
 realParam("nominal-shaft-torque", "Номинальный момент общего вала, Н м", GPA_DIMLESS_UNIT, false, 5800),
 realParam("bearing-loss-zeta", "Потери в подшипниках", GPA_DIMLESS_UNIT, false, 0.004),
 realParam("windage-loss-zeta", "Вентиляционные потери", GPA_DIMLESS_UNIT, false, 0.0064),
 realParam("turbine-valve-opening", "Открытие клапана детандера", GPA_DIMLESS_UNIT, true, 0),
 realParam("compressor-valve-opening", "Открытие клапана компрессора", GPA_DIMLESS_UNIT, true, 1)};
const gpaSensorInfoVector gpaTurboExpanderV2::m_sensors = {
 scalarSensor("rotation-speed", "Частота вращения ротора, об/мин", GPA_DIMLESS_UNIT),
 scalarSensor("compressor-power", "Мощность компрессора, кВт", GPA_HEAT_RATE_UNIT),
 scalarSensor("turbine-power", "Мощность детандера, кВт", GPA_HEAT_RATE_UNIT),
 scalarSensor("loss-torque", "Момент механических потерь, Н·м", GPA_DIMLESS_UNIT),
 scalarSensor("turbine-mass-flow", "Массовый расход через детандер, кг/с", GPA_MASS_RATE_UNIT),
 scalarSensor("turbine-stage-inlet-pressure", "Давление перед ступенью детандера, кПа", GPA_PRESSURE_UNIT),
 scalarSensor("compressor-mass-flow", "Массовый расход через компрессор, кг/с", GPA_MASS_RATE_UNIT),
 scalarSensor("compressor-discharge-pressure", "Давление после колеса компрессора, кПа", GPA_PRESSURE_UNIT),
 scalarSensor("compressor-outlet-pressure", "Давление после клапана компрессора, кПа", GPA_PRESSURE_UNIT),
 scalarSensor("turbine-torque", "Момент детандера, Н·м", GPA_DIMLESS_UNIT),
 scalarSensor("compressor-torque", "Момент компрессора, Н·м", GPA_DIMLESS_UNIT),
 scalarSensor("shaft-acceleration", "Ускорение вала, об/(мин·с)", GPA_DIMLESS_UNIT),
 scalarSensor("compressor-enthalpy-change", "Прирост энтальпии в компрессоре, кДж/кг", GPA_MASS_ENERGY_UNIT),
 scalarSensor("turbine-enthalpy-change", "Изменение энтальпии в детандере, кДж/кг", GPA_MASS_ENERGY_UNIT),
 scalarSensor("compressor-enthalpy-formula", "Прирост энтальпии компрессора по формуле, кДж/кг", GPA_MASS_ENERGY_UNIT),
 scalarSensor("turbine-enthalpy-formula", "Изменение энтальпии детандера по формуле, кДж/кг", GPA_MASS_ENERGY_UNIT),
 scalarSensor("compressor-enthalpy-deviation", "Отклонение формулы от константы компрессора, кДж/кг", GPA_MASS_ENERGY_UNIT),
 scalarSensor("turbine-enthalpy-deviation", "Отклонение формулы от константы детандера, кДж/кг", GPA_MASS_ENERGY_UNIT),
 scalarSensor("compressor-formula-power", "Мощность компрессора по расчётному Δh, кВт", GPA_HEAT_RATE_UNIT),
 scalarSensor("turbine-formula-power", "Мощность детандера по расчётному Δh, кВт", GPA_HEAT_RATE_UNIT),
 scalarSensor("compressor-power-to-nominal", "Отношение мощности компрессора к номинальной", GPA_DIMLESS_UNIT),
 scalarSensor("turbine-power-to-nominal", "Отношение мощности детандера к номинальной", GPA_DIMLESS_UNIT),
 scalarSensor("compressor-inlet-enthalpy", "Энтальпия на входе компрессора, кДж/кмоль", GPA_MOLAR_ENERGY_UNIT),
 scalarSensor("compressor-outlet-enthalpy", "Энтальпия на выходе компрессора, кДж/кмоль", GPA_MOLAR_ENERGY_UNIT),
 scalarSensor("turbine-inlet-enthalpy", "Энтальпия на входе детандера, кДж/кмоль", GPA_MOLAR_ENERGY_UNIT),
 scalarSensor("turbine-outlet-enthalpy", "Энтальпия на выходе детандера, кДж/кмоль", GPA_MOLAR_ENERGY_UNIT),
 scalarSensor("turbine-valve-opening", "Фактическое открытие клапана детандера", GPA_DIMLESS_UNIT),
 scalarSensor("compressor-valve-opening", "Фактическое открытие клапана компрессора", GPA_DIMLESS_UNIT)};
gpaTurboExpanderV2::gpaTurboExpanderV2(const gpaCoreInterfaces* i, gpaCoreReference r) : gpaExportSystem(i, r), m_element(asParent()) {}
gpaResult gpaTurboExpanderV2::checkParameterValues(const gpaParameterValueVector& p) const { if (p.size() < COUNT) return GPA_ERROR_WRONG_ARGS; for(gpaUInt i=0;i<COUNT;++i) if(!std::isfinite(p[i].getReal())) return GPA_ERROR_WRONG_ARGS; return p[shaftInertia].getReal()>0 && p[nominalSpeed].getReal()>0 ? GPA_RESULT_OK : GPA_ERROR_WRONG_ARGS; }
gpaResult gpaTurboExpanderV2::setParameterValues(const gpaParameterValueVector& p, bool check) { if(check){auto r=checkParameterValues(p);if(r!=GPA_RESULT_OK)return r;} m_element.setShaftInertia(p[shaftInertia].getReal());m_element.setNominalSpeed(p[nominalSpeed].getReal());m_element.setInitialSpeed(p[initialSpeed].getReal());m_element.setNominalCompressorMassFlow(p[nominalCompressorMassFlow].getReal());m_element.setNominalTurbineMassFlow(p[nominalTurbineMassFlow].getReal());m_element.setNominalCompressorHead(p[nominalCompressorHead].getReal());m_element.setNominalTurbinePressureDrop(p[nominalTurbinePressureDrop].getReal());m_element.setCompressorHeadCoefficient(p[compressorHeadCoefficient].getReal());m_element.setCompressorResistanceZeta(p[compressorResistanceZeta].getReal());m_element.setCompressorValveZeta(p[compressorValveZeta].getReal());m_element.setCompressorValveFlowArea(p[compressorValveFlowArea].getReal());m_element.setCompressorValveTimeConstant(p[compressorValveTimeConstant].getReal());m_element.setTurbineResistanceZeta(p[turbineResistanceZeta].getReal());m_element.setTurbineValveZeta(p[turbineValveZeta].getReal());m_element.setTurbineValveFlowArea(p[turbineValveFlowArea].getReal());m_element.setTurbineValveTimeConstant(p[turbineValveTimeConstant].getReal());m_element.setTurbineBackPressure(p[turbineBackPressure].getReal());m_element.setTurbineTemperature(p[turbineTemperature].getReal());m_element.setTurbineHeatCapacity(p[turbineHeatCapacity].getReal());m_element.setTurbineHeatCapacityRatio(p[turbineHeatCapacityRatio].getReal());m_element.setTurbineEfficiency(p[turbineEfficiency].getReal());m_element.setCompressorHeatCapacity(p[compressorHeatCapacity].getReal());m_element.setCompressorHeatCapacityRatio(p[compressorHeatCapacityRatio].getReal());m_element.setCompressorEfficiency(p[compressorEfficiency].getReal());m_element.setCompressorEnthalpyChange(p[compressorEnthalpyChange].getReal());m_element.setTurbineEnthalpyChange(p[turbineEnthalpyChange].getReal());m_element.setTurbineSpeedFactor(p[turbineSpeedFactor].getReal());m_element.setNominalShaftTorque(p[nominalShaftTorque].getReal());m_element.setBearingLossZeta(p[bearingLossZeta].getReal());m_element.setWindageLossZeta(p[windageLossZeta].getReal());m_element.setValveOpening(p[turbineValveOpening].getReal());m_element.setCompressorValveOpening(p[compressorValveOpening].getReal());return GPA_RESULT_OK; }
gpaResult gpaTurboExpanderV2::setSignals(const gpaConstVector& s) { gpaReal value{NAN}; if(getSignalValue(s,turbineValveOpening,value)){ if(!std::isfinite(value)) return GPA_ERROR_WRONG_ARGS; m_element.setValveOpening(value); } if(getSignalValue(s,compressorValveOpening,value)){ if(!std::isfinite(value)) return GPA_ERROR_WRONG_ARGS; m_element.setCompressorValveOpening(value); } return GPA_RESULT_OK; }
gpaResult gpaTurboExpanderV2::initialize(){ const gpaResult linkResult=linkToCoreStreams(); if(linkResult!=GPA_RESULT_OK)return linkResult; bool ok=true; for(gpaUInt c=0;c<2;++c){ok=ok&&m_element.connectMixStream(c,0,getEdgeMixStream(c,0),GPA_STREAM_INLET);ok=ok&&m_element.connectMixStream(c,1,getEdgeMixStream(c,1),GPA_STREAM_OUTLET);} if(!ok)return GPA_ERROR_WRONG_ARGS;setElements({&m_element});setStreams({});if(!createSimStructures({}, {getOuterEntrance(0,0),getOuterEntrance(1,0)}))return GPA_ERROR_WRONG_ARGS;return gpaExportSystem::initialize(); }
gpaResult gpaTurboExpanderV2::calcSensors(gpaVector& s) const
{
    if (s.size() < m_sensors.size())
        return GPA_ERROR_WRONG_ARGS;
    setSensorValue(s, 0, m_element.getSpeed());
    setSensorValue(s, 1, m_element.getCompressorPower() / 1000);
    setSensorValue(s, 2, m_element.getTurbinePower() / 1000);
    setSensorValue(s, 3, m_element.getLossTorque());
    setSensorValue(s, 4, m_element.getTurbineMassFlow());
    setSensorValue(s, 5, m_element.getTurbineStageInletPressure());
    setSensorValue(s, 6, m_element.getCompressorMassFlow());
    setSensorValue(s, 7, m_element.getCompressorDischargePressure());
    setSensorValue(s, 8, m_element.getCompressorOutletPressure());
    setSensorValue(s, 9, m_element.getTurbineTorque());
    setSensorValue(s, 10, m_element.getCompressorTorque());
    setSensorValue(s, 11, m_element.getShaftAcceleration());
    setSensorValue(s, 12, m_element.getCompressorEnthalpyChange());
    setSensorValue(s, 13, m_element.getTurbineEnthalpyChange());
    setSensorValue(s, 14, m_element.getCompressorFormulaEnthalpyChange());
    setSensorValue(s, 15, m_element.getTurbineFormulaEnthalpyChange());
    setSensorValue(s, 16, m_element.getCompressorFormulaEnthalpyChange() - m_element.getCompressorEnthalpyChange());
    setSensorValue(s, 17, m_element.getTurbineFormulaEnthalpyChange() - m_element.getTurbineEnthalpyChange());
    setSensorValue(s, 18, m_element.getCompressorFormulaPower() / 1000);
    setSensorValue(s, 19, m_element.getTurbineFormulaPower() / 1000);
    setSensorValue(s, 20, m_element.getCompressorPower() /
        std::max(1.0, m_element.getCompressorNominalPower()));
    setSensorValue(s, 21, m_element.getTurbinePower() /
        std::max(1.0, m_element.getTurbineNominalPower()));
    setSensorValue(s, 22, m_element.getCompressorInletEnthalpy());
    setSensorValue(s, 23, m_element.getCompressorOutletEnthalpy());
    setSensorValue(s, 24, m_element.getTurbineInletEnthalpy());
    setSensorValue(s, 25, m_element.getTurbineOutletEnthalpy());
    setSensorValue(s, 26, m_element.getTurbineValveOpening());
    setSensorValue(s, 27, m_element.getCompressorValveOpening());

    return GPA_RESULT_OK;
}
const gpaString* gpaTurboExpanderV2::getMixCircuitName(gpaUInt i) const{return i<2?&m_circuitNames[i]:nullptr;}
const gpaString* gpaTurboExpanderV2::getMixPortName(gpaUInt c,gpaUInt p) const{return c<2&&p<2?&m_portNames[c*2+p]:nullptr;}
