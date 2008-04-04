#ifndef VOR_INTEGER_FILTERS_H
#define VOR_INTEGER_FILTERS_H


#define VIF_CRES  16
#define VIF_CFACT (1<<VIF_CRES)
#define VIF_PCOEF(a) (a * VIF_CFACT + 0.5)
#define VIF_NCOEF(a) (a * VIF_CFACT - 0.5)

#define VIF_SRES  10
#define VIF_SFACT (1<<VIF_SRES)

#define VIF_SSCALE 5

inline int32_t vor_int_filter_bp_var( uint16_t xn) {

  const int32_t a0 = VIF_PCOEF( 0.0000294918571461433008684162315748977790);
  const int32_t a1 = VIF_PCOEF( 0.0000884755714384299093815122727590960494);
  const int32_t a2 = VIF_PCOEF( 0.0000884755714384299093815122727590960494);
  const int32_t a3 = VIF_PCOEF( 0.0000294918571461433008684162315748977790);

  const int32_t b1 = VIF_NCOEF(-2.8738524677701420273479016032069921493530);
  const int32_t b2 = VIF_PCOEF( 2.7555363566291544152875303552718833088875);
  const int32_t b3 = VIF_NCOEF(-0.8814479540018430592240861187747213989496);

  //  printf("%d %d %d %d\n", a0, a1, a2, a3);
  //  printf("%d %d %d\n", b1, b2, b3);

  static uint16_t x1 = 0;
  static uint16_t x2 = 0;
  static uint16_t x3 = 0;

  static int32_t y1 = 0;
  static int32_t y2 = 0;
  static int32_t y3 = 0;

  /* scale input */
  xn = (xn << VIF_SSCALE);

  int32_t yn =  a0 * xn
              + a1 * x1
              + a2 * x2
              + a3 * x3
              - b1 * y1
              - b2 * y2
              - b3 * y3;
  //  printf("%d\n", yn);
  yn = (yn >> VIF_CRES);
  
  x3 = x2;
  x2 = x1;
  x1 = xn;

  y3 = y2;
  y2 = y1;
  y1 = yn;

  return (yn >> VIF_SSCALE);
}

inline int32_t vor_int_filter_bp_ref( uint16_t xn) {

  const int32_t a0 = VIF_PCOEF( 0.0175489156490784836694984960558940656483);
  //  const int32_t a1 = VIF_PCOEF( 0.0000000000000000000000000000000000000000);
  const int32_t a2 = VIF_NCOEF(-0.0526467469472354510084954881676821969450);
  //  const int32_t a3 = VIF_PCOEF( 0.0000000000000000000000000000000000000000);
  const int32_t a4 = VIF_PCOEF( 0.0526467469472354510084954881676821969450);
  //  const int32_t a5 = VIF_PCOEF( 0.0000000000000000000000000000000000000000);
  const int32_t a6 = VIF_NCOEF(-0.0175489156490784836694984960558940656483);

  const int32_t b1 = VIF_PCOEF( 2.5508195725874163173330089193768799304962);
  const int32_t b2 = VIF_PCOEF( 3.9857308274503626677187639870680868625641);
  const int32_t b3 = VIF_PCOEF( 3.8245798569419009460546021728077903389931);
  const int32_t b4 = VIF_PCOEF( 2.6296760724926846464200025366153568029404);
  const int32_t b5 = VIF_PCOEF( 1.0926715614934312537087635064381174743176);
  const int32_t b6 = VIF_PCOEF( 0.2825578543465128156242371915141120553017);

  //  printf("%d %d %d %d %d %d %d\n", a0, a1, a2, a3, a4, a5, a6);
  //  printf("%d %d %d %d %d %d\n", b1, b2, b3, b4, b5, b6);

  static uint16_t x1 = 0;
  static uint16_t x2 = 0;
  static uint16_t x3 = 0;
  static uint16_t x4 = 0;
  static uint16_t x5 = 0;
  static uint16_t x6 = 0;

  static int32_t y1 = 0;
  static int32_t y2 = 0;
  static int32_t y3 = 0;
  static int32_t y4 = 0;
  static int32_t y5 = 0;
  static int32_t y6 = 0;

  /* scale input */
  xn = (xn << VIF_SSCALE);

  int32_t yn =  a0 * xn
  //          + a1 * x1
              + a2 * x2
  //          + a3 * x3
              + a4 * x4
  //          + a5 * x5
              + a6 * x6
              - b1 * y1
              - b2 * y2
              - b3 * y3
              - b4 * y4
              - b5 * y5
              - b6 * y6;
  //  printf("%d\n", yn);
  yn = (yn >> VIF_CRES);
  
  x6 = x5;
  x5 = x4;
  x4 = x3;
  x3 = x2;
  x2 = x1;
  x1 = xn;

  y6 = y5;
  y5 = y4;
  y4 = y3;
  y3 = y2;
  y2 = y1;
  y1 = yn;

  return (yn >> VIF_SSCALE);
}


#endif /* VOR_INTEGER_FILTERS_H */
