/*
Began working on 12/17/2014
 by Alexander Zahabizadeh
 
 The goal is to communicate with the Maxon Motor EPOS2
 
 The getObject function deals with almost all of the transmission. Most important and interesting part of the program.
 
 In setup the software version is confirmed. If there are issues check if the software version has been updated.
 
 In loop the options are given and can be used. The debugging is pretty bad but helps if you are having sporadic communications.
 */


//Size of the arrays
#define dataFSWSize 8

//used to manipulate the response data from the Maxon
byte response[20];

//Transmission to be sent: 10 01 2003 0201 A888
//Sends as: 10 01 03 20 01 02 88 A8
byte softwareVersionData[8] = {
  16, 1, 3, 32, 1, 2, 219, 253   };

//Transmission to be sent: 10 01 2003 2023 CC2C
//Sends as: 10 01 03 20 23 20 2C CC
byte electricalAngleData[8] = {
  16, 1, 3, 32, 35, 32, 44, 204   };

//Transmission to be sent: 10 01 2003 6064 F9C3
//Sends as: 10 01 03 20 64 60 C3 F9
byte positionActualData[8] = {
  16, 1, 3, 32, 100, 96, 195, 249  };

//Transmission to be sent: 10 01 2003 6079 2A7E
//Sends as: 10 01 03 20 79 60 7E 2A
byte currentActualData[8] = {
  16, 1, 3, 32, 121, 96, 126, 42  };

//Transmission to be sent: 10 01 2003 2031 FE5F
//Sends as: 10 01 03 20 31 20 5F FE
byte currentDemandData[8] = {
  16, 1, 3, 32, 49, 32, 95, 254  };


//function to check getObject
void getObject(byte comms[], int commsSize, int talkative);


//Setup program that checks software version before moving on
void setup()
{
  //Serial to PC
  Serial.begin(115200);
  Serial.print("Connected to PC \n\n");

  //Serial to Maxon
  Serial1.begin(115200);
  Serial.print("Arduino is setup, will begin testing connection to Maxon");
  Serial.write(16);

  delay(5000);

  //Check Software Version
  int check = 1;
  while (check == 1);
  {
    Serial.print('.');

    //Call for softwareversion
    getObject(softwareVersionData, dataFSWSize, 0);

    //check if correct software version
    if (response[XX] == XX && response [XX] == [XX])
    {
      check = 0;
    }

    delay(500);
  }//end of Software Version Check to start
  Serial.print("\n\nSoftware Version Confirmed\n\n");

  //Give user options
  Serial.print("You can now choose to:\n");
}//end of setup










//main loop with options
void loop()
{
  //char to read value entered by user on PC
  char readFromPC = '0';


  //Option 1, Run for everything
  if (readFromPC == '1')
  {
    delay(100);

    int pass = 1;
    
    int running = 1;
    
    while(1)
    {
      //4 spots to save answers
      long answers[4];
      
      //start transmistion time tracker
      long startTime = millis();
      
      //Check Electrical Angle
      getObject(electricalAngleData, dataFSWSize, 0);
      answers[0] = response[8] * 16 + response[9];

      //Check Position Acutal
      getObject(positionActualData, dataFSWSize, 0);
      answers[1] = response[8] * 16 + response[9];

      //Check Current Actual
      getObject(currentActualData, dataFSWSize, 0);
      answers[2] = response[8] * 16 + response[9];

      //Check Current Demand
      getObject(currentDemandData, dataFSWSize, 0);
      answers[3] = response[8] * 16 + response[9];
      
      //stop transmission time tracker
      long elapsedTime = startTime - millis();
      
      if(pass == 20)
      {
        //Serial.
        pass = 0;
      }
      
      //Printout transmission round
      //Serial.print(");
      
      if (Serial.available() && Serial.read() == 'x')
      {
        running = 0;
      }
        
    }//loop
  }//end of Option1


  //Option 2, debugs Software Version
  else if (readFromPC == '2')
  {
    Serial.print("\nDebug mode on software version\n\n");

    delay(100);

    //Check Software Version
    getObject(softwareVersionData, dataFSWSize, 1);

    //reset for next user input
    readFromPC = '0';
  }


  //Option 3, debugs Electrical Angle
  else if (readFromPC == '3')
  {
    Serial.print("\nDebug mode on software version\n\n");

    delay(100);

    //Check Electrical Angle
    getObject(electricalAngleData, dataFSWSize, 1);
  }


  //Option 4, debugs Position Acutal
  else if (readFromPC == '4')
  {
    Serial.print("\nDebug mode on software version\n\n");

    delay(100);

    //Check Position Acutal
    getObject(positionActualData, dataFSWSize, 1);
  }


  //Option 5, debugs Current Actual
  else if (readFromPC == '5')
  {
    Serial.print("\nDebug mode on software version\n\n");

    delay(100);

    //Check Current Actual
    getObject(currentActualData, dataFSWSize, 1);
  }


  //Option 6, debugs Current Demand
  else if (readFromPC == '6')
  {
    Serial.print("\nDebug mode on software version\n\n");

    delay(100);

    //Check Current Demand
    getObject(currentDemandData, dataFSWSize, 1);
  }

  //small delay between runs
  delay(500);

  //clears Serial read buffer after each pass just as a debugging measure
  for (int i = 0; i < 64; i++)
  {
    Serial1.read();
  }
}//end of loop




/************************************************************************************
 * 
 * The getObject function uses arguments to transmitten the required bytes to receive
 * the intended information from the object dictionary.
 * 
 * Technically can work with any transmission but has not been tested.
 * 
 * comm[] - the pointer to the array of bytes that are to be sent
 * commsSize - size of comm[]
 * talkative - Activate debugging
 * 
 * Additional Info:
 * - Adapts to send any byte array size
 * - Can recieve up to a 20 byte response
 * - Does a decent job at filtering out any issues such as:
 * - Reading null bytes, which are seen as 255
 * - Dealing with speed of checksum
 * - This is probably the only part of the program people would ever care about.
 * 
 **********************************************************************************/


void getObject(byte comms[], int commsSize, int talkative)
{
  //set response to null
  byte firstConfirm = 0;

  //debug
  if (talkative == 1)
  {
    Serial.print("\nAttempting to get this object");
  }

  //Send Op code
  Serial1.write(comms[0]);

  //Small delay for response
  delayMicroseconds(240);

  //Read Response
  firstConfirm = Serial1.read();

  //If response was 'O' as in OK then op code handshake was confirmed
  if (firstConfirm == 'O')
  {
    //debug
    if (talkative == 1)
    {
      Serial.println("Op code recieved correctly, sending data \n");
    }

    //set reponse to 255 to filter for an answer
    byte secondConfirm = 255;

    //send data of message
    for (int i = 1; i < commsSize; i++)
    {
      Serial1.write(comms[i]);
    }

    //wait for maxon to calculate checksum and send answer op code
    delayMicroseconds(700);

    //Filters for an answer
    while ((secondConfirm == 255) && (Serial1.available() >= 1))
    {
      secondConfirm = Serial1.read();
    }

    //if repsonse is confirmed again
    if (secondConfirm == 'O')
    {

      //debug
      if (talkative == 1)
      {
        Serial.write("Data was recieved correctly, waiting for response\n");
      }

      //filter for response again
      response[0] = 255;
      delayMicroseconds(240);
      while ((response[0] == 255) && (Serial1.available() >= 1))
      {
        response[0] = Serial1.read();
      }

      //response was received and confirmed to be an answer of 0, which would be correct
      if (response[0] == 0)
      {

        //send acknoledgment of recieving response OP
        Serial1.write(79);

        //recieve responses
        for (int i = 1; i < 20; i++)
        {
          delayMicroseconds(240);
          response[i] = Serial1.read();
        }

        //send confirmation of answer, without checking checksum
        Serial1.print('O', HEX);

        //debug
        if (talkative == 1)
        {
          Serial.print("\nAnswer recieved \n");
        }

        //debug, prints the response completely out
        if (talkative == 1)
        {
          for (int i = 0; i < 20; i++)
          {
            if (response[i] != 255)
            {
              Serial.print("\n");
              Serial.print(i + 1);
              Serial.print(":   ");
              Serial.print(response[i], HEX);
            }
          }
        }//end of debug
      }//end of responseOP
      else //reponseOP issue
      {
        Serial.print("response[0] was: ");
        Serial.print(response[0]);
      }
    }//second confirm
    else if (secondConfirm == 'F')
    {
      Serial.print("Got an F for secondConfirm");
    }
    else
    {
      Serial.print("Invalid secondConfirm: ");
      Serial.print(secondConfirm);
    }
  }//first response end
  else if (firstConfirm == 'F')
  {
    Serial.print("Got an F for firstConfirm");
  }
  else //invalid firstConfirm
  {
    Serial.print("Invalid firstConfirm: ");
    Serial.print(firstConfirm);
  }
}//end of getObject



