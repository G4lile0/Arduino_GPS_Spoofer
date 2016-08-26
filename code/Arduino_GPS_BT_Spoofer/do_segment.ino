void do_segment(double FromLat, double FromLon, double FromAlt, double ToLat, double ToLon, double ToAlt)
{
	double Rate;			// ascent(+ve) / decent(-ve) rate meters per second (for segment)
	int Duration;			// calculated segment duration in seconds
	double Elapsed;			// floating point duration
	double Lat,Lon,Alt;		// clacualted for each step
	double	Course,Speed;	        // calculated for entire segment
	double Distance;		// distance between coordinates
	double DeltaLat,DeltaLon,DeltaAlt;	// Latitude, Longtitude and Altitude differences
	double AdjLon;			// Adjusted Longtitude difference
	int i;				// counter

	DeltaAlt = (ToAlt - FromAlt);
	DeltaLat = (ToLat - FromLat);
	DeltaLon = (ToLon - FromLon);

  
  	if (DeltaAlt >= 0)
	{ // ascending
		Rate = 200.0;
	}
	else
	{ // descending - clculate the geometric mean of the expected velocities at To and From altitude
		Rate = -200.0 * sqrt(pow(LOG_BASE,(FromAlt / LOG_POWER)) * pow(LOG_BASE,(ToAlt / LOG_POWER)));
	}

	// calcualte time (secs) between co-ordinates
	Elapsed = DeltaAlt / Rate; // always positive
	Duration = (int)Elapsed;
	if ((Elapsed - (float)Duration) >= 0.5)
		Duration++; // round duration of segment to nearest integer

	// Calculate Course (degrees relative to north) for entire segment
	Course = atan2(DeltaLat,DeltaLon);	// result is +PI radians (cw) to -PI radians (ccw) from x (Longtitude) axis

	Course = DEGREES(Course);			// convert radians to degrees
	if (Course <= 90.0)
		Course = 90.0 - Course;
	else
		Course = 450.0 - Course;		// convert to 0 - 360 clockwise from north

	// Calculate Speed (m/sec) for entire segment
	AdjLon = cos(RADIANS((FromLat + ToLat) / 2.0)) * DeltaLon;
	Distance = (sqrt((DeltaLat * DeltaLat) + (AdjLon * AdjLon)) * 111194.9266); // result in meters

	Speed =  Distance / (double)Duration; // meters per second

	// calculate 1 second "step" sizes
	DeltaAlt /= (double)Duration;
	DeltaLat /= (double)Duration;
	DeltaLon /= (double)Duration;

	// now output the NMEA for each step between From and To (but excluding To - which is picked up on next segment)
	Lat = FromLat;
	Lon = FromLon;
	Alt = FromAlt;

	for (i = 0; i < Duration; i++)
	{
                delay(1000); //simulate GPS at 1hz
		Output_NEMA(Now,Lat,Lon,Alt,Course,Speed);
		Now++;				// 1 second steps
		Lat += DeltaLat;
		Lon += DeltaLon;
		Alt += DeltaAlt;
            
            
	}
}

