#pragma once

#include <Components/ExtraClasses/Utility/SingletonDecorator.h>

#include <map>
#include <stdexcept>
#include <math.h>

namespace Maths
{

namespace Measurement
{

/* Usage example:
    Measurement::kilometer valueKm (1);
    Measurement::meter valueM (1);

    Measurement::day      valueDay (1);
    Measurement::hour     valueHour (1);
    Measurement::minute   valueMinute (1);
    Measurement::second   valueSecond (1);

    valueKm += valueM;
    valueMinute -= valueSecond;
*/

/**
 * @brief The MeasurementType enum Type of measurement value
 */
enum MeasureType
{
    MT_Any = 0,
    MT_Length,
    MT_Time,

    MT_UserType = 100,
};

/**
 * @brief The MeasureCoeff enum Regular conversion coefficients
 */
enum MeasureCoeff
{
    MC_None = 0, // Default value

    // 10^-N
    MC_Atto  ,
    MC_Femto ,
    MC_Pico  ,
    MC_Nano  ,
    MC_Micro ,
    MC_Milli ,
    MC_Santi ,
    MC_Deci  ,

    // 10^N
    MC_Deca ,
    MC_Gecto,
    MC_Kilo ,
    MC_Mega ,
    MC_Giga ,
    MC_Tera ,
    MC_Peta ,
    MC_Exa  ,

    // Times special
    MC_TimeMin, // Sec in minute
    MC_TimeHour,
    MC_TimeDay,
    MC_TimeMonth,
    MC_TimeYear,

    MC_UserType = 100,
};

typedef double measure_t;

/**
 * @brief The MeasConverter class Converts measurements between each other
 */
class MeasConverter : public ExtraClasses::SingletonDecorator
{
public:
    static MeasConverter& getInstance() {
        static MeasConverter inst;
        return inst;
    }

    /**
     * @brief registerConversion    Add custom conversion grade
     * @param measureGrade          Grade to use later
     * @param conversionCoeff       Coefficient of conversion from MC_None to this grade
     */
    static void registerConversion(int measureGrade, const measure_t& conversionCoeff) {
        getInstance().m_multipliers[measureGrade] = conversionCoeff;
    }

    /**
     * @brief getZeroConversion Deconvert value to MC_None
     * @param inputV            Value to deconvert
     * @param measureGrade      Grade of value
     * @return                  Deconverted value. For example, km will be deconverted to m
     */
    static measure_t getZeroConversion(const measure_t& inputV, int measureGrade) {
        auto& inst = getInstance();
        auto multiplTo = inst.m_multipliers.find(measureGrade);
        if (inst.m_multipliers.end() == multiplTo) {
            throw std::invalid_argument("Invalid conversion measurement grade to");
        }
        return inputV * multiplTo->second;
    }

    /**
     * @brief convert           Convert measurement value from grade FROM to grade TO
     * @param inputV            Value to convert
     * @param measureGradeFrom  Grade of value
     * @param measureGradeTo    Grade of output value
     * @return                  Converted value
     * @throws                  std::invalid_argument if grade FROM or grade TO not exist
     */
    static measure_t convert(const measure_t& inputV, int measureGradeFrom, int measureGradeTo) {
        auto& inst = getInstance();
        auto multiplFrom = inst.m_multipliers.find(measureGradeFrom);
        if (inst.m_multipliers.end() == multiplFrom) {
            throw std::invalid_argument("Invalid conversion measurement grade from");
        }
        auto multiplTo = inst.m_multipliers.find(measureGradeTo);
        if (inst.m_multipliers.end() == multiplTo) {
            throw std::invalid_argument("Invalid conversion measurement grade to");
        }
        return inputV *multiplTo->second / multiplFrom->second;
    }

    /**
     * @brief initDefault   Init all default multipliers
     */
    static void initDefault() {
        auto& inst = getInstance();

        inst.m_multipliers[ MC_Atto  ] = std::pow(10, -18.0);
        inst.m_multipliers[ MC_Femto ] = std::pow(10, -15.0);
        inst.m_multipliers[ MC_Pico  ] = std::pow(10, -12.0);
        inst.m_multipliers[ MC_Nano  ] = std::pow(10, -9.0);
        inst.m_multipliers[ MC_Micro ] = std::pow(10, -6.0);
        inst.m_multipliers[ MC_Milli ] = 0.001;
        inst.m_multipliers[ MC_Santi ] = 0.01;
        inst.m_multipliers[ MC_Deci  ] = 0.1;

        inst.m_multipliers[ MC_None  ] = 1;

        inst.m_multipliers[ MC_Deca  ] = 10;
        inst.m_multipliers[ MC_Gecto ] = 100;
        inst.m_multipliers[ MC_Kilo  ] = 1'000;
        inst.m_multipliers[ MC_Mega  ] = std::pow(10, 6.0);
        inst.m_multipliers[ MC_Giga  ] = std::pow(10, 9.0);
        inst.m_multipliers[ MC_Tera  ] = std::pow(10, 12.0);
        inst.m_multipliers[ MC_Peta  ] = std::pow(10, 15.0);
        inst.m_multipliers[ MC_Exa   ] = std::pow(10, 18.0);

        // Time
        inst.m_multipliers[ MC_TimeMin ]     = 60;
        inst.m_multipliers[ MC_TimeHour ]    = 60 * 60;
        inst.m_multipliers[ MC_TimeDay ]     = 60 * 60 * 24;
        inst.m_multipliers[ MC_TimeMonth ]   = 60 * 60 * 24 * 31;
        inst.m_multipliers[ MC_TimeYear ]    = 60*60*24;
    }
private:
    std::map<int, measure_t> m_multipliers;
};

/**
 * @brief The MeasValue class   Measurement handle value
 */
template <int MEASURE_TYPE_ID, int MULTIPLIER_VALUE_GRADE>
class MeasValue
{
    measure_t m_selfValue {0};
public:
    using type = measure_t;
    static const int meas_type {MEASURE_TYPE_ID};
    static const int multi_grade {MULTIPLIER_VALUE_GRADE};

    MeasValue(const measure_t& initV = {}) :
        m_selfValue {initV} {
    }
    MeasValue(const MeasValue&) = default;
    MeasValue(MeasValue&&) = default;

     void setValue(measure_t v) {
         m_selfValue = v;
     }

     measure_t getValue() const {
         return m_selfValue;
     }

     // Get value in unified metrics
     measure_t getUnifiedValue() const {
         return MeasConverter::getZeroConversion(m_selfValue, MULTIPLIER_VALUE_GRADE);
     }

     // Multiplier – relative to value
     template <int O_MULTIPLIER_VALUE_GRADE>
     measure_t getValueRelative(const MeasValue<MEASURE_TYPE_ID, O_MULTIPLIER_VALUE_GRADE>& multiplier) const {
        return MeasConverter::convert(m_selfValue, MULTIPLIER_VALUE_GRADE, O_MULTIPLIER_VALUE_GRADE);
     }

     MeasValue<MEASURE_TYPE_ID, MULTIPLIER_VALUE_GRADE>& operator =(const MeasValue<MEASURE_TYPE_ID, MULTIPLIER_VALUE_GRADE>& _otherMeasure) = default;

     template <int O_MULTIPLIER_VALUE_GRADE>
     MeasValue<MEASURE_TYPE_ID, MULTIPLIER_VALUE_GRADE>& operator +=(const MeasValue<MEASURE_TYPE_ID, O_MULTIPLIER_VALUE_GRADE>& _otherMeasure) {
         m_selfValue += _otherMeasure.getValueRelative(*this);
         return *this;
     }

     template <int O_MULTIPLIER_VALUE_GRADE>
     MeasValue<MEASURE_TYPE_ID, MULTIPLIER_VALUE_GRADE>& operator -=(const MeasValue<MEASURE_TYPE_ID, O_MULTIPLIER_VALUE_GRADE>& _otherMeasure) {
         m_selfValue -= _otherMeasure.getValueRelative(*this);
         return *this;
     }

     template <int O_MULTIPLIER_VALUE_GRADE>
     MeasValue<MEASURE_TYPE_ID, MULTIPLIER_VALUE_GRADE> operator +(const MeasValue<MEASURE_TYPE_ID, O_MULTIPLIER_VALUE_GRADE>& _otherMeasure) {
         return (m_selfValue + _otherMeasure.getValueRelative(*this));
     }

     template <int O_MULTIPLIER_VALUE_GRADE>
     MeasValue<MEASURE_TYPE_ID, MULTIPLIER_VALUE_GRADE> operator -(const MeasValue<MEASURE_TYPE_ID, O_MULTIPLIER_VALUE_GRADE>& _otherMeasure) {
         return (m_selfValue - _otherMeasure.getValueRelative(*this));
     }
};

// Imperial
using santimeter    = MeasValue<MeasureType::MT_Length, MeasureCoeff::MC_Santi>;
using decimeter     = MeasValue<MeasureType::MT_Length, MeasureCoeff::MC_Deci>;
using meter         = MeasValue<MeasureType::MT_Length, MeasureCoeff::MC_None>;
using kilometer     = MeasValue<MeasureType::MT_Length, MeasureCoeff::MC_Kilo>;

// Second to year
using second  = MeasValue<MeasureType::MT_Time, MeasureCoeff::MC_None>;
using minute  = MeasValue<MeasureType::MT_Time, MeasureCoeff::MC_TimeMin>;
using hour    = MeasValue<MeasureType::MT_Time, MeasureCoeff::MC_TimeHour>;
using day     = MeasValue<MeasureType::MT_Time, MeasureCoeff::MC_TimeDay>;
using month   = MeasValue<MeasureType::MT_Time, MeasureCoeff::MC_TimeMonth>;
using year    = MeasValue<MeasureType::MT_Time, MeasureCoeff::MC_TimeYear>;

}

}
