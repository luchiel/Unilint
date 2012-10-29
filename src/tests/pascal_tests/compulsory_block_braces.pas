var
  i, j, k: integer;
begin
  while i < j do
    i += k;

  //this one is ok
  if i = j then begin
    k = i;
  end
  else begin
    k = j;
  end;
end.