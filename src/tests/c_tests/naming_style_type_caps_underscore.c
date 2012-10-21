/*
style ordering:
    underscore
    pascal
    camel
    caps_underscore
    invalid
*/

//structs
struct abc_abc { int a; };
struct AbcAbc { int a; };
struct abcAbc { int a; };
struct ABC_ABC { int a; };
struct Abc_abC { int a; };

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