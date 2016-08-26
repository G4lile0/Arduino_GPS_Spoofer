// A function to update the horizontal posisiton of the balloon
// Designed to overcome some of the difficulties of using more 
// demanding mathematcial functions on a 8bit uC

/*

After doing a little bit of research, I have decided to make the 
wild assumption that a delta of 1m of from a position translates 
into 0.000009 units of lon or lat

1m @ 0°    = 000°00'00.0326?N, 000°00'00.0000?E = 0.000009,0
1m @ 45°   = 000°00'00.0230?N, 000°00'00.0229?E = 0.000006,0.000006
1m @ 90°   = 000°00'00.0000?N, 000°00'00.0323?E = 0.0,0.000009
10m@ 90°   = 000°00'00.0000?N, 000°00'00.3234?E = 0.0,0.000090
*/

void updateWindWalk(float oldLat, float oldLon, float brng, float s) 
{
  
  float dlat1 = (cos(radians(brng))*s)*0.000009;
  float dlon1 = (sin(radians(brng))*s)*0.000009;

  CurLat += dlat1; 
  CurLon += dlon1;

  if(DEBUG) Serial.print("[dps:");
  if(DEBUG) Serial.print(distancePerStep,2);
  if(DEBUG) Serial.print(" :: brng:");
  if(DEBUG) Serial.print(brng, 2);
  if(DEBUG) Serial.print(" :: CurLat:");
  if(DEBUG) Serial.print(CurLat,8);
  if(DEBUG) Serial.print(" :: CurLon:");
  if(DEBUG) Serial.print(CurLon,8); 
  if(DEBUG) Serial.print("]   ");
}
