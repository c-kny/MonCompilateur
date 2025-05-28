VAR 
      a,b : INTEGER;
      c: BOOLEAN.
a:=1;
b:=6;
WHILE a<12 DO
BEGIN
        IF a%2==0 THEN
                DISPLAY a
        ELSE
                DISPLAY a*2;
        a:=a+1;
        c:=(a<=b);
        DISPLAY c
END.