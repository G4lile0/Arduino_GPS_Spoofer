// Speed in Kph
// Course over ground relative to North
//
// In normal operation the GPS emulated sends the following sequence of messages
// $GPGGA, $GPRMC, $GPVTG
// $GPGGA, $GPGSA, $GPRMC, $GPVTG
// $GPGGA, $GPGSV  ... $GPGSV, $GPRMC, $GPVTG


// Output_NEMA - Output the position in NMEA
// Latitude & Longtitude in degrees, Altitude in meters Speed in meters/sec

void Output_NEMA(time_t Time, double Lat, double Lon, double Alt, double Course, double Speed)
{
	int LatDeg;				// latitude - degree part
	double LatMin;				// latitude - minute part
	char LatDir;				// latitude - direction N/S
	int LonDeg;				// longtitude - degree part
	double LonMin;				// longtitude - minute part
	char LonDir;				// longtitude - direction E/W

	if (Lat >= 0)
		LatDir = 'N';
	else
		LatDir = 'S';

	Lat = fabs(Lat);
	LatDeg = (int)(Lat);
	LatMin = (Lat - (double)LatDeg) * 60.0;

	if (Lon >= 0)
		LonDir = 'E';
	else
		LonDir = 'W';

	Lon = fabs(Lon);
	LonDeg = (int)(Lon);
	LonMin = (Lon - (double)LonDeg) * 60.0;

	// $GPGGA - 1st in epoc - 5 satellites in view, FixQual = 1, 45m Geoidal separation HDOP = 2.4

        PString GPGGA(buf, sizeof(buf));
        
        GPGGA.print("$GPGGA,");
        GPGGA.print(hour(Time));
        GPGGA.print(minute(Time));
        GPGGA.print(second(Time));
        GPGGA.print(",");
        GPGGA.print(LatDeg);
        if (LatMin  < 10) GPGGA.print("0");
        GPGGA.print(LatMin);
        GPGGA.print(",");
        GPGGA.print(LatDir);
        GPGGA.print(",");
        GPGGA.print(LonDeg);
        if (LonMin  < 10) GPGGA.print("0");
        GPGGA.print(LonMin);
        GPGGA.print(",");
        GPGGA.print(LonDir);
        GPGGA.print(",1,05,02.4,");
        GPGGA.print(Alt);
        GPGGA.print(",M,45.0,M,,*");
 
	Serial.print(buf); // Print Buf
        do_crc(buf); // Print CRC
       
	switch((int)Time % 3)
	{ // include 'none' or $GPGSA or $GPGSV in 3 second cycle
	case 0:
        {
                break;
        }
	case -1:
        {
		// 3D fix - 5 satellites (3,7,18,19 & 22) in view. PDOP = 3.3,HDOP = 2.4, VDOP = 2.3
                PString GPGSA(buf, sizeof(buf),"$GPGSA,A,3,03,07,18,19,22,,,,,,,,3.3,2.4,2.3*");
 
        	Serial.print(buf); // Print Buf
                do_crc(buf); // Print CRC
		break;
        }
	case -2:
	{
  	        // two lines of GPGSV messages - 1st line of 2, 8 satellites being tracked in total
		// 03,07 in view 11,12 being tracked
                PString GPGSV1(buf, sizeof(buf),"$GPGSV,2,1,08,03,89,276,30,07,63,181,22,11,,,,12,,,*");
        	Serial.print(buf); // Print Buf
                do_crc(buf); // Print CRC
		// GPGSV 2nd line of 2, 8 satellites being tracked in total
		// 18,19,22 in view 27 being tracked
                PString GPGSV2(buf, sizeof(buf),"$GPGSV,2.2,08,18,73,111,35,19,33,057,27,22,57,173,37,27,,,*");
        	Serial.print(buf); // Print Buf
                do_crc(buf); // Print CRC
		break;
        }
      }

	//$GPRMC
	//sprintf(buf,"$GPRMC,%02d%02d%02d.000,A,%02d%07.4f,%c,%03d%07.4f,%c,%.2f,%.2f,%02d%02d%02d,,,A*",
	//	hour(Time),minute(Time),second(Time),LatDeg,LatMin,LatDir,LonDeg,LonMin,LonDir,Speed * 1.943844,Course,day(Time),month(Time) + 1,year(Time) % 100);

        PString GPRMC(buf, sizeof(buf));
        
        GPRMC.print("$GPRMC,");
        GPRMC.print(hour(Time));
        GPRMC.print(minute(Time));
        GPRMC.print(second(Time));
        GPRMC.print(",A,");
        GPRMC.print(LatDeg);
        if (LatMin  < 10) GPRMC.print("0");
        GPRMC.print(LatMin);
        GPRMC.print(",");
        GPRMC.print(LatDir);
        GPRMC.print(",");
        GPRMC.print(LonDeg);
        if (LonMin  < 10) GPRMC.print("0");
        GPRMC.print(LonMin);
        GPRMC.print(",");
        GPRMC.print(LonDir);
        
        GPRMC.print(",");
        GPRMC.print(Speed * 1.943844);
        GPRMC.print(",");
        GPRMC.print(Course);
        GPRMC.print(",");
        GPRMC.print(day(Time));
        GPRMC.print(month(Time) + 1);
        GPRMC.print(year(Time) % 100);
        GPRMC.print(",,,A*");

	Serial.print(buf); // Print Buf
        do_crc(buf); // Print CRC
       
	// $GPVTG message last in epoc
	//sprintf(buf,"$GPVTG,%.2f,T,,,%.2f,N,%.2f,K,A*",Course,Speed * 1.943844,Speed * 3.6);

        PString GPVTG(buf, sizeof(buf));
        
        GPVTG.print("$GPVTG,");
        GPVTG.print(Course);
        GPVTG.print(",T,,,");
        GPVTG.print(Speed * 1.943844);
        GPVTG.print(",N,");
        GPVTG.print(Speed * 3.6);
        GPVTG.print(",K,A*");

	Serial.print(buf); // Print Buf
        do_crc(buf); // Print CRC
        
        
}

