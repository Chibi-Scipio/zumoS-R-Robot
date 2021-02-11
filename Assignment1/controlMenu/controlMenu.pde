// Need G4P library
import g4p_controls.*;
//Need serial to communicate
import processing.serial.*;
Serial myPort;
char[] corner = new char[1];

public void setup(){
  size(420, 320, JAVA2D);
  createGUI();
  
  // Place your setup code here
  
 myPort = new Serial(this,"COM3",9600); //port being used and communication at 9600 bits
 corner[0] = '#';
  
}

public void draw(){
  background(15);
  fill(255,255,255);
  zumoMessageField.setTextBold();
  
}

// Use this method to add additional statements
// to customise the GUI controls
