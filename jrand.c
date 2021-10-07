int jrand(value)
 unsigned int value;
 {
  int i,j,k;

  if (value == 0) return(0);  
  i = (int) (value)*drand48();
  return(i);
 }         
