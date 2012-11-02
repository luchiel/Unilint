function bb: boolean;
begin
    //always at new line, no indent
    if a > b then
    begin
      //at new line, no indent
    end
    else begin
      //at same line, no indent
    end;
end;

procedure a;
  begin
  //always at new line, indented
    if a > b then
      begin
        //new, indented
      end
    else begin
        //same, indented
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