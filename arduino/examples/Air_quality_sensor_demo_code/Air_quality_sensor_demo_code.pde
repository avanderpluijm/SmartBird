/***************************     2011 Seeedstudio      ************************
* File Name          : air quality sensor.pde
* Author             : Seeedteam
* Version            : V1.0
* Date               : 29/11/2011
* Description        : 
********************************************************************************
*
*******************************************************************************/
boolean low_pollution;
boolean high_pollution;
boolean air_fresh;
boolean force;
boolean error;
boolean signal=0;
unsigned char counter=0;
unsigned char i=0;
unsigned int temp=0;
float low_voltage=0;
int first_vol=0;
int last_vol=0;
long vol_standard=0;

/*******************************************************************************
* Function Name  : sys_init
* Description    : detect whether sensor is ready
* Input          : low_voltage
* Output         : None
* Return         : None
*******************************************************************************/
void sys_init()
    {
    	unsigned char i=0;
    	digitalWrite(13,LOW);
    	Serial.println("sys_starting...");
    	delay(20000);//300000
    	digitalWrite(13,HIGH);
    	low_voltage=analogRead(A0);
    	Serial.println(low_voltage);
    	while(low_voltage)
	    	{
	    		if(low_voltage<798 && low_voltage>10)//798 corresponding voltage is 3.9V
	    		{
	    			first_vol=analogRead(A0);//initialize first value
	    			last_vol=first_vol;
	    			vol_standard=last_vol;
	    			Serial.println("Sensor ready.");
	    			error=0;
	    			break;
	    		}
	    		else if(low_voltage>798||low_voltage<=10)
        		{	
        			i++;
        			delay(60000);//600000
        			Serial.println("waitting sensor init..");
        			low_voltage=analogRead(A0);
        			if(i==5)
					{
						i=0;
						error=1;
						Serial.println("Sensor Error!");
					}
	    		  }
				else 
	    		  	break;
        	}
}
/*******************************************************************************
* Function Name  : interrupt_init
* Description    : time for 2 seconds
* Input          : 
* Output         : None
* Return         : None
*******************************************************************************/
void interrupt_init()
	{
		TCCR2A=0;//normal model
		TCCR2B=0x07;//set clock as 1024*(1/16M)
		TIMSK2=0x01;//enable overflow interrupt
}
/*******************************************************************************
* Function Name  : slope
* Description    : measure the voltage change to calculate the slope of voltage in unit time
* Input          : 
* Output         : None
* Return         : None
*******************************************************************************/
void slope()
{
	while(signal)
	{
    	if(first_vol-last_vol>400||first_vol>700)
    		{
				low_pollution=0;
    			        high_pollution=0;
				force=1;
                                air_fresh=0;
				Serial.println("High pollution! Force signal active.");		
				signal=0;	
				avg_voltage();		
				break;
    		}
    	else if((first_vol-last_vol>400&&first_vol<700)||first_vol-vol_standard>150)
    		{
				high_pollution=1;
				low_pollution=0;
				force=0;
				air_fresh=0;		
				Serial.print("sensor_value:");		
				Serial.print(first_vol);      		
				Serial.println("\t High pollution!");		
				signal=0;	
				avg_voltage();		
				break;
    			}
    	else if((first_vol-last_vol>200&&first_vol<700)||first_vol-vol_standard>50)
    		{
    			low_pollution=1;
    			high_pollution=0;
    			//low_pollution=0;
    			force=0;
    			air_fresh=0;
    			//Serial.println(first_vol-last_vol);
    			Serial.print("sensor_value:");
    			Serial.print(first_vol);      		
    			Serial.println("\t Low pollution!");		
    			signal=0;
    			avg_voltage();
    			break;
    			}
    	else
    		{
    			//temp+=first_vol;
    			avg_voltage();
    			high_pollution=0;
    			low_pollution=0;
    			force=0;
    			air_fresh=0;		
    			Serial.print("sensor_value:");
    			Serial.print(first_vol);
    			//Serial.print("\t avg_vol:");
    			//Serial.print(avg_vol);
    			Serial.println("\t Air fresh");
    			//Serial.println(i,DEC);
    			signal=0;
    		}
	}
}
/*******************************************************************************
* Function Name  : avg_voltage
* Description    : 
* Input          : 
* Output         : None
* Return         : None
*******************************************************************************/
void avg_voltage()
{
	if(i==150)//sum 2 minutes
	{
		vol_standard=temp/150;
		temp=0;
		Serial.print("Vol_standard:");			
		Serial.println(vol_standard);		
		i=0;		
	}
    else 
	{
		temp+=first_vol;
		//Serial.println(i,DEC);
		i++;
	}
}

/*******************************************************************************
* Function Name  : check
* Description    : set LED to display air condition
* Input          : 
* Output         : digital port D2,D3
* Return         : None
*******************************************************************************/
/*
void check()
{
	if(force!=0)
		{
			digitalWrite(2,HIGH);
			digitalWrite(3,HIGH);
			}
  else if(high_pollution!=0)
  	{
  		digitalWrite(3,HIGH);
  		digitalWrite(2,LOW);
  		}
  else if(low_pollution!=0)
  	{ 
  		digitalWrite(2,HIGH);
  		digitalWrite(3,LOW);
  		}
  else
  	{
  		digitalWrite(2,LOW);
  		digitalWrite(3,LOW);
  		}
}*/
/*******************************************************************************
* Function Name  : setup
* Description    : 
* Input          : 
* Output         : None
* Return         : None
*******************************************************************************/
void setup()
	{
		Serial.begin(9600);
		pinMode(A0,INPUT);
		pinMode(13,OUTPUT);
		//pinMode(2,OUTPUT);
		//pinMode(3,OUTPUT);
		sys_init();
		interrupt_init();
		Serial.println("Test begin...");
		sei();
}
/*******************************************************************************
* Function Name  : loop
* Description    : 
* Input          : 
* Output         : None
* Return         : None
*******************************************************************************/

void loop()
{
	slope();
	//check();
}
/*******************************************************************************
* Function Name  : ISR TIMER2_OVF_vect
* Description    : difinite 2 seconds time to measure sensor's voltage
* Input          : 
* Output         : None
* Return         : None
*******************************************************************************/
ISR(TIMER2_OVF_vect)
{
	if(counter==122)//set 2 seconds as a detected duty
		{

			last_vol=first_vol;
			first_vol=analogRead(A0);
			counter=0;
			signal=1;
			PORTB=PORTB^0x20;
	}
	else
		{
			counter++;
			//Serial.println(counter,DEC);
			}
}
