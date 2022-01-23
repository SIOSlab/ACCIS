*===============================================================================

      SUBROUTINE GRAVTY(NDEG, RGC, GGC)

*-------------------------------------------------------------------------------
*              Earth's Gravitational Field (Truncated EGM2008)
*
*  IN:  NDEG - Degree of geopotential model (0-100)
*       RGC  - Geocentric position vector (km, WGS84 frame)
*
*  OUT: GGC  - Gravitational acceleration (km/s^2, WGS84 frame) 
*
*  References
*
*   - Gottlieb, R.G., "Fast Gravity, Gravity Partials, Normalized Gravity,
*     Gravity Gradient Torque and Magnetic Field: Derivation, Code and Data",
*     NASA Contractor Report 188243, February 1993.
*
*   - Pavlis, N.K., S.A. Holmes, S.C. Kenyon, and J.K. Factor,
*     "An Earth Gravitational Model to Degree 2160: EGM2008", presented at the
*     2008 General Assembly of the European Geosciences Union, Vienna, Austria,
*     April 13-18, 2008.
*
*-------------------------------------------------------------------------------

      IMPLICIT DOUBLE PRECISION (A-L,O-Z)
      
      PARAMETER(NMAX = 100)

      PARAMETER(NF = 100)

      PARAMETER(GM = 3.986004415D5, RE = 6.3781363D3)

      DIMENSION RGC(1:3), GGC(1:3),
     &    CC(2:NMAX, 0:NMAX), SC(2:NMAX, 0:NMAX),
     &    ETA(2:NMAX, 0:NMAX-1), XI(2:NMAX, 0:NMAX-1),
     &    ZETA(0:NMAX, 0:NMAX),
     &    P(0:NMAX, 0:NMAX),
     &    RH(1:3), RHO(1:NMAX), S(0:NMAX), C(0:NMAX)

      LOGICAL SETUP
      SAVE SETUP, CC, SC, ETA, XI, ZETA, P
      DATA SETUP /.FALSE./

*-------------------------------------------------------------------------------

      IF (.NOT.SETUP) THEN

*  Read C & S coefficients for EGM2008 from file
      OPEN(NF,
     &    FILE = 'roam/EGM2008_to100_TideFree',
     &    STATUS = 'OLD')
  10  READ(NF,*) N, M, CNM, SNM, CSTD, SSTD
      IF (N .LE. NMAX) THEN
          CC(N,M) = CNM
          SC(N,M) = SNM
          GOTO 10
      ENDIF
      CLOSE(NF)

*  Precompute eta & xi coefficients
      DO 20 N = 2,NMAX
          DO 30 M = 0,N-1
              ETA(N,M) = DSQRT((N+M-1) * (2*N+1) * (N-M-1) /
     &                     (1D0 * (N+M) * (N-M) * (2*N-3)))
              XI(N,M) = DSQRT((2*N-1) * (2*N+1) / (1D0 * (N-M) * (N+M)))
  30      CONTINUE
  20  CONTINUE
      
*  Precompute zeta coefficients
      DO 40 N = 0,NMAX
          ZETA(N,0) = DSQRT(N * (N+1) / 2D0)
          DO 50 M = 1,N
              ZETA(N,M) = DSQRT(1D0 * (N-M) * (N+M+1))
  50      CONTINUE
  40  CONTINUE

*  Precompute constant Legendre functions 
      P(0,0) = 1
      P(1,1) = DSQRT(3D0)
      DO 60 N = 2,NMAX
          P(N,N) = DSQRT((2*N+1) / (2D0 * N)) * P(N-1,N-1)
  60  CONTINUE

*  Indicate that everything is set up
      SETUP = .TRUE.

      ENDIF      

*-------------------------------------------------------------------------------

*  Radius
      R = DSQRT(RGC(1)**2 + RGC(2)**2 + RGC(3)**2)

*  Radial unit vector
      RH(1) = RGC(1) / R
      RH(2) = RGC(2) / R
      RH(3) = RGC(3) / R

*  Powers of RE/R
      RHO(1) = RE / R
      DO 70 N = 2,NDEG
          RHO(N) = RHO(N-1) * RHO(1)
  70  CONTINUE

*  C~ & S~ Functions
      C(0) = 1
      S(0) = 0
      C(1) = RH(1)
      S(1) = RH(2)
      DO 80 N = 2,NDEG
          C(N) = C(1)*C(N-1) - S(1)*S(N-1)
          S(N) = S(1)*C(N-1) + C(1)*S(N-1)
  80  CONTINUE  

*  Legendre Functions
      EPS = RH(3)
      P(1,0) = EPS * P(1,1) 
      DO 90 N = 2,NDEG
          P(N,N-1) = XI(N,N-1)*EPS*P(N-1,N-1)
          DO 100 M = 0,N-2
              P(N,M) = XI(N,M)*EPS*P(N-1,M) - ETA(N,M)*P(N-2,M)
 100      CONTINUE
  90  CONTINUE

*  Summations
      J = 0
      K = 0
      Y = 1
      H = 0
      DO 110 N = 2,NDEG
          JN = 0
          KN = 0
          YN = (N+1) * CC(N,0) * P(N,0)
          HN = ZETA(N,0) * CC(N,0) * P(N,1)
          DO 120 M = 1,N
              JN = JN + M * P(N,M) * (CC(N,M)*C(M-1) + SC(N,M)*S(M-1))
              KN = KN - M * P(N,M) * (CC(N,M)*S(M-1) - SC(N,M)*C(M-1))
              YN = YN + (1+N+M) * P(N,M) *(CC(N,M)*C(M) + SC(N,M)*S(M))
 120      CONTINUE
          DO 130 M = 1,N-1
              HN = HN + ZETA(N,M) * P(N,M+1) *
     &               (CC(N,M)*C(M) + SC(N,M)*S(M))
 130      CONTINUE
          J = J + RHO(N) * JN
          K = K + RHO(N) * KN
          Y = Y + RHO(N) * YN
          H = H + RHO(N) * HN
 110  CONTINUE
      L = Y + RH(3) * H

*  Gravitational acceleration
      G = GM / (R * R)
      GGC(1) = -G * (L * RH(1) - J)
      GGC(2) = -G * (L * RH(2) - K)
      GGC(3) = -G * (L * RH(3) - H)

*-------------------------------------------------------------------------------

      RETURN
      END

*===============================================================================

      SUBROUTINE GEOD(RGC, TOL, LAT, LON, ALT)

*-------------------------------------------------------------------------------
*              Geocentric to Geodetic Position Conversion (WGS84)
*
*  IN:  RGC - Geocentric position vector (km)
*       TOL - Latitude tolerance (deg)
*  OUT: LAT - Geodetic latitude (deg)
*       LON - Longitude (deg)
*       ALT - Geodetic altitude (km)
*
*-------------------------------------------------------------------------------

      IMPLICIT DOUBLE PRECISION (A-Z)

      DIMENSION RGC(3)

      PARAMETER(RE = 6.3781370D3, E2 = 6.69437999014D-3)
      PARAMETER(DEGRAD = 57.29577951308232D0)

      LON = DATAN2(RGC(2), RGC(1))

      R = DSQRT(RGC(1)**2 + RGC(2)**2 + RGC(3)**2)
      P = DSQRT(RGC(1)**2 + RGC(2)**2)
      Z = RGC(3)

      LATNEW = DATAN2(P, Z)

 140  LAT = LATNEW

      SLAT = DSIN(LAT)
      CLAT = DCOS(LAT)

      RN = RE / DSQRT(1 - E2 * SLAT**2)
      
      ALT = P / CLAT - RN

      LATNEW = DATAN((Z/P) / (1 - E2 * RN / (RN + ALT)))

      IF (DABS(LATNEW-LAT) .GT. (TOL / DEGRAD)) GOTO 140

      LAT = LAT * DEGRAD
      LON = LON * DEGRAD

      RETURN
      END

*===============================================================================

      SUBROUTINE ROTAX3(IAX, A, R)

*-------------------------------------------------------------------------------
*               Rotation About a Principal Axis in 3D
*  IN:     IAX - Axis index (1, 2, or 3)
*          A   - Angle (deg)
*  IN/OUT: R   - Vector to be rotated
*-------------------------------------------------------------------------------

      IMPLICIT DOUBLE PRECISION (A-H,O-Z)

      DIMENSION R(3)
      
      PARAMETER(RADDEG = 0.017453292519943295D0)

      AR = A * RADDEG

      C = DCOS(AR)
      S = DSIN(AR)

      IF     (IAX .EQ. 1) THEN
          R2 = R(2)
          R3 = R(3)
          R(2) =  R2*C + R3*S
          R(3) = -R2*S + R3*C
      ELSEIF (IAX .EQ. 2) THEN
          R1 = R(1)
          R3 = R(3)
          R(1) =  R1*C - R3*S
          R(3) =  R1*S + R3*C
      ELSEIF (IAX .EQ. 3) THEN
          R1 = R(1)
          R2 = R(2)
          R(1) =  R1*C + R2*S
          R(2) = -R1*S + R2*C
      ENDIF

      RETURN
      END 

*===============================================================================

      SUBROUTINE QTDER(Q, W, QD)

*-------------------------------------------------------------------------------
*                   Time Derivative of Quaternion
*   IN:  Q  - Quaternion (vector/imaginary part first)
*        W  - Angular velocity
*   OUT: QD - Time derivative of Q
*-------------------------------------------------------------------------------
    
      DOUBLE PRECISION Q(4), W(3), QD(4)

      QD(1) =  0.5 * (Q(4)*W(1) + Q(2)*W(3) - Q(3)*W(2)) 
      QD(2) =  0.5 * (Q(4)*W(2) + Q(3)*W(1) - Q(1)*W(3))
      QD(3) =  0.5 * (Q(4)*W(3) + Q(1)*W(2) - Q(2)*W(1))

      QD(4) = -0.5 * (Q(1)*W(1) + Q(2)*W(2) + Q(3)*W(3))

      RETURN
      END

*===============================================================================
