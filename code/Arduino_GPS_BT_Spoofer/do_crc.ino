// calculate a CRC for the line of input
// tested against http://nmeachecksum.eqth.net/ 

void do_crc(char *pch)
{
	unsigned char crc;

        //PString CRC_buf(buf, sizeof(buf));

	if (*pch != '$')
		return;		// does not start with '$' - so can't CRC

	pch++;			// skip '$'
	crc = 0;

	// scan between '$' and '*' (or until CR LF or EOL)
	while ((*pch != '*') && (*pch != '\0') && (*pch != '\r') && (*pch != '\n'))
	{ // checksum calcualtion done over characters between '$' and '*'
		crc ^= *pch;
		pch++;
	}

	// add or re-write checksum
	//sprintf(pch,"*%02X\r\n",(unsigned int)crc);

        Serial.println((unsigned int)crc,HEX);

         
}

