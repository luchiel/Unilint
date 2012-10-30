procedure a;
  begin
    //always at new line, indented
    if a > b then
      begin
      //new, indented
      end;
  end;

begin
  //this is exceptional and is not marked as badly indented in all cases
  for i := 1 to 2 do begin
    //no indent
  end;
  while true do
  begin
      //indented
  end;
end.