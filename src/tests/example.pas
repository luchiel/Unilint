procedure arara(var i: integer);
begin
    i := i + 1;
end;

function bububu: integer;
begin
    Result := 1;
end;

var
    j: integer;
begin
    j := bububu;
    arara(bububu);
end.

