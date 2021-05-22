/*
    Se ejecuta mediante una IRQ en el pin 4 (T0)
    Threshold -> nivel de sensibilidad
    main.cpp Setup() >> touchAttachInterrupt(T0, resetButton_Flag, Threshold);
*/
bool btnFactoryDefault = false;
unsigned long tempoCleanApPreferences = 0;


void cleanApPreferences();


void resetButton_Flag(){
    btnFactoryDefault = true;
}

/*
    Restaura los valores por defecto pasados 5000 millis
*/

void resetButton(){
    if(touchRead(T0) < 45){ //GPIO 4 otra forma touchRead(4) 
		_APP_DEBUG_("TOUCHPAD","PUSH");
		if(tempoCleanApPreferences == 0){
			tempoCleanApPreferences = millis();
		}else{
			if ((millis() - tempoCleanApPreferences) > 5000){
				_APP_DEBUG_("TOUCHPAD", "Clean AP Preferences");
				cleanApPreferences();
				wait(100);
				ESP.restart();
			}


		}
	}else{
		tempoCleanApPreferences = 0; //Reinicia el temporizador
        btnFactoryDefault = false;
	}
}