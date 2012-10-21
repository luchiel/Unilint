//this one should be ok with either indented = true or false
int zubr() {}

int abaca()
    {
        //indented block
        return 0;
    }

int ababaca()
    {
        //non-indented block
        int z = 0;
        if(z)
            {
                zubr();
            }
        else
        {
            abaca();
        }

        if(z)
            z += z;

        if(z) {
                //indented block
                z++;
            }
    }