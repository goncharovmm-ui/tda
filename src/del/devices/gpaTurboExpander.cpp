#include "gpaTurboExpander.h"
#include <iostream>

constexpr gpaUInt ID_CIRCUIT_COMPRESSOR = 0;
constexpr gpaUInt ID_CIRCUIT_EXPANDER = 1;
constexpr gpaUInt NUM_CIRCUITS = 2;

constexpr gpaUInt ID_PORT_INLET = 0;
constexpr gpaUInt ID_PORT_OUTLET = 1;
constexpr gpaUInt NUM_PORTS = 4;

// Gas Blower Parameters IDs
constexpr gpaUInt ID_PARAM_MOMENT_OF_INERTIA = 0x00;
constexpr gpaUInt ID_PARAM_RATED_SPEED = 0x01;
constexpr gpaUInt ID_PARAM_INITIAL_SPEED = 0x02;
constexpr gpaUInt ID_PARAM_RATED_MFR_KS = 0x03;
constexpr gpaUInt ID_PARAM_RELATIVE_MFR_KS_START = ID_PARAM_RATED_MFR_KS + 1;
constexpr gpaUInt ID_PARAM_POLYTROPIC_ETA_START = ID_PARAM_RELATIVE_MFR_KS_START + 10;
constexpr gpaUInt ID_PARAM_RATED_MFR_TDS = ID_PARAM_POLYTROPIC_ETA_START + 10;
constexpr gpaUInt ID_PARAM_RELATIVE_MFR_TDS_START = ID_PARAM_RATED_MFR_TDS + 1;
constexpr gpaUInt ID_PARAM_ADIABATIC_ETA_START = ID_PARAM_RELATIVE_MFR_TDS_START + 10;
// constexpr gpaUInt ID_PARAM_EFFICIENCY_VEC = 0x05;

constexpr gpaUInt ID_SENSOR_PRESSURE_DIFF = 0x00;
constexpr gpaUInt ID_SENSOR_TEMPERATURE_DIFF = 0x01;
constexpr gpaUInt ID_SENSOR_POWER = 0x02;
constexpr gpaUInt ID_SENSOR_ENERGY = 0x03;
constexpr gpaUInt ID_SENSOR_ELECTRIC_POWER = 0x04;
constexpr gpaUInt ID_SENSOR_ROTATION_SPEED = 0x05;
constexpr gpaUInt ID_SENSOR_POLYTROPIC_EFF = 0x06;
constexpr gpaUInt ID_SENSOR_ADIABATIC_HEAD = 0x07;
constexpr gpaUInt ID_SENSOR_POLYTROPIC_HEAD = 0x08;

const gpaString gpaTurboExpander::m_typeName = "turbo-expander";
const gpaString gpaTurboExpander::m_description = "Турбодетандер";

const gpaParameterInfoVector gpaTurboExpander::m_parameters =
    {
        realParam("moment-of-inertia", "Момент инерции рабочего колеса, кг·м^2", GPA_DIMLESS_UNIT, false, 100.0),
        realParam("rated-speed", "Номинальная частота вращения колеса", GPA_DIMLESS_UNIT, false, 10500.0),
        realParam("initial-speed", "Начальная частота вращения колеса", GPA_DIMLESS_UNIT, false, 5.0),
        realParam("mass-flow-ks", "Номинальный расход компрессора", GPA_MASS_RATE_UNIT, false, 0.0),
        realParam("relative-mass-flow-ks-1", "Относительный расход компрессора 1", GPA_DIMLESS_UNIT, false, 0.0),
        realParam("relative-mass-flow-ks-2", "Относительный расход компрессора 2", GPA_DIMLESS_UNIT, false, 0.0),
        realParam("relative-mass-flow-ks-3", "Относительный расход компрессора 3", GPA_DIMLESS_UNIT, false, 0.0),
        realParam("relative-mass-flow-ks-4", "Относительный расход компрессора 4", GPA_DIMLESS_UNIT, false, 0.0),
        realParam("relative-mass-flow-ks-5", "Относительный расход компрессора 5", GPA_DIMLESS_UNIT, false, 0.0),
        realParam("relative-mass-flow-ks-6", "Относительный расход компрессора 6", GPA_DIMLESS_UNIT, false, 0.0),
        realParam("relative-mass-flow-ks-7", "Относительный расход компрессора 7", GPA_DIMLESS_UNIT, false, 0.0),
        realParam("relative-mass-flow-ks-8", "Относительный расход компрессора 8", GPA_DIMLESS_UNIT, false, 0.0),
        realParam("relative-mass-flow-ks-9", "Относительный расход компрессора 9", GPA_DIMLESS_UNIT, false, 0.0),
        realParam("relative-mass-flow-ks-10", "Относительный расход компрессора 10", GPA_DIMLESS_UNIT, false, 0.0),
        realParam("polytropic-eta-ks-1", "Показатель политропы компрессора 1", GPA_DIMLESS_UNIT, false, 0.0),
        realParam("polytropic-eta-ks-2", "Показатель политропы компрессора 2", GPA_DIMLESS_UNIT, false, 0.0),
        realParam("polytropic-eta-ks-3", "Показатель политропы компрессора 3", GPA_DIMLESS_UNIT, false, 0.0),
        realParam("polytropic-eta-ks-4", "Показатель политропы компрессора 4", GPA_DIMLESS_UNIT, false, 0.0),
        realParam("polytropic-eta-ks-5", "Показатель политропы компрессора 5", GPA_DIMLESS_UNIT, false, 0.0),
        realParam("polytropic-eta-ks-6", "Показатель политропы компрессора 6", GPA_DIMLESS_UNIT, false, 0.0),
        realParam("polytropic-eta-ks-7", "Показатель политропы компрессора 7", GPA_DIMLESS_UNIT, false, 0.0),
        realParam("polytropic-eta-ks-8", "Показатель политропы компрессора 8", GPA_DIMLESS_UNIT, false, 0.0),
        realParam("polytropic-eta-ks-9", "Показатель политропы компрессора 9", GPA_DIMLESS_UNIT, false, 0.0),
        realParam("polytropic-eta-ks-10", "Показатель политропы компрессора 10", GPA_DIMLESS_UNIT, false, 0.0),
        realParam("mass-flow-tds", "Номинальный расход детандера", GPA_MASS_RATE_UNIT, false, 0.0),
        realParam("relative-mass-flow-tds-1", "Относительный расход детандера 1", GPA_DIMLESS_UNIT, false, 0.0),
        realParam("relative-mass-flow-tds-2", "Относительный расход детандера 2", GPA_DIMLESS_UNIT, false, 0.0),
        realParam("relative-mass-flow-tds-3", "Относительный расход детандера 3", GPA_DIMLESS_UNIT, false, 0.0),
        realParam("relative-mass-flow-tds-4", "Относительный расход детандера 4", GPA_DIMLESS_UNIT, false, 0.0),
        realParam("relative-mass-flow-tds-5", "Относительный расход детандера 5", GPA_DIMLESS_UNIT, false, 0.0),
        realParam("relative-mass-flow-tds-6", "Относительный расход детандера 6", GPA_DIMLESS_UNIT, false, 0.0),
        realParam("relative-mass-flow-tds-7", "Относительный расход детандера 7", GPA_DIMLESS_UNIT, false, 0.0),
        realParam("relative-mass-flow-tds-8", "Относительный расход детандера 8", GPA_DIMLESS_UNIT, false, 0.0),
        realParam("relative-mass-flow-tds-9", "Относительный расход детандера 9", GPA_DIMLESS_UNIT, false, 0.0),
        realParam("relative-mass-flow-tds-10", "Относительный расход детандера 10", GPA_DIMLESS_UNIT, false, 0.0),
        realParam("adibatic-eta-tds-1", "Показатель адиабаты детандера 1", GPA_DIMLESS_UNIT, false, 0.0),
        realParam("adibatic-eta-tds-2", "Показатель адиабаты детандера 2", GPA_DIMLESS_UNIT, false, 0.0),
        realParam("adibatic-eta-tds-3", "Показатель адиабаты детандера 3", GPA_DIMLESS_UNIT, false, 0.0),
        realParam("adibatic-eta-tds-4", "Показатель адиабаты детандера 4", GPA_DIMLESS_UNIT, false, 0.0),
        realParam("adibatic-eta-tds-5", "Показатель адиабаты детандера 5", GPA_DIMLESS_UNIT, false, 0.0),
        realParam("adibatic-eta-tds-6", "Показатель адиабаты детандера 6", GPA_DIMLESS_UNIT, false, 0.0),
        realParam("adibatic-eta-tds-7", "Показатель адиабаты детандера 7", GPA_DIMLESS_UNIT, false, 0.0),
        realParam("adibatic-eta-tds-8", "Показатель адиабаты детандера 8", GPA_DIMLESS_UNIT, false, 0.0),
        realParam("adibatic-eta-tds-9", "Показатель адиабаты детандера 9", GPA_DIMLESS_UNIT, false, 0.0),
        realParam("adibatic-eta-tds-10", "Показатель адиабаты детандера 10", GPA_DIMLESS_UNIT, false, 0.0),
        // realVecParam("mass-flow-vec", "Расход", GPA_MASS_RATE_UNIT, false, {}),
        // realVecParam("efficiency-vec", "КПД процесса", GPA_DIMLESS_UNIT, false, {}),
};

const gpaSensorInfoVector gpaTurboExpander::m_sensors =
    {
        scalarSensor("pressure-diff", "Перепад давления на компрессоре, Па", GPA_PRES_DIFFER_UNIT),
        scalarSensor("temperature-diff", "Перепад температуры на компрессоре, К", GPA_TEMP_DIFFER_UNIT),
        scalarSensor("power", "Полезная мощность компрессора, Вт", GPA_HEAT_RATE_UNIT),
        scalarSensor("energy", "Энергия, затраченная на процесс сжатия, Вт", GPA_HEAT_RATE_UNIT),
        scalarSensor("electric-power", "Потребляемая электрическая мощность аппарата, Вт", GPA_HEAT_RATE_UNIT),
        scalarSensor("rotation-speed", "Частота вращения ротора компрессора, Гц", GPA_DIMLESS_UNIT),
        scalarSensor("polytropic-eff", "Политропный КПД, д.ед", GPA_DIMLESS_UNIT),
        scalarSensor("adiabatic-head", "Адиабатная высота входа, м", GPA_LENGTH_UNIT),
        scalarSensor("polytropic-head", "Политропная высота входа, м", GPA_LENGTH_UNIT)};

gpaTurboExpander::gpaTurboExpander(const gpaCoreInterfaces *interfaces, gpaCoreReference importRef) : gpaExportSystem(interfaces, importRef),
                                                                                                      m_expander(asParent())
{
}

const gpaStringVector gpaTurboExpander::m_circuitNames =
    {
        "Expander circuit",
        "Compressor circuit"};

const gpaStringVector gpaTurboExpander::m_portNames =
    {
        "122-018",
        "122-027",
        "122-065",
        "122-077"};

gpaUInt gpaTurboExpander::getDeviceNumMixCircuits()
{
    return NUM_CIRCUITS;
}

gpaUInt gpaTurboExpander::getDeviceMinNumMixPorts(gpaUInt index)
{
    switch (index)
    {
    case ID_CIRCUIT_EXPANDER:
        return 2;
        break;
    case ID_CIRCUIT_COMPRESSOR:
        return 2;
        break;

    default:
        return 0;
        break;
    }
}

gpaUInt gpaTurboExpander::getDeviceMaxNumMixPorts(gpaUInt index)
{
    switch (index)
    {
    case ID_CIRCUIT_EXPANDER:
        return 2;
        break;
    case ID_CIRCUIT_COMPRESSOR:
        return 2;
        break;

    default:
        return 0;
        break;
    }
}

gpaResult gpaTurboExpander::checkParameterValues(const gpaParameterValueVector& parameters) const
{
    return GPA_RESULT_OK;
}

const gpaString *gpaTurboExpander::getDeviceMixCircuitName(gpaUInt index)
{
    return index < NUM_CIRCUITS ? &m_circuitNames[index] : NULL;
}

const gpaString *gpaTurboExpander::getDeviceMixPortName(gpaUInt circIdx, gpaUInt portIdx)
{
    return portIdx < NUM_PORTS ? &m_portNames[portIdx + circIdx * 2] : NULL;
}

void gpaTurboExpander::setNames()
{
    m_expander.setName("expander");
}

gpaResult gpaTurboExpander::setParameterValues(const gpaParameterValueVector& parameters, bool checkNeeded)
{
    m_expander.setMOI(parameters[ID_PARAM_MOMENT_OF_INERTIA].getReal());
    m_expander.setRatedParameters(
        parameters[ID_PARAM_RATED_SPEED].getReal(),
        parameters[ID_PARAM_RATED_MFR_KS].getReal(),
        parameters[ID_PARAM_RATED_MFR_TDS].getReal());
    std::vector<double> relative_MFR_KS, polytropic_eta, relative_MFR_TDS, adiabatic_eta;
    for(gpaUInt i = 0; i < 10; i++)
    {
        relative_MFR_KS.push_back(parameters[ID_PARAM_RELATIVE_MFR_KS_START + i].getReal());
        polytropic_eta.push_back(parameters[ID_PARAM_POLYTROPIC_ETA_START + i].getReal());
        relative_MFR_TDS.push_back(parameters[ID_PARAM_RELATIVE_MFR_TDS_START + i].getReal());
        adiabatic_eta.push_back(parameters[ID_PARAM_ADIABATIC_ETA_START + i].getReal());
    }
    m_expander.setPolytropicCurve(relative_MFR_KS, polytropic_eta);
    m_expander.setAdiabaticCurve(relative_MFR_TDS, adiabatic_eta);
    m_expander.setInitialRPM(parameters[ID_PARAM_INITIAL_SPEED].getReal());
    return GPA_RESULT_OK;
}

gpaResult gpaTurboExpander::buildDevice()
{
    bool good = true;

    good = good && m_expander.connectMixStream(ID_CIRCUIT_EXPANDER, ID_PORT_INLET, getEdgeMixStream(ID_CIRCUIT_EXPANDER, ID_PORT_INLET), GPA_STREAM_INLET);
    good = good && m_expander.connectMixStream(ID_CIRCUIT_EXPANDER, ID_PORT_OUTLET, getEdgeMixStream(ID_CIRCUIT_EXPANDER, ID_PORT_OUTLET), GPA_STREAM_OUTLET);
    good = good && m_expander.connectMixStream(ID_CIRCUIT_COMPRESSOR, ID_PORT_INLET, getEdgeMixStream(ID_CIRCUIT_COMPRESSOR, ID_PORT_INLET), GPA_STREAM_INLET);
    good = good && m_expander.connectMixStream(ID_CIRCUIT_COMPRESSOR, ID_PORT_OUTLET, getEdgeMixStream(ID_CIRCUIT_COMPRESSOR, ID_PORT_OUTLET), GPA_STREAM_OUTLET);
    // Добавляем в систему все элементы и потоки
    setElements({&m_expander});
    // setStreams({ });
    return GPA_RESULT_OK;
}

gpaResult gpaTurboExpander::initialize()
{
    // Подключить экспортируемый элемент к потокам в интеграторе
    gpaResult res = linkToCoreStreams();
    if (res != GPA_RESULT_OK)
        return res;
    // Соединяем все элементы друг с другом
    res = buildDevice();
    if (res != GPA_RESULT_OK)
        return res;
    // Создаем схемы течений (нужны соединенные элементы)
    auto oe1 = getOuterEntrance(ID_CIRCUIT_EXPANDER, ID_PORT_INLET);
    auto oe2 = getOuterEntrance(ID_CIRCUIT_COMPRESSOR, ID_PORT_INLET);
    auto ret = createSimStructures({}, {oe1, oe2});
    if (!ret)
        return GPA_ERROR_WRONG_ARGS;
    // Инициализируем систему
    return gpaExportSystem::initialize();
}

gpaResult gpaTurboExpander::calcSensors(gpaVector &sensors) const
{
    setSensorValue(sensors, ID_SENSOR_PRESSURE_DIFF,
                   m_expander.getMixPort(ID_CIRCUIT_COMPRESSOR, ID_PORT_OUTLET)->getStreamPressure() -
                       m_expander.getMixPort(ID_CIRCUIT_COMPRESSOR, ID_PORT_INLET)->getStreamPressure());
    setSensorValue(sensors, ID_SENSOR_TEMPERATURE_DIFF,
                   m_expander.getMixPort(ID_CIRCUIT_COMPRESSOR, ID_PORT_OUTLET)->getStreamTemperature() -
                       m_expander.getMixPort(ID_CIRCUIT_COMPRESSOR, ID_PORT_INLET)->getStreamTemperature());
    setSensorValue(sensors, ID_SENSOR_POWER, m_expander.getPower());
    setSensorValue(sensors, ID_SENSOR_ENERGY, 0.0);
    setSensorValue(sensors, ID_SENSOR_ELECTRIC_POWER, 0.0);
    setSensorValue(sensors, ID_SENSOR_ROTATION_SPEED, m_expander.getRPM());
    setSensorValue(sensors, ID_SENSOR_POLYTROPIC_EFF, 0.0);
    setSensorValue(sensors, ID_SENSOR_ADIABATIC_HEAD, 0.0);
    setSensorValue(sensors, ID_SENSOR_POLYTROPIC_HEAD, 0.0);

    return GPA_RESULT_OK;
}