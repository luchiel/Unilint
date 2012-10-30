var
    a_global: integer;
    
procedure increase_a;
begin
  a_global := a_global + 1;
end;

var
    i, j: integer;
    begin
    i := 1;
    j := 6;
    while i < 10 do begin
        if i > j then
          Inc(i)
        else
            i := i + 2;
    end;
end.
