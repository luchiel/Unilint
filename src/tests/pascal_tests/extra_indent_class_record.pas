type
  Bet =
    record
      money_on, money_against: integer;
    end;
  
  Point = class
      x, y: integer;
    end;

  Square = record
    a, b, c, d: Point;
  end;