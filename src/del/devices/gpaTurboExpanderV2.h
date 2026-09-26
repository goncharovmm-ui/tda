#ifndef GPA_TURBOEXPANDER_V2_H
#define GPA_TURBOEXPANDER_V2_H

#include "gpaExportSystem.h"
#include "gpaTurboExpanderV2Element.h"

class gpaTurboExpanderV2 : public gpaExportSystem
{
public:
    gpaTurboExpanderV2(const gpaCoreInterfaces* interfaces, gpaCoreReference importRef);
    gpaTurboExpanderV2(const gpaTurboExpanderV2&) = delete;
    ~gpaTurboExpanderV2() override = default;
    gpaTurboExpanderV2* copy() const override { return nullptr; }
    const gpaString& getTypeName() const override { return m_typeName; }
    const gpaString& getDescription() const override { return m_description; }
    gpaUInt getNumMixCircuits() const override { return 2; }
    gpaUInt getMinNumMixPorts(gpaUInt index) const override { return index < 2 ? 2 : 0; }
    gpaUInt getMaxNumMixPorts(gpaUInt index) const override { return index < 2 ? 2 : 0; }
    gpaResult initialize() override;
    gpaResult setSignals(const gpaConstVector& signals) override;
    gpaResult calcSensors(gpaVector& sensors) const override;
    const gpaString* getMixCircuitName(gpaUInt index) const override;
    const gpaString* getMixPortName(gpaUInt circuit, gpaUInt port) const override;
    static const gpaString& getDeviceTypeName() { return m_typeName; }
    static const gpaString& getDeviceDescription() { return m_description; }
    static const gpaParameterInfoVector* getParametersInfo() { return &m_parameters; }
    static const gpaSensorInfoVector* getSensorsInfo() { return &m_sensors; }
protected:
    const gpaParameterInfoVector* getParamsInfo() const override { return &m_parameters; }
    const gpaSensorInfoVector* getSensesInfo() const override { return &m_sensors; }
    gpaResult checkParameterValues(const gpaParameterValueVector& parameters) const override;
    gpaResult setParameterValues(const gpaParameterValueVector& parameters, bool checkNeeded) override;
private:
    gpaTurboExpanderV2Element m_element;
    static const gpaString m_typeName, m_description;
    static const gpaParameterInfoVector m_parameters;
    static const gpaSensorInfoVector m_sensors;
    static const gpaStringVector m_circuitNames, m_portNames;
};

#endif // GPA_TURBOEXPANDER_V2_H
