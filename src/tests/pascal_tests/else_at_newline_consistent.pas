procedure Yay;
begin
  writeln('Yay!');
end;

begin
  if a > b then
    Yay
  else
    b := a;

  if a > b then begin
    Yay;
  end
  else
    b := a;
    
  if a > b then
    Yay;
  end else begin
    b := a;
  end;
end.