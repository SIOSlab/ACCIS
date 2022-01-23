*===============================================================================

      SUBROUTINE DRAGFS(TJDH, TJDL, RECI, VECI, BC, FSDR)

*-------------------------------------------------------------------------------
*                       Specific Drag Force
*
*     IN:  TJDH, TJDL - Time (Julian day, higher- and lower-order part)
*          RECI       - Satellite position (km, ECI)
*          VECI       - Satellite velocity (km/s, ECI)
*          BC         - Satellite ballistic coefficient - M/CD*A (kg/m**2)
*
*     OUT: FSDR       - Specific force due to drag (km/s**2, ECI)
*
*-------------------------------------------------------------------------------

      IMPLICIT DOUBLE PRECISION (A-H, O-Z)
     
      DIMENSION RECI(3), VECI(3), FSDR(3), VA(3) 

* Angular speed of Earth (rad/s)
      PARAMETER(WE = 7.2921150D-5) 

* Air velocity (km/s)
      VA(1) = VECI(1) + WE * RECI(2)
      VA(2) = VECI(2) - WE * RECI(1)
      VA(3) = VECI(3)

* Airspeed (m/s)
      V = 1000 * DSQRT(VA(1)**2 + VA(2)**2 + VA(3)**2)

* Air density (kg/m**3)
      RHO = DENSTY(TJDH, TJDL, RECI)

* Ratio of specific drag force magnitude to airspeed (s**-1)
      P = RHO * V / (2 * BC) 

* Specific drag force (km/s**2)
      FSDR(1) = -P * VA(1)
      FSDR(2) = -P * VA(2)
      FSDR(3) = -P * VA(3)

      END SUBROUTINE

*===============================================================================

      DOUBLE PRECISION FUNCTION DENSTY(TJDH, TJDL, RECI)

*-------------------------------------------------------------------------------
*                       Atmospheric Density
*
*     IN:  TJDH, TJDL - Time (Julian day, higher- and lower-order part)
*          RECI       - Satellite position (km, ECI)
*
*     OUT: DENSTY - Total atmospheric density (kg/m**3)
*
*-------------------------------------------------------------------------------

      IMPLICIT DOUBLE PRECISION (A-H, O-Z)

      DIMENSION RECI(3), SUN(2), SAT(3), TEMP(2)

* Modified Julian day epoch
      PARAMETER(AMJD0 = 2400000.5D0) 

* Modified Julian day - 0 Jan 1950 
      PARAMETER(AMJD50 = 33281.0D0)

* Modified Juluan day
      AMJD = (TJDH - AMJD0) + TJDL

* Days since 0 Jan 1950
      D1950 = AMJD - AMJD50

* Sun position
      CALL SUNPOS(AMJD, SOLRAS, SOLDEC)
      SUN(1) = SOLRAS
      SUN(2) = SOLDEC

* Satellite position
      CALL SATPOS(RECI, SATRAS, SATDEC, SATALT)
      SAT(1) = SATRAS
      SAT(2) = SATDEC
      SAT(3) = SATALT

* Read solar indices F10 and S10 (1 day lag)
      DLAG = 1
      T1950 = D1950 - DLAG
      CALL SOLFSMY(T1950, F10, F10B, S10, S10B, XMX, XMXB, XYX, XYXB)
      IF ((F10.LT.40.).OR.(F10B.LT.40.)) GOTO 10
      IF ((S10.LT.40.).OR.(S10B.LT.40.)) GOTO 10

* Read M10 (2 day lag)
      DLAG = 2
      T1950 = D1950 - DLAG
      CALL SOLFSMY(T1950, XFX, XFXB, XSX, XSXB, XM10, XM10B, XYX, XYXB)
      IF ((XM10.LT.40.).OR.(XM10B.LT.40.)) GOTO 10

* Read Y10 (5 day lag)      
      DLAG = 5
      T1950 = D1950 - DLAG
      CALL SOLFSMY(T1950, XFX, XFXB, XSX, XSXB, XMX, XMXB, Y10, Y10B)
      IF ((Y10.LT.40.).OR.(Y10B.LT.40.)) GOTO 10

* Geomagnetic storm DTC value
      CALL DTCVAL(D1950, IDTCVAL)
      DSTDTC = IDTCVAL

* Compute temperature & density
      CALL JB2008(AMJD, SUN, SAT, F10, F10B, S10, S10B, XM10, XM10B,
     &  Y10, Y10B, DSTDTC, TEMP, RHO)

* Return density
      DENSTY = RHO
      RETURN

* Write error message
   10 WRITE(*,*) 'ERROR IN ATMOSPHERIC DENSITY MODEL'

      END FUNCTION

*===============================================================================

      SUBROUTINE SATPOS(RECI, SATRAS, SATDEC, SATALT)

*-------------------------------------------------------------------------------
*             Satellite Position Conversion (For JB2008)
*   
*      IN:  RECI   - Position (km, ECI frame)
*
*      OUT: SATRAS - Right ascension (rad)
*           SATDEC - Declination (rad)
*           SATALT - Geodetic altitude (km)
*
*-------------------------------------------------------------------------------

      IMPLICIT DOUBLE PRECISION (A-H, O-Z)

      DIMENSION RECI(3)

* Equatorial radius (km) and squared eccentricity of Earth (WGS-84)
      PARAMETER(RE = 6.3781370D3)
      PARAMETER(E2 = 6.69437999014D-3)

* Altitude tolerance (km)
      PARAMETER(ALTTOL = 1.0D-6)

* Radii
      R = DSQRT(RECI(1)**2 + RECI(2)**2 + RECI(3)**2)
      P = DSQRT(RECI(1)**2 + RECI(2)**2)

* Right ascension
      SATRAS = DATAN2(RECI(2), RECI(1))
      
* Declination
      SATDEC = DASIN(RECI(3) / R) 

* Geodetic altitude (iterative solution)
      PHI = SATDEC
      H = R - RE
      GOTO 20
   30 PHI = DATAN((RECI(3) / P) / (1 - E2 * RN / (RN + H))) 
   20 HOLD = H 
      RN = RE / DSQRT(1 - E2 * DSIN(PHI)**2)
      H = P / DCOS(PHI) - RN 
      IF (DABS(H - HOLD) .GT. ALTTOL) GOTO 30
      SATALT = H

      END SUBROUTINE

*=============================================================================== 
