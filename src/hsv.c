int HSVtoRGB( float *r, float *g, float *b, float h, float s, float v);

int HSVtoRGB( float *r, float *g, float *b, float h, float s, float v){
  if( (h>360.)||(h<0.)){
    printf("H: HSV out of range %f %f %f %d %d\n", h, s, v);
    return(1);
  }

  if((s<0.)||(s>1.)){
    printf("S: HSV out of range %f %f %f %d %d\n", h, s, v);
    return(1);
  }

  if((v<0.)||(v>1.)){
    printf("V: HSV out of range %f %f %f %d %d\n", h, s, v);
    return(1);
  }
  if (h==360.){
    h=0;
  }

  int i;
  float f, p, q, t;
  if(s == 0) {
    /* achromatic (grey) */
    *r = *g = *b = v;
    return 0;
  }
  float H, S, V;
  H = h; V = v; S = s;
  h /= 60.; // sector 0 to 5
  i = (int)floor(h);
  f = h - i; // factorial part of h
  p = v * (1. - s);
  q = v * (1. - s * f);
  t = v * (1. - s * (1. - f));
  switch( i ) {
    case 0: *r = v; *g = t; *b = p; break;
    case 1: *r = q; *g = v; *b = p; break;
    case 2: *r = p; *g = v; *b = t; break;
    case 3: *r = p; *g = q; *b = v; break;
    case 4: *r = t; *g = p; *b = v; break;
    case 5: *r = v; *g = p; *b = q; break;
    default: printf("\nERROR HSV to RGB"); printf("i=%d hsv= %f %f %f\n", i, H, S, V);
  }
  return 0;
}