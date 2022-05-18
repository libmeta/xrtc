#include <valarray>
#include "jmi.h"
#include <iostream>
#include <type_traits>
#include <array>
#include <vector>
using namespace std;
using namespace jmi;

extern "C" jint JNICALL JNI_OnLoad(JavaVM* vm, void*)
{
    JNIEnv* env = nullptr;
    if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK || !env) {
        std::cerr << "GetEnv for JNI_VERSION_1_4 failed" << std::endl;
        return -1;
    }
    jmi::javaVM(vm);
    return JNI_VERSION_1_4;
}

jint write(jfloatArray, jint, jint) {return 0;}

void test1(jint, const char* const, vector<jboolean>) {}
jint test2() {return 0;}
std::string test3() {return string();}

int main(int argc, char *argv[])
{
    cout << "jmi test" << endl;

    //cout << jmi::signature<decltype(&write)>::value << std::endl;
    cout << jmi::signature_of(write).data() << std::endl;
    //cout << jmi::signature_of(1.2f) << endl;
    cout << jmi::signature_of<std::string>().data() << endl;
    std::valarray<jfloat> f;
    cout << jmi::signature_of<decltype(&f)>() << endl;
    cout << jmi::signature_of<decltype(f)>().data() << endl;
    std::vector<std::string> s;
    cout << jmi::signature_of<decltype(std::ref(s))>().data() << endl;
    //std::vector<std::reference_wrapper<int>> v;
    //cout << jmi::signature_of(v);
    std::array<jint, 4> a;
    cout << jmi::signature_of<decltype(a)>().data() << endl;
    jfloat mat4[16];
    cout << jmi::signature_of<decltype(mat4)>().data() << endl;
    cout << "ref(mat4): " << jmi::signature_of<decltype(std::ref(mat4))>().data() << endl;
    //cout << "signature_of_args: " << jmi::signature_of_args<jint, jbyte, jlong>::value << endl;
    //std::unordered_map<jfloat, string> m;
    //cout << jmi::signature_of(m);
    cout << "test1: " << signature_of(test1).data() << endl;
    cout << "test2: " << signature_of(test2).data() << endl;
    cout << "test3: " << signature_of(test3).data() << endl;
}
