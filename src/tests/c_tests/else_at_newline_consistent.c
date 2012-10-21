int main()
{
    int a = 0, b = 0;
    if(a == 0) {
        hug();
    } else {
        dont_hug();
    }

    if(b == 0) {
        cry();
    }
    else {
        dont_cry();
    }

    //this case is exceptional, no error
    if(b == 0)
        cry_loud();
    else
        dont_cry_at_all();
}