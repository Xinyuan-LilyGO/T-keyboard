This is code for connecting to the T-keyboard through an esp32.
All you need to do is put the include in your librries folder and use the main.cpp as your project code.
It finds a ble device with the name of the T-keybaord connects to it and reads his input to the notify function in the TKeyboardClient.h.
you should modify that function for your needs.
creadit for [allthemod](https://github.com/allthemod) and for [chegewara](https://github.com/chegewara)