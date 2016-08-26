void LCDFlight()
{
 //Lat & lon
 if(CurLat>0&&CurLat<10) lcd.setCursor(2, 0); //eg. 2.345  
 if(CurLat>10) lcd.setCursor(1, 0);  //eg. 23.456
 if(CurLat<0&&CurLat>-10) lcd.setCursor(1, 0);  //eg. -2.345
 if(CurLat<-10) lcd.setCursor(0, 0);  //eg. -23.456
 lcd.print(CurLat,3);
 
 if(CurLon>0&&CurLon<10) lcd.setCursor(2, 1); //eg. 2.345  
 if(CurLon>10) lcd.setCursor(1, 1);  //eg. 23.456
 if(CurLon<0&&CurLon>-10) lcd.setCursor(1, 1);  //eg. -2.345
 if(CurLon<-10) lcd.setCursor(0, 1);  //eg. -23.456
 lcd.print(CurLon,3);
 

 //Current Alt
 if(walk_Speed>10000) 
 {
   lcd.setCursor(8, 0);  
 }

 if(walk_Speed<10000) 
 {
   lcd.setCursor(8, 0);  
   lcd.print(" ");
   lcd.setCursor(9, 0);  
 }

 if(walk_Speed<1000) 
 {
   lcd.setCursor(8, 0);  
   lcd.print("  ");
   lcd.setCursor(10, 0);  
 }

 if(walk_Speed<100) 
 {
   lcd.setCursor(8, 0);  
   lcd.print("   ");
   lcd.setCursor(10, 0);  
 }

 if(walk_Speed<10) 
 {
   lcd.setCursor(8, 0);  
   lcd.print("    ");
   lcd.setCursor(11, 0);  
 }

 if(walk_Speed<1) 
 {
   lcd.setCursor(8, 0);  
   lcd.print("     ");
   lcd.setCursor(12, 0);  
 }

 lcd.print(walk_Speed,0);



// Current Speed ////////////BUG HERE////////////

// if(CurSpeed>9) 
// {
//   lcd.setCursor(14, 0);  
// }
// if(CurSpeed<10) 
// {
//   lcd.setCursor(14, 0);  
//   lcd.print(" ");
//   lcd.setCursor(15, 0);  
// }
// lcd.print(CurSpeed,0);





// Current Bearing
 
 if(Bearing>100) 
 {
   lcd.setCursor(10, 1);
 }
 
 if(Bearing<100)
 { 
   lcd.setCursor(10, 1);
   lcd.print(" ");
   lcd.setCursor(11, 1);
 }
 
 if(Bearing<10)  
 {
   lcd.setCursor(10, 1);
   lcd.print("  ");
   lcd.setCursor(12, 1);
 }
 lcd.print(Bearing,0);



// Status Icon

// lcd.setCursor(15, 1);

//   if(Status==0) lcd.print((char)0);
//   if(Status==1) lcd.print((char)1);
//   if(Status==2) lcd.print((char)2);



// graph Bearing
float graph_pos = fscale(0,360,0,9,Bearing,0);


 lcd.setCursor(8, 0);
 if(int(graph_pos)==0) lcd.print((char)0b00011000);    //  N
 if(int(graph_pos)==1) lcd.print((char)3);    //  NE
 if(int(graph_pos)==2) lcd.print((char)0b00011010);    //  E  -->
 if(int(graph_pos)==3) lcd.print((char)4);    //  SE
 if(int(graph_pos)==4) lcd.print((char)0b00011001);    //  S
 if(int(graph_pos)==5) lcd.print((char)5);    //  SW
 if(int(graph_pos)==6) lcd.print((char)0b00011011);    //  W
 if(int(graph_pos)==7) lcd.print((char)6);    //  NW
 if(int(graph_pos)==8) lcd.print((char)0b00011000);    //  N

 lcd.setCursor(8, 1);

 if(int(graph_pos)==0) lcd.print((char)0b00011000);    //  N
 if(int(graph_pos)==1) lcd.print((char)3);    //  NE
 if(int(graph_pos)==2) lcd.print((char)0b00011010);    //  E  -->
 if(int(graph_pos)==3) lcd.print((char)4);    //  SE
 if(int(graph_pos)==4) lcd.print((char)0b00011001);    //  S
 if(int(graph_pos)==5) lcd.print((char)5);    //  SW
 if(int(graph_pos)==6) lcd.print((char)0b00011011);    //  W
 if(int(graph_pos)==7) lcd.print((char)6);    //  NW
 if(int(graph_pos)==8) lcd.print((char)0b00011000);    //  N


}











