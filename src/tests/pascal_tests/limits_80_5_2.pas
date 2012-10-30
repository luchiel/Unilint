var
    i, j, k, l: integer;
    long_long_long_identifier_that_breaks_limits_is_so_long: int64;
begin
    for i := 1 to 100 do begin
        for j := 1 to 1000 do
            for k := 1 to 10000 do
                for l := 1 to 100000 do
                    for z := 1 to 1000000 do
                        //too deep there
                        long_long_long_identifier_that_breaks_limits_is_so_long := z * i;
    end;



    i := 0;
end.
