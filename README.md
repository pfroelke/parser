# parser
  All variables are interpreted as "int" in C. They are global and there's no need to declare them.  
  Every variable can be an integer or have a special value "Nul". "Nul" is the default value of a variable.  
  Variables are represented as a string of latin language characters. Size of characters does matter.  
  Arithmetic operators '=', '==', '<', '>', '<=', '>=', '+', '-', '/', '\*', '%', work the same way as in C.  
  If any of the variables' value is "Nul", then the result is also "Nul".  
  The language also includes logic operators '&', '|', '!'. Value 'false' is represented as "Nul" and 'true' as any other value.  
  Logic operators return 'true' as "0".  
  Operator '-u' is unary.  
  
  Priority of operators from lowest:  
   - '='
   - '|'
   - '&'
   - '!=', '=='
   - '<', '>', '<=', '>='
   - '+', '-'
   - '/', '\*', '%'
   - '!', '-u'
   
   The language also covers conditional statements('?') and while loops('@').  
    - '?(condition){code}'  
    - '@(condition){code}'  
    
#### Input:
   First line describes the max operation counter.  
   Second line descibes the list of variables that will be printed at the end.  
   Next lines are the code.  
   
   Every line can contain max 1000 characters.  
  
#### Output:
   In the first line the operation counter is printed.  
   Next lines are variables and their values.  
   
#### Examples:
-
   - input:  
     2  
     a b  
   - output:  
     0  
     a Nul  
     b Nul  
-
   - input:  
     6  
     a b  
     a=4  
     b=a-5*6  
   - output:  
     4  
     a 4  
     b -26  
