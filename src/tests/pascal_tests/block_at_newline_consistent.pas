var
  i, j, k: integer;
  l: array [1..10] of integer;
begin
  for i := 1 to 10 do
  begin
    if i > l[i] then begin
      j := i;
    end;
    if j > k then
    begin
      k := j;
    end;
  end;
end.
