#pragma once
#include "jmi.h"
#include <array>
#include <valarray>
#include <vector>

struct JMITestClassTag : jmi::ClassTag { static constexpr auto name() { return JMISTR("JMITest");} };
class JMITestCached : public jmi::Object<JMITestCached> // or jmi::JObject<JMITestClassTag>
{
public:
    static constexpr auto name() { return JMISTR("JMITest");} // required if derive from JObject<JMITestCached>
    static void resetStatic();
    void setX(jint v);
    jint getX() const;
    static void setY(jfloat v);
    static jfloat getY();
    void setStr(const char* v);
    std::string getStr() const;
    // java array is of fixed size
    static void getSStr(std::array<std::string,1>& v);
    static std::vector<std::string> getStrArrayS();
    static std::string getSub(jint beginIndex, jint endIndex, std::string s);
    std::string sub(jint beginIndex, jint endIndex) const;
    std::vector<std::string> getStrArray() const;
    std::valarray<jint> getIntArray() const;
    void getIntArrayAsParam(jint v[2]) const;
    void getIntArrayAsParam(std::array<jint, 2>& v) const;
    JMITestCached getSelf() const;
    void getSelfArray(std::array<JMITestCached,2>& v) const;
};

class JMITestUncached
{
public:
    bool create() { return obj.create(); } // DO NOT forget to call it
    static void resetStatic();
    void setX(jint v);
    jint getX() const;
    static void setY(jfloat v);
    static jfloat getY();

    static std::string getSub(jint beginIndex, jint endIndex, std::string s);
    std::string sub(jint beginIndex, jint endIndex) const;

    void setStr(const std::string& v);
    std::string getStr() const;

    static std::vector<std::string> getStrArrayS();
    std::vector<std::string> getStrArray() const;
    std::vector<jint> getIntArray() const;
    void getIntArrayAsParam(jint v[2]) const;
    void getIntArrayAsParam(std::array<jint, 2>& v) const;
private:
    jmi::JObject<JMITestClassTag> obj;
};