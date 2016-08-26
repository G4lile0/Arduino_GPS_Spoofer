void update_alt()
{
  
 
  // If the Current altitude ever falls below the LAUNCH _ALT, then the payload is deemed to have landed
  if (CurAlt < LAUNCH_ALT)
  {
    Status = 2;
    return;
  }
  
  // http://artvb.oatmeal.dhs.org/?c=Glider
  // update altitude
  
  if (Status == 0)  // Ascent 
  {
    // while Ascending, the speed is simply the ASCENT_RATE 
    CurSpeed =  ASCENT_RATE;
    
    // We currently use a linear model for asscent, based on the ASCENT_RATE
 // modificado para que no suba.
 // CurAlt += (CurSpeed*simAccel)/SIM_HZ;

    // As we ascend the atmospheric pressure reduces.  Again we are using a linear aproximation
    // although there are well published equations for how KpA falls off with altitude. 
    CurKPA = getPressure(CurAlt);
    
    // If the outside pressure falls below that of what is defined as BURST_KPA, the balloon is 
    // deemed to have popped, and the payload starts it's descent
    if (CurKPA < BURST_KPA) Status = 1;
  }

  if (Status == 1) //Descent
  {
    // As we descend back though the atmospheric pressure increases.  
    // Again we are using a linear aproximation which needs to be fixed
    CurKPA = getPressure(CurAlt);
 
    // The payload initaly falls at it's terminal velocity
    // (although it should really accelerate towards it's TV at 9.8m/s sq - ignored for simplicity)
    // as the KpA increases, so does drag, eventualy slowing
    // the payload to it's ideal descent rate.


    // Drag is a normalised value bsed on the current KPA where the KPA at burst altitude 
    // results in a vale for Drag ~1 and the KPA at sea level is gives a value ~= 0.
    // this is effectivly saying how far removed CurSpeed is from TERMINAL_VELOCITY
    
    // Note: these are (obviously) not real equations for calculating Drag, they simply simulate the behaviour.
    
    Drag = 1-fscale(0,100,0,1,CurKPA,0);
    CurSpeed = TERMINAL_VELOCITY*Drag;
   
    // the payload is not able to Fall at speeds slower than it's targeted DESCENT_RATE 
    if (CurSpeed<DESCENT_RATE) CurSpeed=DESCENT_RATE;
    
    CurAlt -= (CurSpeed/SIM_HZ)*simAccel;
  }

  if(DEBUG) Serial.print("[alt:");
  if(DEBUG) Serial.print(CurAlt);
  if(DEBUG) Serial.print("m :: s:");
  if(DEBUG) Serial.print(Status);
  if(DEBUG) Serial.print(" :: Vs:");
  if(DEBUG) Serial.print(CurSpeed);
  if(DEBUG) Serial.print("m/s :: KpA:");
  if(DEBUG) Serial.print(CurKPA,4);
  if(DEBUG) Serial.print(":: Drag:");
  if(DEBUG) Serial.print(Drag);
  if(DEBUG) Serial.print("]   ");
}


float getPressure(float h)
// basic implementation of an atmospheric density equation
// p = 101325 (1 - 2.25577 10^5 h)^5.25588   
// based on: http://www.engineeringtoolbox.com/air-altitude-pressure-d_462.html
{
  float p = 0.0;
  p = (101325 * pow((1 - 2.25577 * pow(10,-5) * h),5.25588)+random(-0.5,0.5));
  return p/1000;
}

