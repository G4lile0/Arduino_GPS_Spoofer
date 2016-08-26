/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  */
/* Vincenty Direct Solution of Geodesics on the Ellipsoid (c) Chris Veness 2005-2011              */
/*                                                                                                */
/* from: Vincenty direct formula - T Vincenty, "Direct and Inverse Solutions of Geodesics on the  */
/*       Ellipsoid with application of nested equations", Survey Review, vol XXII no 176, 1975    */
/*       http://www.ngs.noaa.gov/PUBS_LIB/inverse.pdf                                             */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  */
// http://www.movable-type.co.uk/scripts/latlong-vincenty-direct.html
/**
 * Calculates destination point given start point lat/long, bearing & distance, 
 * using Vincenty inverse formula for ellipsoids
 *
 * @param   {Number} lat1, lon1: first point in decimal degrees
 * @param   {Number} brng: initial bearing in decimal degrees
 * @param   {Number} dist: distance along bearing in metres
 * @returns (LatLon} destination point
 */
 

/*

Note about this function:
------------------------
The Vincenty Direct equation puts quite a lot of demand on the precsion of the instrument 
that is running it.  Unfourtunatly, the Arduino with it's ATMEGA chip is only an 8bit uC and 
does not allow for more than 4 bytes of precision for floats or doubles.  This is not enough
to allow for calculation to be performed and StrangeThings(tm) happen if you try and calculate
distance per step of less than 4m.  This means that the balloon either has to be traveling 
_really_ fast (jet stream fast) or the simulation steps have to be really slow.  Neither is ideal. 
*/


 /*
1m @ 0°    = 000°00'00.0326?N, 000°00'00.0000?E = 0.000009,0
1m @ 45°   = 000°00'00.0230?N, 000°00'00.0229?E = 0.000006,0.000006
1m @ 90°   = 000°00'00.0000?N, 000°00'00.0323?E = 0.0,0.000009
10m@90°	   = 000°00'00.0000?N, 000°00'00.3234?E = 0.0,0.000090
*/
 
void updateWind(float lat1, float lon1, float brng, float s) 
{
 
  float a = 6378137, b = 6356752.3142,  f = 1/298.257223563;  // WGS-84 ellipsiod
  float alpha1 = radians(brng);
  float sinAlpha1 = sin(alpha1);
  float cosAlpha1 = cos(alpha1);

  float tanU1 = (1-f) * tan(radians(lat1));
  float cosU1 = 1 / sqrt((1 + tanU1*tanU1)), sinU1 = tanU1*cosU1;
  float sigma1 = atan2(tanU1, cosAlpha1);
  float sinAlpha = cosU1 * sinAlpha1;
  float cosSqAlpha = 1 - sinAlpha*sinAlpha;
  float uSq = cosSqAlpha * (a*a - b*b) / (b*b);
  float A = 1 + uSq/16384*(4096+uSq*(-768+uSq*(320-175*uSq)));
  float B = uSq/1024 * (256+uSq*(-128+uSq*(74-47*uSq)));
  float sigma = s / (b*A), sigmaP = 2*PI;
  
  //added by jim
  float sinSigma = sin(sigma);
  float cosSigma = cos(sigma);
  float cos2SigmaM = cos(2*sigma1 + sigma);
  float deltaSigma = B*sinSigma*(cos2SigmaM+B/4*(cosSigma*(-1+2*cos2SigmaM*cos2SigmaM) - B/6*cos2SigmaM*(-3+4*sinSigma*sinSigma)*(-3+4*cos2SigmaM*cos2SigmaM)));

  while (abs(sigma-sigmaP) > 1e-12) 
  {
    cos2SigmaM = cos(2*sigma1 + sigma);
    sinSigma = sin(sigma);
    cosSigma = cos(sigma);
    deltaSigma = B*sinSigma*(cos2SigmaM+B/4*(cosSigma*(-1+2*cos2SigmaM*cos2SigmaM) - B/6*cos2SigmaM*(-3+4*sinSigma*sinSigma)*(-3+4*cos2SigmaM*cos2SigmaM)));
    sigmaP = sigma;
    sigma = s / (b*A) + deltaSigma;
  }

  float tmp = sinU1*sinSigma - cosU1*cosSigma*cosAlpha1;
  float lat2 = atan2(sinU1*cosSigma + cosU1*sinSigma*cosAlpha1,(1-f)*sqrt(sinAlpha*sinAlpha + tmp*tmp));
  float lambda = atan2(sinSigma*sinAlpha1, cosU1*cosSigma - sinU1*sinSigma*cosAlpha1);
  float C = f/16*cosSqAlpha*(4+f*(4-3*cosSqAlpha));
  float L = lambda - (1-C) * f * sinAlpha * (sigma + C*sinSigma*(cos2SigmaM+C*cosSigma*(-1+2*cos2SigmaM*cos2SigmaM)));

  float lon2 = fmod((radians(lon1)+L+3*PI) , (2*PI)) - PI;  // normalise to -180...+180

  float revAz = atan2(sinAlpha, -tmp);  // final bearing, if required
  
  CurLat = degrees(lat2);
  CurLon = degrees(lon2);
 
 // if(DEBUG) Serial.print(" :: WindBearing:",6);
 // if(DEBUG) Serial.print(Bearing);
  if(DEBUG) Serial.print(":: bearing:");
  if(DEBUG) Serial.print(brng, 2);
  if(DEBUG) Serial.print(" :: d: ");
  if(DEBUG) Serial.print(distancePerStep,4);
  if(DEBUG) Serial.print(" :: CurLat: ");
  if(DEBUG) Serial.print(CurLat,8);
  if(DEBUG) Serial.print(" :: CurLon: ");
  if(DEBUG) Serial.print(CurLon,8); 
 
}





















// would not have been possible if it wasn't for:
// http://www.movable-type.co.uk/scripts/latlong.html
// Destination point given distance and bearing from start point

// useful blending function:
// http://danthompsonsblog.blogspot.com/2009/02/smoothstep-interpolation-with-arduino.html


void update_wind()
{
  
 /**
 * Returns the destination point from this point having travelled the given distance (in km) on the 
 * given initial bearing (bearing may vary before destination is reached)
 *
 *   see http://williams.best.vwh.net/avform.htm#LL
 *
 * @param   {Number} brng: Initial bearing in degrees
 * @param   {Number} dist: Distance in km
 * @returns {LatLon} Destination point


//LatLon.prototype.destinationPoint = function(brng, dist) {
  dist = typeof(dist)=='number' ? dist : typeof(dist)=='string' && dist.trim()!='' ? +dist : NaN;
  dist = dist/this._radius;  // convert dist to angular distance in radians
  brng = brng.toRad();  // 
  
  
  var lat1 = this._lat.toRad(), lon1 = this._lon.toRad();

  var lat2 = Math.asin( Math.sin(lat1)*Math.cos(dist) + Math.cos(lat1)*Math.sin(dist)*Math.cos(brng) );
 
  var lon2 = lon1 + Math.atan2(Math.sin(brng)*Math.sin(dist)*Math.cos(lat1), Math.cos(dist)-Math.sin(lat1)*Math.sin(lat2));
  
  
  lon2 = (lon2+3*Math.PI) % (2*Math.PI) - Math.PI;  // normalise to -180..+180º

  return new LatLon(lat2.toDeg(), lon2.toDeg());
//}
 */
  
 /* 
  int radius = 6371; // mean radius of the earth
  int Bearing = 90; // Bearing of travel
  int Dist = 1; // km per update
  Bearing = radians(Bearing); // convert Bearing to Radians
  Dist = Dist/radius; // convert dist to angular distance in radians
  
  float DestLat = asin(sin(CurLat)*cos(Dist)+cos(CurLat)*sin(Dist)*cos(Bearing));
  float DestLon = CurLon + atan2(sin(Bearing)*sin(Dist)*cos(CurLat),cos(Dist)-sin(CurLat)*sin(DestLat));
  
  
  DestLon = fmod((DestLon+3*PI),(2*PI)) - PI;  // normalise to -180..+180º

  if(DEBUG) Serial.print(":: bearing:");
  if(DEBUG) Serial.print(Bearing);
  if(DEBUG) Serial.print(" :: CurLat: ");
  if(DEBUG) Serial.print(CurLat,4);
  if(DEBUG) Serial.print(" :: DestLat: ");
  if(DEBUG) Serial.print(DestLat,4);
  if(DEBUG) Serial.print(" :: CurLon: ");
  if(DEBUG) Serial.print(CurLon,4);
  if(DEBUG) Serial.print(" :: DestLon: ");
  if(DEBUG) Serial.print(DestLon,4);
  if(DEBUG) Serial.print(" :: ");

  CurLat = DestLat;
  CurLon = DestLon;
*/
}
