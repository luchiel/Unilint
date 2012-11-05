procedure first;
begin

  procedure second;
  begin
  end;
  
  if true then begin
    //see: this begin is ok
  end;
  
  if true then
  begin
    //and this one is not
  end;
end;