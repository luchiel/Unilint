void test() {
    //good indent here
}

int main() {
    for(int i = 9; i > 6; --i) {
      //indent too small
        //indent ok
          //indent too deep
    }
    if(1)
        test();
}