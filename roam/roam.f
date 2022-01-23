*===============================================================================

      SUBROUTINE ROAM(TI, XI, NT, T, X)

*-------------------------------------------------------------------------------
*                            State Propagation
*  IN:  TI - Initial time (seconds since epoch T0)
*       XI - Initial state
*       NT - Number of output times
*       T  - Times (seconds since epoch T0)
*  OUT: X  - State at times T
*-------------------------------------------------------------------------------

      IMPLICIT DOUBLE PRECISION (A-H,O-Z)

*  State Dimension      
      PARAMETER(NX = 13)

*  Arguments
      DIMENSION XI(*), T(*), X(NX,*)

*  State Propagation Function
      EXTERNAL ROAMF

*  ODE Solver Arguments
      PARAMETER(LENWRK = 32*NX)
      PARAMETER(H0 = 0.0D0)
      DIMENSION WORK(LENWRK), THR(NX), XGOT(NX), XD(NX), XMAX(NX) 

*  Options
      INCLUDE 'roamopt.fi'

*  Threshold
      TOL = 10D0**TL
      THR = 10D0**TH

*  Setup Runge-Kutta solver
      CALL SETUP(NX, TI, XI, T(NT), TOL, THR, MT, 'U', .FALSE., H0,
     &    WORK, LENWRK, .FALSE.)

*  Run Runge-Kutta solver
      DO K = 1,NT
          CALL UT(ROAMF, T(K), TGOT, XGOT, XD, XMAX, WORK, IFLAG)
          DO I = 1,NX
              X(I,K) = XGOT(I)
          ENDDO    
      ENDDO 

      RETURN
      END

*===============================================================================

      SUBROUTINE ROAMF(T, X, XD)

*-------------------------------------------------------------------------------
*                           State Derivative
*  IN:  T  - Time (seconds since epoch T0)
*       X  - State (see subroutine ROAM)
*  OUT: XD - State derivative
*-------------------------------------------------------------------------------

      IMPLICIT NONE

*  State Dimension      
      INTEGER NX
      PARAMETER(NX = 13)

*  Arguments
      DOUBLE PRECISION T, X(NX), XD(NX)

*  Times & time conversions
      DOUBLE PRECISION SECDAY, TTL, UTL, UT
      PARAMETER(SECDAY = 86400)

*  Angular speed of Earth's rotation
      DOUBLE PRECISION WE
      PARAMETER(WE = 7.2921150D-5)

*  Total acceleration & torque
      DOUBLE PRECISION ATOT(3), TTOT(3)

*  Positions
      DOUBLE PRECISION RECI(3), RECEF(3)

*  Gravitational accelerations
      DOUBLE PRECISION GECI(3), GECEF(3)

*  Ballistic coefficient & specific drag force
      DOUBLE PRECISION BC, FSDR(3)

*  Parameters & Options
      INCLUDE 'roampar.fi'
      INCLUDE 'roamopt.fi'

*-------------------------------------------------------------------------------

*  Time conversions
      TTL = T / SECDAY
      UTL = TTL - DT
      UT  = T0 + UTL
      CALL SETDT(DT)

*  Controls & disturbances
      ATOT(1) = (FC(1) + FD(1)) / (1000*MS)
      ATOT(2) = (FC(2) + FD(2)) / (1000*MS)
      ATOT(3) = (FC(3) + FD(3)) / (1000*MS)
      TTOT(1) = TC(1) + TD(1)
      TTOT(2) = TC(2) + TD(2)
      TTOT(3) = TC(3) + TD(3)

*  Geocentric position
      RECI(1) = X(1)
      RECI(2) = X(2)
      RECI(3) = X(3)
      CALL CELTER(T0, UTL, XP(1), XP(2), RECI, RECEF)

*  Earth gravity
      CALL GRAVTY(GD, RECEF, GECEF)
      CALL TERCEL(T0, UTL, XP(1), XP(2), GECEF, GECI)
      ATOT(1) = ATOT(1) + GECI(1)
      ATOT(2) = ATOT(2) + GECI(2)
      ATOT(3) = ATOT(3) + GECI(3)

*  Drag force
      IF (DR .NE. 0) THEN
          BC = MS / (CD * AS) 
          CALL DRAGFS(T0, UTL, X(1), X(4), BC, FSDR)
          ATOT(1) = ATOT(1) + FSDR(1)
          ATOT(2) = ATOT(2) + FSDR(2)
          ATOT(3) = ATOT(3) + FSDR(3)
      ENDIF

*  Lunar gravity -- TO DO

*  Solar gravity -- TO DO

*  Position & velocity derivatives  
      XD(1) = X(4)
      XD(2) = X(5)
      XD(3) = X(6)
      XD(4) = ATOT(1)
      XD(5) = ATOT(2)
      XD(6) = ATOT(3)

*  Angular velocity derivatives
      XD(7) = ((JP(2) - JP(3))*X(8)*X(9) + TTOT(1)) / JP(1)
      XD(8) = ((JP(3) - JP(1))*X(9)*X(7) + TTOT(2)) / JP(2)
      XD(9) = ((JP(1) - JP(2))*X(7)*X(8) + TTOT(3)) / JP(3)

*  Attitude derivatives
      CALL QTDER(X(10), X(7), XD(10))

      RETURN
      END

*===============================================================================
 
      SUBROUTINE ROAMPS(VAR, VAL)

*-------------------------------------------------------------------------------
*                        Set ROAM Parameters
*  IN:  VAR - Name of variable to be set (see 'roampar.fi')
*       VAL - New value for VAR
*-------------------------------------------------------------------------------

      IMPLICIT NONE

*  Arguments
      CHARACTER VAR*2
      DOUBLE PRECISION VAL(*)

*  Options
      INCLUDE 'roampar.fi'

*  Satellite Parameters
      IF     (VAR .EQ. 'MS') THEN
          MS    = VAL(1)
      ELSEIF (VAR .EQ. 'AS') THEN
          AS    = VAL(1)
      ELSEIF (VAR .EQ. 'CD') THEN
          CD    = VAL(1)
      ELSEIF (VAR .EQ. 'JP') THEN
          JP(1) = VAL(1)
          JP(2) = VAL(2)
          JP(3) = VAL(3)
*  Controls & Disturbances
      ELSEIF (VAR .EQ. 'FC') THEN
          FC(1) = VAL(1)
          FC(2) = VAL(2)
          FC(3) = VAL(3)
      ELSEIF (VAR .EQ. 'FD') THEN
          FD(1) = VAL(1)
          FD(2) = VAL(2)
          FD(3) = VAL(3)
      ELSEIF (VAR .EQ. 'TC') THEN
          TC(1) = VAL(1)
          TC(2) = VAL(2)
          TC(3) = VAL(3)
      ELSEIF (VAR .EQ. 'TD') THEN
          TD(1) = VAL(1)
          TD(2) = VAL(2)
          TD(3) = VAL(3)
*  Epoch
      ELSEIF (VAR .EQ. 'T0') THEN
          T0    = VAL(1)
*  Time-Varying Astrometric Data
      ELSEIF (VAR .EQ. 'XP') THEN
          XP(1) = VAL(1)
          XP(2) = VAL(2)
      ELSEIF (VAR .EQ. 'DT') THEN
          DT    = VAL(1)
*  Time-Varying Atmospheric Data
      ELSEIF (VAR .EQ. 'DF') THEN
          DF    = VAL(1)
      ELSEIF (VAR .EQ. 'AF') THEN
          AF    = VAL(1)
      ELSEIF (VAR .EQ. 'AP') THEN
          AP    = VAL(2)
      ENDIF

      RETURN
      END

*===============================================================================

      SUBROUTINE ROAMOS(VAR, VAL)

*-------------------------------------------------------------------------------
*                       Set ROAM Options
*  IN:  VAR - Name of variable to be set (see 'roamopt.fi')
*       VAL - New value for VAR
*-------------------------------------------------------------------------------

      IMPLICIT NONE

*  Arguments
      CHARACTER VAR*2
      INTEGER VAL

*  Options
      INCLUDE 'roamopt.fi'

*  ODE Solver Settings
      IF     (VAR .EQ. 'TL') THEN
          TL = VAL
      ELSEIF (VAR .EQ. 'TH') THEN
          TH = VAL
      ELSEIF (VAR .EQ. 'MT') THEN
          MT = VAL
*  Perturbation Settings
      ELSEIF (VAR .EQ. 'GD') THEN
          GD = VAL
      ELSEIF (VAR .EQ. 'LG') THEN
          LG = VAL
      ELSEIF (VAR .EQ. 'SG') THEN
          SG = VAL
      ELSEIF (VAR .EQ. 'DR') THEN
          DR = VAL
      ENDIF

      RETURN
      END

*===============================================================================

      BLOCK DATA ROAMPD

*-------------------------------------------------------------------------------
*                         Default Parameters
*-------------------------------------------------------------------------------

      INCLUDE 'roampar.fi'
      
      DATA MS /100/, AS /1/, CD /2/, JP /100,100,100/,
     &  FC /0,0,0/, FD /0,0,0/, TC /0,0,0/, TD /0,0,0/,
     &  T0 /2459215.5D0/,
     &  XP /0, 0/, DT /70/,
     &  DF /100/, AF /100/, AP /30/

      END

*===============================================================================

      BLOCK DATA ROAMOD

*-------------------------------------------------------------------------------
*                          Default Options
*-------------------------------------------------------------------------------

      INCLUDE 'roamopt.fi'
      
      DATA TL /-8/, TH /-9/, MT /1/,
     &  GD /10/, LG /1/, SG /1/, DR /1/

      END

*===============================================================================

      SUBROUTINE COE2RV(Y, X)

*-------------------------------------------------------------------------------
*            Classical Orbital Elements to Position & Velocity 
*-------------------------------------------------------------------------------

      IMPLICIT DOUBLE PRECISION (A-Z)

      DIMENSION Y(6), X(6), R(3), V(3)

      PARAMETER(RADDEG = 0.017453292519943295D0)

*  Gravitational Parameter (EGM2008)
      PARAMETER(GM = 3.986004415D5)

*  Unpack Orbital Elements
      A = Y(1)
      E = Y(2)
      I = Y(3)
      W = Y(4)
      U = Y(5)
      F = Y(6)

*  Compute Position & Velocity

      P = A * (1 - E**2)

      CF = DCOS(F * RADDEG)
      SF = DSIN(F * RADDEG)

      RC = P / (1 + E*CF)
      VC = DSQRT(GM / P)

      R(1) = RC * CF
      R(2) = RC * SF
      R(3) = 0

      V(1) = -VC * SF
      V(2) =  VC * (E + CF)
      V(3) = 0

      CALL ROTAX3(3, -U, R)
      CALL ROTAX3(1, -I, R)
      CALL ROTAX3(3, -W, R)

      CALL ROTAX3(3, -U, V)
      CALL ROTAX3(1, -I, V)
      CALL ROTAX3(3, -W, V)

*  Pack State
      X(1) = R(1)
      X(2) = R(2)
      X(3) = R(3)
      X(4) = V(1)
      X(5) = V(2)
      X(6) = V(3)

      RETURN
      END

*===============================================================================

      SUBROUTINE RV2COE(X, Y)

*-------------------------------------------------------------------------------
*            Position & Velocity to Classical Orbital Elements
*-------------------------------------------------------------------------------

      IMPLICIT DOUBLE PRECISION (A-Z)

      DIMENSION X(6), Y(6), R(3), V(3), HV(3), NV(2), EV(3)

      PARAMETER(DEGRAD = 57.29577951308232D0)

*  Gravitational Parameter (EGM2008)
      PARAMETER(GM = 3.986004415D5)

*  Unpack State
      R(1) = X(1)
      R(2) = X(2)
      R(3) = X(3)
      V(1) = X(4)
      V(2) = X(5)
      V(3) = X(6)

*  Compute Elements

      HV(1) = R(2)*V(3) - R(3)*V(2)
      HV(2) = R(3)*V(1) - R(1)*V(3)
      HV(3) = R(1)*V(2) - R(2)*V(1)

      H = DSQRT(HV(1)**2 + HV(2)**2 + HV(3)**2)

      NV(1) = -HV(2)
      NV(2) =  HV(1)

      N = DSQRT(NV(1)**2 + NV(2)**2)

      RI = 1D0 / DSQRT(R(1)**2 + R(2)**2 + R(3)**2)
      V2 = V(1)**2 + V(2)**2 + V(3)**2
      VR = R(1)*V(1) + R(2)*V(2) + R(3)*V(3) 

      XI = V2/2 - GM*RI

      ECR = V2/GM - RI
      ECV = -VR/GM

      EV(1) = ECR*R(1) + ECV*V(1) 
      EV(2) = ECR*R(2) + ECV*V(2) 
      EV(3) = ECR*R(3) + ECV*V(3)

      A = -GM / (2*XI)

      E = DSQRT(EV(1)**2 + EV(2)**2 + EV(3)**2)

      I = DEGRAD * DACOS(HV(3) / H)

      W = DEGRAD * DATAN2(NV(2), NV(1))

      U = DEGRAD * DACOS((NV(1)*EV(1) + NV(2)*EV(2)) / (N*E))
      IF (EV(3) .LT. 0) U = -U

      F = DEGRAD * DACOS((EV(1)*R(1) + EV(2)*R(2) + EV(3)*R(3)) * RI/E)
      IF (VR    .LT. 0) F = -F  

*  Pack Orbital Elements
      Y(1) = A
      Y(2) = E
      Y(3) = I
      Y(4) = W
      Y(5) = U
      Y(6) = F

      RETURN
      END

*===============================================================================

      SUBROUTINE LOSGEO(T, RECI, VLOS, TOL, ALAT, ALON, DIST)

*-------------------------------------------------------------------------------
*           Observed Positions on Earth's surface (WGS84)
*
*  IN:  T    - Time (seconds since epoch T0)  
*       RECI - Geocentric position of observer (km, ECI frame)
*       VLOS - Line-of-sight vector (arbitrary units, ECI frame)
*       TOL  - Latitude tolerance (deg)
*  OUT: ALAT - Geodetic latitude (deg)
*       ALON - Longitude (deg)
*       DIST - Distance (km)
*
*-------------------------------------------------------------------------------

      IMPLICIT DOUBLE PRECISION (A-Z)

      INCLUDE 'roampar.fi' 

      DIMENSION RECI(3), VLOS(3), RECEF(3), R(3), V(3)

*  Earth semi-major & semi-minor axis
      PARAMETER(AG = 6.3781370D3, BG = 6.356752314245D3)
      PARAMETER(AG2 = AG**2, BG2 = BG**2)

*  Time conversions
      PARAMETER(SECDAY = 86400)
      TTL = T / SECDAY
      UTL = TTL - DT
      CALL SETDT(DT)

*  Convert position to ECEF
      CALL CELTER(T0, UTL, XP(1), XP(2), RECI, RECEF)
      CALL CELTER(T0, UTL, XP(1), XP(2), VLOS, V)

*  Compute Earth surface position for line-of-sight

      A = (V(1)**2 + V(2)**2)/AG2 + V(3)**2/BG2

      B = 2*((RECEF(1)*V(1) + RECEF(2)*V(2))/AG2 +
     &            RECEF(3)*V(3)/BG2)
      
      C = (RECEF(1)**2 + RECEF(2)**2)/AG2 + RECEF(3)**2/BG2 - 1
          
      S = (-B - DSQRT(B**2 - 4*A*C)) / (2*A)

      R(1) = RECEF(1) + S*V(1)
      R(2) = RECEF(2) + S*V(2)
      R(3) = RECEF(3) + S*V(3)

*  Compute latitude, longitude, and distance

      CALL GEOD(R, TOL, ALAT, ALON, ALT)

      DIST = S * DSQRT(V(1)**2 + V(2)**2 + V(3)**2)

      RETURN
      END

*===============================================================================

      SUBROUTINE LL2ECI(T, LAT, LON, ALT, RECI)

*-------------------------------------------------------------------------------
*                Latitude & Longitude to ECI Position
*
*  IN:  T    - Time (seconds since epoch T0)
*       LAT - Geodetic latitude (deg)
*       LON - Longitude (deg)
*       ALT  - Geodetic altitude (km)
*  OUT: RECI - Position (km, ECI frame)
*
*-------------------------------------------------------------------------------

      IMPLICIT DOUBLE PRECISION (A-Z) 

      INCLUDE 'roampar.fi' 

      DIMENSION RECEF(3), RECI(3)

*  Earth semi-major & semi-minor axis
      PARAMETER(AG = 6.3781370D3, BG = 6.356752314245D3)

*  Time conversions
      PARAMETER(SECDAY = 86400)
      TTL = T / SECDAY
      UTL = TTL - DT
      CALL SETDT(DT)

*  Position in ECEF
      RECEF(1) = (AG + ALT) * DCOS(LAT) * DCOS(LON)
      RECEF(2) = (AG + ALT) * DCOS(LAT) * DSIN(LON)
      RECEF(3) = (BG + ALT) * DSIN(LAT)

*  Position in ECI
      CALL TERCEL(T0, UTL, XP(1), XP(2), RECEF, RECI)

      RETURN
      END

*===============================================================================

      SUBROUTINE ECI2LL(T, RECI, TOL, LAT, LON, ALT)

*-------------------------------------------------------------------------------
*                ECI Position to Latitude & Longitude 
*
*  IN:  T    - Time (seconds since epoch T0)
*       RECI - Position (km, ECI frame)
*       TOL  - Latitude tolerance (deg)
*  OUT: LAT - Geodetic latitude (deg)
*       LON - Longitude (deg)
*       ALT  - Geodetic altitude (km)
*
*-------------------------------------------------------------------------------
      
      IMPLICIT DOUBLE PRECISION (A-Z) 

      INCLUDE 'roampar.fi' 

      DIMENSION RECEF(3), RECI(3)

*  Time conversions
      PARAMETER(SECDAY = 86400)
      TTL = T / SECDAY
      UTL = TTL - DT
      CALL SETDT(DT)

*  ECEF position
      CALL CELTER(T0, UTL, XP(1), XP(2), RECI, RECEF)

*  Latitude, longitude, & altitude
      CALL GEOD(RECEF, TOL, LAT, LON, ALT)
      
      RETURN
      END

*===============================================================================
