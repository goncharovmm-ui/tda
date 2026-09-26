#include "gpaExpander.h"
#include "spline.h"
#include <cassert>

constexpr gpaUInt ID_CIRCUIT_COMPRESSOR = 0;
constexpr gpaUInt ID_CIRCUIT_EXPANDER = 1;

constexpr gpaUInt ID_PORT_INLET = 0;
constexpr gpaUInt ID_PORT_OUTLET = 1;

const gpaString initPresErrorStr = "Initial volume pressure must be positive";
const gpaString initTempErrorStr =
    "Initial volume temperature must be positive";
const gpaString initFractErrorStr =
    "Initial mole fractions must be in range [0,1] and their sum is less or "
    "equal to 1.0";
const gpaString liqHeatTransCoefErrorStr =
    "Gas-wall heat transfer coefficient must be non-negative";
const gpaString gasHeatTransCoefErrorStr =
    "Liquid-wall heat transfer coefficient must be non-negative";

const gpaString gpaExpander::m_typeName = "expander";
const gpaString gpaExpander::m_description = "";

gpaExpander::gpaExpander(const gpaParentLink *link) {
  // XDD
  setMixCircuit(new gpaElementMixCircuit(this, 0));
  setMixCircuit(new gpaElementMixCircuit(this, 1));
  m_I = 100;             // [Момент инерции вала турбины, кг * м^2]
  m_w_nom = 10500;       // [Номинальная скорость вращения вала, об/мин]
  m_G_KS_nom = 137.26;   // [Номинальный расход, кг/с]
  m_G_TDS_nom = 95.8528; // [Номинальный расход, кг/с]
  m_Gr_TDS = {0,      0.1745, 0.2662, 0.3549, 0.4406, 0.5230, 0.6022, 0.6780,
              0.7503, 0.8192, 0.8844, 0.9458, 1.004,  1.057,  1.107,  1.153,
              1.194,  1.231,  1.264,  1.293,  1.317,  1.336,  5};
  m_adiabatic_eta_TDS = {0.36,   0.3651, 0.4730, 0.5654, 0.6434, 0.7080,
                         0.7603, 0.8014, 0.8323, 0.8542, 0.8682, 0.8752,
                         0.8764, 0.8729, 0.8657, 0.8559, 0.8446, 0.8329,
                         0.8219, 0.8126, 0.8061, 0.8035, 0.2};
  m_Gr_KS = {0,      0.1745, 0.2662, 0.3549, 0.4406, 0.5230, 0.6022, 0.6780,
             0.7503, 0.8192, 0.8844, 0.9458, 1.004,  1.057,  1.107,  1.153,
             1.194,  1.231,  1.264,  1.293,  1.317,  1.336,  5};
  m_polytropic_eta_KS = {0.65,   0.6574, 0.7016, 0.7373, 0.7652, 0.7858,
                         0.7995, 0.8068, 0.8083, 0.8044, 0.7957, 0.7826,
                         0.7657, 0.7455, 0.7224, 0.6970, 0.6698, 0.6413,
                         0.6120, 0.5823, 0.5529, 0.5241, 0.1};
  m_w_init = 50; // [начальная скорость вращения турбины, об/с]
  m_h_KS = 0;
  m_h_TDS = 0;
}

gpaExpander::gpaExpander(const gpaExpander &expander) {}

gpaExpander::~gpaExpander() {
  if (m_compressorMedium != 0)
    delete m_compressorMedium;
  if (m_expanderMedium != 0)
    delete m_expanderMedium;
}

gpaExpander *gpaExpander::copy() const { return new gpaExpander(*this); }

bool gpaExpander::setRatedParameters(gpaReal omega, gpaReal G_KS,
                                        gpaReal G_TDS) {
  m_w_nom = omega;
  m_G_KS_nom = G_KS;
  m_G_TDS_nom = G_TDS;
  return true;
}

bool gpaExpander::setPolytropicCurve(std::vector<double> relative_MFR,
                                        std::vector<double> polytropic_eta) {
  if (std::all_of(relative_MFR.begin(), relative_MFR.end(),
                  [](double n) { return n == .0; }) &&
      std::all_of(polytropic_eta.begin(), polytropic_eta.end(),
                  [](double n) { return n == .0; }))
    return true;
  m_Gr_KS = relative_MFR;
  m_polytropic_eta_KS = polytropic_eta;
  return true;
}

bool gpaExpander::setAdiabaticCurve(std::vector<double> relative_MFR,
                                       std::vector<double> adiabatic_eta) {
  if (std::all_of(relative_MFR.begin(), relative_MFR.end(),
                  [](double n) { return n == .0; }) &&
      std::all_of(adiabatic_eta.begin(), adiabatic_eta.end(),
                  [](double n) { return n == .0; }))
    return true;
  m_Gr_TDS = relative_MFR;
  m_polytropic_eta_KS = adiabatic_eta;
  return true;
}

bool gpaExpander::setMOI(gpaReal moment) {
  m_I = moment;
  return true;
}

bool gpaExpander::setInitialRPM(gpaReal omega) {
  m_w_init = omega;
  return true;
}

gpaReal gpaExpander::getRPM() const { return m_w; }

gpaReal gpaExpander::getPower() const { return m_power; }

gpaUInt gpaExpander::getMinNumMixPorts(gpaUInt index) const {
  switch (index) {
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

gpaUInt gpaExpander::getMaxNumMixPorts(gpaUInt index) const {
  switch (index) {
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

gpaUInt gpaExpander::getNumVariables() const { return 1; }

gpaResult gpaExpander::initialize() {
  const gpaMedium *medium = getCircuitMediumAt(ID_CIRCUIT_COMPRESSOR);
  if (!medium)
    return GPA_ERROR_WRONG_ARGS;
  if (!medium->supportsFlash(PS_FLASH) || !medium->supportsFlash(PH_FLASH))
    return GPA_ERROR_UNSUPPORTED;
  m_compressorMedium = medium->copyTo(asOwner());
  medium = getCircuitMediumAt(ID_CIRCUIT_EXPANDER);
  if (!medium)
    return GPA_ERROR_WRONG_ARGS;
  if (!medium->supportsFlash(PS_FLASH) || !medium->supportsFlash(PH_FLASH))
    return GPA_ERROR_UNSUPPORTED;
  m_expanderMedium = medium->copyTo(asOwner());
  return GPA_RESULT_OK;
}

void gpaExpander::fillVarNames(gpaStringVector &varNames) const {
  varNames[0] = "Hz";
}

gpaResult gpaExpander::initVariables(gpaVector &variables) const {
  variables[0] = m_w_init;
  return GPA_RESULT_OK;
}

gpaResult gpaExpander::fillMassMatrix(gpaUInt refRow, gpaUInt refCol) const {
  gpaResult res = setMassMatrCoef(refRow, refCol, 1.0);
  if (res != GPA_RESULT_OK)
    return res;
  return GPA_RESULT_OK;
}

gpaResult gpaExpander::calcKSProcess(std::pair<gpaReal, gpaReal> &result,
                                     gpaReal dp) {
  gpaMixPort *port = getMixPort(ID_CIRCUIT_COMPRESSOR, ID_PORT_INLET);
  gpaReal p_out = port->getStreamPressure() + dp;
  gpaReal entropy = port->getStreamMedium()->getEntropy();
  gpaResult res = m_compressorMedium->calcFlashPS(p_out, entropy,
                                                  port->getStreamFractions());
  if (res != GPA_RESULT_OK)
    return res;
  gpaReal ro2_ideal = m_compressorMedium->getDensity();
  gpaReal enthalpy = m_compressorMedium->getEnthalpy();
  gpaReal ro2_actual =
      getMixPort(ID_CIRCUIT_COMPRESSOR, ID_PORT_OUTLET)->getStreamDensity();
  gpaReal h_out = enthalpy;
  gpaReal ro_in = port->getStreamDensity();
  gpaReal n = (log(p_out / port->getStreamPressure()) + 1e-12) /
                  (log(ro2_actual / ro_in) + 1e-12) +
              1e-12; // Polytropic Exponent
  gpaReal k = (log(p_out / port->getStreamPressure()) + 1e-12) /
                  (log(ro2_ideal / ro_in) + 1e-12) +
              1e-12; // Isentropic Exponent (Cp/Cv)
  // Пропорциональность между коэффициентами политропным и адиабатным кпд
  gpaReal k_prop =
      ((std::pow(p_out / port->getStreamPressure(), ((n - 1) / n)) - 1) *
       (n / (n - 1)) * ((k - 1) / k) /
       ((std::pow(p_out / port->getStreamPressure(), ((k - 1) / k)) - 1) +
        1e-12));
  result = {h_out, k_prop};
  return GPA_RESULT_OK;
}

gpaResult gpaExpander::calcTDSProcess(std::pair<gpaReal, gpaReal> &result,
                                      gpaReal dp) {
  gpaMixPort *port = getMixPort(ID_CIRCUIT_EXPANDER, ID_PORT_INLET);
  gpaReal p_out = port->getStreamPressure() - dp;
  gpaReal entropy = port->getStreamMedium()->getEntropy();
  gpaResult res =
      m_expanderMedium->calcFlashPS(p_out, entropy, port->getStreamFractions());
  if (res != GPA_RESULT_OK)
    return res;
  gpaReal ro2_ideal = m_expanderMedium->getDensity();
  gpaReal enthalpy = m_expanderMedium->getEnthalpy();
  gpaReal ro2_actual =
      getMixPort(ID_CIRCUIT_EXPANDER, ID_PORT_OUTLET)->getStreamDensity();
  gpaReal h_out = enthalpy;
  gpaReal ro_in = port->getStreamDensity();
  gpaReal n = (log(port->getStreamPressure() / p_out) + 1e-12) /
                  (log(ro_in / ro2_actual) + 1e-12) +
              1e-12; // Polytropic Exponent
  gpaReal k = (log(port->getStreamPressure() / p_out) + 1e-12) /
                  (log(ro_in / ro2_ideal) + 1e-12) +
              1e-12; // Isentropic Exponent (Cp/Cv)
  // Пропорциональность между коэффициентами политропным и адиабатным кпд
  gpaReal k_prop =
      ((std::pow(p_out / port->getStreamPressure(), ((n - 1) / n)) - 1) *
       (n / (n - 1)) * ((k - 1) / k) /
       ((p_out / std::pow(port->getStreamPressure(), ((k - 1) / k)) - 1) +
        1e-12));
  result = {h_out, k_prop};
  return GPA_RESULT_OK;
}

gpaResult gpaExpander::calcFuncValues(const gpaConstVector &,
                                      const gpaConstVector &variables,
                                      gpaVector &values,
                                      const gpaSetOfEquations &eqIds) {
  m_w = std::max(0., variables[0]);
  if (m_w > 10500) {
    return GPA_ERROR_WRONG_STATE;
  }

  gpaReal dP_tds =
      getMixPort(ID_CIRCUIT_EXPANDER, ID_PORT_OUTLET)->getStreamPressure() -
      getMixPort(ID_CIRCUIT_EXPANDER, ID_PORT_INLET)->getStreamPressure() +
      1.545e-3;
  gpaReal dP_ks =
      getMixPort(ID_CIRCUIT_COMPRESSOR, ID_PORT_OUTLET)->getStreamPressure() -
      getMixPort(ID_CIRCUIT_COMPRESSOR, ID_PORT_INLET)->getStreamPressure() +
      0.545e-3;

  gpaReal G_TDS =
      m_G_TDS_nom *
      ((m_w + 1) / m_w_nom); // Расчетный массовый расход турбодетандерной части
                             // при номинальном dP
  gpaReal G_KS =
      m_G_KS_nom *
      ((m_w + 1) / m_w_nom); // Расчетный массовый расход компрессорной части
                             // при номинальном dP
  // YOLOOO
  // std::vector<gpaReal> dP_TDS(m_P_out_detander);
  // for (auto &element : dP_TDS)
  //     element = m_P_in_detander - element;
  // std::vector<gpaReal> dP_KS(m_P_out_compressor);
  // for (auto &element : dP_KS)
  //     element -= m_P_in_compressor;

  // gpaReal dP_TDS_calc = tk::spline(m_G_TDS, dP_TDS)(G_TDS) * 1e6;
  // gpaReal dP_KS_calc = tk::spline(m_G_KS, dP_KS)(G_KS) * 1e6;

  // gpaReal relative_dP_TDS = std::sqrt(dP_tds / dP_TDS_calc); // Коэффициент
  // пропорциональности давления на портах по отношению к номинальному перепаду
  // по характеристике gpaReal relative_dP_KS = std::sqrt(dP_ks / dP_KS_calc);

  // G_TDS = G_TDS * relative_dP_TDS; // Обновленный массовый расход с учетом
  // смещения фактического перепада давления относительно характеристики G_KS =
  // G_KS / relative_dP_KS;

  gpaReal Gr_TDS = G_TDS / m_G_TDS_nom;
  gpaReal Gr_KS = G_KS / m_G_KS_nom;

  gpaReal eta_tds =
      tk::spline(m_Gr_TDS, m_adiabatic_eta_TDS, tk::spline::linear)(Gr_TDS);
  gpaReal poly_eta_ks =
      tk::spline(m_Gr_KS, m_polytropic_eta_KS, tk::spline::linear)(Gr_KS);

  std::pair<gpaReal, gpaReal> calcResult;

  gpaResult res = calcKSProcess(calcResult, dP_ks);
  if (res != GPA_RESULT_OK)
    return GPA_ERROR_MIX_STATE;
  auto [h_out_ks, k_prop] = calcResult;
  gpaReal eta_ks = poly_eta_ks / (k_prop + 1e-12);
  m_h_KS =
      getMixPort(ID_CIRCUIT_COMPRESSOR, ID_PORT_INLET)->getStreamEnthalpy() +
      (h_out_ks -
       getMixPort(ID_CIRCUIT_COMPRESSOR, ID_PORT_INLET)->getStreamEnthalpy()) /
          eta_ks;
  res = calcTDSProcess(calcResult, dP_tds);
  auto [h_out_tds, _] = calcResult;
  m_h_TDS =
      getMixPort(ID_CIRCUIT_EXPANDER, ID_PORT_INLET)->getStreamEnthalpy() +
      (h_out_tds -
       getMixPort(ID_CIRCUIT_EXPANDER, ID_PORT_INLET)->getStreamEnthalpy()) /
          eta_tds;

  // Мощность турбодетандерной части
  gpaReal power_tds = G_TDS * dP_tds * eta_tds * GPA_KILO_WATT_TO_WATT;
  // Мощность компрессорной части
  m_power = G_KS * dP_ks * eta_ks * GPA_KILO_WATT_TO_WATT;

  gpaReal M_tds = power_tds / (m_w + 1);
  gpaReal M_ks = m_power / (m_w + 1);

  values(0) = (M_tds - M_ks) / m_I;

  gpaElementMixCircuit *circuit =
      getMixCircuit(ID_CIRCUIT_COMPRESSOR); // compressor
  values(1) = circuit->getPort(ID_PORT_OUTLET)->getStreamPressure() -
              circuit->getPort(ID_PORT_INLET)->getStreamPressure() - dP_ks;
  values(2) = (circuit->getPort(ID_PORT_OUTLET)->getIncomingMassRate() -
               circuit->getPort(ID_PORT_INLET)->getIncomingMassRate()) /
                  2 -
              G_KS;
  circuit = getMixCircuit(ID_CIRCUIT_EXPANDER); // expander
  values(3) = circuit->getPort(ID_PORT_OUTLET)->getStreamPressure() -
              circuit->getPort(ID_PORT_INLET)->getStreamPressure() - dP_tds;
  values(4) = (circuit->getPort(ID_PORT_OUTLET)->getIncomingMassRate() -
               circuit->getPort(ID_PORT_INLET)->getIncomingMassRate()) /
                  2 -
              G_TDS;
  return GPA_RESULT_OK;
}

gpaResult gpaExpander::calcEnthalpyFuncValueAt(gpaUInt circIdx, gpaUInt portIdx,
                                               gpaReal enthalpy,
                                               gpaReal &funcValue) const {

  gpaMixPort *oppPort = getMixCircuit(circIdx)->getPort(1 - portIdx);
  gpaReal portEnthalpy = oppPort->getStreamEnthalpy();
  // funcValue = enthalpy - (circIdx == 0 ? m_h_KS : m_h_TDS);
  funcValue = enthalpy - portEnthalpy;
  printf("Enthalpy for %d-%d: %f\n", circIdx, portIdx, funcValue);
  return GPA_RESULT_OK;
}

gpaResult
gpaExpander::calcFractionsFuncValuesAt(gpaUInt circIdx, gpaUInt portIdx,
                                       const gpaConstVector &fractions,
                                       gpaVector &funcValues) const {

  gpaMixPort *oppPort = getMixCircuit(circIdx)->getPort(1 - portIdx);
  gpaDataVector portFractions = oppPort->getStreamFractions();
  printf("Fractions %d\n", portIdx);
  for (gpaUInt i = 0; i < funcValues.size(); i++) {
    funcValues[i] = fractions[i] - portFractions[i];
    printf("%f--", funcValues[i]);
  }
  printf("\n");
  return GPA_RESULT_OK;
}
