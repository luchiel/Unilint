/*
style ordering:
    underscore
    pascal
    camel
    caps_underscore
    invalid
*/

//structs
struct abc_abc { int A; };
struct AbcAbc { int A; };
struct abcAbc { int A; };
struct ABC_ABC { int A; };
struct Abc_abC { int ABC_ABC; };

//functions
int qwerty_1() { return 1; }
int Qwerty2() { return 2; }
int qwertyThree() { return 3; }
int QWERTY_FOUR() { return 4; }
int Querty_5() { return 5; }

//typedefs
typedef sabaka_ int;
typedef SabakA int;
typedef sabakA int;
typedef SABAKA_ int;
typedef SaB_aKa int;

//variables
int avada_kedavra;
int avadaKedavra;
int AvadaKedavra;
int AVADA_KEDAVRA;
int 111avada_kenTavra;